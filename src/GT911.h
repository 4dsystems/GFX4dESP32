#ifndef GT911_H
#define GT911_H

#include <Arduino.h>
#include <Wire.h>
#include "GT911_Structs.h"

// 0x28/0x29 (0x14 7bit)
#define GT911_I2C_ADDR_28  0x14
// 0xBA/0xBB (0x5D 7bit)
#define GT911_I2C_ADDR_BA  0x5D

#define GT911_MAX_CONTACTS 5

#define GT911_REG_CFG 0x8047
#define GT911_REG_CHECKSUM 0x80FF
#define GT911_REG_DATA 0x8140
#define GT911_REG_ID 0x8140
#define GT911_REG_COORD_ADDR 0x814E

#define GT928_TOUCH_CONTROLLER		0x01
#define GT967_TOUCH_CONTROLLER		0x03

#define GOODIX_REG_CONFIG_DATA  0x8047

#define GOODIX_REG_CONFIG_MIDDLE	0x80A2
#define GOODIX_REG_CONFIG_END		0x80FE
#define GOODIX_REG_CONFIG_END_967	0x8128

#define RESOLUTION_SET_SUCCESS		0x00
#define RESOLUTION_SET_FAIL_READ_1	0x01
#define RESOLUTION_SET_FAIL_READ_2	0x02
#define RESOLUTION_SET_FAIL_WRITE	0x03
#define RESOLUTION_ALREADY_SET		0x04

enum : uint8_t {
  GT911_MODE_INTERRUPT,
  GT911_MODE_POLLING
};

class GT911 {
  public:
    enum class Rotate {
      _0,
      _90,
      _180,
      _270,
    };
  private:
    TwoWire *_wire;
    int8_t _intPin;
    int8_t _rstPin;
    uint8_t _addr;
	bool RSToverride;
    bool _configLoaded = false;
    GTConfig _config;
    GTInfo _info;
    GTPoint _points[GT911_MAX_CONTACTS];

    Rotate _rotation = Rotate::_90;

    void reset();
    void i2cStart(uint16_t reg);
    bool write(uint16_t reg, uint8_t data);
    uint8_t read(uint16_t reg);
    bool writeBytes(uint16_t reg, uint8_t *data, uint16_t size);
    bool readBytes(uint16_t reg, uint8_t *data, uint16_t size);
    uint8_t calcChecksum(uint8_t *buf, uint8_t len);
    uint8_t readChecksum();
    int8_t readTouches();
    bool readTouchPoints();
  public:
    GT911(TwoWire *twi = &Wire);
	bool begin(int8_t intPin = -1, int8_t rstPin = -1, uint8_t addr = GT911_I2C_ADDR_BA,
               uint32_t clk = 400000);
    bool productID(uint8_t *buf, uint8_t len);
    GTConfig* readConfig();
    bool writeConfig();
    GTInfo* readInfo();

    int8_t touched(uint8_t mode = GT911_MODE_INTERRUPT);
    GTPoint getPoint(uint8_t num);
    GTPoint *getPoints();

    void SetRotation(int rt);
	void setRotation(Rotate rotation);
	
	int fwResolution(uint16_t maxX, uint16_t maxY, int tCont);
	void fwWrite(const uint8_t* fwDat, int fwSize, int tCont);
	int GetHRES();
	int GetVRES();
};

#endif
