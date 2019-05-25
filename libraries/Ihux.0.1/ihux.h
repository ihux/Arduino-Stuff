// ihux.h

#define CCP const char *

class Ihux
{
  private:
    typedef uint16_t wrd;

  private:
    byte level;

  public:
    Ihux(byte lev) { level = lev; }
    bool trc(byte lev) { return (level >= lev); }
    void trace(byte l, CCP p) { if (trc(l)) { DEBUG("     [iHux:%d]      ",level); DEBUG(p);} }
    void print(byte l, CCP p) { if(trc(l)) { DEBUG(p); DEBUG("\n");} }

    void stop(CCP msg) // read byte from serial input
    {
      DEBUG("[*] *** stop! ");  DEBUG(msg);
      DEBUG("\n");
      for(;;);
    }
};

#undef CCP

extern Ihux iHux;    // easy to find these lines by searching 'iHux'
