
/********************************************************************/
void deviceMode_init() {
  char _dispStr[22];
  switch(deviceState.DEVICE_MODE) {
    case DM_IDLE:
      //statusLED.pulse_repeat(1, 20);
      break;
    case DM_GPSRCVR:
      //statusLED.pulse_repeat(1);
      gpsSerial = &Serial2;
      if(gps.gnss_init(*gpsSerial, GPS_BAUD_RATE)) {
        gpsEnabled = true;
        sprintf(_dispStr, "GPS CONN UBPV=%02d.%02d",
                gps.getProtocolVersionHigh(),
                gps.getProtocolVersionLow());
        msg_update(_dispStr);
      } else {
        gpsEnabled = false;
        msg_update("ERROR - GPS Missing");
      }
      break;
    case DM_GPSLOGR:
      //statusLED.pulse_repeat(1);
      gpsSerial = &Serial2;
      if(gps.gnss_init(*gpsSerial, GPS_BAUD_RATE)) {
        gpsEnabled = true;
        sprintf(_dispStr, "GPS CONN UBPV=%02d.%02d",
                gps.getProtocolVersionHigh(),
                gps.getProtocolVersionLow());
        msg_update(_dispStr);
      } else {
        gpsEnabled = false;
        msg_update("ERROR - GPS Missing");
      }
      break;
    case DM_GPSNSAT:
      //statusLED.pulse_repeat(1);
      gpsSerial = &Serial2;
      if(gps.gnss_init(*gpsSerial, GPS_BAUD_RATE, 1, 10)) {
        gpsEnabled = true;
        sprintf(_dispStr, "GPS CONN UBPV=%02d.%02d",
                gps.getProtocolVersionHigh(),
                gps.getProtocolVersionLow());
        msg_update(_dispStr);
      } else {
        gpsEnabled = false;
        msg_update("ERROR - GPS Missing");
      }
      break;
    case DM_GPSCAPT:
      //statusLED.pulse_repeat(1);
      gpsSerial = &Serial2;
      gpsSerial->begin(GPS_BAUD_RATE);
      msg_update("GPS Serial Enabled");
      break;
    case DM_GPSSSTP:
      //statusLED.pulse_repeat(1);
      gpsSerial = &Serial2;
      gpsSerial->begin(GPS_BAUD_RATE);
      msg_update("GPS Serial Enabled");
      break;
    case DM_GPSEMUL:
      //statusLED.pulse_repeat(1);
      emulatorSerial = &Serial2;
      if(emulator_setup(*emulatorSerial, EMULATOR_BAUD_RATE)) {
        msg_update("EMU Serial Enabled");
      } else {
        msg_update("ERROR - EMU Setup");
      }
      break;
  }
}

/********************************************************************/
void deviceMode_end() {
  switch(deviceState.DEVICE_MODE) {
    case DM_GPSRCVR:
      gpsEnabled = false;
      msg_update("GPS Receiver Stopped");
      break;
    case DM_GPSLOGR:
      gpsEnabled = false;
      msg_update("GPS Logger Stopped");
      break;
    case DM_GPSNSAT:
      gpsEnabled = false;
      msg_update("GPS NAVSAT Stopped");
      break;
    case DM_GPSCAPT:
      msg_update("Capture Mode Stopped");
      break;
    case DM_GPSSSTP:
      msg_update("Stepper Mode Stopped");
      break;
    case DM_GPSEMUL:
      emulator_end();
      msg_update("Emulator Stopped");
      break;
  }
  clockTime_valid = false;
  deviceState.DEVICE_MODE = DM_IDLE;
  //statusLED.pulse_repeat(1, 20);
}

