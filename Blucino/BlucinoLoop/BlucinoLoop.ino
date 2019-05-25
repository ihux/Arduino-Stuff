//===================================================================================================
// BlucinoReceive.ino
//
// To run this program the 'Serial Monitor' window must be closed during upload
// After completed upload the Serial Monitor can be opened in Tools>Serial Monitor menu item.
//===================================================================================================

#include "blucino.h"

    int callback(Blucino::Command cmd, Blucino::Message *pMsg);  // forward declaration

    Blucino::Trace trace(1);     // provide a trace instance for mesh
    Blucino::Mesh  mesh;         // create a mesh object
    Blucino::Led   led;          // provide an LED instance

    Blucino::Model button(Blucino::GenOnOffClt,callback);
    Blucino::Model actor (Blucino::GenOnOffSvr,callback);

    int callback(Blucino::Command cmd, Blucino::Message *pMsg)
    {
        trace.print(1,"callback: command <");
        trace.print(1,cmd);
        trace.println(1,">");
        
        switch (cmd)
        {
            case Blucino::cmdRxModels:
                 {
                      Blucino::MsgModels *p = &pMsg->models;
                      trace.println(1,"Supported models");
                      for (int i=0; i < p->count; i++)
                      {
                          trace.print(1,"   ");  trace.wrd(1,p->mids[i]);
                          trace.println(1,"");
                      }
                 }
                 return 0;
            default:
                trace.print(1,"unsupported command: ");  trace.println(1,cmd);
        }
        return 0;
    }

    void setup()
    {
        trace.set(2);             // set trace level 2 (uncomment if you won't see tracing)
        mesh.init(callback);      // init Blucino, provide event callback
    }

    void loop()
    {
        static int i = 0;
     
        if (mesh.loop())
            for(;;);
            
        delay(2000);
        i++;
    }
