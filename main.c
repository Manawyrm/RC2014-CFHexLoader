#include "main.h"
#include <stdio.h>
#include "ff.h"
#include "romfunctions.h"
#include <z80.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

void print_memory(const void *addr, uint16_t size)
{
    uint16_t printed = 0;
    uint16_t i;
    const unsigned char* pc = addr;
    for (i=0; i<size; ++i)
    {
        int  g;
        g = (*(pc+i) >> 4) & 0xf;
        g += g >= 10 ? 'a'-10 : '0';
        rom_putchar_uart(g);
        printed++;

        g = *(pc+i) & 0xf;
        g += g >= 10 ? 'a'-10 : '0';
        rom_putchar_uart(g);
        printed++;
        if (printed % 32 == 0) rom_putchar_uart('\n');
        else if (printed % 4 == 0) rom_putchar_uart(' ');
    }
}

uint8_t linebuf[64];
uint8_t readline()
{
	uint8_t input = 0xFF; 
	uint8_t i = 0;
	// loop until we get a return or the buffer is full
	while (input != '\r' && i != 64) 
	{
		// wait for character
		while (!rom_uart_available()) {}

		input = rom_uart_read();
		
		// backspace
		if (input == 0x7F)
		{
			rom_putchar_uart(0x08);
			rom_putchar_uart(0x20);
			rom_putchar_uart(0x08);
			i--;
		}
		else
		{
			// echo
			rom_putchar_uart(input);

			// don't put newlines into the buffer
			if (!(input == '\r' || input == '\n'))
				linebuf[i++] = input;
		}

		
	}
	linebuf[i] = 0x00;

	//print_memory(linebuf, 64);
	return i;
}



uint16_t parse_hex_nibbles(uint8_t* buffer, uint16_t start)
{
	uint8_t result = 0; 

	for (uint8_t i = 0; i < 2; i++)
	{
		unsigned char nibble = buffer[start + i];

		result <<= 4;

		if(nibble >= 'A')
		{
			result += nibble - 'A' + 10;
		} 
		else
		{
			result += nibble - '0';
		}
	}

	return result; 
}

void die (FRESULT rc, const char* reason)
{
	//printf("Failed with rc=%u.\n", rc);
	rom_putstring_uart("fat error at ");
	rom_putstring_uart(reason);
	rom_putstring_uart(" - error code: 0x");
	print_memory(&rc, sizeof(rc));
	rom_putstring_uart("\n");

	for (;;) ;
}

void main()
{
	FATFS FatFs;		/* FatFs work area needed for each volume */
	FIL Fil;			/* File object needed for each open file */
	UINT bw;
	FRESULT fr;

	uint8_t done = 0; 
	uint8_t linebuffer[260];
	uint8_t databuffer[128];
	uint16_t linebufferPos = 0;
	uint8_t byte = 0;
	uint8_t payloadLength = 0;
	uint8_t payloadType = 0;
	uint16_t address = 0;
	uint16_t bytesRead = 0;

	/*__asm
	di
	__endasm;
	*/

	rom_putstring_uart("hello from the cf hexload adapter!\n");

	memset(linebuffer, 0x00, sizeof(linebuffer) - 1);

	fr = f_mount(&FatFs, "", 0);		/* Give a work area to the default drive */
	if (fr != FR_OK)
		die(fr, "mount");

	rom_putstring_uart("Please enter filename to write: \n");
	while (!readline())
	{
		rom_putstring_uart("Please enter filename to write: \n");
	}

	fr = f_open(&Fil, linebuf, FA_WRITE | FA_CREATE_ALWAYS);	/* Create a file */
	if (fr == FR_OK)
	{	
		while (!done)
		{
			if (rom_uart_available())
			{
				byte = rom_uart_read();

				if (linebufferPos > 258)
				{
					rom_putstring_uart("line too long! skip. \n");
					linebufferPos = 0;
				}

				if (byte == ':')
				{
					linebufferPos = 0;
				}
				else
				{
					linebuffer[linebufferPos] = byte;
					linebufferPos++;
				}

				if (linebufferPos == 2)
				{
					payloadLength = parse_hex_nibbles(linebuffer, 0);
				}

				if (linebufferPos == (payloadLength * 2) + 10)
				{
					// :1080D000772377237723C9E9D630D8FE0A3FD0C665
					//  0123456789
					// :00000001FF
					//  0123456789

					//address = parse_hex_nibbles(linebuffer, 2, 4);
					payloadType = parse_hex_nibbles(linebuffer, 6);

					/*rom_putstring_uart("address: \n"); 
					print_memory(&address, 2);
					rom_putstring_uart("\n"); 
					rom_putstring_uart("payloadType: \n"); 
					print_memory(&payloadType, 1);
					rom_putstring_uart("\n"); */

					if (payloadType == 0x00)
					{
						for (int i = 0; i < payloadLength; ++i)
						{
							databuffer[i] = parse_hex_nibbles(linebuffer, 8 + i * 2);
						}
						f_write(&Fil, databuffer , payloadLength, &bw);	/* Write data to the file */
						rom_putchar_uart('#');
					}
					if (payloadType == 0x01)
					{
						rom_putstring_uart("intel hex finished, writing... "); 
						fr = f_close(&Fil);							/* Close the file */
						if (fr == FR_OK)
						{
							rom_putstring_uart("done.\n");
						}
						else
						{
							die(fr, "write");
						}
					}
				}
			}
		}
	}
	else
	{
		die(fr, "open");
	}

	rom_putstring_uart("reached the end?!\n");
	for (;;) ;
}
