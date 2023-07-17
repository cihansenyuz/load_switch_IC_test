#include "display_funcs.h"

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

void successMessage(char* testNumber)
{
	displayClear();
	displayMessage("Test Done! IC OK!");
	displayNewLine();
	displayMessage("Test #");
	displayMessage(testNumber);
}
