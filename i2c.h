extern void i2c_init(unsigned char);
extern uint8_t i2c_io(uint8_t, uint8_t *, uint16_t,
		      uint8_t *, uint16_t, uint8_t *, uint16_t);
void uv_init(uint8_t *, uint8_t *);
void get_data(uint8_t *, uint8_t *);
void get_display_data(uint8_t *, uint8_t *);
float get_index(uint8_t *, uint8_t *, uint8_t *, uint8_t *);
void display_index(float);
void print_float(float);
