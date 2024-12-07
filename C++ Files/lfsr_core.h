/**
 * File made by Muhammad H. Ali
 * This is the driver for the LFSR core I included under the MMIO
 * as a new slot, specifically user_slot4.
 *
 */

#ifndef LFSR_CORE_H_INCLUDED
#define LFSR_CORE_H_INCLUDED

#include "chu_io_rw.h"
#include "chu_io_map.h"
#include "timer_core.h"


class LfsrCore {
public:

	// Register Map:
	enum {
		READ_REG = 0,
		DATA_REG = 2
	};

	// Modes that can be used:
	enum {
		INIT = 0x00000001, // Init bit
		GENERATE = 0x00000000  // Start randomization
	};
	// Constructor
	LfsrCore(uint32_t core_base_addr);
	~LfsrCore(); // Destructor

	// Will read back the bits that have been randomized.
	uint32_t read();

	// Initialize back to reset signal in specification
	void reset();

	// Will reset the LFSR core and make it start incrementing
	// until start() has been called
	void generate();


private:
	// Each of the data uint32_t's are the data vectors
	// in the LFSR.vhd file.
	uint32_t base_addr;
	uint32_t writeVal;
};

#endif
