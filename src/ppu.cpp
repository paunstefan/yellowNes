#include "ppu.h"
#include "nes.h"
#include <SDL2/SDL.h>
#include <vector>

PPU::PPU(){
	/* Colors initialization - from savtool's NES palette*/
	palette_colors = {
		{84, 84, 84}, {0, 30, 116}, {8, 16, 144}, {48, 0, 136},
		{68, 0, 100}, {92, 0, 48}, {84, 4, 0}, {60, 24, 0},
		{32, 42, 0}, {8, 58, 0}, {0, 64, 0}, {0, 60, 0},
		{0, 50, 60}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0},

		{152, 150, 152}, {8, 76, 196}, {48, 50, 236}, {92, 30, 228},
		{136, 20, 176}, {160, 20, 100}, {152, 34, 32}, {120, 60, 0}, 
		{84, 90, 0}, {40, 114, 0}, {8, 124, 0}, {0, 118, 40},
		{0, 102, 120}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0},

		{236, 238, 236}, {76, 154, 236}, {120, 124, 236}, {176, 89, 236},
		{228, 84, 236}, {236, 88, 180}, {236, 106, 100}, {212, 136, 32},
		{160, 170, 0}, {116, 196, 0}, {76, 208, 32}, {56, 204, 108},
		{56, 180, 204}, {60, 60, 60}, {0, 0, 0}, {0, 0, 0},

		{236, 238, 236}, {168, 204, 236}, {188, 188, 236}, {212, 178, 236},
		{236, 174, 236}, {236, 174, 212}, {236, 180, 176}, {228, 196, 144},
		{204, 210, 120}, {180, 222, 120}, {168, 226, 144}, {152, 226, 180},
		{160, 214, 228}, {160, 162, 160}, {0, 0, 0}, {0, 0, 0},
	};

	
}
PPU::~PPU(){
	
}


void PPU::ppu_write(uint16_t addr, uint8_t data){
	machine->ppu_write(addr, data);
}
uint8_t PPU::ppu_read(uint16_t addr){
	return machine->ppu_read(addr);
}

uint32_t PPU::ColorToPixel(color col){
	uint32_t res = 0;
	res |= col.r << 16;
	res |= col.g << 8;
	res |= col.b;
	res |= 0xFF << 24;

	return res;
};

void PPU::clock(){

	auto IncrementScrollX = [&](){
		if(mask.render_background || mask.render_sprites){
			if(vram_addr.coarse_x == 31){
				vram_addr.coarse_x = 0;
				vram_addr.nametable_x = ~vram_addr.nametable_x;
			}
			else{
				vram_addr.coarse_x++;
			}
		}
	};

	auto IncrementScrollY = [&](){
		if(mask.render_background || mask.render_sprites){
			if(vram_addr.fine_y < 7){
				vram_addr.fine_y++;
			}
			else{
				vram_addr.fine_y = 0;

				if(vram_addr.coarse_y == 29){
					vram_addr.coarse_y = 0;
					vram_addr.nametable_y = ~vram_addr.nametable_y;
				}
				else if(vram_addr.coarse_y == 31){
					vram_addr.coarse_y = 0;
				}
				else{
					vram_addr.coarse_y++;
				}
			}
		}
	};

	auto TransferAddressX = [&](){
		if (mask.render_background || mask.render_sprites){
			vram_addr.nametable_x = tram_addr.nametable_x;
			vram_addr.coarse_x    = tram_addr.coarse_x;
		}
	};

	auto TransferAddressY = [&](){
		if (mask.render_background || mask.render_sprites){
			vram_addr.fine_y      = tram_addr.fine_y;
			vram_addr.nametable_y = tram_addr.nametable_y;
			vram_addr.coarse_y    = tram_addr.coarse_y;
		}
	};

	auto LoadBackgroundShifters = [&](){	
		bg_shifter_pattern_lo = (bg_shifter_pattern_lo & 0xFF00) | bg_next_tile_lsb;
		bg_shifter_pattern_hi = (bg_shifter_pattern_hi & 0xFF00) | bg_next_tile_msb;
		bg_shifter_attrib_lo  = (bg_shifter_attrib_lo & 0xFF00) | ((bg_next_tile_atrib & 0b01) ? 0xFF : 0x00);
		bg_shifter_attrib_hi  = (bg_shifter_attrib_hi & 0xFF00) | ((bg_next_tile_atrib & 0b10) ? 0xFF : 0x00);
	};

	auto UpdateShifters = [&](){
		if (mask.render_background){
			bg_shifter_pattern_lo <<= 1;
			bg_shifter_pattern_hi <<= 1;

			bg_shifter_attrib_lo <<= 1;
			bg_shifter_attrib_hi <<= 1;
		}
	};

	if(scanline >= -1 && scanline < 240){
		/* Start of screen */
		if(-1 == scanline && 1 == cycle){
			status.vertical_blank = 0;
		}

		if(0 == scanline && 0 == cycle){
			cycle = 1;
		}

		if((cycle >= 2 && cycle < 258) || (cycle >= 321 && cycle < 338)){

			UpdateShifters();

			switch((cycle - 1) % 8){
				case 0:
					LoadBackgroundShifters();
					bg_next_tile_id = ppu_read(0x2000 | (vram_addr.reg & 0x0FFF));
					break;
				case 2:
					bg_next_tile_atrib = ppu_read(0x23C0 | (vram_addr.nametable_y << 11)
													| (vram_addr.nametable_x << 10)
													| ((vram_addr.coarse_y >> 2) << 3)
													| (vram_addr.coarse_x >> 2));
					
					if(vram_addr.coarse_y && 0x02){
						bg_next_tile_atrib >>= 4;
					}
					if(vram_addr.coarse_x && 0x02){
						bg_next_tile_atrib >>= 2;
					}
					bg_next_tile_atrib &= 0x03;
					break;
				case 4:
					bg_next_tile_lsb =  ppu_read((control.pattern_background << 12)
												+ ((uint16_t)bg_next_tile_id << 4)
												+ vram_addr.fine_y + 0);
					break;
				case 6:
					bg_next_tile_msb =  ppu_read((control.pattern_background << 12)
								+ ((uint16_t)bg_next_tile_id << 4)
								+ vram_addr.fine_y + 8);
					break;
				case 7:
					IncrementScrollX();
					break;
			}
		}

		if(256 == cycle){
			IncrementScrollY();
		}

		if(257 == cycle){
			LoadBackgroundShifters();
			TransferAddressX();
		}

		if(338 == cycle || 340 == cycle){
			bg_next_tile_id = ppu_read(0x2000 | (vram_addr.reg & 0x0FFF));
		}

		if(scanline == -1 && cycle >= 280 && cycle < 305){
			TransferAddressY();
		}
	}

	if(240 == scanline){
		// Nothing
	}

	if(scanline >= 241 && scanline < 261){
		/* End of screen */
		if(241 == scanline && cycle == 1){
			status.vertical_blank = 1;

			if(control.enable_nmi){
				nmi = true;
			}
		}
	}

	uint8_t bg_pixel = 0x00;
	uint8_t bg_palette = 0x00;

	if(mask.render_background){
		uint16_t bit_mux = 0x8000 >> fine_x;

		uint8_t p0_pixel = (bg_shifter_pattern_lo & bit_mux) > 0;
		uint8_t p1_pixel = (bg_shifter_pattern_hi & bit_mux) > 0;
		bg_pixel = (p1_pixel << 1) | p0_pixel;

		uint8_t bg_pal0 = (bg_shifter_attrib_lo & bit_mux) > 0;
		uint8_t bg_pal1 = (bg_shifter_attrib_hi & bit_mux) > 0;
		bg_palette = (bg_pal1 << 1) | bg_pal0;
	}

#if 0
	if((scanline >= 0 && scanline < 240) && (cycle >= 2 && cycle < 258)){
		machine->screen_buf[scanline * 256 + cycle - 1] = ColorToPixel(get_col_from_ram(bg_palette, bg_pixel));
	}
#endif

	cycle++;

	// 340 cycles per line
	if(cycle >= 341){
		cycle = 0;
		scanline++;
		// 263 lines per frame
		if(scanline >= 261){
			scanline = -1;
#if 0
			machine->render_screen();
#endif

#ifdef PATTERN
			machine->display_pattern_table();
#endif
		}
	}
}


PPU::color PPU::get_col_from_ram(uint8_t palette, uint8_t pixel){
	return palette_colors[ppu_read(0x3F00 + (palette << 2) + pixel)];
}

void PPU::cpu_write(uint16_t addr, uint8_t data){
	switch(addr){
		case 0x0000:	// Control
			control.reg = data;
			tram_addr.nametable_x = control.nametable_x;
			tram_addr.nametable_y = control.nametable_y;
			break;
		case 0x0001:	// Mask
			mask.reg = data;
			break;
		case 0x0002:	// Status
			// Can't write to status register
			break;
		case 0x0003:	// OAM Address

			break;
		case 0x0004:	// OAM data

			break;
		case 0x0005:	// Scroll
			if(address_latch == 0){
				fine_x = data & 0x07;
				tram_addr.coarse_x = data >> 3;
				address_latch = 1;
			}
			else{
				tram_addr.fine_y = data & 0x07;
				tram_addr.coarse_y = data >> 3;
				address_latch = 0;
			}
			break;
		case 0x0006:	// PPU address
			if(address_latch == 0){
				tram_addr.reg = (uint16_t)((data & 0x3F) << 8) | (tram_addr.reg & 0xFF00);
				address_latch = 1;
			}
			else{
				tram_addr.reg = (tram_addr.reg & 0xFF00) | data;
				vram_addr = tram_addr;
				address_latch = 0;
			}
			break;
		case 0x0007:	// PPU data
			ppu_write(vram_addr.reg, data);
			vram_addr.reg += (control.increment_mode ? 32 : 1);
			break;
	}
}
uint8_t PPU::cpu_read(uint16_t addr){
	uint8_t data = 0x00;
	switch(addr){
		case 0x0000:	// Control
			break;
		case 0x0001:	// Mask
			break;
		case 0x0002:	// Status
			data = (status.reg & 0xE0) | (ppu_data_buffer & 0x1F);
			status.vertical_blank = 0;
			address_latch = 0;
			break;
		case 0x0003:	// OAM Address

			break;
		case 0x0004:	// OAM data

			break;
		case 0x0005:	// Scroll

			break;
		case 0x0006:	// PPU address
			break;
		case 0x0007:	// PPU data
			data = ppu_data_buffer;
			ppu_data_buffer = ppu_read(vram_addr.reg);

			if(vram_addr.reg > 0x3F00){
				data = ppu_data_buffer;
			}
			vram_addr.reg += (control.increment_mode ? 32 : 1);
			break;
	}

	return data;
}