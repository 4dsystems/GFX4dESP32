#include "GT911.h"
#include <Wire.h>

#ifndef ICACHE_RAM_ATTR
#define ICACHE_RAM_ATTR
#endif

GT911::GT911(TwoWire *twi) : _wire(twi ? twi : &Wire) {

}

void GT911::reset() {
  
}

void GT911::i2cStart(uint16_t reg) {
  Wire.beginTransmission(_addr);
  Wire.write(reg >> 8);
  Wire.write(reg & 0xFF);
}

bool GT911::write(uint16_t reg, uint8_t data) {
  i2cStart(reg);
  Wire.write(data);
  return Wire.endTransmission() == 0;
}

uint8_t GT911::read(uint16_t reg) {
  i2cStart(reg);
  Wire.endTransmission();
  Wire.requestFrom(_addr, (uint8_t)1);
  while (Wire.available()) {
    return Wire.read();
  }
  return 0;
}

bool GT911::writeBytes(uint16_t reg, uint8_t *data, uint16_t size) {
  i2cStart(reg);
  for (uint16_t i = 0; i < size; i++) {
    Wire.write(data[i]);
  }
  return Wire.endTransmission() == 0;
}

bool GT911::readBytes(uint16_t reg, uint8_t *data, uint16_t size) {
  i2cStart(reg);
  Wire.endTransmission();

  uint16_t index = 0;
  while (index < size) {
    uint8_t req = _min(size - index, I2C_BUFFER_LENGTH);
    Wire.requestFrom(_addr, req);
    while (Wire.available()) {
      data[index++] = Wire.read();
    }
    index++;
  }

  return size == index - 1;
}

uint8_t GT911::calcChecksum(uint8_t *buf, uint8_t len) {
  uint8_t ccsum = 0;
  for (uint8_t i = 0; i < len; i++) {
    ccsum += buf[i];
  }

  return (~ccsum) + 1;
}

uint8_t GT911::readChecksum() {
  return read(GT911_REG_CHECKSUM);
}

int8_t GT911::readTouches() {
  uint32_t timeout = millis() + 20;
  do {
    uint8_t flag = read(GT911_REG_COORD_ADDR);
    if ((flag & 0x80) && ((flag & 0x0F) < GT911_MAX_CONTACTS)) {
      write(GT911_REG_COORD_ADDR, 0);
      return flag & 0x0F;
    }
    delay(1);
  } while (millis() < timeout);

  return -1;
}

bool GT911::readTouchPoints() {
  bool result = readBytes(GT911_REG_COORD_ADDR + 1, (uint8_t*)_points, sizeof(GTPoint) * GT911_MAX_CONTACTS);
  /*
  if (result && _rotation != Rotate::_0) {
    for (uint8_t i = 0; i < 5; i++) {
        _points[i].x = _points[i].x;
        _points[i].y = _points[i].y;
    }
  }
  */
  if (result && _rotation != Rotate::_0) {
    for (uint8_t i = 0; i < 5; i++) {
	  if (_rotation == Rotate::_90) {
        int temp = _points[i].x;
		_points[i].x = _info.yResolution - _points[i].y;
        _points[i].y = temp;
      }
	  if (_rotation == Rotate::_270) {
        int temp = _points[i].x;
		_points[i].x = _info.yResolution - _points[i].y;
        _points[i].y = temp;
      }
	  if (_rotation == Rotate::_180) {
        int temp = _points[i].x;
		_points[i].x = _info.yResolution - _points[i].y;
        _points[i].y = temp;
      }
    }
  }

  return result;
}

bool GT911::begin(int8_t intPin, int8_t rstPin, uint8_t addr, uint32_t clk) {
  _intPin = intPin;
  _rstPin = rstPin;
  _addr = addr;
  Wire.beginTransmission(_addr);
  if (Wire.endTransmission() == 0) {
	readInfo(); // Need to get resolution to use rotation	
    return true;
  }
  return false;
}

void GT911::fwWrite(const uint8_t* fwDat, int fwSize, int tCont){
	uint8_t buf3[2];
	buf3[0] = calcChecksum((uint8_t*)fwDat, fwSize);
    buf3[1] = 0x01;
	writeBytes(GOODIX_REG_CONFIG_DATA, (uint8_t*)fwDat, fwSize);
	if(tCont == 1) writeBytes(GOODIX_REG_CONFIG_END+1, buf3, 2);
	if(tCont == 3 || tCont == 4) writeBytes(GOODIX_REG_CONFIG_END_967+1, buf3, 2);
}

int GT911::fwResolution(uint16_t maxX, uint16_t maxY, int tCont) {
  int err = 0;
  if(_info.yResolution == maxY && _info.xResolution == maxX) return 4;
  uint8_t len1 = GOODIX_REG_CONFIG_MIDDLE - GOODIX_REG_CONFIG_DATA +1;
  uint8_t len2, len3;
  len3 = 0;
  len2 = GOODIX_REG_CONFIG_END - GOODIX_REG_CONFIG_MIDDLE;
  //if (tCont == 3) len2 = GOODIX_REG_CONFIG_END_967 - GOODIX_REG_CONFIG_MIDDLE;
  if (tCont == 3) len3 = GOODIX_REG_CONFIG_END_967 - GOODIX_REG_CONFIG_END;
  uint8_t buf1[len1];
  uint8_t buf2[len2];
  uint8_t buf4[len3];
  uint8_t buf3[2];
  uint8_t buf[len1+len2+len3];
  if (!(readBytes(GOODIX_REG_CONFIG_DATA, buf1, len1))) return 1;
    if (!(readBytes(GOODIX_REG_CONFIG_MIDDLE+1, buf2, len2))) return 2;
    if (tCont == 3){
    readBytes(GOODIX_REG_CONFIG_END+1, buf4, len3);
  }
  memcpy(buf, buf1, sizeof(buf1));
  memcpy(buf+sizeof(buf1), buf2, sizeof(buf2));
  if (tCont == 3){
    memcpy(buf+(sizeof(buf1)+sizeof(buf2)), buf4, sizeof(buf4));
  }
  int tempx, tempy;
  tempx = buf[1] + (buf[2] << 8);
  tempy = buf[3] + (buf[4] << 8);  
  if (tempx != maxX || tempy != maxY){
    buf[0]++;  
    buf[1] = (maxX & 0xff);
    buf[2] = (maxX >> 8);
    buf[3] = (maxY & 0xff);
    buf[4] = (maxY >> 8);
    buf3[0] = calcChecksum(buf, len1+len2);
    buf3[1] = 0x01;
    if (!(writeBytes(GOODIX_REG_CONFIG_DATA, buf, len1+len2))) return 3;
    if(tCont == 1) writeBytes(GOODIX_REG_CONFIG_END+1, buf3, 2);
    if(tCont == 3 || tCont == 4) writeBytes(GOODIX_REG_CONFIG_END_967+1, buf3, 2);
  }
  readInfo();
  return err;
}

bool GT911::productID(uint8_t *buf, uint8_t len) {
  if (len < 4) {
    return false;
  }

  memset(buf, 0, 4);
  return readBytes(GT911_REG_ID, buf, 4);
}

GTConfig* GT911::readConfig() {
  readBytes(GT911_REG_CFG, (uint8_t*)&_config, sizeof(_config));

  if (readChecksum() == calcChecksum((uint8_t*)&_config, sizeof(_config))) {
    _configLoaded = true;
    return &_config;
  }
  return nullptr;
}

bool GT911::writeConfig() {
  uint8_t checksum = calcChecksum((uint8_t*)&_config, sizeof(_config));
  if (_configLoaded && readChecksum() != checksum) { // Config is different
    writeBytes(GT911_REG_CFG, (uint8_t*)&_config, sizeof(_config));

    uint8_t buf[2] = { checksum, 1 };
    writeBytes(GT911_REG_CHECKSUM, buf, sizeof(buf));
    return true;
  }
  return false;
}

GTInfo* GT911::readInfo() {
  readBytes(GT911_REG_DATA, (uint8_t*)&_info, sizeof(_info));
  return &_info;
}

int GT911::GetHRES(){
  return (int)_info.xResolution;
}

int GT911::GetVRES(){
  return (int)_info.yResolution;
}

int8_t GT911::touched(uint8_t mode) {
  int contacts = readTouches();
  if (contacts > 0) {
    readTouchPoints();
  }
  return contacts;
}

GTPoint GT911::getPoint(uint8_t num) {
  return _points[num];
}

GTPoint *GT911::getPoints() {
  return _points;
}

void GT911::SetRotation(int rt) {
  if(rt == 0) _rotation = Rotate::_90;
  if(rt == 1) _rotation = Rotate::_180;
  if(rt == 2) _rotation = Rotate::_270;
  if(rt == 3) _rotation = Rotate::_0;
}

void GT911::setRotation(Rotate rotation) {
  _rotation = rotation;
}
