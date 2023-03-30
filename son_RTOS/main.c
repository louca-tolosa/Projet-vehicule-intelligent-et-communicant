/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "Driver_USART.h"               // ::CMSIS Driver:USART

extern ARM_DRIVER_USART Driver_USART1;

void Tache1 (void const*argument);
osThreadId  ID_Tache1;//Définition idebtifiant tâche
osThreadDef(Tache1,osPriorityNormal,1,0);//Définition caractéristique tâche
//void Tache2 (void const*argument);
//osThreadId  ID_Tache2;//Définition idebtifiant tâche
//osThreadDef(Tache2,osPriorityNormal,1,0);//Définition caractéristique tâche

void Send_DFP(char commande, char P1, char P2);
void Init_UART(void);

/*
 * main: initialize and start the system
 */
int main (void) {
  osKernelInitialize ();                    // initialize CMSIS-RTOS
	Init_UART();
	
	ID_Tache1=osThreadCreate(osThread(Tache1),NULL);
//	ID_Tache2=osThreadCreate(osThread(Tache2),NULL);
  // initialize peripherals here

  // create 'thread' functions that start executing,
  // example: tid_name = osThreadCreate (osThread(name), NULL);
	
		Send_DFP(0x09, 0x00, 0x01);
		osDelay(10);
		
		Send_DFP(0x10, 0x01, 0x0F);
		osDelay(10);

  osKernelStart ();                         // start thread execution 
	osDelay(osWaitForever);
	return 0;
}

void Tache1(void const * argument)
{
	
	while(1)
	{
		Send_DFP(0x03, 0x00, 0x01);
		Send_DFP(0x41, 0x00, 0x00);
		osDelay(5000);
		
		Send_DFP(0x01, 0x00, 0x00);
		osDelay(5000);
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