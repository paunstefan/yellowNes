
#ifndef MAP_H
#define MAP_H
#include <cstdint>

class Mapper{
public:
	Mapper(uint8_t prg, uint8_t chr): prg_banks{prg}, chr_banks{chr}{

	}

	virtual bool cpu_read_addr(uint16_t addr, uint32_t &ret) = 0;
	virtual bool cpu_write_addr(uint16_t addr, uint32_t &ret) = 0;
	virtual bool ppu_read_addr(uint16_t addr, uint32_t &ret) = 0;
	virtual bool ppu_write_addr(uint16_t addr, uint32_t &ret) = 0;


protected:
	uint8_t prg_banks;
	uint8_t chr_banks;
};

#endif