//===================================================================================================
// BlucinoListen.cpp
//
// To run this program the 'Serial Monitor' window must be closed during upload
// After completed upload the Serial Monitor can be opened in Tools>Serial Monitor menu item.
//===================================================================================================

    typedef uint32_t MsT;

    int ledPin = 13;
    byte b = 0x00;
    MsT timeMs = 0;

    void setup()
    {
        pinMode(ledPin, OUTPUT);

        Serial.begin(9600);
        Serial2.begin(57600);

        delay(1000);                        // wait for completing Serial init
        Serial.println("Serial interface initialized!");
    }

    void printhex(const char *pMsg, byte hex)
    {
        Serial.print(pMsg);
        for (int i=0; i < 2; i++)
        {
            byte dig = (i == 0) ?  (hex / 16) : (hex % 16);
            char ch = (dig <= 9) ? '0' + dig : 'A' + (dig - 10);
            Serial.print(ch);
        }
    }

    void loop()
    {
        static int i = 0;
        static bool junk = true;          // start of junk phase
        
        if (Serial2.available())
        {
            uint8_t b = Serial2.read();
            if (junk && b != 0xAA)
                return;
            
            if (b == 0xAA)
            {
                junk = false;             // end of junk phase
                i = (i+1) % 5;
                if (i == 0)
                {
                    delay(2000);
                    Serial.println("");
                    Serial.println("");
                    junk = true;          // continue with junk phase
                    return;
                }
 
                Serial.println("");
            }
            printhex(" ",b);
        }
    }
  
