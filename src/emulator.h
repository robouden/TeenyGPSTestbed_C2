
/********************************************************************/
// EMULATOR
bool emulatorEnabled;
bool emulatorSDCardEnabled;
#include "TeenyGPSEmulate.h"
TeenyGPSEmulate emulator;

/********************************************************************/
bool emulator_setup(HardwareSerial &serialPort, uint32_t baudRate) {
  if(emulator.init(serialPort, baudRate)) {
    emulatorEnabled = true;
    emulatorSDCardEnabled = false; //sdcard_openLogFile(0);
  } else {
    emulatorEnabled = false;
    emulatorSDCardEnabled = false;
  }
  return emulatorEnabled;
}

/********************************************************************/
void emulator_end() {
  if(emulatorSDCardEnabled) {
    //sdcard_closeLogFile();
  }
}

