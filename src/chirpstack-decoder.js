function decodeUplink(input) {
    if (input.fPort == 1) {
        var decoded = {};
        if(input.bytes[4] != 255){
            decoded.hum = input.bytes[4]; 
        }
        if(input.bytes[2] != 255 || input.bytes[3] != 255) decoded.temp = ((input.bytes[2]<<24>>16 | input.bytes[3]) / 10);
        decoded.AnalogZero=(input.bytes[0]<<8 | input.bytes[1]);
     
        return {
            data: decoded,
            warnings: [],
            errors: []
        };
        
    } else
    {
    return {
      data: null,
      warnings: [],
      errors: []
    };
    }
}
