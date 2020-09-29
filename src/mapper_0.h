
#ifndef MAP0_H
#define MAP0_H

#include <cstdint>
#include "mapper_0.h"
#include "mapper.h"

class Mapper_0 : public Mapper{
public:
	Mapper_0(uint8_t prg, uint8_t chr): Mapper(prg, chr){

	}

	bool cpu_read_addr(uint16_t addr, uint32_t &ret) override;
	bool cpu_write_addr(uint16_t addr, uint32_t &ret) override;
	bool ppu_read_addr(uint16_t addr, uint32_t &ret) override;
	bool ppu_write_addr(uint16_t addr, uint32_t &ret) override;
};

#endif