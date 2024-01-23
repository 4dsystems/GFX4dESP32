#include "gfx4desp32_touch.h"

gfx4desp32_touch::gfx4desp32_touch() : gfx4desp32() {}

gfx4desp32_touch::~gfx4desp32_touch() {}

/**********************************************************************/
/*!
  @brief      Get status of touch inversion on current display
  @returns    __TImode    0 - no inversion  1 inverted
*/
/**********************************************************************/
uint8_t gfx4desp32_touch::touch_GetInvertMode() {
  return __TImode;
}

/****************************************************************************/
/*!
  @brief  Return status of pen (touch)
  @note   returns 0 - NO_TOUCH, 1 - TOUCH_PRESSED, 2 - TOUCH_RELEASED
*/
/****************************************************************************/
int16_t gfx4desp32_touch::touch_GetPen() { return tPen; }

/****************************************************************************/
/*!
  @brief  Returns X position of touched area
*/
/****************************************************************************/
int16_t gfx4desp32_touch::touch_GetX() { return touchXpos; }

/****************************************************************************/
/*!
  @brief  Returns Y position of touched area
*/
/****************************************************************************/
int16_t gfx4desp32_touch::touch_GetY() { return touchYpos; }

/****************************************************************************/
/*!
  @brief  Returns last X position of touched area
  @note   Requirement for LVGL
*/
/****************************************************************************/
int16_t gfx4desp32_touch::touch_GetLastX() { return lasttouchXpos; }

/****************************************************************************/
/*!
  @brief  Returns last Y position of touched area
  @note   Requirement for LVGL
*/
/****************************************************************************/
int16_t gfx4desp32_touch::touch_GetLastY() { return lasttouchYpos; }

/****************************************************************************/
/*!
  @brief  Open4dGFX helper function
  @param  file4d - previously selected filename
  @param  alloxMAX - choose between allocating memory for amound in uSD of set
  MAX
  @note to save memory, allocMAX can be set to allocate enough for counted
  objects
*/
/****************************************************************************/
void gfx4desp32_touch::Open4dGFX(String file4d) {
  if (!gciImagesUsed) {
    gciImagesUsed = true;
    gciobjtouchenable = (uint8_t*)malloc(MAX_WIDGETS);
    memset(gciobjtouchenable, 0, MAX_WIDGETS);
    cdv = (uint8_t*)malloc(MAX_WIDGETS);
    gciobjframes = (uint16_t*)malloc(MAX_WIDGETS << 1);
    tuix = (int16_t*)malloc(MAX_WIDGETS << 1);
    tuiy = (int16_t*)malloc(MAX_WIDGETS << 1);
    tuiw = (int16_t*)malloc(MAX_WIDGETS << 1);
    tuih = (int16_t*)malloc(MAX_WIDGETS << 1);
    tuiImageIndex = (uint16_t*)malloc(MAX_WIDGETS << 1);
    tuiIndex = (uint32_t*)malloc(MAX_WIDGETS << 2);
    tuiExtra1 = (uint16_t*)malloc(MAX_WIDGETS << 1);
    tuiExtra2 = (uint16_t*)malloc(MAX_WIDGETS << 1);
  }
  _Open4dGFX(file4d, false);
  opgfx = 1;
}

/****************************************************************************/
/*!
  @brief  Open4dGFX helper function
  @param  file4d - previously selected filename
  @param  scan - pre count number of objects for memory saving allocation
*/
/****************************************************************************/
void gfx4desp32_touch::_Open4dGFX(String file4d, bool scan) {
  if (userImag)
    Close4dGFX();
  uint8_t strpos = 0;
  uint8_t gotchar = 0;
  uint8_t ofset = 0;
  gciobjnum = 0;
  if (file4d != "gfx4dDummy")
    GCItype = GCI_SYSTEM_USD;
  if (!(scan))
    imageTouchEnable(-1, false);
  String inputString;
  dat4d = file4d + ".dat";
  gci4d = file4d + ".gci";
#ifdef USE_LITTLEFS_FILE_SYSTEM
  if (GCItype == GCI_SYSTEM_USD) {
    dat4d = "/" + dat4d;
    gci4d = "/" + gci4d;
    userDat = LittleFS.open((char*)dat4d.c_str(), "r");
  }
  if (GCItype == GCI_SYSTEM_PROGMEM) {
    datArrayPos = 0;
    gciArrayPos = 0;
    gcidatArray = false;
    if (datArraySize > 0 && gciArraySize > 0) {
      gcidatArray = true;
    }
  }
#else
  if (GCItype == GCI_SYSTEM_USD) {
#ifdef USE_SDMMC_FILE_SYSTEM
    dat4d = "/" + dat4d;
    gci4d = "/" + gci4d;
    userDat = SD_MMC.open(dat4d);
#else
    userDat = uSD.open(dat4d);
#endif
  }
  if (GCItype == GCI_SYSTEM_PROGMEM) {
    datArrayPos = 0;
    gciArrayPos = 0;
    gcidatArray = false;
    if (datArraySize > 0 && gciArraySize > 0) {
      gcidatArray = true;
    }
  }
#endif
  if (userDat || gcidatArray) {
    char c;
    char prevc = 0;
    if (GCItype == GCI_SYSTEM_USD) {
      while (userDat.available() > 0) {
        c = userDat.read();
        if (c != 13 && c != 10) {
          strpos++;
          if (c == 34) {
            gotchar++;
          }
          if (gotchar == 2) {
            ofset = strpos;
            gotchar = 0;
          }
          inputString = inputString + char(c);
        }
        if (c == 13 || (c == 10 && prevc != 13)) {
          strpos = 0;
          String tempis = inputString;
          uint32_t tuindex = getIndexfromString(tempis, (2 + ofset));
          if (!(scan))
            tuiIndex[gciobjnum] = tuindex;
          getCoordfromString(tempis, (12 + ofset)); // dummy read required
          if (!(scan))
            tuix[gciobjnum] = xic;
          if (!(scan))
            tuiy[gciobjnum] = yic;
          inputString = "";
          gciobjnum++;
        }
        prevc = c;
      }
    }
    if (GCItype == GCI_SYSTEM_PROGMEM) {
      while (datArrayPos < datArraySize) {
        c = (char)DATarray[datArrayPos++];
        if (c != 13 && c != 10) {
          strpos++;
          if (c == 34) {
            gotchar++;
          }
          if (gotchar == 2) {
            ofset = strpos;
            gotchar = 0;
          }
          inputString = inputString + char(c);
        }
        if (c == 13 || (c == 10 && prevc != 13)) {
          strpos = 0;
          String tempis = inputString;
          uint32_t tuindex = getIndexfromString(tempis, (2 + ofset));
          if (!(scan))
            tuiIndex[gciobjnum] = tuindex;
          getCoordfromString(tempis, (12 + ofset)); // dummy read required
          if (!(scan))
            tuix[gciobjnum] = xic;
          if (!(scan))
            tuiy[gciobjnum] = yic;
          inputString = "";
          gciobjnum++;
        }
        prevc = c;
      }
    }
  }
  if (GCItype == GCI_SYSTEM_USD) {
    userDat.close();
  }
  if (GCItype == GCI_SYSTEM_PROGMEM) {
    datArrayPos = 0;
  }
  if (scan)
    return;
#ifdef USE_LITTLEFS_FILE_SYSTEM
  if (GCItype == GCI_SYSTEM_USD) {
    userImag = LittleFS.open((char*)gci4d.c_str(), "r");
  }
  if (GCItype == GCI_SYSTEM_PROGMEM) {
    gciArrayPos = 0;
  }
#else
  if (GCItype == GCI_SYSTEM_USD) {
#ifdef USE_SDMMC_FILE_SYSTEM
    userImag = SD_MMC.open((char*)gci4d.c_str());
#else 
    userImag = uSD.open((char*)gci4d.c_str());
#endif
  }
  if (GCItype == GCI_SYSTEM_PROGMEM) {
    gciArrayPos = 0;
  }
#endif

  uint32_t tIndex;

  for (int n = 0; n < gciobjnum; n++) {
    tIndex = tuiIndex[n];
    GCIseek(tIndex);
    if (!(scan))
      tuiw[n] = (GCIread() << 8) + GCIread();
    if (!(scan))
      tuih[n] = (GCIread() << 8) + GCIread();
    if (!(scan))
      cdv[n] = GCIread();
    int frms = GCIread();
    if (!(scan)) gciobjframes[n] = 0;
    if (frms != 0 && !(scan)) {
      gciobjframes[n] = (GCIread() << 8) + GCIread();
    }
  }
}

/****************************************************************************/
/*!
  @brief  close opened gci file
*/
/****************************************************************************/
void gfx4desp32_touch::Close4dGFX() {
  gfx4desp32::Close4dGFX();
  if (userImag) {
    imageTouchEnable(-1, false);
  }
}

/****************************************************************************/
/*!
  @brief  Enable or disable touch detection of widget
  @param  gcinum - nuber of widget in gci or ALL (-1) for all widgets
  @param  en - enable (true) disable (false)
*/
/****************************************************************************/
void gfx4desp32_touch::imageTouchEnable(int gcinum, boolean en) {
  if (opgfx) {
    if (gcinum < 0) {
      for (int n = 0; n < MAX_WIDGETS; n++) {
        gciobjtouchenable[n] =
          (gciobjtouchenable[n] & 0xf0) | ((uint8_t)en & 0x0f);
      }
    }
    else {
      gciobjtouchenable[gcinum] =
        (gciobjtouchenable[gcinum] & 0xf0) | ((uint8_t)en & 0x0f);
    }
  }
}

void gfx4desp32_touch::imageTouchEnable(int gcinum, boolean en, int type) {
  if (opgfx) {
    gciobjtouchenable[gcinum] = (type << 4) | ((uint8_t)en & 0x0f);
  }
}

void gfx4desp32_touch::imageTouchEnable(int gcinum, boolean en, int type,
  int frames, bool orientation, int gap1,
  int gap2, uint16_t tc) {
  if (opgfx) {
    gciobjtouchenable[gcinum] = (type << 4) | ((uint8_t)en & 0x0f);
    gciobjframes[gcinum] = frames;
    tuiExtra1[gcinum] = ((orientation == true) << 15) + (gap1 << 8) + gap2;
    tuiExtra2[gcinum] = tc;
  }
}

void gfx4desp32_touch::imageTouchEnableRange(int gcinumFrom, int gcinumTo,
  boolean en) {
  if (opgfx) {
    for (int n = gcinumFrom; n <= gcinumTo; n++) {
      gciobjtouchenable[n] =
        (gciobjtouchenable[n] & 0xf0) | ((uint8_t)en & 0x0f);
    }
  }
}

void gfx4desp32_touch::imageTouchEnableRange(int gcinumFrom, int gcinumTo,
  boolean en, int type) {
  if (opgfx) {
    for (int n = gcinumFrom; n <= gcinumTo; n++) {
      gciobjtouchenable[n] = (type << 4) | ((uint8_t)en & 0x0f);
    }
  }
}

/****************************************************************************/
/*!
  @brief  Draw frame from UserImages set
  @param  uisnb - UserImage ID
  @param  framenb - frame number
  @note pre - drawWidget function
*/
/****************************************************************************/
void gfx4desp32_touch::UserImages(uint16_t uisnb, int16_t framenb) {
  tuiImageIndex[uisnb] = framenb;
  boolean setemp = sEnable;
  ScrollEnable(false);
  if (framenb > (gciobjframes[uisnb] - 1) || framenb < 0) {
    outofrange(tuix[uisnb], tuiy[uisnb], tuiw[uisnb], tuih[uisnb]);
  }
  else {
    switch (gciobjtouchenable[uisnb] >> 4) {
    case SLIDER3IMAGE:
      UserImages3image(uisnb, framenb, gciobjframes[uisnb],
        ((tuiExtra1[uisnb] >> 15) == 1),
        (tuiExtra1[uisnb] >> 8) & 0x7f, tuiExtra1[uisnb] & 0x7f,
        tuiExtra2[uisnb]);
      break;
    case GAUGE2IMAGE:
      UserImages2image(uisnb, framenb, gciobjframes[uisnb],
        ((tuiExtra1[uisnb] >> 15) == 1),
        (tuiExtra1[uisnb] >> 8) & 0x7f, tuiExtra1[uisnb] & 0x7f);
      break;
    default:
      DrawWidget(tuiIndex[uisnb], tuix[uisnb], tuiy[uisnb], tuiw[uisnb],
        tuih[uisnb], framenb, 0, true, cdv[uisnb]);
    }
  }
  ScrollEnable(setemp);
}

int16_t gfx4desp32_touch::ImageTouchedAuto() {
  if (!(opgfx) || imageTouched() == -1)
    return -1;
  int itouched;
  bool shifted = false;
  uint8_t UpdateImages;
  UpdateImages = gciobjtouchenable[imageTouched()] >> 4;
  if (UpdateImages == 0 || UpdateImages > 4)
    return -1;
  if ((shift || caps) && UpdateImages == KEYPAD)
    shifted = true;
  if (UpdateImages == KEYPAD)
    decodeKP = true;
  int butcntrl = 0;
  int state = touch_GetPen();
  int temppressed = -1;

  if ((UpdateImages == TOGGLE) || (UpdateImages == TOGGLE4STATES)) {
    if (state == TOUCH_PRESSED) {
      itouched = imageTouched();
      if (itouched != pressed) {
        if (UpdateImages == TOGGLE4STATES) // if 4 state
        {
          if (tuiImageIndex[itouched] ==
            2) // dont use getImageValue(itouched) as we need more info
          {
            UserImages(itouched, 3); // show down pressed
          }
          else {
            UserImages(itouched, 1); // show up presses
          }
        }
        pressed = itouched;
        return -1;
      }
    }
    if (state == 2) {
      if (UpdateImages == TOGGLE4STATES) // if 4 state
      {
        if (tuiImageIndex[pressed] ==
          1) { // dont use getImageValue(itouched) as we need more info
          UserImages(pressed, 2);
        }
        else {
          UserImages(pressed, 0);
        }
      }
      else {
        if (getImageValue(pressed) == 0) {
          UserImages(pressed, 1);
        }
        else {
          UserImages(pressed, 0);
        }
      }
      temppressed = pressed;
      pressed = -1;
      return temppressed;
    }
  }
  if (UpdateImages == KEYPAD) {
    UpdateImages = DRAW_UPDOWN;
    butcntrl = ((shifted) * 2);
  }
  if (state == TOUCH_PRESSED) {
    itouched = imageTouched();
    if (itouched > -1 && itouched < getNumberofObjects()) {
      if (UpdateImages && itouched != pressed) {
        if (pressed > -1 && pressed < getNumberofObjects() &&
          gciobjframes[pressed] >= butcntrl)
          UserImages(pressed, butcntrl);
        if (gciobjframes[itouched] >= 1 + butcntrl) {
          UserImages(itouched, 1 + butcntrl);
        }
        else {
          UserImages(itouched, 1);
        }
      }
      if (gciobjframes[itouched] >= butcntrl)
        pressed = itouched;
    }
    return -1;
  }
  if (state == 2 && pressed > -1 && pressed < getNumberofObjects()) {
    if (UpdateImages == DRAW_UPDOWN && gciobjframes[pressed] >= butcntrl) {
      UserImages(pressed, butcntrl);
    }
    else {
      UserImages(pressed, 0);
    }
    temppressed = pressed;
    pressed = -1;
    return temppressed;
  }
  if (state == 0 && pressed > -1 && pressed < getNumberofObjects()) {
    if (UpdateImages == DRAW_UPDOWN && gciobjframes[pressed] >= butcntrl) {
      UserImages(pressed, butcntrl);
    }
    else {
      UserImages(pressed, 0);
    }
    temppressed = pressed;
    pressed = -1;
    return temppressed;
  }
  if (state == 0 || state == 2) {
    return -1;
  }
  return -1;
}

uint16_t gfx4desp32_touch::GetSliderValue(uint16_t ui, uint8_t axis,
  uint16_t uiv, uint16_t ming,
  uint16_t maxg) {
  int wpos = 0;
  int wsiz = 0;
  if (axis == HORIZONTAL_SLIDER) {
    wpos = uiv - tuix[ui] - ming;
    wsiz = tuiw[ui];
    if (wpos < 0)
      wpos = 0;
    else if (wpos > (wsiz - maxg))
      wpos = gciobjframes[ui] - 1;
    else
      wpos = (gciobjframes[ui] - 1) * wpos /
      (wsiz - maxg);
    return wpos;
  }
  if (axis == VERTICAL_SLIDER) {
    wpos = uiv - tuiy[ui] - ming;
    wsiz = tuih[ui];
    if (wpos < 0)
      wpos = gciobjframes[ui] - 1;
    else if (wpos > (wsiz - maxg))
      wpos = 0;
    else
      wpos = (gciobjframes[ui] - 1) -
      (gciobjframes[ui] - 1) * wpos /
      (wsiz - maxg);
    return wpos;
  }
  return wpos;
}

int gfx4desp32_touch::DecodeKeypad(int kpad, int kpress, byte* kbks,
  int8_t* kbck) {
  if (decodeKP == false)
    return -1;
  decodeKP = false;
  int key = -1;
  int kv = 0;
  int koff = 0;
  if (shift)
    koff = kbck[10];
  if (caps)
    koff = koff + (2 * kbck[10]);
  if (ctrl)
    koff = (3 * kbck[10]);
  bool skip = false;
  if (kpress > -1) {
    key = kbks[kpress - kpad - 1 + koff];
    kv = (kpress - kpad - 1) % kbck[10];
    if (key == 0xff && !shift && (kv == kbck[5] || kv == kbck[6])) {
      if (!caps)
        UserImages(kpad, 1);
      if (caps)
        UserImages(kpad, 0);
      shift = true;
      skip = true;
      ctrl = false;
    }
    if (key == 0xff && shift && (kv == kbck[5] || kv == kbck[6]) && !skip) {
      if (!caps)
        UserImages(kpad, 0);
      if (caps)
        UserImages(kpad, 1);
      shift = false;
      ctrl = false;
    }
    skip = false;
    if (key == 0xff && kv == kbck[9] && !caps) {
      UserImages(kpad, 1);
      caps = true;
      skip = true;
      ctrl = false;
    }
    if (key == 0xff && kv == kbck[9] && caps && !skip) {
      UserImages(kpad, 0);
      caps = false;
      ctrl = false;
    }
    skip = false;
    if (key == 0xff && (kv == kbck[7] || kv == kbck[8]) && ctrl == false) {
      if (!caps)
        UserImages(kpad, 0);
      if (caps)
        UserImages(kpad, 1);
      ctrl = true;
      shift = false;
      skip = true;
    }
    if (key == 0xff && (kv == kbck[7] || kv == kbck[8]) && !skip) {
      ctrl = false;
    }
  }
  if (key == 0xff)
    key = -1;
  if (key != -1 && shift) {
    if (!caps)
      UserImages(kpad, 0);
    if (caps)
      UserImages(kpad, 1);
    shift = false;
    ctrl = false;
  }
  return key;
}

void gfx4desp32_touch::ResetKeypad() {
  shift = false;
  caps = false;
  ctrl = false;
}

bool gfx4desp32_touch::KeypadStatus(int keyType) {
  if (keyType == SHIFT)
    return shift;
  if (keyType == CAPSLOCK)
    return caps;
  if (keyType == CTRL)
    return ctrl;
  return false;
}

int gfx4desp32_touch::imageTouched() {
  if (opgfx) {
    return gciobjtouched;
  }
  else {
    return -1;
  }
}

uint8_t gfx4desp32_touch::CheckButtons(void) {
  // butchnge = true;
  touch_Update();
  int ret = 255;
  uint8_t tpen = touch_GetPen();
  boolean skip = false;
  uint16_t tx = touch_GetX();
  uint16_t ty = touch_GetY();
  for (int n = 0; n < 128; n++) {
    if (bstat[n] == 1 && (tpen == NOTOUCH || tpen == TOUCH_RELEASED)) {
      bstat[n] = 0;
      ButtonUp(n);
      skip = true;
    }
    if (bactive[n] && tpen == TOUCH_PRESSED && skip == false) {
      if (tx > bposx[n] && tx < (bposx[n] + bposw[n]) && ty > bposy[n] &&
        ty < (bposy[n] + bposh[n])) {
        if (bstat[n] != 1) {
          bstat[n] = 1;
          ButtonDown(n);
          oldbut = n;
          ret = n;
        }
      }
    }
  }
  oldtpen = tpen;
  return ret;
}

int gfx4desp32_touch::SpriteTouched() {
  int tresp = -1;
  int stx;
  int sty;
  if (touch_Update()) {
    if (touch_GetPen() == 1) {
      stx = touch_GetX();
      sty = touch_GetY();
      for (int nt = numSprites - 1; nt > -1; nt--) {
        if (GetSprite(nt, SPRITE_ACTIVE) == 1) {
          if (stx >= GetSprite(nt, SPRITE_X) &&
            stx <= (GetSprite(nt, SPRITE_X) + GetSprite(nt, SPRITE_WIDTH)) &&
            sty >= GetSprite(nt, SPRITE_Y) &&
            sty <= (GetSprite(nt, SPRITE_Y) + GetSprite(nt, SPRITE_HEIGHT))) {
            tresp = nt;
            break;
          }
        }
      }
    }
  }
  return tresp;
}

int16_t gfx4desp32_touch::imageAutoSlider(uint16_t ui, uint8_t axis,
  uint16_t uiv, uint16_t ming,
  uint16_t maxg) {
  int wpos;
  int wsiz;
  if (axis == HORIZONTAL_SLIDER) {
    wpos = uiv - tuix[ui] - ming;
    wsiz = tuiw[ui];
  }
  else // if (axis == VERTICAL_SLIDER)
  {
    wpos = uiv - tuiy[ui] - ming;
    wsiz = tuih[ui];
  }
  // use gciobjframes[ui] instead of -1 to ensure even spread of values with
  // integer
  wpos = map(wpos, 0, wsiz - ming - maxg, 0,
    gciobjframes[ui]); // because using -ve mapping for vertical gives
  // incorrect result
  wpos = constrain(wpos, 0,
    gciobjframes[ui] -
    1); // constrain after map else 'max' could be 1 high

  if (axis == VERTICAL_SLIDER)
    wpos = gciobjframes[ui] - 1 -
    wpos; // because vertical slider runs in other direction
  UserImages(ui, wpos);
  return wpos;
}

int16_t gfx4desp32_touch::imageAutoKnob(int hndl, int uix, int uiy, int minarc,
  int maxarc, int ming, int maxg) {
  int degdiff = maxarc - minarc;
  int posit;
  int deg =
    XYposToDegree(uix - (tuix[hndl] + (tuiw[hndl] >> 1)),
      uiy - (tuiy[hndl] + (tuih[hndl] >> 1))); // x - CentreX
  if (deg < minarc) // anything in the first 'dead zone' is minvalue
    deg = 0;
  else {
    if (deg > maxarc) // anything in the last 'dead zone' is maxvalue
      deg = degdiff;
    else
      deg -= minarc; // offset by -baseangle
  }
  posit = deg * maxg / degdiff; // convert deg to position
  UserImages(hndl, posit);      // Knob1
  return posit;
}

int16_t gfx4desp32_touch::getImageValue(uint16_t ui) {
  if ((gciobjtouchenable[ui] >> 4) == TOGGLE4STATES)
    if (tuiImageIndex[ui])
      return 1;
    else
      return 0;
  else
    return tuiImageIndex[ui];
}

void gfx4desp32_touch::UserImageHide(int hndl, uint16_t color) {
  if (hndl > 0) {
    RectangleFilled(tuix[hndl], tuiy[hndl], tuiw[hndl], tuih[hndl], color);
    imageTouchEnable(hndl, false);
  }
  else {
    for (int n = 0; n > MAX_WIDGETS; n++) {
      RectangleFilled(tuix[n], tuiy[n], tuiw[n], tuih[n], color);
      imageTouchEnable(n, false);
    }
  }
}

void gfx4desp32_touch::UserImageHideBG(int hndl, int objBG) {
  if (hndl > 0) {
    UserImageDR(objBG, tuix[hndl], tuiy[hndl], tuiw[hndl], tuih[hndl],
      tuix[hndl], tuiy[hndl]);
    imageTouchEnable(hndl, false);
  }
  else {
    for (int n = 0; n > MAX_WIDGETS; n++) {
      UserImageDR(objBG, tuix[n], tuiy[n], tuiw[n], tuih[n], tuix[n], tuiy[n]);
      imageTouchEnable(n, false);
    }
  }
}