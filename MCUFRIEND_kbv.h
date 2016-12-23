#define MCUFRIEND_KBV_H_   280
#include "Adafruit_GFX.h"

class MCUFRIEND_kbv : public Adafruit_GFX {

	public:
	MCUFRIEND_kbv();
	void     reset(void);
	void     begin(uint16_t ID);
	virtual void     drawPixel(int16_t x, int16_t y, uint16_t color);  // and these three
	void     WriteCmdData(uint16_t cmd, uint16_t dat);                 // ?public methods !!!
	uint16_t color565(uint8_t r, uint8_t g, uint8_t b) { return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3); }
	uint16_t readID(void);
	virtual void     fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
	virtual void     drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) { fillRect(x, y, 1, h, color); }
	virtual void     drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) { fillRect(x, y, w, 1, color); }
	virtual void     fillScreen(uint16_t color)                                     { fillRect(0, 0, _width, _height, color); }
	virtual void     setRotation(uint8_t r);
  virtual void     invertDisplay(boolean i);

	uint16_t readReg(uint16_t reg);
	uint32_t readReg32(uint16_t reg);
	int16_t  readGRAM(int16_t x, int16_t y, uint16_t *block, int16_t w, int16_t h);
	uint16_t readPixel(int16_t x, int16_t y) { uint16_t color; readGRAM(x, y, &color, 1, 1); return color; }
	void     setAddrWindow(int16_t x, int16_t y, int16_t x1, int16_t y1);
	void     pushColors(uint16_t *block, int16_t n, bool first);
	void     pushColors(uint8_t *block, int16_t n, bool first);
	void     pushColors(const uint8_t *block, int16_t n, bool first);
	void     vertScroll(int16_t top, int16_t scrollines, int16_t offset);
	
	private:
	uint16_t _lcd_ID, _lcd_rev, _lcd_madctl, _lcd_drivOut, _MC, _MP, _MW, _SC, _EC, _SP, _EP;
  uint16_t  _lcd_xor, _lcd_capable;
};
