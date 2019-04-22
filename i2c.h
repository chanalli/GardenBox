extern void i2c_init(unsigned char);
extern uint8_t i2c_io(uint8_t, uint8_t *, uint16_t,
		      uint8_t *, uint16_t, uint8_t *, uint16_t);
void uv_init(uint8_t *, uint8_t *);
void get_data(uint8_t *, uint8_t *);
void get_display_data(uint8_t *, uint8_t *);
float get_index(uint8_t *, uint8_t *, uint8_t *, uint8_t *);
void display_index(float);
void print_float(float);
void display_UV_level(float);

//Temperature Sensor Functions
void config_temp_reg(uint8_t *, uint8_t *);
void start_conv(uint8_t *);
void init_temp(uint8_t *, uint8_t *, uint8_t *);
float get_temp_data(uint8_t *, uint8_t *);