
/********************************************************************/
// RTC Date/Time Struct
typedef struct {
  uint16_t year;
  uint8_t  month;
  uint8_t  day;
  uint8_t  hour;
  uint8_t  minute;
  uint8_t  second;
  uint8_t  pad0;
} rtc_datetime_t;

/********************************************************************/
// RTC
bool clockTime_valid;

/********************************************************************/
void rtc_setup() {
  clockTime_valid = false;
}

/********************************************************************/
rtc_datetime_t getRTCTime() {
  rtc_datetime_t _datetime;
  RTC_TimeTypeDef rtcTime;
  RTC_DateTypeDef rtcDate;
  M5.Rtc.GetTime(&rtcTime);
  M5.Rtc.GetDate(&rtcDate);
  _datetime.year   = rtcDate.Year;
  _datetime.month  = rtcDate.Month;
  _datetime.day    = rtcDate.Date;
  _datetime.hour   = rtcTime.Hours;
  _datetime.minute = rtcTime.Minutes;
  _datetime.second = rtcTime.Seconds;
  return _datetime;
}

/********************************************************************/
void setRTCTime(uint8_t hour, uint8_t minute, uint8_t second,
                uint8_t day, uint8_t month, uint16_t year) {
  RTC_TimeTypeDef rtcTime;
  RTC_DateTypeDef rtcDate;
  rtcTime.Hours   = hour;
  rtcTime.Minutes = minute;
  rtcTime.Seconds = second;
  M5.Rtc.SetTime(&rtcTime);
  rtcDate.Year  = year;
  rtcDate.Month = month;
  rtcDate.Date  = day;
  M5.Rtc.SetDate(&rtcDate);
}

