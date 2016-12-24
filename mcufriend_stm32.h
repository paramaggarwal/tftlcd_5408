#define RD_PORT GPIOB
#define RD_PIN  9
#define WR_PORT GPIOB
#define WR_PIN  8
#define CD_PORT GPIOB
#define CD_PIN  7
#define CS_PORT GPIOB
#define CS_PIN  6
#define RESET_PORT GPIOB
#define RESET_PIN  5

#define write_8(x)		(GPIOA->regs->ODR = (x & 0xFF))
#define read_8()			(GPIOA->regs->IDR & 0xFF)
#define setWriteDir() { \
	gpio_set_mode(GPIOA, 0, GPIO_OUTPUT_PP); \
	gpio_set_mode(GPIOA, 1, GPIO_OUTPUT_PP); \
	gpio_set_mode(GPIOA, 2, GPIO_OUTPUT_PP); \
	gpio_set_mode(GPIOA, 3, GPIO_OUTPUT_PP); \
	gpio_set_mode(GPIOA, 4, GPIO_OUTPUT_PP); \
	gpio_set_mode(GPIOA, 5, GPIO_OUTPUT_PP); \
	gpio_set_mode(GPIOA, 6, GPIO_OUTPUT_PP); \
	gpio_set_mode(GPIOA, 7, GPIO_OUTPUT_PP); \
}
#define setReadDir()  { \
	gpio_set_mode(GPIOA, 0, GPIO_INPUT_FLOATING); \
	gpio_set_mode(GPIOA, 1, GPIO_INPUT_FLOATING); \
	gpio_set_mode(GPIOA, 2, GPIO_INPUT_FLOATING); \
	gpio_set_mode(GPIOA, 3, GPIO_INPUT_FLOATING); \
	gpio_set_mode(GPIOA, 4, GPIO_INPUT_FLOATING); \
	gpio_set_mode(GPIOA, 5, GPIO_INPUT_FLOATING); \
	gpio_set_mode(GPIOA, 6, GPIO_INPUT_FLOATING); \
	gpio_set_mode(GPIOA, 7, GPIO_INPUT_FLOATING); \
}
#define write8(x)     { write_8(x); WR_STROBE; }
#define write16(x)    { uint8_t h = (x)>>8, l = x; write8(h); write8(l); }
#define READ_8(dst)   { RD_STROBE; dst = read_8(); RD_IDLE; }
#define READ_16(dst)  { uint8_t hi; READ_8(hi); READ_8(dst); dst |= (hi << 8); }

#define PIN_LOW(p, b)        gpio_write_bit(p, b, LOW)
#define PIN_HIGH(p, b)       gpio_write_bit(p, b, HIGH)
#define PIN_OUTPUT(p, b)     gpio_set_mode(p, b, GPIO_OUTPUT_PP);

#define RD_ACTIVE  PIN_LOW(RD_PORT, RD_PIN)
#define RD_IDLE    PIN_HIGH(RD_PORT, RD_PIN)
#define RD_OUTPUT  PIN_OUTPUT(RD_PORT, RD_PIN)
#define WR_ACTIVE  PIN_LOW(WR_PORT, WR_PIN)
#define WR_IDLE    PIN_HIGH(WR_PORT, WR_PIN)
#define WR_OUTPUT  PIN_OUTPUT(WR_PORT, WR_PIN)
#define CD_COMMAND PIN_LOW(CD_PORT, CD_PIN)
#define CD_DATA    PIN_HIGH(CD_PORT, CD_PIN)
#define CD_OUTPUT  PIN_OUTPUT(CD_PORT, CD_PIN)
#define CS_ACTIVE  PIN_LOW(CS_PORT, CS_PIN)
#define CS_IDLE    PIN_HIGH(CS_PORT, CS_PIN)
#define CS_OUTPUT  PIN_OUTPUT(CS_PORT, CS_PIN)
#define RESET_ACTIVE  PIN_LOW(RESET_PORT, RESET_PIN)
#define RESET_IDLE    PIN_HIGH(RESET_PORT, RESET_PIN)
#define RESET_OUTPUT  PIN_OUTPUT(RESET_PORT, RESET_PIN)

 // General macros.   IOCLR registers are 1 cycle when optimised.
#define WR_STROBE { WR_ACTIVE; WR_IDLE; }       //PWLW=TWRL=50ns
#define RD_STROBE RD_IDLE, RD_ACTIVE, RD_ACTIVE, RD_ACTIVE, RD_ACTIVE, RD_ACTIVE, RD_ACTIVE      //PWLR=TRDL=150ns, tDDR=100ns

#define CTL_INIT()   { RD_OUTPUT; WR_OUTPUT; CD_OUTPUT; CS_OUTPUT; RESET_OUTPUT; }
#define WriteCmd(x)  { CD_COMMAND; write16(x); }
#define WriteData(x) { CD_DATA; write16(x); }
