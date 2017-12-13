#ifndef PIONEER_SYSTEM_BUS_H
#define PIONEER_SYSTEM_BUS_H

#include "Arduino.h"

class PioneerSystemBus {
public:
    PioneerSystemBus();
    ~PioneerSystemBus();



private:
    // Breaks down the 32 bit value into Pioneer specific bytes
    union PioneerPacket
    {
        unsigned long full;
        struct
        {
            byte commandChecksum; // Calculated by makePacket
            byte command; // Command to send. ex. KEY_FM
            byte deviceChecksum; // Calculated by makePacket
            byte device; // ex. PIONEER_TUNER
        };
    };
    void send(byte device, byte command);
    PioneerPacket makePacket(byte device, byte command);
    byte mirror(byte b);

}

#endif