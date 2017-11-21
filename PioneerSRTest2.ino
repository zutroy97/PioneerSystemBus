#define PIONEER_TUNER 0xA4

#define HDR_MARK    8200
#define HDR_PAUSE   4200
#define BIT_MARK    550
#define ONE_PAUSE   1550 
#define ZERO_PAUSE  550

#define KEY_0   0x00
#define KEY_1   0x01
#define KEY_2   0x02
#define KEY_3   0x03
#define KEY_4   0x04
#define KEY_5   0x05
#define KEY_6   0x06
#define KEY_7   0x07
#define KEY_8   0x08
#define KEY_9   0x09
#define KEY_AM  0x0E
#define KEY_FM  0x0D

#include <CmdBuffer.hpp>
#include <CmdCallback.hpp>
#include <CmdParser.hpp>

CmdCallback<3> cmdCallback;
char strWrite[] = "W";
char strPlus[] = "+";
// Nano 328P

union pioneerPacket
{
  unsigned long full;
  struct
  {
    byte commandChecksum;
    byte command;
    byte deviceChecksum;
    byte device;
  };
};

union pioneerPacket packet;

#define NUM_COMMANDS 2
unsigned long commands[NUM_COMMANDS] = {KEY_AM, KEY_FM};

byte mirroredLookup[8] = {7,6,5,4,3,2,1,0};
void makePacket(byte device, byte command)
{
  switch(device)
  {
    case PIONEER_TUNER:
      packet.device = 0x25;
      packet.deviceChecksum = 0xDA;
    default:
      packet.device = Mirror(device);
      packet.deviceChecksum = ~packet.device;
  }
  packet.command = Mirror(command);
  packet.commandChecksum = ~packet.command;
}



byte Mirror(byte b)
{
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

void functWrite(CmdParser *myParser)
{
  Serial.print("Received Command ");
  String raw = String(myParser->getCmdParam(1));
  Serial.print(raw);Serial.print(" byte:");
  byte c = (byte)raw.toInt();
  Serial.print(raw);Serial.print(" 0x");
  Serial.print(c, HEX);
  Serial.print(" mirror: 0x");Serial.println(Mirror(c));
  makePacket(0xA4, c);sendCode(packet.full);
}

void functPlus(CmdParser *myParser)
{
  byte c = 0;
  c |= (1 << 7);
  Serial.println(c, HEX);
  packet.device = 0x25;
  packet.deviceChecksum = 0xDA;
  packet.command = 0x6A;
  packet.commandChecksum = 0x95;
  sendCode(packet.full);
  
}
void setup() {
  // put your setup code here, to run once:
  pinMode(11, OUTPUT);
  Serial.begin(9600);
  digitalWrite(11, 0);
  cmdCallback.addCmd(strWrite, &functWrite);
  cmdCallback.addCmd(strPlus, &functPlus);
  Serial.println("Ready");
}

void AmFmLoop()
{
  Serial.println("Sending AM");
  makePacket(0xA4, 0x0E);sendCode(packet.full);
  delay(5000);
  Serial.println("Sending FM");
  makePacket(0xA4, 0x0D);sendCode(packet.full);
  delay(5000);  
}
void loop() {
  CmdBuffer<32> myBuffer;
  CmdParser     myParser;
  //AmFmLoop();
  cmdCallback.loopCmdProcessing(&myParser, &myBuffer, &Serial);
  //sendAllTheCodes();

}

void sendAllTheCodes()
{
  for (int i = 0x0A; i < 0x0F; i++)
  {
    makePacket(0xA4, (byte)i);
    Serial.print(i);Serial.print(") ");
    sendCode(packet.full);
    delay(2000);
  }
}
void sendCode(unsigned long command)
{
  byte i = 0;
  Serial.print("Sending Command: 0x");Serial.println(command, HEX);
  
  sendPulse(HDR_MARK);
  delayMicroseconds(HDR_PAUSE);
  for (i=0; i < 32; i++){
    sendPulse(BIT_MARK);
    if (command & 0x80000000){
      delayMicroseconds(ONE_PAUSE);
      //Serial.print("1");
    }
    else{
      delayMicroseconds(ZERO_PAUSE);
      //Serial.print("0");
    }

    command = command << 1;
  }
  sendPulse(BIT_MARK);
}

void sendPulse(unsigned int microseconds)
{
  //digitalWrite(LED_BUILTIN, 1);
  PORTB |= 0x20;
  delayMicroseconds(microseconds);
  //digitalWrite(LED_BUILTIN, 0);
  PORTB &= 0xDF;
}
