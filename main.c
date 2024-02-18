#include "main.h"

static char quitSignal = 0;
static int counterLoaded = 0;
static int counterUnloaded = 0;
static int counter10ms = 0;
static int counterErrorRelay = 0;
static char flag100ms = 0;
static char flagTestDone = 0;

int main(void)
{
	configSetup();
	startMessage();
	
	unsigned int currentTestNo = 1;
	unsigned int totalTestNo = TOTAL_TEST;
	char* stringTestNo;			/* to print test no on the display */
	char flagNewTest = 1;
	
	while(1)
	{
		if(flag100ms >= 1)
		{
			flag100ms = 0;
			timer100msControl();
			if(currentTestNo <= totalTestNo)
			{
				if(flagNewTest) /* if new test starts, then update LCD */
				{
					flagNewTest = 0;
					stringTestNo = int2char(currentTestNo); /* convert to string cause 1602 needs string */
					printTest(stringTestNo);
				}
				setCounterLoaded(LOAD_DURITION);				
				if(flagTestDone)
				{
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
					else
					{
						supplyErrorMessage(stringTestNo);
						return 0;
					}
					
					if(quitSignal)
						break;
					currentTestNo++;
					flagNewTest = 1;
					free(stringTestNo); /* into2char function allocate memory each time it s called. Need to be deallocated */
				}
			}
			else
			{
				successMessage(int2char(currentTestNo-1));
				return 0;
			}
			relayControl();
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


void setCounterLoaded(int count)
{
	if(counterLoaded == 0 && counterUnloaded == 0 && flagTestDone == 0) /* set counterLoaded if all proceses are over */
		counterLoaded = count;
}

void timer100msControl(void)
{
	if(counterLoaded > 0)
	{
		counterLoaded--;
		if(counterLoaded <= 0)
			counterUnloaded = UNLOAD_DURITION +1; /* +1 to balance -- in the next if{} */
	}
	if(counterUnloaded > 0)
	{
		counterUnloaded--;
		if(counterUnloaded <= 0)
		{
			flagTestDone = 1;
		}
	}
}

void relayControl(void)
{
	if(counterLoaded > 0)
		write_GP(PA,1,LOW); 		/* load the output via relay */
	else if(counterUnloaded > 0)
		write_GP(PA,1,HIGH); 		/* unload the output via relay */
}

