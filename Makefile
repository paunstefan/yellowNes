CXX=g++
#CFLAGS=-Wall -Wextra -Werror
OBJ=src/main.cpp src/cpu.cpp src/nes.cpp src/mapper.h src/cartridge.cpp src/mapper_0.cpp src/ppu.cpp

ifdef DEBUG
	CFLAGS+= -DDEBUG=1
endif

ifdef PATTERN
	CFLAGS+= -DPATTERN=1
endif

all: nesemu

nesemu: $(OBJ)
	$(CXX) -o $@ $^ $(CFLAGS) -lSDL2 -g

clean:
	rm ./nesemu