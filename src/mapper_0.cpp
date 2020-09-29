#include <cstdint>
#include "mapper_0.h"

bool Mapper_0::cpu_read_addr(uint16_t addr, uint32_t &ret){
	if(addr >= 0x8000 && addr <= 0xFFFF){
		ret = addr & (prg_banks == 2 ? 0x7FFF : 0x3FFF);
		return true;
	}
	return false;
}

bool Mapper_0::cpu_write_addr(uint16_t addr, uint32_t &ret){
	if(addr >= 0x8000 && addr <= 0xFFFF){
		ret = addr & (prg_banks == 2 ? 0x7FFF : 0x3FFF);
		// Don't allow writing in ROM for the moment
		return false;
	}
	return false;
}

bool Mapper_0::ppu_read_addr(uint16_t addr, uint32_t &ret){
	if(addr >= 0x0000 && addr <= 0x1FFF){
		ret = addr;
		return true;
	}
	return false;
}
bool Mapper_0::ppu_write_addr(uint16_t addr, uint32_t &ret){
	if(addr >= 0x0000 && addr <= 0x1FFF){
		ret = addr;
		// Only ROM for the moment
		return false;
	}
	return false;
}