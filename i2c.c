/*********************************************************************
*       i2c - I/O routines for the ATmega168 TWI interface,
*       which is the same functionally as I2C.
*
*       Note: These routines were written to help students in EE459Lx
*       at USC.  They may contain errors, and definitely are not
*       ready for use in a real product.  Use at your own risk.
*
* Revision History
* Date     Author      Description
* 04/14/11 A. Weber    First release
* 02/07/12 A. Weber    Added i2c_write and i2c_read1 routines
* 02/07/12 A. Weber    Added i2c_write1 routine
* 02/17/12 A. Weber    Changes to comments and types
* 04/19/12 A. Weber    Combined write and read routines into one function
* 05/08/12 A. Weber    Added code to handle NAKs better
* 04/09/15 A. Weber    More comments
*********************************************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include "lcd.h"
#include "i2c.h"

#define NUM_INTEGRATION_TIMES 5

uint8_t i2c_io(uint8_t, uint8_t *, uint16_t, uint8_t *, uint16_t, uint8_t *, uint16_t);
void i2c_init(unsigned char);
//used to stringout rotary encoder values
char str[50];
const float UV_ALPHA = 1.0;
const float UV_BETA = 1.0;
const float UV_GAMMA = 1.0;
const float UV_DELTA = 1.0;

const float UVA_A_COEF = 2.22;
const float UVA_B_COEF = 1.33;
const float UVA_C_COEF = 2.95;
const float UVA_D_COEF = 1.75;


#define UVA_RESPONSIVITY_100MS_UNCOVERED 0.001111
#define UVB_RESPONSIVITY_100MS_UNCOVERED 0.00125

const float UVA_RESPONSIVITY[NUM_INTEGRATION_TIMES] = 
{
    UVA_RESPONSIVITY_100MS_UNCOVERED / 0.5016286645, // 50ms
    UVA_RESPONSIVITY_100MS_UNCOVERED,                // 100ms
    UVA_RESPONSIVITY_100MS_UNCOVERED / 2.039087948,  // 200ms
    UVA_RESPONSIVITY_100MS_UNCOVERED / 3.781758958,  // 400ms
    UVA_RESPONSIVITY_100MS_UNCOVERED / 7.371335505   // 800ms
};

const float UVB_RESPONSIVITY[NUM_INTEGRATION_TIMES] = 
{
    UVB_RESPONSIVITY_100MS_UNCOVERED / 0.5016286645, // 50ms
    UVB_RESPONSIVITY_100MS_UNCOVERED,                // 100ms
    UVB_RESPONSIVITY_100MS_UNCOVERED / 2.039087948,  // 200ms
    UVB_RESPONSIVITY_100MS_UNCOVERED / 3.781758958,  // 400ms
    UVB_RESPONSIVITY_100MS_UNCOVERED / 7.371335505   // 800ms
};


#define FOSC 7372800            // Clock frequency = Oscillator freq.
#define BAUD 9600               // UART0 baud rate
#define MYUBRR FOSC/16/BAUD-1   // Value for UBRR0 register
#define BDIV (FOSC / 100000 - 16) / 2 + 1    // Puts I2C rate just below 100kHz

/* Address of the UVSENSOR on the I2C bus */
#define UVSENSOR_ADDR  0x20

void uv_init(uint8_t *commandCode, uint8_t *config){
	i2c_io(0x20, commandCode, 1, config, 2, NULL, 0);
	i2c_init(BDIV); 
}

void get_display_data(uint8_t *commandCode, uint8_t *rdata){
	i2c_io(0x20, commandCode, 1, NULL, 0, rdata, 2);
	lcd_moveto(3,15);
	snprintf(str,4, "%3d", rdata[0]);
	lcd_stringout(str);
	snprintf(str,4, "%3d", rdata[1]);
	lcd_stringout(str);
}
void display_index(float index){
	lcd_moveto(3,15);
	int wholePart = index;
	int fractPart = (index - wholePart) * 10;
	sprintf(str, "%d.%d", wholePart, fractPart);
	lcd_stringout(str);
}
void print_float(float num){
	lcd_moveto(3,10);
	int wholePart = num;
	int fractPart = (num - wholePart) * 100000;
	sprintf(str, "%d.00%d", wholePart, fractPart);
	lcd_stringout(str);
}
float get_index(uint8_t * uva, uint8_t * uvb, uint8_t * uvcomp1, uint8_t * uvcomp2){
	
	float aResponsivity = UVA_RESPONSIVITY[2];
    float bResponsivity = UVB_RESPONSIVITY[2];
	
	uint16_t rawUva=(uva[0] & 0x00FF) | ((uva[1] & 0x00FF) << 8);
	uint16_t rawUvb=(uvb[0] & 0x00FF) | ((uvb[1] & 0x00FF) << 8);
	
	uint16_t uvComp1=(uvcomp1[0] & 0x00FF) | ((uvcomp1[1] &0x00FF) << 8);
	uint16_t uvComp2=(uvcomp2[0] & 0x00FF) | ((uvcomp2[1] & 0x00FF) << 8);
	
	float uvaCalc = (float)rawUva - ((UVA_A_COEF * UV_ALPHA * (float)uvComp1) / UV_GAMMA) - ((UVA_B_COEF * UV_ALPHA * (float)uvComp2) / UV_DELTA);
	float uvbCalc = (float)rawUvb - ((UVA_C_COEF * UV_BETA * (float)uvComp1) / UV_GAMMA)- ((UVA_D_COEF * UV_BETA * (float)uvComp2) / UV_DELTA);
	
    float uvia = uvaCalc * (1.0 / UV_ALPHA) * aResponsivity;
    float uvib = uvbCalc * (1.0 / UV_BETA) * bResponsivity;
	print_float(uvia);
    float index = (uvia + uvib) / 2.0;
	
	return index;
}


/*
  i2c_io - write and read bytes to a slave I2C device

  Usage:      status = i2c_io(device_addr, ap, an, wp, wn, rp, rn);
  Arguments:  device_addr - This is the EIGHT-BIT I2C device bus address.
              Some datasheets specify a seven bit address.  This argument
              is the seven-bit address shifted left one place with a zero
              in the LSB.  This is also sometimes referred to as the
              address for writing to the device.
              ap, wp, rp - Pointers to three buffers containing data to be
              written (ap and wp), or to receive data that is read (rp).
              an, wn, rn - Number of bytes to write or read to/from the
              corresponding buffers.

  This funtions write "an" bytes from array "ap" and then "wn" bytes from array
  "wp" to I2C device at bus address "device_addr".  It then reads "rn" bytes
  from the same device to array "rp".

  Return values (might not be a complete list):
        0    - Success
        0x20 - NAK received after sending device address for writing
        0x30 - NAK received after sending data
        0x38 - Arbitration lost with address or data
        0x48 - NAK received after sending device address for reading

  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

This "i2c_io" I2C routine is an attempt to provide an I/O function for both
reading and writing, rather than have separate functions.

I2C writes consist of sending a stream of bytes to the slave device.  In some
cases the first few bytes may be the internal address in the device, and then
the data to be stored follows.  For example, EEPROMs like the 24LC256 require a
two-byte address to precede the data.  The DS1307 RTC requires a one-byte
address.

I2C reads often consist of first writing one or two bytes of internal address
data to the device and then reading back a stream of bytes starting from that
address.  Some devices appear to claim that that reads can be done without
first doing the address writes, but so far I haven't been able to get any to
work that way.

This function does writing and reading by using pointers to three arrays "ap",
"wp", and "rp".  The function performs the following actions in this order:
    If "an" is greater than zero, then "an" bytes are written from array "ap"
    If "wn" is greater then zero, then "wn" bytes are written from array "wp"
    If "rn" is greater then zero, then "rn" byte are read into array "rp"
Any of the "an", "wn", or "rn" can be zero.

The reason for separate "ap" and "wp" arrays is that the address data can be
taken from one array (ap), and then the write data from another (wp) without
requiring that the contents be merged into one array before calling the
function.  This means the following three calls all do exactly the same thing.

    i2c_io(0xA0, buf, 100, NULL, 0, NULL, 0);
    i2c_io(0xA0, NULL, 0, buf, 100, NULL, 0);
    12c_io(0xA0, buf, 2, buf+2, 98, NULL, 0);

In all cases 100 bytes from array "buf" will be written to the I2C device at
bus address 0xA0.

A typical write with a 2-byte address is done with

    i2c_io(0xA0, abuf, 2, wbuf, 50, NULL, 0);

A typical read with a 1-byte address is done with

    i2c_io(0xD0, abuf, 1, NULL, 0, rbuf, 20);
*/

uint8_t i2c_io(uint8_t device_addr, uint8_t *ap, uint16_t an, 
               uint8_t *wp, uint16_t wn, uint8_t *rp, uint16_t rn)
{
    uint8_t status, send_stop, wrote, start_stat;

    status = 0;
    wrote = 0;
    send_stop = 0;

    if (an > 0 || wn > 0) {
        wrote = 1;
        send_stop = 1;

        TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTA);  // Send start condition
        while (!(TWCR & (1 << TWINT)));     // Wait for TWINT to be set
        status = TWSR & 0xf8;
        if (status != 0x08)                 // Check that START was sent OK
            return(status);

        TWDR = device_addr & 0xfe;          // Load device address and R/W = 0;
        TWCR = (1 << TWINT) | (1 << TWEN);  // Start transmission
        while (!(TWCR & (1 << TWINT)));     // Wait for TWINT to be set
        status = TWSR & 0xf8;
        if (status != 0x18) {               // Check that SLA+W was sent OK
            if (status == 0x20)             // Check for NAK
                goto nakstop;               // Send STOP condition
            return(status);                 // Otherwise just return the status
        }

        // Write "an" data bytes to the slave device
        while (an-- > 0) {
            TWDR = *ap++;                   // Put next data byte in TWDR
            TWCR = (1 << TWINT) | (1 << TWEN); // Start transmission
            while (!(TWCR & (1 << TWINT))); // Wait for TWINT to be set
            status = TWSR & 0xf8;
            if (status != 0x28) {           // Check that data was sent OK
                if (status == 0x30)         // Check for NAK
                    goto nakstop;           // Send STOP condition
                return(status);             // Otherwise just return the status
            }
        }

        // Write "wn" data bytes to the slave device
        while (wn-- > 0) {
            TWDR = *wp++;                   // Put next data byte in TWDR
            TWCR = (1 << TWINT) | (1 << TWEN); // Start transmission
            while (!(TWCR & (1 << TWINT))); // Wait for TWINT to be set
            status = TWSR & 0xf8;
            if (status != 0x28) {           // Check that data was sent OK
                if (status == 0x30)         // Check for NAK
                    goto nakstop;           // Send STOP condition
                return(status);             // Otherwise just return the status
            }
        }

        status = 0;                         // Set status value to successful
    }

    if (rn > 0) {
        send_stop = 1;

        // Set the status value to check for depending on whether this is a
        // START or repeated START
        start_stat = (wrote) ? 0x10 : 0x08;

        // Put TWI into Master Receive mode by sending a START, which could
        // be a repeated START condition if we just finished writing.
        TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTA);
                                            // Send start (or repeated start) condition
        while (!(TWCR & (1 << TWINT)));     // Wait for TWINT to be set
        status = TWSR & 0xf8;
        if (status != start_stat)           // Check that START or repeated START sent OK
            return(status);

        TWDR = device_addr  | 0x01;         // Load device address and R/W = 1;
        TWCR = (1 << TWINT) | (1 << TWEN);  // Send address+r
        while (!(TWCR & (1 << TWINT)));     // Wait for TWINT to be set
        status = TWSR & 0xf8;
        if (status != 0x40) {               // Check that SLA+R was sent OK
            if (status == 0x48)             // Check for NAK
                goto nakstop;
            return(status);
        }

        // Read all but the last of n bytes from the slave device in this loop
        rn--;
        while (rn-- > 0) {
            TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA); // Read byte and send ACK
            while (!(TWCR & (1 << TWINT))); // Wait for TWINT to be set
            status = TWSR & 0xf8;
            if (status != 0x50)             // Check that data received OK
                return(status);
            *rp++ = TWDR;                   // Read the data
        }

        // Read the last byte
        TWCR = (1 << TWINT) | (1 << TWEN);  // Read last byte with NOT ACK sent
        while (!(TWCR & (1 << TWINT)));     // Wait for TWINT to be set
        status = TWSR & 0xf8;
        if (status != 0x58)                 // Check that data received OK
            return(status);
        *rp++ = TWDR;                       // Read the data

        status = 0;                         // Set status value to successful
    }
    
nakstop:                                    // Come here to send STOP after a NAK
    if (send_stop)
        TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);  // Send STOP condition

    return(status);
}


/*
  i2c_init - Initialize the I2C port
*/
void i2c_init(uint8_t bdiv)
{
    TWSR = 0;                           // Set prescalar for 1
    TWBR = bdiv;                        // Set bit rate register
}
