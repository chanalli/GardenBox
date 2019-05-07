/* VEML6075 slave address */
#define VEML6075_ADDR 0x20 // 7-bit: 0x10
/* Registers define */
#define VEML6075_CONF_REG 0x00
#define VEML6075_UVA_DATA_REG 0x07
#define VEML6075_UVB_DATA_REG 0x09
#define VEML6075_UVCOMP1_DATA_REG 0x0A
#define VEML6075_UVCOMP2_DATA_REG 0x0B
#define VEML6075_ID_REG 0x0C
/* Register value define : CONF */
#define VEML6075_CONF_SD 0x01
#define VEML6075_CONF_UV_AF_AUTO 0x00
#define VEML6075_CONF_UV_AF_FORCE 0x02
#define VEML6075_CONF_UV_TRIG_NO 0x00
#define VEML6075_CONF_UV_TRIG_ONCE 0x04
#define VEML6075_CONF_HD 0x08
#define VEML6075_CONF_UV_IT_MASK 0x70
#define VEML6075_CONF_UV_IT_50MS 0x00
#define VEML6075_CONF_UV_IT_100MS 0x10
#define VEML6075_CONF_UV_IT_200MS 0x20
#define VEML6075_CONF_UV_IT_400MS 0x30
#define VEML6075_CONF_UV_IT_800MS 0x40
#define VEML6075_CONF_DEFAULT (VEML6075_CONF_UV_AF_AUTO | ; VEML6075_CONF_UV_TRIG_NO | ; VEML6075_CONF_UV_IT_100MS)

/* I2C message, used for I2C transaction */
struct i2c_msg {
 WORD addr;
 WORD flags;
#define I2C_M_TEN 0x0010
#define I2C_M_RD 0x0001
#define I2C_M_NOSTART 0x4000
#define I2C_M_REV_DIR_ADDR 0x2000
#define I2C_M_IGNORE_NAK 0x1000
#define I2C_M_NO_RD_ACK 0x0800
#define I2C_M_RECV_LEN 0x0400
 WORD len;
 BYTE *buf;
};

extern int i2c_transfer(struct i2c_msg *msgs, int num);
/*------------------------------------------------------------------------
 C main function
--------------------------------------------------------------------------
*/
void main(void)
{
 WORD VEML6075_conf;
 WORD uva_data;
 WORD uvb_data;
 WORD uvcomp1_data; 
 
 /* Shut down VEML6075 */
 VEML6075_conf = VEML6075_CONF_DEFAULT | VEML6075_CONF_SD;
 VEML6075_write_word(VEML6075_ADDR, VEML6075_CONF_REG, VEML6075_conf);
 /* Enable VEML6075 */
 VEML6075_conf = VEML6075_CONF_DEFAULT;
 VEML6075_write_word(VEML6075_ADDR, VEML6075_CONF_REG, VEML6075_conf);
 /* Loop for polling VEML6075 data */
 while (1)
 {
 delay(150);
 VEML6075_read_word(VEML6075_ADDR, VEML6075_UVA_DATA_REG, &uva_data
 VEML6075_read_word(VEML6075_ADDR, VEML6075_UVB_DATA_REG, &uvb_data
 VEML6075_read_word(VEML6075_ADDR, VEML6075_UVCOMP1_DATA_REG,
&uvcomp1_data);
 VEML6075_read_word(VEML6075_ADDR, VEML6075_UVCOMP2_DATA_REG,
&uvcomp2_data);
 }
}
int VEML6075_read_word(WORD addr, BYTE reg, WORD *val)
{
 int err = 0;
 int retry = 3;
 struct i2c_msg msg[2];
 BYTE data[2];
 while (retry--)
 {
 /* Send slave address & register */
 msg[0].addr = addr >> 1;
 msg[0].flags = I2C_M_WR;
 msg[0].len = 1;
 msg[0].buf = &reg;
 /* Read word data */
 msg[1].addr = addr >> 1;
 msg[1].flags = I2C_M_RD;
 msg[1].len = 2;
 msg[1].buf = data;
 err = i2c_transfer(msg, 2);
 if (err >= 0)
 {
 *val = ((WORD)data[1] << 8) | (WORD)data[0];
 return err;
 }
 }
 return err;
} 
int VEML6075_write_word(WORD addr, BYTE reg, WORD val)
{
 int err = 0;
 int retry = 3;
 struct i2c_msg msg;
 BYTE data[3];
 while (retry--)
 {
 data[0] = reg;
 data[1] = (BYTE)(val & 0xFF);
 data[2] = (BYTE)((val & 0xFF00) >> 8);
 /* Send slave address, register and word data */
 msg.addr = addr >> 1;
 msg.flags = I2C_M_WR;
 msg.len = 3;
 msg.buf = data;
 err = i2c_transfer(msg, 1);
 if (err >= 0)
 return 0;
 }
 return err;
} 