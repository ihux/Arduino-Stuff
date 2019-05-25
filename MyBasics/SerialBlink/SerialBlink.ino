// SerialBlink
//
// To run this program the 'Serial Monitor' window must be closed during upload
// After completed upload the Serial Monitor can be opened in Tools>Serial Monitor menu item.

    int ledPin = 13;   
    byte b = 0xA0;
    
    void setup()
    {
        pinMode(ledPin, OUTPUT);                    // init LED pin mode
    
        Serial.begin(9600);                         // init Serial interface
        delay(1000);                                // wait for Serial interface initializing
        
        Serial.println("Serial init complete!\n");  // Serial interface is now initialized      
    }
    void loop() 
    {       
        digitalWrite(ledPin, HIGH);
        delay(200);
        digitalWrite(ledPin, LOW);
        delay(800);

        b++;
        Serial.println(b,HEX);
    }
