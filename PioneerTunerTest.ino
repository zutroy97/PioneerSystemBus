
#include <PioneerSystemBus.h>


#include <CmdBuffer.hpp>
#include <CmdCallback.hpp>
#include <CmdParser.hpp>

CmdCallback_P<3> cmdCallback;
PioneerSystemBus bus();

// Sends a command (number/byte) entered on the command line to a connected Pioneer Tuner
void functWrite(CmdParser *myParser)
{
  Serial.print("Received Command ");
  String raw = String(myParser->getCmdParam(1));
  Serial.print(raw);Serial.print(" byte:");
  byte c = (byte)raw.toInt();
  Serial.print(raw);Serial.print(" 0x");
  Serial.println(c, HEX);
  //Send(PIONEER_TUNER, c);
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
  bus.SelectPreset(c);
  printPrompt();
  return;
}

void printPrompt()
{
  Serial.print("> ");
}
void setup() {
  
  Serial.begin(9600);
  
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

