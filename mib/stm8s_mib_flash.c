/**
  ******************************************************************************
  * @file stm8s_mib_flash.c
  * @brief Functions to write and read data to FLASH inside the chip.
  * @author MYMEDIA Co., Ltd.
  * @version V1.0.0
  * @date 2023.1.6
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Global variables ----------------------------------------------------------*/
/* Public functions ----------------------------------------------------------*/
void mmFlashLock(void);
uint8_t mmFlashUnlock(void);
uint8_t mmFlashWrite(uint32_t vdwAddress,uint8_t vbData);
uint8_t mmFlashRead(uint32_t vdwAddress);
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------  
/*
  How to program STM8S and STM8A Flash program memory and data EEPROM (PM0051)
  <https://www.st.com/resource/en/programming_manual/pm0051-how-to-program-stm8s-and-stm8a-flash-program-memory-and-data-eeprom-stmicroelectronics.pdf>

  STM8S microcontrollers offer low density (8 Kbytes), medium density (from 16 to 32 Kbytes) 
  and high density (from 32 to 128 Kbytes) Flash program memory, plus data EEPROM.
  STM8A microcontrollers feature medium density (from 8 to 32 Kbytes) and high density
  (from 32 to 128 Kbytes) Flash memory, plus data EEPROM.

                  BLOCK      -> PAGE 
    8K  (low)     64 bytes      0 (1 page = 1 block) to n 
    32K (medium)  128 bytes     0 (1 page = 4 block) to n
    128K(high)    128 bytes     0 (1 page = 4 block) to n

  Data EEPROM and option bytes
      Unlock : Write 0xAE then 56h in FLASH_DUKR (0x00 5064)(1)(2)
      Lock   : Reset bit 3 (DUL) in FLASH_IAPSR (0x00 505F)
  Program memory
      Unlock : Write 0x56 then 0xAE in FLASH_PUKR (0x00 5062)(3)
      Lock   : Reset bit 1 (PUL) in FLASH_IAPSR (0x00 505F)

    1. The OPT and NOPT bits of FLASH_CR2 and FLASH_NCR2 registers must be set/cleared to enable access to the option bytes.
    2. If wrong keys have been entered, another key programming sequence can be issued without resetting the device.
    3. If wrong keys have been entered, the device must be reset, and a key program sequence issued.


*/

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------  
/**
  * @brief Data Flash unlocking routine
  * @par Parameters: None
  * @retval None
  */  
uint8_t mmFlashUnlock(void) 
{
  volatile uint16_t vwDelay = 0x7fff;
	volatile uint8_t DataUnlocked = 0;
	
	/* Unlock Data memory Keys registers */		
	while( DataUnlocked == 0 )
	{
    FLASH->PUKR = 0x56;
    FLASH->PUKR = 0xAE;
		
		DataUnlocked = (uint8_t)FLASH->IAPSR;	
		DataUnlocked &= FLASH_IAPSR_PUL;
    if(vwDelay) vwDelay--;
    else 
    {
      return 0;
    }
	}
  return 1;
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------  
/**
  * @brief Data Flash unlocking routine
  * @par Parameters: None
  * @retval None
  */  
void mmFlashLock(void) 
{
  /* Lock memory */  
  FLASH->IAPSR &= (uint8_t)(~FLASH_IAPSR_PUL); 
}
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------- 
/**
  * @brief Byte Flash writing routine
  * @par Parameters:
  * add: destination adress in flash
	* data_in: data array to be written in flash
  * @retval None
  */
uint8_t mmFlashWrite(uint32_t vdwAddress,uint8_t vbData) 
{	
  volatile uint16_t vwDelay = 0x7fff;
	FLASH->CR1 &= (uint8_t)(~FLASH_CR1_FIX);		// Set Standard programming time (max 6.6 ms)
  if(vdwAddress > FLASH_PROG_END_PHYSICAL_ADDRESS)
  {
    return 0;
  }
  else if(vdwAddress < FLASH_PROG_START_PHYSICAL_ADDRESS)
  {
    return 0; // false...
  }
  else
	{	
		*((PointerAttr uint8_t*)(vdwAddress)) = vbData;
	}
	while( !(FLASH->IAPSR & FLASH_IAPSR_EOP) )
  {
    if(vwDelay) vwDelay--;
    else
    {
      return 0;
    }
  }
  return 1;
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------- 
/**
  * @brief Byte Flash reading routine
  * @par Parameters:
  * add: destination adress in flash
  * data_out: array updated with read temperature
  * @retval Return value:
  * None
  */
uint8_t mmFlashRead(uint32_t vdwAddress)
{
	uint8_t vbData=0x0;
  if(vdwAddress > FLASH_PROG_END_PHYSICAL_ADDRESS)
  {
    return 0;
  }
  else if(vdwAddress < FLASH_PROG_START_PHYSICAL_ADDRESS)
  {
    return 0; // false...
  }
  else
	{
		vbData = *((uint8_t *)(vdwAddress));
		return vbData;
	}
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------- 




