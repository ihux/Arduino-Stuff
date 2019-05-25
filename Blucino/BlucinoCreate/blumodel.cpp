//===================================================================================================
//  blumodel.c
//  mesh model internals
//
//  Created by Hugo Pristauz on 28.03.2019.
//  Copyright © 2019 Bluenetics GmbH. All rights reserved.
//===================================================================================================

#include <string.h>
#include "blucino.h"

//===================================================================================================
// order a health client instance
//===================================================================================================
/*
    byte RegisterHealthClt(byte *pr)
    {
        static const byte registry[] =
        {
            lowByte(BM_HEALTH_CLIENT),
            highByte(BM_HEALTH_CLIENT),
        };
        
        byte len = BfArraySize(registry);
        if (pr)
            memcpy(pr, registry, len);
        
        return len;
    }
*/
//===================================================================================================
// order a health server instance
//===================================================================================================

    static byte RegisterHealthSvr(byte *pr)
    {
        static const byte registry[] =
        {
            lowByte (Blucino::midHealthSvr),
            highByte(Blucino::midHealthSvr),

            0x01,   //Number of company IDs
            
            lowByte(SILVAIR_ID),
            highByte(SILVAIR_ID),
        };
        
        byte len = sizeof(registry);
        if (pr)
            memcpy(pr, registry, len);
        
        return len;
    }

//===================================================================================================
// order a generic onoff server instance
//===================================================================================================

    static byte RegisterGenOnOffSvr(byte *pr)
    {
        static const byte registry[] =
        {
            lowByte (Blucino::midGenOnOffSvr),
            highByte(Blucino::midGenOnOffSvr),
        };
        
        byte len = sizeof(registry);
        if (pr)
            memcpy(pr, registry, len);
        
        return len;
    }

//===================================================================================================
// order a generic onoff client instance
//===================================================================================================

    static byte RegisterGenOnOffClt(byte *pr)
    {
        static const byte registry[] =
        {
            lowByte (Blucino::midGenOnOffClt),
            highByte(Blucino::midGenOnOffClt),
        };

        byte len = sizeof(registry);
        if (pr)
            memcpy(pr, registry, len);
            
        return len;
    }

//===================================================================================================
// order a generic level client instance
//===================================================================================================

    static byte RegisterGenLevelClt(byte *pr)
    {
        static const byte registry[] =
        {
            lowByte (Blucino::midGenLevelClt),
            highByte(Blucino::midGenLevelClt),
        };
        
        byte len = sizeof(registry);
        if (pr)
            memcpy(pr, registry, len);
        
        return len;
    }

//===================================================================================================
// order a generic level server instance
//===================================================================================================

    static byte RegisterGenLevelSvr(byte *pr)
    {
        static const byte registry[] =
        {
            lowByte (Blucino::midGenLevelSvr),
            highByte(Blucino::midGenLevelSvr),
        };
        
        byte len = sizeof(registry);
        if (pr)
            memcpy(pr, registry, len);
        
        return len;
    }

//===================================================================================================
// order a model
//===================================================================================================

    byte Blucino::Mesh::addorder(uint16_t mid, byte *pr)  // register model
    {
        switch (mid)
        {
            case Blucino::midHealthSvr:
                return RegisterHealthSvr(pr);
            case Blucino::midGenOnOffSvr:
                return RegisterGenOnOffSvr(pr);
            case Blucino::midGenOnOffClt:
                return RegisterGenOnOffClt(pr);
            case Blucino::midGenLevelSvr:
                return RegisterGenLevelSvr(pr);
            case Blucino::midGenLevelClt:
                return RegisterGenLevelClt(pr);
            default:
                return 0;   // no bytes added to configuration
        }
    }
