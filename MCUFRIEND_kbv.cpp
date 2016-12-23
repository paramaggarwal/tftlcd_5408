#include "MCUFRIEND_kbv.h"
// #include "mcufriend_uno.h"
#include "mcufriend_stm32.h"

#define wait_ms(ms)  delay(ms)
#define READ_BGR        (1<<1)
#define INVERT_GS       (1<<2)
#define REV_SCREEN      (1<<3)

MCUFRIEND_kbv::MCUFRIEND_kbv():Adafruit_GFX(240, 320)
{
    // we can not access GPIO pins until AHB has been enabled.
}

static uint8_t done_reset, is8347;

void MCUFRIEND_kbv::reset(void)
{
    done_reset = 1;
    setWriteDir();
    CTL_INIT();
    CS_IDLE;
    RD_IDLE;
    WR_IDLE;
    RESET_IDLE;
    wait_ms(50);
    RESET_ACTIVE;
    wait_ms(100);
    RESET_IDLE;
    wait_ms(100);
}

void MCUFRIEND_kbv::WriteCmdData(uint16_t cmd, uint16_t dat)
{
    CS_ACTIVE;
    WriteCmd(cmd);
    WriteData(dat);
    CS_IDLE;
}

static void WriteCmdParamN(uint16_t cmd, int8_t N, uint8_t * block)
{
    CS_ACTIVE;
    WriteCmd(cmd);
    while (N-- > 0) {
        uint8_t u8 = *block++;
        CD_DATA;
        write8(u8);
        if (N && is8347) {
            cmd++;
            WriteCmd(cmd);
        }
    }
    CS_IDLE;
}

static inline void WriteCmdParam4(uint8_t cmd, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4)
{
    uint8_t d[4];
    d[0] = d1, d[1] = d2, d[2] = d3, d[3] = d4;
    WriteCmdParamN(cmd, 4, d);
}

static uint16_t read16bits(void)
{
    uint16_t ret;
    uint8_t lo;
    READ_8(ret);

    READ_8(lo);
    return (ret << 8) | lo;
}

uint16_t MCUFRIEND_kbv::readReg(uint16_t reg)
{
    uint16_t ret;
    uint8_t lo;
    if (!done_reset)
        reset();
    CS_ACTIVE;
    WriteCmd(reg);
    setReadDir();
    CD_DATA;
    //    READ_16(ret);
    ret = read16bits();
    RD_IDLE;
    CS_IDLE;
    setWriteDir();
    return ret;
}

uint32_t MCUFRIEND_kbv::readReg32(uint16_t reg)
{
    uint16_t h, l;
    CS_ACTIVE;
    WriteCmd(reg);
    setReadDir();
    CD_DATA;
    h = read16bits();
    l = read16bits();
    RD_IDLE;
    CS_IDLE;
    setWriteDir();
    return ((uint32_t) h << 16) | (l);
}

uint16_t MCUFRIEND_kbv::readID(void)
{    
    return readReg(0);
}

 // independent cursor and window registers.   S6D0154, ST7781 increments.  ILI92320/5 do not.  
int16_t MCUFRIEND_kbv::readGRAM(int16_t x, int16_t y, uint16_t * block, int16_t w, int16_t h)
{
    uint16_t ret, dummy, _MR = _MW;
    int16_t n = w * h, row = 0, col = 0;
    uint8_t r, g, b, tmp;

    setAddrWindow(x, y, x + w - 1, y + h - 1);
    while (n > 0) {
        WriteCmdData(_MC, x + col);
        WriteCmdData(_MP, y + row);

        CS_ACTIVE;
        WriteCmd(_MR);
        setReadDir();
        CD_DATA;
        READ_16(dummy);

        while (n) {
            READ_16(ret);
            ret = (ret & 0x07E0) | (ret >> 11) | (ret << 11);
            *block++ = ret;
            n--;
            break;
        }
        if (++col >= w) {
            col = 0;
            if (++row >= h)
                row = 0;
        }
        RD_IDLE;
        CS_IDLE;
        setWriteDir();
    }

    setAddrWindow(0, 0, width() - 1, height() - 1);
    return 0;
}

void MCUFRIEND_kbv::setRotation(uint8_t r)
{
    uint16_t GS, SS, ORG, REV = _lcd_rev;
    uint8_t val, d[3];
    rotation = r & 3;           // just perform the operation ourselves on the protected variables
    _width = (rotation & 1) ? HEIGHT : WIDTH;
    _height = (rotation & 1) ? WIDTH : HEIGHT;
    switch (rotation) {
    case 0:                    //PORTRAIT:
        val = 0x48;             //MY=0, MX=1, MV=0, ML=0, BGR=1
        break;
    case 1:                    //LANDSCAPE: 90 degrees
        val = 0x28;             //MY=0, MX=0, MV=1, ML=0, BGR=1
        break;
    case 2:                    //PORTRAIT_REV: 180 degrees
        val = 0x98;             //MY=1, MX=0, MV=0, ML=1, BGR=1
        break;
    case 3:                    //LANDSCAPE_REV: 270 degrees
        val = 0xF8;             //MY=1, MX=1, MV=1, ML=1, BGR=1
        break;
    }

    if (_lcd_capable & INVERT_GS)
        val ^= 0x80;

    _MC = 0x20, _MP = 0x21, _MW = 0x22, _SC = 0x50, _EC = 0x51, _SP = 0x52, _EP = 0x53;
    GS = (val & 0x80) ? (1 << 15) : 0;
    WriteCmdData(0x60, GS | 0x2700);    // Gate Scan Line (0xA700)

    SS = (val & 0x40) ? (1 << 8) : 0;
    WriteCmdData(0x01, SS);     // set Driver Output Control

    ORG = (val & 0x20) ? (1 << 3) : 0;
    if (val & 0x08)
        ORG |= 0x1000;  //BGR
    _lcd_madctl = ORG | 0x0030;
    WriteCmdData(0x03, _lcd_madctl);    // set GRAM write direction and BGR=1.

    setAddrWindow(0, 0, width() - 1, height() - 1);
    vertScroll(0, HEIGHT, 0);   //reset scrolling after a rotation
}

void MCUFRIEND_kbv::drawPixel(int16_t x, int16_t y, uint16_t color)
{
    // MCUFRIEND just plots at edge if you try to write outside of the box:
    if (x < 0 || y < 0 || x >= width() || y >= height())
        return;

    WriteCmdData(_MC, x);
    WriteCmdData(_MP, y);
    WriteCmdData(_MW, color);
}

void MCUFRIEND_kbv::setAddrWindow(int16_t x, int16_t y, int16_t x1, int16_t y1)
{
    WriteCmdData(_MC, x);
    WriteCmdData(_MP, y);
    WriteCmdData(_SC, x);
    WriteCmdData(_SP, y);
    WriteCmdData(_EC, x1);
    WriteCmdData(_EP, y1);
}

void MCUFRIEND_kbv::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    int16_t end;
    if (w < 0) {
        w = -w;
        x -= w;
    }                           //+ve w
    end = x + w;
    if (x < 0)
        x = 0;
    if (end > width())
        end = width();
    w = end - x;
    if (h < 0) {
        h = -h;
        y -= h;
    }                           //+ve h
    end = y + h;
    if (y < 0)
        y = 0;
    if (end > height())
        end = height();
    h = end - y;

    setAddrWindow(x, y, x + w - 1, y + h - 1);
    CS_ACTIVE;
    WriteCmd(_MW);
    if (h > w) {
        end = h;
        h = w;
        w = end;
    }
    uint8_t hi = color >> 8, lo = color & 0xFF;
    CD_DATA;
    while (h-- > 0) {
        end = w;
        do {
            write8(hi);
            write8(lo);
        } while (--end != 0);
    }
    CS_IDLE;
    setAddrWindow(0, 0, width() - 1, height() - 1);
}

void MCUFRIEND_kbv::pushColors(uint16_t * block, int16_t n, bool first)
{
    uint16_t color;
    CS_ACTIVE;
    if (first) {
        WriteCmd(_MW);
    }
    CD_DATA;
    while (n-- > 0) {
        color = *block++;
        write16(color);
    }
    CS_IDLE;
}

void MCUFRIEND_kbv::pushColors(uint8_t * block, int16_t n, bool first)
{
    uint16_t color;
    uint8_t h, l;
    CS_ACTIVE;
    if (first) {
        WriteCmd(_MW);
    }
    CD_DATA;
    while (n-- > 0) {
        h = (*block++);
        l = (*block++);
        color = h << 8 | l;
        write16(color);
    }
    CS_IDLE;
}

void MCUFRIEND_kbv::pushColors(const uint8_t * block, int16_t n, bool first)
{
    uint16_t color;
    uint8_t h, l;
    CS_ACTIVE;
    if (first) {
        WriteCmd(_MW);
    }
    CD_DATA;
    while (n-- > 0) {
        l = pgm_read_byte(block++);
        h = pgm_read_byte(block++);
        color = h << 8 | l;
        write16(color);
    }
    CS_IDLE;
}

void MCUFRIEND_kbv::vertScroll(int16_t top, int16_t scrollines, int16_t offset)
{
    int16_t vsp;
    vsp = top + offset; // vertical start position
    if (offset < 0)
        vsp += scrollines;          //keep in unsigned range

    WriteCmdData(0x61, (1 << 1) | _lcd_rev);        //!NDL, VLE, REV
    WriteCmdData(0x6A, vsp);        //VL#
}

void MCUFRIEND_kbv::invertDisplay(boolean i)
{
    uint8_t val;
    _lcd_rev = ((_lcd_capable & REV_SCREEN) != 0) ^ i;
    WriteCmdData(0x61, _lcd_rev);
}

#define TFTLCD_DELAY 0xFFFF
#define TFTLCD_DELAY8 0xFF
static void init_table(const void *table, int16_t size)
{
    uint8_t *p = (uint8_t *) table, dat[16];
    while (size > 0) {
        uint8_t cmd = pgm_read_byte(p++);
        uint8_t len = pgm_read_byte(p++);
        if (cmd == TFTLCD_DELAY8) {
            delay(len);
            len = 0;
        } else {
            for (uint8_t i = 0; i < len; i++)
                dat[i] = pgm_read_byte(p++);
            WriteCmdParamN(cmd, len, dat);
        }
        size -= len + 2;
    }
}

static void init_table16(const void *table, int16_t size)
{
    uint16_t *p = (uint16_t *) table;
    while (size > 0) {
        uint16_t cmd = pgm_read_word(p++);
        uint16_t d = pgm_read_word(p++);
        if (cmd == TFTLCD_DELAY)
            delay(d);
        else {
            CS_ACTIVE;
            WriteCmd(cmd);
            WriteData(d);
            CS_IDLE;
        }
        size -= 2 * sizeof(int16_t);
    }
}

void MCUFRIEND_kbv::begin(uint16_t ID)
{
    int16_t *p16;               //so we can "write" to a const protected variable.
    reset();
    _lcd_xor = 0;
    if (ID == 0)
        ID = readID();
    switch (_lcd_ID = ID) {
    case 0x5408:
        _lcd_capable = 0 | REV_SCREEN | READ_BGR | INVERT_GS;
        static const uint16_t regValues[] PROGMEM = {
            0x00e5, 0x8000,
            0x0000, 0x0001,
            0x0001, 0x100,
            0x0002, 0x0700,
            0x0003, 0x1030,
            0x0004, 0x0000,
            0x0008, 0x0202,
            0x0009, 0x0000,
            0x000A, 0x0000,
            0x000C, 0x0000,
            0x000D, 0x0000,
            0x000F, 0x0000,
            //-----Power On sequence-----------------------
            0x0010, 0x0000,
            0x0011, 0x0007,
            0x0012, 0x0000,
            0x0013, 0x0000,
            TFTLCD_DELAY, 50,
            0x0010, 0x17B0,
            0x0011, 0x0007,
            TFTLCD_DELAY, 10,
            0x0012, 0x013A,
            TFTLCD_DELAY, 10,
            0x0013, 0x1A00,
            0x0029, 0x000c,
            TFTLCD_DELAY, 10,
            //-----Gamma control-----------------------
            0x0030, 0x0000,
            0x0031, 0x0505,
            0x0032, 0x0004,
            0x0035, 0x0006,
            0x0036, 0x0707,
            0x0037, 0x0105,
            0x0038, 0x0002,
            0x0039, 0x0707,
            0x003C, 0x0704,
            0x003D, 0x0807,
            //-----Set RAM area-----------------------
            0x0050, 0x0000,
            0x0051, 0x00EF,
            0x0052, 0x0000,
            0x0053, 0x013F,
            0x0060, 0xA700,     //GS=1
            0x0061, 0x0001,
            0x006A, 0x0000,
            0x0021, 0x0000,
            0x0020, 0x0000,
            //-----Partial Display Control------------
            0x0080, 0x0000,
            0x0081, 0x0000,
            0x0082, 0x0000,
            0x0083, 0x0000,
            0x0084, 0x0000,
            0x0085, 0x0000,
            //-----Panel Control----------------------
            0x0090, 0x0010,
            0x0092, 0x0000,
            0x0093, 0x0003,
            0x0095, 0x0110,
            0x0097, 0x0000,
            0x0098, 0x0000,
            //-----Display on-----------------------
            0x0007, 0x0173,
            TFTLCD_DELAY, 50,
        };
        init_table16(regValues, sizeof(regValues));
        break;
    }
    _lcd_rev = ((_lcd_capable & REV_SCREEN) != 0);
    setRotation(0);             //PORTRAIT
    invertDisplay(false);
}
