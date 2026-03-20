//
// NB! This is a file generated from the .4Dino file, changes will be lost
//     the next time the .4Dino file is built
//
#include "gfx4desp32_gen4_ESP32_43CT.h"

gfx4desp32_gen4_ESP32_43CT gfx = gfx4desp32_gen4_ESP32_43CT();

#include "MidiKeyboardAAConst.h"    // Note. This file will not be created if there are no generated graphics

/************************************************************************************************************
* A simple Midi Keyboard for the 4.3" to 9.0" 800 x 480 displays. The Midi Keyboard can be configured via   *
* on-screen setup menu. Output is best with BLE using the Windows MidiBerry App. It is also possible to use *
* Midi serial bridges via the USB cable such as hairless or use the gen4-UPA for a hardware serial          *
* connection to a MIDI device using a suitable adapter or circuit. The demo was designed to show how to     *
* create a nice graphical interface using only built in drawing functions, frame buffers and no uSD card.   *
************************************************************************************************************/

#include <src/BLEMIDI_Transport.h>
#include <src/hardware/BLEMIDI_ESP32.h>

BLEMIDI_CREATE_INSTANCE("MIDI_KEYBOARD_4D", MIDI);

#define    MIDI_NOTE_ON    144
#define    MIDI_NOTE_OFF   128
#define    PROGRAM_CHANGE  192
#define    CHANNEL_MODE    176
#define    MIDI_TIMEOUT    300

#define    MAX_PENS        7
#define    CTP_ADRS        0x70
#define    CTP_DATA_LEN    1 + 6 * MAX_PENS

const uint8_t SIcon[1018] = {
#include "SettingsIcon.h"                                  // settings icon image array
};

const char *tones = "                 \0" \
                    "Acoustic G Piano \0" \
                    "Acoustic Br Piano\0" \
                    "Elect Grand Piano\0" \
                    "Honky-tonk Piano \0" \
                    "Electric Piano 1 \0" \
                    "Electric Piano 2 \0" \
                    "Harpsichord      \0" \
                    "Clavi            \0" \
                    "Celesta          \0" \
                    "Glockenspiel     \0" \
                    "Music Box        \0" \
                    "Vibraphone       \0" \
                    "Marimba          \0" \
                    "Xylophone        \0" \
                    "Tubular Bells    \0" \
                    "Dulcimer         \0" \
                    "Drawbar Organ    \0" \
                    "Percussive Organ \0" \
                    "Rock Organ       \0" \
                    "Church Organ     \0" \
                    "Reed Organ       \0" \
                    "Accordion        \0" \
                    "Harmonica        \0" \
                    "Tango Accordion  \0" \
                    "Acoustic Guitar N\0" \
                    "Acoustic Guitar S\0" \
                    "Electric Guitar J\0" \
                    "Electric Guitar C\0" \
                    "Electric Guitar M\0" \
                    "Overdriven Guitar\0" \
                    "Distortion Guitar\0" \
                    "Guitar Harmonics \0" \
                    "Acoustic Bass    \0" \
                    "Electric Bass fin\0" \
                    "Electric Bass pck\0" \
                    "Fretless Bass    \0" \
                    "Slap Bass 1      \0" \
                    "Slap Bass 2      \0" \
                    "Synth Bass 1     \0" \
                    "Synth Bass 2     \0" \
                    "Violin           \0" \
                    "Viola            \0" \
                    "Cello            \0" \
                    "Contrabass       \0" \
                    "Tremolo Strings  \0" \
                    "Pizzicato Strings\0" \
                    "Orchestral Harp  \0" \
                    "Timpani          \0" \
                    "String Ensemble 1\0" \
                    "String Ensemble 2\0" \
                    "Synth Strings 1  \0" \
                    "Synth Strings 2  \0" \
                    "Choir Aahs       \0" \
                    "Voice Oohs       \0" \
                    "Synth Voice      \0" \
                    "Orchestra Hit    \0" \
                    "Trumpet          \0" \
                    "Trombone         \0" \
                    "Tuba             \0" \
                    "Muted Trumpet    \0" \
                    "French Horn      \0" \
                    "Brass Section    \0" \
                    "Synth Brass 1    \0" \
                    "Synth Brass 2    \0" \
                    "Soprano Sax      \0" \
                    "Alto Sax         \0" \
                    "Tenor Sax        \0" \
                    "Baritone Sax     \0" \
                    "Oboe             \0" \
                    "English Horn     \0" \
                    "Bassoon          \0" \
                    "Clarinet         \0" \
                    "Piccolo          \0" \
                    "Flute            \0" \
                    "Recorder         \0" \
                    "Pan Flute        \0" \
                    "Blown bottle     \0" \
                    "Shakuhachi       \0" \
                    "Whistle          \0" \
                    "Ocarina          \0" \
                    "Lead 1 square    \0" \
                    "Lead 2 sawtooth  \0" \
                    "Lead 3 calliope  \0" \
                    "Lead 4 chiff     \0" \
                    "Lead 5 charang   \0" \
                    "Lead 6 voice     \0" \
                    "Lead 7 fifths    \0" \
                    "Lead 8 bass+lead \0" \
                    "Pad 1 new age    \0" \
                    "Pad 2 warm       \0" \
                    "Pad 3 polysynth  \0" \
                    "Pad 4 choir      \0" \
                    "Pad 5 bowed      \0" \
                    "Pad 6 metallic   \0" \
                    "Pad 7 halo       \0" \
                    "Pad 8 sweep      \0" \
                    "FX 1 rain        \0" \
                    "FX 2 soundtrack  \0" \
                    "FX 3 crystal     \0" \
                    "FX 4 atmosphere  \0" \
                    "FX 5 brightness  \0" \
                    "FX 6 goblins     \0" \
                    "FX 7 echoes      \0" \
                    "FX 8 sci-fi      \0" \
                    "Sitar            \0" \
                    "Banjo            \0" \
                    "Shamisen         \0" \
                    "Koto             \0" \
                    "Kalimba          \0" \
                    "Bag pipe         \0" \
                    "Fiddle           \0" \
                    "Shanai           \0" \
                    "Tinkle Bell      \0" \
                    "Agogo            \0" \
                    "Steel Drums      \0" \
                    "Woodblock        \0" \
                    "Taiko Drum       \0" \
                    "Melodic Tom      \0" \
                    "Synth Drum       \0" \
                    "Reverse Cymbal   \0" \
                    "Guitar Fret Noise\0" \
                    "Breath Noise     \0" \
                    "Seashore         \0" \
                    "Bird Tweet       \0" \
                    "Telephone Ring   \0" \
                    "Helicopter       \0" \
                    "Applause         \0" \
                    "Gunshot          \0" \
                    "                 \0";

char tonename[20];

bool initialk = true;
bool initialt = true;
bool initialbc[20];
int tonelist[17];
int lastTone = -1;
int toneXY[2];
int keyco[200];
int keys[24] = {1, 4, 2, 4, 3, 1, 4, 2, 4, 2, 4, 3};
int lastkey = -1;
int posn;
int channel = 0;
int bdown = 0;
int pbend = 0;
int pc;
int oldpc;
int octave[16];
int velocity[16];
int toneVal[16];
int octv = 4;
int bdowno = -1;
int pressed;
int pressed1;
String buttonString = "        ";
int touchBut[50];
bool yInvert;
uint8_t gCTPData[CTP_DATA_LEN];                 // multi touch x y buffer
int schk;
int echk;
int bw;
int keynum1[14] = {0, 2, 4, 5, 7, 9, 11, 12, 14, 16, 17, 19, 21, 23};
int keynum2[14] = {1, 3, -1, 6, 8, 10, -1, 13, 15, -1, 18, 20, 22, -1};
int keynum3[14] = {0, -1, 4, 5, -1, -1, 11, 12, -1, 16, 17, -1, -1, 23};
int lasttouchBut[24];
int kpad[4] = {567, 7, 773, 213};               // channel keypad buttons touch area
int toneb[4] = {260, 170, 370, 197};            // tone buttons touch area
int octb[4] = {380, 170, 495, 197};             // octave buttons touch area
int slidr[4] = {508, 8, 553, 212};              // velocity touch area
int mrot[4] = {22, 10, 228, 216};               // tone knob touch area
int smenu[4] = {0, 0, 40, 40};                  // settings icon touch area
int backb[4] = {600, 40, 720, 150};             // settings back button touch area
int set0a[4] = {565, 113, 595, 143};            // MIDI output BLE select option touch area
int set1a[4] = {565, 163, 595, 193};            // MIDI output usb select option touch area
int set1b[4] = {565, 213, 595, 243};            // MIDI output hardware serial select option touch area
int set2a[4] = {565, 303, 595, 333};            // USB baud 115200 select option touch area
int set2b[4] = {565, 353, 595, 383};            // USB baud 31250 select option touch area
int set3a[4] = {645, 303, 675, 333};            // hardware serial baud 115200 select option touch area
int set3b[4] = {645, 353, 675, 383};            // hardware serial baud 31250 select option touch area
int notesoff[4] = {482, 56, 624, 95};           // all notes off - not supported
int kpadv;
int lastkpad;
String messageOP = "                    ";      // Empty string to message construction
int xyo[2];                                     // orbit x y array
bool settingsMenu;                              // settings menu active flag
bool leftSettings;                              // settings just left flag
bool settings[4] = {false, false, false, false};       // initial settings
bool lastsettings[4] = {false, false, false, false};   // settings backup to compare for changes
int Serialrec;
bool BLEset;
bool progchange;
int BLE = 0;


void setup() {
  // put your setup code here, to run once:
  for (int sv = 0; sv < 16; sv ++) {
    velocity[sv] = 127;
    octave[sv] = 4;
  }
  gfx.DisplayControl(DISP_CTRL_BOUNCE_BUFFER_SIZE, BOUNCE_BUFF_OPTIMAL);
  gfx.begin(16); // Adjust this value for PCLK should frame shifting occur when selecting BLE
  gfx.Cls();
  gfx.ScrollEnable(true);
  gfx.BacklightOn(true);
  gfx.Orientation(LANDSCAPE);
  gfx.SmoothScrollSpeed(0);
  gfx.TextWrap(OFF);
  gfx.TextColor(WHITE, BLACK); gfx.Font(2);  gfx.TextSize(1);
  gfx.ClipWindow(0, 0, 550, 240);
  for (int n = 0; n < 17; n++) {
    tonelist[n] = 1;
  }
  showForm();
  Wire.begin(GEN4_I2C_SDA, GEN4_I2C_SCL, 400000);
  yInvert = gfx.touch_GetInvertMode();
  bw = 56;
  Serial.begin(115200);
  Serial0.begin(115200);
  UpdateVelocity();
  UpdateTone();
  if (settings[3]) {
    MIDI.begin(MIDI_CHANNEL_OMNI);
    BLEset = true;
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  if (pc != oldpc) {
    if(settingsMenu){
      gfx.DrawToframebuffer(2);
      MIDImessage(PROGRAM_CHANGE + channel, pc, 0);
      ShowSettings();
    } else {
      MIDImessage(PROGRAM_CHANGE + channel, pc, 0);
    }
    Serial0.write(PROGRAM_CHANGE + channel);
    Serial0.write(pc);
    oldpc = pc;
  }
  CheckThru();
  CheckPC();
  TouchMulti();
}

void showForm() {
  pianoKeyboard(0, 0, 264, 800, 216, 0xC5EF, BLACK, 14);
  Panel(0, 217, 800, 47, BLACK, BLACK, 2, 1, 1, 1, 1, 0x1, 0x1, 20, 30, 5);
  Panel(8, 1, 777, 241, BLACK, BLACK, 3, 10, 10, 10, 10, 0x1, 0x0, 35, 0, 5);
  Panel(17, 4, 216, 216, BLACK, RED, 6, 108, 108, 108, 108, 0x0, -1, 40, 30, 0);
  Panel(260, 8, 235, 156, BLACK, 0x3000, 3, 7, 7, 7, 7, 0x1, 0x1, 35, 0, 0);
  for (int n = 0; n < 16; n++) {
    initialbc[n] = true;
    ChannelButton((n == 0), n);
  }
  gfx.TextColor(0x8410, BLACK); gfx.Font(2);
  gfx.MoveTo(568, 220);
  gfx.print("Channel");
  gfx.MoveTo(22, 220);
  gfx.print("Tone");
  gfx.MoveTo(260, 220);
  gfx.print("4D Midi Keyboard");
  gfx.MoveTo(519, 220);
  gfx.print("Vel.");
  ToneOctaveButton(0, 0);
  ToneOctaveButton(0, 1);
  ToneOctaveButton(0, 2);
  ToneOctaveButton(0, 3);
  gfx.MoveTo(268, 75);
  gfx.TextColor(RED, 0x3000);
  gfx.print("Channel "); gfx.print(channel + 1); gfx.print(" ");
  gfx.HlineD(92, 268, 445, RED);
  gfx.MoveTo(268, 20);
  gfx.print("Octave "); gfx.print(octv); gfx.print(" ");
  gfx.HlineD(37, 268, 445, RED);
  SettingsIcon();
  Volume(127);
  Tone(0);
  UpdateTone();
}

void pianoKeyboard(int val, int kpx, int kpy, int kpw, int kph, uint16_t kpcol1, uint16_t kpcol2, int kpval1) {
  gfx.StartWrite();
  int numKeys = 0; int kval = 0; int kw = 0; int kh1 = 0; int kh2 = 0; int n = 0; int kc = 0; int kt = 0;
  int kbw = 0; int kcr = 0; int lastval = 0; int dark = 0; int col1 = 0; int col2 = 0; int col3 = 0; int l1 = 0;
  int l2 = 0; int l3 = 0; int l4 = 0; int tx = 0; int ty = 0; int tw = 0; int th = 0; int t1 = 0; int t2 = 0;
  int updown = 0; int aval = 0;
  lastval = 0;
  updown = val >> 7;
  aval = val & 0x7f;
  if (kpval1 != 0) kw = (kpw / kpval1) - 1;
  kh1 = kph * 3 / 5;
  kh2 = kph * 2 / 5;
  col1 = kpcol1;
  col2 = kpcol2;
  kbw = (kw * 3) >> 2;
  numKeys = kpval1;
  for (n = 0; n < numKeys; n++) {
    kt = keys[kcr % 12];
    l1 = 35; l2 = 30; l3 = 0; l4 = 0;
    t1 = 1; t2 = 1;
    col3 = gfx.Grad(0, 1, l2, kh1 + 1, 0, col1);
    if ((initialk || aval == kcr || aval == -1)) {
      if (!(initialk) && aval == kcr && updown == 1) {
        l3 = 0; l4 = 0; l1 = 0; l2 = 0;
        t1 = 0; t2 = 0;
        col3 = gfx.Grad(0, 1, l2, kh1 + 1, kh1, col1);
      }
      if (kt == 1) {
        gfx.gradientShape(0, 2, kpx + (kc * kw), kpy, kw - (kbw >> 1), kh1 + 1, 2, 2, 2, 2, 0, col1, 1, l1, col1, 1, l2, 0);
      }
      if (kt == 2) {
        gfx.gradientShape(0, 2, kpx + (kc * kw) + kw - (kw - (kbw >> 1)) + 1, kpy, kw - kbw - 1, kh1 + 1, 2, 2, 2, 2, 0, col1, 1, l1, col1, 1, l2, 0);
      }
      if (kt == 3) {
        gfx.gradientShape(0, 2, kpx + (kc * kw) + kw - (kw - (kbw >> 1)) + 1, kpy, kw - (kbw >> 1) - 2, kh1 + 1, 2, 2, 2, 2, 0, col1, 1, l1, col1, 1, l2, 0);
      }
      if (kt == 1 || kt == 2 || kt == 3) {
        tx = kpx + (kc * kw); ty = kpy + kh1 - 1; tw = kw - 1; th = kh2 + 1;
        gfx.gradientShape(0, 2, tx, ty, tw, th, 2, 2, 2, 2, 0, col3, 0, l3, col3, 0, l4, 0);
        if (initialk) keyco[kcr << 2] = tx; keyco[(kcr << 2) + 1] = /*ty*/kpy; keyco[(kcr << 2) + 2] = tx + tw - 1; keyco[(kcr << 2) + 3] = ty + th - 1;
      }
      if (kt == 4) {
        tx = kpx + (kc * kw) - (kbw >> 1) + 1; ty = kpy; tw = kbw - 1; th = kh1 - 2;
        gfx.gradientShape(0, 2, tx, ty, tw, th, 2, 2, 2, 2, dark, col2, t1, 30, col2, t2, 20, 0);
        if (initialk) keyco[kcr << 2] = tx; keyco[(kcr << 2) + 1] = ty; keyco[(kcr << 2) + 2] = tx + tw - 1; keyco[(kcr << 2) + 3] = ty + th - 1;
      }
    }
    if (kt == 4) {
      n --;
      kc --;
    }
    kc ++;
    kcr ++;
  }
  initialk = false;
  gfx.EndWrite();
  return;

}

void Panel(int x, int y, int w, int h, uint16_t orCol, uint16_t irCol, int owidth, int radTopLeft, int radTopRight, int radBottomLeft, int radBottomRight, int sr3, int sr4, int gl1, int gl2, int dark) {
  gfx.gradientShapeAA(0, owidth, x, y, w, h, radTopLeft, radTopRight, radBottomLeft, radBottomRight, dark, orCol, sr3, gl1, irCol, sr4, gl2, 0);
}

void ChannelButton(int val, int button) {
  int x, y;
  x = ((button % 4) * 52) + 568;
  y = ((button / 4) * 52) + 8;
  if (initialbc[button]) {
    gfx.gradientShapeAA(0, 4, x, y, 50, 50, 16, 16, 16, 16, 0, BLACK, 0, 26, BLACK, 1, 20, 0);
  }
  if (val == 1) {
    gfx.gradientShapeAA(0, 2, x + 6, y + 6, 38, 38, 11, 11, 11, 11, 0, RED, 0, 30, BLACK, 0, 13, 0);
    gfx.AlphaBlendLevel(230);
    gfx.AlphaBlend(ON);
    gfx.TextColor(0xad75, 0xad75); gfx.Font(2);
    gfx.MoveTo(x + (21 - (4 * ((button + 1) > 9))), y + 17);
    if ((button + 1) == 10) {
      gfx.print("1O");
    } else {
      gfx.print(button + 1);
    }
    gfx.AlphaBlend(OFF);
  } else {
    gfx.gradientShapeAA(0, 2, x + 6, y + 6, 38, 38, 11, 11, 11, 11, 0, 0x3800, 0, 30, BLACK, 0, 26, 0);
    gfx.AlphaBlendLevel(230);
    gfx.AlphaBlend(ON);
    gfx.TextColor(0xd6ba, 0xd6ba); gfx.Font(2);
    gfx.MoveTo(x + (21 - (4 * ((button + 1) > 9))), y + 17);
    if ((button + 1) == 10) {
      gfx.print("1O");
    } else {
      gfx.print(button + 1);
    }
    gfx.AlphaBlend(OFF);
  }

  initialbc[button] = false;
}

void Volume(int val) {
  int x = 508;
  int y = 8;
  gfx.DrawToframebuffer(1);
  gfx.RectangleFilled(508, 8, 552, 211, BLACK);
  int pos = 181 - ((((160 * 100) / 128) * val) / 100);
  gfx.gradientShape(1, 4, x + 14, y + 4, 26, pos, 13, 13, 2, 2, 0, BLACK, 6, 13, BLACK, 4, 26, 0);
  gfx.gradientShape(1, 4, x + 14, y + pos, 26, 198 - pos, 2, 2, 13, 13, 0, BLACK, 6, 13, RED, 4, 26, 0);
  gfx.gradientShapeAA(0, 5, x + 7, y + pos - 20, 40, 40, 20, 20, 20, 20, 0, BLACK, 0, 30, RED, 1, 45, 0);
  gfx.DrawToframebuffer(0);
  gfx.DrawFrameBufferArea(1, 508, 8, 552, 211);
  int posg = 119 * val / 127;
  int d;
  for (int n = 0; n < 119; n ++) {
    d = n % 3;
    if (d == 0 || d == 1) {
      if (n <= posg && posg != 0) {
        gfx.Hline(456, 136 - n, 28, RED);
      } else {
        gfx.Hline(456, 136 - n, 28, 0x7000);
      }
    }
  }
}

void Tone(int val) {
  if (initialt) {
    Panel(22, 10, 206, 206, RED, BLACK, 6, 103, 103, 103, 103, 0x0, 0x0, 40, 50, 0);
    Panel(76, 64, 98, 98, BLACK, BLACK, 2, 49, 49, 49, 49, 0x1, 0x0, 63, 0, 0);
    gfx.DrawToframebuffer(1);
    Panel(22, 10, 206, 206, RED, BLACK, 6, 103, 103, 103, 103, 0x0, 0x0, 40, 50, 0);
    Panel(76, 64, 98, 98, BLACK, BLACK, 2, 49, 49, 49, 49, 0x1, 0x0, 63, 0, 0);
    gfx.DrawToframebuffer(0);
  }
  gfx.MoveTo(92, 99);
  gfx.FontStyle(DOTMATRIXSQUARE);
  gfx.TextSize(4);
  gfx.Font(FONT1);
  gfx.TextColor(RED, BLACK);
  val ++;
  if (val < 10) {
    gfx.print("  "); gfx.print(val);
  } else if (val < 100) {
    gfx.print(" "); gfx.print(val);
  } else {
    gfx.print(val);
  }
  val --;
  gfx.FontStyle(SOLID);
  gfx.TextSize(1);
  gfx.Font(FONT1);
  initialt = false;
  if (val != lastTone) {
    gfx.StartWrite();
    gfx.MoveTo(125, 113);
    gfx.Orbit(((((270 * 100) / 128) * lastTone) / 100) + 225, 72, toneXY);
    gfx.DrawFrameBufferArea(1, toneXY[0] - 15, toneXY[1] - 15, toneXY[0] + 15, toneXY[1] + 15);
    gfx.MoveTo(125, 113);
    gfx.Orbit(((((270 * 100) / 128) * val) / 100) + 225, 72, toneXY);
    gfx.gradientShapeAA(0, 3, toneXY[0] - 15, toneXY[1] - 15, 30, 30, 15, 15, 15, 15, (toneXY[1] - 36) / 10, BLACK, 1, 45, RED, 1, 45, 0);
    gfx.EndWrite();
  }
  lastTone = val;
}

void ToneOctaveButton(int val, int button) {
  int x, s;
  switch (button) {
    case 0:
      x = 260;
      s = 0;
      buttonString = "Tone -";
      break;
    case 1:
      x = 316;
      s = 1;
      buttonString = "Tone +";
      break;
    case 2:
      x = 384;
      s = 0;
      buttonString = "Oct -";
      break;
    case 3:
      x = 440;
      s = 1;
      buttonString = "Oct +";
      break;
  }
  if (val == 1) {
    if (s == 0) {
      gfx.gradientShapeAA(0, 2, x, 172, 55, 30, 15, 2, 15, 2, 14, 0x2965, 0, 45, BLACK, 0, 45, 0);
    } else {
      gfx.gradientShapeAA(0, 2, x, 172, 55, 30, 2, 15, 2, 15, 14, 0x2965, 0, 45, BLACK, 0, 45, 0);
    }
    gfx.TextColor(0xad75, 0xad75); gfx.Font(1);
    gfx.MoveTo(x + 28 + ((s == 0) * 3) - (s * 2) - 16 - (3 * (button < 2)), 183);
    gfx.print(buttonString);
  } else {
    if (s == 0) {
      gfx.gradientShapeAA(0, 2, x, 172, 55, 30, 15, 2, 15, 2, 5, 0x2965, 0, 45, BLACK, 0, 45, 0);
    } else {
      gfx.gradientShapeAA(0, 2, x, 172, 55, 30, 2, 15, 2, 15, 5, 0x2965, 0, 45, BLACK, 0, 45, 0);
    }
    gfx.TextColor(0xd6ba, 0xd6ba); gfx.Font(1);
    gfx.MoveTo(x + 28 + ((s == 0) * 3) - (s * 2) - 16 - (3 * (button < 2)), 183);
    gfx.print(buttonString);
  }
  gfx.Font(2);
  gfx.Opacity(OPAQUE);
}

void UpdateTone() {
  int t = (toneVal[channel] + 1) * 18;
  Tone(toneVal[channel]);
  gfx.Font(FONT2);
  gfx.MoveTo(268, 98);
  gfx.TextColor(RED, 0x3000);
  writeTonename(t - 18);
  gfx.MoveTo(268, 115);
  gfx.TextColor(0x3000, RED);
  writeTonename(t);
  gfx.MoveTo(268, 132);
  gfx.TextColor(RED, 0x3000);
  writeTonename(t + 18);
  gfx.Font(FONT2);
}

void writeTonename(int pos) {
  for (int n = 0; n < 17; n++) {
    gfx.write(tones[pos + n]);
  }
}

void TouchMulti() {

  int i, x, y, tps, tid, n, w ;
  int cn;
  memset(touchBut, 0, 100);
  int bytesReceived;
  Wire.beginTransmission(0x38);
  Wire.write((byte)2);
  Wire.endTransmission();
  bytesReceived = Wire.requestFrom(0x38, CTP_DATA_LEN);
  Wire.readBytes(gCTPData, bytesReceived);
  Wire.endTransmission();
  tps = gCTPData[0] ;
  if (tps != 0xff) {                           // some FT chips return garbage before first valid read, try and detect that
    if (tps < 1) pressed = 0;
    if (tps > MAX_PENS) tps = MAX_PENS ;
    for (i = 0; i < tps; i++) {
      if (yInvert) {
        y = gfx.getHeight() - (((gCTPData[1 + i * 6] & 0x0f) << 8) + gCTPData[2 + i * 6]) - 1;
      } else {
        y = ((gCTPData[1 + i * 6] & 0x0f) << 8) + gCTPData[2 + i * 6] ;
      }
      x   = ((gCTPData[3 + i * 6] & 0x0f) << 8) + gCTPData[4 + i * 6] ;
      tid = ((gCTPData[3 + i * 6] & 0xf0) >> 4); // Associated TouchID
      if (gfx_PointWithinRectangle(x, y, kpad) && tps == 1 && settingsMenu == false) {
        w = (((y - 7) / 52) << 2) + ((x - 567) / 52);
        if (w != bdown) {
          if (w >= 0 && w <= 15) {
            ButtonsChange(bdown, 0);
            ButtonsChange(w, 1);
            if(w == 15){
              //gfx.ScreenCapture(0, 0, 800, 480, "MIDIkeyb.gci");
            }
            bdown = w;
            channel = w;
            gfx.MoveTo(268, 75);
            gfx.Opacity(OPAQUE);
            gfx.FGcolour(RED);
            gfx.BGcolour(0x3000);
            gfx.print("Channel "); gfx.print(channel + 1); gfx.print(" ");
            UpdateVelocity();
            UpdateTone();
            UpdateOctave();
          }
        }
      }
      if (gfx_PointWithinRectangle(x, y, backb) && settingsMenu == true) {
        CloseSettings();
      }
      if (gfx_PointWithinRectangle(x, y, smenu)) {
        if (settingsMenu == false) ShowSettings();
      }
      if (gfx_PointWithinRectangle(x, y, set0a) && settingsMenu == true) {
        settings[3] = true;
        UpdateSettings(false);
        if(BLEset == false){
          MIDI.begin(MIDI_CHANNEL_OMNI);
          BLEset = true;
        }
      }
      if (gfx_PointWithinRectangle(x, y, set1a) && settingsMenu == true) {
        settings[3] = false;
        settings[0] = false;
        UpdateSettings(false);
      }
      if (gfx_PointWithinRectangle(x, y, set1b) && settingsMenu == true) {
        settings[3] = false;
        settings[0] = true;
        UpdateSettings(false);
      }
      if (gfx_PointWithinRectangle(x, y, set2a) && settingsMenu == true) {
        settings[1] = false;
        UpdateSettings(false);
      }
      if (gfx_PointWithinRectangle(x, y, set2b) && settingsMenu == true) {
        settings[1] = true;
        UpdateSettings(false);
      }
      if (gfx_PointWithinRectangle(x, y, set3a) && settingsMenu == true) {
        settings[2] = false;
        UpdateSettings(false);
      }
      if (gfx_PointWithinRectangle(x, y, set3b) && settingsMenu == true) {
        settings[2] = true;
        UpdateSettings(false);
      }
      if (gfx_PointWithinRectangle(x, y, mrot) && settingsMenu == false) {
        int minarc = 45; int maxarc = 315;
        int degdiff = maxarc - minarc;
        int posit;
        int deg = gfx.XYposToDegree(x - 125, y - 113); // x - CentreX
        if (deg < minarc) // anything in the first 'dead zone' is minvalue
          deg = 0;
        else {
          if (deg > maxarc) // anything in the last 'dead zone' is maxvalue
            deg = degdiff;
          else
            deg -= minarc; // offset by -baseangle
        }
        pc = (deg * 127 / degdiff); // convert deg to position
        toneVal[channel] = pc;
        UpdateTone();
      }
      if (gfx_PointWithinRectangle(x, y, slidr) && settingsMenu == false) {
        int wpos;
        int wsiz;
        int ming = 20; int maxg = 20;
        wpos = y - slidr[1] - ming;
        wsiz = slidr[3] - slidr[1];
        wpos = map(wpos, 0, wsiz - ming - maxg, 0, 128); // because using -ve mapping for vertical gives incorrect result
        wpos = constrain(wpos, 0, 128 - 1); // constrain after map else 'max' could be 1 high
        wpos = 128 - 1 - wpos; // because vertical slider runs in other direction
        velocity[channel] = wpos;
        UpdateVelocity();
      }
      if (gfx_PointWithinRectangle(x, y, toneb) && settingsMenu == false) {
        if (x < 316) {
          if (pressed == 0) {
            w = 20;
            toneVal[channel] --;
            if (toneVal[channel] < 0) toneVal[channel] = 0;
            bdowno = w;
            ButtonsChange(w, 1);
            pressed = 1;
            pc = toneVal[channel];
            UpdateTone();
          }
        } else {
          if (pressed == 0) {
            w = 21;
            toneVal[channel] ++;
            if (toneVal[channel] > 127) toneVal[channel] = 127;
            bdowno = w;
            ButtonsChange(w, 1);
            pressed = 1;
            pc = toneVal[channel];
            UpdateTone();
          }
        }
      }
      if (gfx_PointWithinRectangle(x, y, octb) && settingsMenu == false) {
        if (x < 439) {
          if (pressed == 0) {
            w = 18;
            octave[channel] --;
            if (octave[channel] < 0) octave[channel] = 0;
            bdowno = w;
            ButtonsChange(w, 1);
            pressed = 1;
            UpdateOctave();
          }
        } else {
          if (pressed == 0) {
            w = 19;
            octave[channel] ++;
            if (octave[channel] > 8) octave[channel] = 8;
            bdowno = w;
            ButtonsChange(w, 1);
            pressed = 1;
            UpdateOctave();
          }
        }
      }
      if (y > 262 && settingsMenu == false) {
        if (x < 220) {
          schk = 0;
          echk = 6;
        } else if (x >= 198 && x < 396) {
          schk = 6;
          echk = 12;
        } else if (x >= 396 && x < 594) {
          schk = 12;
          echk = 18;
        } else {
          schk = 18;
          echk = 24;
        }
        if (y > 400) {
          n = keynum1[x / bw];
        } else {
          if (x - 33 > -1) n = keynum2[(x - 33) / bw];
          if (n == -1) n = keynum1[x / bw];
        }
        if (n > -1 && x >= keyco[n << 2] && x <= keyco[(n << 2) + 2] && y >= keyco[(n << 2) + 1] && y <= keyco[(n << 2) + 3]) {
          if (n < 24) touchBut[n] = 1;
        }
      }
    }
  }
  for (cn = 0; cn < 24; cn++) {
    if (touchBut[cn] != lasttouchBut[cn]) {
      if (touchBut[cn] == 1) {
        MIDImessage(MIDI_NOTE_ON + channel, (octave[channel] * 12) + 6 + cn, velocity[channel]);
        pianoKeyboard(0x80 + cn, 0, 264, 800, 216, 0xC5EF, BLACK, 14);
      }
      if (touchBut[cn] == 0) {
        MIDImessage(MIDI_NOTE_OFF + channel, (octave[channel] * 12) + 6 + cn, velocity[channel]);
        pianoKeyboard(cn, 0, 264, 800, 216, 0xC5EF, BLACK, 14);
      }
      lasttouchBut[cn] = touchBut[cn];
    }
  }
  if (pressed == 0) {
    if (leftSettings) {
      delay(500);
      settingsMenu = false;
      leftSettings = false;
    }
    if (bdowno != -1) {
      ButtonsChange(bdowno, 0);
      bdowno = -1;
    }
  }
}

bool gfx_PointWithinRectangle(int x, int y, int *area) {
  if (x >= area[0] && x <= area[2] && y >= area[1] && y <= area[3]) {
    return true;
  } else {
    return false;
  }
  return false;
}

void MIDImessage(int command, int MIDInote, int MIDIvelocity) {
  if (settings[3]) {
    if (progchange){
      MIDI.sendProgramChange(toneVal[channel], channel + 1);
      progchange = false;
    }
    if ((command & 0xf0) == MIDI_NOTE_ON) {
      MIDI.sendNoteOn(MIDInote, MIDIvelocity, channel + 1);
    }
    if ((command & 0xf0) == MIDI_NOTE_OFF) {
      MIDI.sendNoteOff(MIDInote, MIDIvelocity, channel + 1);
    }
    if ((command & 0xf0) == PROGRAM_CHANGE) {
      MIDI.sendProgramChange(MIDInote, channel + 1);
    }
  } else {
    if (progchange){
      if (settings[0] == false) {
        Serial.write(0xC0 + channel);//send note on or note off command
        Serial.write(toneVal[channel]);//send pitch data
      } else {
        Serial0.write(0xC0 + channel);//send note on or note off command
        Serial0.write(toneVal[channel]);//send pitch data
      }
      progchange = false;
    }
    if (settings[0] == false) {
      Serial.write(command);//send note on or note off command
      Serial.write(MIDInote);//send pitch data
    } else {
      Serial0.write(command);//send note on or note off command
      Serial0.write(MIDInote);//send pitch data
    }
    if (command >> 4 != 0x0c) {
      if (settings[0] == false) {
        Serial.write(MIDIvelocity); //send velocity data
      } else {
        Serial0.write(MIDIvelocity); //send velocity data
      }
    }
  }
  gfx.MoveTo(268, 46);
  gfx.FGcolour(RED);
  gfx.BGcolour(0x3000);
  gfx.Opacity(OPAQUE);
  if (command >> 4 == 0x09) {
    messageOP = "Note  ON:" + String(MIDInote) + " CH:" + String(channel + 1);
  }
  if (command >> 4 == 0x08) {
    messageOP = "Note OFF:" + String(MIDInote) + " CH:" + String(channel + 1);
  }
  if (command >> 4 == 0x0C) {
    if ((channel + 1) < 10 || (MIDInote + 1) < 100) {
      messageOP = "Prog Change:" + String(MIDInote + 1) + " CH:" + String(channel + 1);
    } else {
      messageOP = "Prog Change:" + String(MIDInote + 1) + " CH" + String(channel + 1);
    }
  }
  int sp = 20 - messageOP.length();
  gfx.print(messageOP);
  if (sp > 0) {
    for (int n = 0; n < sp; n++) {
      gfx.print(" ");
    }
  }
}

void UpdateOctave() {
  gfx.MoveTo(268, 20);
  gfx.FGcolour(RED);
  gfx.BGcolour(0x3000);
  gfx.print("Octave "); gfx.print(octave[channel]); gfx.print(" ");
}

void UpdateVelocity() {
  Volume(velocity[channel]);
  gfx.MoveTo(457, 142);
  //gfx.Font(FONT1);
  gfx.BGcolour(RED);
  gfx.FGcolour(0x3000);
  gfx.VlineD(456, 142, 157, RED);
  if (velocity[channel] < 10) {
    gfx.print("00"); gfx.print(velocity[channel]);
  }
  if (velocity[channel] > 9 && velocity[channel] < 100) {
    gfx.print("0"); gfx.print(velocity[channel]);
  }
  if (velocity[channel] > 99) {
    gfx.print(velocity[channel]);
  }
}

void ButtonsChange(int bn, int bs) {
  if (bn >= 0 && bn <= 15) {
    ChannelButton(bs, bn);
  }
  if (bn >= 20 && bn <= 21) {
    ToneOctaveButton(bs, bn - 20);
  }
  if (bn >= 18 && bn <= 19) {
    ToneOctaveButton(bs, bn - 16);
  }
}

void SettingsIcon() {
  gfx.AlphaBlendLevel(180);
  gfx.AlphaBlend(ON);
  gfx.SetGRAM(8, 8, 30, 29);
  gfx.WrGRAMs(SIcon, 509);
  gfx.AlphaBlend(OFF);
}

void ShowSettings() {
  if(settingsMenu){
    gfx.DrawToframebuffer(4);
    gfx.DrawFrameBufferArea(2, 0, 0, 550, 240);
    gfx.Clipping(ON);
  } else {
    gfx.DrawToframebuffer(2);
    gfx.DrawFrameBufferArea(0, 0, 0, 799, 479);
    gfx.DrawToframebuffer(0);
  }
  gfx.AlphaBlendLevel(187);
  gfx.AlphaBlend(ON);
  if(!settingsMenu){
    gfx.RectangleFilled(100, 50, 699, 429, BLACK);
  } else {
    gfx.RectangleFilled(100, 50, 550, 240, BLACK);
  }
  gfx.AlphaBlendLevel(157);
  gfx.TextColor(WHITE, WHITE);
  gfx.Rectangle(100, 50, 699, 429, WHITE);
  gfx.Hline(140, 95, 440, WHITE);
  gfx.MoveTo(140, 78); gfx.print("MIDI Out Interface");
  gfx.MoveTo(180, 120); gfx.print("BLE - Bluetooth BLE");
  gfx.Font(FONT2);
  gfx.MoveTo(180, 170); gfx.print("USB - Via Hairless MIDI serial bridge");
  if(!settingsMenu){
    gfx.MoveTo(190, 190); gfx.Font(FONT1); gfx.print("https://projectgus.github.io/hairless-midiserial/");
    gfx.Font(FONT2);
    gfx.MoveTo(180, 220); gfx.print("Hardware Serial - Via gen4-IB or UPA");
    gfx.Rectangle(629, 56, 692, 95, WHITE);
    gfx.MoveTo(643, 69); gfx.TextColor(WHITE, WHITE); gfx.print("BACK");
    gfx.CircleAA(580, 128, 15, 2, WHITE);
    gfx.CircleAA(580, 178, 15, 2, WHITE);
    gfx.CircleAA(580, 228, 15, 2, WHITE);
    gfx.MoveTo(190, 240); gfx.Font(FONT1); gfx.print("hardware serial can be used for MIDI thru when");
    gfx.MoveTo(190, 250); gfx.Font(FONT1); gfx.print("MIDI output is set to USB");
    gfx.Font(FONT2);
    gfx.Hline(140, 285, 519, WHITE);
    gfx.MoveTo(140, 268); gfx.print("Baud Rate");
    gfx.MoveTo(180, 310); gfx.print("115200 - Default MIDI serial bridge baud");
    gfx.CircleAA(580, 318, 15, 2, WHITE);
    gfx.MoveTo(180, 360); gfx.print("31250  - Default MIDI baud rate");
    gfx.CircleAA(580, 368, 15, 2, WHITE);
    gfx.Hline(140, 415, 519, WHITE);
    gfx.Vline(620, 305, 100, WHITE);
    gfx.CircleAA(660, 318, 15, 2, WHITE);
    gfx.CircleAA(660, 368, 15, 2, WHITE);
    gfx.MoveTo(568, 393); gfx.print("USB");
    gfx.MoveTo(648, 393); gfx.print("H/S");
    gfx.MoveTo(340, 418); gfx.Font(FONT1); gfx.print("www.4dsystems.com.au");
  }
  gfx.Font(FONT2);
  gfx.AlphaBlend(OFF);
  if(settingsMenu){
    gfx.Clipping(OFF);
    gfx.DrawToframebuffer(0);
    gfx.DrawFrameBufferArea(4, 0, 0, 550, 240);
  } else {
    UpdateSettings(true);
  }
  settingsMenu = true;
}

void UpdateSettings(bool refresh) {
  bool trefresh = false;
  for (int n = 0; n < 4; n++) {
    if (settings[n] != lastsettings[n]) trefresh = true;
  }
  if (refresh == true || trefresh == true) {
    DrawOption(set0a, (settings[3]));
    DrawOption(set1a, (!settings[0] && !settings[3]));
    DrawOption(set1b, (settings[0] && !settings[3]));
    lastsettings[0] = settings[0];
    lastsettings[3] = settings[3];
    DrawOption(set2a, (!settings[1]));
    DrawOption(set2b, settings[1]);
    lastsettings[1] = settings[1];
    DrawOption(set3a, (!settings[2]));
    DrawOption(set3b, settings[2]);
    lastsettings[2] = settings[2];
  }
}

void DrawOption(int *coords, bool state) {
  gfx.DrawToframebuffer(3);
  gfx.DrawFrameBufferArea(2, coords[0], coords[1], coords[2], coords[3]);
  gfx.AlphaBlendLevel(187);
  gfx.AlphaBlend(ON);
  gfx.RectangleFilled(coords[0], coords[1], coords[2], coords[3], BLACK);
  gfx.AlphaBlendLevel(157);
  gfx.CircleAA(coords[0] + 15, coords[1] + 15, 15, 2, WHITE);
  gfx.AlphaBlend(OFF);
  if (state) gfx.CircleFilledAA(coords[0] + 15, coords[1] + 15, 10, 0xD6BA);
  gfx.DrawToframebuffer(0);
  gfx.DrawFrameBufferArea(3, coords[0], coords[1], coords[2], coords[3]);
}

void CloseSettings() {
  leftSettings = true;
  gfx.DrawToframebuffer(0);
  gfx.DrawFrameBufferArea(2, 0, 0, 699, 429);
  gfx.TextColor(RED, 0x3000);
  gfx.Opacity(OPAQUE);
  if (settings[1] == false) {
    Serial.begin(115200);
  } else {
    Serial.begin(31250);
  }
  if (settings[2] == false) {
    Serial0.begin(115200);
  } else {
    Serial0.begin(31250);
  }
  progchange = true;
}

void CheckThru() {
  int c, mn, mv, tnow;
  if (Serial0.available() > 0) {
    c = Serial0.read();
    if (c >> 7 != 1) return;
    tnow = millis();
    while (Serial0.available() < 1 && (millis() - tnow < MIDI_TIMEOUT));
    mn = Serial0.read();
    tnow = millis();
    if (c >> 4 == 0x0C) {
      if(settingsMenu){
        gfx.DrawToframebuffer(2);
        MIDImessage(c, mn, 0);
        ShowSettings();
      } else {
        MIDImessage(c, mn, 0);
      }
      toneVal[c & 0x0F] = mn;
      if (channel == (c & 0x0F)) {
        if(settingsMenu){
          gfx.DrawToframebuffer(2);
          UpdateTone();
          ShowSettings();
        } else {
          UpdateTone();
        }
      }
    } else {
      while (Serial0.available() < 1 && (millis() - tnow < MIDI_TIMEOUT));
      mv = Serial0.read();
      if(settingsMenu){
        gfx.DrawToframebuffer(2);
        MIDImessage(c, mn, mv);
        ShowSettings();
      } else {
        MIDImessage(c, mn, mv);
      }
    }
  }
}

void CheckPC() {
  int c, mn, tnow;
  if (settings[3]) {
    if (MIDI.read()) {               // Is there a MIDI message incoming ?
    /*
      switch (MIDI.getType()) {    // Get the type of the message we caught
        case midi::ProgramChange:       // If it is a Program Change,
          c = MIDI.getChannel() - 1;
          toneVal[c] = MIDI.getData1();
          if (channel == c){
            if(settingsMenu){
              gfx.DrawToframebuffer(2);
              UpdateTone();
              ShowSettings();
              gfx.DrawToframebuffer(0);
            } else {
              UpdateTone();
            }
          }
          break;
        default:
          break;
      }
      */
    }
  } else {
    if (settings[0] == false) {
      if (Serial.available() > 0) {
        c = Serial.read();
        tnow = millis();
        if (c >> 4 == 0x0C) {
          while (Serial.available() < 1 && millis() - tnow < MIDI_TIMEOUT);
          mn = Serial.read();
          toneVal[c & 0x0F] = mn;
          if (channel == (c & 0x0F)) {
            if(settingsMenu){
              gfx.DrawToframebuffer(2);
              UpdateTone();
              ShowSettings();
              gfx.DrawToframebuffer(0);
            } else {
              UpdateTone();
            }
          }
          Serialrec = 1;
        }
      }
    } else {
      if (Serial0.available() > 0) {
        c = Serial0.read();
        tnow = millis();
        if (c >> 4 == 0x0C) {
          while (Serial0.available() < 1 && millis() - tnow < MIDI_TIMEOUT);
          mn = Serial0.read();
          toneVal[c & 0x0F] = mn;
          if (channel == (c & 0x0F)) {
            if(settingsMenu){
              gfx.DrawToframebuffer(2);
              UpdateTone();
              ShowSettings();
              gfx.DrawToframebuffer(0);
            } else {
              UpdateTone();
            }
          }
          Serialrec = 1;
        }
      }
    }
  }
}

void AllNotesOFF() {
  for (int n = 0; n < 15; n++) {
    if (settings[0]) {
      Serial.write(CHANNEL_MODE + n);
      Serial.write(123);
      Serial.write(0);
    } else {
      Serial0.write(CHANNEL_MODE + n);
      Serial0.write(123);
      Serial0.write(0);
    }
  }
}


