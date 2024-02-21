/**
  ******************************************************************************
  * @file    main.h
  * @author  Cihan Senyuz
  * @brief   Header for main.c file.
  *                 This file contains the common defines of the application.
  * 
  *     STM32F103C8 MCU controlled USB load test for 5V load switch IC on a TV mainboard.
  * It replicates some USB port load insertion (such as USB harddrive) and ejection. How
  * long the connection will be set can be configured.
  *
  * This project uses my own libraries and some standard libraries for STM32F103C6. This
  * project is designed in a modular way to be set for different test needs.
  *
  * Before starting the test, must follow the below instructions list
  *
  * - Set TOTAL_TEST, LOAD_DURITION, and UNLOAD_DURITION for example; 1000 test cycles for
  * 1500 milisecs of USB load plugged, and for 300 milisecs of USB load unplugged.
  * - Build the project and upload it to STM32.
  * - Prepare the setup by doing all hardware connections. Use the
  * "Load_switch_IC_schematic.pdf" file on the GitHub for your reference.
  * - Once the hardware setup connection is ready, power it to run the test.
  * 
  * @note   Source code depends on stm32f10x.h, gp_driver.h, display_funcs.h,
  * and timer.h libraries
  ******************************************************************************
  */

#include "display_funcs.h"  /* also includes "1602_i2c_driver.h" "help_func.h" "systick_delay.h" */
#include "stm32f10x.h"      /* we need it for GPIO interrupt */
#include "gp_driver.h"
#include "timer.h"

/* -------------------- TEST SETTINGS --------------------*/
#define TOTAL_TEST 1000  		/* set it up to 4,294,967,295 */
#define LOAD_DURITION 15 		/* set it in 100 miliseconds  */
#define UNLOAD_DURITION 3		/* set it in 100 miliseconds  */
/* ------------------------------------------------------ */

/* 
* Checks if all test and analysis is done, then sets load durition counter to start new cycle
*
* @param count how many ms to load the IC
* @return none
*/
void setCounterLoaded(int count);

/* 
* Tests the IC whether it is still operational or not
*
* @param none
* @return 1: operational, 0: defected
*/
int checkDevice(void);

/* 
* Checks if the IC supply is present.
*
* @param none
* @return 1: supplied, 0: no supply
*/
int checkDeviceSupply(void);

/* 
* Controls the relay whether it is still working properly.
*
* @param none
* @return 1: operational, 0: sticked to the last position
*/
int checkRelay(void);

/* 
* Configurates and initiliazes GPIOs, GPIO interrupt, I2C, timer interrupt and display.
*
* @param none
* @return none
*/
void configSetup(void);


/* 
* Handles GPIO interrupt. It is from STM32 library.
*
* @param none
* @return none
*/
void EXTI0_IRQHandler(void);

/* 
* Handles TIMER interrupt. It is from STM32 library.
*
* @param none
* @return none
*/
void TIM1_UP_IRQHandler(void);

/* 
* When called after each 100 ms, updates current status of load and unload counters. If load durition is done, starts unload durition. If unload durition is done, raises tests are done flag. Catch the flag to begin analysis.
*
* @param none
* @return none
*/
void updateTestStatus(void);

/* 
* Handles current status of the relay. If process is load or unload, loads or unloads the IC respectively.
*
* @param none
* @return none
*/
void relayControl(void);
