
/********************************************************************/
// Display
bool displayEnabled;
#include <Wire.h>
#include <TeenyPrtVal.h>
#include <TeenyMenu.h>
//M5Display display = M5.lcd;
//TeenyPrtVal<M5Display> displayPV(display);
//TeenyMenu<M5Display> menu(display, 12, 16, false, 16, 16, 6, 10, 11, 154, 6, 0, 20);
TFT_eSprite display = TFT_eSprite(&M5.Lcd);
TeenyPrtVal<TFT_eSprite> displayPV(display);
TeenyMenu<TFT_eSprite> menu(display, 12, 16, false, 16, 16, 6, 10, 11, 154, 6, 0, 20);

/********************************************************************/
// Display/Message Defines
#define DISPLAY_REFRESH_PERIOD  200
#define MSG_DISPLAY_TIME        5000
int16_t  MENU_MODE = DM_IDLE;
bool displayRefresh;
bool displayUpdate;

/********************************************************************/
// forward declarations
bool msg_update(const char* msgStr);
void deviceMode_init();
void deviceMode_end();

/********************************************************************/
// Menus
//
// menu mode item
void menu_menuModeCB(); // forward declaration
SelectOptionUint8t selectMenuModeOptions[] = {
  {"IDLE",   DM_IDLE},
  {"GPSRCV", DM_GPSRCVR},
  {"GPSLOG", DM_GPSLOGR},
  {"GPSSAT", DM_GPSNSAT},
//{"GPSCPT", DM_GPSCAPT},
//{"GPSSST", DM_GPSSSTP},
  {"GPSEMU", DM_GPSEMUL}};
TeenyMenuSelect selectMenuMode(sizeof(selectMenuModeOptions)/sizeof(SelectOptionUint8t), selectMenuModeOptions);
TeenyMenuItem menuItemMenuMode("Device Mode", MENU_MODE, selectMenuMode, menu_menuModeCB);
//
// gps receiver unit
void menu_entrGPSRcvrCB(); // forward declaration
void menu_exitGPSRcvrCB(); // forward declaration
TeenyMenuPage menuPageGPSRcvr("GPS RECEIVER MODE", menu_entrGPSRcvrCB, menu_exitGPSRcvrCB);
TeenyMenuItem menuItemGPSRcvr("*START GPS RCVR*", menuPageGPSRcvr);
TeenyMenuItem menuItemGPSRcvrExit(false); // optional return menu item
TeenyMenuItem menuItemGPSRcvrLabel0("");
TeenyMenuItem menuItemGPSRcvrLabel1("");
//
// gps logging unit
void menu_entrGPSLogrCB(); // forward declaration
void menu_exitGPSLogrCB(); // forward declaration
TeenyMenuPage menuPageGPSLogr("GPS LOGGING MODE", menu_entrGPSLogrCB, menu_exitGPSLogrCB);
TeenyMenuItem menuItemGPSLogr("*START GPS LOGGER*", menuPageGPSLogr);
TeenyMenuItem menuItemGPSLogrExit(false); // optional return menu item
TeenyMenuItem menuItemGPSLogrLabel0("");
TeenyMenuItem menuItemGPSLogrLabel1("");
TeenyMenuItem menuItemGPSLogrLabel2("");
TeenyMenuItem menuItemGPSLogrLabel3("");
TeenyMenuItem menuItemGPSLogrLabel4("");
void menu_startGPSLogrCB(); // forward declaration
TeenyMenuItem menuItemGPSLogrStrtLog("Start PVT Logging", menu_startGPSLogrCB);
void menu_stopGPSLogrCB(); // forward declaration
TeenyMenuItem menuItemGPSLogrStopLog("Stop PVT Logging", menu_stopGPSLogrCB);
//
// gps navsat unit
void menu_entrGPSNsatCB(); // forward declaration
void menu_exitGPSNsatCB(); // forward declaration
TeenyMenuPage menuPageGPSNsat("GPS NAVSAT MODE", menu_entrGPSNsatCB, menu_exitGPSNsatCB);
TeenyMenuItem menuItemGPSNsat("*START GPS NAVSAT*", menuPageGPSNsat);
TeenyMenuItem menuItemGPSNsatExit(false); // optional return menu item
TeenyMenuItem menuItemGPSNsatLabel0("");
//
// gps transceiver unit
void menu_entrGPSCaptCB(); // forward declaration
void menu_exitGPSCaptCB(); // forward declaration
TeenyMenuPage menuPageGPSCapt("GPS CAPTURE MODE", menu_entrGPSCaptCB, menu_exitGPSCaptCB);
TeenyMenuItem menuItemGPSCapt("*START GPS CAPTURE*", menuPageGPSCapt);
TeenyMenuItem menuItemGPSCaptExit(false); // optional return menu item
TeenyMenuItem menuItemGPSCaptLabel0("");
bool menu_captRxPktInProgress = false;
void menu_captStrtRxPktCB(); // forward declaration
TeenyMenuItem menuItemGPSCaptStrtRxPkt("Capture RX PKT", menu_captStrtRxPktCB);
void menu_captStopRxPktCB(); // forward declaration
TeenyMenuItem menuItemGPSCaptStopRxPkt("Save RX PKT", menu_captStopRxPktCB);
//
// gps single step unit
void menu_entrGPSStepCB(); // forward declaration
void menu_exitGPSStepCB(); // forward declaration
TeenyMenuPage menuPageGPSStep("GPS SINGLESTEP MODE", menu_entrGPSStepCB, menu_exitGPSStepCB);
TeenyMenuItem menuItemGPSStep("*START GPS STEPPER*", menuPageGPSStep);
TeenyMenuItem menuItemGPSStepExit(false); // optional return menu item
void menu_sstBeginCB(); // forward declaration
TeenyMenuItem menuItemGPSStepBegin("gnss.begin", menu_sstBeginCB);
void menu_sstSetBaudCB(); // forward declaration
TeenyMenuItem menuItemGPSStepSetBaud("gnss.setSerialRate", menu_sstSetBaudCB);
void menu_sstSaveConfigCB(); // forward declaration
TeenyMenuItem menuItemGPSStepSaveCfg("gnss.saveConfig", menu_sstSaveConfigCB);
void menu_sstGetVersionCB(); // forward declaration
TeenyMenuItem menuItemGPSStepGetVersion("gnss.getVersion", menu_sstGetVersionCB);
void menu_sstSetPortOutputCB(); // forward declaration
TeenyMenuItem menuItemGPSStepSetPortOutput("gnss.setPortOutput", menu_sstSetPortOutputCB);
void menu_sstSetMeasRateCB(); // forward declaration
TeenyMenuItem menuItemGPSStepMeasRate("gnss.setMeasRate", menu_sstSetMeasRateCB);
void menu_sstSetNavRateCB(); // forward declaration
TeenyMenuItem menuItemGPSStepSetNavRate("gnss.setNavRate", menu_sstSetNavRateCB);
void menu_sstTogAutoPVTCB(); // forward declaration
TeenyMenuItem menuItemGPSStepTogAutoPVT("gnss.togAutoPVT", menu_sstTogAutoPVTCB);
void menu_sstReqCfgPrtCB(); // forward declaration
TeenyMenuItem menuItemGPSStepReqCfgPrt("reqCfgPrt_CMD", menu_sstReqCfgPrtCB);
void menu_sstReqCfgNavx5CB(); // forward declaration
TeenyMenuItem menuItemGPSStepReqCfgNavx5("reqCfgNavx5_CMD", menu_sstReqCfgNavx5CB);
void menu_sstColdStartCB(); // forward declaration
TeenyMenuItem menuItemGPSStepcoldStart("coldStart_CMD", menu_sstColdStartCB);
void menu_sstReqMonVerCB(); // forward declaration
TeenyMenuItem menuItemGPSStepReqMonVer("reqMonVer_CMD", menu_sstReqMonVerCB);
void menu_sstReqMonSmgrCB(); // forward declaration
TeenyMenuItem menuItemGPSStepReqMonSmgr("reqMonSmgr_CMD", menu_sstReqMonSmgrCB);
void menu_sstSetCfgPrtCB(); // forward declaration
TeenyMenuItem menuItemGPSStepSetCfgPrt("setCfgPrt_CMD", menu_sstSetCfgPrtCB);
void menu_sstReqCfgRateCB(); // forward declaration
TeenyMenuItem menuItemGPSStepReqCfgRate("reqCfgRate_CMD", menu_sstReqCfgRateCB);
void menu_sstSetCfgRateCB(); // forward declaration
TeenyMenuItem menuItemGPSStepSetCfgRate("setCfgRate_CMD", menu_sstSetCfgRateCB);
void menu_sstSetCfgCfgCB(); // forward declaration
TeenyMenuItem menuItemGPSStepSetCfgCfg("setCfgCfg_CMD", menu_sstSetCfgCfgCB);
void menu_sstSetCfgMsgCB(); // forward declaration
TeenyMenuItem menuItemGPSStepSetCfgMsg("setCfgMsg_CMD", menu_sstSetCfgMsgCB);
void menu_sstReqNavPvtCB(); // forward declaration
TeenyMenuItem menuItemGPSStepReqNavPvt("reqNavPvt_CMD", menu_sstReqNavPvtCB);
//
// gps emulator unit
void menu_entrGPSEmulCB(); // forward declaration
void menu_exitGPSEmulCB(); // forward declaration
TeenyMenuPage menuPageGPSEmul("GPS EMULATOR MODE", menu_entrGPSEmulCB, menu_exitGPSEmulCB);
TeenyMenuItem menuItemGPSEmul("*START EMULATOR*", menuPageGPSEmul);
TeenyMenuItem menuItemGPSEmulExit(false); // optional return menu item
TeenyMenuItem menuItemGPSEmulLabel0("");
TeenyMenuItem menuItemGPSEmulLabel1("");
//
// TEENYGPSEMULATOR SETTINGS
//
// settings menu
TeenyMenuPage menuPageTopLevelSettings("SETTINGS MENU");
TeenyMenuItem menuItemTopLevelSettings("Settings", menuPageTopLevelSettings);
TeenyMenuItem menuItemTopLevelSettingsExit(false); // optional return menu item
//
// RTC SETTINGS
//
// rtc settings menu
void menu_getRTC_CB(); // forward declaration
void menu_setRTC_CB(); // forward declaration
TeenyMenuPage menuPageRTCSettings("RT CLOCK SETTINGS", menu_getRTC_CB);
TeenyMenuItem menuItemRTCSettings("RT Clock Settings", menuPageRTCSettings);
TeenyMenuItem menuItemRTCSettingsExit(false); // optional return menu item
//
// rtc year
int16_t menuRTCYear = 2023;
int16_t menuRTCYearMin = 2023;
int16_t menuRTCYearMax = 2042;
TeenyMenuItem menuItemRTCYear("RTC Year", menuRTCYear, menuRTCYearMin, menuRTCYearMax, menu_setRTC_CB);
//
// rtc month
int16_t menuRTCMonth = 1;
int16_t menuRTCMonthMin = 1;
int16_t menuRTCMonthMax = 12;
TeenyMenuItem menuItemRTCMonth("RTC Month", menuRTCMonth, menuRTCMonthMin, menuRTCMonthMax, menu_setRTC_CB);
//
// rtc day
int16_t menuRTCDay = 1;
int16_t menuRTCDayMin = 1;
int16_t menuRTCDayMax = 31;
TeenyMenuItem menuItemRTCDay("RTC Day", menuRTCDay, menuRTCDayMin, menuRTCDayMax, menu_setRTC_CB);
//
// rtc hour
int16_t menuRTCHour = 0;
int16_t menuRTCHourMin = 0;
int16_t menuRTCHourMax = 23;
TeenyMenuItem menuItemRTCHour("RTC Hour", menuRTCHour, menuRTCHourMin, menuRTCHourMax, menu_setRTC_CB);
//
// rtc minute
int16_t menuRTCMinute = 0;
int16_t menuRTCMinuteMin = 0;
int16_t menuRTCMinuteMax = 59;
TeenyMenuItem menuItemRTCMinute("RTC Minute", menuRTCMinute, menuRTCMinuteMin, menuRTCMinuteMax, menu_setRTC_CB);
//
// rtc second
int16_t menuRTCSecond = 0;
int16_t menuRTCSecondMin = 0;
int16_t menuRTCSecondMax = 59;
TeenyMenuItem menuItemRTCSecond("RTC Second", menuRTCSecond, menuRTCSecondMin, menuRTCSecondMax, menu_setRTC_CB);
//
// EMULATOR SETTINGS
//
// emulator settings menu
TeenyMenuPage menuPageEMULSettings("EMULATOR SETTINGS", menu_getRTC_CB);
TeenyMenuItem menuItemEMULSettings("Emulator Settings", menuPageEMULSettings);
TeenyMenuItem menuItemEMULSettingsExit(false); // optional return menu item
//
// display timeout
int16_t menuColdStartPVTPktsMin = 0;
int16_t menuColdStartPVTPktsMax = 60;
TeenyMenuItem menuItemColdStartPVTPkts("ColdStrtPkts", deviceState.EMUL_NUMCOLDSTARTPVTPACKETS, menuColdStartPVTPktsMin, menuColdStartPVTPktsMax);
//
// display timeout
int16_t menuDisplayTimeoutMin = 1;
int16_t menuDisplayTimeoutMax = 20;
TeenyMenuItem menuItemDisplayTimeout("Display Time", deviceState.DISPLAYTIMEOUT, menuDisplayTimeoutMin, menuDisplayTimeoutMax);
//
// status led
TeenyMenuItem menuItemStatusLED("Status LED", deviceState.STATUSLED, nullptr, "OFF", "ON");
//
// SAVE SETTINGS
//
// save settings menu
void menu_cancelSaveSettingsCB(); // forward declaration
TeenyMenuPage menuPageSaveSettings("SAVE DEVICE SETTINGS", nullptr, menu_cancelSaveSettingsCB);
TeenyMenuItem menuItemSaveSettings("Save Dev Settings", menuPageSaveSettings);
TeenyMenuItem menuItemSaveSettingsExit(false); // optional return menu item
//
// save state to flash
void menu_confirmSaveSettingsCB(); // forward declaration
TeenyMenuItem menuItemConfirmSaveSettings("Confirm Save?", menu_confirmSaveSettingsCB);
//
// RESTART DEVICE
//
// restart device menu
void menu_cancelRestartDeviceCB(); // forward declaration
TeenyMenuPage menuPageRestartDevice("RESTART DEVICE", nullptr, menu_cancelRestartDeviceCB);
TeenyMenuItem menuItemRestartDevice("Restart Device", menuPageRestartDevice);
TeenyMenuItem menuItemRestartDeviceExit(false); // optional return menu item
//
// confirm restart
void menu_confirmRestartDeviceCB(); // forward declaration
TeenyMenuItem menuItemConfirmRestartDevice("Confirm Restart?", menu_confirmRestartDeviceCB);
//
// RESET DEVICE
//
// reset device menu
void menu_cancelResetDeviceCB(); // forward declaration
TeenyMenuPage menuPageResetDevice("RESET DEVICE", nullptr, menu_cancelResetDeviceCB);
TeenyMenuItem menuItemResetDevice("Reset Device", menuPageResetDevice);
TeenyMenuItem menuItemResetDeviceExit(false); // optional return menu item
//
// confirm reset
void menu_confirmResetDeviceCB(); // forward declaration
TeenyMenuItem menuItemConfirmResetDevice("Confirm Reset?", menu_confirmResetDeviceCB);
//
// MAIN MENU
//
//TeenyMenuPage menuPageMain("TEENY GPS TESTBED", nullptr, menu_exit);
TeenyMenuPage menuPageMain("TEENY GPS TESTBED");
TeenyMenuItem menuItemLabel0("");
TeenyMenuItem menuItemLabel1("");
TeenyMenuItem menuItemLabel2("");
TeenyMenuItem menuItemLabel3("");
//
// menu variables
uint32_t menuInputTime;
bool menuDisplaySleepMode;

/********************************************************************/
void menu_setup() {
  // add menu items
  menuPageMain.addMenuItem(menuItemLabel0);
  menuPageMain.addMenuItem(menuItemGPSRcvr);
  menuPageGPSRcvr.addMenuItem(menuItemGPSRcvrLabel0);
  menuPageGPSRcvr.addMenuItem(menuItemGPSRcvrLabel1);
  //menuPageBaseUnit.addMenuItem(menuItemGPSRcvrExit);
  menuPageMain.addMenuItem(menuItemGPSLogr);
  menuPageGPSLogr.addMenuItem(menuItemGPSLogrLabel0);
  menuPageGPSLogr.addMenuItem(menuItemGPSLogrLabel1);
  menuPageGPSLogr.addMenuItem(menuItemGPSLogrLabel2);
  menuPageGPSLogr.addMenuItem(menuItemGPSLogrLabel3);
  menuPageGPSLogr.addMenuItem(menuItemGPSLogrLabel4);
  menuPageGPSLogr.addMenuItem(menuItemGPSLogrStrtLog);
  menuPageGPSLogr.addMenuItem(menuItemGPSLogrStopLog);
  //menuPageGPSLogr.addMenuItem(menuItemGPSLogrExit);
  menuPageMain.addMenuItem(menuItemGPSNsat);
  menuPageGPSNsat.addMenuItem(menuItemGPSNsatLabel0);
  //menuPageGPSNsat.addMenuItem(menuItemGPSNsatExit);
  menuPageMain.addMenuItem(menuItemGPSCapt);
  menuPageGPSCapt.addMenuItem(menuItemGPSCaptLabel0);
  menuPageGPSCapt.addMenuItem(menuItemGPSCaptStrtRxPkt);
  menuPageGPSCapt.addMenuItem(menuItemGPSCaptStopRxPkt);
  //menuPageGPSCapt.addMenuItem(menuItemGPSCaptExit);
  menuPageMain.addMenuItem(menuItemGPSStep);
  menuPageGPSStep.addMenuItem(menuItemGPSStepBegin);
  menuPageGPSStep.addMenuItem(menuItemGPSStepSetBaud);
  menuPageGPSStep.addMenuItem(menuItemGPSStepSaveCfg);
  menuPageGPSStep.addMenuItem(menuItemGPSStepGetVersion);
  menuPageGPSStep.addMenuItem(menuItemGPSStepSetPortOutput);
  menuPageGPSStep.addMenuItem(menuItemGPSStepMeasRate);
  menuPageGPSStep.addMenuItem(menuItemGPSStepSetNavRate);
  menuPageGPSStep.addMenuItem(menuItemGPSStepTogAutoPVT);
  menuPageGPSStep.addMenuItem(menuItemGPSStepReqCfgPrt);
  menuPageGPSStep.addMenuItem(menuItemGPSStepReqCfgNavx5);
  menuPageGPSStep.addMenuItem(menuItemGPSStepcoldStart);
  menuPageGPSStep.addMenuItem(menuItemGPSStepReqMonVer);
  menuPageGPSStep.addMenuItem(menuItemGPSStepReqMonSmgr);
  menuPageGPSStep.addMenuItem(menuItemGPSStepSetCfgPrt);
  menuPageGPSStep.addMenuItem(menuItemGPSStepReqCfgRate);
  menuPageGPSStep.addMenuItem(menuItemGPSStepSetCfgRate);
  menuPageGPSStep.addMenuItem(menuItemGPSStepSetCfgCfg);
  menuPageGPSStep.addMenuItem(menuItemGPSStepSetCfgMsg);
  menuPageGPSStep.addMenuItem(menuItemGPSStepReqNavPvt);
  //menuPageGPSStep.addMenuItem(menuItemGPSStepExit);
  menuPageMain.addMenuItem(menuItemGPSEmul);
  menuPageGPSEmul.addMenuItem(menuItemGPSEmulLabel0);
  menuPageGPSEmul.addMenuItem(menuItemGPSEmulLabel1);
  //menuPageGPSEmul.addMenuItem(menuItemGPSEmulExit);
  menuPageMain.addMenuItem(menuItemLabel1);
  menuPageMain.addMenuItem(menuItemMenuMode);
  menuPageMain.addMenuItem(menuItemLabel2);
  menuPageMain.addMenuItem(menuItemLabel3);
  menuPageMain.addMenuItem(menuItemTopLevelSettings);
  menuPageTopLevelSettings.addMenuItem(menuItemRTCSettings);
  menuPageRTCSettings.addMenuItem(menuItemRTCYear);
  menuPageRTCSettings.addMenuItem(menuItemRTCMonth);
  menuPageRTCSettings.addMenuItem(menuItemRTCDay);
  menuPageRTCSettings.addMenuItem(menuItemRTCHour);
  menuPageRTCSettings.addMenuItem(menuItemRTCMinute);
  menuPageRTCSettings.addMenuItem(menuItemRTCSecond);
  menuPageRTCSettings.addMenuItem(menuItemRTCSettingsExit); // optional return menu item
  menuPageTopLevelSettings.addMenuItem(menuItemEMULSettings);
  menuPageEMULSettings.addMenuItem(menuItemColdStartPVTPkts);
  menuPageEMULSettings.addMenuItem(menuItemEMULSettingsExit); // optional return menu item
  menuPageTopLevelSettings.addMenuItem(menuItemDisplayTimeout);
  menuPageTopLevelSettings.addMenuItem(menuItemStatusLED);
  menuPageTopLevelSettings.addMenuItem(menuItemSaveSettings);
  menuPageSaveSettings.addMenuItem(menuItemConfirmSaveSettings);
  menuPageSaveSettings.addMenuItem(menuItemSaveSettingsExit);
  menuPageTopLevelSettings.addMenuItem(menuItemRestartDevice);
  menuPageRestartDevice.addMenuItem(menuItemConfirmRestartDevice);
  menuPageRestartDevice.addMenuItem(menuItemRestartDeviceExit);
  menuPageTopLevelSettings.addMenuItem(menuItemResetDevice);
  menuPageResetDevice.addMenuItem(menuItemConfirmResetDevice);
  menuPageResetDevice.addMenuItem(menuItemResetDeviceExit);
  menuPageTopLevelSettings.addMenuItem(menuItemTopLevelSettingsExit);
  // assign the starting menu page
  menu.setMenuPageCurrent(menuPageMain);
  switch(deviceState.DEVICE_MODE) {
    case DM_GPSRCVR:
      menu.linkMenuPage(menuPageGPSRcvr);
      break;
    case DM_GPSLOGR:
      menu.linkMenuPage(menuPageGPSLogr);
      break;
    case DM_GPSNSAT:
      menu.linkMenuPage(menuPageGPSLogr);
      break;
    case DM_GPSCAPT:
      menu.linkMenuPage(menuPageGPSCapt);
      break;
    case DM_GPSSSTP:
      menu.linkMenuPage(menuPageGPSStep);
      break;
    case DM_GPSEMUL:
      menu.linkMenuPage(menuPageGPSEmul);
      break;
  }
  // init and enter menu
  menu.setTextColor(WHITE, BLACK);
  menuDisplaySleepMode = false;
}

/********************************************************************/
void menu_init() {
  menu_menuModeCB();
}

/********************************************************************/
void menu_enter() {
  menu.resetMenu();
  displayRefresh = true;
}

/********************************************************************/
void menu_exit() {
  // This is an optional exitAction for menuPageMain (top level menu page)
  // You can execute something here to exit to another/master GUI
}

/********************************************************************/
void menu_idle_timer() {
  if((millis()-menuInputTime) > (uint32_t)(deviceState.DISPLAYTIMEOUT*MILLIS_PER_MIN)) {
    if(!menuDisplaySleepMode) {
      menuDisplaySleepMode = true;
      //msg_update("Enter Sleep Mode");
      M5.lcd.sleep();
    }
  } else if(menuDisplaySleepMode) {
    menuDisplaySleepMode = false;
    M5.lcd.wakeup();
    msg_update("Exit Sleep Mode");
  }
}

/********************************************************************/
void menu_menuModeCB() {
  // choose submenu item based on MENU_MODE
  switch(MENU_MODE) {
    case DM_IDLE:
      menuItemGPSRcvr.hide();
      menuItemGPSLogr.hide();
      menuItemGPSNsat.hide();
      menuItemGPSStep.hide();
      menuItemGPSCapt.hide();
      menuItemGPSEmul.hide();
      menuItemLabel2.show();
      break;
    case DM_GPSRCVR:
      menuItemGPSRcvr.show();
      menuItemGPSLogr.hide();
      menuItemGPSNsat.hide();
      menuItemGPSStep.hide();
      menuItemGPSCapt.hide();
      menuItemGPSEmul.hide();
      menuItemLabel2.hide();
      break;
    case DM_GPSLOGR:
      menuItemGPSRcvr.hide();
      menuItemGPSLogr.show();
      menuItemGPSNsat.hide();
      menuItemGPSStep.hide();
      menuItemGPSCapt.hide();
      menuItemGPSEmul.hide();
      menuItemLabel2.hide();
      menuItemGPSLogrStrtLog.hide(ubxLoggingInProgress ? true : false);
      menuItemGPSLogrStopLog.hide(ubxLoggingInProgress ? false : true);
      break;
    case DM_GPSNSAT:
      menuItemGPSRcvr.hide();
      menuItemGPSLogr.hide();
      menuItemGPSNsat.show();
      menuItemGPSStep.hide();
      menuItemGPSCapt.hide();
      menuItemGPSEmul.hide();
      menuItemLabel2.hide();
      break;
    case DM_GPSCAPT:
      menuItemGPSRcvr.hide();
      menuItemGPSLogr.hide();
      menuItemGPSNsat.hide();
      menuItemGPSStep.hide();
      menuItemGPSCapt.show();
      menuItemGPSEmul.hide();
      menuItemLabel2.hide();
      menuItemGPSCaptStrtRxPkt.hide(menu_captRxPktInProgress ? true : false);
      menuItemGPSCaptStopRxPkt.hide(menu_captRxPktInProgress ? false : true);
      break;
    case DM_GPSSSTP:
      menuItemGPSRcvr.hide();
      menuItemGPSLogr.hide();
      menuItemGPSNsat.hide();
      menuItemGPSStep.show();
      menuItemGPSCapt.hide();
      menuItemGPSEmul.hide();
      menuItemLabel2.hide();
      break;
    case DM_GPSEMUL:
      menuItemGPSRcvr.hide();
      menuItemGPSLogr.hide();
      menuItemGPSNsat.hide();
      menuItemGPSStep.hide();
      menuItemGPSCapt.hide();
      menuItemGPSEmul.show();
      menuItemLabel2.hide();
      break;
  }
  if(MENU_MODE==DM_IDLE) {
    menuItemMenuMode.setTitle("Device Mode");
  } else {
    menuItemMenuMode.setTitle("Change Mode");
    menu.resetMenu();
  }
}

/********************************************************************/
void menu_entrGPSRcvrCB() {
  deviceState.DEVICE_MODE = DM_GPSRCVR;
  deviceMode_init();
  displayRefresh = true;
}

/********************************************************************/
void menu_exitGPSRcvrCB() {
  deviceMode_end();
  menu.exitToParentMenuPage();
  displayRefresh = true;
}

/********************************************************************/
void menu_entrGPSLogrCB() {
  deviceState.DEVICE_MODE = DM_GPSLOGR;
  deviceMode_init();
  displayRefresh = true;
}

/********************************************************************/
void menu_startGPSLogrCB() {
  if(ubxLoggingInProgress) return;
  if(sdcard_openLoggingFile()) {
    ubxLoggingInProgress = true;
    menuItemGPSLogrStrtLog.hide();
    menuItemGPSLogrStopLog.show();
    msg_update("PVT Logging Started");
  } else {
    msg_update("SD Card Error");
  }
  displayRefresh = true;
}

/********************************************************************/
void menu_stopGPSLogrCB() {
  char _msgStr[22];
  if(!ubxLoggingInProgress) return;
  sdcard_closeLoggingFile();
  ubxLoggingInProgress = false;
  menuItemGPSLogrStrtLog.show();
  menuItemGPSLogrStopLog.hide();
  sprintf(_msgStr, "F%02d TP=%04d VP=%04d",
          ubxLoggingFileNum,
          min(ubxLoggingFileWriteCount, 9999),
          min(ubxLoggingFileWriteValidCount, 9999));
  msg_update(_msgStr);
  displayRefresh = true;
}

/********************************************************************/
void menu_exitGPSLogrCB() {
  menu_stopGPSLogrCB();
  deviceMode_end();
  menu.exitToParentMenuPage();
  displayRefresh = true;
}

/********************************************************************/
void menu_entrGPSNsatCB() {
  deviceState.DEVICE_MODE = DM_GPSNSAT;
  deviceMode_init();
  displayRefresh = true;
}

/********************************************************************/
void menu_exitGPSNsatCB() {
  deviceMode_end();
  menu.exitToParentMenuPage();
  displayRefresh = true;
}

/********************************************************************/
void menu_entrGPSCaptCB() {
  deviceState.DEVICE_MODE = DM_GPSCAPT;
  deviceMode_init();
  displayRefresh = true;
}

/********************************************************************/
void menu_captStrtRxPktCB() {
  if(menu_captRxPktInProgress) return;
  if(sdcard_openRxPktFile()) {
    menu_captRxPktInProgress = true;
    menuItemGPSCaptStrtRxPkt.hide();
    menuItemGPSCaptStopRxPkt.show();
    msg_update("Ready for RX PKT");
  } else {
    msg_update("SD Card Error");
  }
  displayRefresh = true;
}

/********************************************************************/
void menu_captStopRxPktCB() {
  char _msgStr[22];
  if(!menu_captRxPktInProgress) return;
  sdcard_closeRxPktFile();
  menu_captRxPktInProgress = false;
  menuItemGPSCaptStrtRxPkt.show();
  menuItemGPSCaptStopRxPkt.hide();
  sprintf(_msgStr, "%s l=%d",
          rxPktFileName,
          min(rxPktWriteCount, 999));
  msg_update(_msgStr);
  displayRefresh = true;
}

/********************************************************************/
void menu_exitGPSCaptCB() {
  menu_captStopRxPktCB();
  deviceMode_end();
  menu.exitToParentMenuPage();
  displayRefresh = true;
}

/********************************************************************/
void menu_entrGPSStepCB() {
  deviceState.DEVICE_MODE = DM_GPSSSTP;
  deviceMode_init();
  displayRefresh = true;
}

/********************************************************************/
void menu_exitGPSStepCB() {
  deviceMode_end();
  menu.exitToParentMenuPage();
  displayRefresh = true;
}

/********************************************************************/
void menu_sstBeginCB() {
  char _msgStr[22];
  gpsSerial->begin(GPS_BAUD_RATE);
  bool rcode = gps.gnss_ss_begin(*gpsSerial);
  sprintf(_msgStr, "gnss.begin rc=%d", rcode);
  msg_update(_msgStr);
  displayRefresh = true;
}

/********************************************************************/
void menu_sstSetBaudCB() {
  char _msgStr[22];
  gps.gnss_ss_setSerialRate(GPS_BAUD_RATE);
  sprintf(_msgStr, "gnss.setSerRate");
  msg_update(_msgStr);
  displayRefresh = true;
}

/********************************************************************/
void menu_sstSaveConfigCB() {
  char _msgStr[22];
  while(gpsSerial->available()) gpsSerial->read();
  bool rcode = gps.gnss_ss_saveConfiguration();
  sprintf(_msgStr, "gnss.saveCfg rc=%d", rcode);
  msg_update(_msgStr);
  displayRefresh = true;
}

/********************************************************************/
void menu_sstGetVersionCB() {
  char _msgStr[22];
  while(gpsSerial->available()) gpsSerial->read();
  bool rcode = gps.gnss_ss_getProtocolVersion();
  sprintf(_msgStr, "gnss.getPV rc=%d", rcode);
  msg_update(_msgStr);
  displayRefresh = true;
}

/********************************************************************/
void menu_sstSetPortOutputCB() {
  char _msgStr[22];
  while(gpsSerial->available()) gpsSerial->read();
  bool rcode = gps.gnss_ss_setPortOutput(COM_PORT_UART1, COM_TYPE_UBX);
  sprintf(_msgStr, "gnss.setPtOut rc=%d", rcode);
  msg_update(_msgStr);
  displayRefresh = true;
}

/********************************************************************/
void menu_sstSetMeasRateCB() {
  char _msgStr[22];
  while(gpsSerial->available()) gpsSerial->read();
  bool rcode = gps.gnss_ss_setMeasurementRate(1000);
  sprintf(_msgStr, "gnss.setMR rc=%d", rcode);
  msg_update(_msgStr);
  displayRefresh = true;
}

/********************************************************************/
void menu_sstSetNavRateCB() {
  char _msgStr[22];
  while(gpsSerial->available()) gpsSerial->read();
  bool rcode = gps.gnss_ss_setNavigationRate(1);
  sprintf(_msgStr, "gnss.setNR rc=%d", rcode);
  msg_update(_msgStr);
  displayRefresh = true;
}

/********************************************************************/
void menu_sstTogAutoPVTCB() {
  char _msgStr[22];
  static bool enable = true;
  while(gpsSerial->available()) gpsSerial->read();
  bool rcode = gps.gnss_ss_setAutoNAVPVT(enable);
  sprintf(_msgStr, "gnss.setANPVT rc=%d", rcode);
  msg_update(_msgStr);
  displayRefresh = true;
  enable = !enable;
}

/********************************************************************/
void menu_sstReqCfgPrtCB() {
  char _msgStr[22];
  uint8_t host_reqCFGPRT[9]  = {0xb5,0x62,0x06,0x00,0x01,0x00,0x01,0x08,0x22};
  while(gpsSerial->available()) gpsSerial->read();
  if(!sdcard_openRxPktFile()) return;
  uint32_t _nowMS = millis();
  gpsSerial->write(host_reqCFGPRT, sizeof(host_reqCFGPRT));
  while((millis()-_nowMS)<500) {
    sdcard_writeRxPktFile();
  }
  sdcard_closeRxPktFile();
  sprintf(_msgStr, "%s l=%d",
          rxPktFileName,
          min(rxPktWriteCount, 999));
  msg_update(_msgStr);
  displayRefresh = true;
}

/********************************************************************/
void menu_sstReqCfgNavx5CB() {
  char _msgStr[22];
  uint8_t host_reqCFGNAVX5[8]  = {0xb5,0x62,0x06,0x23,0x00,0x00,0x29,0x81};
  while(gpsSerial->available()) gpsSerial->read();
  if(!sdcard_openRxPktFile()) return;
  uint32_t _nowMS = millis();
  gpsSerial->write(host_reqCFGNAVX5, sizeof(host_reqCFGNAVX5));
  while((millis()-_nowMS)<500) {
    sdcard_writeRxPktFile();
  }
  sdcard_closeRxPktFile();
  sprintf(_msgStr, "%s l=%d",
          rxPktFileName,
          min(rxPktWriteCount, 999));
  msg_update(_msgStr);
  displayRefresh = true;
}

/********************************************************************/
void menu_sstColdStartCB() {
  char _msgStr[22];
  const uint8_t host_coldStart[12] = {0xB5,0x62,0x06,0x04,0x04,0x00,0xFF,0xB9,0x02,0x00,0xC8,0x8F};
  while(gpsSerial->available()) gpsSerial->read();
  if(!sdcard_openRxPktFile()) return;
  uint32_t _nowMS = millis();
  gpsSerial->write(host_coldStart, sizeof(host_coldStart));
  while((millis()-_nowMS)<500) {
    sdcard_writeRxPktFile();
  }
  sdcard_closeRxPktFile();
  sprintf(_msgStr, "%s l=%d",
          rxPktFileName,
          min(rxPktWriteCount, 999));
  msg_update(_msgStr);
  displayRefresh = true;
}

/********************************************************************/
void menu_sstReqMonVerCB() {
  char _msgStr[22];
  uint8_t host_reqMONVER[8]  = {0xb5,0x62,0x0a,0x04,0x00,0x00,0x0e,0x34};
  while(gpsSerial->available()) gpsSerial->read();
  if(!sdcard_openRxPktFile()) return;
  uint32_t _nowMS = millis();
  gpsSerial->write(host_reqMONVER, sizeof(host_reqMONVER));
  while((millis()-_nowMS)<500) {
    sdcard_writeRxPktFile();
  }
  sdcard_closeRxPktFile();
  sprintf(_msgStr, "%s l=%d",
          rxPktFileName,
          min(rxPktWriteCount, 999));
  msg_update(_msgStr);
  displayRefresh = true;
}

/********************************************************************/
void menu_sstReqMonSmgrCB() {
  char _msgStr[22];
  uint8_t host_reqMONSMGR[8]  = {0xb5,0x62,0x0a,0x2e,0x00,0x00,0x38,0xb2};
  while(gpsSerial->available()) gpsSerial->read();
  if(!sdcard_openRxPktFile()) return;
  uint32_t _nowMS = millis();
  gpsSerial->write(host_reqMONSMGR, sizeof(host_reqMONSMGR));
  while((millis()-_nowMS)<500) {
    sdcard_writeRxPktFile();
  }
  sdcard_closeRxPktFile();
  sprintf(_msgStr, "%s l=%d",
          rxPktFileName,
          min(rxPktWriteCount, 999));
  msg_update(_msgStr);
  displayRefresh = true;
}

/********************************************************************/
void menu_sstSetCfgPrtCB() {
  char _msgStr[22];
  uint8_t host_setCFGPRT[28]  = {0xb5,0x62,0x06,0x00,0x14,0x00,0x01,0x00,0x00,0x00,0xc0,0x08,0x00,0x00,0x00,0x96,0x00,0x00,0x07,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x81,0x84};
  while(gpsSerial->available()) gpsSerial->read();
  if(!sdcard_openRxPktFile()) return;
  uint32_t _nowMS = millis();
  gpsSerial->write(host_setCFGPRT, sizeof(host_setCFGPRT));
  while((millis()-_nowMS)<500) {
    sdcard_writeRxPktFile();
  }
  sdcard_closeRxPktFile();
  sprintf(_msgStr, "%s l=%d",
          rxPktFileName,
          min(rxPktWriteCount, 999));
  msg_update(_msgStr);
  displayRefresh = true;
}

/********************************************************************/
void menu_sstReqCfgRateCB() {
  char _msgStr[22];
  uint8_t host_reqCFGRATE[8]  = {0xb5,0x62,0x06,0x08,0x00,0x00,0x0e,0x30};
  while(gpsSerial->available()) gpsSerial->read();
  if(!sdcard_openRxPktFile()) return;
  uint32_t _nowMS = millis();
  gpsSerial->write(host_reqCFGRATE, sizeof(host_reqCFGRATE));
  while((millis()-_nowMS)<500) {
    sdcard_writeRxPktFile();
  }
  sdcard_closeRxPktFile();
  sprintf(_msgStr, "%s l=%d",
          rxPktFileName,
          min(rxPktWriteCount, 999));
  msg_update(_msgStr);
  displayRefresh = true;
}

/********************************************************************/
void menu_sstSetCfgRateCB() {
  char _msgStr[22];
  uint8_t host_setCFGRATE[14] = {0xb5,0x62,0x06,0x08,0x06,0x00,0xe8,0x03,0x01,0x00,0x01,0x00,0x01,0x39};
  while(gpsSerial->available()) gpsSerial->read();
  if(!sdcard_openRxPktFile()) return;
  uint32_t _nowMS = millis();
  gpsSerial->write(host_setCFGRATE, sizeof(host_setCFGRATE));
  while((millis()-_nowMS)<500) {
    sdcard_writeRxPktFile();
  }
  sdcard_closeRxPktFile();
  sprintf(_msgStr, "%s l=%d",
          rxPktFileName,
          min(rxPktWriteCount, 999));
  msg_update(_msgStr);
  displayRefresh = true;
}

/********************************************************************/
void menu_sstSetCfgCfgCB() {
  char _msgStr[22];
  uint8_t host_setCFGCFG[20]  = {0xb5,0x62,0x06,0x09,0x0c,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x19,0x80};
  while(gpsSerial->available()) gpsSerial->read();
  if(!sdcard_openRxPktFile()) return;
  uint32_t _nowMS = millis();
  gpsSerial->write(host_setCFGCFG, sizeof(host_setCFGCFG));
  while((millis()-_nowMS)<500) {
    sdcard_writeRxPktFile();
  }
  sdcard_closeRxPktFile();
  sprintf(_msgStr, "%s l=%d",
          rxPktFileName,
          min(rxPktWriteCount, 999));
  msg_update(_msgStr);
  displayRefresh = true;
}

/********************************************************************/
void menu_sstSetCfgMsgCB() {
  char _msgStr[22];
  static bool enable = true;
  uint8_t host_setCFGMSG1[11]  = {0xb5,0x62,0x06,0x01,0x03,0x00,0x01,0x07,0x01,0x13,0x51};
  uint8_t host_setCFGMSG0[11]  = {0xb5,0x62,0x06,0x01,0x03,0x00,0x01,0x07,0x00,0x12,0x50};
  while(gpsSerial->available()) gpsSerial->read();
  if(!sdcard_openRxPktFile()) return;
  uint32_t _nowMS = millis();
  if(enable) {
    gpsSerial->write(host_setCFGMSG1, sizeof(host_setCFGMSG1));
  } else {
    gpsSerial->write(host_setCFGMSG0, sizeof(host_setCFGMSG0));
  }
  while((millis()-_nowMS)<500) {
    sdcard_writeRxPktFile();
  }
  sdcard_closeRxPktFile();
  sprintf(_msgStr, "%s l=%d",
          rxPktFileName,
          min(rxPktWriteCount, 999));
  msg_update(_msgStr);
  displayRefresh = true;
  enable = !enable;
}

/********************************************************************/
void menu_sstReqNavPvtCB() {
  char _msgStr[22];
  uint8_t host_reqNAVPVT[8]  = {0xb5,0x62,0x01,0x07,0x00,0x00,0x08,0x19};
  while(gpsSerial->available()) gpsSerial->read();
  if(!sdcard_openRxPktFile()) return;
  uint32_t _nowMS = millis();
  gpsSerial->write(host_reqNAVPVT, sizeof(host_reqNAVPVT));
  while((millis()-_nowMS)<1500) {
    sdcard_writeRxPktFile();
  }
  sdcard_closeRxPktFile();
  sprintf(_msgStr, "%s l=%d",
          rxPktFileName,
          min(rxPktWriteCount, 999));
  msg_update(_msgStr);
  displayRefresh = true;
}

/********************************************************************/
void menu_entrGPSEmulCB() {
  deviceState.DEVICE_MODE = DM_GPSEMUL;
  deviceMode_init();
  displayRefresh = true;
}

/********************************************************************/
void menu_exitGPSEmulCB() {
  deviceMode_end();
  menu.exitToParentMenuPage();
  displayRefresh = true;
}

/********************************************************************/
void menu_getRTC_CB() {
  if(clockTime_valid) {
    rtc_datetime_t now = getRTCTime(); // get the RTC
    menuRTCYear    = now.year;
    menuRTCMonth   = now.month;
    menuRTCDay     = now.day;
    menuRTCHour    = now.hour;
    menuRTCMinute  = now.minute;
    menuRTCSecond  = now.second;
  }
}

/********************************************************************/
void menu_setRTC_CB() {
  setRTCTime(menuRTCHour, menuRTCMinute, menuRTCSecond,
             menuRTCDay, menuRTCMonth, menuRTCYear);
  clockTime_valid = true;
}

/********************************************************************/
void menu_cancelSaveSettingsCB() {
  menu.exitToParentMenuPage();
  msg_update("Save Canceled");
  displayRefresh = true;
}

/********************************************************************/
void menu_confirmSaveSettingsCB() {
  menu.exitToParentMenuPage();
  if(sdcard_deviceStateSave()) {
    msg_update("Settings Saved");
  } else {
    msg_update("Save Settings Failed");
  }
  displayRefresh = true;
}

/********************************************************************/
void menu_cancelRestartDeviceCB() {
  menu.exitToParentMenuPage();
  msg_update("Restart Canceled");
  displayRefresh = true;
}

/********************************************************************/
void menu_confirmRestartDeviceCB() {
  device_reset();
}

/********************************************************************/
void menu_cancelResetDeviceCB() {
  menu.exitToParentMenuPage();
  msg_update("Reset Canceled");
  displayRefresh = true;
}

/********************************************************************/
void menu_confirmResetDeviceCB() {
  sdcard_deviceStateReset();
  device_reset();
}

/********************************************************************/
void display_clearDisplay() {
  display.fillSprite(BLACK);
}

/********************************************************************/
void display_display() {
  display.pushSprite(0, 0);
}

/********************************************************************/
bool display_setup() {
  // Setup display
  M5.lcd.setRotation(2);
  display.setPsram(true); // Use psram
  display.setColorDepth(1); // Monochrome
  display.setTextSize(2); // font is 12x16
  display.setTextColor(WHITE);
  display.createSprite(240, 320); //320x240 rotated 90 degrees
  display_clearDisplay();
  displayPV.prt_str("TeenyGPSTestbed_C2", 20, 0, 0);
  display_display();
  delay(1000);
  displayEnabled = true;
  return displayEnabled;
}

