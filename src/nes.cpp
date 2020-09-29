#include <cstdlib>
#include "nes.h"
#include "cpu.h"

#ifdef PATTERN
const int WIDTH_PT = 512;
const int HEIGHT_PT = 1024;
#endif

const int WIDTH = 512;
const int HEIGHT = 480;

NES::NES(std::string filename): cart{Cartridge(filename)}{
	this->cpu.connect(this);
	this->ppu.connect(this);
#if 0
	/* SDL window initialization */
	win = SDL_CreateWindow("YellowNES", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);

	if(win == NULL){
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		exit(1);
	}

	renderer = SDL_CreateRenderer(win, -1, 0);
	SDL_RenderSetLogicalSize(renderer, WIDTH, HEIGHT);

	texture = SDL_CreateTexture(renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		256, 240);

	screen_buf = new uint32_t[256 * 240];
#endif

#ifdef PATTERN
	/* SDL window initialization */
	win_sprite = SDL_CreateWindow("Pattern table", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH_PT, HEIGHT_PT, SDL_WINDOW_SHOWN);

	if(win_sprite == NULL){
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		exit(1);
	}

	ren_sprite = SDL_CreateRenderer(win_sprite, -1, 0);
	SDL_RenderSetLogicalSize(ren_sprite, WIDTH_PT, HEIGHT_PT);

	tx_sprite = SDL_CreateTexture(ren_sprite,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		128, 256);

	pattern_buf = new uint32_t[256 * 128];
#endif
}

NES::~NES(){
	delete[] screen_buf;
#ifdef PATTERN
	delete[] pattern_buf;
#endif
}

/* CPU read/write operations */
void NES::cpu_write(uint16_t addr, uint8_t data){
	if(addr < 0x2000){
		ram[addr & 0x07FF] = data;
	}
	else if(addr >= 0x2000 && addr <= 0x3FFF){
		// PPU registers
		ppu.cpu_write(addr & 0x0007, data);
	}
	else if(addr >= 0x4000 && addr <= 0x401F){
		// TODO: APU and IO
	}
	else if(addr >= 4020){
		cart.cpu_write(addr, data);
	}
}

uint8_t NES::cpu_read(uint16_t addr){
	uint8_t data = 0x00;
	if(addr < 0x2000){
		data = ram[addr & 0x07FF];
	}
	else if(addr >= 0x2000 && addr <= 0x3FFF){
		// PPU registers
		data = ppu.cpu_read(addr & 0x0007);
	}
	else if(addr >= 0x4000 && addr <= 0x401F){
		// TODO: APU and IO
		data = 0;
	}
	else if(addr >= 4020){
		data = cart.cpu_read(addr);
	}
	return data;
}

/* PPU read/write operations */
void NES::ppu_write(uint16_t addr, uint8_t data){
	if(addr < 0x2000){
		// Pattern tables
		cart.ppu_write(addr, data);
	}
	else if(addr >= 0x2000 && addr <= 0x3EFF){
		// Nametables
		uint16_t abs_addr = (addr & 0x0FFF);
		if(cart.namespace_mirroring == Cartridge::HORIZONTAL){
			abs_addr = abs_addr & 0xFBFF;
			vram[abs_addr] = data;
		}
		else if(cart.namespace_mirroring == Cartridge::VERTICAL){
			abs_addr = abs_addr & 0xF7FF;
			vram[abs_addr] = data;
		}
	}
	else if(addr >= 0x3F00 && addr <= 0x3FFF){
		// Palette
		addr &= 0x001F;
		if(addr == 0x0010) addr = 0x0000;
		if(addr == 0x0014) addr = 0x0004;
		if(addr == 0x0018) addr = 0x0008;
		if(addr == 0x001C) addr = 0x000C;
		palette_ram[addr] = data;
	}
}

uint8_t NES::ppu_read(uint16_t addr){
	uint8_t data = 0x00;
	if(addr < 0x2000){
		data = cart.ppu_read(addr);
	}
	else if(addr >= 0x2000 && addr <= 0x3EFF){
		uint16_t abs_addr = addr & 0x0FFF;
		if(cart.namespace_mirroring == Cartridge::HORIZONTAL){
			abs_addr = abs_addr & 0xFBFF;
			data = vram[abs_addr];
		}
		else if(cart.namespace_mirroring == Cartridge::VERTICAL){
			abs_addr = abs_addr & 0xF7FF;
			data = vram[abs_addr];
		}
	}
	else if(addr >= 0x3F00 && addr <= 0x3FFF){
		addr &= 0x001F;
		if(addr == 0x0010) addr = 0x0000;
		if(addr == 0x0014) addr = 0x0004;
		if(addr == 0x0018) addr = 0x0008;
		if(addr == 0x001C) addr = 0x000C;
		data = palette_ram[addr];
	}
	return data;
}

void NES::render_screen(){
	SDL_UpdateTexture(texture, NULL, screen_buf, 256 * sizeof(uint32_t));
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}

/* Operational tasks */
void NES::tick(){
	
	ppu.clock();

	if(ticks % 3 == 0){
		cpu.execute();
	}

	if(ppu.nmi){
		ppu.nmi = false;
		cpu.nmi();
	}

	ticks++;

	/* SDL input processing */
	SDL_Event event;
	bool done = false;
	if (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_QUIT:
				done = true;
				break;
			case SDL_KEYDOWN:
				switch( event.key.keysym.sym ){
						case SDLK_n:
							palette_index = (palette_index + 1) % 8;
							update_pattern_table();
							break;
						case SDLK_LEFT:
							break;
						case SDLK_RIGHT:
							break;
						case SDLK_UP:
							break;
						case SDLK_DOWN:
							break;
						default:
							break;
				}
				break;
			case SDL_KEYUP:
				break;
			default:
				break;
		}
	}

	if(done){
		SDL_Quit();
		std::exit(0);
	}

}

void NES::run(){
	cpu.reset();

	for(;;){
		this->tick();
		// sleep
	}
}

#ifdef PATTERN

void NES::update_pattern_table(){
	for(int32_t row = 0; row < 32; row++){
		for(int32_t col = 0; col < 16; col++){
			int32_t pt_index = row * (16*16) + col * 16;

			for(int32_t px_row = 0; px_row < 8; px_row++){
				uint8_t first_plane = ppu_read(pt_index + px_row + 0);
				uint8_t second_plane = ppu_read(pt_index + px_row + 8);

				for(int32_t px_col = 0; px_col < 8; px_col++){
					uint8_t mask = 1 << (7 - px_col);

					uint8_t color = ((first_plane & mask) ? 1:0) + ((second_plane & mask) ? 2:0);
					pattern_buf[(row * 1024 + col * 8) + px_row * 128 + px_col] = ppu.ColorToPixel(ppu.get_col_from_ram(palette_index, color));	// this gives SDL color
				}
			}

		}
	}

	SDL_UpdateTexture(tx_sprite, NULL, pattern_buf, 128 * sizeof(uint32_t));	
}

void NES::display_pattern_table(){
	SDL_RenderClear(ren_sprite);
	SDL_RenderCopy(ren_sprite, tx_sprite, NULL, NULL);
	SDL_RenderPresent(ren_sprite);
}
#endif