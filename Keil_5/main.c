/*
 * INCLUDE FILES
 ****************************************************************************************
 */
 
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "arch_system.h"
#include "uart.h"
#include "datasheet.h"
#include "system_library.h"
#include "rwip_config.h"
#include "syscntl.h"
#include "uart_utils.h"
#include "user_periph_setup.h"
#include "gpio.h"
#include "i2c.h"
#include "arch_console.h"
#include "dsp.h"


void i2c_bitbangmode(void);
void i2c_periphmode(void);
uint32_t data;
uint8_t buffer[10];
uint8_t reg0[2]={CHIP_ID_REG>>8,CHIP_ID_REG&0xff};

int main (void)
{

    system_init();  
		periph_init();
		i2c_bitbangmode();
}


void i2c_bitbangmode(void)
{
		printbanner();
		I2C_init();
		I2C_stop_cond();
		delay_us_t(500);
		I2C_receive(0x67, &reg0[0] , &buffer[0] , 2, 4 );
		data =(((uint32_t)(buffer[0])) << 24) |(((uint32_t)(buffer[1])) << 16)  |(((uint32_t)(buffer[2]))<< 8)  | (((uint32_t)(buffer[3])));
		print_word(UART2,data);
		while(1)
		printbanner();
}

void i2c_periphmode(void)
{
	
}
	



