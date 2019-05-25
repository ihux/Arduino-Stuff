//==================================================================================================
//  blucino.c
//  Blucino library implementation
//  interfacing to Bluenetics Bluetooth Mesh module
//
//  Created by Hugo Pristauz on 30.03.2019
//  Copyright © 2019 Bluenetics GmbH. All rights reserved.
//==================================================================================================

#include <assert.h>
#include "blucino.h"

//==================================================================================================
// store pointer to trace object
//==================================================================================================

    static Blucino::Trace *ourPointerTrace = 0;
    
    void Blucino::Trace::store(Trace *pTrc)          // store trace pointer
    {
        ourPointerTrace = pTrc;
    }

//==================================================================================================
// Blucino error handler
//==================================================================================================

    static void BlucinoError(Blucino::Error err, const char *msg)
    {
        Serial.print("*** Blucino error #"); Serial.print((int)err);
        Serial.print(" - "); Serial.println(msg);
        
        switch (err)
        {
            case Blucino::errInit:
                Serial.println("*** Blucino lib not initialized!");  break;
            case Blucino::errMemory:
                Serial.println("*** out of heap memory!");  break;
            case Blucino::errResource:
                Serial.println("*** out of resources!");  break;
            default:
                Serial.println("*** general error!");  break;
        }
        for (;;);   // loop forever
    }

    void Blucino::Mesh::error(Error err, const char *msg) // error method for Mesh class
    {
        BlucinoError(err, msg);
    }

    void Blucino::error(Error err, const char *msg)       // error method for Blucino class
    {
        BlucinoError(err, msg);
    }

    Blucino::Trace *Blucino::Mesh::pTrace()                         // recall trace pointer
    {
        return ourPointerTrace;
    }

//==================================================================================================
// CRC calculations
//==================================================================================================

    uint16_t Blucino::Mesh::crc16(byte data, uint16_t crc)
    {
        static const uint16_t polynomial = 0x8005u;      // CRC16 polynomial used in this algorithm

        for (byte i = 0; i < 8; i++)
        {
            if (((crc & 0x8000) >> 8) ^ (data & 0x80))
            {
                crc = (crc << 1) ^ polynomial;
            }
            else
            {
                crc = (crc << 1);
            }
            data <<= 1;
        }

        return crc;
    }

    uint16_t Blucino::Mesh::crc16(byte *data, size_t len, uint16_t inival)
    {
        uint16_t crc = inival;
        for (size_t i = 0; i < len; i++)
        {
            crc = crc16(data[i], crc);
        }

        return crc;
    }

    uint16_t Blucino::Mesh::crc16(byte len, byte cmd, byte *data)
    {
        uint16_t crc = 0xFFFFu;                           // CRC16 init value
        crc      = crc16(&len, sizeof(len), crc);
        crc      = crc16(&cmd, sizeof(cmd), crc);
        crc      = crc16(data, len, crc);
        return crc;
    }

//==================================================================================================
// receiving a message
//==================================================================================================

    Blucino::Command Blucino::Mesh::receive()
    {
        if (!initialized)
            error(errInit,"Blucino::Mesh::receive()");
            
        for (;;)
        {
            if ( !available() )
                return cmdInvalid;              // message in progress is not yet complete

            byte b = read();                    // otherwise read next byte from UART
            tracehex(2," ", b);                      // echo received byte

            switch (state)
            {
                case iniState:
                    index = 0;                  // init payload index
                    if (b == preamble1)
                        state = preState;
                    else
                    {
                        trace(3,"\n>>> ignoring byte <");
                        tracehex(3,b);
                        traceln(3,"> while seeking for preamble");
                    }
                    continue;

                case preState:
                    if (b == preamble2)
                        state = sizeState;      // next byte means payload size
                    else
                    {
                        state = iniState;       // otherwise start from INIT
                        trace(3,"\n>>> ignoring byte <");
                        tracehex(3,b);
                        traceln(3,"> while seeking for preamble");
                    }
                    continue;

                case sizeState:
                    frame.len = (b <= sizeof(frame.pay)) ? b : sizeof(frame.pay);
                    state = cmdState;           // next byte is command
                    continue;

                case cmdState:
                    frame.cmd = (Command)b; 
                    state = payState;  // next bytes are comprising payload
                    continue;

                case payState:
                    frame.pay[index++] = b;
                    if (index >= frame.len)
                        state = checkState;
                    continue;

                case checkState:
                    crc = b;  state = finState;
                    continue;

                case finState:
                    crc +=  ((uint16_t)b) << 8;
                    bool valid = (crc == crc16(frame.len, frame.cmd, frame.pay));

                    state = iniState;
                    traceln(2,"");                     // terminate byte echo on trace level 1
                    traceln(2,"message complete!");  

                    if (valid)
                        return frame.cmd;              // received message complete
                    else
                        traceln(3," bad CRC!");
            } // switch
        } // for
        return cmdInvalid;       // to make compiler happy :-)
    }

//===================================================================================================
// invoking THE Blucino event callback
//===================================================================================================

    int Blucino::Mesh::event(Command cmd, Message *pMsg)
    {
        if (callback != NULL)
            return callback(cmd, pMsg);
        return -1;
    }

//===================================================================================================
// syntactic sugar
//===================================================================================================

    int Blucino::Mesh::rxModels(uint16_t *mids, byte count)
    {
        Blucino::Message msg = { .models = { .mids = mids, .count = count} };
        traceln(0,"rxModels(): cmdRxModels");
        int failed = validate(mids,count);                  // validate mesh models
        if (failed > 0)
        {
            trace(0,"*** ");
            trace(0,failed);
            traceln(0," unsupported mesh model(s)!");
        }
        return callback(cmdRxModels, &msg);
    }

    int Blucino::Mesh::txCreate(byte *pRegistry, byte len)
    {
        Blucino::Message msg = { .registry = { .pRegistry = pRegistry, .len = len} };
        return callback(cmdTxCreate, &msg);
    }

    int Blucino::Mesh::rxCreate(byte *pIids, byte len)
    {
        Blucino::Message msg = { .iids = { .pIids = pIids, .len = len} };
        return callback(cmdRxCreate, &msg);
    }

//==================================================================================================
// Blucino loop function - triggers 'all' activities
//==================================================================================================

    static void byte2word(byte *pb, uint16_t *pw, byte count)  // convert from little endian representation 
    {
        for (int i=0; i < count; i++)
            pw[i] = ((uint16_t)pb[2*i+1])*256 + pb[2*i];  // little endian
    }
  
    int Blucino::Mesh::loop()
    {
        if (!initialized)
            error(errInit,"Blucino::loop()");

        Command cmd = receive();      // receive UART command

        if (cmd != cmdInvalid)
        {
            trace(2,"UART command <");
            tracehex(2,(byte)cmd);
            traceln(2,"> received");
        }

        switch (frame.cmd)
        {
            case cmdInvalid:
                break;                // command not yet complete
            case cmdRxModels:
            {
                byte count = frame.len/2;
                uint16_t mids[count]; 
                byte2word(frame.pay,mids,count); 
                rxModels(mids,count);
                break;
            }
            case cmdRxCreate:
                rxCreate(frame.pay,frame.len);
                break;
            default:
                traceln(0,"Blucino::loop: undispatched message");
        }
        return frame.cmd;
    }

//==================================================================================================
// mesh model instances
//==================================================================================================

#define MAX_MODEL_INSTANCES 32

    typedef struct
            {
                byte hdl;             // modem handle (like iid for modem) 
                uint16_t mid;         // mesh model ID
                Blucino::Callback cb; // event callback
                bool support;         // is mesh model supported by Mesh modem
            } Instance;

    static Instance ourInstances[MAX_MODEL_INSTANCES];
    static byte ourInstanceCount = 0;

    static byte allocInstance()
    {
        if (ourInstanceCount + 1 >= MAX_MODEL_INSTANCES)
            BlucinoError(Blucino::errResource,"");

        return ourInstanceCount++;
    }

    static Instance *instancePointer(byte iid)
    {
        return ourInstances + iid;   
    }

    static int instanceCount()
    {
        return ourInstanceCount;   
    }

//==================================================================================================
// mesh model class members
//==================================================================================================

    void Blucino::Model::init(uint16_t mid, Blucino::Callback cb)
    {
        iid = allocInstance();
        Instance *pi = instancePointer(iid);
        pi->hdl = 0xff;
        pi->mid = mid;
        pi->cb = cb;
        pi->support = false;                  // not supported by default
        
        trace(1,"Mesh model <");
        tracewrd(1,mid);
        traceln(1,"> created");
    }

    Blucino::Trace *Blucino::Model::pTrace()                  // recall trace pointer
    {
        return ourPointerTrace;
    }

//==================================================================================================
// Mesh model validation
//==================================================================================================

    int Blucino::Mesh::validate(uint16_t *mids, byte len)
    {
        byte fail = 0;
        
        int ni = instanceCount();
        for (byte iid=0; iid < ni; iid++)
        {
            Instance *pi = instancePointer(iid);
            for (byte j = 0; j < len; j++)
            {
                 uint16_t mid = mids[j];
                 if (mid == pi->mid)         // can validate :-)
                 {
                     pi->support = true;
                     break;
                 } 
            }    

                // report if model ID is not supported!
                
            if (!pi->support)
            {
                fail++;
                
                trace(0,"*** model <");
                tracewrd(0,pi->mid);
                traceln(0,"> is not supported!");
            }
        }
        return fail;
    }
