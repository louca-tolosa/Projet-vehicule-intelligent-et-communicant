#include "Driver_USART.h"               // ::CMSIS Driver:USART



#include "stdio.h"
#include "LPC17xx.h"                    // Device header
#include "GPIO_LPC17xx.h"               // Keil::Device:GPIO
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "Board_GLCD.h"                 // ::Board Support:Graphic LCD
#include "GLCD_Config.h"                // Keil.MCB1700::Board Support:Graphic LCD

extern ARM_DRIVER_USART Driver_USART1;
extern GLCD_FONT GLCD_Font_16x24;


void tache1(void const *argument); //prototype
osThreadId ID_tache1;
osThreadDef (tache1, osPriorityNormal, 1, 0); //1 instance,taille pile par défaut

void Send_DFP(char commande, char P1, char P2);

void Init_UART(void){
	Driver_USART1.Initialize(NULL);
	Driver_USART1.PowerControl(ARM_POWER_FULL);
	Driver_USART1.Control(	ARM_USART_MODE_ASYNCHRONOUS |
							ARM_USART_DATA_BITS_8		|
							ARM_USART_STOP_BITS_1		|
							ARM_USART_PARITY_NONE		|
							ARM_USART_FLOW_CONTROL_NONE,
							9600);
	Driver_USART1.Control(ARM_USART_CONTROL_TX,1);
	Driver_USART1.Control(ARM_USART_CONTROL_RX,1);
}

int main (void){
	
	GLCD_Initialize();
	Init_UART();
	osKernelInitialize ();
	
	ID_tache1 = osThreadCreate ( osThread ( tache1 ), NULL ) ; //changer le nul si plusieurs instances
	GLCD_ClearScreen();
	GLCD_SetFont(&GLCD_Font_16x24);
	
	Send_DFP(0x09, 0x00, 0x01);
	osDelay(10);
		
	Send_DFP(0x10, 0x01, 0x0F);
	osDelay(10);

	osKernelStart();
	osDelay(osWaitForever) ;
	return 0;
}


void tache1(void const *argument) {
	uint8_t tab_W[64];
	uint8_t tab_R[15];
	int i;
	while(1)
	{
//	while(Driver_USART1.GetStatus().tx_busy == 1); // attente buffer TX vide
//	Driver_USART1.Send(tab_W,16);
	
	  Driver_USART1.Receive(tab_R,14);
//		while (Driver_USART1.GetRxCount() <1); // on attend que 1 case soit pleine
		if((tab_R[0] == 0x02) && (tab_R[13] == 0x03))
		{
			if((tab_R[1]==0x30) && (tab_R[2]==0x44) && (tab_R[3]==0x30) && (tab_R[4]==0x30) && (tab_R[5]==0x39) && (tab_R[6]==0x33) && (tab_R[7]==0x36) && (tab_R[8]==0x34) && (tab_R[9]==0x31) && (tab_R[10]==0x42) && (tab_R[11]==0x45) && (tab_R[12]==0x31)) //ID : 0D0093641BE1 en ASCII
			{
					GLCD_DrawString(50,100,"                 ");
					GLCD_DrawString(50,100,"Acces autorise");
					Send_DFP(0x03, 0x00, 0x07);
//					osDelay(2000);
					for(i=0; i<15; i++)
					{
						tab_R[i] = 0;
					}
			}
		
			else 
			{
				GLCD_DrawString(50,100,"                  ");
				GLCD_DrawString(50,100,"Acces refuse");
				Send_DFP(0x03, 0x00, 0x08);
//				osDelay(2000);
				for(i=0; i<15; i++)
				{
					tab_R[i] = 0;
				}
				
			}
		}
		
	}
}

void Send_DFP(char commande, char P1, char P2)
{
	short CS;
	short CS1, CS2;
	char tab[10] = {0x7E, 0xFF, 0x06, 0, 0x00, 0, 0, 0, 0, 0xEF}; ;
	
	CS = -0xFF-0x06-commande-P1-P2;
	CS1 = (CS & 0xFF00) >> 8;
	CS2 = CS & 0x00FF;
	
	tab[3] = commande;
	tab[5] = P1;
	tab[6] = P2;
 	tab[7] = CS1;
 	tab[8] = CS2; 
	
	while(Driver_USART1.GetStatus().tx_busy == 1); // attente buffer TX vide
	Driver_USART1.Send( tab,10); //0x7E ,0xFF, 0x06, 0x09, 0x00, 0x00, 0x02, 0x01, 0x10, 0xEF
	//osDelay(50);
}
