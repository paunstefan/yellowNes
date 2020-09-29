#include <cstdio>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>

#include "cartridge.h"
#include "mapper_0.h"

Cartridge::Cartridge(std::string filename){
	struct stat st;
	size_t file_size;
	uint8_t *file;

	int fd = open(filename.c_str(), O_RDONLY);
	if(fd < 0){
		printf("ERROR: couldn't open file\n");
		exit(1);
	}

	if(fstat(fd, &st) < 0){
		printf("ERROR: fstat\n");
		exit(1);
	}

	file_size = st.st_size;

	if((file = (uint8_t*)mmap(NULL, file_size, PROT_READ, MAP_FILE|MAP_PRIVATE, fd, 0)) == MAP_FAILED){
		printf("ERROR: mmap\n");
		exit(1);
	}

	if((file[0] != 'N') || (file[1] != 'E') || (file[2] != 'S') || (file[3] != 0x1A)){
		printf("ROM not valid (identification string)\n");
		exit(1);
	}

	prg_rom_s = file[4];
	chr_rom_s = file[5];

	namespace_mirroring = file[6] & 1 ? VERTICAL : HORIZONTAL;
	if(file[6] & 0x08){
		namespace_mirroring = FOUR_SCREEN;
	}
	trainer = file[6] & 0x04 ? 1 : 0;

	mapper = (file[6] & 0xF0) + ((file[7] & 0xF0) >> 4); 

	prg_rom.resize(prg_rom_s * 16384);
	chr_rom.resize(chr_rom_s * 8192);

	memcpy(&prg_rom[0], file + 16 + (trainer ? 512 : 0), prg_rom_s * 16384);
	memcpy(&chr_rom[0], file + 16 + (trainer ? 512 : 0) + prg_rom_s * 16384, chr_rom_s * 8192);

	if(munmap(file, file_size) == -1){
		printf("ERROR: munmap\n");
		exit(1);
	}
	close(fd);

#ifdef DEBUG
    printf("ROM: %s\nPRG ROM: %d * 16KB\nCHR ROM: %d * 8KB\nMapper: %d\nMirroring: %d\nTrainer: %s\n\n",
    		filename.c_str(), prg_rom_s, chr_rom_s, mapper, namespace_mirroring, trainer ? "yes":"no");
#endif

	switch(mapper){
		case 0:
			map = std::make_shared<Mapper_0>(prg_rom_s, chr_rom_s);
			break;
		default:
			printf("ROM uses unimplmenented mapper %d\n", mapper);
			exit(1);
	}


}

Cartridge::~Cartridge(){

}

void Cartridge::cpu_write(uint16_t addr, uint8_t data){
	uint32_t mapped_addr;
	if(map->cpu_write_addr(addr, mapped_addr)){
		prg_rom[mapped_addr] = data;
	}
}
uint8_t Cartridge::cpu_read(uint16_t addr){
	uint32_t mapped_addr;
	if(map->cpu_read_addr(addr, mapped_addr)){
		return prg_rom[mapped_addr];
	}

	return 0;

}

void Cartridge::ppu_write(uint16_t addr, uint8_t data){
	uint32_t mapped_addr;
	if(map->ppu_write_addr(addr, mapped_addr)){
		chr_rom[mapped_addr] = data;
	}
}
uint8_t Cartridge::ppu_read(uint16_t addr){
	uint32_t mapped_addr;
	if(map->ppu_read_addr(addr, mapped_addr)){
		return chr_rom[mapped_addr];
	}

	return 0;

}