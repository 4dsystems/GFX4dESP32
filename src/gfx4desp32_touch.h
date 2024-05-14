#ifndef __GFX4DESP32_TOUCH__
#define __GFX4DESP32_TOUCH__

#include "gfx4desp32.h"

#include <Preferences.h>

class gfx4desp32_touch : virtual public gfx4desp32 {
protected:
    long touchTime;
    uint8_t* gciobjtouchenable;
    int16_t gciobjtouched;
    int16_t pressed = -1;
    uint8_t oldbut;
    bool decodeKP;

    int8_t oldtpen;

    uint8_t tPen;
    uint8_t lasttPen;
    int16_t touchXpos;
    int16_t touchYpos;
    int16_t lasttouchXpos;
    int16_t lasttouchYpos;
    bool _TouchEnable;

    bool touchYswap;
    bool touchXswap;

    bool touchFirstEnable = true;

    Preferences preferences;

public:
    gfx4desp32_touch();
    ~gfx4desp32_touch();

    uint8_t touch_GetInvertMode();

    // these should be in rgb/spi touch files
    virtual void touch_Set(uint8_t mode) = 0;
    virtual bool touch_Update() = 0;

    int16_t touch_GetPen();
    int16_t touch_GetX();
    int16_t touch_GetY();
    int16_t touch_GetLastX();
    int16_t touch_GetLastY();

    // Graphics related functions

    using gfx4desp32::Open4dGFX;

    virtual void Open4dGFX(String file4d) override;
    virtual void _Open4dGFX(String file4d, bool scan) override;
    virtual void Close4dGFX() override;

    using gfx4desp32::UserImages;

    virtual void UserImages(uint16_t uisnb, int16_t framenb) override;
    virtual int16_t getImageValue(uint16_t ui) override;
    virtual void UserImageHide(int hndl, uint16_t color) override;
    virtual void UserImageHideBG(int hndl, int objBG) override;

    void imageTouchEnable(int gcinum, boolean en);
    void imageTouchEnable(int gcinum, boolean en, int type);
    void imageTouchEnableRange(int gcinumFrom, int gcinumTo, boolean en);
    void imageTouchEnableRange(int gcinumFrom, int gcinumTo, boolean en,
        int type);
    void imageTouchEnable(int gcinum, boolean en, int type, int frames,
        bool orientation, int gap1, int gap2, uint16_t tc = 0);
    int16_t ImageTouchedAuto();
    int imageTouched();

    int16_t imageAutoSlider(uint16_t ui, uint8_t axis, uint16_t uiv,
        uint16_t ming, uint16_t maxg);
    int16_t imageAutoKnob(int hndl, int uix, int uiy, int minarc, int maxarc,
        int ming, int maxg);

    uint8_t CheckButtons(void);

    uint16_t GetSliderValue(uint16_t ui, uint8_t axis, uint16_t uiv,
        uint16_t ming, uint16_t maxg);

    int DecodeKeypad(int kpad, int kpress, byte* kbks, int8_t* kbck);
    void ResetKeypad();
    bool KeypadStatus(int keyType);

    int SpriteTouched();
};

#endif // __GFX4DESP32_TOUCH__