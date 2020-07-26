/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */
#include "romfunctions.h"


void print_memory(const void *addr, uint16_t size);

void print_address(void* ptr)
{
    uint16_t printed = 0;
    int16_t i;
   
    for (i=sizeof(ptr) - 1; i >= 0; i--)
    {
        unsigned char pc = ((uint32_t) ptr >> (i*8)) & 0xFF;

        int  g;
        g = (pc >> 4) & 0xf;
        g += g >= 10 ? 'a'-10 : '0';
        rom_putchar_uart(g);
        printed++;

        g = pc & 0xf;
        g += g >= 10 ? 'a'-10 : '0';
        rom_putchar_uart(g);
        printed++;
        if (printed % 32 == 0) rom_putchar_uart('\n');
        else if (printed % 4 == 0) rom_putchar_uart(' ');
    }
}


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;
	int result;

	return 0;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;
	int result;

	rom_cf_init();

	return 0;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	DRESULT res;
	int result;

/*	rom_putstring_uart("disk_read: ");
	rom_putstring_uart("buff_a:");
	print_address(buff);

	rom_putstring_uart(" lba: "); 
	print_memory(&sector, 4);
	rom_putstring_uart(" count: "); 
	print_memory(&count, 2);
	rom_putstring_uart("\n");
*/
	
	for (uint8_t i = 0; i < count; ++i)
	{
		rom_cf_read(sector + i, (buff + (i * 512)));
	}

	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	DRESULT res;
	int result;

	rom_putstring_uart("disk_write: ");
	rom_putstring_uart("buff_a:");
	print_address(buff);

	rom_putstring_uart(" lba: "); 
	print_memory(&sector, 4);
	rom_putstring_uart(" count: "); 
	print_memory(&count, 2);
	rom_putstring_uart("\n");


	for (int i = 0; i < count; ++i)
	{
		rom_cf_write(sector + i, (buff + (i * 512)));
	}

	return RES_OK;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;
	int result;

	rom_putstring_uart("ioctl: "); 
	print_memory(&cmd, 1);
	rom_putstring_uart("\n"); 

	/*switch (pdrv) {
	case DEV_RAM :

		// Process of the command for the RAM drive

		return res;

	case DEV_MMC :

		// Process of the command for the MMC/SD card

		return res;

	case DEV_USB :

		// Process of the command the USB drive

		return res;
	}*/

	return RES_OK;
}

