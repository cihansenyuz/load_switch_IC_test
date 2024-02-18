#include "display_funcs.h"

/* 
* Prints defected IC message on the display with a test number indicator
*
* @param testNumber test number as string
* @return none
*/
void errorMessage(char* testNumber)
{
	displayClear();
	displayMessage("IC failed!");
	displayNewLine();
	displayMessage("Test #");
	displayMessage(testNumber);
}

/* 
* Prints no supply message on the display with a test number indicator
*
* @param testNumber test number as string
* @return none
*/
void supplyErrorMessage(char* testNumber)
{
	displayClear();
	displayMessage("No IC Supply!");
	displayNewLine();
	displayMessage("Test #");
	displayMessage(testNumber);
}

/* 
* Prints defected relay message on the display with a test number indicator
*
* @param testNumber test number as string
* @return none
*/
void relayErrorMessage(char* testNum)
{
	displayClear();
	displayMessage("Relay NOK!");
	displayNewLine();
	displayMessage("Test: ");
	displayMessage(testNum);
}

/* 
* Prints test number on the display
*
* @param testNumber test number as string
* @return none
*/
void printTest(char* testNumber)
{
	displayClear();
	displayMessage("Test #");
	displayMessage(testNumber);
}

/* 
* Indicates test begining on the display counting from 3 to 0.
*
* @param none
* @return none
*/
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

/* 
* Prints successful test message on the display indicating test number.
*
* @param testNumber test number as string
* @return none
*/
void successMessage(char* testNumber)
{
	displayClear();
	displayMessage("Test Done! IC OK!");
	displayNewLine();
	displayMessage("Test #");
	displayMessage(testNumber);
}
