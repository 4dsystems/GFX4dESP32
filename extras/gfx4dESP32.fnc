SdFat& gfx.getSdFatInstance(), SdFat&; // Get the SdFat instance to use from the main sketch
// Syntax	: gfx.getSdFatInstance();
// Usage	: gfx.getSdFatInstance();
// Notes 	: Returns a reference to the SdFat instance created during gfx.begin();
//

void gfx.Vline(x, y, h, colour), void; // Draw fast vertical line at x, y, with height h and colour.
// Syntax	: gfx.Vline(x, y, h, colour);
// Usage	: gfx.Vline(10, 10, 100, MAGENTA);
// Notes 	: Draw fast vertical line at x, y, with height h and colour
//			: Height can be -ve indicating an upwards direction.
//

void gfx.Hline(x, y, w, colour), void; // Draw fast horizontal line at x, y, with width w and colour.
// Syntax	: gfx.Hline(x, y, w, hcolor);
// Usage	: gfx.Hline(200, 200, -100, CYAN);
// Notes 	: Draw fast horizontal line at x, y, with width w and colour.
//			: Height can be -ve indicating the line draws to the left.
//

void gfx.SetGRAM(x1, y1, x2, y2), void; // Define pixel write area in the current framebuffer
// Syntax	: gfx.SetGRAM(x1, y1, x2, y2);
// Usage	: gfx.SetGRAM(0, 0, 239, 319);
// Notes 	: Define pixel write area in the current framebuffer
//

void gfx.WrGRAMs(color_data, len), void; // Write an array of pixels with the specified length len from an array to the selected framebuffer
// Syntax	: gfx.WrGRAMs(color_data, len);
// Usage	: gfx.WrGRAMs(color_data, w * h);
//			: gfx.WrGRAMs(image_array, 200);
// Notes 	: Write an array of pixels with the specified length len from an array to the selected framebuffer
//			: The array can be 8-bit, 16-bit or 32-bit
//			: WrGRAMs should be used intead of pushColors if image data is likely to exceed display boundaries or if transparency is used
//			: 32bit data arrays can used for IoD compatibility
//

void gfx.WrGRAM(colour), void; // Write a single 16-bit colour to GRAM window to the selected framebuffer
// Syntax	: gfx.WrGRAM(colour);
// Usage	: gfx.WrGRAM(WHITE);
// Notes 	: Write a single 16-bit colour to GRAM window to the selected framebuffer
//

void gfx.pushColors(color_data, len), void; // Write an array of pixels with the specified length len from an array to the selected framebuffer
// Syntax	: gfx.pushColors(color_data, len);
// Usage	: gfx.pushColors(image_array, 200);
// Notes 	: Write an array of pixels with the specified length len from an array to the selected framebuffer
//			: The array can be 8-bit, 16-bit or 32-bit
//			: pushColors operates faster than WrGRAMs due to no boundary or transparency checks
//			: Care must be taken to ensure image area does not exceed the displays boundaries as an error will occur
//			: 32bit data arrays can used for IoD compatibility
//

boolean gfx.StartWrite(), 1; // Sets the start write condition
// Syntax	: gfx.StartWrite();
// Usage	: gfx.StartWrite();
// Notes 	: Sets the start write condition preventing subsequent writes to the framebuffer to update the display until gfx.EndWrite() is used
//

void gfx.EndWrite(), void; // Unsets the start write condition
// Syntax	: gfx.EndWrite();
// Usage	: gfx.EndWrite();
// Notes 	: Unsets the start write condition allowing all changes to the framebuffer to update the display
//

void gfx.setScrollArea(y1, y2), void; // Set scroll window specified by the top and bottom pixel positions
// Syntax	: gfx.setScrollArea(y1, y2);
// Usage	: gfx.setScrollArea(0, 319);
// Notes 	: Set scroll window specified by the top and bottom pixel positions
//			: This assumes the horizontal area is the full width of the screen
//

void gfx.setScrollArea(x1, y1, x2, y2), void; // Set scroll window specified by the rectangle with diagonal at (x1, y1) to (x2, y2)
// Syntax	: gfx.setScrollArea(x1, y1, x2, y2);
// Usage	: gfx.setScrollArea(10, 0, 229, 319);
// Notes 	: Set scroll window specified by the rectangle with diagonal at (x1, y1) to (x2, y2)
//

void gfx.setScrollDirection(scrDir), void; // Set scroll direction
// Syntax	: gfx.setScrollArea(scrDir);
// Usage	: gfx.setScrollArea(SCROLL_UP);
// Notes 	: directions
//			: SCROLL_UP
//			: SCROLL_DOWN
//			: SCROLL_LEFT
//			: SCROLL_RIGHT
//

void gfx.Scroll(steps), void; // Scroll by 'steps' pixel lines.
// Syntax	: gfx.Scroll(steps);
// Usage	: gfx.Scroll(10);
// Notes 	: Scroll by 'steps' pixel lines
//			: If SmoothScrollSpeed has been set, it will be scrolled pixel line by pixel line
//			: delayed by the ms value set using gfx.SmoothScrollSpeed(speed)
//

void gfx.setScrollBlankingColor(colour), void; // Set blanking line colour after scroll has moved
// Syntax	: gfx.setScrollBlankingColor(colour);
// Usage	: gfx.setScrollBlankingColor(WHITE);
// Notes 	: Set blanking line colour after scroll has moved
//			: This can be used to match the current text background colour
//

void gfx.SmoothScrollSpeed(speed), void; // Set scroll behaviour
// Syntax	: gfx.SmoothScrollSpeed(speed);
// Usage	: gfx.SmoothScrollSpeed(10);
// Notes 	: 0 - will scroll height defined by character height in one step.
//			: A value higher than 0 will scroll pixel line by pixel line
//			: delayed by the value in ms
//

void gfx.PutPixel(x, y, colour), void; // Write 1 pixel at co-ordinates x, y and colour.
// Syntax	: gfx.PutPixel(x, y, colour);
// Usage	: gfx.PutPixel(10, 15, LIME);
// Notes 	: Write 1 pixel at co-ordinates x, y and colour.
//

void gfx.PutPixelAlpha(x, y, colour, alpha), void; // Draws a pixel to the current frame buffer with color parameter switch
// Syntax	: gfx.PutPixelAlpha(x, y, colour);
// Usage	: gfx.PutPixelAlpha(10, 10, GCI_IMAGE + 3, 255);
//			: gfx.PutPixelAlpha(10, 10, FRAMEBUFFER_IMAGE + 3, 127);
//			: gfx.PutPixelAlpha(10, 10, RED, 127);
// Notes 	: Draws a pixel to the current frame buffer with color parameter switch.
//			: 'colour' can be a 16bit color or a GCI image or a FrameBuffer
//			:     ex. gfx.PutPixelAlpha(10, 10, GCI_IMAGE + 3, 255) draws a pixel at 10, 10
//			:         from GCI image index 3 and full alpha level. x and y are relative to objects x and y.
//			:     ex. gfx.PutPixelAlpha(10, 10, FRAMEBUFFER_IMAGE + 3, 127) draws a pixel at 10, 100
//			:         from GCI image index 3 with half alpha at the same x and y position.
//			:     ex. gfx.PutPixelAlpha(10, 10, RED, 127) draws a RED pixel at 10, 10
//			:         with half alpha at the same x and y position.
//


int16_t gfx.getHeight(), word; // Returns the height of the display in pixels.
// Syntax	: gfx.getHeight();
// Usage	: gfx.getHeight();
// Notes 	: Returns the height of the display in pixels.
//

int16_t gfx.getWidth(), word; // Returns the width of the display in pixels.
// Syntax	: gfx.getWidth();
// Usage	: gfx.getWidth();
// Notes 	: Returns the width of the display in pixels.
//

void gfx.FillScreen(colour), void; // Fills the screen with specified colour.
// Syntax	: gfx.FillScreen(colour);
// Usage	: gfx.FillScreen(BLACK);
// Notes 	: Fills the screen with specified colour.
//

void gfx.BacklightOn(mode), void; // Sets backlight on or off, true or false
// Syntax	: gfx.BacklightOn(mode);
// Usage	: gfx.BacklightOn(false);
// Notes 	: Sets backlight on or off, true or false
//

void gfx.Contrast(value), void; // Sets the backlight level from 0-15
// Syntax	: gfx.Contrast(value);
// Usage	: gfx.Contrast(7);
// Notes 	: Sets the backlight level from 0-15
//			: 0 - off, 1-15 turns the backlight on at different levels
//

void gfx.Invert(mode), void; // Inverts all pixels on the screen.
// Syntax	: gfx.Invert(mode);
// Usage	: gfx.Invert(true);
// Notes 	: Inverts all pixels on the screen.
//

void gfx.Transparency(mode [, colour]), void; // Enables setting a colour as transparent
// Syntax	: gfx.Transparency(mode, colour);
//			: gfx.Transparency(mode);
// Usage	: gfx.Transparency(true, BLACK);
//			: gfx.Transparency(true);
// Notes 	: Enables setting a colour as transparent
//			: gfx.TransparentColor can also be used to change the transparent colour
//

void gfx.TransparentColor(colour), void; // Sets the colour that will be treated as transparent and will not be drawn
// Syntax	: gfx.TransparentColor(colour);
// Usage	: gfx.TransparentColor(BLACK);
// Notes 	: Sets the colour that will be treated as transparent and will not be drawn
//			: gfx.Transparency must be used and set to 'true' for this to take effect
//

uint16_t gfx.ReadPixel(x, y), uint16_t; // Read a single pixel from the framebuffer
// Syntax	: gfx.ReadPixel(x, y);
// Usage	: gfx.ReadPixel(10, 15);
// Notes 	: Read a single pixel from the framebuffer
//

uint16_t gfx.ReadLine(x, y, w, data), uint16_t; // Read a line of pixels starting from (x, y) and store it in the specified 16-bit data array
// Syntax	: gfx.ReadLine(x, y, w, data);
// Usage	: gfx.ReadLine(10, 15, 100, data_array);
// Notes 	: Read a line of pixels and store it in the specified 16-bit data array
//

void gfx.WriteLine(x, y, w, data), void; // Writes a line of pixels from the specified 16-bit array starting from (x, y)
// Syntax	: gfx.WriteLine(x, y, w, data);
// Usage	: gfx.WriteLine(x, y, w, data);
// Notes 	: Writes a line of pixels from the specified 16-bit array starting from (x, y)
//

void gfx.DrawFrameBuffer(fbnum), void; // Flush the whole specified framebuffer
// Syntax	: gfx.DrawFrameBuffer(fbnum);
// Usage	: gfx.DrawFrameBuffer(fbnum);
// Notes 	: Flush the whole specified framebuffer
//

void gfx.DrawFrameBufferArea(fbnum, id), void; // Flush the area occupied by GCI widget specified by id from the selected framebuffer fbnum
// Syntax	: gfx.DrawFrameBufferArea(fbnum, ui);
// Usage	: gfx.DrawFrameBufferArea(fbnum, ui);
// Notes 	: Flush the area occupied by GCI widget specified by id from the selected framebuffer fbnum
//

void gfx.DrawFrameBufferArea(fbnum, x1, y1, x2, y2), void; // Flush the rectangular area specified by the diagonal (x1, y1), (x2, y2) from the selected framebuffer fbnum
// Syntax	: gfx.DrawFrameBufferArea(fbnum, x1, y1, x2, y2);
// Usage	: gfx.DrawFrameBufferArea(fbnum, x1, y1, x2, y2);
// Notes 	: Flush the rectangular area specified by the diagonal (x1, y1), (x2, y2) from the selected framebuffer fbnum
//

void gfx.MergeFrameBuffers(fbto, fbfrom1, fbfrom2, transColor), void; // Merge 2 frame buffers and send to specified frame buffer.
// Syntax	: gfx.MergeFrameBuffers(fbto, fbfrom1, fbfrom2, transColor);
// Usage	: gfx.MergeFrameBuffers(fbto, fbfrom1, fbfrom2, transColor);
// Notes 	: Merge 2 frame buffers and send to specified frame buffer.
//			: Using this function without first writing to a frame buffer will cause issue
//

void gfx.MergeFrameBuffers(fbto, fbfrom1, fbfrom2, fbfrom3, transColor, transColor1), void; // Merge 3 frame buffers and send to specified frame buffer.
// Syntax	: gfx.MergeFrameBuffers(fbto, fbfrom1, fbfrom2, fbfrom3, transColor, transColor1);
// Usage	: gfx.MergeFrameBuffers(fbto, fbfrom1, fbfrom2, fbfrom3, transColor, transColor1);
// Notes 	: Merge 3 frame buffers and send to specified frame buffer.
//			: Using this function without first writing to a frame buffer will cause issue
//

void gfx.drawBitmap(x1, y1, x2, y2, c_data), void; // Draws a bitmap using the data array (c_data) at the given co-ordinates 
// Syntax	: gfx.drawBitmap(x1, y1, x2, y2, c_data);
// Usage	: gfx.drawBitmap(x1, y1, x2, y2, c_data);
// Notes 	: Draws a bitmap using the data array (c_data) at the given co-ordinates (for SPI displays only)
//			: Support for RGB displays will be available in the future
//

void gfx.PinMode(pin, mode), void; // Sets the operation mode of the specified pin
// Syntax	: gfx.PinMode(pin, mode);
// Usage	: gfx.PinMode(GEN4_ESP32_PIN5, OUTPUT);
// Notes 	: Sets the operation mode of the specified pin
//

void gfx.DigitalWrite(pin, state), void; // Sets the specified pin to either HIGH or LOW
// Syntax	: gfx.DigitalWrite(pin, state);
// Usage	: gfx.DigitalWrite(GEN4_ESP32_PIN5, HIGH);
// Notes 	: Sets the specified pin to either HIGH or LOW
//			: The pin must first be set as OUTPUT using gfx.PinMode
//

int gfx.DigitalRead(byte pin), 1; // Reads the current state of the specified pin
// Syntax	: gfx.DigitalRead(byte pin);
// Usage	: gfx.DigitalRead(byte pin);
// Notes 	: Reads the current state of the specified pin
//			: The pin must first be set as INPUT using gfx.PinMode
//

void gfx.Cls([colour]), void; // Clears the screen with the specified colour or BLACK if no colour is specified
// Syntax	: gfx.Cls(colour);
//			: gfx.Cls();
// Usage	: gfx.Cls(WHITE);
//			: gfx.Cls();
// Notes 	: Clears the screen with the specified colour or BLACK if no colour is specified
//

void gfx.begin(), void; // Initialize the screen
// Syntax	: gfx.begin();
// Usage	: gfx.begin();
// Notes 	: Initialize global variables, orientation and clears the screen
//			: uSD is mounted at this stage and called separately.
//			:     begin();                          // start with default settings
//			:     begin(IPS_DISPLAY);               // use ips display
//			:     begin(18);                        // set display clock to 18Mhz
//			:     begin(false);                     // start without backlight on
//			:     begin(IPS_DISPLAY, 18);           // start with ips at 18Mhz
//			:     begin(IPS_DISPLAY, true);         // start with ips and backlight on
//			:     begin(20, false);                 // start at 20Mhz and backlight off
//			:     begin(IPS_DISPLAY, 40, false);    // start with ips at 40Mhz and backlight off
//

void gfx.ScrollEnable(mode), void; // Enable or disable auto scrolling
// Syntax	: gfx.ScrollEnable(mode);
// Usage	: gfx.ScrollEnable(true);
// Notes 	: Enable or disable auto scrolling
//

int16_t gfx.getX(), word; // Returns the current X position of the cursor.
// Syntax	: gfx.getX();
// Usage	: gfx.getX();
// Notes 	: Returns the current X position of the cursor.
//

int16_t gfx.getY(), word; // Returns the current Y position of the cursor.
// Syntax	: gfx.getY();
// Usage	: gfx.getY();
// Notes 	: Returns the current Y position of the cursor.
//

void gfx.Clipping(mode), void; // Enables previously defined clipping area.
// Syntax	: gfx.Clipping(mode);
// Usage	: gfx.Clipping(true);
// Notes 	: Enables previously defined clipping area.
//

void gfx.ClipWindow(x1, y1, x2, y2), void; // Set Clip window specified by the rectangle with diagonal at (x1, y1) to (x2, y2)
// Syntax	: gfx.ClipWindow(x1, y1, x2, y2);
// Usage	: gfx.ClipWindow(10, 0, 229, 319);
// Notes 	: Set clip window specified by the rectangle with diagonal at (x1, y1) to (x2, y2)
//

void gfx.CircleFilled(x, y, r, colour), void; // Draw a filled circle with center at (x, y), with radius r and colour 'colour'.
// Syntax	: gfx.CircleFilled(x, y, r, colour);
// Usage	: gfx.CircleFilled(120, 100, 50, CYAN);
// Notes 	: Draw a filled circle with center at (x, y), with radius r and colour 'colour'.
//

void gfx.CircleFilledAA(x, y, r, colour), void; // Draw a smooth filled circle with center at (x, y), with radius r and colour 'colour'.
// Syntax	: gfx.CircleFilled(x, y, r, colour);
// Usage	: gfx.CircleFilled(120.5, 100.7, 50, CYAN);
// Notes 	: Draw a smooth filled circle with center at (x, y), with radius r and colour 'colour'.
//

void gfx.Circle(x, y, r, colour), void; // Draw a circle with center at (x, y), with radius r and colour 'colour'.
// Syntax	: gfx.Circle(x, y, r, colour);
// Usage	: gfx.Circle(100, 120, 40, LIME);
// Notes 	: Draw a circle at x, y, with radius r and colour 'colour'.
//

void gfx.EllipseFilled(xe, ye, radx, rady, colour), void; // Draw a filled ellipse at x, y, with horizontal and vertical radii 'radx' and 'rady' and colour 'colour'.
// Syntax	: gfx.EllipseFilled(x, y, radx, rady, colour);
// Usage	: gfx.EllipseFilled(100, 100, 40, 30, BLUE);
// Notes 	: Draw a filled ellipse at x, y, with horizontal and vertical radii 'radx' and 'rady' and colour 'colour'.
//

void gfx.Ellipse(x, y, radx, rady, colour), void; // Draw an ellipse with center at (x, y), with horizontal and vertical radii 'radx' and 'rady' and colour 'colour'.
// Syntax	: gfx.Ellipse(x, y, radx, rady, colour);
// Usage	: gfx.Ellipse(150, 100, 40, 50, GREEN);
// Notes 	: Draw an ellipse with center at (x, y), with horizontal and vertical radii 'radx' and 'rady' and colour 'colour'.
//

void gfx.ArcFilled(x, y, r, topBottom, gap, colour), void; // Draw 90-degree Arcs with center at (x, y) and gap between left and right parts
// Syntax	: gfx.ArcFilled(x, y, r, topBottom, gap, colour);
// Usage	: gfx.ArcFilled(100, 100, 50, 3, 0, RED);
// Notes 	: Draw 90-degree Arcs with center at (x, y) and gap between left and right parts
//			: Support function for filled rounded rectangles and circles
//			:   topBottom - indicates whether to draw the top arcs (1), bottom arcs (2) or both sets (3)
//			:   gap - indicates the gap between the left and right arcs
//

void gfx.Arc(x, y, r, quadrants, colour), void; // Draw a 90-degree Arc with center at (x, y) using the specified option
// Syntax	: gfx.Arc(x, y, r, quadrants, colour);
// Usage	: gfx.Arc(100, 100, 50, 0x8 | 0x4, RED);
// Notes 	: Draw a 90-degree Arc with center at (x, y) at the specified quadrants
//			: The quadrants can be multiple and are determined by the following bits:
//			:       0x01 - Top left
//			:       0x02 - Top right
//			:       0x04 - Bottom right
//			:       0x08 - Bottom left
//			: Support function for outline rounded rectangles and circles
//

void gfx.RectangleFilledX(x1, y1, x2, y2, colour), void; // Draws a rectangle to the current frame buffer with color parameter switch
// Syntax	: gfx.RectangleFilledX(x1, y1, x2, y2, colour);
// Usage	: gfx.RectangleFilledX(10, 10, 100, 100, GCI_IMAGE + 3);
//			: gfx.RectangleFilledX(10, 10, 100, 100, FRAMEBUFFER_IMAGE + 3);
// Notes 	: Draws a filled rectangle having a diagonal with endpoints at (x1, y1) and (x2, y2) with colour 'colour'.
//			: 'colour' can be a 16bit color or a GCI image or a FrameBuffer
//			:     ex. gfx.RectangleFilledX(10, 10, 100, 100, GCI_IMAGE + 3) draws a rectangle at 10, 10
//			:           from GCI image index 3. x and y are relative to objects x and y.
//			:     ex. gfx.RectangleFilledX(10, 10, 100, 100, FRAMEBUFFER_IMAGE + 3) draws a rectangle at 10, 10
//			:           from framebuffer 3 at the same x and y position.
//

void gfx.RectangleFilled(x1, y1, x2, y2, colour), void; // Draws a filled rectangle having a diagonal with endpoints at (x1, y1) and (x2, y2) with colour 'colour'.
// Syntax	: gfx.RectangleFilled(x1, y1, x2, y2, colour);
// Usage	: gfx.RectangleFilled(10, 20, 110, 170, RED);
// Notes 	: Draws a filled rectangle having a diagonal with endpoints at (x1, y1) and (x2, y2) with colour 'colour'.
//

void gfx.Rectangle(x1, y1, x2, y2, colour), void; // Draws a rectangle having a diagonal with endpoints at (x1, y1) and (x2, y2) with colour 'colour'.
// Syntax	: gfx.Rectangle(x, y, w, h, colour);
// Usage	: gfx.Rectangle(20, 50, 120, 160, MAGENTA);
// Notes 	: Draws a rectangle having a diagonal with endpoints at (x1, y1) and (x2, y2) with colour 'colour'.
//

void gfx.Orientation([orient]), void; // Changes the screen orientation to LANDSCAPE, LANDSCAPE_R, PORTRAIT or PORTRAIT_R, query the current orientation.
// Syntax	: gfx.Orientation(orient);
//			: gfx.Orientation();
// Usage	: gfx.Orientation(LANDSCAPE);
//			: gfx.Orientation();
// Notes 	: Changes the screen orientation to LANDSCAPE, LANDSCAPE_R, PORTRAIT or PORTRAIT_R
//			: or query the current orientation.
//

void gfx.RoundRectFilled(x1, y1, x2, y2, r, colour), void; // Draws a filled rounded rectangle having a diagonal with endpoints at (x1, y1) and (x2, y2), corner radius 'r' and colour 'colour'.
// Syntax	: gfx.RoundRectFilled(x1, y1, x2, y2, r, colour);
// Usage	: gfx.RoundRectFilled(50, 50, 200, 200, 25, RED);
// Notes 	: Draws a filled rounded rectangle having a diagonal with endpoints at (x1, y1) and (x2, y2), corner radius 'r' and colour 'colour'.
//

void gfx.RoundRect(x1, y1, x2, y2, r, colour), void; // Draws a rounded rectangle having a diagonal with endpoints at (x1, y1) and (x2, y2), corner radius 'r' and colour 'colour'.
// Syntax	: gfx.RoundRect(x1, y1, x2, y2, r, colour);
// Usage	: gfx.RoundRect(50, 50, 200, 200, 25, YELLOW);
// Notes 	: Draws a rounded rectangle having a diagonal with endpoints at (x1, y1) and (x2, y2), corner radius 'r' and colour 'colour'.
//

void gfx.TriangleFilled(x1, y1, x2, y2, x3, y3, colour), void; // Draw a filled triangle specified by 3 points using the colour 'colour'
// Syntax	: gfx.TriangleFilled(x1, y1, x2, y2, x3, y3, colour);
// Usage	: gfx.TriangleFilled(10, 20, 100, 40, 60, 100, GREEN);
// Notes 	: Draw a filled triangle specified by 3 points using the colour 'colour'
//

void gfx.Triangle(x0, y0, x1, y1, x2, y2, colour), void; // Draw a triangle specified by 3 points using the colour 'colour'
// Syntax	: gfx.Triangle(x0, y0, x1, y1, x2, y2, colour);
// Usage	: gfx.Triangle(10, 20, 100, 40, 60, 100, BLUE);
// Notes 	: Draw a triangle specified by 3 points using the colour 'colour'
//

void gfx.TriangleAA(x0, y0, x1, y1, x2, y2, w, colour), void; // Draw a smooth triangle specified by 3 points using the colour 'colour' and line width 'w'
// Syntax	: gfx.TriangleAA(x0, y0, x1, y1, x2, y2, w, colour);
// Usage	: gfx.TriangleAA(10, 20, 100, 40, 60, 100, 3, BLUE);
// Notes 	: Draw a smooth triangle specified by 3 points using the colour 'colour' and line width of 'w'
//

void gfx.Line(x0, y0, x1, y1, colour), void; // Draw a line specified by the endpoints (x0, y0) and (x1, y1) using colour 'colour'.
// Syntax	: gfx.Line(x0, y0, x1, y1, colour);
// Usage	: gfx.Line(1, 15, 115, 200, RED);
// Notes 	: Draw a line specified by the endpoints (x0, y0) and (x1, y1) using colour 'colour'.
//

void gfx.LineAA(x0, y0, x1, y1, w, colour), void; // Draw a smooth line specified by the endpoints (x0, y0) and (x1, y1) using colour 'colour'.
// Syntax	: gfx.Line(x0, y0, x1, y1, w, colour);
// Usage	: gfx.Line(1.5, 15.2, 115.6, 200.3, 5, RED);
// Notes 	: Draw a smooth line specified by the floating point endpoints (x0, y0) and (x1, y1) using colour 'colour' and thickness 'w'.
//			: Optional 2nd width will draw a line with different thickness at each end eg gfx.Line(10, 10, 100, 100, 1, 10, RED); will draw a pointer type line
//

void gfx.setGCIsystem(gs), void; // Sets the GCI system to uSD or from a data array
// Syntax	: gfx.setGCIsystem(gs);
// Usage	: gfx.setGCIsystem(gs);
// Notes 	: Sets the GCI system to uSD or from a data array
//			: Valid options are:
//			:       GCI_SYSTEM_USD - sets GCI system to uSD
//			:       GCI_SYSTEM_PROGMEM - sets GCI system to data array
//

uint8_t gfx.getGCIsystem(), uint8_t; // Returns currently selected GCI system
// Syntax	: gfx.getGCIsystem();
// Usage	: gfx.getGCIsystem();
// Notes 	: Returns currently selected GCI system
//

void gfx.Open4dGFX(filename), void; // Opens 4D DAT file for parsing and GCI file for read using filename without extension.
// Syntax	: gfx.Open4dGFX(filename);
// Usage	: gfx.Open4dGFX("sample");
// Notes 	: Opens 4D DAT file for parsing and GCI file for read using filename without extension.
//

void gfx.Open4dGFX(DATa, DATlen, GCIa, GCIlen), void; // Loads a 4D DAT array and GCI DATA array for reading 4D widgets
// Syntax	: gfx.Open4dGFX(DATa, DATlen, GCIa, GCIlen);
// Usage	: gfx.Open4dGFX(project_dat, project_dat_size, project_gci, project_gci_size);
// Notes 	: Loads a 4D DAT array and GCI DATA array for reading 4D widgets
//

void gfx.Close4dGFX(), void; // Closes the DAT and GCI files opened by gfx.Open4dGFX(filename)
// Syntax	: gfx.Close4dGFX();
// Usage	: gfx.Close4dGFX();
// Notes 	: Closes the DAT and GCI files opened by gfx.Open4dGFX(filename)
//

gfx4d_font gfx.Open4dFont(filename), gfx4d_font; // Opens a Workshop4 font (IFont/GCI format) for reading
// Syntax	: gfx.Open4dFont(filename);
// Usage	: gfx.Open4dFont("sample.g01");
//			: gfx.Open4dFont("sample.IFont1");
// Notes 	: Opens a Workshop4 font (IFont/GCI format) for reading
//			: Returns a reference to the opened font and can be used with gfx.Font
//

void gfx.UserImage(id [, x, y]), void; // Draw a single frame GCI widget in the default location or as specified by (x, y)
// Syntax	: gfx.UserImage(id, x, y);
//			: gfx.UserImage(id);
// Usage	: gfx.UserImage(id, x, y);
//			: gfx.UserImage(id);
// Notes 	: Draw a single frame GCI widget in the default location or as specified by (x, y)
//

void gfx.UserImages(id, frame [, offset, x, y]), void; // Draw the specified frame of GCI widget with the specified ID
// Syntax	: gfx.UserImages(id, frame, offset, x, y);
//			: gfx.UserImages(id, frame, x, y);
//			: gfx.UserImages(id, frame, offset);
//			: gfx.UserImages(id, frame);
// Usage	: gfx.UserImages(iSpectrum0, 50, 5, 10, 10);
//			: gfx.UserImages(iGauge0, 50, 10, 10);
//			: gfx.UserImages(iSpectrum0, 50, 5);
//			: gfx.UserImages(iGauge0, 50);
// Notes 	: Draw the specified frame of GCI widget with the specified ID
//			: This can also be used for Spectrum widgets by specifying the offset
//			: Optionally, new position (x, y) can be specified
//

void gfx.UserImagesDR(id, frame, uxpos, uypos, uwidth, uheight), void; // Draw section of a multiple frame GCI widget
// Syntax	: gfx.UserImagesDR(id, frame, uxpos, uypos, uwidth, uheight);
// Usage	: gfx.UserImagesDR(id, frame, uxpos, uypos, uwidth, uheight);
// Notes 	: Draw section of a GCI widget.
//

void gfx.UserImageDR(id, uxpos, uypos, uwidth, uheight, uix, uiy), void; // Draw section of a single frame GCI widget
// Syntax	: gfx.UserImageDR(id, uxpos, uypos, uwidth, uheight, uix, uiy);
// Usage	: gfx.UserImageDR(id, uxpos, uypos, uwidth, uheight, uix, uiy);
// Notes 	: Draw section of a single frame GCI widget
//

void gfx.UserImageDRcache(id, uxpos, uypos, uwidth, uheight, uix, uiy), void; // Draw section of a single frame GCI widget while utilizing cache
// Syntax	: gfx.UserImageDRcache(id, uxpos, uypos, uwidth, uheight, uix, uiy);
// Usage	: gfx.UserImageDRcache(id, uxpos, uypos, uwidth, uheight, uix, uiy);
// Notes 	: Draw section of a single frame GCI widget while utilizing cache
//

void gfx.LedDigitsDisplaySigned(newval, index, Digits, MinDigits, WidthDigit, LeadingBlanks [, x, y]), void; // Display LedDigits or CustomDigits as signed value
// Syntax	: gfx.LedDigitsDisplaySigned(newval, index, Digits, MinDigits, WidthDigit, LeadingBlanks, altx, alty);
//			: gfx.LedDigitsDisplaySigned(newval, index, Digits, MinDigits, WidthDigit, LeadingBlanks);
// Usage	: gfx.LedDigitsDisplaySigned(newval, index, Digits, MinDigits, WidthDigit, LeadingBlanks, altx, alty);
//			: gfx.LedDigitsDisplaySigned(newval, index, Digits, MinDigits, WidthDigit, LeadingBlanks);
// Notes 	: Displays a signed or unsigned numeric value using graphics from Led or Custom Digits.
//      	: This routine only works with all numbers and must be generated with project option 'Allow -ve Led and Custom Digits and 
//       	: leading blanks on Custom Digits' is set to true. if it is false use gfx.LedDigitsDisplay
//       	: x and y are optional position overrides
//

void gfx.LedDigitsDisplay(newval, index, Digits, MinDigits, WidthDigit, LeadingBlanks [, x, y]), void; // Display LedDigits or CustomDigits as unsigned value
// Syntax	: gfx.LedDigitsDisplay(newval, index, Digits, MinDigits, WidthDigit, LeadingBlanks, x, y);
//			: gfx.LedDigitsDisplay(newval, index, Digits, MinDigits, WidthDigit, LeadingBlanks);
// Usage	: gfx.LedDigitsDisplay(newval, index, Digits, MinDigits, WidthDigit, LeadingBlanks, x, y);
//			: gfx.LedDigitsDisplay(newval, index, Digits, MinDigits, WidthDigit, LeadingBlanks);
// Notes 	: Displays a numeric value using graphics from Led or Custom Digits.
//      	: This routine only works with unsigned numbers and must be generated with project option 'Allow -ve Led and Custom Digits and 
//       	: leading blanks on Custom Digits' is set to false. if it is true use gfx.LedDigitsDisplaySigned
//       	: x and y are optional position overrides
//

void gfx.PrintImage(index), void; // Print image from GCI file at 'index' at current cursor position.
// Syntax	: gfx.PrintImage(index);
// Usage	: gfx.PrintImage(index);
// Notes 	: Print image from GCI file at 'index' at current cursor position.
//

size_t gfx.write(c), 1; // Writes a single character to current cursor position
// Syntax	: gfx.write(c);
// Usage	: gfx.write(c);
// Notes 	: Writes a single character to current cursor position
//

void gfx.MoveTo(x, y), void; // Sets the cursor position to (x, y)
// Syntax	: gfx.MoveTo(x, y);
// Usage	: gfx.MoveTo(x, y);
// Notes 	: Sets the cursor position to (x, y)
//

int8_t gfx.Font(), 1; // Check the current system font or font type being used
// Syntax	: gfx.Font();
// Usage	: gfx.Font();
// Notes 	: Check the current system font or font type being used
//

void gfx.Font(f [, compressed]), void; // Sets the font to a system font, GCI font, or font array
// Syntax	: gfx.Font(f);
//			: gfx.Font(f, compressed);
// Usage	: gfx.Font(FONT1);
//			: gfx.Font(gciFont);
//			: gfx.Font(font_array, true);
// Notes 	: Sets the font to a system font, GCI font, or font array
//			: When using GCI fonts, the font file needs to be opened using gfx.Open4dFont
//			: When using font arrays, an optional parameter can be specified to indicate
//			: whether the font format is compressed or same as GCI
//

uint32_t gfx.bevelColor(colorb), 1; // Returns a darker and lighter colour of the given colour (colorb) by a set 18 steps
// Syntax	: gfx.bevelColor(colorb);
// Usage	: gfx.bevelColor(colorb);
// Notes 	: Returns a darker and lighter colour of the given colour (colorb) by a set 18 steps
//			: Darker colour is the HiWord (result >> 16)
//			: Lighter colour is the LoWord (result & 0xffff)
//

uint32_t gfx.HighlightColors(colorh, step), 1; // Returns a darker and lighter colour of the given colour (colorh) by the given steps
// Syntax	: gfx.HighlightColors(colorh, step);
// Usage	: gfx.HighlightColors(colorh, step);
// Notes 	: Returns a darker and lighter colour of the given colour (colorh) by the given steps
//			: Darker colour is the HiWord (result >> 16)
//			: Lighter colour is the LoWord (result & 0xffff)
//

uint16_t gfx.RGBs2COL(r, g, b), uint16_t; // Converts RGB888 to 16-bit RGB565 value
// Syntax	: gfx.RGBs2COL(r, g, b);
// Usage	: gfx.RGBs2COL(r, g, b);
// Notes 	: Converts RGB888 to 16-bit RGB565 value
//			: Compatible red green blue to RGB565 function from gen4-IoD
//

uint16_t gfx.RGBto565(rc, gc, bc), uint16_t; // Converts red green and blue colour elements to RGB565 16bit colour value
// Syntax	: gfx.RGBto565(rc, gc, bc);
// Usage	: gfx.RGBto565(rc, gc, bc);
// Notes 	: Converts red green and blue colour elements to RGB565 16bit colour value
//

void gfx.TextSize(s), void; // Increase the size of the selected font
// Syntax	: gfx.TextSize(s);
// Usage	: gfx.TextSize(s);
// Notes 	: Increase the size of the selected font
//

void gfx.TextColor(c [, b]), void; // Change text colour and, optionally, the background colour.
// Syntax	: gfx.TextColor(c);
//			: gfx.TextColor(c, b);
// Usage	: gfx.TextColor(LIME);
//			: gfx.TextColor(WHITE, BLACK);
// Notes 	: Change text colour and, optionally, the background colour.
//

void gfx.TextWrap(mode), void; // Enables or disables text wrap
// Syntax	: gfx.TextWrap(mode);
// Usage	: gfx.TextWrap(mode);
// Notes 	: Enables or disables text wrap
//

int8_t gfx.FontHeight(), 1; // Return height of current selected font without the size multiplier
// Syntax	: gfx.FontHeight();
// Usage	: gfx.FontHeight();
// Notes 	: Return height of current selected font without the size multiplier
//

void gfx.ButtonXstyle(bs), void; // Sets the style of primitive buttons
// Syntax	: gfx.ButtonXstyle(bs);
// Usage	: gfx.ButtonXstyle(bs);
// Notes 	: Sets the style of primitive buttons with the following defined styles
//			:       BUTTON_SQUARE
//			:       BUTTON_ROUNDED
//			:       BUTTON_CIRCULAR
//			:       BUTTON_CIRCULAR_TOP
//

void gfx.drawButton(updn, x, y, w, h, colorb, btext, tfont, tfontsize, tfontsizeht, tcolor [, compressed]), void; // Support function for ButtonX functions
// Syntax	: gfx.drawButton(updn, x, y, w, h, colorb, btext, tfont, tfontsize, tfontsizeht, tcolor, compressed);
//			: gfx.drawButton(updn, x, y, w, h, colorb, btext, tfont, tfontsize, tfontsizeht, tcolor);
// Usage	: gfx.drawButton(updn, x, y, w, h, colorb, btext, tfont, tfontsize, tfontsizeht, tcolor, compressed);
//			: gfx.drawButton(updn, x, y, w, h, colorb, btext, tfont, tfontsize, tfontsizeht, tcolor);
// Notes 	: Support function for ButtonX functions
//			: Please refer to library manual for more information
//

void gfx.Slider(state, x1, y1, x2, y2, colorb, colort, scale, value), void; // Draws a slider with diagonal at (x1, y1) and (x2, y2).
// Syntax	: gfx.Slider(state, x, y, r, b, colorb, colort, scale, value);
// Usage	: gfx.Slider(state, x, y, r, b, colorb, colort, scale, value);
// Notes 	: Draws a slider with diagonal at (x1, y1) and (x2, y2).
//			: x2 ad y2 should be greater than x1 and y1 respectively
//			: The thumb will be drawn depending on the specified scale and value.
//

void gfx.Button(state, x, y, colorb, tcolor, tfont, tfontsizeh, tfontsize, btext [, compressed]), void; // Draws a 3-dimensional Text Button at screen location defined by (x, y)
// Syntax	: gfx.Button(state, x, y, colorb, tcolor, tfont, tfontsizeh, tfontsize, btext, compressed);
// Usage	: gfx.Button(state, x, y, colorb, tcolor, tfont, tfontsizeh, tfontsize, btext, compressed);
// Notes 	: Draws a 3-dimensional Text Button at screen location defined by (x, y)
//			: The font to be used can be system font, GCI font (opened using gfx.Open4dFont) or font array
//			: When using font array, an optional parameter can be specified to indicate whether the format is compressed or GCI-like
//

void gfx.Buttonx(hndl, x, y, w, h, colorb, btext, tfont, tcolor [, compressed]), void; // Draws a 3-dimensional Text Button at screen location defined by (x, y) parameters
// Syntax	: gfx.Buttonx(hndl, x, y, w, h, colorb, btext, tfont, tcolor, compressed);
// Usage	: gfx.Buttonx(hndl, x, y, w, h, colorb, btext, tfont, tcolor, compressed);
// Notes 	: Draws a 3-dimensional Text Button at screen location defined by (x, y) parameters
//			: The user needs to specify a handler for the button that will be used by the functions:
//			:       gfx.ButtonUp, gfx.ButtonDown, gfx.ButtonActive, gfx.DeleteButton, gfx.CheckButtons
//			: The font to be used can be system font, GCI font (opened using gfx.Open4dFont) or font array
//			: When using font array, an optional parameter can be specified to indicate whether the format is compressed or GCI-like
//

void gfx.GradTriangleFilled(x0, y0, x1, y1, x2, y2, colour, ncCol, h, ypos, lev, erase), void; // Draw a filled triangle with or without a gradient.
// Syntax	: gfx.GradTriangleFilled(x0, y0, x1, y1, x2, y2, colour, ncCol, h, ypos, lev, erase);
// Usage	: gfx.GradTriangleFilled(x0, y0, x1, y1, x2, y2, colour, ncCol, h, ypos, lev, erase);
// Notes 	: Draw a filled triangle with or without a gradient.
//			: Please refer to the manual for more information
//

void gfx.HlineD(y, x1, x2, colour), void; // Draw fast horizontal line from (x1, y) to (x2, y)
// Syntax	: gfx.HlineD(y, x1, x2, colour);
// Usage	: gfx.HlineD(y, x1, x2, colour);
// Notes 	: Draw fast horizontal line from (x1, y) to (x2, y) - 4DGL compatible function
//

void gfx.HlineX(x, y, w, colour), void; // Draws a horizontal line to the current frame buffer with color parameter switch.
// Syntax	: gfx.HlineX(x, y, h, colour);
// Usage	: gfx.HlineX(10, 10, 100, GCI_IMAGE + 3);
//			: gfx.HlineX(10, 10, 100, FRAMEBUFFER_IMAGE + 3);
// Notes 	: Draws a horizontal line to the current frame buffer with color parameter switch.
//			: 'colour' can be a 16bit color or a GCI image or a FrameBuffer
//			:     ex. gfx.HlineX(10, 10, 100, GCI_IMAGE + 3) draws a horizontal line at 10, 10 and a length of 100 pixels
//			:         from GCI image index 3. x and y are relative to objects x and y.
//			:     ex. gfx.HlineX(10, 10, 100, FRAMEBUFFER_IMAGE + 3) draws a horizontal line at 10, 10 and a length of 100 pixels
//			:         from GCI image index 3 at the same x and y position.
//

void gfx.VlineD(x, y1, y2, colour), void; // Draw fast vertical line from (x1, y) to (x2, y)
// Syntax	: gfx.VlineD(x, y1, y2, colour);
// Usage	: gfx.VlineD(x, y1, y2, colour);
// Notes 	: Draw fast vertical line from (x, y1) to (x, y1) - 4DGL compatible function
//

void gfx.VlineX(x, y, w, colour), void; // Draws a vertical line to the current frame buffer with color parameter switch.
// Syntax	: gfx.VlineX(x, y, w, colour);
// Usage	: gfx.VlineX(10, 10, 100, GCI_IMAGE + 3);
//			: gfx.VlineX(10, 10, 100, FRAMEBUFFER_IMAGE + 3);
// Notes 	: Draws a vertical line to the current frame buffer with color parameter switch.
//			: 'colour' can be a 16bit color or a GCI image or a FrameBuffer
//			:     ex. gfx.VlineX(10, 10, 100, GCI_IMAGE + 3) draws a vertical line at 10, 10 and a length of 100 pixels
//			:         from GCI image index 3. x and y are relative to objects x and y.
//			:     ex. gfx.VlineX(10, 10, 100, FRAMEBUFFER_IMAGE + 3) draws a vertical line at 10, 10 and a length of 100 pixels
//			:         from GCI image index 3 at the same x and y position.
//

void gfx.gradientShape(vert, ow, xPos, yPos, w, h, r1, r2, r3, r4, darken, colour, sr1, gl1, colorD, sr3, gl3, gtb), void; // Draw a shaped color gradient using the supplied parameters
// Syntax	: gfx.gradientShape(vert, ow, xPos, yPos, w, h, r1, r2, r3, r4, darken, colour, sr1, gl1, colorD, sr3, gl3, gtb);
// Usage	: gfx.gradientShape(vert, ow, xPos, yPos, w, h, r1, r2, r3, r4, darken, colour, sr1, gl1, colorD, sr3, gl3, gtb);
// Notes 	: Draws a customizable shape with rounded corners and internal and external gradients
//			:   vert    - Horizontal or Vertical gradient -- 0 or 1
//			:   ow      - Outer gradient width
//			:   xPos    - x co-ordinate
//			:   yPos    - y co-ordinate
//			:   w       - width
//			:   h       - height
//			:   r1      - top left corner radius
//			:   r2      - top right corner radius
//			:   r3      - bottom left corner radius
//			:   r4      - bottom right corner radius
//			:   darken  - Darken both gradients by a value. Can be -ve to lighten
//			:   colour  - Outer gradient colour
//			:   sr1     - Outer gradient type (0 - 3 horizontal, +4 vertical) 0 - Raised, 1 - Sunken, 2 - Raised flatter middle, 3 - Sunken flatter middle
//			:   gl1     - Outer gradient level 0 - 63
//			:   colourD - Inner gradient colour
//			:   sr3     - Inner gradient type (0 - 3 horizontal, +4 vertical) 0 - Raised, 1 - Sunken, 2 - Raised flatter middle, 3 - Sunken flatter middle
//			:   gl3     - Inner gradient level 0 - 63
//			:   gtb     - Split gradient, 0 - no split, 1 - top, 2 - bottom
//

int gfx.Grad(GraisSunk, Gstate, Gglev, Gh1, Gpos, colToAdj), 1; // Support function for gradientShape
// Syntax	: gfx.Grad(GraisSunk, Gstate, Gglev, Gh1, Gpos, colToAdj);
// Usage	: gfx.Grad(GraisSunk, Gstate, Gglev, Gh1, Gpos, colToAdj);
// Notes 	: Support function for gradientShape
//

void gfx.TWprintln(value), void; // Prints the value to the TextWindow followed by new line
// Syntax	: gfx.TWprintln(value);
// Usage	: gfx.TWprintln(value);
// Notes 	: Prints the value to the TextWindow followed by new line
//

void gfx.TWprint(value), void; // Prints the value to the TextWindow
// Syntax	: gfx.TWprint(value);
// Usage	: gfx.TWprint(value);
// Notes 	: Prints the value to the TextWindow
//

string gfx.GetCommand(), string; // Retrieves the text entered in text window since previous carriage return sent
// Syntax	: gfx.GetCommand();
// Usage	: gfx.GetCommand();
// Notes 	: Retrieves the text entered in text window since previous carriage return sent
//

void gfx.TWtextcolor(twc), void; // Sets the print color when printing to the text window
// Syntax	: gfx.TWtextcolor(twc);
// Usage	: gfx.TWtextcolor(twc);
// Notes 	: Sets the print color when printing to the text window
//

boolean gfx.TWMoveTo(twcrx, twcry), 1; // Sets the cursor position for printing in the text window
// Syntax	: gfx.TWMoveTo(twcrx, twcry);
// Usage	: gfx.TWMoveTo(twcrx, twcry);
// Notes 	: Sets the cursor position for printing in the text window
//

void gfx.TWprintAt(pax, pay, istr), void; // Print a string at the specified position in the text window
// Syntax	: gfx.TWprintAt(pax, pay, istr);
// Usage	: gfx.TWprintAt(pax, pay, istr);
// Notes 	: Print a string at the specified position in the text window
//

void gfx.TWwrite(char), void; // Write a character to Text Window.
// Syntax	: gfx.TWwrite(char);
// Usage	: gfx.TWwrite(char);
// Notes 	: Write a character to TextWindow.
//

void gfx.TWcursorOn(twco), void; // Enables cursor in Text Window
// Syntax	: gfx.TWcursorOn(twco);
// Usage	: gfx.TWcursorOn(twco);
// Notes 	: Enables cursor in Text Window
//

void gfx.TWcls(), void; // Clear TextWindows of characters and reset cursor.
// Syntax	: gfx.TWcls();
// Usage	: gfx.TWcls();
// Notes 	: Clear TextWindows of characters and reset cursor.
//


void gfx.TWcolor(fcol [, bcol]), void; // Change TextWindow font colour 'fcol' and, optionally, TextWindow background colour 'bcol'.
// Syntax	: gfx.TWcolor(fcol, bcol);
//			: gfx.TWcolor(fcol);
// Usage	: gfx.TWcolor(fcol, bcol);
//			: gfx.TWcolor(fcol);
// Notes 	: Change TextWindow font colour 'fcol' and, optionally, TextWindow background colour 'bcol'.
//

void gfx.TextWindowImage(x, y, w, h, txtcolor, TWimg, frcolor), void; // Create Text window at x, y, with dimensions w, h and text txtcolor, GCI image TWimg, with optional frame colour frcolor to add frame.
// Syntax	: gfx.TextWindowImage(x, y, w, h, txtcolor, TWimg, frcolor);
// Usage	: gfx.TextWindowImage(x, y, w, h, txtcolor, TWimg, frcolor);
// Notes 	: Create Text window at x, y, with dimensions w, h and text txtcolor,
//			: GCI image TWimg, with optional frame colour frcolor to add frame.
//			: Background image will be automatically replaced with image data as text changes
//

void gfx.TextWindow(x, y, w, h, tc, bc [, fc]), void; // Create Text window at x, y, with dimensions w, h and text colour tc, background colour bc, with optional frame colour fc to add frame.
// Syntax	: gfx.TextWindow(x, y, w, h, tc, bc, fc);
//			: gfx.TextWindow(x, y, w, h, tc, bc);
// Usage	: gfx.TextWindow(x, y, w, h, tc, bc, fc);
//			: gfx.TextWindow(x, y, w, h, tc, bc);
// Notes 	: Create Text window at x, y, with dimensions w, h and text colour tc, background colour bc, with optional frame colour fc to add frame.
//

void gfx.TWenable(mode), void; // Enables/Disables drawing to the text window
// Syntax	: gfx.TWenable(mode);
// Usage	: gfx.TWenable(mode);
// Notes 	: Enables/Disables drawing to the text window
//

void gfx.TextWindowRestore(), void; // Restore a previously created TextWindow.
// Syntax	: gfx.TextWindowRestore();
// Usage	: gfx.TextWindowRestore();
// Notes 	: Restore a previously created TextWindow.
//

void gfx.Panel(x, y, w, h, c), void; // Draw raised Panel at x, y, with dimensions w and h & colour c.
// Syntax	: gfx.Panel(x, y, w, h, c);
// Usage	: gfx.Panel(x, y, w, h, c);
// Notes 	: Draw raised Panel at x, y, with dimensions w and h & colour c.
//

void gfx.PanelRecessed(x, y, w, h, c), void; // Draw recessed Panel at x, y, with dimensions w and h & colour c.
// Syntax	: gfx.PanelRecessed(x, y, w, h, c);
// Usage	: gfx.PanelRecessed(x, y, w, h, c);
// Notes 	: Draw recessed Panel at x, y, with dimensions w and h & colour c.
//

uint16_t gfx.getNumberofObjects(), uint16_t; // Get the number of GCI objects found in the GCI file opened with gfx.Open4dGFX
// Syntax	: gfx.getNumberofObjects();
// Usage	: gfx.getNumberofObjects();
// Notes 	: Get the number of GCI objects found in the GCI file opened with gfx.Open4dGFX
//

void gfx.ButtonActive(id, mode), void; // Enable/disable the specified button.
// Syntax	: gfx.ButtonActive(id, mode);
// Usage	: gfx.ButtonActive(id, mode);
// Notes 	: Enable/disable the specified button.
//

void gfx.FontStyle(ctyp), void; // Select font style for Font1 characters.
// Syntax	: gfx.FontStyle(ctyp);
// Usage	: gfx.FontStyle(ctyp);
// Notes 	: Select font style for Font1 characters.
//			: Adds effect to System Font1 with size of 3 or more.
//			: Valid styles are:
//			:       SOLID
//			:       DOTMATRIXROUND
//			:       DOTMATRIXLED
//			:       DOTMATRIXSQUARE
//			:       DOTMATRIXFADE
//

void gfx.DeleteButton(hndl [, bc]), void; // Delete previously created primitive button n and, optionally, redraw background in colour bc.
// Syntax	: gfx.DeleteButton(hndl, bc);
//			: gfx.DeleteButton(hndl);
// Usage	: gfx.DeleteButton(hndl, bc);
//			: gfx.DeleteButton(hndl);
// Notes 	: Delete previously created primitive button n and, optionally, redraw background in colour bc.
//

void gfx.DeleteButtonBG(hndl, bg), void; // Delete previously created primitive button n and, optionally, redraw section of background image bg.
// Syntax	: gfx.DeleteButtonBG(hndl, bg);
// Usage	: gfx.DeleteButtonBG(hndl, bg);
// Notes 	: Delete previously created primitive button n and, optionally, redraw section of background image bg.
//

void gfx.setAddrWindow(x, y, w, h), void; // Sets the GRAM window as specified by the top-left corner (x, y) and dimensions w and h
// Syntax	: gfx.setAddrWindow(x, y, w, h);
// Usage	: gfx.setAddrWindow(x, y, w, h);
// Notes 	: Sets the GRAM window as specified by the top-left corner (x, y) and dimensions w and h
//

void gfx.Orbit(angle, lngth, oxy), void; // Calculate x & y co-ordinate using given angle and length relative the current cursor position
// Syntax	: gfx.Orbit(angle, lngth, oxy);
// Usage	: gfx.Orbit(angle, lngth, oxy);
// Notes 	: Calculate x & y co-ordinate using given angle and length relative the current cursor position
//			: x and y coordinates are stored in integer 2 cell array oxy
//			: The current cursor position can be set using gfx.MoveTo
//

void gfx.UserImageHide(hndl [, colour]), void; // Hides the GCI widget using the specified colour or BLACK if no colour is specified
// Syntax	: gfx.UserImageHide(hndl, colour);
//			: gfx.UserImageHide(hndl);
// Usage	: gfx.UserImageHide(hndl, colour);
//			: gfx.UserImageHide(hndl);
// Notes 	: Hides the GCI widget using the specified colour or BLACK if no colour is specified
//			: Use -1 for hndl to apply to all GCI widgets
//			: For touch capable display modules, this also disables touch
//

void gfx.UserImageHideBG(hndl, objBG), void; // Hides the GCI widget by drawing the section of the GCI background objBG
// Syntax	: gfx.UserImageHideBG(hndl, objBG);
// Usage	: gfx.UserImageHideBG(hndl, objBG);
// Notes 	: Hides the GCI widget by drawing the section of the GCI background objBG
//			: For touch capable display modules, this also disables touch
//

boolean gfx.ScreenCapture(x, y, w, h, fname), 1; // Captures part of the screen specified by x, y, w and h and save the 16-bit color information with the filename specified
// Syntax	: gfx.ScreenCapture(x, y, w, h, fname);
// Usage	: gfx.ScreenCapture(x, y, w, h, fname);
// Notes 	: Captures part of the screen specified by x, y, w and h and save the 16-bit color information with the filename specified
//

void gfx.DrawToframebuffer(fbnum), void; // Set the frame buffer for drawing functions.
// Syntax	: gfx.DrawToframebuffer(fbnum);
// Usage	: gfx.DrawToframebuffer(fbnum);
// Notes 	: Set the frame buffer for drawing functions.
//			: Once set, all drawing functions will be sent to specified framebuffer.
//			: If frame buffer 0 is set (default) all drawing functions will appear immediately on the display.
//

void gfx.GetFrameBuffer(), uint8_t; // Get the number of the current frame buffer in use.
// Syntax	: gfx.GetFrameBuffer();
// Usage	: gfx.GetFrameBuffer();
// Notes 	: Returns current frame buffer number.
//

void gfx.SpriteAreaSet(x, y, x1, y1), void; // Sets the area of the screen that sprites will be displayed in.
// Syntax	: gfx.SpriteAreaSet(x, y, x1, y1);
// Usage	: gfx.SpriteAreaSet(x, y, x1, y1);
// Notes 	: Sets the area of the screen that sprites will be displayed in.
//

void gfx.SetSprite(num, active, x, y, bscolor, sdata), void; // Sets the position of sprite.
// Syntax	: gfx.SetSprite(num, active, x, y, bscolor, sdata);
// Usage	: gfx.SetSprite(num, active, x, y, bscolor, sdata);
// Notes 	: Sets the position of sprite.
//

void gfx.UpdateSprites(bscolor, sdata), void; // Updates all sprites in the previously set sprite area
// Syntax	: gfx.UpdateSprites(bscolor, sdata);
// Usage	: gfx.UpdateSprites(bscolor, sdata);
// Notes 	: Updates all sprites in the previously set sprite area
//

void gfx.SetNumberSprites(numspr), void; // Sets number of sprites in use
// Syntax	: gfx.SetNumberSprites(numspr);
// Usage	: gfx.SetNumberSprites(numspr);
// Notes 	: Sets number of sprites in use
//

void gfx.SpriteEnable(snum, sen), void; // Enables or disables chosen sprite
// Syntax	: gfx.SpriteEnable(snum, sen);
// Usage	: gfx.SpriteEnable(snum, sen);
// Notes 	: Enables or disables chosen sprite
//

void gfx.SpriteUpdate(tsx, tsy, tsx1, tsy1, bscolor, sdata), void; // Update all sprites in the selected area
// Syntax	: gfx.SpriteUpdate(tsx, tsy, tsx1, tsy1, bscolor, sdata);
// Usage	: gfx.SpriteUpdate(tsx, tsy, tsx1, tsy1, bscolor, sdata);
// Notes 	: Update all sprites in the selected area
//

void gfx.SetMaxNumberSprites(snos), void; // Sets the maximum number of sprites in the sprites list
// Syntax	: gfx.SetMaxNumberSprites(snos);
// Usage	: gfx.SetMaxNumberSprites(snos);
// Notes 	: Sets the maximum number of sprites in the sprites list
//

boolean gfx.SpriteInit(sdata, nums), 1; // Initialize the sprites data array
// Syntax	: gfx.SpriteInit(sdata, nums);
// Usage	: gfx.SpriteInit(sdata, sizeof(sdata));
// Notes 	: Initialize the sprites data array
//

boolean gfx.SpriteAdd(pos, snum, x, y, sdata), 1; // Add a sprite into a specified position in the sprite list.
// Syntax	: gfx.SpriteAdd(pos, snum, x, y, sdata);
// Usage	: gfx.SpriteAdd(pos, snum, x, y, sdata);
// Notes 	: Add a sprite into a specified position in the sprite list.
//			: snum is the number of the sprite in the sprite data.
//			: x and y specifies the sprites start position

uint16_t gfx.SpriteGetPixel(snum, xo, yo, tcolor, sdata), uint16_t; // Returns the colour of the pixel of the chosen sprite at x y position
// Syntax	: gfx.SpriteGetPixel(snum, xo, yo, tcolor, sdata);
// Usage	: gfx.SpriteGetPixel(snum, xo, yo, tcolor, sdata);
// Notes 	: Returns the colour of the pixel of the chosen sprite at x y position
//

uint16_t gfx.SpriteGetPalette(pnumber), uint16_t; // Return the 16 bit colour in the palette array position specified by pnumber
// Syntax	: gfx.SpriteGetPalette(pnumber);
// Usage	: gfx.SpriteGetPalette(pnumber);
// Notes 	: Return the 16 bit colour in the palette array position specified by pnumber
//

int gfx.GetSpritesAt(xo, yo, tcolor, slist, sdata), 1; // Queries the sprites at the current x y position to a created slist array
// Syntax	: gfx.GetSpritesAt(xo, yo, tcolor, slist, sdata);
// Usage	: gfx.GetSpritesAt(xo, yo, tcolor, slist, sdata);
// Notes 	: Queries the sprites at the current x y position to a created slist array
//

int gfx.GetSprite(snum, choice), 1; // Returns the selected sprite parameter.
// Syntax	: gfx.GetSprite(snum, choice);
// Usage	: gfx.GetSprite(snum, choice);
// Notes 	: Returns the selected parameter. Valid paramaters are:
//			:    SPRITE_X           - returns x position of sprite
//			:    SPRITE_Y           - returns y position of sprite
//			:    SPRITE_W           - returns width of sprite
//			:    SPRITE_H           - returns height of sprite
//			:    SPRITE_ACTIVE      - returns 1 if sprite enabled
//			:    SPRITE_COLLIDE1    - returns number of first collided sprite
//			:    SPRITE_COLLIDE2    - returns number of second collided sprite 
//

int gfx.GetNumberSprites(), 1; // Return the total number of initialized sprites
// Syntax	: gfx.GetNumberSprites();
// Usage	: gfx.GetNumberSprites();
// Notes 	: Return the total number of initialized sprites
//

int16_t gfx.GetSpriteImageNum(snum), uint16_t; // Returns the position of the chosen sprite in the sprite list
// Syntax	: gfx.GetSpriteImageNum(snum);
// Usage	: gfx.GetSpriteImageNum(snum);
// Notes 	: Returns the position of the chosen sprite in the sprite list
//

void gfx.SpriteSetPalette(pnumber, plcolor), void; // Set the colour in the 4 bit pallette specified by the colour number (pnumber) with 16bit colour (plcolor)
// Syntax	: gfx.SpriteSetPalette(pnumber, plcolor);
// Usage	: gfx.SpriteSetPalette(pnumber, plcolor);
// Notes 	: Set the colour in the 4 bit pallette specified by the colour number (pnumber) with 16bit colour (plcolor)
//

int16_t gfx.XYposToDegree(curX, curY), 1; // Returns the angular equivalent of the offset of x and y position from the center of the object
// Syntax	: gfx.XYposToDegree(curX, curY);
// Usage	: gfx.XYposToDegree(curX, curY);
// Notes 	: Returns the angular equivalent of the offset of x and y position from the center of the object
//

void gfx.Opacity(opacity), void; // Sets whether to enable or disable font background transparency
// Syntax	: gfx.Opacity(opacity);
// Usage	: gfx.Opacity(opacity);
// Notes 	: Sets whether to enable or disable font background transparency
//

void gfx.BGcolour(c), void; // Set text background colour 
// Syntax	: gfx.BGcolour(c);
// Usage	: gfx.BGcolour(c);
// Notes 	: Set text background colour
//

void gfx.FGcolour(c), void; // Set text foreground colour
// Syntax	: gfx.FGcolour(c);
// Usage	: gfx.FGcolour(c);
// Notes 	: Set text foreground colour
//

void gfx.putstr(strg), void; // Prints string to the current cursor position
// Syntax	: gfx.putstr(strg);
// Usage	: gfx.putstr(strg);
// Notes 	: Prints string to the current cursor position
//

void gfx.putstrXY(xpos, ypos, strg), void; // Prints string to the specified position
// Syntax	: gfx.putstrXY(xpos, ypos, strg);
// Usage	: gfx.putstrXY(xpos, ypos, strg);
// Notes 	: Prints string to the specified position
//

void gfx.putch(chr), void; // Prints a single character to the current cursor position
// Syntax	: gfx.putch(chr);
// Usage	: gfx.putch(chr);
// Notes 	: Prints a single character to the current cursor position
//

void gfx.putchXY(xpos, ypos, chr), void; // Prints a single character to the specified position
// Syntax	: gfx.putchXY(xpos, ypos, chr);
// Usage	: gfx.putchXY(xpos, ypos, chr);
// Notes 	: Prints a single character to the specified position
//

int gfx.charWidth(ch), 1; // Returns the width of the character with the font size multiplier
// Syntax	: gfx.charWidth(ch);
// Usage	: gfx.charWidth(ch);
// Notes 	: Returns the width of the character with the font size multiplier
//

int gfx.charHeight(ch), 1; // Returns the height of the character with the font size multiplier
// Syntax	: gfx.charHeight(ch);
// Usage	: gfx.charHeight(ch);
// Notes 	: Returns the height of the character with the font size multiplier
//

int gfx.strWidth(ts), 1; // Returns the width of the string with the font size multiplier
// Syntax	: gfx.strWidth(ts);
// Usage	: gfx.strWidth(ts);
// Notes 	: Returns the width of the string with the font size multiplier
//

int gfx.imageGetWord(img, controlIndex), 1; // Queries the current selected parameter of the GCI widget
// Syntax	: gfx.imageGetWord(img, controlIndex);
// Usage	: gfx.imageGetWord(img, controlIndex);
// Notes 	: Queries the current selected parameter of the GCI widget
//			: Parameters can be IMAGE_XPOS, IMAGE_YPOS, IMAGE_WIDTH, IMAGE_HEIGHT or IMAGE_INDEX
//

void gfx.imageSetWord(img, controlIndex, val1 [, val2]), void; // Sets the selected parameter(s) of the GCI widget
// Syntax	: gfx.imageSetWord(img, controlIndex, val1, val2);
//			: gfx.imageSetWord(img, controlIndex, val1);
// Usage	: gfx.imageSetWord(img, IMAGE_XYPOS, val1, val2);
//			: gfx.imageSetWord(img, controlIndex, val1);
// Notes 	: Sets the selected parameter(s) of the GCI widget
//			: Parameters can be IMAGE_XPOS, IMAGE_YPOS, IMAGE_XYPOS or IMAGE_INDEX
//			: When using IMAGE_XYPOS, two values are required
//

int16_t gfx.getImageValue(ui), 1; // Returns the current value of a GCI widget
// Syntax	: gfx.getImageValue(ui);
// Usage	: gfx.getImageValue(ui);
// Notes 	: Returns the current value of a GCI widget
//

void gfx.DownloadFile(WebAddr, Fname [, sha1]), void; // Downloads the file from the specified web address and save it with the specified filename.
// Syntax	: gfx.DownloadFile(WebAddr, Fname, sha1);
//			: gfx.DownloadFile(WebAddr, Fname);
// Usage	: gfx.DownloadFile(WebAddr, Fname, sha1);
//			: gfx.DownloadFile(WebAddr, Fname);
// Notes 	: Downloads the file from the specified web address and save it with the specified filename.
//			: Optionally, a certicate can be used
//

void gfx.DownloadFile(Address, port, hfile, Fname), void; // Downloads the file from the specified web address and port and save it with the specified filename.
// Syntax	: gfx.DownloadFile(Address, port, hfile, Fname);
// Usage	: gfx.DownloadFile(Address, port, hfile, Fname);
// Notes 	: Downloads the file from the specified web address and port and save it with the specified filename.
//

void gfx.Download(Address, port, hfile, Fname, certUsed, sha1), void; // Downloads the file from the specified web address and port and save it with the specified filename.
// Syntax	: gfx.Download(Address, port, hfile, Fname, certUsed, sha1);
// Usage	: gfx.Download(Address, port, hfile, Fname, certUsed, sha1);
// Notes 	: Downloads the file from the specified web address and port and save it with the specified filename.
//			: Options to use and specify a certificate is also available
//

void gfx.PrintImageFile(ifile), void; // Print GCI format image from GCI file at current cursor position.
// Syntax	: gfx.PrintImageFile(ifile);
// Usage	: gfx.PrintImageFile(ifile);
// Notes 	: Print GCI format image from GCI file at current cursor position.
//

void gfx.UserCharacter(data, l, x, y, fc, bc), void; // Draw user defined character using data array of size l at x, y, with foreground colour fc and background colour bc.
// Syntax	: gfx.UserCharacter(data, l, x, y, fc, bc);
// Usage	: gfx.UserCharacter(data, l, x, y, fc, bc);
// Notes 	: Draw user defined character using data array of size l at x, y, with foreground colour fc and background colour bc.
//

void gfx.UserCharacterBG(ui, data, ucsize, ucx, ucy, colour, draw), void; // Draw a user defined character with a specified (ui) gci image number being drawn as the character background
// Syntax	: gfx.UserCharacterBG(ui, data, ucsize, ucx, ucy, colour, draw);
// Usage	: gfx.UserCharacterBG(ui, data, ucsize, ucx, ucy, colour, draw);
// Notes 	: Draw a user defined character with a specified (ui) gci image number being drawn as the character background
//			: Setting draw to false will draw backround only.
//			: Setting draw to true will draw background and character.
//

void gfx.UserCharacterBG(data, ucsize, ucx, ucy, colour, draw, bgindex), void; // Draw user defined character using data array of size l at x, y, with foreground colour fc over previously drawn image.
// Syntax	: gfx.UserCharacterBG(data, ucsize, ucx, ucy, colour, draw, bgindex);
// Usage	: gfx.UserCharacterBG(data, ucsize, ucx, ucy, colour, draw, bgindex);
// Notes 	: Draw a user defined character with a downloaded gci image file being drawn as the character background.
//			: bgindex is set to 0 if a a single image exists in the gci image file or can be set to other images if they exist
//			: Setting draw to false will draw backround only.
//			: Setting draw to true will draw background and character.
//

boolean gfx.CheckSD(), 1; // Check if the uSD was properly mounted during gfx.begin
// Syntax	: gfx.CheckSD();
// Usage	: gfx.CheckSD();
// Notes 	: Check if the uSD was properly mounted during gfx.begin
//

boolean gfx.CheckDL(), 1; // Check if the last download performed was successful
// Syntax	: gfx.CheckDL();
// Usage	: gfx.CheckDL();
// Notes 	: Check if the last download performed was successful
//

void gfx.DrawImageFile(Fname), void; // Draw widget from GCI file
// Syntax	: gfx.DrawImageFile(Fname);
// Usage	: gfx.DrawImageFile(Fname);
// Notes 	: Draw widget from GCI file
//

void gfx.DrawImageArray(ImageArray), void; // Draw widget from GCI array
// Syntax	: gfx.DrawImageArray(ImageArray);
// Usage	: gfx.DrawImageArray(ImageArray);
// Notes 	: Draw widget from GCI array
//

void gfx.setCacheSize(cs), void; // Sets the cache size to use with gfx.UserImageDRcache
// Syntax	: gfx.setCacheSize(cs);
// Usage	: gfx.setCacheSize(cs);
// Notes 	: Sets the cache size to use with gfx.UserImageDRcache
//

int16_t gfx.getLastPointerPos(), 1; // Get the x or y value of the change position of 2 & 3 image widgets
// Syntax	: gfx.getLastPointerPos();
// Usage	: gfx.getLastPointerPos();
// Notes 	: Get the x or y value of the change position of 2 & 3 image widgets
//			: Returns x value if horizontal or y value if vertical.
//

void gfx.touch_Set(mode), void; // Enables or disable touch functionality
// Syntax	: gfx.touch_Set(mode);
// Usage	: gfx.touch_Set(TOUCH_ENABLE);
// Notes 	: Enables or disable touch functionality by specifying TOUCH_ENABLE or TOUCH_DISABLE
//			: This function is only available for capacitive and resistive touch displays
//

boolean touch_Update(), 1; // Checks whether a touch event is detected
// Syntax	: gfx.touch_Update();
// Usage	: gfx.touch_Update();
// Notes 	: Checks whether a touch event is detected
//			: If a touch event has occurred, pen, xpos, ypos and images touched will be updated
//			: This function is only available for capacitive and resistive touch displays
//

int16_t gfx.touch_GetPen(), 1; // Returns the latest pen status after the last gfx.touch_Update
// Syntax	: gfx.touch_GetPen();
// Usage	: gfx.touch_GetPen();
// Notes 	: Returns the latest pen status after the last gfx.touch_Update
//			: This function is only available for capacitive and resistive touch displays
//

int16_t gfx.touch_GetX(), 1; // Returns the latest touch X position after the last gfx.touch_Update
// Syntax	: gfx.touch_GetX();
// Usage	: gfx.touch_GetX();
// Notes 	: Returns the latest touch X position after the last gfx.touch_Update
//			: This function is only available for capacitive and resistive touch displays
//

int16_t gfx.touch_GetY(), 1; // Returns the latest touch Y position after the last gfx.touch_Update
// Syntax	: gfx.touch_GetY();
// Usage	: gfx.touch_GetY();
// Notes 	: Returns the latest touch Y position after the last gfx.touch_Update
//			: This function is only available for capacitive and resistive touch displays
//

int16_t gfx.touch_GetLastX(), 1; // Returns the previous touch X position prior to the last gfx.touch_Update
// Syntax	: gfx.touch_GetLastX();
// Usage	: gfx.touch_GetLastX();
// Notes 	: Returns the previous touch X position prior to the last gfx.touch_Update
//			: This function is only available for capacitive and resistive touch displays
//

int16_t gfx.touch_GetLastY(), 1; // Returns the previous touch Y position prior to the last gfx.touch_Update
// Syntax	: gfx.touch_GetLastY();
// Usage	: gfx.touch_GetLastY();
// Notes 	: Returns the previous touch Y position prior to the last gfx.touch_Update
//			: This function is only available for capacitive and resistive touch displays
//

void gfx.imageTouchEnable(gcinum, en [, type]), void; // Enable or disable touch for the specified GCI widget
// Syntax	: gfx.imageTouchEnable(gcinum, en, type);
// Usage	: gfx.imageTouchEnable(gcinum, en, type);
// Notes 	: Enable or disable touch for the specified GCI widget
//			: Optionally, a special type of input widget can be specified:
//			:       - TOGGLE4STATES
//			:       - SLIDER3IMAGE
//			:       - GAUGE2IMAGE
//			:       - MOMENTARY
//			:       - TOGGLE
//			:       - KEYPAD
//			: This function is only available for capacitive and resistive touch displays
//

void gfx.imageTouchEnableRange(gcinumFrom, gcinumTo, en [, type]), void; // Enable or disable touch for the specified range of GCI widget
// Syntax	: gfx.imageTouchEnableRange(gcinumFrom, gcinumTo, en, type);
// Usage	: gfx.imageTouchEnableRange(gcinumFrom, gcinumTo, en, type);
// Notes 	: Enable or disable touch for the specified range of GCI widget
//			: Optionally, a special type of input widget can be specified:
//			:       - TOGGLE4STATES
//			:       - SLIDER3IMAGE
//			:       - GAUGE2IMAGE
//			:       - MOMENTARY
//			:       - TOGGLE
//			:       - KEYPAD
//			: This function is only available for capacitive and resistive touch displays
//

void gfx.imageTouchEnable(gcinum, en, type, frames, orientation, gap1, gap2 [, tc]), void; // Enable or disable touch for the GCI widget
// Syntax	: gfx.imageTouchEnable(gcinum, en, type, frames, orientation, gap1, gap2, tc);
//			: gfx.imageTouchEnable(gcinum, en, type, frames, orientation, gap1, gap2);
// Usage	: gfx.imageTouchEnable(gcinum, en, type, frames, orientation, gap1, gap2, tc);
//			: gfx.imageTouchEnable(gcinum, en, type, frames, orientation, gap1, gap2);
// Notes 	: Enable or disable touch for the GCI widget
//			: Optionally, a special type of input widget can be specified:
//			:       - TOGGLE4STATES
//			:       - SLIDER3IMAGE
//			:       - GAUGE2IMAGE
//			:       - MOMENTARY
//			:       - TOGGLE
//			:       - KEYPAD
//			: This function is only available for capacitive and resistive touch displays
//

int16_t gfx.ImageTouchedAuto(), int16_t; // Evaluate touch events for the input widgets automatically
// Syntax	: gfx.ImageTouchedAuto();
// Usage	: gfx.ImageTouchedAuto();
// Notes 	: Evaluate touch events for the input widgets automatically
//			: gfx.touch_Update must be used prior to this function
//			: This function is only available for capacitive and resistive touch displays
//

int gfx.imageTouched(), 1; // Returns the touched widget or -1 if no touch
// Syntax	: gfx.imageTouched();
// Usage	: gfx.imageTouched();
// Notes 	: Returns the touched widget or -1 if no touch
//			: This function is only available for capacitive and resistive touch displays
//

int16_t gfx.imageAutoSlider(ui, axis, uiv, ming, maxg), 1; // Evaluate and return the new value of the slider based on touch position
// Syntax	: gfx.imageAutoSlider(ui, axis, uiv, ming, maxg);
// Usage	: gfx.imageAutoSlider(ui, axis, uiv, ming, maxg);
// Notes 	: Evaluate and return the new value of the slider based on touch position
//			: This function is only available for capacitive and resistive touch displays
//

int16_t gfx.imageAutoKnob(hndl, uix, uiy, minarc, maxarc, ming, maxg), 1; // Evaluate and return the new value of the knob based on touch position
// Syntax	: gfx.imageAutoKnob(hndl, uix, uiy, minarc, maxarc, ming, maxg);
// Usage	: gfx.imageAutoKnob(hndl, uix, uiy, minarc, maxarc, ming, maxg);
// Notes 	: Evaluate and return the new value of the knob based on touch position
//			: This function is only available for capacitive and resistive touch displays
//

uint8_t gfx.CheckButtons(), uint8_t; // Evaluate the button touches and return the buttonx touched
// Syntax	: gfx.CheckButtons();
// Usage	: gfx.CheckButtons();
// Notes 	: Evaluate the button touches and return the buttonx touched
//			: This function is only available for capacitive and resistive touch displays
//

uint16_t gfx.GetSliderValue(ui, axis, uiv, ming, maxg), uint16_t; // Calculate the value of slider based on touch position
// Syntax	: gfx.GetSliderValue(ui, axis, uiv, ming, maxg);
// Usage	: gfx.GetSliderValue(ui, axis, uiv, ming, maxg);
// Notes 	: Calculate the value of slider based on touch position
//			: This function is only available for capacitive and resistive touch displays
//

int gfx.DecodeKeypad(kpad, kpress, kbks, kbck), 1; // Decodes the key pressed in the keyboard
// Syntax	: gfx.DecodeKeypad(kpad, kpress, kbks, kbck);
// Usage	: gfx.DecodeKeypad(kpad, kpress, kbks, kbck);
// Notes 	: Decodes the key pressed in the keyboard
//			: This function is only available for capacitive and resistive touch displays
//

void gfx.ResetKeypad(), void; // Resets the state of the keypad
// Syntax	: gfx.ResetKeypad();
// Usage	: gfx.ResetKeypad();
// Notes 	: Resets the state of the keypad
//			: This function is only available for capacitive and resistive touch displays
//

boolean gfx.KeypadStatus(keyType), 1; // Checks whether the keypad state is on SHIFT, CAPSLOCK or CTRL
// Syntax	: gfx.KeypadStatus(keyType);
// Usage	: gfx.KeypadStatus(keyType);
// Notes 	: Checks whether the keypad state is on SHIFT, CAPSLOCK or CTRL
//			: This function is only available for capacitive and resistive touch displays
//

int gfx.SpriteTouched(), int; // Returns the id of the sprite touched
// Syntax	: gfx.SpriteTouched();
// Usage	: gfx.SpriteTouched();
// Notes 	: Returns the id of the sprite touched
//			: This function is only available for capacitive and resistive touch displays
//

void gfx.touchCalibration(), void; // Starts touch calibration for resistive touch displays
// Syntax	: gfx.touchCalibration();
// Usage	: gfx.touchCalibration();
// Notes 	: Starts touch calibration for resistive touch displays
//			: This is only available for resistive touch displays
//

uint8_t gfx.touch_GetInvertMode(), uint8_t; // Returns the inversion status of the touch controller.
// Syntax	: gfx.touch_GetInvertMode();
// Usage	: gfx.touch_GetInvertMode();
// Notes 	: Returns the inversion status of the touch controller.
//			:   (0) DISP_TOUCH_NORMAL is no inversion
//			:   (1) DISP_TOUCH_INVERT_Y has Y axis inverted
//			:   (2) DISP_TOUCH_INVERT_X has X axis inverted
//			:   (3) DISP_TOUCH_INVERT_XY has X and Y inverted
//

void gfx.RTCinit(), void; // Initializes the RTC module
// Syntax	: gfx.RTCinit();
// Usage	: gfx.RTCinit();
// Notes 	: Initializes the RTC module
//

void gfx.RTCstartClock(), void; // Starts the clock
// Syntax	: gfx.RTCstartClock();
// Usage	: gfx.RTCstartClock();
// Notes 	: Starts the clock
//

void gfx.RTCstopClock(), void; // Stops the clock
// Syntax	: gfx.RTCstopClock();
// Usage	: gfx.RTCstopClock();
// Notes 	: Stops the clock
//

bool gfx.RTCcheckClockIntegrity(), 1; // Checks for clock integrity
// Syntax	: gfx.RTCcheckClockIntegrity();
// Usage	: gfx.RTCcheckClockIntegrity();
// Notes 	: Checks for clock integrity
//

void gfx.RTCsetYear(year), void; // Sets the year
// Syntax	: gfx.RTCsetYear(year);
// Usage	: gfx.RTCsetYear(2023);
// Notes 	: Sets the year
//

void gfx.RTCsetMonth(month), void; // Sets the month (1 to 12)
// Syntax	: gfx.RTCsetMonth(month);
// Usage	: gfx.RTCsetMonth(9);
// Notes 	: Sets the month (1 to 12)
//

void gfx.RTCsetDay(day), void; // Sets the day (1 to 31)
// Syntax	: gfx.RTCsetDay(day);
// Usage	: gfx.RTCsetDay(5);
// Notes 	: Sets the day (1 to 31)
//

void gfx.RTCsetHour(hour), void; // Sets the hour (0 to 23)
// Syntax	: gfx.RTCsetHour(hour);
// Usage	: gfx.RTCsetHour(14);
// Notes 	: Sets the hour (0 to 23)
//

void gfx.RTCsetMinute(minute), void; // Sets the minutes (0 to 59)
// Syntax	: gfx.RTCsetMinute(minute);
// Usage	: gfx.RTCsetMinute(42);
// Notes 	: Sets the minutes (0 to 59)
//

void gfx.RTCsetSecond(second), void; // Sets the seconds (0 to 59)
// Syntax	: gfx.RTCsetSecond(second);
// Usage	: gfx.RTCsetSecond(57);
// Notes 	: Sets the seconds (0 to 59)
//

Time gfx.RTCgetTime(), Time; // Queries the current time
// Syntax	: gfx.RTCgetTime();
// Usage	: gfx.RTCgetTime();
// Notes 	: Queries the current time and return as Time struct
//

const char * gfx.RTCformatDateTime(style), 1; // Formats the time and return a pointer to the string
// Syntax	: gfx.RTCformatDateTime(style);
// Usage	: gfx.RTCformatDateTime(RTC_TIMEFORMAT_HMS);
// Notes 	: Formats the time and return a pointer to the string
//			: Accepted formats/styles:
//			:   - RTC_TIMEFORMAT_HM
//			:   - RTC_TIMEFORMAT_HMS
//			:   - RTC_TIMEFORMAT_YYYY_MM_DD
//			:   - RTC_TIMEFORMAT_MM_DD_YYYY
//			:   - RTC_TIMEFORMAT_DD_MM_YYYY
//			:   - RTC_TIMEFORMAT_YYYY_MM_DD_H_M_S
//			:   - RTC_TIMEFORMAT_DD_MM_YYYY_H_M_S
//			: If an invalid format is specified, this will default to RTC_TIMEFORMAT_HM
//

void gfx.AlphaBlend(mode), void; // Enables Alpha Blending on all drawing functions
// Syntax	: gfx.AlphaBlend(mode);
// Usage	: gfx.AlphaBlend(true);
//			: gfx.AlphaBlend(ON);
// Notes 	: Enables Alpha Blending of new foreground colour to existing background colour
//

void gfx.AlphaBlendLevel(value), void; // Sets the level of blend intensity
// Syntax	: gfx.AlphaBlendLevel(value);
// Usage	: gfx.AlphaBlendLevel(127);
// Notes 	: Sets the level of alpha blending between new foreground colour and existing background colour
//			: 0 - lowest intensity, 255 highest intensity (overwrite of pixel colour)
//			: gfx.AlphaBlend must be used and set to 'true' or 'ON' for this to take effect
//

void gfx.DisplayControl(value), void; // Executes various Display initialization functions
// Syntax	: gfx.DisplayControl(value);
// Usage	: gfx.DisplayControl(DISP_CTRL_RE_INIT);
// Notes 	: Executes various Display control functions
//			: DISP_CTRL_RE_INIT
//			: DISP_CTRL_RESET
//			: DISP_CTRL_NEW
//			: DISP_CTRL_INIT
//			: DISP_CTRL_STOP
//			: DISP_CTRL_START_TX
//			: DISP_CTRL_DEL
//			: DISP_CTRL_START
//			: DISP_CTRL_FLUSH
//

void gfx.WriteToFrameBuffer(offset, data, len), void; // Writes 8bit or 16bit colour data array directly to current frame buffer
// Syntax	: gfx.WriteToFrameBuffer(offset, data, len);
// Usage	: gfx.WriteToFrameBuffer(0, colour_data, 100);
// Notes 	: Writes colour data array directly to the current frame buffer at given offset.
//			: Needs a gfx.FlushArea(x1, x2, y1, y2, -1); after write to display new data.
//

void gfx.FlushArea(x1, x2, y1, y2, xpos), void; // Flushes display area
// Syntax	: gfx.FlushArea(x1, x2, y1, y2, xpos);
// Usage	: gfx.FlushArea(10, 100, 10, 100, -1);
// Notes	: xpos is set to -1 for normal use or set to a value to refresh one pixel at xpos position.
//