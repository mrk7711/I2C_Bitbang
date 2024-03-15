#include "dsp.h"

void printbanner(void) 
{
	uart_tx("\r\n ---------------");
	uart_tx("\r\n I2C DA1453X_DSP");
  uart_tx("\r\n ---------------\r\n");
}

 void delay_us_t(int i)
 {
	for(int k=0; k<i;k++)
	{
		for(int j=0; j<3; j++)
		__NOP();
	}
}
 

void I2C_init(void)
{
    I2C_SET_SDA
    I2C_SET_SCL
}


void I2C_start_cond(void)
{
    I2C_SET_SCL
    I2C_SET_SDA
    I2C_DELAY
    I2C_CLEAR_SDA
    I2C_DELAY
    I2C_CLEAR_SCL
    I2C_DELAY
}


void I2C_stop_cond(void)
{
    I2C_CLEAR_SDA
    I2C_DELAY
    I2C_SET_SCL
    I2C_DELAY
    I2C_SET_SDA
    I2C_DELAY
}


void I2C_write_bit(uint8_t b)
{
    if (b > 0)
        I2C_SET_SDA
    else
        I2C_CLEAR_SDA

    I2C_DELAY
    I2C_SET_SCL
    I2C_DELAY
    I2C_CLEAR_SCL
}

uint8_t I2C_read_SDA(void)
{
    if (GPIO_GetPinStatus(I2C_SDA_PORT, I2C_SDA_PIN) == true)
        return 1;
    else
        return 0;
    return 0;
}

uint8_t I2C_read_bit(void)
{
    uint8_t b;

    I2C_SET_SDA
		GPIO_ConfigurePin(I2C_SDA_PORT, I2C_SDA_PIN, INPUT, PID_GPIO, false);
    I2C_DELAY
    I2C_SET_SCL
    I2C_DELAY

    b = I2C_read_SDA();

    I2C_CLEAR_SCL
		GPIO_ConfigurePin(I2C_SDA_PORT, I2C_SDA_PIN, OUTPUT, PID_GPIO, false);
    return b;
}

_Bool I2C_write_byte(uint8_t B,_Bool start, _Bool stop)
{
    uint8_t ack = 0;

    if (start)
        I2C_start_cond();

    uint8_t i;
    for (i = 0; i < 8; i++)				//write 8 bits 
    {
        I2C_write_bit(B & 0x80); // write the most-significant bit
        B <<= 1;
    }

    ack = I2C_read_bit();

    if (stop)
		{
				//uart_tx("STOP");
        I2C_stop_cond();
		}

    return !ack; //0-ack, 1-nack
}

uint8_t I2C_read_byte(_Bool ack, _Bool stop)
{
    uint8_t B = 0;

    uint8_t i;
    for (i = 0; i < 8; i++)
    {
        B <<= 1;
        B |= I2C_read_bit();
    }

    if (ack)
        I2C_write_bit(0);
    else
        I2C_write_bit(1);

    if (stop)
        I2C_stop_cond();

    return B;
}

void uart_tx(char* str)
{
	uart_send(UART2,(uint8_t *)str, strlen(str), UART_OP_BLOCKING);
}

void printf_byte(uart_t *uart, uint8_t ch)
{
    // print a Byte in hex format
    char b;
    b = ((0xF0 & ch) >> 4);
    b += (b < 10) ? 48 : 55;

    uart_send(uart, (uint8_t *)&b, 1, UART_OP_BLOCKING);
    b = (0xF & ch);
    b += (b < 10) ? 48 : 55;
    uart_send(uart, (uint8_t *)&b, 1, UART_OP_BLOCKING);
}

void print_word(uart_t *uart, uint32_t aWord)
{
    printf_byte(uart, (aWord >> 24) & 0xFF);
    printf_byte(uart, (aWord >> 16) & 0xFF);
    printf_byte(uart, (aWord >> 8) & 0xFF);
    printf_byte(uart, (aWord) & 0xFF);
}

_Bool I2C_send_byte(uint8_t address,uint8_t data)
{
    //    if( I2C_write_byte( address << 1, true, false ) )   // start, send address, write
    if (I2C_write_byte(address, true, false)) // start, send address, write
    {
        // send data, stop
        if (I2C_write_byte(data, false, true))
            return true;
    }

    I2C_stop_cond(); // make sure to impose a stop if NAK'd
    return false;
}

uint8_t I2C_receive_byte(uint8_t address)
{
    if (I2C_write_byte((address << 1) | 0x01, true, false)) // start, send address, read
    {
        return I2C_read_byte(false, true);
    }

    return 0; // return zero if NAK'd
}

_Bool I2C_send_byte_data(uint8_t address,uint8_t reg,uint8_t data)
{
    //    if( I2C_write_byte( address << 1, true, false ) )   // start, send address, write
    if (I2C_write_byte(address, true, false))
    {
        if (I2C_write_byte(reg, false, false)) // send desired register
        {
            if (I2C_write_byte(data, false, true))
                return true; // send data, stop
        }
    }

    I2C_stop_cond();
    return false;
}

uint8_t I2C_receive_byte_data(uint8_t address,uint8_t reg)
{
    //if( I2C_write_byte( address << 1, true, false ) )   // start, send address, write
    if (I2C_write_byte(address, true, false))
    {
        if (I2C_write_byte(reg, false, false)) // send desired register
        {
            if (I2C_write_byte((address << 1) | 0x01, true, false)) // start again, send address, read
            {
                return I2C_read_byte(false, true); // read data
            }
        }
    }

    I2C_stop_cond();
    return 0; // return zero if NACKed
}

_Bool I2C_transmit(uint8_t address, uint8_t data[], uint8_t size)
{
	
    if (I2C_write_byte(address, true, false)) // first byte
    {
        for (int i = 0; i < size; i++)
        {
            if (i == size - 1)
            {
                if (I2C_write_byte(data[i], false, true))
                    return true;
            }
            else
            {
                if (!I2C_write_byte(data[i], false, false))
                    break; //last byte
            }
        }				
    }

    I2C_stop_cond();
    return false;
}

_Bool I2C_receive(uint8_t address, uint8_t *reg, uint8_t *data, uint8_t reg_size, uint8_t size)
{
   if(I2C_write_byte(address, true, false))
    {
        for (int i = 0; i < reg_size; i++)
        {
            if (!I2C_write_byte(reg[i], false, false))
                break;
						//uart_tx("FOR");
        }
//        if (I2C_write_byte(address | 0x01, true, false)) // start again, send address, read (LSB signifies R or W)
//        {
            for (int j = 0; j < size; j++)
            {
                *data++ = I2C_read_byte(false, false); // read data
            }
            I2C_stop_cond();
            return true;
//        }
    }
    I2C_stop_cond();
    return false;
}

void test1 (void)
{
	while(1)
	{
			I2C_SET_SDA
			I2C_SET_SCL
			I2C_DELAY
			I2C_CLEAR_SCL
			I2C_CLEAR_SDA
			I2C_DELAY
	}
}

