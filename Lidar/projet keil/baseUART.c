#include "Driver_USART.h"               // ::CMSIS Driver:USART
#include "Board_GLCD.h"                 // ::Board Support:Graphic LCD
#include "GLCD_Config.h"                // Keil.MCB1700::Board Support:Graphic LCD
#include "stdio.h"
#include "GPIO.h"


extern GLCD_FONT GLCD_Font_6x8;
extern GLCD_FONT GLCD_Font_16x24;
extern ARM_DRIVER_USART Driver_USART1;


uint8_t tab_cmd[2];
uint8_t tab_reset[2];	
uint8_t tab_stop[2];
	

void pwm(int dc);

void Init_UART(void){
	Driver_USART1.Initialize(NULL);
	Driver_USART1.PowerControl(ARM_POWER_FULL);
	Driver_USART1.Control(	ARM_USART_MODE_ASYNCHRONOUS |
							ARM_USART_DATA_BITS_8		|
							ARM_USART_STOP_BITS_1		|
							ARM_USART_PARITY_NONE		|
							ARM_USART_FLOW_CONTROL_NONE,
							115200);
	Driver_USART1.Control(ARM_USART_CONTROL_TX,1);
	Driver_USART1.Control(ARM_USART_CONTROL_RX,1);
}

int main (void){
	
	uint8_t tab_receive[40];
	char texte[50];

	int centreX=160;
	int centreY=120;
	
	int i;
	
	tab_cmd[0]= 0xA5;
	tab_cmd[1] = 0x20;
	tab_reset[0]= 0xA5 ;
	tab_reset[1]= 0x40 ;
	tab_stop[0]= 0xA5 ;
	tab_stop[1]= 0x25 ;
	
	Init_UART();
	GLCD_Initialize();
	GLCD_ClearScreen();
	GLCD_SetFont(&GLCD_Font_16x24);
	Initialise_GPIO();

	
	
	
	
	/*
	while(Driver_USART1.GetStatus().tx_busy == 1); // attente buffer TX vide
	Driver_USART1.Send(tab_reset, 2);

	for(i=0;i<100000;i++);
	*/
	
	/*while(Driver_USART1.GetStatus().tx_busy == 1); // attente buffer TX vide
	Driver_USART1.Send(tab_cmd, 2);*/
	while (1){
		pwm(95);
		while(Driver_USART1.GetStatus().tx_busy == 1); // attente buffer TX vide
		Driver_USART1.Send(tab_cmd, 2);
		
		for(i=0;i<1000000000;i++);
		
		Driver_USART1.Receive(tab_receive,40); //on a besoin de : 2 octets pour l'angle et deux pour la distance, le premier est pour vf�rifier si une r�solution a �t� faite.
		while(Driver_USART1.GetRxCount()<1); 
		
		pwm(0);
		while(Driver_USART1.GetStatus().tx_busy == 1); // attente buffer TX vide
		Driver_USART1.Send(tab_stop, 2);
		
		sprintf(texte, "trame %2x %2x %2x", tab_receive[0],tab_receive[3], tab_receive[4]); 
		GLCD_DrawString(1,1,texte);
		GLCD_DrawPixel(centreX, centreY);
		for(i=0;i<1000000000;i++);
		
		
		
	}	
	return 0;
}























void pwm(int dc)
{
	LPC_SC->PCONP = LPC_SC->PCONP | 0x00000040;       // enable PWM1

			
	LPC_PWM1 -> MCR |= (2<<0);			// Compteur relanc� quand MR0 repasse � 0
	
	LPC_PWM1->PR = 0; // prescaler
	LPC_PWM1->MR0 = 999; // freqs du PWM vaut 25kHz
	
	
	LPC_PINCON->PINSEL4 = LPC_PINCON->PINSEL4| 0x00000010;	// Validation des sorties PWM1.3 voir la table 84
	LPC_PWM1->LER |= 0x0000000F;
	LPC_PWM1->PCR = LPC_PWM1->PCR | 0x00000800;	//Autorise la sortie PWM3 bit 11 � voir table 452
	
	LPC_PWM1->MR3 = 10*dc;      //On r�gle le duty cycle � dc% de PMW1.3 sur la broche P2.2
	
	LPC_PWM1 -> TCR = 1;
}