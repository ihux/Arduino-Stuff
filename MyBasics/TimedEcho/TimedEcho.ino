//===================================================================================================
// TimedEcho.cpp
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
        delay(1000);                        // wait for completing Serial init
        Serial.println("Serial interface initialized!");        
        
        //Serial2.begin(57600);
        //delay(1000);                      // wait for Serial() init

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
    
    void echo(byte b)
    {
        static byte count = 0;

        printhex(" ", b);
        if (++count == 32)
        {
            count = 0;
            Serial.println("");
        }
    }


    MsT tick(void)                        // get system clock time while blinking LED
    {
        static MsT clockMs = 0;
        MsT periode = 1000;
        MsT duty = 800;

        MsT time = millis();              // system clock time in miliseconds

        while (clockMs <= time)
        {
            clockMs++;                    // increment clock time
        
            if (clockMs % periode == 0)
                digitalWrite(ledPin, LOW);
            else if (clockMs % periode == duty)
                digitalWrite(ledPin, HIGH);
        }

        return clockMs;
    }
    
    void loop() 
    {
        MsT t = tick();

        if (t >= timeMs)
        {
            echo(b++);
            timeMs += 100;      // next echo in 100ms
        }
    }

    
