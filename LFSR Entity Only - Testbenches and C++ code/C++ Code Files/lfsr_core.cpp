/**
 * Muhammad H. Ali
 * Custom driver for the LFSR core
 *
 */

#include "lfsr_core.h"
#include "timer_core.h"

LfsrCore::LfsrCore(uint32_t core_base_addr) {
	base_addr = core_base_addr;
	writeVal = 0x01;
	reset();
}

LfsrCore::~LfsrCore(){}

uint32_t LfsrCore::read() {
	return (uint32_t) io_read(base_addr, DATA_REG);
}

void LfsrCore::reset() {
	uint32_t wdata;
	wdata = 1;//writeVal | INIT;
	io_write(base_addr, DATA_REG, wdata);
}

void LfsrCore::generate() {
	uint32_t wdata;
	wdata = 0; //writeVal & ~GENERATE;
	io_write(base_addr, DATA_REG, wdata);

}

