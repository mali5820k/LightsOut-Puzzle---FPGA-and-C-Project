/*****************************************************************//**
 * @file main_sampler_test.cpp
 *
 * @brief Basic test of nexys4 ddr mmio cores
 *
 * @author p chu
 * @version v1.0: initial release
 *********************************************************************/

// #define _DEBUG
#include "../drv/chu_init.h"
#include "../drv/gpio_cores.h"
#include "../drv/xadc_core.h"
#include "../drv/sseg_core.h"
#include "../drv/spi_core.h"
#include "../drv/i2c_core.h"
#include "../drv/ps2_core.h"
#include "../drv/ddfs_core.h"
#include "../drv/adsr_core.h"
#include "../drv/lfsr_core.h"

GpoCore led(get_slot_addr(BRIDGE_BASE, S2_LED));
GpiCore sw(get_slot_addr(BRIDGE_BASE, S3_SW));
XadcCore adc(get_slot_addr(BRIDGE_BASE, S5_XDAC));
PwmCore pwm(get_slot_addr(BRIDGE_BASE, S6_PWM));
DebounceCore btn(get_slot_addr(BRIDGE_BASE, S7_BTN));
SsegCore sseg(get_slot_addr(BRIDGE_BASE, S8_SSEG));
SpiCore spi(get_slot_addr(BRIDGE_BASE, S9_SPI));
I2cCore adt7420(get_slot_addr(BRIDGE_BASE, S10_I2C));
Ps2Core ps2(get_slot_addr(BRIDGE_BASE, S11_PS2));
DdfsCore ddfs(get_slot_addr(BRIDGE_BASE, S12_DDFS));
AdsrCore adsr(get_slot_addr(BRIDGE_BASE, S13_ADSR), &ddfs);
LfsrCore lfsr(get_slot_addr(BRIDGE_BASE, S4_USER));
TimerCore timer(get_slot_addr(BRIDGE_BASE, S0_SYS_TIMER));

// btnu : btn(0);
// btnr : btn(1);
// btnd : btn(2);
// btnl : btn(3);
// btnc : btn(4);

// Constants for btn index lookup
#define btnci 4
#define btndi 2
#define btnui 0




// Global variables:
int start = 0; // Flag for determining the game has begun
int hasSeed = 0; // Flag for already received random seed

// Function prototypes:
void turnOffDisplay();
void forcedWin();
void forcedWinSw0();
void showPattern();
void playWinningPattern();


// A utility method to turn off the sseg display
void turnOffDisplay() {
	// turn all segments off:
	uint8_t result[8] = {0};

	// Assign all converted hex values to the result array
	// for writing to the SSD
	result[0] = 0xff;
	result[1] = 0xff;
	result[2] = 0xff;
	result[3] = 0xff;

	// Turn off the decimal points
	uint8_t val = 0;
	sseg.set_dp(val);

	// Write the whole hex number to the SSD
	sseg.write_8ptn(result);
}

// Turns all the segments off
void forcedWin() {
	turnOffDisplay();
	playWinningPattern();
}

void forcedWinSw0() {
	turnOffDisplay();
}

// Displays the current puzzle to the seven segment display:
void showPattern(uint32_t &data) {
	uint8_t pattern[8] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

	// format of bit_writes is: top    seg
	//						    middle seg
	//						    bottom seg
	bit_write(pattern[1], 0, bit_read(data, 2)); // Segment D 0
	bit_write(pattern[1], 6, bit_read(data, 1)); // Segment G 1
	bit_write(pattern[1], 3, bit_read(data, 0)); // Segment A 2

	bit_write(pattern[2], 0, bit_read(data, 5)); // Segment D 3
	bit_write(pattern[2], 6, bit_read(data, 4)); // Segment G 4
	bit_write(pattern[2], 3, bit_read(data, 3)); // Segment A 5

	bit_write(pattern[3], 0, bit_read(data, 8)); // Segment D 6
	bit_write(pattern[3], 6, bit_read(data, 7)); // Segment G 7
	bit_write(pattern[3], 3, bit_read(data, 6)); // Segment A 8

	sseg.set_dp(0); // Remove decimals from the seven segment display
	sseg.write_8ptn(pattern); // Display the current pattern
}

// Display the winning pattern
void playWinningPattern() {
	// Need to assign data to turn on the appropriate segments
	uint32_t data5 = 0xFFFFFFFE; // light  9
	uint32_t data4 = 0xFFFFFFF5; // lights 6, 8
	uint32_t data3 = 0xFFFFFFAB; // lights 3, 5, 7
	uint32_t data2 = 0xFFFFFF5F; // lights 2, 4
	uint32_t data1 = 0xFFFFFEFF; // light  1

	while(1) {

		uart.disp("\nPrinting data1\n");
		showPattern(data1);
		if(btn.read_db(btnci) || btn.read_db(btndi)) {
			start = 1;
			turnOffDisplay();
			uart.disp("\nPressed start or reset button\n");
			return;
		}
		timer.sleep(500000); // 0.5 seconds delay

		if(btn.read_db(btnci) || btn.read_db(btndi)) {
			start = 1;
			turnOffDisplay();
			uart.disp("\nPressed start or reset button\n");
			return;
		}

		uart.disp("\nPrinting data2\n");
		showPattern(data2);

		if(btn.read_db(btnci) || btn.read_db(btndi)) {
			start = 1;
			turnOffDisplay();
			uart.disp("\nPressed start or reset button\n");
			return;
		}

		timer.sleep(500000);

		if(btn.read_db(btnci) || btn.read_db(btndi)) {
			start = 1;
			turnOffDisplay();
			uart.disp("\nPressed start or reset button\n");
			return;
		}

		uart.disp("\nPrinting data3\n");
		showPattern(data3);

		if(btn.read_db(btnci) || btn.read_db(btndi)) {
			start = 1;
			turnOffDisplay();
			uart.disp("\nPressed start or reset button\n");
			return;
		}

		timer.sleep(500000);

		if(btn.read_db(btnci) || btn.read_db(btndi)) {
			start = 1;
			turnOffDisplay();
			uart.disp("\nPressed start or reset button\n");
			return;
		}

		uart.disp("\nPrinting data4\n");
		showPattern(data4);

		if(btn.read_db(btnci) || btn.read_db(btndi)) {
			start = 1;
			turnOffDisplay();
			uart.disp("\nPressed start or reset button\n");
			return;
		}

		timer.sleep(500000);

		if(btn.read_db(btnci) || btn.read_db(btndi)) {
			start = 1;
			turnOffDisplay();
			uart.disp("\nPressed start or reset button\n");
			return;
		}

		uart.disp("\nPrinting data5\n");
		showPattern(data5);

		if(btn.read_db(btnci) || btn.read_db(btndi)) {
			start = 1;
			turnOffDisplay();
			uart.disp("\nPressed start or reset button\n");
			return;
		}

		timer.sleep(500000);

		if(btn.read_db(btnci) || btn.read_db(btndi)) {
			start = 1;
			turnOffDisplay();
			uart.disp("\nPressed start or reset button\n");
			return;
		}

	}
}

// main function including changes proposed by Zach Monte on 03/23/2021

int main() {
	init_fix();
	uart.disp("\nStarted Program\n");

	// Switch inputs needed:
	int sw0, sw1, sw2, sw3, sw4, sw5, sw6, sw7, sw8, sw9;

	// Reference for storing the current LFSR value:
	uint32_t data;

	// Initially turn off all segments
	turnOffDisplay();

	// Then start the LFSR number generator
	lfsr.generate();

	while(1) {

		sw0 = sw.read(0);
		sw1 = sw.read(1);
		sw2 = sw.read(2);
		sw3 = sw.read(3);
		sw4 = sw.read(4);
		sw5 = sw.read(5);
		sw6 = sw.read(6);
		sw7 = sw.read(7);
		sw8 = sw.read(8);
		sw9 = sw.read(9);

		// Start the game with the current permutation
		if(btn.read_db(btnci) && start == 0) {

			uart.disp("\nStarting current permutation\n");
			uart.disp((int)data);
			start = 1;
		}
		if(start) {
			int forcedWinCounter = 0;
			if(hasSeed == 0) {
				data = lfsr.read();
				hasSeed = 1;
			}
			showPattern(data);

			// Check if we've won
			if((data & 0x000001FF) == 0x000001FF) {
				start = 0;
				forcedWin();
			}

			// Force the win:
			if(btn.read_db(btnui)) {
				turnOffDisplay();
				start = 0;
				hasSeed = 0;
				forcedWin();
				forcedWinCounter = 1;

			}

			// Reset button
			else if(btn.read_db(btndi)) {
				turnOffDisplay();
				start = 0;
				hasSeed = 0;
				forcedWinCounter = 0;
			}

			// Ask about whether or not we still need sw0
			else if (sw0) {
				while(1) {
					sw0 = sw.read(0);
					if (forcedWinCounter == 0) {
						forcedWinSw0();
						hasSeed = 0;
						forcedWinCounter = 1;
						start = 0;
					}
					if(sw0 == 0) {
						forcedWinCounter = 0;
						break;
					}
				}

			}

			// Invert the corresponding segments
			else if(sw1){
				uart.disp("\nJust pressed sw1\n");
				while(1) {
					uart.disp("\nSw1 is still 1\n");
					if(sw.read(1) == 0) {
						uart.disp("\nSw1 is now 0\n");
						bit_toggle(data, 8); // toggle LED1
						bit_toggle(data, 7); // toggle LED2
						bit_toggle(data, 5); // toggle LED4
						showPattern(data);
						break;
					}
				}
				uart.disp("\nBroke out of while loop for Sw1\n");
			}

			else if(sw2) {
				uart.disp("\nJust pressed sw2\n");
				while(1) {
					uart.disp("\nSw2 is still 1\n");
					if(sw.read(2) == 0) {
						uart.disp("\nSw2 is now 0\n");
						bit_toggle(data, 7); // toggle LED2
						bit_toggle(data, 8); // toggle LED1
						bit_toggle(data, 4); // toggle LED5
						bit_toggle(data, 6); // toggle LED3
						showPattern(data);
						break;
					}
				}
				uart.disp("\nBroke out of while loop for Sw2\n");

			}

			else if(sw3) {
				uart.disp("\nJust pressed sw3\n");
				while(1) {
					uart.disp("\nSw3 is still 1\n");
					if(sw.read(3) == 0) {
						uart.disp("\nSw3 is now 0\n");
						bit_toggle(data, 6); // toggle LED3
						bit_toggle(data, 7); // toggle LED2
						bit_toggle(data, 3); // toggle LED6
						showPattern(data);
						break;
					}
				}
				uart.disp("\nBroke out of while loop for Sw3\n");
			}

			else if(sw4) {
				uart.disp("\nJust pressed sw4\n");
				while(1) {
					uart.disp("\nSw4 is still 1\n");
					if(sw.read(4) == 0) {
						uart.disp("\nSw4 is now 0\n");
						bit_toggle(data, 5); // toggle LED4
						bit_toggle(data, 8); // toggle LED1
						bit_toggle(data, 4); // toggle LED5
						bit_toggle(data, 2); // toggle LED7
						showPattern(data);
						break;
					}
				}
				uart.disp("\nBroke out of while loop for Sw4\n");
			}

			else if(sw5) {
				uart.disp("\nJust pressed sw5\n");
				while(1) {
					uart.disp("\nSw5 is still 1\n");
					if(sw.read(5) == 0) {
						uart.disp("\nSw5 is now 0\n");
						bit_toggle(data, 4); // toggle LED5
						bit_toggle(data, 7); // toggle LED2
						bit_toggle(data, 3); // toggle LED6
						bit_toggle(data, 1); // toggle LED8
						bit_toggle(data, 5); // toggle LED4
						showPattern(data);
						break;
					}
				}
				uart.disp("\nBroke out of while loop for Sw5\n");
			}

			else if(sw6) {
				uart.disp("\nJust pressed sw6\n");
				while(1) {
					uart.disp("\nSw6 is still 1\n");
					if(sw.read(6) == 0) {
						uart.disp("\nSw6 is now 0\n");
						bit_toggle(data, 3); // toggle LED6
						bit_toggle(data, 6); // toggle LED3
						bit_toggle(data, 4); // toggle LED5
						bit_toggle(data, 0); // toggle LED9
						showPattern(data);
						break;
					}
				}
				uart.disp("\nBroke out of while loop for Sw6\n");
			}

			else if(sw7) {
				uart.disp("\nJust pressed sw7\n");
				while(1) {
					uart.disp("\nSw7 is still 1\n");
					if(sw.read(7) == 0) {
						uart.disp("\nSw7 is now 0\n");
						bit_toggle(data, 2); // toggle LED7
						bit_toggle(data, 1); // toggle LED8
						bit_toggle(data, 5); // toggle LED4
						showPattern(data);
						break;
					}
				}
				uart.disp("\nBroke out of while loop for Sw8\n");
			}

			else if(sw8) {
				uart.disp("\nJust pressed sw8\n");
				while(1) {
					uart.disp("\nSw8 is still 1\n");
					if(sw.read(8) == 0) {
						uart.disp("\nSw8 is now 0\n");
						bit_toggle(data, 1); // toggle LED8
						bit_toggle(data, 2); // toggle LED7
						bit_toggle(data, 4); // toggle LED5
						bit_toggle(data, 0); // toggle LED9
						showPattern(data);
						break;
					}
				}
				uart.disp("\nBroke out of while loop for Sw8\n");
			}

			else if(sw9) {
				uart.disp("\nJust pressed sw9\n");
				while(1) {
					uart.disp("\nSw9 is still 1\n");
					if(sw.read(9) == 0) {
						uart.disp("\nSw9 is now 0\n");
						bit_toggle(data, 0); // toggle LED9
						bit_toggle(data, 3); // toggle LED6
						bit_toggle(data, 1); // toggle LED8
						showPattern(data);
						break;
					}
				}
				uart.disp("\nBroke out of while loop for Sw9\n");

			}
		}

	}


}

