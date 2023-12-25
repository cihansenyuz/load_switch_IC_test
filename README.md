# load_switch_IC_test
STM32F103C8 MCU controlled USB load test for 5V load switch IC on a TV mainboard. It replicates some USB port load insertion (such as USB harddrive) and ejection. How long the connection will be set can be configured.

This project uses my own libraries and some standard libraries for STM32F103C6.
This project is designed in a modular way to be set for different test needs.

Before starting the test, must follow the below instructions list
- Set TOTAL_TEST, LOAD_DURITION, and UNLOAD_DURITION for example; 500 test cycles for 500 milisecs of USB load plugged, and for 300 milisecs of USB load unplugged.
- Build the project and upload it to STM32.
- Prepare the setup by doing all hardware connections. Use the "Load_switch_IC_schematic.pdf" file for reference.
- Once the hardware setup connection is ready, power it to run the test.

Project photos, schematics, and flow charts can be found in the "Project Documentation" folder.

Cihan Şenyüz 25.12.2023
