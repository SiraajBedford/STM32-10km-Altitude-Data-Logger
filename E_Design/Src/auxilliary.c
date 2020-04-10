/*
 * auxilliary.c
 *
 *  Created on: 28 Feb 2019
 *      Author: 20797540
 */

/*
 * Author: S. Bedford
 *
 * Credit: A. Esmael for Demo 2 code
 *
 * */



#include "stm32f3xx_it.h"
#include "auxilliary.h"
#include "main.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"



extern uint8_t GPS_data[100];
float longitude;
float latitude;
float altitude;
char hour[3] = "00";
char sec[3]="00";
char min[3]="00";
volatile uint8_t flag;

int lasttick ;
char output_string [91];
int student_num=21093741;
char GPGGA_string [75] = "\0";
char Time[8];
int range_buffer[5] = {0,0,0,0,0};
int burn_start;
int burn_flag;
int runtime;
int field_check[6] = {0,0,0,0,0,0};//[0]=time, [1]=latitude,[2]=latitude direction,[3]=longitude,[4]=longitude direction,[5]=altitude
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
char altitude_c[9] = "0.0000000";

char disp[6];
char lcdMessage[17];
char altitude_in_message[6];

extern float hum;
extern float press;
extern float temperature;

//User: S. Bedford, SU: 21093741
//Permission A. Esmael, SU: 20797540
void var_init()
{
	/* USER CODE BEGIN 2 */
	runtime = 0;
	lasttick = 0;
	student_num = 21093741;
	flag = 0;
	longitude = 0;
	latitude = 0;
	altitude = 0;
	GPS_array_clear();
	burn_flag = 0;
	burn_start = 0;
	ADC_volatage_in_volts=0;
	ADC_current_in_milliamps=0;
}

//User: S. Bedford, SU: 21093741
//Permission A. Esmael, SU: 20797540
void GPS_array_clear()
{
	int i;
	for(i = 0 ; i < sizeof(GPGGA_string) ; i++)
	{
		GPGGA_string[i] = 0;
	}
}

//User: S. Bedford, SU: 21093741 + Added Functionality for LCD
//Permission A. Esmael, SU: 20797540
void StringPrint()
{

	//float ne = 1111.1;

	lasttick = HAL_GetTick();
	if(flag == 1)
	{

		sprintf(output_string , "$%8d,%5d,%2s:%2s:%2s,%3d,%3d,%3d,%4d,%4d,%4d,%10f,%11f,%7.1f,%3d,%3.1f\n" , student_num , lasttick/1000 , hour , min , sec ,(int)temperature,(int)hum,(int)press,(int)accx,(int)accy,(int)accz,latitude,longitude, altitude,(int) ave_current, ave_voltage);


		//sprintf(output_string, "$%8d,%5d\n", student_num, lasttick/1000,0);
		//HAL_UART_Transmit_IT(&huart1, (uint8_t*)output_string, 91);
		//HAL_UART_Transmit(&huart1, (uint8_t*)output_string, 91, 1000);
		HAL_UART_Transmit_IT(&huart1, (uint8_t*)output_string, 91);

		// lcd update



		itoa((int)altitude, altitude_in_message, 10);
		strcat(altitude_in_message,"n");
		sprintf(disp,"%-5s",altitude_in_message);

		sprintf(lcdMessage, "%-6s   %c  %3dC", disp,burn_flag ? 'B' : ' ', (int)temperature);

		lcdPrint(lcdMessage);

		flag = 0;
	}

	//	/* Demonstrate with numbers. */
	//	    printf ("<%d> is not justified.\n", x);
	//	    printf ("<%5d> is right-justified.\n", x);
	//	    printf ("<%-5d> The minus sign makes it left-justified.\n", x);
	//	    /* Demonstrate with strings. */
	//	    printf ("'%s' is not justified.\n", y);
	//	    printf ("'%10s' is right-justified.\n", y);
	//	    printf ("'%-10s' is left-justified using a minus sign.\n", y);





}

//User: S. Bedford, SU: 21093741
//Permission A. Esmael, SU: 20797540
void BurnReleaseOn()
{
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, 1);
}

//User: S. Bedford, SU: 21093741
//Permission A. Esmael, SU: 20797540
void BurnReleaseOff()
{
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, 0);
}

//User: S. Bedford, SU: 21093741
//Permission A. Esmael, SU: 20797540

void GPSReceive()
{
	HAL_UART_Receive(&huart1, (uint8_t*)GPGGA_string,79 , 1000 );

}



//User: S. Bedford, SU: 21093741
//Permission A. Esmael, SU: 20797540
int check_sum(const char *s)
{

	int i =0;
	char tempStr[74];
	char checkSum[3];
	strcpy(tempStr , s);


	while(tempStr[i] != '*')
	{
		i++;
	}

	strncpy(checkSum , tempStr+ i+1 , 2);
	checkSum[2] = '\0';

	strncpy(tempStr , s , i+1);
	tempStr[i+1] = '\0';


	int c = 0;
	i = 1;
	while(tempStr[i] != '*')
	{
		c ^= tempStr[i];
		i++;
	}

	return (c==(int)strtol(checkSum, NULL, 16));
}
//User: S. Bedford, SU: 21093741
//Permission A. Esmael, SU: 20797540
void GPGGA_Split(const char *s)
{

	char time_c[10];

	char GPGGA[71];
	strcpy(GPGGA , s);
	const char delimeter[2] = ",";
	char *token;
	char temp[10];

	char GPGGA2[71];
	strcpy(GPGGA2 , s);
	char *token2;

	if(check_sum(GPGGA_string))                    ///DO THIS
	{
		char message_type[7];
		strncpy(message_type, s , 6);
		message_type[6] = 0;
		const char GPGGA_ident[] = "$GPGGA";
		if(strcmp(GPGGA_ident,message_type)==0)
		{
			presence_check(GPGGA_string);

			if(field_check[0]==1)
			{
				GetTime(s);
				//				token = strtok(NULL, delimeter); //time
				//				strcpy(time_c, token);//take out time
				//				strncpy(hour, time_c, 2);
				//				strncpy(min, time_c+2, 2);
				//				strncpy(sec, time_c+4, 2);
			}

			//begin latitude
			if(field_check[1]==1)
			{
				GetLatitude(s);
				//				token = strtok(NULL, delimeter);
				//				strcpy(temp, token);
				//				char lat_deg_char[3];
				//				char lat_mins_char[8];
				//				strncpy(lat_deg_char, temp, 2);
				//				strncpy(lat_mins_char, temp+2, 8);
				//
				//				latitude = atof(lat_deg_char) + atof(lat_mins_char)/60 ;
				//
				//				token = strtok(NULL, delimeter);
				//				strcpy(temp, token);
				//
				//				if(*temp == 'S')
				//				{
				//					latitude *= -1;
				//				}
			}

			//begin longitude
			if(field_check[3]==1)
			{


				GetLongitude(s);
				//				token = strtok(NULL, delimeter);
				//
				//				strcpy(temp, token);
				//
				//				char long_deg_char[4];
				//				char long_mins_char[8];
				//
				//				strncpy(long_deg_char, temp, 3);
				//				strncpy(long_mins_char, temp+3, 8);
				//
				//				longitude = atof(long_deg_char) + atof(long_mins_char)/60 ;
				//
				//				token = strtok(NULL, delimeter);
				//
				//				strcpy(temp, token);
				//
				//				if(*temp == 'W')
				//				{
				//					longitude *= -1;
				//				}
			}
			if(field_check[5]==1)
			{

				GetAltitude(s);

			}
			//		GPS_array_clear();
		}
	}
	if(burn_flag == 0)
	{
		range_check();
	}
	GPS_array_clear();
}

//User: S. Bedford, SU: 21093741
//Permission A. Esmael, SU: 20797540
void add_element(int element)
{
	int i = 0;
	for(i = 4 ; i >= 1 ; i --)
	{
		range_buffer[i] = range_buffer[i-1];
	}
	range_buffer[0] = element;
}

//User: S. Bedford, SU: 21093741
//Permission A. Esmael, SU: 20797540
int five_consecutive_check()
{
	int i = 0 ;
	while(range_buffer[i] != 0 && i < 5)
	{
		i++;
	}
	return (i == 5);
}

//User: S. Bedford, SU: 21093741
//Permission A. Esmael, SU: 20797540
void range_check()
{
	if(altitude > 10000 && (longitude > 18.9354 || longitude <  17.976343))
	{
		add_element(1);
	}
	else
	{
		add_element(0);
	}
}

//User: S. Bedford, SU: 21093741
//Permission A. Esmael, SU: 20797540
void clear_range_buffer()
{
	int i;
	for(i = 0 ; i < 5 ; i++)
	{
		range_buffer[i] = 0;
	}
}








//User: S. Bedford, SU: 21093741
//Permission A. Esmael, SU: 20797540
void presence_check(const char *s)
{	int i = 0;//LHS comma
int j = 0;//RHS comma
//time check begins
while(s[i] != ',')
{
	i++;
}
j=i+1;
while(s[j] != ',')
{
	j++;
}

if((j - i > 1))
{
	field_check[0] = 1;
}
else
{
	field_check[0] = 0;
}


//time check ends
//latitude begin
i = j;
j+=1;
while(s[j] != ',')
{
	j++;
}
if((j - i > 1))
{
	field_check[1] = 1;
}
else
{
	field_check[1] = 0;
}
//latitude end
//latitude direction begin
i = j;
j++;
while(s[j] != ',')
{
	j++;
}
if((j - i > 1))
{
	field_check[2] = 1;
}
else
{
	field_check[2] = 0;
}
//latitude direction end
//longitude begin
i = j;
j++;
while(s[j] != ',')
{
	j++;
}
if((j - i > 1))
{
	field_check[3] = 1;
}
else
{
	field_check[3] = 0;
}
//longitude end
//longitude direction begin

i = j;
j++;
while(s[j] != ',')
{
	j++;
}
if((j - i > 1))
{
	field_check[4] = 1;
}
else
{
	field_check[4] = 0;
}
//longitude direction end

j++;
i = j;
int comma_count =0;
while(comma_count != 3)
{
	if(s[i] == ',')
	{
		comma_count++;
	}
	if(comma_count !=3)
	{
		i++;
	}
}

j= i+1;
while(s[j] != ',')
{
	j++;
}
if((j - i > 1))
{
	field_check[5] = 1;
}
else
{
	field_check[5] = 0;
}
}


//User: S. Bedford, SU: 21093741
//Permission A. Esmael, SU: 20797540
void GetAltitude(const char *s)
{
	int i = 0 , j = 0;
	int comma_count =0;
	while(comma_count < 9)
	{
		if(s[i] ==  ',')
		{
			comma_count++;
		}
		i++;
	}
	j = i+1;
	while(s[j] != ',')
	{
		j++;
	}


	strncpy(altitude_c , s+i , j- i);
	altitude = atof(altitude_c);

}



//User: S. Bedford, SU: 21093741
//Permission A. Esmael, SU: 20797540
void GetTime(const char *s)
{
	char time_c[11];
	int i = 0 , j = 0;
	int comma_count =0;
	while(comma_count < 1)
	{
		if(s[i] ==  ',')
		{
			comma_count++;
		}
		i++;
	}
	j = i+1;
	while(s[j] != ',')
	{
		j++;
	}
	strncpy(time_c , s+i , j- i);
	strncpy(hour, time_c, 2);
	strncpy(min, time_c+2, 2);
	strncpy(sec, time_c+4, 2);

}

//User: S. Bedford, SU: 21093741
//Permission A. Esmael, SU: 20797540
void GetLatitude(const char *s)
{
	char latitude_c[11];
	int i = 0 , j = 0;
	int comma_count =0;
	while(comma_count < 2)
	{
		if(s[i] ==  ',')
		{
			comma_count++;
		}
		i++;
	}
	j = i+1;
	while(s[j] != ',')
	{
		j++;
	}

	strncpy(latitude_c , s+i , j- i);
	char lat_deg_char[3];
	char lat_mins_char[8];
	strncpy(lat_deg_char, latitude_c, 2);
	strncpy(lat_mins_char, latitude_c+2, 8);

	latitude = atof(lat_deg_char) + atof(lat_mins_char)/60 ;


	char direction = s[j+1];
	if(direction== 'S')
	{
		latitude *= -1;
	}

}

//User: S. Bedford, SU: 21093741
//Permission A. Esmael, SU: 20797540
void GetLongitude(const char *s)
{
	char longitude_c[11];
	int i = 0 , j = 0;
	int comma_count =0;
	while(comma_count < 4)
	{
		if(s[i] ==  ',')
		{
			comma_count++;
		}
		i++;
	}
	j = i+1;
	while(s[j] != ',')
	{
		j++;
	}

	strncpy(longitude_c , s+i , j- i);
	char long_deg_char[4];
	char long_mins_char[8];
	strncpy(long_deg_char, longitude_c, 3);
	strncpy(long_mins_char, longitude_c+3, 8);

	longitude = atof(long_deg_char) + atof(long_mins_char)/60 ;


	char direction = s[j+1];
	if(direction== 'S')
	{
		longitude *= -1;
	}

}

