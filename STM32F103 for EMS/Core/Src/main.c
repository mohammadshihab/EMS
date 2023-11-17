/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ILI9341_STM32_Driver.h"
#include "ILI9341_GFX.h"
#include <string.h>
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
ADC_HandleTypeDef hadc1;

I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;
DMA_HandleTypeDef hdma_spi1_tx;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

//Variable for external button interrupt
uint32_t button1 = 0;
uint32_t button2 = 0;
uint32_t button3 = 0; // Variables for button press count

uint32_t button1CurrMillis=0;
uint32_t button1PrevMillis=0;
uint32_t button2CurrMillis=0;
uint32_t button2PrevMillis=0;
uint32_t button3CurrMillis=0;
uint32_t button3PrevMillis=0; // Variables for preventing button de-bouncing

// For memory writing

#define EEPROM_ADDR 0xA0

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_SPI1_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */



//Variable for UART received data
char UART1_rxBuffer[7] = {"\n"};
char UART1_rxBuffer_sorted[6]={"\n"};  // Use this if sorting or ordering required;for this call sorting_rx_in_order(UART1_rxBuffer, UART1_rxBuffer_sorted);
                                       //////
 int IntRecData = 0; // After receiving the char data, it should be converted into integer
 int IntRecDataTemp = 0;
double IntRecDataTempDouble=0;

 int intSenData = 0;  // Integer data  that should be sent to web page through esp32 UART communication
char UART1_txBuffer[6] = {0};    // Before sending intSenData, it should be converted into STRING

//Variable for TIMERS
uint16_t timerInSec1=0;
uint16_t timerInSec2=0;
uint16_t timerInSec3=0;

//Variables for analog input
uint16_t aReadValue1;
uint16_t aReadValue2;
uint16_t aReadValue3;
ADC_ChannelConfTypeDef sConfigPrivate = {0};


// making all double into float

double solar_pow_in_kw;   // Read from analog1
double load_pow_req_in_kw;   // Read from analog2
double grid_supply_volt;   // Read from analog3




//Placing those variable declaration as required
double total_balance=0;

double timer_sol_to_grid=0;
int timer_sol_to_grid_temp=0;
double timer_grid_to_load=0;
int timer_grid_to_load_temp=0;

double timer_global=0;
int timer_global_temp=0;


double motor_switch=0; // should be declared initially, will be 1 for the interrupt of motor switch button, in case of 1, the load power will be around 2.2kw and the calculation will be so on.


//Place those variable declaration as required
double pow_sol_to_grid;
double unit_sol_to_grid;
double unit_sol_to_grid_total=0;

double pow_grid_to_load;
double unit_grid_to_load;
double unit_grid_to_load_total=0;




double solar_power_price = 5.5;
double purchase_price = 8 ;



//int last_flash_data;
// end of all float

char grid_availability[5];
char wifi_status[12];
int wifi_stat=0;
char motor_status[5];
char subgrid_solar_supply_status[10];
int subgrid_solar_supply=0;
char op_comment1[20];
char op_comment2[20];

int write_flash_second_count=0; // To count time and reset in a fixed interval
int write_flash_second_count_temp=0;
char write_flash_status[15]; // for write status

int reset_flash_temp=0;
char reset_flash_status[15];  // For reset status

int tempTimeCount=0;  //A normal count of every 5 seconds

int wifi_time_sec=0;
int wifi_time_min=0;
int wifi_time_hour=0;



int subpage_temp1=0;
int subpage_temp2=0;
int subpage_temp3=0;
int subpage_temp4=0;
int subpage_temp5=0;
int subpage_temp6=0;

uint16_t XX=0;
uint16_t YY=0;

int natemp=0;   // variable that will be used fo noise interruption prevention due to sudden AC load connection
//////



//Analog input configuration
void analogReadFnc(){
	sConfigPrivate.Rank = ADC_REGULAR_RANK_1;
	sConfigPrivate.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;

		  sConfigPrivate.Channel = ADC_CHANNEL_0;
	  	  HAL_ADC_ConfigChannel(&hadc1, &sConfigPrivate);
		  HAL_ADC_Start(&hadc1);
		  HAL_ADC_PollForConversion(&hadc1,1000);
		  aReadValue1 = HAL_ADC_GetValue(&hadc1);
		  HAL_ADC_Stop(&hadc1);

		  sConfigPrivate.Channel = ADC_CHANNEL_1;
		  HAL_ADC_ConfigChannel(&hadc1, &sConfigPrivate);
		  HAL_ADC_Start(&hadc1);
		  HAL_ADC_PollForConversion(&hadc1,1000);
		  aReadValue2 = HAL_ADC_GetValue(&hadc1);
		  HAL_ADC_Stop(&hadc1);

		  sConfigPrivate.Channel = ADC_CHANNEL_2;
		  HAL_ADC_ConfigChannel(&hadc1, &sConfigPrivate);
		  HAL_ADC_Start(&hadc1);
		  HAL_ADC_PollForConversion(&hadc1,1000);
		  aReadValue3 = HAL_ADC_GetValue(&hadc1);


		  solar_pow_in_kw= (double)aReadValue1/1338; // Assuming solar pow as 3kw, aRead1 is div by 1338;
		  load_pow_req_in_kw=(double) aReadValue2/1820; // Assuming load pow req as 2.25kw , aRead2 is div by 1820
		  grid_supply_volt=(double)aReadValue3*100/1735;   // Assuming grid supply is 230v, aRead3 is div by 17.35

}



//-----------------------------------------------------------------------------------//
// All the CUSTOM Function can be created here
//----------------------------------------------------------------------------------//










//------------------------------------------------------------------







int raw_diag_temp;

// All the root level variables can be printed from here
void raw_diagnosis(void){
	if (raw_diag_temp==0){ //Showing the text for only one time
		ILI9341_FillScreen(WHITE);
		//For printing the UART receiving data
			  ILI9341_DrawText("Rx/Rx-S:", FONT4, 5, 5, RED, WHITE);                          //Line-1
			  ILI9341_DrawText("Tx-: ", FONT4, 5, 30, BLUE, WHITE);

		//For Timers
			 // ILI9341_DrawText("timer1 in sec: ", FONT4, 5, 30, GREEN, WHITE);                  //Line-2  // There might be some issues with timer1
			  ILI9341_DrawText("timer2 in sec: ", FONT4, 5, 55, RED, WHITE);                    //Line-3
			  ILI9341_DrawText("timer3 in sec: ", FONT4, 5, 80, RED, WHITE);                    //Line-4
		//For analog input values
			  ILI9341_DrawText("Solar supply (w): ", FONT4, 5, 105, BLACK, WHITE);                  //Line-5
			  ILI9341_DrawText("Load req (w) : ", FONT4, 5, 130, BLACK, WHITE);                  //Line-6
			  ILI9341_DrawText("Grid supply(v): ", FONT4, 5, 155, BLACK, WHITE);                 //Line-7
		//For button interrupt
			  ILI9341_DrawText("Button1 pressed- : ", FONT4, 5, 180, RED, WHITE);               //Line-8
			  ILI9341_DrawText("Button2 pressed- : ", FONT4, 5, 205, RED, WHITE);               //Line-9
			  ILI9341_DrawText("Button3 pressed- : ", FONT4, 5, 230, RED, WHITE);                //Line-10
			  raw_diag_temp=1;
	}

	  //For printing the UART receiving data
	  ILI9341_DrawText(UART1_rxBuffer, FONT4, 85, 5, RED, WHITE);
	  HAL_Delay(200);

	  //sorting_rx_in_order(UART1_rxBuffer, UART1_rxBuffer_sorted); // For sorting the received data
	  ILI9341_DrawText(UART1_rxBuffer_sorted, FONT4, 160, 5, RED, WHITE); // In same line as UART1_rxBuffer

	  IntRecData = atoi(UART1_rxBuffer_sorted); // Converting the received char data into integer

	  HAL_Delay(1000);
	  intSenData = IntRecData-4; // For testing purpose of operation
	  sprintf(UART1_txBuffer, "%05d", intSenData); // Sending data is converted into Char now and stored in UART1_txBuffer
	  UART1_txBuffer[5]='s';   // Putting 's' as last char
	  UART1_txBuffer[6]='\0';   // Null terminator in last position

	  ILI9341_DrawText(UART1_txBuffer, FONT4, 85, 30, BLUE, WHITE);
	  //For transmitting the sending data
	  HAL_UART_Transmit(&huart1,UART1_txBuffer, 6, 500); // The sending data is being transmitted here



//For Timers
 	  //ILI9341_DrawText("  ", FONT4, 170, 55, RED, WHITE);                         There might be some issues with timer1
 	 // ILI9341_DrawNumber(timerInSec1, FONT4, 170, 55, RED, WHITE);

	  ILI9341_DrawText("  ", FONT4, 125, 55, RED, WHITE);
	  ILI9341_DrawNumber(timerInSec2, FONT4, 125, 55, DARKGREY, WHITE);

	  ILI9341_DrawText("      ", FONT4, 125, 80, RED, WHITE);
	  ILI9341_DrawNumber(timerInSec3, FONT4, 125, 80, DARKGREY, WHITE);

//For analog input values
	  analogReadFnc();
	  ILI9341_DrawText("      ", FONT4, 155, 105, BLACK, WHITE);
	  ILI9341_DrawNumber(solar_pow_in_kw, FONT4, 155, 105, BLACK, WHITE);  // Replaced by aRead1

	  ILI9341_DrawText("      ", FONT4, 145, 130, BLACK, WHITE);
	  ILI9341_DrawNumber(load_pow_req_in_kw, FONT4, 145, 130, BLACK, WHITE);  // Replaced by aRead2

	  ILI9341_DrawText("      ", FONT4, 140, 155, BLACK, WHITE);
	  ILI9341_DrawNumber(grid_supply_volt, FONT4, 140, 155, BLACK, WHITE);    // Replaced by aRead3


//For button interrupt
	  ILI9341_DrawText("      ", FONT4, 145, 180, BLACK, WHITE);
	  ILI9341_DrawNumber(button1, FONT4, 145, 180, BLACK, WHITE);

	  ILI9341_DrawText("      ", FONT4, 145, 205, BLACK, WHITE);
	  ILI9341_DrawNumber(button2, FONT4, 145, 205, BLACK, WHITE);

	  ILI9341_DrawText("      ", FONT4, 145, 230, BLACK, WHITE);
	  ILI9341_DrawNumber(button3, FONT4, 145, 230, BLACK, WHITE);


}



//-------------------------------------------------------------------------------//
// This is a random function that correct the order of RX string FINDING 's'
// Which is for sorting RX data in order
void sorting_rx_in_order(char sttr[6], char sttrr[6]){  // After converting sttr[8], it will be stored into sttrr[8]
    int pos;
    int beg=0;
    int len = strlen(sttr);
    if(len<=1){
    	sttrr[6]="00000s\n";
    }
    else{
        for (int jk = 0; jk < len; jk++) {
            if (sttr[jk] == 's') {
                pos=jk;
                beg=0;
                //break;
            }
        }
        for(int kl=pos+1; kl<=len; kl++){
            sttrr[beg]=sttr[kl];
            beg++;
        }
        beg=beg-1;
        for(int lm=0; lm<=pos; lm++){
            sttrr[beg]=sttr[lm];
            beg++;
        }
        sttrr[beg] = '\0';
    }
}


//-----Function to count wi-fi network update time------//
int timerInSec3temp=0;

void wifi_time_update(void){
	if(timerInSec3!=timerInSec3temp){
		timerInSec3temp=timerInSec3;

		if(wifi_stat!=0){
			wifi_time_sec++;
			if(wifi_time_sec>=60){
				wifi_time_min++;
				wifi_time_sec=0;
				if(wifi_time_min>=60){
					wifi_time_hour++;
					wifi_time_min=0;
				}
			}
		}
		  if(wifi_stat!=1){
			  wifi_time_sec=0;
			  wifi_time_min=0;
			  wifi_time_hour=0;
		  }

	}
}

// Function to write all Operational comment2//
void operational_comment2(void){
	if ((motor_switch==1)&&(solar_pow_in_kw>=load_pow_req_in_kw)){
		strcpy(op_comment2,"Running by Solar   ");
	}
	else if((motor_switch==1)&&(solar_pow_in_kw<load_pow_req_in_kw)){
		if(subgrid_solar_supply==1){
			strcpy(op_comment2,"Running by s-g solar");
		}
		else{
			strcpy(op_comment2,"Running by Grid    ");
		}
	}
	else if((motor_switch==0)&&(solar_pow_in_kw>=load_pow_req_in_kw)){
		if((grid_supply_volt>=220)){
			strcpy(op_comment2,"Power selling back ");
		}
		else if((grid_supply_volt<220)){
			strcpy(op_comment2,"Grid unavailable   ");
		}
	}
	else if((motor_switch==0)&&(solar_pow_in_kw<=load_pow_req_in_kw)){  // This condition is unnecessary
		strcpy(op_comment2,"              ");
	}
}

// Function to WRITE AND READ DATA IN FLASH MEMORY

//-------------------AT24C32 EEPROM--------------//

void EEPROM_Write(double num, uint16_t MemAddress){

	uint8_t convertedValue[30];
	sprintf(convertedValue, "%.3f", num);
	HAL_I2C_Mem_Write(&hi2c1, EEPROM_ADDR, MemAddress, 2, convertedValue, sizeof(convertedValue), 2000);
	HAL_Delay(30);

}


double EEPROM_Read(uint16_t MemAddress){
	uint8_t readStr[30];
	HAL_I2C_Mem_Read(&hi2c1, EEPROM_ADDR, MemAddress, 2, readStr, sizeof(readStr), 2000);
	double num;
	sscanf((char*)readStr, "%lf", &num);
	return num;
}


void EEPROM_Reset_All(void){
	unit_sol_to_grid_total=0;
	unit_grid_to_load_total=0;
	total_balance=0;

	IntRecDataTemp = 0; // The last received char data through serial communication
	IntRecData=0;
	IntRecDataTempDouble=0;
}


void EEPROM_Write_All(void){
	EEPROM_Write(unit_sol_to_grid_total, 0);
	HAL_Delay(70);
	EEPROM_Write(unit_grid_to_load_total, 64);
	HAL_Delay(70);
	EEPROM_Write(total_balance, 96);
	HAL_Delay(70);
	EEPROM_Write(IntRecDataTemp, 192);
	HAL_Delay(70);
}

void EEPROM_Read_All(void){
	unit_sol_to_grid_total=EEPROM_Read(0);
	HAL_Delay(100);
	unit_grid_to_load_total=EEPROM_Read(64);
	HAL_Delay(100);
	total_balance=EEPROM_Read(96);
	HAL_Delay(100);
	IntRecDataTemp = EEPROM_Read(192);
	HAL_Delay(100);
}



//-----------------------------------//

int main_page_cursor_button=1; // Select the position of cursor in main page
int page_select_button=0; // For going to second page from main page under a specific cursor/menu
int on_off_button=0; // for turning ON or OFF anything under any page, it will turn GPIO_Output ON or OFF

int temp1=0;
int iii=0;

// All the operational calculation is done here
void operational_calculation(void){
	//For wifi time update
	wifi_time_update();
	// for analog readings
	analogReadFnc();
	// for receiving data or recharged amount
	for (iii=0; iii<3; iii++){  // to prevent any garbage value
		IntRecData = atoi(UART1_rxBuffer_sorted); // Converting the received char data into integer
		HAL_Delay(30);
	}
	if(IntRecData!=IntRecDataTemp){  // To prevent adding same received data again
		total_balance = total_balance+ IntRecData; // adding the recharged amount
		if(IntRecData!=0){
			IntRecDataTemp=IntRecData;
			IntRecDataTempDouble = (double)IntRecDataTemp;
		}
	}
	//Subgrid signal for power supply from solar
	if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15) == 1){  //Subgrid signal for power supply from solar is YES or 1
		strcpy(subgrid_solar_supply_status,"fSolar");
		subgrid_solar_supply = 1;
	}
	else if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15) == 0){
		strcpy(subgrid_solar_supply_status,"not fSolar");
		subgrid_solar_supply = 0;
	}
	//Is the wi-fi available or not
	if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12) == 1){
		strcpy(wifi_status,"Yes");
		wifi_stat=1;
	}
	else if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12) == 0){
		strcpy(wifi_status,"No ");
		wifi_stat=0;
	}
	// IS THE MOTOR ON OR OFF?
	if(motor_switch==1){
		strcpy(motor_status,"On");
		load_pow_req_in_kw=load_pow_req_in_kw;
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, 1); //To turn motor ON
	}
	else if(motor_switch==0){
		strcpy(motor_status,"Off");
		load_pow_req_in_kw=0;
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, 0); //To turn motor OFF
	}

	// CALCULATION
	if (timer_global_temp==1){
		if (solar_pow_in_kw > load_pow_req_in_kw){    // SOLAR POWER IS MORE THAN SUFFICIENT
			pow_sol_to_grid = solar_pow_in_kw - load_pow_req_in_kw; //excess pow to grid
				if (grid_supply_volt>=220){ // if the grid supply is available
					unit_sol_to_grid =(double) pow_sol_to_grid*0.0013889;  // (5/3600=0.0013889)
					unit_sol_to_grid_total = (double)unit_sol_to_grid_total + (double)unit_sol_to_grid;  // can be shown in ELECTRICAL SUMMERY (?)
				    total_balance = (double)total_balance + (double)(unit_sol_to_grid*solar_power_price);
				}
				else if(grid_supply_volt<220){ // in case of grid unavailable , extra power will not be added with POWER SOLD
					// show message "THE GRID IS UNAVAILABLE"
				}
		}

		if (solar_pow_in_kw <= load_pow_req_in_kw){     // SOLAR POWER IS INSUFFICIENT
			pow_grid_to_load = load_pow_req_in_kw - solar_pow_in_kw; // shortage power to load
				if (grid_supply_volt>=220){   // if the grid supply is available
					unit_grid_to_load = (double)pow_grid_to_load * 0.0013889;
				    unit_grid_to_load_total = (double)unit_grid_to_load_total + (double)unit_grid_to_load;  // can be shown in ELECTRICAL SUMMERY (?)
				    if(subgrid_solar_supply==0){
				    	total_balance = (double)total_balance - (double)(unit_grid_to_load * purchase_price);
				    }
				    else if(subgrid_solar_supply==1){
				    	total_balance = (double)total_balance - (double)(unit_grid_to_load * solar_power_price);
				    }
					if(total_balance<=0){ // in case of insufficient balance, the motor should be turned ON
						motor_switch=0;
						//Show message "RECHARGE PLEASE"
					}
				}
				else if(grid_supply_volt<220){   // in case of grid unavailable while grid power is required to run the LOAD
					// show message "THE GRID IS UNAVAILABLE"
					motor_switch=0;
				}
		}
		timer_global_temp=0;
		tempTimeCount=tempTimeCount+5;
	}

	if(total_balance<=0){
		strcpy(op_comment1,"Recharge Please   ");
	}
	else if(total_balance>=0){
		strcpy(op_comment1,"Balance Available");
	}

	if(grid_supply_volt>=220){
		strcpy(grid_availability,"Yes");
	}
	else if(grid_supply_volt<220){
		strcpy(grid_availability,"No  ");
	}



	operational_comment2();  // To show all the operational comment2


	if(page_select_button!=0){
		HAL_Delay(500);  // Delay is applicable only when the page is not on main menu page
	}

	//now the updated total balance should be sent to esp32
	intSenData = total_balance; // This data will be sent to esp32 and shown in webpage as REMAINING BALANCE
	sprintf(UART1_txBuffer, "%05d", intSenData); // Sending data is converted into Char now and stored in UART1_txBuffer
	UART1_txBuffer[5]='s';   // Putting 's' as last char
	UART1_txBuffer[6]='\0';   // Null terminator in last position

	//For transmitting the sending data
	HAL_UART_Transmit(&huart1,UART1_txBuffer, 6, 500); // The sending data is being transmitted here

	strcpy(reset_flash_status,"reset flash?  ");
	if(reset_flash_temp==1){
		EEPROM_Reset_All();
		strcpy(reset_flash_status,"flash reseted");
		reset_flash_temp=0;
	}
	strcpy(write_flash_status,"!written");
	if(write_flash_second_count_temp==1){
		EEPROM_Write_All();
		write_flash_second_count_temp=0;
		strcpy(write_flash_status,"written");
	}

	HAL_Delay(50);

	// for preventing noise interruption // the last additional modificaiton
	ILI9341_DrawText("test", FONT4, 60, 300, BLACK, WHITE);
	if (natemp==0){
		if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12) != 0){ // Will measure 3 times (200ms interval) if the controlling switch for 3 button is turned on
			HAL_Delay(200);
			if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12) != 0){
				HAL_Delay(200);
			    if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12) != 0){
			    	natemp=1;
			    	ILI9341_DrawText("ON", FONT4, 5, 300, BLACK, WHITE);
			    }
			}
		}
	}

	if (natemp==1){
		if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12) != 1){
			natemp=0;
			ILI9341_DrawText("OFF", FONT4, 5, 300, BLACK, WHITE);
		}
	  }

}




//------------------End of calculation. NOW,Display functions--------------------//




void sub_page_1(void){
	if(subpage_temp1==0){
		ILI9341_FillScreen(WHITE);
		ILI9341_DrawText(" 1.Power source status ", FONT4, 5, 5, RED, WHITE);
		ILI9341_DrawText("solar power(kw):", FONT4, 5, 35, RED, WHITE);
		ILI9341_DrawText("Grid availability:", FONT4, 5, 65, RED, WHITE);
		subpage_temp1=1;
	}
	ILI9341_DrawNumber(solar_pow_in_kw, FONT4, 145, 35, RED, WHITE);
	ILI9341_DrawText(grid_availability, FONT4, 145, 65, RED, WHITE);
}


void sub_page_2(void){
	if(subpage_temp2==0){
		ILI9341_FillScreen(WHITE);
		ILI9341_DrawText("    2.Pump Control    ", FONT4, 5, 5, RED, WHITE);
		ILI9341_DrawText("Turn Motor-", FONT4, 5, 35, RED, WHITE);
		subpage_temp2=1;
	}
	ILI9341_DrawText(motor_status, FONT4, 110, 35, RED, WHITE);
	ILI9341_DrawText(op_comment2, FONT4, 5, 80, RED, WHITE);
	ILI9341_DrawText(op_comment1, FONT4, 5, 110, RED, WHITE);
}



void sub_page_3(void){
	XX=5;
	YY=65;
	if(subpage_temp3==0){
		ILI9341_FillScreen(WHITE);
		ILI9341_DrawText("     3.Network Status    ", FONT4, 5, 5, RED, WHITE);
		ILI9341_DrawText("Wi-Fi:", FONT4, 5, 35, RED, WHITE);
		ILI9341_DrawText("Updated:", FONT4, XX, YY, BLACK, WHITE);
		ILI9341_DrawText(":", FONT4, XX+95, YY, BLACK, WHITE);
		ILI9341_DrawText(":", FONT4, XX+120, YY, BLACK, WHITE);
		ILI9341_DrawText("ago", FONT4, XX+150, YY, BLACK, WHITE);
		subpage_temp3=1;
	}
	ILI9341_DrawText(wifi_status, FONT4, 80, 35, RED, WHITE);
	ILI9341_DrawNumberInt(wifi_time_hour,FONT4,XX+75,YY,RED,WHITE);
	ILI9341_DrawNumberInt(wifi_time_min,FONT4,XX+100,YY,RED,WHITE);
	ILI9341_DrawNumberInt(wifi_time_sec,FONT4,XX+125,YY,RED,WHITE);
	if(wifi_time_sec==59){
		HAL_Delay(600);
		ILI9341_DrawText(". ", FONT4, XX+75, YY, BLACK, WHITE);
		ILI9341_DrawText(". ", FONT4, XX+100, YY, BLACK, WHITE);
		ILI9341_DrawText(". ", FONT4, XX+125, YY, BLACK, WHITE);
	}
}


void sub_page_4(void){
	if(subpage_temp4==0){
		ILI9341_FillScreen(WHITE);
		ILI9341_DrawText("        4.Balance       ", FONT4, 5, 5, RED, WHITE);
		ILI9341_DrawText("Current Balance(tk):", FONT4, 5, 35, RED, WHITE);
		ILI9341_DrawText("Last Recharge(tk):", FONT4, 5, 65, RED, WHITE);

		subpage_temp4=1;
	}
	ILI9341_DrawNumber(total_balance, FONT4, 175, 35, RED, WHITE);
	ILI9341_DrawNumberInt(IntRecDataTemp, FONT4, 175, 65, RED, WHITE);
}


void sub_page_5(void){
	if(subpage_temp5==0){
		ILI9341_FillScreen(WHITE);
		ILI9341_DrawText(" 5. Electrical summary ", FONT4, 5, 5, BLACK, WHITE);
		ILI9341_DrawText("Sold to Grid(kwh):", FONT4, 5, 35, RED, WHITE);
		ILI9341_DrawText("Purchased(kwh):", FONT4, 5, 65, RED, WHITE);
		ILI9341_DrawText("Sold to Grid(tk):", FONT4, 5, 115, RED, WHITE);
		subpage_temp5=1;
	}
	ILI9341_DrawNumber(unit_sol_to_grid_total, FONT4, 160, 35, RED, WHITE);
	ILI9341_DrawNumber(unit_grid_to_load_total, FONT4, 145, 65, RED, WHITE);

	ILI9341_DrawNumber((unit_sol_to_grid_total*5.5), FONT4, 160, 115, RED, WHITE);
	ILI9341_DrawText(reset_flash_status, FONT4, 50, 185, RED, WHITE);
}


void raw_diagnosis2(void){

	if(subpage_temp6==0){
		ILI9341_FillScreen(WHITE);
		ILI9341_DrawText("UnitSoldToGrid:", FONT4, 5, 5, RED, WHITE);//line text-1
		ILI9341_DrawText("UnitGridToLoad:", FONT4, 5, 35, RED, WHITE);//line text-2
		ILI9341_DrawText("solar power(kw): ", FONT4, 5, 65, RED, WHITE);//line text-3
		ILI9341_DrawText("load power(kw):", FONT4, 5, 95, RED, WHITE);//line text-4
		ILI9341_DrawText("grid supply volt:", FONT4, 5, 125, RED, WHITE);//line text-5
		ILI9341_DrawText("Current Balance(tk):", FONT4, 5, 155, RED, WHITE);//line text-6
		ILI9341_DrawText("motor_status:", FONT4, 5, 180, BLACK, WHITE);//line text-7
		ILI9341_DrawText("sub_sol_sup:", FONT4, 5, 205, BLACK, WHITE);//line text-8
		ILI9341_DrawText("wifi status:", FONT4, 5, 235, BLACK, WHITE);//line text-9
		ILI9341_DrawText("Last Recharge:", FONT4, 5, 270, BLACK, WHITE);//line text-10
		subpage_temp6=1;
	}
	ILI9341_DrawNumber(unit_sol_to_grid_total, FONT4, 150, 5, RED, WHITE);//line value-1
	ILI9341_DrawNumber(unit_grid_to_load_total, FONT4, 150, 35, RED, WHITE);//line value-2
	ILI9341_DrawNumber(solar_pow_in_kw, FONT4, 150, 65, RED, WHITE);//line value-3
	ILI9341_DrawNumber(load_pow_req_in_kw, FONT4, 150, 95, RED, WHITE);//line value-4
	ILI9341_DrawNumber(grid_supply_volt, FONT4, 150, 125, RED, WHITE);//line value-5
	ILI9341_DrawNumber(total_balance, FONT4, 175, 155, RED, WHITE);//line value-6
	ILI9341_DrawText(motor_status, FONT4, 175, 180, BLACK, WHITE);//line value-7   // To show wether the motor is ON or OFF;
	ILI9341_DrawText(subgrid_solar_supply_status, FONT4, 120, 205, BLACK, WHITE);//line value-8
	ILI9341_DrawText(wifi_status, FONT4, 120, 235, BLACK, WHITE);//line value-9
	ILI9341_DrawNumberInt(IntRecDataTemp, FONT4, 150, 270, RED, WHITE);//line value-10
	ILI9341_DrawText(write_flash_status, FONT4, 120, 300, BLUE, WHITE);//line value-10
	//ILI9341_DrawText(reset_flash_status, FONT4, 5, 300, BLUE, WHITE);//line value-10
	HAL_Delay(100);
}


void sub_page(){
	if (page_select_button==1){ // Trying to enter in sub_page
		if(main_page_cursor_button==1){
			sub_page_1();
			//ILI9341_DrawText("sub-menu 1", FONT4, 5, 300, RED, WHITE);
			HAL_Delay(100);
		}

		if(main_page_cursor_button==2){
			sub_page_2();
			//ILI9341_DrawText("sub-menu 2", FONT4, 5, 300, RED, WHITE);
			HAL_Delay(100);
		}

		if(main_page_cursor_button==3){
			sub_page_3();
			//ILI9341_DrawText("sub-menu 3", FONT4, 5, 300, RED, WHITE);
			HAL_Delay(100);
		}

		if(main_page_cursor_button==4){
			sub_page_4();
			//ILI9341_DrawText("sub-menu 4", FONT4, 5, 300, RED, WHITE);
			HAL_Delay(100);
		}

		if(main_page_cursor_button==5){
			sub_page_5();
			HAL_Delay(100);
			//ILI9341_DrawText("sub-menu 5", FONT4, 5, 300, RED, WHITE);
		}

		if(main_page_cursor_button==6){
			raw_diagnosis2();
			HAL_Delay(100);
		}

	}
}





void main_page_cursor(void){
	int prev_erase=main_page_cursor_button-1;
	if(prev_erase==0){
		prev_erase=6;
	}
	ILI9341_DrawText(" ", FONT4, 5, 5+((prev_erase)*35), BLACK, WHITE);
	ILI9341_DrawText(">", FONT4, 5, 5+(main_page_cursor_button*35), BLACK, WHITE);

	HAL_Delay(5);
}


void main_page(){
	if (page_select_button==0){
		if (temp1==0){
			ILI9341_FillScreen(WHITE);
			ILI9341_DrawText("Select Menu", FONT4, 60, 5, BLUE, WHITE);               // Y increment=35
			ILI9341_DrawText("1.Power Source Status", FONT4, 25, 40, RED, WHITE);
			ILI9341_DrawText("2.Pump Control", FONT4, 25, 75, RED, WHITE);
			ILI9341_DrawText("3.Network Status", FONT4, 25, 110, RED, WHITE);
			ILI9341_DrawText("4.Balance", FONT4, 25, 145, RED, WHITE);
			ILI9341_DrawText("5.Electrical Summary", FONT4, 25, 180, RED, WHITE);
			ILI9341_DrawText("6.Others", FONT4, 25, 215, RED, WHITE);
			HAL_Delay(100);


			subpage_temp1=0;
			subpage_temp2=0;
			subpage_temp3=0;
			subpage_temp4=0;
			subpage_temp5=0;
			subpage_temp6=0;
		}
		temp1=1;
		raw_diag_temp=0;
		main_page_cursor();
	}
}











//XXXXX----------NO CODE FOR CUSTOM FUNCTION AFTER THIS-----------XXXXX///


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
  MX_DMA_Init();
  MX_SPI1_Init();
  MX_ADC1_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */

  //For receiving data
  HAL_UART_Receive_IT(&huart1, (char*)&UART1_rxBuffer, 6);

  // For sendind data
  HAL_UART_Transmit_IT(&huart1, UART1_txBuffer, 6);

  //for ADC
  HAL_ADC_Start(&hadc1);

  //FOR  TIMER
  HAL_TIM_Base_Start_IT(&htim1);
  HAL_TIM_Base_Start_IT(&htim2);
  HAL_TIM_Base_Start_IT(&htim3);

  //For ILI9341 display
  ILI9341_Init();
  ILI9341_FillScreen(WHITE);
  ILI9341_SetRotation(SCREEN_VERTICAL_2);
  //ILI9341_DrawText("Welcome", FONT4, 2, 2, BLACK, WHITE);
  ILI9341_FillScreen(WHITE);
  HAL_Delay(500);

  //EEPROM_Reset_All();
  EEPROM_Read_All();

  sorting_rx_in_order(UART1_rxBuffer, UART1_rxBuffer_sorted); // For sorting the received data

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN  WHILE */

  while (1)
  {

	  sorting_rx_in_order(UART1_rxBuffer, UART1_rxBuffer_sorted); // For sorting the received data
	  //raw_diagnosis();
	  operational_calculation();
	  main_page();
	  sub_page();



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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 7200-1;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 10000;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 7200-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 10000;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

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
  htim3.Init.Prescaler = 7200-1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 10000;
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
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);

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
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_10|GPIO_PIN_12
                          |GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pins : PB0 PB1 PB10 PB12
                           PB13 PB14 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_10
                          |GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB3 PB4 PB5 */
  GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* Configure GPIO pins : PA12 PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);


  /* Configure GPIO pins : PA12 PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);


  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);

  HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

//This function is for UART receiving data interrupt


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

    HAL_UART_Receive_IT(&huart1, (char*)&UART1_rxBuffer, 6);
    sorting_rx_in_order(UART1_rxBuffer, UART1_rxBuffer_sorted);    // Dont use this function here, use somewhere else where needed

}

//This is the function where the custom timer settings is done as required

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(htim->Instance == TIM1)  // Timer-1
  {
    timerInSec1=timerInSec1+1;
  }

  if(htim->Instance == TIM2)  // Timer-2
  {
	  timerInSec2=timerInSec2+1;

	  write_flash_second_count++;           // To count time for writing flash in every 20 seconds
	  if(write_flash_second_count>=20){
		  write_flash_second_count_temp=1;
		  write_flash_second_count=0;
	  }

	  timer_global++;
	  if(timer_global>=5){        // TO UPDATE the both calculation in 5 sec
		  timer_global_temp=1;
		  timer_global=0;
	  }
  }

  if(htim->Instance == TIM3)  // Timer-3
  {
	  timerInSec3=timerInSec3+1;

  }
}



//This is the function for external interrupt using button


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  button1CurrMillis = HAL_GetTick();
  button2CurrMillis = HAL_GetTick();
  button3CurrMillis = HAL_GetTick();

  // PB3, PB4, PB5 has been chosen as interrupt pin
  if (GPIO_Pin == GPIO_PIN_3 && (button1CurrMillis - button1PrevMillis > 300))
  {
	  if (natemp==1){
		    button1++; //For testing only
		    if(page_select_button==0){ // That means current page is the home page
		    	main_page_cursor_button++ ; // This will move the cursor in home page
		    	if (main_page_cursor_button>=7){
		    		main_page_cursor_button=1;
		    	}
		    }
		    button1PrevMillis = button1CurrMillis;
		    natemp=0;
	  }
  }

  if (GPIO_Pin == GPIO_PIN_4 && (button2CurrMillis - button2PrevMillis > 300))
    {
	  if (natemp==1){
	      button2++; //For testing only
	      page_select_button++;      // This button will help for going to sub-page from main page
	      if(page_select_button>=2){
	    	  page_select_button=0;    // 0=main page ; 1= sub page
	      }
	      temp1++; // This temp will be used in main page
	      if (temp1>=1){
	    	  temp1=0;
	      }

	      button2PrevMillis = button2CurrMillis;
	      natemp=0;
	  }
    }

  if (GPIO_Pin == GPIO_PIN_5 && (button3CurrMillis - button3PrevMillis > 300))
    {
	  if (natemp==1){
	      button3++; //For testing only
	      if((main_page_cursor_button==2)&&(page_select_button==1)){ // Turn ON motor only from sub-page of 2nd menu
	          motor_switch++;
	          if(motor_switch>=2){
	        	  motor_switch=0;
	          }
	      }

	      if((main_page_cursor_button==5)&&(page_select_button==1)){  //For reseting the flash memory
	    	  reset_flash_temp=1;
	      }
	      button3PrevMillis = button3CurrMillis;
	      natemp==0;
	  }
    }
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
