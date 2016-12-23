1. Install <Adafruit_GFX.h> library from GitHub into your User libraries folder if not already there.

2. Unzip the attached "MCUFRIEND_kbv.zip" into your User libraries folder.

3. Insert your Mcufriend style display shield into UNO.

4. Start your Arduino IDE.

5. Build any of the Examples from the File->Examples->Mcufriend_kbv menu.
Most of them will write some text to the Serial window (9600 baud).   The BMP examples require you to copy the pictures from the bitmaps/ folder to your microSD (root directory)

6. This library is only intended for UNO and these specific Shields.   It will work on a MEGA2560 but not very fast.

7. The constructor takes no arguments (because it only works with these shields)

8. The examples are the standard Adafruit ones.   You can see my edits by searching for "kbv"

9. Any Adafruit sketch should work with the MCUFRIEND_kbv constructor() but should allow extra ID values
An Adafruit constructor(cs, rs, wr, rd, rst) IGNORES any arguments.   i.e. it only uses the shield control pins

10. It currently supports UNO shields with "mcufriend.com" pcbs with controllers:
----- HX8347-A 240x320  ID=0x8347 new Untested
ILI9320  240x320  ID=0x9320
ILI9325  240x320  ID=0x9325
ILI9327  240x400  ID=0x9327
ILI9329  240x320  ID=0x9329 
ILI9335  240x320  ID=0x9335 
ILI9341  240x320  ID=0x9341 
ILI9481  320x480  ID=0x9481  
ILI9486  320x480  ID=0x9486
ILI9488  320x480  ID=0x9488
LGDP4535 240x320  ID=0x4535
RM68090  240x320  ID=0x6809
R61505V  240x320  ID=0xB505
R61505W  240x320  ID=0xC505 new Untested
R61509V  240x400  ID=0xB509
----- S6D0139  240x320  ID=0x0139 removed due to lack of tester
S6D0154  240x320  ID=0x0154
SPFD5408 240x320  ID=0x5408
----- SSD1963  800x480  ID=0x1963 new Untested
SSD1289  240x320  ID=0x1289 
ST7781   240x320  ID=0x7783
ST7789V  240x320  ID=0x7789

11. It should run on a UNO, MEGA2560, LEONARDO, DUE, ZERO and M0-PRO.

12. These Mcufriend-style shields tend to have a resistive TouchScreen on A1, 7, A2, 6 but are not always the same rotation of direction.
Run the TouchScreen_Calibr_kbv.ino sketch to diagnose your model and then scale the result from TouchScreen.h methods()
The Resistive TouchScreen does not work on the Due.   It seems to be upset by sharing pins.  I have discovered why.   
(You need a new Touch library.)

13. The aspect_kbv.ino sketch should show the 4 different rotations.   Please report the results for an ILI9335, ILI9327. 

14. The scroll_kbv.ino sketch should scroll a window or subwindow for most chips.   Not all chips can scroll.

15. The readpixel_kbv.ino sketch should display memory in each aspect. 

16. The GLUE_Demo_320x240.ino sketch uses a "GLUE" class to display a UTFT sketch on supported mcufriend shields.   
    It is NOT perfect.   Please report any problems.    It is designed as a CONVENIENCE for legacy UTFT code.   
    Please use MCUFRIEND_kbv method()s in new code. 

CHANGE HISTORY:

Will Never Happen:
    ILI9327 vertical scroll will ALWAYS be limited in Landscape and Portrait_Rev
    ST7781 vert scroll is not implemented
    ILI9320, 9325, ... can never vert scroll sub-window 
    Itead CPLD with ID=0x3082 ... will never work

To Be Fixed:
    GLUE Demo does not work with Due and IDE v1.6.6

v2.8: posted 26 February 2016
    MCUFRIEND_kbv.cpp:  LANDSCAPE is now 90 degrees instead of 270 degrees
	                    methods branch on MIPI / 9320 style
						Added SPFD5408
						Added R61505W     Untested
						Added HX8347-A    Untested
						Added SSD1963     Untested
    graphictest_kbv.ino: smaller buffer for Software Scroll.  Just fits in Uno Flash
    TouchScreen_Calibr_kbv: wait for consistent Touch
	LCD_ID_readreg:     Added some more registers
	
v2.7: posted ........ NOT YET
v2.6: posted ........ NOT YET
    MCUFRIEND_kbv.cpp:  
                        Added R65105V
						
v2.5: posted 11 January 2016
    MCUFRIEND_kbv.cpp:  
                        Added ST7789V
                        Added RM68090
                        Added ILI9481
                        Added pushColors() from Flash
                        Improved downward Vertical Scroll.   API is changed.
                        ILI9327 should work in all aspects. vertical scroll in PORT/LAND_REV
                        S6D0154 should vertical scroll a subwindow
                                            
    graphictest_kbv.ino: smooth colour gradation on rounded rectangle tests on bigger displays
                        added colour grading range.
                        added vertical scroll down
                        improve messages
    
    graphictest_slim.ino: reduced-fat version of graphictest_kbv.ino
    testcard_kbv.ino:   drawn version of common BMP photo.
	scroll_kbv.ino:     changed vertScroll() call for new API    
    
    UTFTGLUE.h:         improve calculate width and height of rectangles from coordinates.
    
v2.4: posted 10 December 2015
    MCUFRIEND_kbv.cpp:  LGDP4535 initialisation table fixed - UNTESTED 
    
v2.3: posted 8 December 2015
    MCUFRIEND_kbv.cpp:  added S6D0139 UNTESTED
                        detect EXTC=0 ILI9341 shield from DX with ID == 0x0000.   drive in RGB mode
                        ILI9327 should Vertical Scroll now. UNTESTED
    UTFTGLUE.h:         extra print(const char*) method to avoid warning messages
                        
    graphictest_kbv.ino: software scroll a smaller block at top of screen
    GLUE_Demo_320x240:  removed unnecessary "include Adafruit_TFTLCD.h" 
    aspect_kbv.ino:     invert display when in different aspects 
    readpixel_kbv.ino:  support smaller geometries
    LCD_ID_readreg.ino: report some more regs
    TouchScreen_Calibr_kbv: for resistive Touch screens like on Mcufriend shields
    UTouch_Calibr_kbv:  can use UTouch.h library for hardware Touch Controllers (and UTFT)

v2.2: posted 15 November 2015
    MCUFRIEND_kbv.cpp:  changed readGRAM() for ILI9327, ILI9486,
                        added REV_SCREEN flag to _lcd_capable
                        implement invertDisplay()
                        added LGDP4535
                        ILI9327: set Partial Area to 0, 399                        
    MCUFRIEND_kbv.h:    USE_GFX_KBV default argument: println(char *s="")
    MCUFRIEND_shield.h: regular Uno shield drivers for Uno, Mega, Leonardo, Due, Zero
    MCUFRIEND_special.h:  experimental drivers
    UTFTGLUE.h:         identity kludges for non-UNO shields
    LCD_ID_readreg.ino: report reg(0x09)

v2.1: posted 9 November 2015
    MCUFRIEND_kbv.cpp:  return 0x9327 / 0x9481 in readID()
    MCUFRIEND_shield.h: increase low pulse width in RD_STROBE
    EXAMPLES/:          add scroll_kbv.ino sketch
                        edit readpixel_kbv_ino,  aspect_kbv.ino

v2.0: posted 8 November 2015
    Massive changes from v1.0


