#ifndef __FLASH_H
#define __FLASH_H
#include <stdint.h>
#include <string.h>
#include "stm32f1xx.h"

#pragma park(1)  //chong phan manh bo nho
/* struct */
typedef struct{
	uint8_t no;
	uint8_t ssid[30];
	uint8_t pass[30];
}wifi_info_t;
#pragma park()

/* function remove data in flash */
void Flash_Erase(uint32_t address);
/* function write int */
void Flash_Write_Int(uint32_t address, int value);
/* function write float */
void Flash_Write_Float(uint32_t address, float f);
/* function write array */
void Flash_Write_Array(uint32_t address, uint8_t *arr, uint16_t lenght);
/* function write struct */
void Flash_Write_Struct(uint32_t address, wifi_info_t dta);

/* function read int */
int Flash_Read_Int(uint32_t address);
/* function read float */
float Flash_Read_Float(uint32_t address);
/* function read array */
void Flash_Read_Array(uint32_t address, uint8_t *arr,uint16_t lenght);
/* function read struct */
void Flash_Read_Struct(uint32_t address, wifi_info_t *dta);

#endif

