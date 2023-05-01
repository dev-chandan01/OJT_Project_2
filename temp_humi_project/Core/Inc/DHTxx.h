/*
 * DHTxx.h
 *
 *  Created on: Apr 25, 2023
 *      Author: chandan
 */

#ifndef INC_DHTXX_H_
#define INC_DHTXX_H_

/*Initialize htim1*/
TIM_HandleTypeDef htim1;


char str[20] = {0};
char str1[20] = {0};
/*Generate us delay*/
void delay_us (uint16_t us)
{
	__HAL_TIM_SET_COUNTER(&htim1,0);  // set the counter value a 0
	while (__HAL_TIM_GET_COUNTER(&htim1) < us);  // wait for the counter to reach the us input in the parameter
}

/*Display Temperature*/
void Display_Temp (int Temp)
{
	//char str[20] = {0};
	//LCD_nibble_write(0, 0);
	LCD_command(0x80);
	sprintf (str, "TEMP:-%d", Temp);
	LCD_string(str);
	LCD_string("\xDF");
	LCD_data('C');
}

/*Display Humidity*/
void Display_Rh (int Rh)
{
	//char str1[20] = {0};
	//LCD_nibble_write(1, 0);
	LCD_command(0xC0);
	sprintf (str1, "RH:-%d", Rh);
	LCD_string(str1);
	LCD_data('%');
}


/*Initialization*/
uint8_t Rh_byte1, Rh_byte2, Temp_byte1, Temp_byte2;
uint16_t SUM, RH, TEMP;

int read_temp = 0;
int read_humi = 0;
uint8_t Presence = 0;

/*Set Pin as OUTPUT*/
void Set_Pin_Output (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

/*Set Pin as INPUT*/
void Set_Pin_Input (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

/*********************************** DHT11 FUNCTIONS ********************************************/

#define DHT11_PORT GPIOB
#define DHT11_PIN GPIO_PIN_1

void DHT11_Start (void)
{
	Set_Pin_Output (DHT11_PORT, DHT11_PIN);  // set the pin as output
	HAL_GPIO_WritePin (DHT11_PORT, DHT11_PIN, 0);   // pull the pin low
	delay_us(18000);   // wait for 18ms
    HAL_GPIO_WritePin (DHT11_PORT, DHT11_PIN, 1);   // pull the pin high
    delay_us (40);   // wait for 20us
	Set_Pin_Input(DHT11_PORT, DHT11_PIN);    // set as input
}

uint8_t DHT11_Check_Response (void)
{
	uint8_t Response = 0;
	delay_us (40);
	if (!(HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN)))
	{
		delay_us (80);
		if ((HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN))) Response = 1;
		else Response = -1; // 255
	}
	while ((HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN)));   // wait for the pin to go low

	return Response;
}

uint8_t DHT11_Read (void)
{
	uint8_t i,j;
	for (j=0;j<8;j++)
	{
		while (!(HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN)));   // wait for the pin to go high
		delay_us (40);   // wait for 40 us
		if (!(HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN)))   // if the pin is low
		{
			i&= ~(1<<(7-j));   // write 0
		}
		else i|= (1<<(7-j));  // if the pin is high, write 1
		while ((HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN)));  // wait for the pin to go low
	}
	return i;
}

/*****************************sense value ********************************/

void sense_value(void){
	      Display_Temp(read_temp);
		  Display_Rh(read_humi);

		/********************** DHT11 *********************/

		      DHT11_Start();
		 	  Presence = DHT11_Check_Response();
		 	  Rh_byte1 = DHT11_Read ();
		 	  Rh_byte2 = DHT11_Read ();
		 	  Temp_byte1 = DHT11_Read ();
		 	  Temp_byte2 = DHT11_Read ();
		 	  SUM = DHT11_Read();

		 	  TEMP = Temp_byte1;
		 	  RH = Rh_byte1;

		 	 read_temp = (int) TEMP;
		 	read_humi = (int) RH;


		 	 HAL_Delay(1000);
}
#endif /* INC_DHTXX_H_ */



/*For Analog sensor*/
//void temp_value (void){
//	HAL_ADC_Start(&hadc1);
//
//					adcval1 = HAL_ADC_GetValue(&hadc1);//reads the adc value after conversion and stores it in a variable called adcval1.
//
//
//					temp= ((double)adcval1/4096) * 330;//converts the adcval1 value into Degree celsius
//					Humi = (float) ((adcval1<<8)|adcval1)/10;
//
//				LCD_command(0x80);
//				LCD_string("Temp:");
//				LCD_data(((temp/10)%10)+48);
//				LCD_data((temp%10)+48);
//				LCD_string("\xDF");
//			    LCD_string("C");
//		        LCD_command(0xC0);
//				LCD_string("Humi:");
//				LCD_data(((Humi/10)%10)+48);
//				LCD_data((Humi%10)+48);
//				LCD_string("%");
//}
