#include "main.h"

static int quitSignal;

int main(void)
{
	configSetup();
	
	unsigned int currentTestNo;				/* counter for current test */
	unsigned int totalTestNo = TOTAL_TEST;	/* target test number */
	char* temp;								/* temporary string to send test number to the display */
	int errorCounter = 0;					/* to prevent false call */
	
	startMessage();
	quitSignal = 0;							/* Signal for quit test button */
	
	for(currentTestNo=1; totalTestNo >= currentTestNo; currentTestNo++)
	{
		temp = int2char(currentTestNo);
		printTest(temp); 				/* convert to string cause 1602 needs string */
		loaded(LOAD_DURITION);
		unloaded(UNLOAD_DURITION);
		
		if(checkRelay() == 0) 			/* check relay if it returns back initial state */
		{
			errorCounter++;
			if(errorCounter == 3) 		/* decide error if fails 3 times in a row */
			{
				relayErrorMessage(temp);
				return 0;
			}				
		}
		else
			errorCounter = 0; 			/* reset error counter if test is successfull */
		
		if(checkDeviceSupply() == 1) 	/* check whether DUT has proper supply */
		{
			if(checkDevice() == 0) 		/* DUT has supply, so now check device health */
			{
				errorMessage(int2char(currentTestNo));
				return 0;
			}
		}
		else
		{
			supplyErrorMessage(int2char(currentTestNo));
			return 0;
		}
		
		/* into2char function allocate memory each time it s called. Need to be deallocated */
		free(temp); 					
		if(quitSignal)
			break;
	}
	successMessage(int2char(currentTestNo-1));
	unloaded(0); 						/* finish the test unloaded with no durition */
	return 0;
}

/* 
* Loads the IC output switching the relay
*
* @param milisec how much durition need to be loaded
* @return none
*/
void loaded(unsigned int miliSec)
{
	write_GP(PA,1,LOW);
	delayMS(miliSec);
}

/* 
* Removes the load from the IC output switching the relay
*
* @param milisec how much durition need to kept unloaded
* @return none
*/
void unloaded(unsigned int miliSec)
{
	write_GP(PA,1,HIGH);
	if(miliSec > 0)
		delayMS(miliSec);
}

/* 
* Tests the IC whether it is still operational or not
*
* @param none
* @return 1: operational, 0: defected
*/
int checkDevice(void)
{
	write_GP(PA,2,LOW); /* disable DUT */
	delayMS(20); /* to prevent reading much earlier than output react */
	if(read_GP(PA,3) == 1) /* reads Vout of DUT, if High, Vin and Vout is short circuited inside DUT */
		return 0;
	write_GP(PA,2,HIGH); /* enable DUT */
	return 1;
}

/* 
* Checks if the IC supply is present.
*
* @param none
* @return 1: supplied, 0: no supply
*/
int checkDeviceSupply(void)
{
	if(read_GP(PA,4) == 0) /* if high, Vin of DUT is supplied */
		return 0;
	return 1;
}

/* 
* Controls the relay whether it is still working properly.
*
* @param none
* @return 1: operational, 0: sticked to the last position
*/
int checkRelay(void)
{
	if(read_GP(PA,5) == 0) /* if PA5 is low, relay sticks to last position. */
		return 0;
	return 1;
}

/* 
* Configurates and initiliazes GPIOs, GPIO interrupt, I2C and display.
*
* @param none
* @return none
*/
void configSetup(void)
{
	gpio_init(PA,1,OUT50,OUT_GP_PP);
	gpio_init(PA,2,OUT50,OUT_GP_PP);
	gpio_init(PA,3,IN,IN_PP);
	gpio_init(PA,4,IN,IN_PP);
	gpio_init(PA,5,IN,IN_PP);
	GPIOA->ODR |= 0x18; 			/* set inputs 3,4 pull up */
	
	gpio_init(PB,0,IN,IN_PP);
	GPIOA->ODR |= 0x00; 			/* set it pull down input */
	RCC->APB2ENR |= 0x01; 			/* enable clock for AFIO */
	
	/* interrupt config */
	__disable_irq();
	AFIO->EXTICR[0] = 1; 			/* to select portB pin0 for interrupt */
	EXTI->IMR |= 0x01; 				/* disable interrupt mask for pin0 */
	EXTI->RTSR |= 0x01; 			/* enable rising edge trigger */
	NVIC_EnableIRQ(EXTI0_IRQn);
	__enable_irq();
	/* end of interrupt config */
	
	write_GP(PA,2,HIGH); 			/* begin DUT enabled */
	write_GP(PA,1,HIGH); 			/* begin to test unloaded */
	
	systick_init();
	i2c_init(2,i2c_FastMode);
	delayMS(40);
	displayInit();
}

/* 
* Handles GPIO interrupt. It is from STM32 library.
*
* @param none
* @return none
*/
void EXTI0_IRQHandler(void)
{
	quitSignal = 1;
	EXTI->PR |= 0x01; 				/* to reset interrupt */
}
