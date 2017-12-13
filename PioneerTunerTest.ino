// Device Codes
#define PIONEER_TUNER 0xA4

// Timing delays
#define HDR_MARK    8200
#define HDR_PAUSE   4200
#define BIT_MARK    550
#define ONE_PAUSE   1550 
#define ZERO_PAUSE  550

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

#include <CmdBuffer.hpp>
#include <CmdCallback.hpp>
#include <CmdParser.hpp>

CmdCallback_P<3> cmdCallback;

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

// Creates the Pioneer packet for the device and command given, then sends.
void Send(byte device, byte command)
{
  union PioneerPacket packet;
  packet = MakePacket(device, command);
  sendCode(packet);
}

// Returns the packet for the device and command given.
PioneerPacket MakePacket(byte device, byte command)
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

// Return a byte which is a mirror image of the byte passed
// example 1100 0010 mirrored is 0100 0011
byte Mirror(byte b)
{
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

// Sends a command (number/byte) entered on the command line to a connected Pioneer Tuner
void functWrite(CmdParser *myParser)
{
  Serial.print("Received Command ");
  String raw = String(myParser->getCmdParam(1));
  Serial.print(raw);Serial.print(" byte:");
  byte c = (byte)raw.toInt();
  Serial.print(raw);Serial.print(" 0x");
  Serial.println(c, HEX);
  Send(PIONEER_TUNER, c);
}

void functionPreset(CmdParser *myParser)
{
  String raw = String(myParser->getCmdParam(1));
  byte c = (byte) raw.toInt();

  if (c == 0 || c > 16){
    Serial.println(F("Station preset 1 - 16 only."));
    printPrompt();
    return;
  }
  Serial.print(F("Setting station preset ")); Serial.print(c);Serial.println();
  if (c > 8){ // If in upper preset band
    Send(PIONEER_TUNER, TUNER_UPPER_PRESET); // Set range 9-16
    Send(PIONEER_TUNER, c-8);
    Send(PIONEER_TUNER, TUNER_UPPER_PRESET); // Clear Memory 9-16 button
  }else{
    Send(PIONEER_TUNER, c);
  }
  printPrompt();
  return;
}

void printPrompt()
{
  Serial.print("> ");
}
void setup() {
  pinMode(11, OUTPUT);
  Serial.begin(9600);
  digitalWrite(11, 0); // Make sure we are off
  cmdCallback.addCmd(PSTR("W"), &functWrite);
  cmdCallback.addCmd(PSTR("PRESET"), &functionPreset);
  Serial.println(PSTR("READY")); // I miss my old C64
  printPrompt();
}

void loop() {
  CmdBuffer<32> myBuffer;
  CmdParser     myParser;
  cmdCallback.loopCmdProcessing(&myParser, &myBuffer, &Serial);
}

// protected function - Send the Pioneer Packet
//TODO: Can this use timer interrupt?
void sendCode(PioneerPacket packet)
{
  byte i = 0;
  //Serial.print("Sending Command: 0x");Serial.println(command, HEX);
  
  sendPulse(HDR_MARK);
  delayMicroseconds(HDR_PAUSE); // Start Pulse
  for (i=0; i < 32; i++){
    sendPulse(BIT_MARK);
    if (packet.full & 0x80000000){
      delayMicroseconds(ONE_PAUSE); // Could this be a State Machine?
      //Serial.print("1");
    }else{
      delayMicroseconds(ZERO_PAUSE); // Could this be a State Machine?
      //Serial.print("0");
    }
    packet.full = packet.full << 1;
  }
  sendPulse(BIT_MARK);
  delay(25); // Tail space - Could this be a State Machine?
}

// protected function
void sendPulse(unsigned int microseconds)
{
  //digitalWrite(LED_BUILTIN, 1);
  //TODO: Test if digitalWrite is fast enought
  PORTB |= 0x20; // Faster then digitalWrite;
  delayMicroseconds(microseconds); // Could this be a State Machine?
  //digitalWrite(LED_BUILTIN, 0);
  PORTB &= 0xDF;
}
