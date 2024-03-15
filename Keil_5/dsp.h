/*
 * INCLUDE FILES
 ****************************************************************************************
 */
 
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "arch_system.h"
#include "uart.h"
#include "uart_utils.h"
#include "user_periph_setup.h"
#include "gpio.h"
#include "i2c.h"
#include "arch_console.h"

/*
 * DEFINES
 ****************************************************************************************
 */
 
#define START_DATA 1
	
#define I2C_SET_SDA GPIO_SetActive(I2C_SDA_PORT, I2C_SDA_PIN);

#define I2C_CLEAR_SDA GPIO_SetInactive(I2C_SDA_PORT, I2C_SDA_PIN);

#define I2C_SET_SCL GPIO_SetActive(I2C_SCL_PORT, I2C_SCL_PIN);

#define I2C_CLEAR_SCL GPIO_SetInactive(I2C_SCL_PORT, I2C_SCL_PIN);

#define I2C_DELAY delay_us_t(10); // 10 microsecond delay

#define DSP_I2C_SLAVE_ADDR		(0x66)						//Device(Slave) Address

/*
 * DEFINE DSP REGISTERS
 ****************************************************************************************
 */		

#define CHIP_ID_REG      				(0x0000)
#define TRIM_REG0      					(0x0001)
#define POWER_ON_CTRL_REG     	(0x0017)

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */
void printbanner(void);

void uart_tx(char *str);

void print_word(uart_t *uart, uint32_t aWord);

void printf_byte(uart_t *uart, uint8_t ch);

void delay_us_t(int i);

void I2C_init(void);

void I2C_start_cond(void);

void I2C_stop_cond(void);

void I2C_write_bit(uint8_t b);

uint8_t I2C_read_SDA(void);

uint8_t I2C_read_bit(void);

_Bool I2C_write_byte(uint8_t B,_Bool start, _Bool stop);

uint8_t I2C_read_byte(_Bool ack, _Bool stop);

_Bool I2C_send_byte(uint8_t address,uint8_t data);

uint8_t I2C_receive_byte(uint8_t address);

_Bool I2C_send_byte_data(uint8_t address,uint8_t reg,uint8_t data);

uint8_t I2C_receive_byte_data(uint8_t address,uint8_t reg);

_Bool I2C_transmit(uint8_t address, uint8_t data[], uint8_t size);

_Bool I2C_receive(uint8_t address, uint8_t *reg, uint8_t *data, uint8_t reg_size, uint8_t size);

uint8_t parsing_data(uint8_t *data, uint32_t len);

void test1 (void);