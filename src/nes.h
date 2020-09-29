
#ifndef NES_H
#define NES_H

#include <cstdint>
#include <cstdio>
#include <string>
#include "cpu.h"
#include "cartridge.h"
#include "ppu.h"
#include <SDL2/SDL.h>

class NES{
public:
	NES(std::string filename);
	~NES();

	CPU6502 cpu;
	PPU ppu;
	uint8_t palette_ram[32];

	uint32_t *screen_buf;

	void cpu_write(uint16_t addr, uint8_t data);
	uint8_t cpu_read(uint16_t addr);

	void ppu_write(uint16_t addr, uint8_t data);
	uint8_t ppu_read(uint16_t addr);

	void render_screen();

	void tick();
	void run();

#ifdef PATTERN
	uint32_t *pattern_buf;
	void display_pattern_table();
	void update_pattern_table();
	uint8_t palette_index = 0;
#endif

private:
	uint8_t ram[2048];
	uint8_t vram[2048];
	uint64_t ticks = 0;

	Cartridge cart;

	/* Game screen */
	SDL_Window *win = NULL;
	SDL_Renderer *renderer = NULL;
	SDL_Texture *texture = NULL;

#ifdef PATTERN
	/* For pattern tables display */
	SDL_Window *win_sprite = NULL;
	SDL_Renderer *ren_sprite = NULL;
	SDL_Texture *tx_sprite = NULL;
#endif

};

#endif