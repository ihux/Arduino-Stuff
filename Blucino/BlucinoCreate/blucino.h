//==================================================================================================
//  blucino.h
//  Blucino library interface to Bluenetics Bluetooth Mesh module
//
//  Created by Hugo Pristauz on 30.03.2019
//  Copyright © 2019 Bluenetics GmbH. All rights reserved.
//==================================================================================================

#ifndef BLUCINO_H
#define BLUCINO_H

#include <assert.h>
#include <stdint.h>
#include "arduino.h"

#undef lowByte
#undef highByte

    inline byte lowByte(uint16_t w)
    {
        return (byte)(w & 0xFF);
    }

    inline byte highByte(uint16_t w)
    {
        return (byte)((w >> 8) & 0xFF);
    }
    
//==================================================================================================
// THE Blucino class
//==================================================================================================

    #ifndef Uart
        #define Uart Serial2
    #endif
    #define SILVAIR_ID                     0x0136u

                        //===========================================//

    class Blucino
    {
        private: // private constants
            static const byte preamble1 = 0xAA;
            static const byte preamble2 = 0x55;

                        //===========================================//

        public: // supported generic mesh model ID
            static const uint16_t midConfigSvr   = 0x0000;
            static const uint16_t midConfigClt   = 0x0001;
            
            static const uint16_t midHealthSvr   = 0x0002;
            static const uint16_t midHealthClt   = 0x0003;
            
            static const uint16_t midGenOnOffSvr = 0x1000;
            static const uint16_t midGenOnOffClt = 0x1001;

            static const uint16_t midGenLevelSvr = 0x1002;
            static const uint16_t midGenLevelClt = 0x1003;

        public: // public types
            typedef enum
                {
                    errInit       = 0x01u,    // Blucino lib not initialized
                    errMemory     = 0x02u,    // out of dynamic memory
                    errResource   = 0x03u,    // out of static resources
                    errAny        = 0x04u,    // any other error
                } Error;
            typedef enum
                {
                    cmdInvalid    = 0x00u,    // invalid command
                    cmdTxPing     = 0x01u,    // ping request
                    cmdRxPong     = 0x02u,    // pong response
                    cmdRxModels   = 0x03u,    // available models (needed during init device)
                    cmdTxCreate   = 0x04u,    // request model instance creation (create instances request)
                    cmdRxCreate   = 0x05u,    // receive created instance IDs (create instance response)
                    cmdInitNode   = 0x06u,    // (init node event)
                    cmdTxMesh     = 0x07u,    // transmit mesh message (mesh message request)
                    cmdTxStart    = 0x09u,    // request to start node (start node request)
                    cmdRxStart    = 0x0Bu,    // node has been started (start node response)
                    cmdTxFactory  = 0x0Cu,    // (factory reset request)
                    cmdRxFactory  = 0x0Du,    // (factory reset response)
                    cmdRxReset    = 0x0Eu,    // (factory reset event)
                    cmdRxMesh     = 0x0Fu,    // receive mesh message (mesh message response)
                      /*
                    BLUCINO_CMD_MESH_MESSAGE_RESPONSE             = 0x0Fu,
                    BLUCINO_CMD_CURRENT_STATE_REQUEST             = 0x10u,
                    BLUCINO_CMD_CURRENT_STATE_RESPONSE            = 0x11u,
                    BLUCINO_CMD_ERROR                             = 0x12u,
                    BLUCINO_CMD_MODEM_FIRMWARE_VERSION_REQUEST    = 0x13u,
                    BLUCINO_CMD_MODEM_FIRMWARE_VERSION_RESPONSE   = 0x14u,
                    BLUCINO_CMD_SENSOR_UPDATE_REQUEST             = 0x15u,
                    BLUCINO_CMD_ATTENTION_EVENT                   = 0x16u,
                    BLUCINO_CMD_SOFTWARE_RESET_REQUEST            = 0x17u,
                    BLUCINO_CMD_SOFTWARE_RESET_RESPONSE           = 0x18u,
                    BLUCINO_CMD_SENSOR_UPDATE_RESPONSE            = 0x19u,
                    BLUCINO_CMD_DEVICE_UUID_REQUEST               = 0x1Au,
                    BLUCINO_CMD_DEVICE_UUID_RESPONSE              = 0x1Bu,
                    BLUCINO_CMD_SET_FAULT_REQUEST                 = 0x1Cu,
                    BLUCINO_CMD_SET_FAULT_RESPONSE                = 0x1Du,
                    BLUCINO_CMD_CLEAR_FAULT_REQUEST               = 0x1Eu,
                    BLUCINO_CMD_CLEAR_FAULT_RESPONSE              = 0x1Fu,
                    BLUCINO_CMD_START_TEST_REQ                    = 0x20u,
                    BLUCINO_CMD_START_TEST_RESP                   = 0x21u,
                    BLUCINO_CMD_TEST_FINISHED_REQ                 = 0x22u,
                    BLUCINO_CMD_TEST_FINISHED_RESP                = 0x23u,
                    BLUCINO_CMD_FIRMWARE_VERSION_SET_REQ          = 0x24u,
                    BLUCINO_CMD_FIRMWARE_VERSION_SET_RESP         = 0x25u,

                    BLUCINO_CMD_DFU_INIT_REQ                      = 0x80u,
                    BLUCINO_CMD_DFU_INIT_RESP                     = 0x81u,
                    BLUCINO_CMD_DFU_STATUS_REQ                    = 0x82u,
                    BLUCINO_CMD_DFU_STATUS_RESP                   = 0x83u,
                    BLUCINO_CMD_DFU_PAGE_CREATE_REQ               = 0x84u,
                    BLUCINO_CMD_DFU_PAGE_CREATE_RESP              = 0x85u,
                    BLUCINO_CMD_DFU_WRITE_DATA_EVENT              = 0x86u,
                    BLUCINO_CMD_DFU_PAGE_STORE_REQ                = 0x87u,
                    BLUCINO_CMD_DFU_PAGE_STORE_RESP               = 0x88u,
                    BLUCINO_CMD_DFU_STATE_CHECK_REQ               = 0x89u,
                    BLUCINO_CMD_DFU_STATE_CHECK_RESP              = 0x8Au,
                    BLUCINO_CMD_DFU_CANCEL_REQ                    = 0x8Bu,
                    BLUCINO_CMD_DFU_CANCEL_RESP                   = 0x8Cu,
                  */
                } Command;

            typedef struct
            {
                uint16_t *mids;      // points to array of supported model IDs
                byte count;          // number of model IDs (not length in bytes!!!)
            } MsgModels;

            typedef struct
            {
                byte *pRegistry;     // points to byte array holding registry info
                byte len;            // length of registry array in bytes
            } MsgRegistry;

            typedef struct
            {
                byte *pIids;         // points to byte array holding instance ID info
                byte len;            // length of instance ID array in bytes
            } MsgIids;

            typedef union
            {
                MsgModels    models;
                MsgRegistry  registry;
                MsgIids      iids;
            } Message;

            typedef int (*Callback)(Command cmd, Message *pMessage);

                        //===========================================//

        private: // private types
            typedef enum
                {iniState, preState, sizeState, cmdState, payState, checkState, finState } StateT;
            typedef const char *ccp;
            typedef struct
                {
                    Command cmd;              // message command
                    byte pay[127];            // message payload
                    byte len;                 // message length
                } Frame;                      // UART message frame (receive)
        
                        //===========================================//

        private: // private methods
            byte lowbyte(uint16_t w)          { return (byte)(w & 0xFF); }
            byte highbyte(uint16_t w)         { return (byte)((w >> 8) & 0xFF); }
        
                        //===========================================//

        public: // the Blucino class
            Blucino()  {  error(errInit,"Blucino(): not allowed to use the Blucino() constructor"); }
            void error(Error err, const char *msg);
                        //===========================================//

        public: // an LED class
            class Led
            {
                private:
                    byte pin;                                                  // pin number
                    bool onoff;
                    void update()         { digitalWrite(pin,onoff); }
                public:
                    Led(int adr = 13)     { pin = adr; pinMode(pin,OUTPUT); }  // default pin number is 13
                    void on()             { onoff = true; update(); }
                    void off()            { onoff = false; update(); }
                    void toggle()         { onoff = !onoff; update(); }
            };

                        //===========================================//

        public:  // a Trace class
            class Trace
            {
                private:
                    byte level;                               // trace level threshold
                    byte cnt;

                private:
                    void store(Trace *pTrc);                  // store trace pointer
                public:
                    Trace(byte lev=0)
                        { store(this); Serial.begin(9600); while(!Serial); level = lev; cnt = 0; }
                    void hex(const char *pMsg, byte b)
                    {
                        Serial.print(pMsg);
                        for (int i=0; i < 2; i++)
                        {
                            byte dig = (i == 0) ?  (b / 16) : (b % 16);
                            char ch = (dig <= 9) ? '0' + dig : 'A' + (dig - 10);
                            Serial.print(ch);

                            cnt = (cnt + 1) % 30;
                            if (cnt == 0)
                                Serial.println("");
                        }
                    }
                    void hex(byte b)   { hex("", b); }            // print hex byte without message
                    void hex(byte lev, byte b)                    { if (level >= lev) hex(b); }
                    void hex(byte lev, const char *pMsg, byte b)  { if (level >= lev) hex(pMsg,b); }

                    void wrd(byte lev, const char *pMsg, uint16_t w)
                        { if (level >= lev) { hex(pMsg,(byte)(w>>8)); hex((byte)(w&0xFF)); }}
                    void wrd(const char *pMsg, uint16_t w)        { wrd(0,pMsg,w); }
                    void wrd(uint16_t w)                          { wrd("", w); }
                    void wrd(byte lev, uint16_t w)                { if (level >= lev) wrd(w); }

                    void println(byte lev, const char *msg)       { if (level >= lev) Serial.println(msg); }
                    void println(const char *msg)                 { println(0,msg); }
                    void println(byte lev, int i)                 { if (level >= lev) Serial.println(i); }
                    void println(int i)                           { println(0,i); }
                    void print(byte lev, const char *msg)         { if (level >= lev) Serial.print(msg); }
                    void print(const char *msg)                   { print(0,msg); }
                    void print(byte lev, int i)                   { if (level >= lev) Serial.print(i); }
                    void print(int i)                             { print(0,i); }
                    byte get() { return level; }                  // get trace level
                    void set(byte lev) { level = lev; }           // set trace level
            };

                        //===========================================//

        public: // a Mesh class - communicating via UART with a Bluetooth mesh processor
            class Mesh
            {

                private: // private data members
                    bool initialized;                           // init status
                    byte state;                                 // state of receiver finite state machine
                    byte index;                                 // payload receiving index
                    uint16_t crc;                               // crc check sum
                    Frame frame;                                // received message frame from UART
                    Callback callback;                          // common event callback

                private: // private methods
                    byte addorder(uint16_t mid, byte *pr);   // order a mesh model
                    void order();
                    uint16_t crc16(byte data, uint16_t crc);
                    uint16_t crc16(byte len, byte cmd, byte *data);
                    uint16_t crc16(byte * data, size_t len, uint16_t inival);
                    void error(Error err, const char *msg);
                    Trace *pTrace();
                    void traceln(byte l,ccp p)   { if (pTrace()) pTrace()->println(l,p); }
                    void traceln(ccp p)          { traceln(0,p); }
                    void trace(byte l,ccp p)     { if (pTrace()) pTrace()->print(l,p); }
                    void trace(ccp p)            { trace(0,p); }
                    void trace(byte l,int i)     { if (pTrace()) pTrace()->print(l,i); }
                    void tracehex(byte l,ccp m,byte b) { if (pTrace()) pTrace()->hex(l,m,b); }
                    void tracehex(byte l,byte b) { tracehex(l,"",b); }
                    void tracehex(byte b)        { tracehex(0,b); }
                    void tracewrd(byte l,ccp m,uint16_t w) { if (pTrace()) pTrace()->wrd(l,m,w); }
                    void tracewrd(byte l,uint16_t w) { tracewrd(l,"",w); }
                    void tracewrd(uint16_t w)    { tracewrd(0,w); }

                        //===========================================//

                public: // public methods
                    Mesh()                       { initialized = false; }
                    void init(Callback cb=0)
                         {
                             assert(sizeof(byte)==1);
                             callback = cb; state = iniState; index = 0;
                             Uart.begin(57600); while(!Uart);
                             initialized = true;
                         }
                    void write(byte b)           { Uart.write(b); }
                    byte read()                  { return Uart.read(); }
                    bool available()             { return Uart.available(); }
                    int  validate(uint16_t *mids, byte len);
                    Command receive();           // construct message frame, return if complete
                    void transmit(byte len, byte cmd, byte *pPay);
                    int event(Command cmd, Message *pMessage);
                    int rxModels(uint16_t *pMids, byte count);
                    int txCreate(byte *pRegistry, byte len);
                    int rxCreate(byte *pInst, byte len);

                    int loop();                  // Blucino loop - all loop activities
            }; // class Blucino::Mesh

                        //===========================================//

        public: // a Mesh class - communicating via UART with a Bluetooth mesh processor

            class Model
            {
                private:
                    byte iid;

                private:
                   void init(uint16_t mid, Callback cb);
                   Trace *pTrace();
                    void traceln(byte l,ccp p)   { if (pTrace()) pTrace()->println(l,p); }
                    void traceln(ccp p)          { traceln(0,p); }
                    void trace(byte l,ccp p)     { if (pTrace()) pTrace()->print(l,p); }
                    void trace(ccp p)            { trace(0,p); }
                    void tracehex(byte l,ccp m,byte b) { if (pTrace()) pTrace()->hex(l,m,b); }
                    void tracehex(byte l,byte b) { tracehex(l,"",b); }
                    void tracehex(byte b)        { tracehex(0,b); }
                    void tracewrd(byte l,ccp m,uint16_t w) { if (pTrace()) pTrace()->wrd(l,m,w); }
                    void tracewrd(byte l,uint16_t w) { tracewrd(l,"",w); }
                    void tracewrd(uint16_t w)    { tracewrd(0,w); }

                public:
                    Model(uint16_t mid, Callback cb=0) { init(mid,cb); }
                    byte getIid() { return iid; }
            };

    };  // class Blucino

//==================================================================================================
// Blucino Mesh Model
//==================================================================================================


#endif // BLUCINO_H
