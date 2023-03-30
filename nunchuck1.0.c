/* -----------------------------------------------------------------------------
 * Copyright (c) 2022-2023 
 * 
 * 
 * 
 * $Date:        30. March 2023
 * $Revision:    V1.0
 *
 * Project:      Control for Vehicle 
 * -------------------------------------------------------------------------- */



#include "GPIO_LPC17xx.h"               // Keil::Device:GPIO
#include "LPC17xx.h"                    // Device header
#include "PIN_LPC17xx.h"                // Keil::Device:PIN
#include "Driver_I2C.h"                 // ::CMSIS Driver:I2C
#include "Board_GLCD.h"                 // ::Board Support:Graphic LCD
#include "GLCD_Config.h"                // Keil.MCB1700::Board Support:Graphic LCD
#include "stdio.h"

#define registreCommand1	0xF0
#define registreCommand2	0x55
#define registreCommand3	0x00
#define SLAVE_I2C_ADDR	  0x52
#define registreX	0x00
#define registreY	0x01

extern ARM_DRIVER_I2C Driver_I2C0;
extern GLCD_FONT GLCD_Font_6x8;
extern GLCD_FONT GLCD_Font_16x24;

unsigned	int PosX;
unsigned	int PosY;	

uint8_t tab[10];


char texte1[50];
char texte2[50];
char texte3[50];

void Init_I2C(void){
	Driver_I2C0.Initialize(NULL);
	Driver_I2C0.PowerControl(ARM_POWER_FULL);
	Driver_I2C0.Control(	ARM_I2C_BUS_SPEED,				// 2nd argument = débit
							ARM_I2C_BUS_SPEED_STANDARD  );	// 100 kHz
	Driver_I2C0.Control(	ARM_I2C_BUS_CLEAR,0 );
}
void Init_LCD(void){
	GLCD_Initialize();
	GLCD_ClearScreen();
	GLCD_SetFont(&GLCD_Font_16x24);
	GLCD_SetForegroundColor(GLCD_COLOR_BLACK);
}
void Timer_Init(unsigned int prescaler, unsigned int valeur){
LPC_SC->PCONP |= (1<<1); //allume le timer 0 (facultatif, déjà allumé après un reset)

LPC_TIM0->PR =  prescaler;
LPC_TIM0->MR0 = valeur; 
LPC_TIM0->MCR = 3;	/*reset counter si MR0=COUNTER + interrupt*/

LPC_TIM0->TCR = 1; //démarre le comptage
}
void write1byte (unsigned char composant , unsigned char registre, unsigned char valeur){
	uint8_t tab[10];
	
	tab[0] = registre;
	tab[1] = valeur;
	
	// Ecriture vers registre esclave : START + ADDR(W) + 1W_DATA + 1W_DATA + STOP
	Driver_I2C0.MasterTransmit (composant , tab, 2, false);		// false = avec stop
	while (Driver_I2C0.GetStatus().busy == 1);	// attente fin transmission
	

}
void write2byte ( unsigned char registre, unsigned char valeur){
	uint8_t tab[10];
	
	tab[0] = valeur;

	
	// Ecriture vers registre esclave : START + ADDR(W) + 1W_DATA + 1W_DATA + STOP
	Driver_I2C0.MasterTransmit (registre, tab, 1, false);		// false = avec stop
	while (Driver_I2C0.GetStatus().busy == 1);	// attente fin transmission
	

}
unsigned char read1byte (unsigned char composant, unsigned char registre){
	unsigned char retour;
	
	
	
	tab[0] = registre;
	// Ecriture vers registre esclave : START + ADDR(W) + 1W_DATA + RESTART
//	Driver_I2C0.MasterTransmit (composant , 0, 1, true);		// true = avec restart
//	while (Driver_I2C0.GetStatus().busy == 1);	// attente fin transmission
//		
	// Lecture de data esclave : START + ADDR(R) + 1R_DATA + STOP
	Driver_I2C0.MasterReceive (composant, tab,6, false);		// false = avec stop
	while (Driver_I2C0.GetStatus().busy == 1);	// attente fin transmission
	
	return retour;
}


int main (void)
{

	Init_I2C();
	Init_LCD();
	Timer_Init(100, 25000);
	
	
	write1byte(SLAVE_I2C_ADDR,registreCommand1,registreCommand2);

	
	NVIC_EnableIRQ(TIMER0_IRQn);
	NVIC_SetPriority(TIMER0_IRQn,2);
	
	NVIC_SetPriority(I2C0_IRQn,0);
	NVIC_SetPriority(SSP1_IRQn,0);

	
	while(1)
	{
		
	}
	
}

void TIMER0_IRQHandler(void)
{
	int j;
	LPC_TIM0->IR |= 1<<0;
  
	write2byte(SLAVE_I2C_ADDR,registreCommand3);
	
	for (j=0;j<=10000;j++)
	{
		
	}
	read1byte(SLAVE_I2C_ADDR,registreX);
	
	PosX=tab[0];
	PosY=tab[1];
	
	sprintf(texte1,"La position X: %4d",PosX);
	GLCD_DrawString(1, 1, texte1);
	
	sprintf(texte2,"La position Y: %4d",PosY);
	GLCD_DrawString (1, 40,texte2);
	

}
