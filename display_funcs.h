/**
  ******************************************************************************
  * @file    display_funcs.h
  * @author  Cihan Senyuz
  * @brief   Header for display_funcs.c file.
  *                 This file contains the common defines of the application.
  * 
  * 
  * @note   Source code depends on 1602_i2c_driver.h and help_func.h libraries
  ******************************************************************************
  */

#include "1602_i2c_driver.h" /* this header includes systick_delay.h */
#include "help_func.h"

/* 
* Prints no supply message on the display with a test number indicator
*
* @param testNumber test number as string
* @return none
*/
void supplyErrorMessage(char* testNumber);

/* 
* Prints defected relay message on the display with a test number indicator
*
* @param testNumber test number as string
* @return none
*/
void relayErrorMessage(char* testNumber);

/* 
* Prints test number on the display
*
* @param testNumber test number as string
* @return none
*/
void printTest(char*);

/* 
* Indicates test begining on the display counting from 3 to 0.
*
* @param none
* @return none
*/
void startMessage(void);

/* 
* Prints successful test message on the display indicating test number.
*
* @param testNumber test number as string
* @return none
*/
void successMessage(char* testNumber);

/* 
* Prints defected IC message on the display with a test number indicator
*
* @param testNumber test number as string
* @return none
*/
void errorMessage(char* testNumber);
