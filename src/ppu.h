#ifndef PPU_H
#define PPU_H

#include <cstdint>
#include <vector>

class NES;

class PPU{
public:
	PPU();
	~PPU();

	NES *machine;
	uint32_t rgb_palettes[64];
	bool nmi = false;

	struct color{
		uint8_t r;
		uint8_t g;
		uint8_t b;
	};

	void ppu_write(uint16_t addr, uint8_t data);
	uint8_t ppu_read(uint16_t addr);

	// For PPU registers, shared with CPU
	void cpu_write(uint16_t addr, uint8_t data);
	uint8_t cpu_read(uint16_t addr);

	uint32_t ColorToPixel(color col);
	color get_col_from_ram(uint8_t palette, uint8_t pixel);

	void clock();

	void connect(NES *mach){
		this->machine = mach;
	}
private:

	int32_t cycle = 0;
	int32_t scanline = 0;

	// Holds all possible colors
	std::vector<color> palette_colors;

	union{
		struct{
			uint8_t unused : 5;
			uint8_t sprite_overflow : 1;
			uint8_t sprite_zero_hit : 1;
			uint8_t vertical_blank : 1;
		};
		uint8_t reg;
	} status;

	union{
		struct{
			uint8_t grayscale : 1;
			uint8_t render_background_left : 1;
			uint8_t render_sprites_left : 1;
			uint8_t render_background : 1;
			uint8_t render_sprites : 1;
			uint8_t enhance_red : 1;
			uint8_t enhance_green : 1;
			uint8_t enhance_blue : 1;
		};
		uint8_t reg;
	} mask;

	union{
		struct{
			uint8_t nametable_x : 1;
			uint8_t nametable_y : 1;
			uint8_t increment_mode : 1;
			uint8_t pattern_sprite : 1;
			uint8_t pattern_background : 1;
			uint8_t sprite_size : 1;
			uint8_t slave_mode : 1;
			uint8_t enable_nmi : 1;
		};
		uint8_t reg;
	} control;

	union loopy_register{
		struct{
			uint16_t coarse_x : 5;
			uint16_t coarse_y : 5;
			uint16_t nametable_x : 1;
			uint16_t nametable_y : 1;
			uint16_t fine_y : 3;
			uint16_t unused : 1;
		};
		uint16_t reg = 0x0000;
	};

	uint8_t address_latch = 0x00;
	uint8_t ppu_data_buffer = 0x00;
	loopy_register vram_addr;
	loopy_register tram_addr;

	uint8_t fine_x = 0x00;


	uint8_t bg_next_tile_id = 0x00;
	uint8_t bg_next_tile_atrib = 0x00;
	uint8_t bg_next_tile_lsb = 0x00;
	uint8_t bg_next_tile_msb = 0x00;

	uint16_t bg_shifter_pattern_lo = 0x0000;
	uint16_t bg_shifter_pattern_hi = 0x0000;
	uint16_t bg_shifter_attrib_lo  = 0x0000;
	uint16_t bg_shifter_attrib_hi  = 0x0000;

};

#endif