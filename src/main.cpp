// TeenyGPSEmulator Arduino Sketch
// 2023-12-08 by Michael Beakes <beakes@bezwax.com>
//
// Changelog:
// 2023-03-17 - Initial release
//
// To-Do:
//

#include <M5Core2.h>

#include <Arduino.h>

// When defined, measures and displays max loop iteration time
//#define DEBUG_LOOP_TIMING
//#define DEBUG_LOOP_PERIOD 2500

/********************************************************************/
// Reset
void device_reset() {
  // send reboot command -----
  ESP.restart();
}

/********************************************************************/
// Device Modes
enum device_mode_t : int {
  DM_IDLE = 0,
  DM_GPSRCVR,
  DM_GPSLOGR,
  DM_GPSNSAT,
  DM_GPSCAPT,
  DM_GPSSSTP,
  DM_GPSEMUL
};

/********************************************************************/
// Device State
typedef struct {
  int16_t  DEVICE_MODE = DM_IDLE;
  int16_t  EMUL_NUMCOLDSTARTPVTPACKETS = 10;
  int16_t  DISPLAYTIMEOUT = 10;
  bool     STATUSLED = true;
  uint8_t  pad0;
} device_state_t;
device_state_t deviceState;

/********************************************************************/
// Prototypes
#include "prototypes.h"

/********************************************************************/
// RTC
#include "rtc.h"

/********************************************************************/
// GPS
HardwareSerial *gpsSerial;
#include "gps.h"

/********************************************************************/
// SD Card
#include "sdcard.h"

/********************************************************************/
// Emulator
HardwareSerial *emulatorSerial;
#include "emulator.h"

/********************************************************************/
// Display
#include "display_menu.h"
#include "msg.h"

/********************************************************************/
// Buttons
#include "buttons.h"

/********************************************************************/
// Pulsed Outputs
//#include <TeenyPulser.h>
// Status LED Defines
//TeenyPulser statusLED(13, true, 20, 50, 4000);

/********************************************************************/
// Device Mode
#include "mode.h"

/********************************************************************/
// Periodic Timer
#include "timer.h"

/********************************************************************/
// Display Draw
#include "display_draw.h"


/********************************************************************/
void setup() {

  // setup M5
  M5.begin();

  // setup statusLED
//  statusLED.init();

  // Setup display
  display_setup();

  //Setup buttons
  if(displayEnabled) {
    buttons_setup();
    if(displayEnabled) {
      displayPV.prt_str("- Buttons Enabled", 20, 0, 16);
      display_display();
      delay(1000);
    }
  } else {
    if(displayEnabled) {
      displayPV.prt_str("- Buttons Missing", 20, 0, 16);
      display_display();
      delay(1000);
    }
  }

  //Setup rtc
  rtc_setup();
  if(displayEnabled) {
    displayPV.prt_str("- Setup RTC", 20, 0, 32);
    display_display();
    delay(1000);
  }

  //Setup sdcard
  if(sdcard_setup()) {
    if(displayEnabled) {
      displayPV.prt_str("- SD Card Enabled", 20, 0, 48);
      display_display();
      delay(1000);
    }
  } else {
    if(displayEnabled) {
      displayPV.prt_str("- SD Card Missing", 20, 0, 48);
      display_display();
      delay(1000);
    }
  }

  //Select device mode
  sdcard_deviceStateRestore();

  // Initalize device mode
  deviceMode_init();

  // setup timer
  itimer0_setup();

  if(displayEnabled) {
    displayPV.prt_str("Setup Complete", 20, 0, 112);
    display_display();
    delay(1000);
    display_clearDisplay();
    display_display();
  }

  // setup and init menu
  if(displayEnabled) {
    menu_setup(); // must be done after device_state_restore()
    menu_init();
    menu_enter();
  }

}


/********************************************************************/
void loop() {

#ifdef DEBUG_LOOP_TIMING
  char _msgStr[22];
  uint32_t _nowMS = millis();
  static uint32_t max_loop_time=0;
  static uint32_t max_loop_check=_nowMS;
#endif

  // Update clock
  rtc_datetime_t _rtcTime = getRTCTime(); // get the RTC
  uint32_t _clockTime = (uint32_t)(_rtcTime.hour*3600) + (uint32_t)(_rtcTime.minute*60) + _rtcTime.second;
  static uint32_t _prevClockTime = 86400; // This is 24hr rollover seconds so it will never match _clockTime
  bool _clockTick_1sec = false;
  if((_prevClockTime) != (_clockTime)) {
    displayRefresh = true;
    _prevClockTime = _clockTime;
    _clockTick_1sec = true;
  }

  // Update device based on mode
  switch(deviceState.DEVICE_MODE) {
    case DM_GPSRCVR:
      if(gps.getNAVPVT()) {
        if((!clockTime_valid) && gps.isDateValid() && gps.isTimeValid()) {
          setRTCTime(gps.getHour(), gps.getMinute(), gps.getSecond(),
                     gps.getDay(), gps.getMonth(), gps.getYear());
          clockTime_valid = true;
        }
        displayRefresh = true;
      }
      break;
    case DM_GPSLOGR:
      if(gps.getNAVPVT()) {
        if((!clockTime_valid) && gps.isDateValid() && gps.isTimeValid()) {
          setRTCTime(gps.getHour(), gps.getMinute(), gps.getSecond(),
                     gps.getDay(), gps.getMonth(), gps.getYear());
          clockTime_valid = true;
        }
        if(ubxLoggingInProgress) {
          uint8_t pvtPacket[UBX_NAV_PVT_PACKETLENGTH];
          gps.getNAVPVTPacket(pvtPacket);
          sdcard_writeLoggingFile(pvtPacket, UBX_NAV_PVT_PACKETLENGTH);
          ubxLoggingFileWriteCount++;
          if(gps.isLocationValid()) ubxLoggingFileWriteValidCount++;
        }
        displayRefresh = true;
      }
      break;
    case DM_GPSNSAT:
      if(gps.getNAVPVT()) {
        if((!clockTime_valid) && gps.isDateValid() && gps.isTimeValid()) {
          setRTCTime(gps.getHour(), gps.getMinute(), gps.getSecond(),
                     gps.getDay(), gps.getMonth(), gps.getYear());
          clockTime_valid = true;
        }
        displayRefresh = true;
      } else if(gps.getNAVSAT()) {
        displayRefresh = true;
      }
      break;
    case DM_GPSCAPT:
      if(menu_captRxPktInProgress) {
        sdcard_writeRxPktFile();
      }
      break;
    case DM_GPSSSTP:
      break;
    case DM_GPSEMUL:
      // Process host commands
      emulator.processIncomingPacket();
      // Transmit autoPVT packets if enabled
//*** NEED TO INCORPORATE TRANSMISSION RATE INTO sendPVTPacket()
//*** ALSO NEED TO FACTOR IN LOG RATE VS TRANSMISSION RATE
//uint32_t getPVTTransmissionRate();
      uint8_t _ubxPVTBuf[100];
      ubxNAVPVTInfo_t _ubxNAVPVTInfo;
      static uint8_t _coldPVTPacketCount = 0;
      if(_clockTick_1sec &&
         (emulator.isAutoPVTEnabled() || emulator.isPVTPacketRequested())) {
        if(!clockTime_valid) {
          if(_coldPVTPacketCount < deviceState.EMUL_NUMCOLDSTARTPVTPACKETS) {
            _coldPVTPacketCount++;
            emulator.setPVTColdPacket();
            emulator.sendPVTPacket();
            //statusLED.pulse(1);
          } else {
            if(emulatorSDCardEnabled &&
               sdcard_readLogFile(_ubxPVTBuf, sizeof(_ubxPVTBuf))) {
              emulator.setPVTPacket(_ubxPVTBuf, sizeof(_ubxPVTBuf));
            } else {
              emulator.setPVTLoopPacket();
            }
            _ubxNAVPVTInfo = emulator.getPVTPacketInfo();
            if(_ubxNAVPVTInfo.dateValid && _ubxNAVPVTInfo.timeValid) {
              setRTCTime(_ubxNAVPVTInfo.hour, _ubxNAVPVTInfo.minute, _ubxNAVPVTInfo.second,
                         _ubxNAVPVTInfo.day, _ubxNAVPVTInfo.month, _ubxNAVPVTInfo.year);
              clockTime_valid = true;
              _prevClockTime = (uint32_t)(_ubxNAVPVTInfo.hour*3600) +
                               (uint32_t)(_ubxNAVPVTInfo.minute*60) +
                               _ubxNAVPVTInfo.second;
            }
            emulator.sendPVTPacket();
            //statusLED.pulse(_ubxNAVPVTInfo.locationValid ? 2 : 1);
          }
        } else {
          if(emulatorSDCardEnabled &&
             sdcard_readLogFile(_ubxPVTBuf, sizeof(_ubxPVTBuf))) {
            emulator.setPVTPacket(_ubxPVTBuf, sizeof(_ubxPVTBuf));
          } else {
            emulator.setPVTLoopPacket();
          }
          _ubxNAVPVTInfo = emulator.getPVTPacketInfo();
          emulator.setPVTPacketDateTime(_rtcTime.year, _rtcTime.month, _rtcTime.day,
                                        _rtcTime.hour, _rtcTime.minute, _rtcTime.second);
          emulator.sendPVTPacket();
          //statusLED.pulse(_ubxNAVPVTInfo.locationValid ? 2 : 1);
        }
        displayRefresh = true;
      }
      break;
  }

  ////CHECKBUTTONACTIVITY////
  buttons_click();
  ////CHECKBUTTONACTIVITY////

  if(displayEnabled) {
    menu_idle_timer();
    display_refresh(); // refresh sprite buffer
    display_update();  // push sprite to display
  }

#ifdef DEBUG_LOOP_TIMING
  if(_nowMS-max_loop_check > DEBUG_LOOP_PERIOD) {
    max_loop_check = _nowMS;
    max_loop_time = 0;
  }
  if(millis()-_nowMS > max_loop_time) {
    max_loop_time = max(max_loop_time, millis()-_nowMS);
    char _tempStr[22];
    sprintf(_tempStr, "maxloop=%d", max_loop_time);
    msg_update(_tempStr);
  }
#endif

}

