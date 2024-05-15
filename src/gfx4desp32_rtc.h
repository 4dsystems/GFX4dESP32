#ifndef __GFX4DESP32_RTC__
#define __GFX4DESP32_RTC__

#include "gfx4desp32.h"
#include "Wire.h"

struct Time
{
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t weekday;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
};

enum {
    RTC_TIMEFORMAT_HM,
    RTC_TIMEFORMAT_HMS,
    RTC_TIMEFORMAT_YYYY_MM_DD,
    RTC_TIMEFORMAT_MM_DD_YYYY,
    RTC_TIMEFORMAT_DD_MM_YYYY,
    RTC_TIMEFORMAT_YYYY_MM_DD_H_M_S,
    RTC_TIMEFORMAT_DD_MM_YYYY_H_M_S,
};

class gfx4desp32_rtc : virtual public gfx4desp32 {
protected:

    uint8_t RTCread(uint8_t address);                           // read one byte from selected register
    void RTCwrite(uint8_t address, uint8_t data);               // write one byte of data to the register
    void RTCwrite_OR(uint8_t address, uint8_t data);            // write data to the register using OR operations
    void RTCwrite_AND(uint8_t address, uint8_t data);           // write data to the register using AND operation
    unsigned char bcd_to_number(uint8_t first, uint8_t second); // convert two digits to one number
    uint8_t get_first_number(unsigned short number);            // get ten’s place digit of the number
    uint8_t get_second_number(unsigned short number);           // get unit place digit of the number
    enum registers
    {
        PCF8563_address = 0x51,
        Control_status_1 = 0x00,
        Control_status_2 = 0x01,
        VL_seconds = 0x02,
        Minutes = 0x03,
        Hours = 0x04,
        Days = 0x05,
        Weekdays = 0x06,
        Century_months = 0x07,
        Years = 0x08,
        Minute_alarm = 0x09,
        Hour_alarm = 0x0A,
        Day_alarm = 0x0B,
        Weekday_alarm = 0x0C,
        CLKOUT_control = 0x0D,
        Timer_control = 0x0E,
        Timer = 0x0F,
    };

    char format[128];

public:
    gfx4desp32_rtc();
    ~gfx4desp32_rtc();

    void RTCinit();                     // initialize the chip
    void RTCstopClock();                // stop the clock
    void RTCstartClock();               // start the clock
    void RTCsetYear(uint16_t year);     // set year
    void RTCsetMonth(uint8_t month);    // set month
    void RTCsetDay(uint8_t day);        // set day
    void RTCsetHour(uint8_t hour);      // set hour
    void RTCsetMinute(uint8_t minute);  // set minut
    void RTCsetSecond(uint8_t second);  // set second
    Time RTCgetTime();                  // get time
    bool RTCcheckClockIntegrity();      // check clock integrity
    const char* RTCformatDateTime(uint8_t sytle);


};

#endif // __GFX4DESP32_RTC__