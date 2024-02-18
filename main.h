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

void setCounterLoaded(int);
int checkDevice(void);
int checkDeviceSupply(void);
int checkRelay(void);
void configSetup(void);

void EXTI0_IRQHandler(void);
void TIM1_UP_IRQHandler(void);
void updateTestStatus(void);
void relayControl(void);
