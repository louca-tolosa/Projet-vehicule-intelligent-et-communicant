/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "osObjects.h"                      // RTOS object definitions
#include "bibli_lidar.h"
#include "GPIO.h"
#include "GLCD_Config.h"                // Keil.MCB1700::Board Support:Graphic LCD
#include "Board_GLCD.h"                 // ::Board Support:Graphic LCD
#include "stdio.h"
#include "Driver_USART.h"               // ::CMSIS Driver:USART



extern GLCD_FONT GLCD_Font_6x8;
extern GLCD_FONT GLCD_Font_16x24;
extern ARM_DRIVER_USART Driver_USART1;


typedef struct{
	short angle;
	short dist;
}colissimo;

osMailQId ID_BAL;
osMailQDef(BAL,1,colissimo);


osThreadId IDtache_Lidar_On;
osThreadId IDtache_traitement;

void tache_Lidar_On (void const *argument);
void tache_traitement (void const *argument);

void tache_Lidar_On (void const *argument)
{
	
	int i,j;
	char QUALITE_LIDAR;
	char tab[20];
	char DEBUT_LIDAR[7];
	char ANGLE_LIDAR[2];
	char PAQUET_INUTILE[4];
	char DIST_LIDAR[2];
	unsigned short envoi_angle;
	unsigned short envoi_dist;
	char etat;
	char affichage[25];
	char TAB_COMPAR_LIDAR_DEBUT[7] = {0xA5 , 0x5A, 0x05, 0x00, 0x00, 0x40, 0x81};
	colissimo *ptr1;
	etat=0;
	
	while(1)
	{
		ptr1=osMailAlloc(ID_BAL,1000);
		switch(etat)
		{
			case 0 : //lidar On
			{
				/*preparation au debug*/
				Eteindre_1LED(0);
				Eteindre_1LED(1);
				Eteindre_1LED(2);
				Eteindre_1LED(6);
				Eteindre_1LED(7);

				pwm(85);
				
				laser_on();
			//	osDelay(2000);
				
				etat=1;
				break;
			}//fin case 0
			
			case 1 : //reception de la trame de debut
			{
				Driver_USART1.Receive(DEBUT_LIDAR,7);
				osSignalWait(0x0001, osWaitForever);
				Allumer_1LED(6);
				
				
				if (strncmp(TAB_COMPAR_LIDAR_DEBUT,DEBUT_LIDAR,7)==0) /*comparaison avec le test*/
				{
					Allumer_1LED(0); //temoin que la trame est bonne
					etat =2;
										
				}//fin if
				else//si la comparaison n'est pas bonne
				{
					Allumer_1LED(7);
					reset_lidar();
					//pwm(0);
					osDelay(2000);
					etat=0;
					
				}//fin else
				break;
			}//fin case 1
			
			case 2 : //reception des donnees
			{
				Driver_USART1.Receive(&QUALITE_LIDAR,1);
				osSignalWait(0x0001, osWaitForever);//sommeil jusqu'a recep du tableau total
				if (QUALITE_LIDAR > 48)
				{
					Driver_USART1.Receive(ANGLE_LIDAR,2);
					osSignalWait(0x0001, osWaitForever);
					
					
					envoi_angle = (short)(((short)ANGLE_LIDAR[1])<<7) || (ANGLE_LIDAR[0] >> 1);
					ptr1->angle=envoi_angle;
					Driver_USART1.Receive(DIST_LIDAR,2);
					
					osSignalWait(0x0001, osWaitForever);
					
					
					envoi_dist = (short)(((short)DIST_LIDAR[1])<<8) || (DIST_LIDAR[0]);
					ptr1->dist=envoi_dist;
					osMailPut(ID_BAL,ptr1);
					
				}//fin if 
				else{
					Driver_USART1.Receive(PAQUET_INUTILE,4);
					osSignalWait(0x0001, osWaitForever);}
				
				Allumer_1LED(1); //témoin pour la recep d'une partie
				etat=3;
				break;
			}//fin case 2
			
			case 3 : //relancement et passage au traitement
			{
//				reset_lidar();
				osDelay(2);
				osSignalSet(IDtache_traitement, 0x0002);
				//etat=0;	
				
				break;
			}//fin case 3
		}//fin switch	
		
	}//fin while
}//fin tache 1






void tache_traitement (void const *argument)
{
	int i;
	colissimo *ptr2, val_retour;
	osEvent EVretour;
	char affichage[25];
	while(1)	
	{
		EVretour = osMailGet(ID_BAL,1000);
		ptr2 = EVretour.value.p;
		val_retour = *ptr2;
		osMailFree(ID_BAL, ptr2);
		sprintf(affichage,"%5d deg %5d mm",(val_retour.angle>>6), (val_retour.dist>>2));
		GLCD_DrawString(1,10,affichage);	
		osSignalWait(0x0002, osWaitForever);
		Allumer_1LED(2);
		
		
		
		osDelay(osWaitForever);
		
	
	}//fin while

}//fin tache 2

osThreadDef(tache_Lidar_On, osPriorityAboveNormal,1,0);
osThreadDef(tache_traitement, osPriorityNormal,1,0);

/*
 * main: initialize and start the system
 */
int main (void) {
  
	osKernelInitialize ();                    // initialize CMSIS-RTOS
	Initialise_GPIO();
	
 	Init_UART_Lidar();

	GLCD_Initialize(); // initialisation LCD
	GLCD_ClearScreen();
	GLCD_SetFont(&GLCD_Font_16x24); 
	
//	LPC_PINCON->PINSEL4 |= 1<<20; // Choix de P2.10 comme EINT0
//	LPC_SC->EXTMODE = 1; // EINT0 sensible niveau
//	LPC_SC->EXTPOLAR = 1; // montant
//	NVIC_SetPriority (EINT0_IRQn , 3) ; // priorite EINT0
//	NVIC_EnableIRQ(EINT0_IRQn) ; // validation IT pour EINT0
	IDtache_Lidar_On = osThreadCreate(osThread(tache_Lidar_On), NULL);
	IDtache_traitement = osThreadCreate(osThread(tache_traitement), NULL);
	ID_BAL=osMailCreate(osMailQ(BAL),NULL);

  osKernelStart ();                         // start thread execution 
	osDelay(osWaitForever);
	return 0;
}

void uart_cb(uint32_t event)
	{
	switch(event){
		case ARM_USART_EVENT_RECEIVE_COMPLETE:
			osSignalSet(IDtache_Lidar_On, 0x0001);
			break;
	}
}
	
