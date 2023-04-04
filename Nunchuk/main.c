/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "GPIO_LPC17xx.h"               // Keil::Device:GPIO
#include "LPC17xx.h"                    // Device header
#include "PIN_LPC17xx.h"                // Keil::Device:PIN
#include "Driver_I2C.h"                 // ::CMSIS Driver:I2C
#include "Board_GLCD.h"                 // ::Board Support:Graphic LCD
#include "GLCD_Config.h"                // Keil.MCB1700::Board Support:Graphic LCD
#include "stdio.h"
#include "Driver_USART.h"               // ::CMSIS Driver:USART


#define registreCommand1	0xF0
#define registreCommand2	0x55
#define registreCommand3	0x00
#define SLAVE_I2C_ADDR	  0x52
#define registreX	0x00
#define registreY	0x01

extern ARM_DRIVER_I2C Driver_I2C0;
extern GLCD_FONT GLCD_Font_6x8;
extern GLCD_FONT GLCD_Font_16x24;
extern ARM_DRIVER_USART Driver_USART1;

osThreadId ID_tache1,ID_tache2,ID_tache3;

unsigned	int PosX;
unsigned	int PosY;	
uint8_t tab[10];

void tache1(void const* argument)
{
	uint8_t tab[10];
	while(1)
	{
		osSignalWait(0x0001,osWaitForever);
		tab[0] = registreCommand1;
		tab[1] = registreCommand2;
	
		// Ecriture vers registre esclave : START + ADDR(W) + 1W_DATA + 1W_DATA + STOP
		Driver_I2C0.MasterTransmit (SLAVE_I2C_ADDR , tab, 2, false);		// false = avec stop
		while (Driver_I2C0.GetStatus().busy == 1);	// attente fin transmission
		osSignalSet(ID_tache2,0x0001);
		osDelay(osWaitForever);
	}
}

void tache2(void const* argument)
{
	uint8_t tab[10];
	
	while(1)
	{	
		osSignalWait(0x0001,osWaitForever);
		
		tab[0] = registreCommand3;
		
		// Ecriture vers registre esclave : START + ADDR(W) + 1W_DATA + 1W_DATA + STOP
		Driver_I2C0.MasterTransmit (SLAVE_I2C_ADDR	, tab, 1, false);		// false = avec stop
		while (Driver_I2C0.GetStatus().busy == 1);	// attente fin transmission
		
		osDelay(2);
		osSignalSet(ID_tache3,0x0001);
		osDelay(50);
	}
}

void tache3(void const* argument)
{

	uint8_t tab[10];
	
	
	while(1)
	{
		osSignalWait(0x0001,osWaitForever);
		Driver_I2C0.MasterReceive (SLAVE_I2C_ADDR, tab,6, false);		// false = avec stop
		while (Driver_I2C0.GetStatus().busy == 1);	// attente fin transmission

		osDelay(10);
		osSignalSet(ID_tache2,0x0001);
		
	}
}

osThreadDef (tache1,osPriorityNormal,1,0);

osThreadDef (tache2,osPriorityNormal,1,0);

osThreadDef (tache3,osPriorityNormal,1,0);



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

/*
 * main: initialize and start the system
 */
int main (void) {
  osKernelInitialize ();                    // initialize CMSIS-RTOS

  // initialize peripherals here
	Init_I2C();
	Init_LCD();
	Timer_Init(100, 25000);
	Init_UART();
	GLCD_Initialize();
	GLCD_ClearScreen();
	GLCD_SetFont(&GLCD_Font_16x24);
	

	
  // create 'thread' functions that start executing,
  // example: tid_name = osThreadCreate (osThread(name), NULL);
	ID_tache1=osThreadCreate (osThread (tache1),NULL);
	ID_tache2=osThreadCreate (osThread (tache2),NULL);
	ID_tache3=osThreadCreate (osThread (tache3),NULL);

	
	
  osKernelStart ();                         // start thread execution 
	osSignalSet(ID_tache1,0x0001);
	osDelay(osWaitForever);
	return 0;
}


