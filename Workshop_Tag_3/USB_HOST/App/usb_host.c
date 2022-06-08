/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file            : usb_host.c
  * @version         : v2.0_Cube
  * @brief           : This file implements the USB Host
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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

#include "usb_host.h"
#include "usbh_core.h"
#include "usbh_msc.h"

/* USER CODE BEGIN Includes */
#include "fatfs.h"
#include "usb_host.h"
#include "main.h"

/* USER CODE END Includes */

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/


 I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart2;

uint8_t HT221_read= 0xBF;
uint8_t HT221_write= 0xBE;
uint8_t ctrl_reg1= 0x20;
uint8_t tempout_lo= 0x2A;
uint8_t tempout_hi= 0x2B;
uint8_t whoami= 0x0F;
uint8_t data= 0xFF;
uint8_t sensor_on[2] = { 0x20 , 0x81 };
uint8_t temp_lo;
uint8_t temp_hi;
uint16_t temp;

char msg[100];



/* USER CODE END PV */

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_I2C1_Init(void);
void MX_USB_HOST_Process(void);


/* USER CODE END PFP */

/* USB Host core handle declaration */
USBH_HandleTypeDef hUsbHostFS;
ApplicationTypeDef Appli_state = APPLICATION_IDLE;

/*
 * -- Insert your variables declaration here --
 */
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*
 * user callback declaration
 */
static void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id);

/*
 * -- Insert your external function declaration here --
 */
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**
  * Init USB host library, add supported class and start the library
  * @retval None
  */
void MX_USB_HOST_Init(void)
{
  /* USER CODE BEGIN USB_HOST_Init_PreTreatment */

  /* USER CODE END USB_HOST_Init_PreTreatment */

  /* Init host Library, add supported class and start the library. */
  if (USBH_Init(&hUsbHostFS, USBH_UserProcess, HOST_FS) != USBH_OK)
  {
    Error_Handler();
  }
  if (USBH_RegisterClass(&hUsbHostFS, USBH_MSC_CLASS) != USBH_OK)
  {
    Error_Handler();
  }
  if (USBH_Start(&hUsbHostFS) != USBH_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USB_HOST_Init_PostTreatment */

  /* USER CODE END USB_HOST_Init_PostTreatment */
}

/*
 * Background task
 */
void MX_USB_HOST_Process(void)
{
  /* USB Host Background task */
  USBH_Process(&hUsbHostFS);
}
/*
 * user callback definition
 */
static void USBH_UserProcess  (USBH_HandleTypeDef *phost, uint8_t id)
{
  /* USER CODE BEGIN CALL_BACK_1 */
  switch(id)
  {
  case HOST_USER_SELECT_CONFIGURATION:
  break;

  case HOST_USER_DISCONNECTION:
  Appli_state = APPLICATION_DISCONNECT;
  break;

  case HOST_USER_CLASS_ACTIVE:
  Appli_state = APPLICATION_READY;


  Mount_USB();
  Check_USB_Details();
  Scan_USB("/");
  Create_File("/TempertureToday.txt");
  Write_File("/TempertureToday.txt", "It was warm today! YAY!");

  for (int i = 0; i < 6; i++)
  {

  HAL_I2C_Master_Transmit(&hi2c1, HT221_write, &sensor_on, 2, 1000);
 	  HAL_I2C_Master_Receive(&hi2c1, HT221_read, &data, 1, 1000);
 	  HAL_Delay(1000);
 	  //sprintf(msg, "%d", data);
 	  HAL_UART_Transmit(&huart2, &msg, strlen(msg), 1000);

 	 HAL_I2C_Master_Transmit(&hi2c1, HT221_write, &sensor_on, 2, 1000);
 		  HAL_I2C_Master_Receive(&hi2c1, HT221_read, &data, 1, 1000);
 		  HAL_Delay(1000);
 		  //sprintf(msg, "%d", data);
 		  HAL_UART_Transmit(&huart2, &msg, strlen(msg), 1000);



 		 HAL_I2C_Master_Transmit(&hi2c1, HT221_write, &tempout_lo, 1, 1000);
 		 	  HAL_I2C_Master_Receive(&hi2c1, HT221_read, &temp_lo, 1, 1000);

 		 	  HAL_I2C_Master_Transmit(&hi2c1, HT221_write, &tempout_hi, 1, 1000);
 		 	  HAL_I2C_Master_Receive(&hi2c1, HT221_read, &temp_hi, 1, 1000);

 		 	  HAL_Delay(1000);
 		 	  //sprintf(msg, "%d \n\r", temp_hi);
 		 	  HAL_UART_Transmit(&huart2, &msg, strlen(msg), 1000);
 		 	  //sprintf(msg, "%d \n\r", temp_lo);
 		 	  HAL_UART_Transmit(&huart2, &msg, strlen(msg), 1000);

 		 	  temp = (temp_hi*256+temp_lo);
 		 	  temp = (20+temp/100);
 		 	  sprintf(msg, "The temperture is %d \n\r", temp);
 		 	  HAL_UART_Transmit(&huart2, &msg, strlen(msg), 1000);
 		 	 Update_File("/TempertureToday.txt", msg);

  }

  Unmount_USB();

  break;

  case HOST_USER_CONNECTION:
  Appli_state = APPLICATION_START;
  break;

  default:
  break;
  }
  /* USER CODE END CALL_BACK_1 */
}

/**
  * @}
  */

/**
  * @}
  */

