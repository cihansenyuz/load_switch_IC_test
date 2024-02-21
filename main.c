#include "main.h"

static char quitSignal = 0;
static int counterLoaded = 0;		/* to count load durition */
static int counterUnloaded = 0;		/* to count unload durition */
static int counterErrorRelay = 0;
static char flag100ms = 0;			/* to count each 100 ms */
static char flagTestDone = 0;		/* indicates test is over, analysis may begin */

int main(void)
{
	configSetup();
	startMessage();
	
	unsigned int currentTestNo = 1;
	unsigned int totalTestNo = TOTAL_TEST;
	char* stringTestNo;				/* to print test no on the display */
	char flagNewTest = 1;			/* indicates begining of a new test cycle */
	
	while(1)
	{
		if(flag100ms >= 1)
		{
			flag100ms = 0;
			updateTestStatus();	/* update load/unload durition left each 100 ms */
			if(currentTestNo <= totalTestNo)
			{
				if(flagNewTest) 	/* if new test starts, then update LCD */
				{
					flagNewTest = 0;
					stringTestNo = int2char(currentTestNo); /* convert to string cause 1602 needs string */
					printTest(stringTestNo);
				}
				setCounterLoaded(LOAD_DURITION); /* since new test started, begin load durition */			
				if(flagTestDone)
				{
					/* test cycle (load, unload) is done, analysis (checks) begins */
					flagTestDone = 0;
					/* ------- Relay Check ------ */
					if(checkRelay() == 0) /* check relay if it returns back initial state */
					{
						relayErrorMessage(stringTestNo);
						return 0;				
					}
					/* ------- Device & Supply Check ------ */
					if(checkDeviceSupply() == 1) /* check whether DUT has proper supply */
					{
						if(checkDevice() == 0) /* DUT has supply, so now check device health */
						{
							errorMessage(stringTestNo);
							return 0;
						}
					}
					else /* no supply case */
					{
						supplyErrorMessage(stringTestNo);
						return 0;
					}
					
					if(quitSignal) /* user quit button check */
						break;
					currentTestNo++;
					flagNewTest = 1;
					free(stringTestNo); /* into2char function allocate memory each time it s called. Need to be deallocated */
				}
			}
			else /* test number reached to target test number */
			{
				successMessage(int2char(currentTestNo-1));
				return 0;
			}
			relayControl(); /* handle relay position */
		}
	}
	displayMessage("QUITED");
	return 0;
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
	delayMS(30); /* to prevent reading much earlier than output react */
	
	if(read_GP(PA,3) == 1) /* reads Vout of DUT, if High, Vin and Vout is short circuited inside DUT */
		return 0;
	write_GP(PA,2,HIGH); /* enable DUT */
	return 1;
}


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
	{
		counterErrorRelay++;
		if(counterErrorRelay == 3) /* decide error if fails 3 times in a row */
			return 0;
		else
			return 1;
	}
	counterErrorRelay = 0;	/* reset error counter if test is successfull */
	return 1;
}

/* 
* Configurates and initiliazes GPIOs, GPIO interrupt, I2C, timer interrupt and display.
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
	timer_interrupt_start_ms(timer1,10);
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

/* 
* Handles TIMER interrupt. It is from STM32 library.
*
* @param none
* @return none
*/
void TIM1_UP_IRQHandler(void)
{
	timer_interrupt_flag_reset(timer1);
	counter10ms++;
	if(counter10ms >= 10 ) 			/* count until 10 to rise 100ms flag */
	{
		counter10ms = 0;
		flag100ms = 1;
	}
}

/* 
* Checks if all test and analysis is done, then sets load durition counter to start new cycle
*
* @param count how many ms to load the IC
* @return none
*/
void setCounterLoaded(int count)
{
	if(counterLoaded == 0 && counterUnloaded == 0 && flagTestDone == 0) /* set counterLoaded if all proceses are over which means beginning of a new test cycle */
		counterLoaded = count;
}

/* 
* When called after each 100 ms, updates current status of load and unload counters. If load durition is done, starts unload durition. If unload durition is done, raises tests are done flag. Catch the flag to begin analysis.
*
* @param none
* @return none
*/
void updateTestStatus(void)
{
	if(counterLoaded > 0)			/* if process is in load durition, decrease load counter */
	{
		counterLoaded--;
		if(counterLoaded <= 0)		/* load durition is over, start unload durition */
			counterUnloaded = UNLOAD_DURITION +1; /* +1 to balance -- in the next if{} */
	}
	if(counterUnloaded > 0)			/* if process is in unload durition, decrease unload counter */
	{
		counterUnloaded--;
		if(counterUnloaded <= 0)	/* unload durition is over, raise the test done flag! */
			flagTestDone = 1;
	}
}

/* 
* Handles current status of the relay. If process is load or unload, loads or unloads the IC respectively.
*
* @param none
* @return none
*/
void relayControl(void)
{
	if(counterLoaded > 0)			/* if process is in load durition, load the output via relay */
		write_GP(PA,1,LOW);
	else if(counterUnloaded > 0)	/* if process is in unload durition, unload the output via relay */
		write_GP(PA,1,HIGH);
}

