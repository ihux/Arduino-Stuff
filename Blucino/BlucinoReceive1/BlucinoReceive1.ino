//===================================================================================================
// BlucinoReceive1.ino
//
//    simple receiver for mesh UART commands without whistles and bells
//    means: no tracing and no LED blinking
//
// Important note:
//    To run this program the 'Serial Monitor' window must be closed during upload. After
//    completed upload the 'Serial Monitor' can be opened in Tools>Serial Monitor menu item.
//
// Resource usage:
//    Sketch uses 8948 bytes (14%) of program storage space
//    Global variables use 2256 bytes (27%) of dynamic memory (168 bytes more for mesh instance)
// Mesh instance:
//    bool initialized:    1
//    byte state:          1
//    byte index:          1
//    uint16_t crc:        2
//    Frame frame:           
//      Command cmd:       1
//      byte pay[127]:   127
//      byte len:          1
//    BlucinoCb callback:  4
//    Trace *pTrace:       4
// __________________________
//    Total              142    (26 bytes less than listed above)
//===================================================================================================

#include "blucino.h"

    typedef unsigned long MsT;
    
    Blucino::Mesh mesh;           // simplest form of Mesh instance creation

    void setup()
    {
        Serial.begin(9600);       // use Serial UART for tracing
        while (!Serial);          // wait until Serial initialized
        mesh.init();              // plain Blucino init (no event callback provided)
    }

    void loop()
    {
        Blucino::Command cmd = mesh.receive();
        if ( cmd )
        {
            Serial.print("message <");  Serial.print(cmd);  Serial.println("> received");
            delay(1000);          // delay 1000 ms
        }
    }

    
