#ifndef PIONEER_SYSTEM_BUS_H
#define PIONEER_SYSTEM_BUS_H

#include "Arduino.h"

// Device Codes
#define PIONEER_TUNER 0xA4

// PIONEER TUNER Key Codes
#define TUNER_KEY_0   0x00
#define TUNER_KEY_1   0x01
#define TUNER_KEY_2   0x02
#define TUNER_KEY_3   0x03
#define TUNER_KEY_4   0x04
#define TUNER_KEY_5   0x05
#define TUNER_KEY_6   0x06
#define TUNER_KEY_7   0x07
#define TUNER_KEY_8   0x08
#define TUNER_KEY_9   0x09
#define TUNER_KEY_AM  0x0E
#define TUNER_KEY_FM  0x0D
#define TUNER_UPPER_PRESET  0x40 

// Timing delays
#define PIONEER_HDR_MARK    8200
#define PIONEER_HDR_PAUSE   4200
#define PIONEER_BIT_MARK    550
#define PIONEER_ONE_PAUSE   1550 
#define PIONEER_ZERO_PAUSE  550

class PioneerSystemBus {
public:
    PioneerSystemBus();
    //~PioneerSystemBus();
    void SelectPreset(byte presetNumber);



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
    void sendCode(PioneerPacket packet);
    void sendPulse(unsigned int microseconds);
}

#endif