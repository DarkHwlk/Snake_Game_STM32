/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include<stdio.h>
#include <stdbool.h>
#include <string.h>
#include "fonts.h"
#include "ssd1306.h"
#include "test.h"
#include "music_icon.h"
#include "flash.h"
#define ADDRESS_DATA_STORAGE (0x800FC00)  //address page 64 

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c2;

TIM_HandleTypeDef htim3;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C2_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */

void randomPrey();
void predatorDraw();
void predatorClear();
void preyDraw();
void preyClear();
/* System */
/* Function read and write array uint16_t */
void Flash_Write_Array_Uint16(uint32_t address, uint16_t *arr, uint16_t lenght){
	HAL_FLASH_Unlock();
	uint16_t *pt = arr;  //moi lan lay 2 byte trong arr
	for(uint16_t i=0; i<(lenght+1); i++){
		/* float = 4 bytes == word */
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,address + 2*i,*pt);
		pt++;  //moi lan cong dia chi pt dich 16 bit
	}
	HAL_FLASH_Lock();
}
void Flash_Read_Array_Uint16(uint32_t address, uint16_t *arr,uint16_t lenght){
	uint16_t *pt = arr;
	for(uint16_t i=0; i<(lenght+1); i++){
		*pt = *(__IO uint16_t *)(address+2*i);
		pt++;
	}
}

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

int test = 0;
/* Setup */
static uint32_t timeBellBegin=0;
static uint32_t timeCur=0;
int areaEdge_x[2]={0,128};  //diem cuc bien theo truc x
int areaEdge_y[2]={16,64};  //diem cuc bien theo truc y
int level = 1;  //mac dinh
char str_level[2];
uint8_t sound_status = 0;  // 0:OFF|1:ON
uint8_t type_setup = 0;  // 0:level | 1:sound |

/* variable user */
uint16_t user_score = 0;
char str_user_score[5];
uint8_t userStart = 0;  //0-chua bat dau|1-batdau|2-che do setup
uint8_t gameStatus = 0;  //trang thai game: 1-hoat dong|0-k hoat dong
uint16_t top_user_score[5];

/* variable predator */
int predator_x = 25;
int predator_y = 30;
uint8_t predator_direct = 1; // 1:left|2:right|3:up|4:down
uint8_t predator_mount = 0;  //1: dong mom|0: mo mom
const uint8_t predator_size = 6;
/* end variable predator */

/* variable prey */
int prey_x = 32;
int prey_y = 52;
int prey_x_arr[20]={20,5,10,50,30,70,60,80,40,90,20,100,10,50,30,70,60,80,40,90};
int prey_y_arr[20]={20,30,50,40,25,40,50,20,35,45,20,35,50,40,25,40,50,20,35,45};	
/* end variable prey */

/* function general */
void showLevel(){
	SSD1306_GotoXY(72,20);
	sprintf(str_level, "%d", level);
	SSD1306_Puts(str_level,&Font_7x10,1);
	SSD1306_UpdateScreen();  //display
}

void showSoundStatus(){
	SSD1306_GotoXY(72,35);
	if(sound_status) SSD1306_Puts("ON ",&Font_7x10,1);
	else SSD1306_Puts("OFF",&Font_7x10,1);
	SSD1306_UpdateScreen();  //display
}

void initGame(){
	/* Setup GAME */
	user_score = 0;
	predator_x = 32;
	predator_y = 52;
	predator_direct = 1;
	predator_mount = 0;
	prey_x = 32;
	prey_y = 52;
	Flash_Read_Array_Uint16(ADDRESS_DATA_STORAGE, top_user_score, 5);  //get pre-score
	
	/* Introduce GAME */
	SSD1306_GotoXY(0,0);
	SSD1306_Puts("Predator",&Font_11x18,1);
	SSD1306_GotoXY(11,20);
	SSD1306_Puts("GAME",&Font_16x26,1);
	SSD1306_GotoXY(0,50);
	SSD1306_Puts("made by DxHwg",&Font_7x10,1);
	SSD1306_UpdateScreen();  //display
	/* Scroll */
	while(userStart==0){
		SSD1306_ScrollRight(0x00,0x0f);  //scroll entire screen
		HAL_Delay(1500);
		SSD1306_ScrollLeft(0x00,0x0f);  //scroll entire screen
		HAL_Delay(1500);
	}
	SSD1306_Stopscroll(); 
	SSD1306_Clear();
	
	/* Setup mode */
	SSD1306_GotoXY(0,0);
	SSD1306_Puts("SETUP MODE",&Font_11x18,1);
	SSD1306_GotoXY(0,20);
	SSD1306_Puts("Level: ",&Font_7x10,1);
	showLevel();
	SSD1306_GotoXY(0,35);
	SSD1306_Puts("Sound: ",&Font_7x10,1);
	showSoundStatus();
	/* Point level  */
	SSD1306_DrawFilledCircle(100,20, 2 ,1);
	SSD1306_UpdateScreen();  //display
	while(userStart==2){
		HAL_Delay(1);  //Phai co delay (Nhung k hieu tai sao)
	}  //Che do setup
	SSD1306_Clear();
	
	/* Init GAME */
	SSD1306_GotoXY(0,0);
	SSD1306_Puts("Score: ",&Font_11x18,1);
	SSD1306_GotoXY(72,0);
	sprintf(str_user_score, "%d", user_score);
	SSD1306_Puts(str_user_score,&Font_11x18,1);
	/* Ve duong bao */
	SSD1306_DrawLine(areaEdge_x[0], areaEdge_y[0], areaEdge_x[1],  areaEdge_y[0], 1);  //edge top
	SSD1306_DrawLine(areaEdge_x[0], areaEdge_y[0], areaEdge_x[0],  areaEdge_y[1], 1);  //edge left
	SSD1306_DrawLine(areaEdge_x[0], areaEdge_y[1], areaEdge_x[1],  areaEdge_y[1], 1);  //edge bottom
	SSD1306_DrawLine(areaEdge_x[1], areaEdge_y[0], areaEdge_x[1],  areaEdge_y[1], 1);	 //edge right
	SSD1306_UpdateScreen();  // display 
	/* Game bat dau */
	gameStatus = 1;
}

void renderScreen(){
	predatorClear();
	preyClear();
	/* Show score */
	SSD1306_GotoXY(70,0);
	sprintf(str_user_score, "%d", user_score);
	SSD1306_Puts(str_user_score,&Font_11x18,1);
	SSD1306_DrawLine(0,15, 128, 15, 1);
	/* draw predator */
	predatorDraw();
	/* draw prey */
	preyDraw();
	SSD1306_UpdateScreen();  //display
}

void sortUserScore(){
	if(user_score>top_user_score[0]){
		for(int i=1;i<5;i++){
			top_user_score[i]=top_user_score[i-1];
		}
		top_user_score[0]=user_score;
	}else if(user_score>top_user_score[1]){
		for(int i=2;i<5;i++){
			top_user_score[i]=top_user_score[i-1];
		}
		top_user_score[1]=user_score;
	}else if(user_score>top_user_score[2]){
		for(int i=3;i<5;i++){
			top_user_score[i]=top_user_score[i-1];
		}
		top_user_score[2]=user_score;
	}else if(user_score>top_user_score[3]){
		for(int i=4;i<5;i++){
			top_user_score[i]=top_user_score[i-1];
		}
		top_user_score[3]=user_score;
	}else if(user_score>top_user_score[4]){
		top_user_score[4]=user_score;
	}
}

void gameOver(){
	/* Hieu ung va thong bao game over */
	SSD1306_Clear();
	SSD1306_GotoXY(0,0);
	SSD1306_Puts("Score: ",&Font_11x18,1);
	SSD1306_GotoXY(72,0);
	sprintf(str_user_score, "%d", user_score);
	SSD1306_Puts(str_user_score,&Font_11x18,1);
	
	SSD1306_GotoXY(11,16);
	SSD1306_Puts("GAME",&Font_11x18,1);
	SSD1306_GotoXY(11,38);
	SSD1306_Puts("OVER",&Font_11x18,1);
	SSD1306_UpdateScreen();  //display
	
	sortUserScore();  //sap xep score user
	Flash_Write_Array_Uint16(ADDRESS_DATA_STORAGE,top_user_score,5);  //luu vao flash
	HAL_Delay(3000);
	SSD1306_Clear();
	
	/* Hien thi top user */
	SSD1306_GotoXY(0,0);
	SSD1306_Puts("Top score: ",&Font_11x18,1);
	for(int i=0; i<5;i++){
		SSD1306_GotoXY(0,18+10*i+1);
		sprintf(str_user_score, "%d", i+1);
		SSD1306_Puts(str_user_score,&Font_7x10,1);
		SSD1306_Puts("->",&Font_7x10,1);
		SSD1306_GotoXY(15,18+10*i+1);
		sprintf(str_user_score, "%d", (int)top_user_score[i]);
		SSD1306_Puts(str_user_score,&Font_7x10,1);
	}
	SSD1306_UpdateScreen();  //display
	HAL_Delay(5000);
	SSD1306_Clear();
	
	/* game ket thuc */
	gameStatus = 0;
	userStart = 0;
	user_score = 0;
	/* Khoi tao lai game */
	initGame();
}

/* Use bell: 
	bellTingTing();
*/
void bellTingTing(){
	timeCur= HAL_GetTick();
	timeBellBegin = timeCur; 
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0,GPIO_PIN_RESET);  //turn on
}
void bellTingTingWait(){
	timeCur= HAL_GetTick();
	if((timeCur-timeBellBegin)>=300) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0,GPIO_PIN_SET);  //turn off
}

/* Change period TIM3 */
static void changePeriodTIM3(int t)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 7199;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = t;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
}

/* Setup Plus */
void setupPlus(){  //button left
	if(type_setup==0) {
		/* level */
		if(level==5) {
				level=1;
				showLevel();
			}
		else {
			level++;
			showLevel();
		}
		changePeriodTIM3(2499-level*400);
	}
	else if(type_setup==1) {
		/* sound */
		sound_status = 1-sound_status;
		showSoundStatus();
	}
}
void setupReduce(){  //button right
	if(type_setup==0) {
		/* level */
		if(level==0) {
				level=5;
				showLevel();
			}
		else {
			level--;
			showLevel();
		}
		changePeriodTIM3(2499-level*400);
	}
	else if(type_setup==1) {
		/* sound */
		sound_status = 1-sound_status;
		showSoundStatus();
	}
}
void changeSetupType(){  //button down
	if(type_setup==0) {
		type_setup=1;
		/* clear Point level  */
		SSD1306_DrawFilledCircle(100,20, 2 ,0);
		/* Point sound */
		SSD1306_DrawFilledCircle(100,35, 2 ,1);
	}
	else if(type_setup==1) {
		type_setup=0;
		/* clear Point sound */
		SSD1306_DrawFilledCircle(100,35, 2 ,0);
		/* Point level  */
		SSD1306_DrawFilledCircle(100,20, 2 ,1);
	}
	SSD1306_UpdateScreen();  //display
}

/* end function general */

/* function prey */
void preyDraw(){
	/* draw prey */
	SSD1306_DrawFilledCircle(prey_x,prey_y, 1 ,1);
}

void preyClear(){
	/* draw prey */
	SSD1306_DrawFilledCircle(prey_x,prey_y, 1 ,0);
}

void randomPrey(){
	preyClear();  //clear truoc khi random
	prey_x = prey_x_arr[user_score];
	prey_y = prey_y_arr[user_score];
}

/* end function prey */

/* function predator */
void  predatorDraw(){
	if(predator_direct == 1){
		/* Left */
		/* Phan than */
		SSD1306_DrawFilledRectangle(predator_x-predator_size/2, predator_y-predator_size/2, predator_size, predator_size, 1);  
		/* Phan mom */
		SSD1306_DrawLine(predator_x-predator_size/2, predator_y-1, predator_x,  predator_y-1, predator_mount);
		SSD1306_DrawLine(predator_x-predator_size/2, predator_y, predator_x,  predator_y, predator_mount);
		SSD1306_DrawLine(predator_x-predator_size/2, predator_y+1, predator_x,  predator_y+1, predator_mount);
		SSD1306_DrawLine(predator_x-predator_size/2, predator_y+2, predator_x,  predator_y+2, predator_mount);
	}else if(predator_direct == 2){
		/* Right */
		/* Phan than */
		SSD1306_DrawFilledRectangle(predator_x-predator_size/2, predator_y-predator_size/2, predator_size, predator_size, 1);  
		/* Phan mom */
		SSD1306_DrawLine(predator_x, predator_y-1, predator_x+predator_size/2,  predator_y-1, predator_mount);
		SSD1306_DrawLine(predator_x, predator_y, predator_x+predator_size/2,  predator_y, predator_mount);
		SSD1306_DrawLine(predator_x, predator_y+1, predator_x+predator_size/2,  predator_y+1, predator_mount);
		SSD1306_DrawLine(predator_x, predator_y+2, predator_x+predator_size/2,  predator_y+2, predator_mount);
	}else if(predator_direct == 3){
		/* Up */
		/* Phan than */
		SSD1306_DrawFilledRectangle(predator_x-predator_size/2, predator_y-predator_size/2, predator_size, predator_size, 1);  
		/* Phan mom */
		SSD1306_DrawLine(predator_x-1, predator_y, predator_x-1,  predator_y-predator_size/2, predator_mount);
		SSD1306_DrawLine(predator_x, predator_y, predator_x,  predator_y-predator_size/2, predator_mount);
		SSD1306_DrawLine(predator_x+1, predator_y, predator_x+1,  predator_y-predator_size/2, predator_mount);
		SSD1306_DrawLine(predator_x+2, predator_y, predator_x+2,  predator_y-predator_size/2, predator_mount);
	}else if(predator_direct == 4){
		/* Down */
		/* Phan than */
		SSD1306_DrawFilledRectangle(predator_x-predator_size/2, predator_y-predator_size/2, predator_size, predator_size, 1);   
		/* Phan mom */
		SSD1306_DrawLine(predator_x-1, predator_y, predator_x-1,  predator_y+predator_size/2, predator_mount);
		SSD1306_DrawLine(predator_x, predator_y, predator_x,  predator_y+predator_size/2, predator_mount);
		SSD1306_DrawLine(predator_x+1, predator_y, predator_x+1,  predator_y+predator_size/2, predator_mount);
		SSD1306_DrawLine(predator_x+2, predator_y, predator_x+2,  predator_y+predator_size/2, predator_mount);
	}
}

void  predatorClear(){
	SSD1306_DrawFilledRectangle(predator_x-predator_size/2, predator_y-predator_size/2, predator_size, predator_size, 0);   
}

void  predatorMove(){
	predatorClear();  //truoc khi di chuyen phai xoa di da
	if(predator_direct == 1){
		/* Left */
		predator_x--;
	}else if(predator_direct == 2){
		/* Right */
		predator_x++;
	}else if(predator_direct == 3){
		/* Right */
		predator_y--;
	}else if(predator_direct == 4){
		/* Down */
		predator_y++;
	}
}

void toggleMount(){
	predator_mount = 1-predator_mount;
}

/* Kiem tra vuot bien */
bool checkAcrossEdge(){
	if((predator_x==areaEdge_x[0])||(predator_x==areaEdge_x[1])||(predator_y==areaEdge_y[0])||(predator_y==areaEdge_y[1])){
		return true;
	}
	return false;
}

/* Check xem predator da eat prey chua? */
bool checkEating(){
	int delta_x = predator_x-prey_x;
	int delta_y = predator_y-prey_y;
	if(predator_direct == 1){
		/* Left */
		if(delta_x<=2){
			if((delta_y>=-2)&&(delta_y<=2)){
				return true;
			}else return false;
		}else return false;
	}else if(predator_direct == 2){
		/* Right */
		if(delta_x>=-2){
			if((delta_y<=2)&&(delta_y>=-2)){
				return true;
			}else return false;
		}else return false;
	}else if(predator_direct == 3){
		/* Up */
		if(delta_y<=2){
			if((delta_x>=-2)&&(delta_x<=2)){
				return true;
			}else return false;
		}else return false;
	}else if(predator_direct == 4){
		/* Down */
		if(delta_y>=-2){
			if((delta_x>=-2)&&(delta_x<=2)){
				return true;
			}else return false;
		}else return false;
	}else return false;
}
/* end function predator */

/* Ngat ngoai: ok */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  UNUSED(GPIO_Pin);
	if(userStart==0) {  
		userStart=2;  //che do setup
	}else if(userStart==2) {  //setup xong
		if(GPIO_Pin == GPIO_PIN_2){
			/* Left - Plus */
			setupPlus();
		}else if(GPIO_Pin == GPIO_PIN_3){
			/* Right - Reduce */
			setupReduce();
		}else if(GPIO_Pin == GPIO_PIN_4){
			/* Up - OK */
			userStart=1; //cho game bat dau
		}else if(GPIO_Pin == GPIO_PIN_5){
			/* Down - change type setup */
			changeSetupType();
		}
	}else if(userStart==1) {
		if(GPIO_Pin == GPIO_PIN_2){
			/* Left */
			predator_direct = 1; 
		}else if(GPIO_Pin == GPIO_PIN_3){
			/* Right */
			predator_direct = 2; 
		}else if(GPIO_Pin == GPIO_PIN_4){
			/* Up */
			predator_direct = 3; 
		}else if(GPIO_Pin == GPIO_PIN_5){
			/* Down */
			predator_direct = 4; 
		}
	}
}
/* end ngat ngoai */

/* Ngat timer */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	UNUSED(htim);
	if(htim->Instance == htim3.Instance){
		/* Neu da bat dau */
		if(gameStatus){
			toggleMount();
			predatorMove();
			/* Kiem tra xem da vuot qua cuc bien chua? */
			if(checkAcrossEdge()){
				/* Neu da vuot qua bien */
				gameOver();
			}else{
				/* Kiem tra xem prey da bi eat chua? */
				if(checkEating()){
					/* Neu da bi eat */
					user_score++;  //tang score
					bellTingTing();  //ting ting bell
					randomPrey();
				}
				renderScreen();
			}
		}
	}
}
/* end ngat timer */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C2_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
	
	/* Start IT */
	HAL_TIM_Base_Start_IT(&htim3);
	/* Init LCD */
	SSD1306_Init();
	initGame();
	renderScreen();
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		
		/* Setup timeout bell */
		bellTingTingWait();
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 400000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 7199;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 2499;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);

  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PA2 PA3 PA4 PA5 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI2_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);

  HAL_NVIC_SetPriority(EXTI3_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);

  HAL_NVIC_SetPriority(EXTI4_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
