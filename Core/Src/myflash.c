#include "myflash.h"
#define STM_SECTOR_SIZE     		  1024   //1K�ֽ�
#define STM32_FLASH_BASE          0x08000000 		//STM32 FLASH����ʼ��ַ
#define FLASH_USER_START_ADDR     ( STM32_FLASH_BASE + STM_SECTOR_SIZE * 63 ) //дFlash�ĵ�ַ������ӵ�12ҳ��ʼ


extern SystemDef sys;
 /**********************************************************************************
  * ��������: ҳ����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������ 
  */
FLASH_EraseInitTypeDef EraseInitStruct;
uint32_t PAGEError = 0;
void Flash_Erase(void)
{  	
   EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
   EraseInitStruct.PageAddress = FLASH_USER_START_ADDR;
   EraseInitStruct.NbPages     = 1;
   if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)  
	 {
		 HAL_FLASH_Lock();  
     Error_Handler();
	 }
}
void ResetInf(void)   
{
	uint32_t addr=FLASH_USER_START_ADDR;
	HAL_FLASH_Unlock();
	Flash_Erase();
	while(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,addr,0x12345678));	//��ʼID
	while(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,addr+4,0));		

	HAL_FLASH_Lock();
}
void WriteInf(void)
{
	uint32_t addr=FLASH_USER_START_ADDR;
	HAL_FLASH_Unlock();
	Flash_Erase();
	while(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,addr,0x12345678));
	while(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,addr+4,sys.language_flash));	//�ն�
	HAL_FLASH_Lock();
}
void ReadInf(void)
{
	uint32_t dateread[2];		//4���ֽڵĴ洢��Ϣ
	uint32_t addr = FLASH_USER_START_ADDR;
	dateread[0]    = *(__IO uint32_t *)(addr);	
  sys.language_flash     = *(__IO uint32_t *)(addr+4);
	if(dateread[0]!=0x12345678)	//������µ��ڴ��
	{
		ResetInf();
		ReadInf(); 
	}
}
