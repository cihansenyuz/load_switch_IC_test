#include "stm32f10x.h" /* we need it for GPIO interrupt */
#include "gp_driver.h"
#include "1602_i2c_driver.h" /* this header includes systick_delay.h */
#include "help_func.h"

/* ----- SETTINGS ----*/

#define TOTAL_TEST 3					/* set it up to 4,294,967,295 */
#define LOAD_DURITION 500 			/* set it in miliseconds */
#define UNLOAD_DURITION 300		/* set it in miliseconds */

/* ------------------ */

void loaded(unsigned int);
void unloaded(unsigned int);
int checkDevice(void);
int checkDeviceSupply(void);
int checkRelay(void);
void errorMessage(char*);
void supplyErrorMessage(char*);
void relayErrorMessage(char*);
void printTest(char*);
void startMessage(void);
void successMessage(char*);
void configSetup(void);
void EXTI0_IRQHandler(void);
static int quitSignal;

int main(void)
{
	configSetup();
	
	unsigned int currentTestNo;
	unsigned int totalTestNo = TOTAL_TEST;
	char* temp;
	int errorCounter = 0;
	
	startMessage();
	quitSignal = 0;
	
	for(currentTestNo=1; totalTestNo >= currentTestNo; currentTestNo++)
	{
		temp = int2char(currentTestNo);
		printTest(temp); /* convert to string cause 1602 needs string */
		loaded(LOAD_DURITION);
		unloaded(UNLOAD_DURITION);
		
		if(checkRelay() == 0) /* check relay if it returns back initial state */
		{
			errorCounter++;
			if(errorCounter == 3) /* decide error if fails 3 times in a row */
			{
				relayErrorMessage(temp);
				return 0;
			}				
		}
		else
			errorCounter = 0; /* reset error counter if test is successfull */
		
		if(checkDeviceSupply() == 1) /* check whether DUT has proper supply */
		{
			if(checkDevice() == 0) /* DUT has supply, so now check device health */
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
		
		free(temp); /* into2char function allocate memory each time it s called. Need to be deallocated */
		if(quitSignal)
			break;
	}
	successMessage(int2char(currentTestNo-1));
	unloaded(0); /* finish the test unloaded with no durition */
	return 0;
}

void loaded(unsigned int miliSec)
{
	write_GP(PA,1,LOW);
	delayMS(miliSec);
}

void unloaded(unsigned int miliSec)
{
	write_GP(PA,1,HIGH);
	if(miliSec > 0)
		delayMS(miliSec);
}

int checkDevice(void)
{
	write_GP(PA,2,LOW); /* disable DUT */
	delayMS(20); /* to prevent reading much earlier than output react */
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

int checkRelay(void)
{
	if(read_GP(PA,5) == 0) /* if PA5 is low, relay sticks to last position. */
		return 0;
	return 1;
}

void errorMessage(char* testNumber)
{
	displayClear();
	displayMessage("IC Arizalandi!");
	displayNewLine();
	displayMessage("Test #");
	displayMessage(testNumber);
}

void supplyErrorMessage(char* testNumber)
{
	displayClear();
	displayMessage("No IC Supply!");
	displayNewLine();
	displayMessage("Test #");
	displayMessage(testNumber);
}

void relayErrorMessage(char* testNum)
{
	displayClear();
	displayMessage("Relay NOK!");
	displayNewLine();
	displayMessage("Test: ");
	displayMessage(testNum);
}

void printTest(char* testNumber)
{
	displayClear();
	displayMessage("Test #");
	displayMessage(testNumber);
}

void startMessage(void)
{
	displayMessage("Starts in 3 secs");
	displayNewLine();
	displayMessage("3, ");
	delayMS(1000);
	displayMessage("2, ");
	delayMS(1000);
	displayMessage("1,");
	delayMS(1000);
	displayClear();
}	

void configSetup(void)
{
	gpio_init(PA,1,OUT50,OUT_GP_PP);
	gpio_init(PA,2,OUT50,OUT_GP_PP);
	gpio_init(PA,3,IN,IN_PP);
	gpio_init(PA,4,IN,IN_PP);
	gpio_init(PA,5,IN,IN_PP);
	GPIOA->ODR |= 0x18; /* set inputs 3,4 pull up */
	
	gpio_init(PB,0,IN,IN_PP);
	GPIOA->ODR |= 0x00; /* set it pull down input */
	RCC->APB2ENR |= 0x01; /* enable clock for AFIO */
	
	/* interrupt config */
	__disable_irq();
	AFIO->EXTICR[0] = 1; /* to select portB pin0 for interrupt */
	EXTI->IMR |= 0x01; /* disable interrupt mask for pin0 */
	EXTI->RTSR |= 0x01; /* enable rising edge trigger */
	NVIC_EnableIRQ(EXTI0_IRQn);
	__enable_irq();
	/* end of interrupt config */
	
	write_GP(PA,2,HIGH); /* begin DUT enabled */
	write_GP(PA,1,HIGH); /* begin to test unloaded */
	
	systick_init();
	i2c_init(2,i2c_FastMode);
	delayMS(40);
	displayInit();
}

void successMessage(char* testNumber)
{
	displayClear();
	displayMessage("Test Done! IC OK!");
	displayNewLine();
	displayMessage("Test #");
	displayMessage(testNumber);
}

void EXTI0_IRQHandler(void)
{
	quitSignal = 1;
	EXTI->PR |= 0x01; /* to reset interrupt */
}
