#include <cstdlib>
#include <cstdio>
#include <string>
#include "nes.h"


int main(int argc, char **argv){

	if(argc != 2){
		printf("Usage: %s [ROM]\n", argv[0]);
		exit(1);
	}

	std::string rom(argv[1]);
	NES machine(rom);

	machine.run();

	return 0;
}