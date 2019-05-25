//===================================================================================================
// BlucinoReceive4.ino
//
//    augmented receiver for mesh UART commands with some whistles and bells
//    working now with Blucino library
//
// Important note:
//    To run this program the 'Serial Monitor' window must be closed during upload. After
//    completed upload the 'Serial Monitor' can be opened in Tools>Serial Monitor menu item.
//
// Resource usage:
//    Sketch uses 9728 bytes (14%) of program storage space (532 bytes more for LED class)
//    Global variables use 2260 bytes (27%) of dynamic memory 
//===================================================================================================

#include "blucino.h"

    Blucino::Trace trace(3);      // add trace instance for augmentation of mesh instance
    Blucino::Mesh mesh;           // augmentation of Mesh instance creation with traceing
    Blucino::Led led;             // declare an LED instance (at default pin number 13)

    void setup()                  // with tracing Serial init is no more required
    {
        mesh.init();              // plain Blucino init (no event callback provided)
    }

    void loop()
    {
        Blucino::Command cmd = mesh.receive();
        if ( cmd )
        {
            trace.print(1,"message <");  trace.print(1,cmd);
            trace.println(1,"> received");
            delay(1000);          // delay 1000 ms
            led.toggle();         // the easiest way to toggle the LED
        }
    }
