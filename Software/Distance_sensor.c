/*
 * Distance_sensor.cpp
 *
 * Created: 07.05.2018 13:23:57
 * Author : Patryk Dusza
 */ 
 
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include "u8g.h"
#include "ws2812.h"
#include "ws2812_syms.h"

u8g_t u8g;
int pulse = 0, flag = 0, prev_dist = 0;

void InitInterrupt(void)
{
	DDRD &= ~(1 << DDD2);  
	PORTD |= (1 << PD2); 
	EICRA|=(1<<ISC00);
	EIMSK|=(1<<INT0);
}

void FullInit(void)
{
	DDRB |= 0xFF;
	u8g_InitI2C(&u8g, &u8g_dev_sh1106_128x64_i2c, U8G_I2C_OPT_NONE);
	InitInterrupt();
	sei();
}

void draw()
{	
	char liczba[20];
	strcpy(liczba,"");
	sprintf(liczba, "Odleglosc: %d", prev_dist);
	u8g_SetFont(&u8g, u8g_font_6x10);
	u8g_DrawStr(&u8g, 0, 15, "Distance sensor");
	u8g_DrawStr(&u8g, 0, 30, liczba);
}

void set_red(int n)
{
	for(uint8_t i=0; i<WS2812_LED_NUM; i++)
	{
		ws2812_set_red(i, n); 
		ws2812_set_green(i, 0);
	}
}

void set_green(int n)
{
	for(uint8_t i=0; i<WS2812_LED_NUM; i++)
	{
		ws2812_set_green(i, n);
		ws2812_set_red(i, 0); 
	}
}

void isDistanceCorrect()
{
	int dist = pulse/58;
	
	if((dist > 0)&&(dist <= 400))
		prev_dist = dist;
}

void ShowDistanceOnLed()
{
	
	
	if(prev_dist <= 200)
	{
		set_red(200-prev_dist);
	}
	else
	{
		set_green(prev_dist-200);
	}
	
	ws2812_display();
}

int main(void)
{
	FullInit();
	ws2812_init();
    while (1) 
    {
			PORTB |= (1 << 1);		
			_delay_us(10);
			sei();
			PORTB &= ~(1 << 1);
			_delay_ms(100);
			isDistanceCorrect();
			u8g_FirstPage(&u8g);
			do{
				draw();
			}while(u8g_NextPage(&u8g));
			ShowDistanceOnLed();
			pulse=0;
    }
}

ISR(INT0_vect){
  if (flag==1)
  {
	  TCCR1B=0;
	  pulse=TCNT1;
	  TCNT1=0;
	  flag=0;
  }
  if (flag==0)
  {
	  TCCR1B|=(1<<CS11);
	  flag=1;
  }
}