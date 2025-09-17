/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "mbedtls.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "usbd_desc.h"
#include "usbd_dfu.h"
#include "usbd_dfu_flash.h"

#include "mbedtls/aes.h"
#include <stdio.h>
#include <string.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#ifndef HSEM_ID_0
#define HSEM_ID_0 (0U) /* HW semaphore 0*/
#endif

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

#define DFU_BOOT_FLAG 0xDEADBEEF
typedef void (*pFunction)(void);
USBD_HandleTypeDef USBD_Device;
extern USBD_HandleTypeDef hUsbDeviceHS;

#define BOOTLOADER_ADDRESS 0x08000000
#define APP1_ADDRESS 0x08040000
#define APP2_ADDRESS 0x080A0000
#define APP_SIZE 0x60000         // 384 KB
#define CRC_ADDRESS_OFFSET (APP_SIZE - 4) // 0x080FFFFC for App2

// Define flash address where encrypted data is stored (adjust as needed)
#define FLASH_ADDRESS 0x080A0000  // Example: QSPI flash base address
#define ENCRYPTED_DATA_SIZE  50640  // Total encrypted data size (bytes)
#define EXPECTED_SUBSET_SIZE 32   // Size of subset for verification

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

CRC_HandleTypeDef hcrc;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

extern int _bflag;
uint32_t *dfu_boot_flag;
pFunction JumpToApplication;
uint32_t JumpAddress;

uint32_t end_address = 0x080AC298;
uint32_t stored_crc;
uint32_t crc_value = 0x14A57875;
int found = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_CRC_Init(void);
/* USER CODE BEGIN PFP */

static void MPU_Config(void);
static void CPU_CACHE_Enable(void);
void Error_Handler(void);
//uint32_t Verify_Application(uint32_t app_address);
//uint32_t CalculateCRC(uint32_t start_address, uint32_t size);
//uint32_t CalculateCRC(void* start_address, size_t size);
uint32_t calculate_crc32(uint32_t start_addr, uint32_t end_addr);
void search_crc_value();
void search_end_address();

HAL_StatusTypeDef erase_flash_sectors(uint32_t start_address, uint32_t size);
HAL_StatusTypeDef program_flash(uint32_t address, const unsigned char *data, uint32_t size);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

//	// Calculate CRC from 0x080A0000 to firmware end
//		    uint32_t start_address = 0x080A0000;
//		    uint32_t end_address = 0x080AC298;
//		    uint32_t stored_crc = *(uint32_t *)0x080AC29C;

//	// Ensure end_address is valid
//	 if (end_address > start_address && end_address <= 0x080fffff) {
//     uint32_t crc_result = calculate_crc32(start_address, end_address);
//	 // Use crc_result (e.g., compare with stored CRC for validation)
//	 printf("crc result: %08X \n\r", crc_result);
//     printf("end address: 0x%08X \n\r", end_address);
//      }
//
//	 printf("crc stored: %08X \n\r", stored_crc);

	// Enable CRC clock
//	    __HAL_RCC_CRC_CLK_ENABLE();

	// Initialize CRC before any jump
//	MX_CRC_Init();

//	    // Calculate CRC of the application (excluding the CRC value)
//	      uint32_t computed_crc = CalculateCRC(APP2_ADDRESS, APP_SIZE - 8); // Exclude last 8 bytes (4 bytes before CRC + 4 bytes CRC
//
//	          printf("Calculated CRC: %08X \n\r", computed_crc); // 0x%08x
//
//	      // Read the stored CRC from the binary
//	      uint32_t stored_crc = *(volatile uint32_t*)(APP2_ADDRESS + CRC_ADDRESS_OFFSET);

//	          printf("Stored CRC: 0x%08x \n\r", stored_crc);

	          // Read stored CRC
//	          uint32_t stored_crc = *(__IO uint32_t *)(APP2_ADDRESS + CRC_ADDRESS_OFFSET);
//	          printf("Stored CRC: 0x%08X\r\n", stored_crc);

	 /* Configure the MPU attributes */
//	  MPU_Config();

	  /* Enable the CPU Cache */
//	  CPU_CACHE_Enable();

	printf("-------------Bootloader User Begin 1 -------------\n\r");

	///////////////////////////////////////////////////////////////////////////////////

//	// Check and jump to applications (comment this block for USB DFU upload)
//	    if (Verify_Application(APP1_ADDRESS)){
//
	  printf("Jumping to Firmware\n\r");
	  dfu_boot_flag = (uint32_t*) (&_bflag); // set in linker script

	  if (*dfu_boot_flag != DFU_BOOT_FLAG) {

	      /* Test if user code is programmed starting from address 0x08000000 */
	      if (((*(__IO uint32_t*) USBD_DFU_APP_DEFAULT_ADD) & 0x2FF80000) == 0x24000000) {

	          /* Jump to user application */
	          JumpAddress = *(__IO uint32_t*) (USBD_DFU_APP_DEFAULT_ADD + 4);
	          JumpToApplication = (pFunction) JumpAddress;

	          /* Initialize user application's Stack Pointer */
	          __set_MSP(*(__IO uint32_t*) USBD_DFU_APP_DEFAULT_ADD);
	          JumpToApplication();
	      }
	  }
	    *dfu_boot_flag=0;
//
//	    }

//	    	printf("Jumping to Second Firmware\n\r");
//
//	    	printf("h7 decrypting code \n\r");
//
//	    	MX_MBEDTLS_Init();
//
//	    	// The key (same as in Python script)
//	    	 unsigned char key[16] = {
//	    	  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
//	    	  0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35
//	    	  };
//
//	     // Read encrypted data from flash
//	     unsigned char *encrypted_data = (unsigned char *)FLASH_ADDRESS;
//	     size_t encrypted_len = ENCRYPTED_DATA_SIZE;
//
//	     //      unsigned char encrypted_data[] = {
//	     //      0xE0, 0xBB, 0x78, 0xC4, 0x35, 0x37, 0x3C, 0xF6,
//	     //      0x8E, 0x4D, 0xBB, 0x03, 0x80, 0xF1, 0x3D, 0x97,
//	     //      0xEC, 0x3D, 0xB7, 0x55, 0xAF, 0x8B, 0x18, 0xE7,
//	     //      0x1E, 0xFE, 0xF2, 0x05, 0x45, 0x45, 0x47, 0xC1
//	     //            };
//	     //     size_t encrypted_len = sizeof(encrypted_data);
//
//	     // Buffer for decrypted data
//	     unsigned char decrypted_data[ENCRYPTED_DATA_SIZE];
//	     memset(decrypted_data, 0, sizeof(decrypted_data));
//
//	     // Expected decrypted data (first 32 bytes from your provided padded_data)
//	     const unsigned char expected[] = {
//	     0xFC, 0xFF, 0x07, 0x24, 0xD9, 0x0E, 0x0A, 0x08,
//	     0xE1, 0x0C, 0x0A, 0x08, 0xE9, 0x0C, 0x0A, 0x08,
//	     0xF1, 0x0C, 0x0A, 0x08, 0xF9, 0x0C, 0x0A, 0x08,
//	     0x01, 0x0D, 0x0A, 0x08, 0x00, 0x00, 0x00, 0x00
//	    };
//
//	     // Expected decrypted data (first 32 bytes from your provided padded_data)
//	     const unsigned char decrypted_expected[] = {
//	      0xFC, 0xFF, 0x07, 0x24
//	    };
//
//	    // Initialize mbedTLS AES context
//	    mbedtls_aes_context aes;
//	    mbedtls_aes_init(&aes);
//
//	    // Set decryption key
//	    if (mbedtls_aes_setkey_dec(&aes, key, 128) != 0) {
//	     printf("Error: Failed to set decryption key\n");
//	     mbedtls_aes_free(&aes);
//	     while (1);
//	    }
//
//	    // Decrypt the data (ECB mode, block-by-block)
//	    for (size_t i = 0; i < encrypted_len; i += 16) {
//	    if (mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_DECRYPT, &encrypted_data[i], &decrypted_data[i]) != 0) {
//	     printf("Error: Decryption failed\n");
//	    mbedtls_aes_free(&aes);
//	     while (1);
//	     }
//	   }
//
//	   // Free AES context
//	   mbedtls_aes_free(&aes);
//
//	   // Print the decrypted data (first 32 bytes for verification)
//	   print_string("Decrypted data: ", decrypted_data, encrypted_len);
//
//	   // Verify the first 32 bytes against expected
////	   if (memcmp(decrypted_data, expected, EXPECTED_SUBSET_SIZE) == 0) {
////	   printf("Success: First %d bytes of decrypted data match expected!\n", EXPECTED_SUBSET_SIZE);
////	   } else {
////	   printf("Error: First %d bytes of decrypted data do not match expected\n", EXPECTED_SUBSET_SIZE);
////	   }
//
//if(memcmp(decrypted_data, decrypted_expected, 4) == 0){
//
// printf("writing the decrypted data to flash!\n");
//
//	   // Erase internal flash sectors
//	   	   if (erase_flash_sectors(FLASH_ADDRESS, ENCRYPTED_DATA_SIZE) != HAL_OK) {
//	   	    while (1);
//	   	   }
//
//	   	   // Program decrypted data to internal flash
//	   	   if (program_flash(FLASH_ADDRESS, decrypted_data, ENCRYPTED_DATA_SIZE) != HAL_OK) {
//	   	   while (1);
//	   	   }
//}
//	   	   // Verify written data by reading back (optional)
//	   	   unsigned char *flash_data = (unsigned char *)FLASH_ADDRESS;
//	   	   print_string("Flash data at 0x080A0000: ", flash_data, encrypted_len);
//
//	   	   if (memcmp(flash_data, decrypted_expected, 4) == 0) {
//	   	   printf("flash data already written! no need to write it again \n", 4);
//	   	   }
//	   	   else {
//	   	   printf("invalid flash data written! need to write it again \n", 4);
//	   	   }
//
//	    dfu_boot_flag = (uint32_t*) (&_bflag); // set in linker script
//
//	    if (*dfu_boot_flag != DFU_BOOT_FLAG) {
//
//	     /* Test if user code is programmed starting from address 0x08000000 */
//	     if (((*(__IO uint32_t*) 0x080A0000) & 0x2FF80000) == 0x24000000) {
//
//	    /* Jump to user application */
//	    JumpAddress = *(__IO uint32_t*) (0x080A0000 + 4);
//	    JumpToApplication = (pFunction) JumpAddress;
//
//	     /* Initialize user application's Stack Pointer */
//	      __set_MSP(*(__IO uint32_t*) 0x080A0000);
//
//	    // Ensure end_address is valid
////	     if (end_address > 0x080A0000 && end_address <= 0x080fffff) {
////	     uint32_t crc_result = calculate_crc32(0x080A0000, end_address);
//	     // Use crc_result (e.g., compare with stored CRC for validation)
////	      printf("crc result: %08X \n\r", crc_result);
////	       printf("end address: 0x%08X \n\r", end_address);
////	      }
//
//	   search_end_address();
//
//	   search_crc_value();
//
//	  // CRC matches, jump to application
//	  if (found)
//	   {
//
//	  JumpToApplication();
//	  }
//   }
//
//       // CRC does not match, jump to application
//	   if (!found){
//
//	   printf("Jumping to first application Firmware\n\r");
//
//	   printf("Stack pointer: 0x%08lX\n", *(__IO uint32_t*)USBD_DFU_APP_DEFAULT_ADD);
//
//	   /* Test if user code is programmed starting from address 0x08000000 */
//	    if (((*(__IO uint32_t*) USBD_DFU_APP_DEFAULT_ADD) & 0x2FF80000) == 0x24000000) {
//
//	    /* Jump to user application */
//	    JumpAddress = *(__IO uint32_t*) (USBD_DFU_APP_DEFAULT_ADD + 4);
//	    JumpToApplication = (pFunction) JumpAddress;
//
//	    /* Initialize user application's Stack Pointer */
//	    __set_MSP(*(__IO uint32_t*) USBD_DFU_APP_DEFAULT_ADD);
//	    JumpToApplication();
//	    }
//
//      }
//
//   }
//        *dfu_boot_flag=0;

	/////////////////////////////////////////////////////////////////////////////////

  /* USER CODE END 1 */
/* USER CODE BEGIN Boot_Mode_Sequence_0 */
  int32_t timeout;
/* USER CODE END Boot_Mode_Sequence_0 */

/* USER CODE BEGIN Boot_Mode_Sequence_1 */
  /* Wait until CPU2 boots and enters in stop mode or timeout*/
  timeout = 0xFFFF;
  while((__HAL_RCC_GET_FLAG(RCC_FLAG_D2CKRDY) != RESET) && (timeout-- > 0));
  if ( timeout < 0 )
  {
  Error_Handler();
  }
/* USER CODE END Boot_Mode_Sequence_1 */
  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* Configure the peripherals common clocks */
  PeriphCommonClock_Config();
/* USER CODE BEGIN Boot_Mode_Sequence_2 */
/* When system initialization is finished, Cortex-M7 will release Cortex-M4 by means of
HSEM notification */
/*HW semaphore Clock enable*/
__HAL_RCC_HSEM_CLK_ENABLE();
/*Take HSEM */
HAL_HSEM_FastTake(HSEM_ID_0);
/*Release HSEM in order to notify the CPU2(CM4)*/
HAL_HSEM_Release(HSEM_ID_0,0);
/* wait until CPU2 wakes up from stop mode */
timeout = 0xFFFF;
while((__HAL_RCC_GET_FLAG(RCC_FLAG_D2CKRDY) == RESET) && (timeout-- > 0));
if ( timeout < 0 )
{
Error_Handler();
}
/* USER CODE END Boot_Mode_Sequence_2 */

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_USB_DEVICE_Init();
  MX_CRC_Init();
  MX_MBEDTLS_Init();
  /* USER CODE BEGIN 2 */

  /* -2- Unlock the Flash to enable the flash control register access
   * ************ */
  HAL_FLASH_Unlock();

  printf("-------------Bootloader User Begin 2 -------------\n\r");

//  HAL_PWREx_EnableUSBVoltageDetector(); // no hard fault
//
//  	    /* Otherwise enters DFU mode to allow user programming his application */
//  	    /* Init Device Library */
//  	    USBD_Init(&USBD_Device, &HS_Desc, 0); // no hard fault
////
//  	    /* Add Supported Class */
//  	    USBD_RegisterClass(&USBD_Device, USBD_DFU_CLASS); // no hard fault
////
//  	    /* Add DFU Media interface */
//  	    USBD_DFU_RegisterMedia(&USBD_Device, &USBD_DFU_Flash_fops); // no hard fault
////
//  	    /* Start Device Process */
//  	    USBD_Start(&USBD_Device); // yaha masla hai, yes hard fault

//  	    MX_USB_DEVICE_Init();

    if (USBD_Init(&hUsbDeviceHS, &HS_Desc, DEVICE_HS) != USBD_OK) // DEVICE_HS, pehla change
    {
      Error_Handler();
    }
    if (USBD_RegisterClass(&hUsbDeviceHS, USBD_DFU_CLASS) != USBD_OK) // &USBD_DFU, dusra change
    {
      Error_Handler();
    }
    if (USBD_DFU_RegisterMedia(&hUsbDeviceHS, &USBD_DFU_Flash_fops) != USBD_OK) // &USBD_DFU_fops_HS, teesra change
    {
      Error_Handler();
    }
    if (USBD_Start(&hUsbDeviceHS) != USBD_OK)
    {
      Error_Handler();
    }

    HAL_PWREx_EnableUSBVoltageDetector();

//    	  printf("Jumping to Firmware\n\r");
//    	  dfu_boot_flag = (uint32_t*) (&_bflag); // set in linker script
//
//    	  if (*dfu_boot_flag != DFU_BOOT_FLAG) {
//
//    	      /* Test if user code is programmed starting from address 0x08000000 */
//    	      if (((*(__IO uint32_t*) USBD_DFU_APP_DEFAULT_ADD) & 0x2FF80000) == 0x24000000) {
//
////    	    	  uint32_t app_stack = *(__IO uint32_t*) APP_BASE_ADDR;
//
//    	          /* Jump to user application */
//    	          JumpAddress = *(__IO uint32_t*) (USBD_DFU_APP_DEFAULT_ADD + 4);
//    	          JumpToApplication = (pFunction) JumpAddress;
//
//    	            // --- Cleanup: ensure STM32 can jump cleanly to app ---
//
//    	            __disable_irq();              // Disable global interrupts
//
//    	            SysTick->CTRL = 0;            // Stop SysTick
//    	            SysTick->LOAD = 0;
//    	            SysTick->VAL = 0;
//
//    	            HAL_RCC_DeInit();             // Reset RCC to default state
//    	            HAL_DeInit();                 // Reset all HAL drivers
//
//    	            SCB_DisableICache();          // Optional, clean reset
//    	            SCB_DisableDCache();          // Optional, clean reset
//
//    	            SCB->VTOR = USBD_DFU_APP_DEFAULT_ADD;    // Set vector table base to app
//
////    	            __set_MSP(app_stack);         // Set main stack pointer to app's
//
//    	          /* Initialize user application's Stack Pointer */
//    	          __set_MSP(*(__IO uint32_t*) USBD_DFU_APP_DEFAULT_ADD);
//    	          JumpToApplication();
//    	      }
//    	  }
//    	    *dfu_boot_flag=0;


// 	  for (int x = 0; x < 10000; x++) {
// 	      HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_11); // Toggle PA11
// 	      HAL_Delay(50);                          // Delay 50 ms
// 	  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

//      HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_11); // Toggle PA11
//      HAL_Delay(500); // Delay for 500 ms
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_DIRECT_SMPS_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI
                              |RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 48;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 5;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV1;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_HSI, RCC_MCODIV_1);
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInitStruct.PLL2.PLL2M = 2;
  PeriphClkInitStruct.PLL2.PLL2N = 12;
  PeriphClkInitStruct.PLL2.PLL2P = 2;
  PeriphClkInitStruct.PLL2.PLL2Q = 2;
  PeriphClkInitStruct.PLL2.PLL2R = 2;
  PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_3;
  PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOMEDIUM;
  PeriphClkInitStruct.PLL2.PLL2FRACN = 0;
  PeriphClkInitStruct.AdcClockSelection = RCC_ADCCLKSOURCE_PLL2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief CRC Initialization Function
  * @param None
  * @retval None
  */
static void MX_CRC_Init(void)
{

  /* USER CODE BEGIN CRC_Init 0 */

  /* USER CODE END CRC_Init 0 */

  /* USER CODE BEGIN CRC_Init 1 */

  /* USER CODE END CRC_Init 1 */
  hcrc.Instance = CRC;
  hcrc.Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_ENABLE;
  hcrc.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_ENABLE;
  hcrc.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_NONE;
  hcrc.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE;
  hcrc.InputDataFormat = CRC_INPUTDATA_FORMAT_WORDS;
  if (HAL_CRC_Init(&hcrc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CRC_Init 2 */

  /* USER CODE END CRC_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOI_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);

  /*Configure GPIO pin : CEC_CK_MCO1_Pin */
  GPIO_InitStruct.Pin = CEC_CK_MCO1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF0_MCO;
  HAL_GPIO_Init(CEC_CK_MCO1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PA11 */
  GPIO_InitStruct.Pin = GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

uint32_t calculate_crc32(uint32_t start_addr, uint32_t end_addr) {
    // Validate address range
    if (end_addr <= start_addr || end_addr > 0x080fffff) {
        return 0; // Invalid range or out of FLASH bounds (0x080A0000 + 384K)
    }

    // Enable CRC clock
//    RCC->AHB1ENR |= RCC_AHB4ENR_CRCEN;
    __HAL_RCC_CRC_CLK_ENABLE();

    // Reset CRC unit
    CRC->CR = CRC_CR_RESET;

    // Configure CRC (Ethernet CRC32 settings)
    CRC->INIT = 0xFFFFFFFF; // Initial value
    CRC->CR &= ~(CRC_CR_REV_IN | CRC_CR_REV_OUT); // No input/output reversal

    // Process memory range
    uint32_t *data = (uint32_t *)start_addr;
    uint32_t word_count = (end_addr - start_addr) / 4; // Number of 32-bit words

    for (uint32_t i = 0; i < word_count; i++) {
        CRC->DR = data[i]; // Feed 32-bit word
    }

    // Handle remaining bytes (if end_addr is not 32-bit aligned)
    uint32_t remaining_bytes = (end_addr - start_addr) % 4;
    if (remaining_bytes > 0) {
        uint8_t *byte_data = (uint8_t *)&data[word_count];
        uint32_t last_word = 0;
        for (uint32_t i = 0; i < remaining_bytes; i++) {
            last_word |= (byte_data[i] << (8 * (3 - i))); // Pack bytes into a word
        }
        CRC->DR = last_word; // Feed the last partial word
    }

    // Read CRC result and apply final XOR
    uint32_t crc = CRC->DR ^ 0xFFFFFFFF;

    // Disable CRC clock
//    RCC->AHB1ENR &= ~RCC_AHB4ENR_CRCEN;
    __HAL_RCC_CRC_CLK_DISABLE();

    return crc;
}

void search_crc_value() {
    // Define the flash memory range
    uint8_t *start_addr = (uint8_t *)0x080A0000;
    uint8_t *end_addr = (uint8_t *)0x080FFFFF;

    uint32_t *data_ptr;


    // Calculate the size of the range
    uint32_t range_size = (uint32_t)(end_addr - start_addr + 1);

    // Search through the range
    for (uint32_t i = 0; i < range_size - 3; i += 4) {
        data_ptr = (uint32_t *)(start_addr + i);
        if (*data_ptr == crc_value) {
            printf("CRC value 0x%08X found at address 0x%08X\n", crc_value, (uint32_t)start_addr + i);
            found = 1;
        }
    }

    if (!found) {
        printf("CRC value 0x%08X not found in the range 0x%08X to 0x%08X.\n", crc_value, (uint32_t)start_addr, (uint32_t)end_addr);
    }
}

void search_end_address() {
    // Define the flash memory range
    uint8_t *start_address_flash = (uint8_t *)0x080A0000;
    uint8_t *end_address_flash = (uint8_t *)0x080FFFFF;
    uint32_t end_address_value = 0xFFFFFFFF;
    uint32_t *data_ptr_address;
    int found_address = 0;
    uint32_t first_occurrence = 0;
        uint32_t second_occurrence = 0; // Address of the second occurrence
        uint32_t latest_sequence_start = 0; // Start of the latest consistent sequence
        uint32_t consecutive_count = 0;
        uint32_t prev_addr = 0; // Track the previous address to check consecutiveness

    // Calculate the size of the range
    uint32_t range_size_address = (uint32_t)(end_address_flash - start_address_flash + 1);

    // Search through the range
    for (uint32_t j = 0; j < range_size_address - 3; j += 4) {
        data_ptr_address = (uint32_t *)(start_address_flash + j);
        uint32_t current_addr = (uint32_t)start_address_flash + j;
        if (*data_ptr_address == end_address_value) {

        	if (!found_address) {
        	                // First occurrence
        	                first_occurrence = current_addr;
        	                found_address = 1;
        	                consecutive_count = 1;
        	                latest_sequence_start = current_addr;
        	                prev_addr = current_addr;
        	            } else {
        	                // Check if consecutive (4-byte increment)
        	                if (current_addr == prev_addr + 4) {
        	                    consecutive_count++;
        	                    if (consecutive_count == 1) {
        	                    latest_sequence_start = current_addr; // Update start of new sequence
        	                   }
        	                } else {
        	                	// Non-consecutive, reset count and update sequence start
        	                	consecutive_count = 1;
        	                    latest_sequence_start = current_addr;
        	                }
        	                prev_addr = current_addr;
        	                if (consecutive_count >= 5) {
        	                    printf("end address value 0x%08X found consistently more than 5 times, first address of latest sequence: 0x%08X\n", end_address_value, latest_sequence_start);

        	                    return;
        	                }
        	            }
        }
    }

    // If only found once or not at all
        if (!found_address) {
            printf("end address value 0x%08X not found in the range 0x%08X to 0x%08X.\n", end_address_value, (uint32_t)start_address_flash, (uint32_t)end_address_flash);

        } else if (found_address && consecutive_count < 5) {
            printf("end address value 0x%08X found only once at address 0x%08X.\n", end_address_value, first_occurrence);

        }
}

void print_string(const char *prefix, const unsigned char *data, size_t len) {
    printf("%s", prefix);
    for (size_t i = 0; i < len && i < 32; i++) { // Limit to 32 bytes for display
        if (data[i] >= 32 && data[i] <= 126) { // Printable ASCII
            printf("%c", data[i]);
        } else {
            printf("\\x%02X", data[i]); // Non-printable as hex
        }
    }
    if (len > 32) {
        printf("... [%zu more bytes]", len - 32);
    }
    printf("\n");
}

// Flash erase and program functions
HAL_StatusTypeDef erase_flash_sectors(uint32_t start_address, uint32_t size) {
    FLASH_EraseInitTypeDef erase_init = {0};
    uint32_t sector_error = 0;

    // Calculate first sector and number of sectors
    uint32_t first_sector = (start_address - 0x08000000) / 0x20000; // 128 KB per sector
    uint32_t nb_sectors = (size + 0x20000 - 1) / 0x20000; // Ceiling division

    erase_init.TypeErase = FLASH_TYPEERASE_SECTORS;
    erase_init.Banks = FLASH_BANK_1;
    erase_init.Sector = 5; // first_sector
    erase_init.NbSectors = 1; // nb_sectors
    erase_init.VoltageRange = FLASH_VOLTAGE_RANGE_3;

    // Unlock flash
    if (HAL_FLASH_Unlock() != HAL_OK) {
        printf("Error: Flash unlock failed\n");
        return HAL_ERROR;
    }

    // Erase sectors
    if (HAL_FLASHEx_Erase(&erase_init, &sector_error) != HAL_OK) {
        printf("Error: Flash erase failed, sector error: %lu\n", sector_error);
//        HAL_FLASH_Lock();
        return HAL_ERROR;
    }

    return HAL_OK;
}

HAL_StatusTypeDef program_flash(uint32_t address, const unsigned char *data, uint32_t size) {
    // Program data in 32-byte chunks (256 bits)
    for (uint32_t i = 0; i < size; i += 32) {
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, address + i, (uint32_t)&data[i]) != HAL_OK) {
            printf("Error: Flash program failed at address 0x%08lX\n", address + i);
//            HAL_FLASH_Lock();
            return HAL_ERROR;
        }
    }

//    HAL_FLASH_Lock();
    return HAL_OK;
}


//void search_end_address() {
//    // Define the flash memory range
//    uint8_t *start_address_flash = (uint8_t *)0x080A0000;
//    uint8_t *end_address_flash = (uint8_t *)0x080FFFFF;
//    uint32_t end_address_value = 0xFFFFFFFF;
//    uint32_t *data_ptr_address;
//    int found_address = 0;
//
//    // Calculate the size of the range
//    uint32_t range_size_address = (uint32_t)(end_address_flash - start_address_flash + 1);
//
//    // Search through the range
//    for (uint32_t j = 0; j < range_size_address - 3; j += 4) {
//        data_ptr_address = (uint32_t *)(start_address_flash + j);
//        if (*data_ptr_address == end_address_value) {
//            printf("end address value 0x%08X found at address 0x%08X\n", end_address_value, (uint32_t)start_address_flash + j);
//            found_address = 1;
//            uint32_t crc_result = calculate_crc32(0x080A0000, (((uint32_t)start_address_flash + j)-8));
//            printf("crc result: %08X \n\r", crc_result);
//            break;
//        }
//    }
//
//    if (!found_address) {
//        printf("end address value 0x%08X not found in the range 0x%08X to 0x%08X.\n", end_address_value, (uint32_t)start_address_flash, (uint32_t)end_address_flash);
//    }
//}


//// Verify application integrity with CRC
//uint32_t Verify_Application(uint32_t app_address) {
//    uint32_t calculated_crc;
//    uint32_t stored_crc;
//    uint8_t *pbuffer = (uint8_t *)app_address;
//    uint32_t byte_length = APP_SIZE - 8; // Exclude last 4 bytes (CRC) and 4 bytes before it
//    uint32_t word_length = byte_length / 4; // Adjust to 98,302 words
//    printf("Buffer start: 0x%08x, Byte Length: %lu, Word Length: %lu\n\r",
//           (uint32_t)pbuffer, byte_length, word_length);
//    __HAL_CRC_DR_RESET(&hcrc); // Reset to 0xFFFFFFFF
//    printf("CRC after reset: 0x%08x\n\r", hcrc.Instance->DR); // Verify reset
//    calculated_crc = HAL_CRC_Accumulate(&hcrc, (uint32_t *)pbuffer, word_length); // Accumulate over adjusted range
//    stored_crc = *(uint32_t *)(app_address + CRC_ADDRESS_OFFSET);
//    printf("Calculated CRC: 0x%08x, Stored CRC: 0x%08x\n\r", calculated_crc, stored_crc);
//    printf("Sample byte at 0x080FFFF4: 0x%02x\n\r", *((uint8_t *)(app_address + byte_length - 4))); // Check before CRC
//    // Software CRC for validation
//    uint32_t sw_crc = 0xFFFFFFFF;
//    for (uint32_t i = 0; i < byte_length; i++) {
//        sw_crc ^= pbuffer[i];
//        for (int j = 0; j < 8; j++) {
//            if (sw_crc & 1) sw_crc = (sw_crc >> 1) ^ 0xEDB88320;
//            else sw_crc >>= 1;
//        }
//    }
//    sw_crc ^= 0xFFFFFFFF;
//    printf("Software CRC (before final XOR): 0x%08x\n\r", sw_crc ^ 0xFFFFFFFF);
//    printf("Software CRC: 0x%08x\n\r", sw_crc);
//    return (calculated_crc == stored_crc) ? 1 : 0;
//}

//uint32_t CalculateCRC(void* start_address, size_t size) // uint32_t uint32_t
//{
//    CRC_HandleTypeDef hcrc;
//    hcrc.Instance = CRC;
//    hcrc.Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_ENABLE; // 0x04C11DB7
//    hcrc.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_ENABLE; // 0xFFFFFFFF
////    hcrc.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_NONE;
//    hcrc.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_BYTE; //
////    hcrc.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE;
//    hcrc.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_ENABLE; //
////    hcrc.InputDataFormat = CRC_INPUTDATA_FORMAT_WORDS;
//    hcrc.InputDataFormat = CRC_INPUTDATA_FORMAT_BYTES;
//
//    if (HAL_CRC_Init(&hcrc) != HAL_OK)
//    {
//        while (1);  // Handle error
//    }
//
////    return HAL_CRC_Calculate(&hcrc, (uint32_t*)start_address, size / 4);
//
//    // Compute the CRC
//    uint32_t crc_result = HAL_CRC_Calculate(&hcrc, (uint32_t*)(start_address), size / 4);
//
//        // Invert to obtain ZLIB CRC32
//        return crc_result ^ 0xFFFFFFFF;
//
////    __HAL_CRC_DR_RESET(&hcrc); // Reset CRC to initial value return
//
////    HAL_CRC_Accumulate(&hcrc, (uint32_t *)start_address, size / 4);
//}

/**
  * @brief  CPU L1-Cache enable.
  * @param  None
  * @retval None
  */
static void CPU_CACHE_Enable(void)
{
  /* Enable I-Cache */
  SCB_EnableICache();

  /* Enable D-Cache */
  SCB_EnableDCache();
}

/**
  * @brief  Configure the MPU attributes
  * @param  None
  * @retval None
  */
static void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct;

  /* Disable the MPU */
  HAL_MPU_Disable();

  /* Configure the MPU as Strongly ordered for not defined regions */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = 0x00;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* Configure the MPU attributes as Normal Non Cacheable for SRAM1 */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = 0x24000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_512KB;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER1;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
  MPU_InitStruct.SubRegionDisable = 0x00;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* Enable the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
