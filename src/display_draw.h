
/********************************************************************/
char* getISO8601DateTimeStr(rtc_datetime_t _dateTime) {
  //e.g. "2020-06-25T15:29:37"
  static char _itdStr[20];
  sprintf(_itdStr, "%u-%02d-%02dT%02d:%02d:%02d",
          _dateTime.year, _dateTime.month, _dateTime.day,
          _dateTime.hour, _dateTime.minute, _dateTime.second);
  return _itdStr;
}

/********************************************************************/
char* getRTCClockISO8601DateTimeStr() {
  static char _itdStr[20];
  if(clockTime_valid) {
    rtc_datetime_t now = getRTCTime(); // get the RTC
    return getISO8601DateTimeStr(now);
  }
  sprintf(_itdStr, "**  NO TIME FIX  **");
  return _itdStr;
}

/********************************************************************/
char* getGPSISO8601DateTimeStr() {
  static char _itdStr[20];
  if(gps.isDateValid() && gps.isTimeValid()) {
    rtc_datetime_t dateTime;
    dateTime.year   = gps.getYear();
    dateTime.month  = gps.getMonth();
    dateTime.day    = gps.getDay();
    dateTime.hour   = gps.getHour();
    dateTime.minute = gps.getMinute();
    dateTime.second = gps.getSecond();
    return getISO8601DateTimeStr(dateTime);
  }
  sprintf(_itdStr, "* PVTTIME INVALID *");
  return _itdStr;
}

/********************************************************************/
char* getPVTPacketISO8601DateTimeStr(ubxNAVPVTInfo_t _ubxNAVPVTInfo) {
  static char _itdStr[20];
  if(_ubxNAVPVTInfo.dateValid && _ubxNAVPVTInfo.timeValid) {
    rtc_datetime_t dateTime;
    dateTime.year   = _ubxNAVPVTInfo.year;
    dateTime.month  = _ubxNAVPVTInfo.month;
    dateTime.day    = _ubxNAVPVTInfo.day;
    dateTime.hour   = _ubxNAVPVTInfo.hour;
    dateTime.minute = _ubxNAVPVTInfo.minute;
    dateTime.second = _ubxNAVPVTInfo.second;
    return getISO8601DateTimeStr(dateTime);
  }
  sprintf(_itdStr, "* PVTTIME INVALID *");
  return _itdStr;
}

/********************************************************************/
char* getLatitudeStr(const float latitude) {
  static char _fStr[10];
  static char _lStr[11];
  if(latitude < 0.0) {
    dtostrf(-latitude, -7, 5, _fStr);
    sprintf(_lStr, "S%s", _fStr);
//} else if(latitude == 0.0) {
//  dtostrf(latitude, -7, 5, _fStr);
//  sprintf(_lStr, "%s", _fStr);
  } else {
    dtostrf(latitude, -7, 5, _fStr);
    sprintf(_lStr, "N%s", _fStr);
  }
  return _lStr;
}

/********************************************************************/
char* getLongitudeStr(const float longitude) {
  static char _fStr[10];
  static char _lStr[11];
  if(longitude < 0.0) {
    dtostrf(-longitude, -7, 5, _fStr);
    sprintf(_lStr, "W%s", _fStr);
//} else if(longitude == 0.0) {
//  dtostrf(longitude, -7, 5, _fStr);
//  sprintf(_lStr, "%s", _fStr);
  } else {
    dtostrf(longitude, -7, 5, _fStr);
    sprintf(_lStr, "E%s", _fStr);
  }
  return _lStr;
}

/********************************************************************/
char* getHeadingStr(const float degrees) {
  float deg = degrees + 11.25;
  if(deg >= 360.0) deg = deg - 360;
  deg = max(0, deg);
  uint8_t headIdx = (uint8_t)(deg / 22.5);
  switch(headIdx) {
    case  0: return "N";
    case  1: return "NNE";
    case  2: return "NE";
    case  3: return "ENE";
    case  4: return "E";
    case  5: return "ESE";
    case  6: return "SE";
    case  7: return "SSE";
    case  8: return "S";
    case  9: return "SSW";
    case 10: return "SW";
    case 11: return "WSW";
    case 12: return "W";
    case 13: return "WNW";
    case 14: return "NW";
    case 15: return "NNW";
  }
  return "";
}

/********************************************************************/
void display_refresh() {
  uint32_t _nowMS = millis();
  static uint32_t _display_refresh_timer = _nowMS;
  if((_nowMS-_display_refresh_timer) < DISPLAY_REFRESH_PERIOD) {
    return;
  }
  _display_refresh_timer = _nowMS;
  char _dispStr[22];
  if(msg_update(nullptr)) {
    displayRefresh = true;
  }
  // Update display
  if(displayRefresh) {
    // clear display buffer
    display_clearDisplay();
    // draw the menu first
    if(!menuDisplaySleepMode) {
      menu.drawMenu();
      if(menu.isMenuPageCurrent(menuPageMain)) {
        // main/idle display
      } else if(menu.isMenuPageCurrent(menuPageGPSRcvr)) {
        // RTC Clock
        displayPV.prt_str(getRTCClockISO8601DateTimeStr(), 19, 6, 16);
        // GPS Clock
        displayPV.prt_str(getGPSISO8601DateTimeStr(), 19, 6, 32);
        // GPS Location
        if(gps.isLocationValid()) {
          displayPV.prt_str(getLatitudeStr(gps.getLatitude()), 10, 0, 48);
          displayPV.prt_str(getLongitudeStr(gps.getLongitude()), 10, 120, 48);
          sprintf(_dispStr, "ALT=%03d", max(min(gps.getAltitudeMSL(), 999), -99));
          displayPV.prt_str(_dispStr, 8, 0, 64);
          sprintf(_dispStr, "HA=%03d", min(gps.getHAccEst(), 999));
          displayPV.prt_str(_dispStr, 8, 90, 64);
          sprintf(_dispStr, "VA=%03d", min(gps.getVAccEst(), 999));
          displayPV.prt_str(_dispStr, 8, 168, 64);
          sprintf(_dispStr, "H=%s", getHeadingStr(gps.getHeading()));
          displayPV.prt_str(_dispStr, 5, 0, 80);
          sprintf(_dispStr, "F=%d", min(gps.getLocationFixType(), 9));
          displayPV.prt_str(_dispStr, 3, 72, 80);
          sprintf(_dispStr, "S=%02d", min(gps.getNumSV(), 99));
          displayPV.prt_str(_dispStr, 4, 114, 80);
          displayPV.prt_str("PP=", 3, 168, 80);
          displayPV.prt_float(min(gps.getPDOP(), 9.9), 3, 1, 204, 80);
        } else {
          sprintf(_dispStr, "**  NO GNSS FIX  **");
          displayPV.prt_str(_dispStr, 19, 6, 56);
        }
        sprintf(_dispStr, "L%XD%XT%X", gps.isLocationValid(),
                gps.isDateValid(), gps.isTimeValid());
        displayPV.prt_str(_dispStr, 20, 0, 96);
      } else if(menu.isMenuPageCurrent(menuPageGPSLogr)) {
        // RTC Clock
        displayPV.prt_str(getRTCClockISO8601DateTimeStr(), 19, 6, 16);
        // GPS Clock
        displayPV.prt_str(getGPSISO8601DateTimeStr(), 19, 6, 32);
        // GPS Location
        if(true || gps.isLocationValid()) {
          displayPV.prt_str(getLatitudeStr(gps.getLatitude()), 10, 0, 48);
          displayPV.prt_str(getLongitudeStr(gps.getLongitude()), 10, 120, 48);
          sprintf(_dispStr, "H=%s", getHeadingStr(gps.getHeading()));
          displayPV.prt_str(_dispStr, 5, 0, 64);
          sprintf(_dispStr, "F=%d", min(gps.getLocationFixType(), 9));
          displayPV.prt_str(_dispStr, 3, 72, 64);
          sprintf(_dispStr, "S=%02d", min(gps.getNumSV(), 99));
          displayPV.prt_str(_dispStr, 4, 114, 64);
          displayPV.prt_str("PP=", 3, 168, 64);
          displayPV.prt_float(min(gps.getPDOP(), 9.9), 3, 1, 204, 64);
        } else {
          sprintf(_dispStr, "**  NO GNSS FIX  **");
          displayPV.prt_str(_dispStr, 19, 6, 80);
        }
        if(ubxLoggingInProgress) {
          sprintf(_dispStr, "  TP=%04d VP=%04d",
                  min(ubxLoggingFileWriteCount, 9999),
                  min(ubxLoggingFileWriteValidCount, 9999));
          displayPV.prt_str(_dispStr, 20, 0, 80);
        }
      } else if(menu.isMenuPageCurrent(menuPageGPSNsat)) {
        // RTC Clock
        displayPV.prt_str(getRTCClockISO8601DateTimeStr(), 19, 6, 16);
        // GPS Clock
        displayPV.prt_str(getGPSISO8601DateTimeStr(), 19, 6, 32);
        // NAVSAT Data
        ubloxPacket_t satPacket;
        ubloxNAVSATInfo_t satInfo;
        gps.getNAVSATPacket(satPacket);
        gps.getNAVSATInfo(satInfo);
        for(uint8_t i=0; i<9; i++) {
          sprintf(_dispStr, "%d%02d/%02d",
                  satInfo.svSortList[i].gnssId,satInfo.svSortList[i].svId, satInfo.svSortList[i].cno);
          displayPV.prt_str(_dispStr, 6, (i%3)*84, ((i/3)*16)+48);
        }
        sprintf(_dispStr, "L%XD%XT%X NS%XS%dH%dU%d",
                gps.isLocationValid(), gps.isDateValid(), gps.isTimeValid(),
                satPacket.validPacket, satInfo.numSvs,
                satInfo.numSvsHealthy, satInfo.numSvsUsed);
        displayPV.prt_str(_dispStr, 20, 0, 96);
      } else if(menu.isMenuPageCurrent(menuPageGPSCapt)) {
        if(menu_captRxPktInProgress) {
          sprintf(_dispStr, " File = %s", rxPktFileName);
          displayPV.prt_str(_dispStr, 20, 0, 64);
          sprintf(_dispStr, " Bytes = %d", rxPktWriteCount);
          displayPV.prt_str(_dispStr, 20, 0, 80);
        }
      } else if(menu.isMenuPageCurrent(menuPageGPSStep)) {
      } else if(menu.isMenuPageCurrent(menuPageGPSEmul)) {
        displayPV.prt_str(getRTCClockISO8601DateTimeStr(), 19, 6, 16);
        ubxNAVPVTInfo_t _ubxNAVPVTInfo = emulator.getPVTPacketInfo();
        displayPV.prt_str(getLatitudeStr((float)_ubxNAVPVTInfo.latitude * 1e-7), 10, 0, 32);
        displayPV.prt_str(getLongitudeStr((float)_ubxNAVPVTInfo.longitude * 1e-7), 10, 120, 32);
        sprintf(_dispStr, "ALT=%03d", max(min(_ubxNAVPVTInfo.altitudeMSL / 1000, 999), -99));
        displayPV.prt_str(_dispStr, 8, 0, 48);
        sprintf(_dispStr, "HA=%03d", min(_ubxNAVPVTInfo.hAcc / 1000, 999));
        displayPV.prt_str(_dispStr, 8, 90, 48);
        sprintf(_dispStr, "VA=%03d", min(_ubxNAVPVTInfo.vAcc / 1000, 999));
        displayPV.prt_str(_dispStr, 8, 168, 48);
        sprintf(_dispStr, "H=%s", getHeadingStr((float)_ubxNAVPVTInfo.headMot * 1e-5));
        displayPV.prt_str(_dispStr, 5, 0, 64);
        sprintf(_dispStr, "F=%d", min(_ubxNAVPVTInfo.fixType, 9));
        displayPV.prt_str(_dispStr, 3, 72, 64);
        sprintf(_dispStr, "S=%02d", min(_ubxNAVPVTInfo.numSV, 99));
        displayPV.prt_str(_dispStr, 4, 114, 64);
        displayPV.prt_str("PP=", 3, 168, 64);
        displayPV.prt_float(min((float)_ubxNAVPVTInfo.pDOP * 1e-2, 9.9), 3, 1, 204, 64);
      //sprintf(_dispStr, "RXP CL%02XID%02XPL%02dV%d",
      //        emulator.receivedPacket.messageClass,
      //        emulator.receivedPacket.messageID,
      //        emulator.receivedPacket.payloadLength,
      //        emulator.receivedPacket.validPacket);
      //displayPV.prt_str(_dispStr, 20, 0, 32);
      //sprintf(_dispStr, "RSP CL%02XID%02XPL%02dV%d",
      //        emulator.responsePacket.messageClass,
      //        emulator.responsePacket.messageID,
      //        emulator.responsePacket.payloadLength,
      //        emulator.responsePacket.validPacket);
      //displayPV.prt_str(_dispStr, 20, 0, 48);
      //sprintf(_dispStr, "AKP CL%02XID%02XPL%02dV%d",
      //        emulator.acknowledgePacket.messageClass,
      //        emulator.acknowledgePacket.messageID,
      //        emulator.acknowledgePacket.payloadLength,
      //        emulator.acknowledgePacket.validPacket);
      //displayPV.prt_str(_dispStr, 20, 0, 64);
      //sprintf(_dispStr, "UNP CL%02XID%02XPL%02dV%d",
      //        emulator.unknownPacket.messageClass,
      //        emulator.unknownPacket.messageID,
      //        emulator.unknownPacket.payloadLength,
      //        emulator.unknownPacket.validPacket);
      //displayPV.prt_str(_dispStr, 20, 0, 80);
      }
    }
    msg_update(nullptr);
    displayRefresh = false;
    displayUpdate = true;
  }
}

/********************************************************************/
void display_update() {
  if(displayUpdate) {
    display_display();
    displayUpdate = false;
  }
}

