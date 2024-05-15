#include "gfx4desp32_rtc.h"

gfx4desp32_rtc::gfx4desp32_rtc() : gfx4desp32() {}

gfx4desp32_rtc::~gfx4desp32_rtc() {}

/****************************************************************************/
/*!
  @brief  initialize PCF8563 RTC
  @note   Clears TESTC and CLKOUT
*/
/****************************************************************************/
void gfx4desp32_rtc::RTCinit() {
    if (I2CInit == false) {
        if (Wire.begin(17, 18, 400000)) {
            I2CInit = true;
        }
        else {
        }
    }    
    RTCwrite_AND(Control_status_1, 247); // clear TESTC bit
    RTCwrite_AND(CLKOUT_control, 127);   // clear CLKOUT enable bit
}
/****************************************************************************/
/*!
  @brief  Start the clock
*/
/****************************************************************************/
void gfx4desp32_rtc::RTCstartClock() {
    RTCwrite_AND(Control_status_1, ~(1 << 5));
}
/****************************************************************************/
/*!
  @brief  Stop the clock
*/
/****************************************************************************/
void gfx4desp32_rtc::RTCstopClock() {
    RTCwrite_OR(Control_status_1, 1 << 5);
}

/****************************************************************************/
/*!
  @brief  Set Year
  @param  Year - 0 to 99
*/
/****************************************************************************/
void gfx4desp32_rtc::RTCsetYear(uint16_t year) {
    uint8_t y1, y2;
    uint16_t year100;
    year100 = year / 100;
    if (year100 == 20) {
        y1 = RTCread(Century_months) & 0x7f;
    }
    else {
        y1 = (RTCread(Century_months) & 0x7f) + 0x80;
    }
    y2 = year - (year100 * 100);
    const uint8_t data = ((get_second_number(y2)) << 4) | (get_first_number(y2));
    RTCwrite(Century_months, y1);
    RTCwrite(Years, data);
}
/****************************************************************************/
/*!
  @brief  Set Month
  @param  Month - 1 to 12
*/
/****************************************************************************/
void gfx4desp32_rtc::RTCsetMonth(uint8_t month) {
    uint8_t century = RTCread(Century_months) & 0x80;
    const uint8_t data =
        ((get_second_number(month)) << 4) | (get_first_number(month));
    RTCwrite(Century_months, (century | (data & 0x7f)));
}

/****************************************************************************/
/*!
  @brief  Set Day
  @param  Day - 1 to 31
*/
/****************************************************************************/
void gfx4desp32_rtc::RTCsetDay(uint8_t day) {
    const uint8_t data =
        ((get_second_number(day)) << 4) | (get_first_number(day));
    RTCwrite(Days, data);
}

/****************************************************************************/
/*!
  @brief  Set Hour
  @param  Hour - 0 to 24
*/
/****************************************************************************/
void gfx4desp32_rtc::RTCsetHour(uint8_t hour) {
    const uint8_t data =
        ((get_second_number(hour)) << 4) | (get_first_number(hour));
    RTCwrite(Hours, data);
}

/****************************************************************************/
/*!
  @brief  Set Minute
  @param  Minute - 0 to 59
*/
/****************************************************************************/
void gfx4desp32_rtc::RTCsetMinute(uint8_t minute) {
    const uint8_t data =
        ((get_second_number(minute)) << 4) | (get_first_number(minute));
    RTCwrite(Minutes, data);
}

/****************************************************************************/
/*!
  @brief  Set Seconds
  @param  Seconds - 0 to 99
*/
/****************************************************************************/
void gfx4desp32_rtc::RTCsetSecond(uint8_t second) {
    const uint8_t data =
        ((get_second_number(second)) << 4) | (get_first_number(second));
    RTCwrite(VL_seconds, data);
}

/****************************************************************************/
/*!
  @brief  Get Current Time
  @note   Time (current code encoded into the Time structure)
*/
/****************************************************************************/
Time gfx4desp32_rtc::RTCgetTime() {
    uint8_t century = (RTCread(Century_months) & 0x80) >> 7;
    uint16_t year100;
    if (century) {
        year100 = 1900;
    }
    else {
        year100 = 2000;
    }
    Time output;
    // read data registers contents
    const uint16_t YEAR = RTCread(Years);
    const uint8_t MONTH = RTCread(Century_months);
    const uint8_t DAY = RTCread(Days);
    const uint8_t WEEKDAY = RTCread(Weekdays);
    const uint8_t HOUR = RTCread(Hours);
    const uint8_t MINUTE = RTCread(Minutes);
    const uint8_t SECONDS = RTCread(VL_seconds);
    // convert readed data to numbers using bcd_to_number function).
    output.year =
        year100 + bcd_to_number((YEAR & 0b11110000) >> 4, YEAR & 0b00001111);
    output.month = bcd_to_number((MONTH & 0b00010000) >> 4, MONTH & 0b00001111);
    output.day = bcd_to_number((DAY & 0b00110000) >> 4, DAY & 0b00001111);
    output.weekday = bcd_to_number(0, WEEKDAY & 0b00000111);
    output.hour = bcd_to_number((HOUR & 0b00110000) >> 4, HOUR & 0b00001111);
    output.minute =
        bcd_to_number((MINUTE & 0b01110000) >> 4, MINUTE & 0b00001111);
    output.second =
        bcd_to_number((SECONDS & 0b01110000) >> 4, SECONDS & 0b00001111);
    return output;
}

/****************************************************************************/
/*!
  @brief  Check clock integrity
  @note   returns clock status
*/
/****************************************************************************/
bool gfx4desp32_rtc::RTCcheckClockIntegrity() {
    const uint8_t data = RTCread(VL_seconds); // read the data
    if (data & (1 << 7)) {
        return 0; // if clock integrity is not guaranteed return 0
    }
    else {
        return 1; // otherwise return 1
    }
}

// Read one byte of data
// Parameters:
//  * uint8_t address  - register read_address
// Returns: readed byte of data (uint8_t)
uint8_t gfx4desp32_rtc::RTCread(uint8_t address) {
    Wire.beginTransmission(PCF8563_address); // begin transmission
    Wire.write(address); // inform chip what register we want to read
    Wire.endTransmission();
    Wire.requestFrom(PCF8563_address, 1); // request one byte from the chip
    uint8_t data = Wire.read();           // read the data
    return data;
}

// Convert BCD format to number
// Parameters:
//  * uint8_t first -> first digit
//  * uint8_t second -> second digit
// Returns: the result of the conversion (unsigned char)
unsigned char gfx4desp32_rtc::bcd_to_number(uint8_t first,
    uint8_t second) {
    unsigned char output;
    output = first * 10;
    output = output + second;
    return output;
}

// Get first digit of the number
// Parameters:
//  * unsigned short ->
// Returns: digit (uint8_t)
uint8_t gfx4desp32_rtc::get_first_number(unsigned short number) {
    uint8_t output = number % 10;
    return output;
}

// Get second digit of the number
// Parameters:
//  * unsigned short ->
// Returns: digit (uint8_t)
uint8_t gfx4desp32_rtc::get_second_number(unsigned short number) {
    uint8_t output = number / 10;
    return output;
}

// Write one byte of data
// Parameters:
//  * uint8_t address  - register read_address
//  * uint8_t data     - byte of data that we want to write to the register
// Returns: none
void gfx4desp32_rtc::RTCwrite(uint8_t address, uint8_t data) {
    Wire.beginTransmission(PCF8563_address);
    Wire.write(address);
    Wire.write(data);
    Wire.endTransmission();
}

// Change state of the register using OR operation
// Parameters:
//  * uint8_t address    - register address
//  * uint8_t data       - one byte of data that we want to put in the register
// Returns: none
void gfx4desp32_rtc::RTCwrite_OR(uint8_t address, uint8_t data) {
    uint8_t c = RTCread(address);
    c = c | data;
    RTCwrite(address, c);
}

// Change state of the register using AND operation
// Parameters:
//  * uint8_t address    - register address
//  * uint8_t data       - one byte of data that we want to put in the register
// Returns: none
void gfx4desp32_rtc::RTCwrite_AND(uint8_t address, uint8_t data) {
    uint8_t c = RTCread(address);
    c = c & data;
    RTCwrite(address, c);
}

const char* gfx4desp32_rtc::RTCformatDateTime(uint8_t sytle) {
    Time t = RTCgetTime();
    switch (sytle) {
    case RTC_TIMEFORMAT_HM:
        snprintf(format, sizeof(format), "%02d:%02d", t.hour, t.minute);
        break;
    case RTC_TIMEFORMAT_HMS:
        snprintf(format, sizeof(format), "%02d:%02d:%02d", t.hour, t.minute,
            t.second);
        break;
    case RTC_TIMEFORMAT_YYYY_MM_DD:
        snprintf(format, sizeof(format), "%04d-%02d-%02d", t.year, t.month, t.day);
        break;
    case RTC_TIMEFORMAT_MM_DD_YYYY:
        snprintf(format, sizeof(format), "%02d-%02d-%04d", t.month, t.day, t.year);
        break;
    case RTC_TIMEFORMAT_DD_MM_YYYY:
        snprintf(format, sizeof(format), "%02d-%02d-%04d", t.day, t.month, t.year);
        break;
    case RTC_TIMEFORMAT_YYYY_MM_DD_H_M_S:
        snprintf(format, sizeof(format), "%d-%d-%d/%d:%d:%d", t.year, t.month,
            t.day, t.hour, t.minute, t.second);
        break;
    case RTC_TIMEFORMAT_DD_MM_YYYY_H_M_S:
        snprintf(format, sizeof(format), "%02d-%02d-%04d/%02d:%02d:%02d", t.day,
            t.month, t.year, t.hour, t.minute, t.second);
        break;
    default:
        snprintf(format, sizeof(format), "%02d:%02d", t.hour, t.minute);
        break;
    }
    return format;
}
