/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "bibli_lidar.h"
#include "GPIO.h"
#include "GLCD_Config.h"                // Keil.MCB1700::Board Support:Graphic LCD
#include "Board_GLCD.h"                 // ::Board Support:Graphic LCD
#include "stdio.h"
#include "Driver_USART.h"               // ::CMSIS Driver:USART


//osMailQId ID_BAL_ANGLE;
//osMailQDef(BAL_ANGLE,1000,short);
//osMailQId ID_BAL_DIST;
//osMailQDef(BAL_DIST,1000,short);

extern GLCD_FONT GLCD_Font_6x8;
extern GLCD_FONT GLCD_Font_16x24;

extern ARM_DRIVER_USART Driver_USART1;

char DONNEE_LIDAR[10000];


//char DONNEE_LIDAR[10000];

osThreadId IDtache_Lidar_On;
osThreadId IDtache_check;
osThreadId IDtache_reception;
//osThreadId IDtache_traitement;

void tache_Lidar_On (void const *argument);
void tache_check (void const *argument);
void tache_reception (void const *argument);
//void tache_traitement (void const *argument);

//void EINT0_IRQHandler (void)
//{
//	osSignalSet(IDtache_Lidar_On, 0x0002);	//Mise à 1 de l’Event 2 de la tache 1 
//	LPC_SC->EXTINT = 1; // drapeau baissé
//}

void tache_Lidar_On (void const *argument)
{
	while(1)
	{
		pwm(85);
		laser_on();
		osDelay(10000);
	}
}//fin tache 1

void tache_check (void const * argument)
{
	short i;
	char DEBUT_LIDAR[7];
	char TAB_COMPAR_LIDAR_DEBUT[7] = {0xA5 , 0x5A, 0x05, 0x00, 0x00, 0x40, 0x81};
	while(1)
	{
		Driver_USART1.Receive(DEBUT_LIDAR,7);
		osSignalWait(0x0001, osWaitForever);
		//Allumer_1LED(0);
		
		/*stockage de la valeur de reception et comparaison avec le test*/
		for (i=0; i<7; i++){
		if (TAB_COMPAR_LIDAR_DEBUT[i] == DEBUT_LIDAR[i]){
					//traitement;
			//GLCD_DrawString(1 ,1,DEBUT_LIDAR);
			Allumer_1LED(0); //temoin que la trame est bonne
			osSignalSet(IDtache_reception, 0x0001);
		}
		else {
					//erreur;
		}
	}
		osDelay(osWaitForever);
	}
}//fin tache 2

void tache_reception (void const *argument)
{
//	char *ptrAngle, *ptrDist;
	int i;
	//short tab_angle[1000];
	//short tab_dist[1000];
	while(1)
	{
		Driver_USART1.Receive(DONNEE_LIDAR,10000);
		osSignalWait(0x0001, osWaitForever);
		Allumer_1LED(1); //témoin pour la recep du tableau total
//		for(i = 0; i < 10000;i++)
//		{
//			if(i%5 > 50) 
//			{
//				tab_angle[i] = (DONNEE_LIDAR[i+2]<<7) || (DONNEE_LIDAR[i+1] >> 1);
//				tab_dist[i] = (DONNEE_LIDAR[i+4]<<8) || (DONNEE_LIDAR[i+3]);
//			}
//			
//		}
//		
//		/*Remplissage des tableaux*/
//		ptrAngle=osMailAlloc(ID_BAL_ANGLE,1000);
//		ptrDist=osMailAlloc(ID_BAL_DIST,1000);
//		
//		/*Mise dans la BAL*/
//		for(i=0;i<1000;i++)
//		{
//			*ptrAngle=tab_angle[i];
//			*ptrDist=tab_dist[i];
//			osMailPut(ID_BAL_ANGLE,ptrAngle);
//			osMailPut(ID_BAL_DIST,ptrDist);
//		}
//		
		osDelay(osWaitForever);
		
	}//fin while
}//fin tache 3

//void tache_traitement (void const *argument)
//{
//	char Dist[15];
//	short *recepAngle;
//	short *recepDist;
//	short tab_angle[1000];
//	short tab_dist [1000];
//	short i, j;
//	int moy_dist;
//	osEvent EVretourAngle;
//	osEvent EVretourDist;

//	while(1)
//	{
//		/*Reception*/
//		EVretourAngle=osMailGet(ID_BAL_ANGLE,osWaitForever);
//		EVretourDist=osMailGet(ID_BAL_DIST,osWaitForever);
//		recepAngle=EVretourAngle.value.p;
//		recepDist=EVretourDist.value.p;
//		for(i=0;i<1000;i++)
//		{
//			tab_angle[i] =  *(recepAngle+i);
//			tab_dist[i]  =  *(recepDist+i) ;
//		}
//		osMailFree(ID_BAL_ANGLE, recepAngle);
//  	osMailFree(ID_BAL_DIST, recepDist);
//		
//		/*traitement*/
//		
//		for(i=0;i<1000;i++)
//		{
//			if(i%40 == 0){
//				
//				for(j=0; j<40; j++){
//					moy_dist += tab_dist[i - j] ;
//					moy_dist /= 40;
//				}
//				sprintf(Dist, "distance = %d", moy_dist>>2);
//				GLCD_DrawString(1, 1+i, Dist);
//			}
//		}
//	}
//}//fin tache 4


osThreadDef(tache_Lidar_On, osPriorityAboveNormal,1,0);
osThreadDef(tache_check, osPriorityNormal,1,0);
osThreadDef(tache_reception, osPriorityBelowNormal,1,0);
//osThreadDef(tache_traitement, osPriorityNormal,1,0);
/*
 * main: initialize and start the system
 */
int main (void) {
  
	osKernelInitialize ();                    // initialize CMSIS-RTOS
	Initialise_GPIO();
	
 	Init_UART_Lidar();
	GLCD_Initialize(); // initialisation LCD
	GLCD_ClearScreen();
	GLCD_SetFont(&GLCD_Font_6x8); 
	
//	LPC_PINCON->PINSEL4 |= 1<<20; // Choix de P2.10 comme EINT0
//	LPC_SC->EXTMODE = 1; // EINT0 sensible niveau
//	LPC_SC->EXTPOLAR = 1; // montant
//	NVIC_SetPriority (EINT0_IRQn , 3) ; // priorite EINT0
//	NVIC_EnableIRQ(EINT0_IRQn) ; // validation IT pour EINT0
	IDtache_Lidar_On = osThreadCreate(osThread(tache_Lidar_On), NULL);
	IDtache_check = osThreadCreate(osThread(tache_check), NULL);
	IDtache_reception = osThreadCreate(osThread(tache_reception), NULL);
//	IDtache_traitement = osThreadCreate(osThread(tache_traitement), NULL);
//	ID_BAL_ANGLE=osMailCreate(osMailQ(BAL_ANGLE),NULL);
//  ID_BAL_DIST=osMailCreate(osMailQ(BAL_DIST),NULL);

  osKernelStart ();                         // start thread execution 
	osDelay(osWaitForever);
	return 0;
}



void uart_cb(uint32_t event)
	{
	switch(event){
		case ARM_USART_EVENT_RECEIVE_COMPLETE:
					osSignalSet(IDtache_check, 0x0001);
			break;
	}
}