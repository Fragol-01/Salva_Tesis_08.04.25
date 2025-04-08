#ifndef __FATFS_SD_H
#define __FATFS_SD_H

#include <stdint.h>
#include <stdbool.h>
#include "main.h"

#define _USE_WRITE 1// Enable write function

// Define los tipos necesarios que anteriormente venían de FatFS
typedef uint8_t DRESULT;  // Disk operation result
typedef uint32_t DWORD;   // 32-bit unsigned integer
typedef uint16_t WORD;    // 16-bit unsigned integer
typedef uint16_t UINT;    // 16-bit unsigned integer

// Define status y result codes
#define STA_NOINIT      0x01    // Drive not initialized
#define STA_NODISK      0x02    // No medium in the drive
#define STA_PROTECT     0x04    // Write protected

#define RES_OK          0x00    // Successful
#define RES_ERROR       0x01    // R/W Error
#define RES_WRPRT       0x02    // Write Protected
#define RES_NOTRDY      0x03    // Not Ready
#define RES_PARERR      0x04    // Invalid Parameter

// Control codes for disk_ioctl
#define CTRL_SYNC           0   // Make sure that no pending write process
#define GET_SECTOR_COUNT    1   // Get number of sectors on the disk
#define GET_SECTOR_SIZE     2   // Get size of sector
#define CTRL_POWER          5   // Get/Set power status
#define MMC_GET_CSD         10  // Get CSD
#define MMC_GET_CID         11  // Get CID
#define MMC_GET_OCR         12  // Get OCR

/* MMC card type flags (MMC_GET_TYPE) */
#define CT_MMC		0x01		/* MMC version 3 */
#define CT_SD1		0x02		/* SD version 1 */
#define CT_SD2		0x04		/* SD version 2 */
#define CT_SDC		0x06		/* SD */
#define CT_BLOCK	0x08		/* Block addressing */

// SPI Timeout in ms
#define SPI_TIMEOUT 100

// Comandos SD
#define CMD0     (0x40+0)     	/* GO_IDLE_STATE */
#define CMD1     (0x40+1)     	/* SEND_OP_COND */
#define CMD8     (0x40+8)     	/* SEND_IF_COND */
#define CMD9     (0x40+9)     	/* SEND_CSD */
#define CMD10    (0x40+10)    	/* SEND_CID */
#define CMD12    (0x40+12)    	/* STOP_TRANSMISSION */
#define CMD16    (0x40+16)    	/* SET_BLOCKLEN */
#define CMD17    (0x40+17)    	/* READ_SINGLE_BLOCK */
#define CMD18    (0x40+18)    	/* READ_MULTIPLE_BLOCK */
#define CMD23    (0x40+23)    	/* SET_BLOCK_COUNT */
#define CMD24    (0x40+24)    	/* WRITE_BLOCK */
#define CMD25    (0x40+25)    	/* WRITE_MULTIPLE_BLOCK */
#define CMD41    (0x40+41)    	/* SEND_OP_COND (ACMD) */
#define CMD55    (0x40+55)    	/* APP_CMD */
#define CMD58    (0x40+58)    	/* READ_OCR */

// Funciones para manejar la SD
void spi_cs_sd_write(bool state);
bool SD_disk_initialize(uint8_t drv);
bool SD_disk_status(uint8_t drv);
DRESULT SD_disk_read(uint8_t pdrv, uint8_t* buff, uint32_t sector, uint16_t count);
DRESULT SD_disk_write(uint8_t pdrv, const uint8_t* buff, uint32_t sector, uint16_t count);
DRESULT SD_disk_ioctl(uint8_t pdrv, uint8_t cmd, void* buff);

#endif
