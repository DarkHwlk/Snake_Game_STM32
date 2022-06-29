#include "flash.h"

/* function remove data in flash */
void Flash_Erase(uint32_t address){
	HAL_FLASH_Unlock();
	FLASH_EraseInitTypeDef EraseInitStruct;
	EraseInitStruct.Banks = 1;  //choose Bank - all pages
	EraseInitStruct.NbPages = 1;  //remove 1 page
	EraseInitStruct.PageAddress = address;  //address page
	EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;  //type remove: page
	uint32_t pageerr;  //create output (non importance)
	HAL_FLASHEx_Erase(&EraseInitStruct, &pageerr);
	HAL_FLASH_Lock();
}
/* function write int */
void Flash_Write_Int(uint32_t address, int value){
	HAL_FLASH_Unlock();
	/* int = 2 bytes == halfword */
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,address,value);
	HAL_FLASH_Lock();
}
/* function write float */
void Flash_Write_Float(uint32_t address, float f){
	uint8_t data[4];
	*(float*)data = f;
	HAL_FLASH_Unlock();
	/* float = 4 bytes == word */
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,address,*(uint32_t*)data);
	HAL_FLASH_Lock();
}
/* function write array */
void Flash_Write_Array(uint32_t address, uint8_t *arr, uint16_t lenght){
	HAL_FLASH_Unlock();
	uint16_t *pt = (uint16_t*)arr;  //moi lan lay 2 byte trong arr
	for(uint16_t i=0; i<(lenght+1)/2; i++){
		/* float = 4 bytes == word */
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,address + 2*i,*pt);
		pt++;  //moi lan cong dia chi pt dich 16 bit
	}
	HAL_FLASH_Lock();
}
/* function write struct */
void Flash_Write_Struct(uint32_t address, wifi_info_t dta){
	Flash_Write_Array(address, (uint8_t*)(&dta), sizeof(dta));	
}

/* function read int */
int Flash_Read_Int(uint32_t address){
	return *(__IO uint16_t *)(address);	
}
/* function read float */
float Flash_Read_Float(uint32_t address){	
	uint32_t data = *(__IO uint32_t *)(address);
	return *(float*)(&data);  //ep kieu float
}
/* function read array */
void Flash_Read_Array(uint32_t address, uint8_t *arr,uint16_t lenght){
	uint16_t *pt = (uint16_t*)arr;
	for(uint16_t i=0; i<(lenght+1)/2; i++){
		*pt = *(__IO uint16_t *)(address+2*i);
		pt++;
	}
}
/* function read struct */
void Flash_Read_Struct(uint32_t address, wifi_info_t *dta){
	Flash_Read_Array(address, (uint8_t*)(&dta), sizeof(dta));
}

