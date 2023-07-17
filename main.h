
/* ----- SETTINGS ----*/

#define TOTAL_TEST 1000  				/* set it up to 4,294,967,295 */
#define LOAD_DURITION 15 			/* set it in 100 miliseconds */
#define UNLOAD_DURITION 3			/* set it in 100 miliseconds */

/* ------------------ */

void setCounterLoaded(int);
int checkDevice(void);
int checkDeviceSupply(void);
int checkRelay(void);
void configSetup(void);

void EXTI0_IRQHandler(void);
void TIM1_UP_IRQHandler(void);
void timer100msControl(void);
/*void timer10msControl(void);*/
void relayControl(void);
