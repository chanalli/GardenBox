extern void i2c_init(unsigned char);
extern uint8_t i2c_io(uint8_t, uint8_t *, uint16_t,
		      uint8_t *, uint16_t, uint8_t *, uint16_t);
void uv_init(uint8_t *, uint8_t *);
void check_uv(uint8_t *, uint8_t *);