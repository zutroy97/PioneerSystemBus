#include "Arduino.h"
#include "PioneerSystemBus.h"

PioneerSystemBus::PioneerSystemBus() 
{
    pinMode(11, OUTPUT);
    digitalWrite(11, 0); // Make sure we are off
}

void PioneerSystemBus::send(byte device, byte command)
{
    union PioneerPacket packet;
    packet = MakePacket(device, command);
    sendCode(packet);
}

byte PioneerSystemBus::mirror(byte b){
    static byte mirroredLookup[8] = {7,6,5,4,3,2,1,0};
    byte result = 0;
    for (byte i = 0; i < 8; i++)
    {
        if ((b & (1 << i)) > 0)
        {
            result |= (byte)(1 << mirroredLookup[i]);
        }
    }
    return result;
}

// Returns the packet for the device and command given.
PioneerPacket PioneerSystemBus::makePacket(byte device, byte command)
{
  union PioneerPacket packet;
  switch(device)
  {
    // Save some calculation for well known devices
    case PIONEER_TUNER:
      packet.device = 0x25;
      packet.deviceChecksum = 0xDA;
    default:
      packet.device = Mirror(device);
      packet.deviceChecksum = ~packet.device;
  }
  packet.command = Mirror(command);
  packet.commandChecksum = ~packet.command;
  return packet;
}

// protected function - Send the Pioneer Packet
//TODO: Can this use timer interrupt?
void PioneerSystemBus::sendCode(PioneerPacket packet)
{
  byte i = 0;
  //Serial.print("Sending Command: 0x");Serial.println(command, HEX);
  
  sendPulse(PIONEER_HDR_MARK);
  delayMicroseconds(PIONEER_HDR_PAUSE); // Start Pulse
  for (i=0; i < 32; i++){
    sendPulse(BIT_MARK);
    if (packet.full & 0x80000000){
      delayMicroseconds(PIONEER_ONE_PAUSE); // Could this be a State Machine?
      //Serial.print("1");
    }else{
      delayMicroseconds(PIONEER_ZERO_PAUSE); // Could this be a State Machine?
      //Serial.print("0");
    }
    packet.full = packet.full << 1;
  }
  sendPulse(PIONEER_BIT_MARK);
  delay(25); // Tail space - Could this be a State Machine?
}

// protected function
void PioneerSystemBus::sendPulse(unsigned int microseconds)
{
  //digitalWrite(LED_BUILTIN, 1);
  //TODO: Test if digitalWrite is fast enought
  PORTB |= 0x20; // Faster then digitalWrite;
  delayMicroseconds(microseconds); // Could this be a State Machine?
  //digitalWrite(LED_BUILTIN, 0);
  PORTB &= 0xDF;
}

void PioneerSystemBus::SelectPreset(byte presetNumber)
{
    if (presetNumber > 8){ // If in upper preset band
        send(PIONEER_TUNER, TUNER_UPPER_PRESET); // Set range 9-16
        send(PIONEER_TUNER, presetNumber-8);
        send(PIONEER_TUNER, TUNER_UPPER_PRESET); // Clear Memory 9-16 button
    }else{
        send(PIONEER_TUNER, presetNumber);
    }

}