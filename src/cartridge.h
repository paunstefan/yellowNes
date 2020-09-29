
#ifndef CART_H
#define CART_H
#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include "mapper.h"

class Cartridge{
public:
	Cartridge(std::string filename);
	~Cartridge();


	enum mirroring {HORIZONTAL, VERTICAL, FOUR_SCREEN};

	mirroring namespace_mirroring;

	void cpu_write(uint16_t addr, uint8_t data);
	uint8_t cpu_read(uint16_t addr);

	void ppu_write(uint16_t addr, uint8_t data);
	uint8_t ppu_read(uint16_t addr);
private:
	uint8_t prg_rom_s;	// in 16KB units
	uint8_t chr_rom_s;	// in 8KB units
	uint8_t mapper;
	bool trainer;		// if it's true, ROM is offset by 512 bytes

	std::vector<uint8_t> prg_rom;
	std::vector<uint8_t> chr_rom;

	std::shared_ptr<Mapper> map;

};

#endif