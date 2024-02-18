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

void supplyErrorMessage(char*);
void relayErrorMessage(char*);
void printTest(char*);
void startMessage(void);
void successMessage(char*);
void errorMessage(char*);
