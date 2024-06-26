#include "functions.h"
#include "io_pins.h"
#include "global.h"
#include "DHT.h"

void Setup_Pins()
{
    pinMode(PIN_INFO_LED, OUTPUT);
}

void Blink_Info_LED()
{
    digitalWrite(PIN_INFO_LED, HIGH);
    delay(30);
    digitalWrite(PIN_INFO_LED, LOW);
    delay(30);
}

// https://provideyourown.com/2012/secret-arduino-voltmeter-measure-battery-voltage/
long ReadVcc() {
  // Read 1.1V reference against AVcc
  // set the reference to Vcc and the measurement to the internal 1.1V reference
  #if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
    ADMUX = _BV(MUX5) | _BV(MUX0);
  #elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
    ADMUX = _BV(MUX3) | _BV(MUX2);
  #else
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #endif  

  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Start conversion
  while (bitRead(ADCSRA, ADSC));

  uint8_t adc_low  = ADCL; // must read ADCL first - it then locks ADCH  
  uint8_t adc_high = ADCH; // unlocks both
  long adc_result = (adc_high<<8) | adc_low;
  adc_result = 1125300L / adc_result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
  return adc_result; // Vcc in millivolts
}

void ReadDHTSensorORIGINAL()
{
  float t = DHTSENSOR.getTemperature();
  float h = DHTSENSOR.getHumidity();

  Serial.print("DHT: ");
  Serial.println(DHTSENSOR.getStatusString());
  
  HUMIDITY = h;
  TEMPERATURE = t;

  Serial.print(HUMIDITY);
  Serial.println(" %");
  Serial.print(TEMPERATURE);
  Serial.println(" °C");
}

void ReadDHTSensor()
{
  float t = DHTSENSOR.getTemperature();
  float h = DHTSENSOR.getHumidity();

  Serial.print("DHT: ");
  const char* status;
  status=DHTSENSOR.getStatusString();
  Serial.println(status);
  Serial.print("DHT Model: ");
  Serial.println(DHTSENSOR.getModel());
    
  HUMIDITY = h;
  TEMPERATURE = t;

  Serial.print(HUMIDITY);
  Serial.println(" %");
  Serial.print(TEMPERATURE);
  Serial.println(" °C");
}

void ReadA0()  // Para la primera prueba. En adelante se usa ReadBoyero
{
  // Leo el valor de A0
  int a0 = analogRead(A0);
  VALUEA0 = a0;
  Serial.print("A0: ");
  Serial.println(VALUEA0);
}

void ReadBoyero()
{
  // El boyero tiene un pulso por segundo
  // Leo valores durante 1.5 segundos (ojo consumo de batería!) 
  // para tratar de agarrar un pulso entero y me quedo con el valor mas alto.
  // Le agregué una salida digital para poder contar los pulsos con el osciloscopio. 
  // Me una medida cada 241us, unas 6000 medidas en los 1500ms.
  // El resto de las mediciones y la transmisión de LoRA parecen no afectarse a pesar de que la lectura es, en teoria, Blocking.

  // Ago2023 - agrego control del OPAMP mediante salida digital D4 (la pierdo como indicador de si corre mas abajo)
  digitalWrite(4,true);
  // 
  unsigned long ahora = millis();
  int max=0;
  int a0;
  int a1;
  //bool digital=false;
  while (millis()< ahora+1500) 
  {
    a0 = analogRead(A0);
    a1 = analogRead(A1);
    if (a0 > max) max=a0;
    if (a1 > max) max=a1;
    //digital = !digital;      // Esto era un indicador rudimentario para troubleshooting
    //digitalWrite(4,digital); // le conectaba D4 al osciloscopio y veia cuando corria esto
  }
  // Ago 2023
  digitalWrite(4,false);
  //
  VALUEA0 = max;
}

void PrintResetReason()
{
  uint8_t mcusr_copy = MCUSR;
  MCUSR = 0;
  Serial.print("MCUSR:");
  if(mcusr_copy & (1<<WDRF)) Serial.print(" WDRF");
  if(mcusr_copy & (1<<BORF)) Serial.print(" BORF");
  if(mcusr_copy & (1<<EXTRF)) Serial.print(" EXTRF");
  if(mcusr_copy & (1<<PORF)) Serial.print(" PORF");
  Serial.println();
}