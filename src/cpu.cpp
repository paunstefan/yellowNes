#include <cstdio>
#include <iostream>
#include "cpu.h"
#include "nes.h"

CPU6502::CPU6502(){

	instruction_table = 
	{
		{"BRK", &CPU6502::BRK, &CPU6502::impl, 7}, {"ORA", &CPU6502::ORA, &CPU6502::ind_x,6}, {"INI", &CPU6502::INI, nullptr,0}, {"INI", &CPU6502::INI, nullptr, 0}, {"INI", &CPU6502::INI, nullptr, 0}, {"ORA", &CPU6502::ORA, &CPU6502::zpg,3}, {"ASL", &CPU6502::ASL, &CPU6502::zpg, 5}, {"INI", &CPU6502::INI, nullptr, 0},
		{"PHP", &CPU6502::PHP, &CPU6502::impl, 3}, {"ORA", &CPU6502::ORA, &CPU6502::imm, 2}, {"ASL", &CPU6502::ASL, &CPU6502::acc, 2}, {"INI", &CPU6502::INI, nullptr, 0}, {"INI", &CPU6502::INI, nullptr, 0}, {"ORA", &CPU6502::ORA, &CPU6502::abs, 4}, {"ASL", &CPU6502::ASL, &CPU6502::abs, 6}, {"INI", &CPU6502::INI, nullptr, 0},
		{"BPL", &CPU6502::BPL, &CPU6502::impl, 2}, {"ORA", &CPU6502::ORA, &CPU6502::ind_y, 5}, {"INI", &CPU6502::INI, nullptr, 0}, {"INI", &CPU6502::INI, nullptr, 0}, {"INI", &CPU6502::INI, nullptr, 0}, {"ORA", &CPU6502::ORA, &CPU6502::zpg_x, 4}, {"ASL", &CPU6502::ASL, &CPU6502::zpg_x, 6}, {"INI", &CPU6502::INI, nullptr, 0},
		{"CLC", &CPU6502::CLC, &CPU6502::impl, 2}, {"ORA", &CPU6502::ORA, &CPU6502::abs_y, 4}, {"INI", &CPU6502::INI, nullptr, 0}, {"INI", &CPU6502::INI, nullptr, 0}, {"INI", &CPU6502::INI, nullptr, 0}, {"ORA", &CPU6502::ORA, &CPU6502::abs_x, 4}, {"ASL", &CPU6502::ASL, &CPU6502::abs_x, 7}, {"INI", &CPU6502::INI, nullptr, 0},
		{"JSR", &CPU6502::JSR, &CPU6502::impl, 6}, {"AND", &CPU6502::AND, &CPU6502::ind_x, 6}, {"INI", &CPU6502::INI, nullptr, 0}, {"INI", &CPU6502::INI, nullptr, 0}, {"BIT", &CPU6502::BIT, &CPU6502::zpg, 3}, {"AND", &CPU6502::AND, &CPU6502::zpg, 3}, {"ROL", &CPU6502::ROL, &CPU6502::zpg, 5}, {"INI", &CPU6502::INI, nullptr, 0},
		{"PLP", &CPU6502::PLP, &CPU6502::impl, 4}, {"AND", &CPU6502::AND, &CPU6502::imm, 2}, {"ROL", &CPU6502::ROL, &CPU6502::acc, 2}, {"INI", &CPU6502::INI, nullptr, 0}, {"BIT", &CPU6502::BIT, &CPU6502::abs, 4}, {"AND", &CPU6502::AND, &CPU6502::abs, 4}, {"ROL", &CPU6502::ROL, &CPU6502::abs, 6}, {"INI", &CPU6502::INI, nullptr, 0},
		{"BMI", &CPU6502::BMI, &CPU6502::impl, 2}, {"AND", &CPU6502::AND, &CPU6502::ind_y, 5}, {"INI", &CPU6502::INI, nullptr, 0}, {"INI", &CPU6502::INI, nullptr, 0}, {"INI", &CPU6502::INI, nullptr, 0}, {"AND", &CPU6502::AND, &CPU6502::zpg_x, 4}, {"ROL", &CPU6502::ROL, &CPU6502::zpg_x, 6}, {"INI", &CPU6502::INI, nullptr, 0},
		{"SEC", &CPU6502::SEC, &CPU6502::impl, 2}, {"AND", &CPU6502::AND, &CPU6502::abs_y, 4}, {"INI", &CPU6502::INI, nullptr, 0}, {"INI", &CPU6502::INI, nullptr, 0}, {"INI", &CPU6502::INI, nullptr, 0}, {"AND", &CPU6502::AND, &CPU6502::abs_x, 4}, {"ROL", &CPU6502::ROL, &CPU6502::abs_x, 7}, {"INI", &CPU6502::INI, nullptr, 0},
		{"RTI", &CPU6502::RTI, &CPU6502::impl, 6}, {"EOR", &CPU6502::EOR, &CPU6502::ind_x, 6}, {"INI", &CPU6502::INI, nullptr, 0}, {"INI", &CPU6502::INI, nullptr, 0}, {"INI", &CPU6502::INI, nullptr, 0}, {"EOR", &CPU6502::EOR, &CPU6502::zpg, 3}, {"LSR", &CPU6502::LSR, &CPU6502::zpg, 5}, {"INI", &CPU6502::INI, nullptr, 0},
		{"PHA", &CPU6502::PHA, &CPU6502::impl, 3}, {"EOR", &CPU6502::EOR, &CPU6502::imm, 2}, {"LSR", &CPU6502::LSR, &CPU6502::acc, 2}, {"INI", &CPU6502::INI, nullptr, 0}, {"JMP", &CPU6502::JMP, &CPU6502::abs, 3}, {"EOR", &CPU6502::EOR, &CPU6502::abs, 4}, {"LSR", &CPU6502::LSR, &CPU6502::abs, 6}, {"INI", &CPU6502::INI, nullptr, 0},
		{"BVC", &CPU6502::BVC, &CPU6502::impl, 2}, {"EOR", &CPU6502::EOR, &CPU6502::ind_y, 5}, {"INI", &CPU6502::INI, nullptr, 0}, {"INI", &CPU6502::INI, nullptr, 0}, {"INI", &CPU6502::INI, nullptr, 0}, {"EOR", &CPU6502::EOR, &CPU6502::zpg_x, 4}, {"LSR", &CPU6502::LSR, &CPU6502::zpg_x, 6}, {"INI", &CPU6502::INI, nullptr, 0},
		{"CLI", &CPU6502::SEC, &CPU6502::impl, 2}, {"EOR", &CPU6502::EOR, &CPU6502::abs_y, 4}, {"INI", &CPU6502::INI, nullptr, 0}, {"INI", &CPU6502::INI, nullptr, 0}, {"INI", &CPU6502::INI, nullptr, 0}, {"EOR", &CPU6502::EOR, &CPU6502::abs_x, 4}, {"LSR", &CPU6502::LSR, &CPU6502::abs_x, 7}, {"INI", &CPU6502::INI, nullptr, 0},
		{"RTS", &CPU6502::RTS, &CPU6502::impl, 6}, {"ADC", &CPU6502::ADC, &CPU6502::ind_x, 6}, {"INI", &CPU6502::INI, nullptr, 0}, {"INI", &CPU6502::INI, nullptr, 0}, {"INI", &CPU6502::INI, nullptr, 0}, {"ADC", &CPU6502::ADC, &CPU6502::zpg, 3}, {"ROR", &CPU6502::ROR, &CPU6502::zpg, 5}, {"INI", &CPU6502::INI, nullptr, 0},
		{"PLA", &CPU6502::PLA, &CPU6502::impl, 4}, {"ADC", &CPU6502::ADC, &CPU6502::imm, 2}, {"ROR", &CPU6502::ROR, &CPU6502::acc, 2}, {"INI", &CPU6502::INI, nullptr, 0}, {"JMP", &CPU6502::JMP, &CPU6502::ind,5}, {"ADC", &CPU6502::ADC, &CPU6502::abs, 4}, {"ROR", &CPU6502::ROR, &CPU6502::abs, 6}, {"INI", &CPU6502::INI, nullptr, 0},
		{"BVS", &CPU6502::BVS, &CPU6502::impl, 2}, {"ADC", &CPU6502::ADC, &CPU6502::ind_y, 5}, {"INI", &CPU6502::INI, nullptr, 0}, {"INI", &CPU6502::INI, nullptr, 0}, {"INI", &CPU6502::INI, nullptr, 0}, {"ADC", &CPU6502::ADC, &CPU6502::zpg_x, 4}, {"ROR", &CPU6502::ROR, &CPU6502::zpg_x, 6}, {"INI", &CPU6502::INI, nullptr, 0},
		{"SEI", &CPU6502::SEI, &CPU6502::impl, 2}, {"ADC", &CPU6502::ADC, &CPU6502::abs_y, 4}, {"INI", &CPU6502::INI, nullptr, 0}, {"INI", &CPU6502::INI, nullptr, 0}, {"INI", &CPU6502::INI, nullptr, 0}, {"ADC", &CPU6502::ADC, &CPU6502::abs_x, 4}, {"ROR", &CPU6502::ROR, &CPU6502::abs_x, 7}, {"INI", &CPU6502::INI, nullptr, 0},
		{"INI", &CPU6502::INI, nullptr, 0},        {"STA", &CPU6502::STA, &CPU6502::ind_x, 6}, {"INI", &CPU6502::INI, nullptr, 0}, {"INI", &CPU6502::INI, nullptr, 0}, {"STY", &CPU6502::STY, &CPU6502::zpg, 3}, {"STA", &CPU6502::STA, &CPU6502::zpg, 3}, {"STX", &CPU6502::STX, &CPU6502::zpg, 3}, {"INI", &CPU6502::INI, nullptr, 0},
		{"DEY", &CPU6502::DEY, &CPU6502::impl, 2}, {"INI", &CPU6502::INI, nullptr, 0}, {"TXA", &CPU6502::TXA, &CPU6502::impl, 2}, {"INI", &CPU6502::INI, nullptr, 0}, {"STY", &CPU6502::STY, &CPU6502::abs, 4}, {"STA", &CPU6502::STA, &CPU6502::abs, 4}, {"STX", &CPU6502::STX, &CPU6502::abs, 4}, {"INI", &CPU6502::INI, nullptr, 0},
		{"BCC", &CPU6502::BCC, &CPU6502::impl, 2}, {"STA", &CPU6502::STA, &CPU6502::ind_y, 6}, {"INI", &CPU6502::INI, nullptr, 0}, {"INI", &CPU6502::INI, nullptr, 0}, {"STY", &CPU6502::STY, &CPU6502::zpg_x, 4}, {"STA", &CPU6502::STA, &CPU6502::zpg_x, 4}, {"STX", &CPU6502::STX, &CPU6502::zpg_y, 4}, {"INI", &CPU6502::INI, nullptr, 0},
		{"TYA", &CPU6502::TYA, &CPU6502::impl, 2}, {"STA", &CPU6502::STA, &CPU6502::abs_y, 5}, {"TXS", &CPU6502::TXS, &CPU6502::impl, 2}, {"INI", &CPU6502::INI, nullptr, 0}, {"INI", &CPU6502::INI, nullptr, 0}, {"STA", &CPU6502::STA, &CPU6502::abs_x, 5}, {"INI", &CPU6502::INI, nullptr, 0}, {"INI", &CPU6502::INI, nullptr, 0},
		{"LDY", &CPU6502::LDY, &CPU6502::imm, 2}, {"LDA", &CPU6502::LDA, &CPU6502::ind_x, 6}, {"LDX", &CPU6502::LDX, &CPU6502::imm, 2}, {"INI", &CPU6502::INI, nullptr, 0}, {"LDY", &CPU6502::LDX, &CPU6502::zpg, 3}, {"LDA", &CPU6502::LDA, &CPU6502::zpg, 3}, {"LDX", &CPU6502::LDX, &CPU6502::zpg, 3}, {"INI", &CPU6502::INI, nullptr, 0},
		{"TAY", &CPU6502::TAY, &CPU6502::impl, 2}, {"LDA", &CPU6502::LDA, &CPU6502::imm, 2}, {"TAX", &CPU6502::TAX, &CPU6502::impl, 2}, {"INI", &CPU6502::INI, nullptr, 0}, {"LDY", &CPU6502::LDY, &CPU6502::abs, 4}, {"LDA", &CPU6502::LDA, &CPU6502::abs, 4}, {"LDX", &CPU6502::LDX, &CPU6502::abs, 4}, {"INI", &CPU6502::INI, nullptr, 0},
		{"BCS", &CPU6502::BCS, &CPU6502::impl, 2}, {"LDA", &CPU6502::LDA, &CPU6502::ind_y, 5}, {"INI", &CPU6502::INI, nullptr, 0}, {"INI", &CPU6502::INI, nullptr, 0}, {"LDY", &CPU6502::LDX, &CPU6502::zpg_x, 4}, {"LDA", &CPU6502::LDA, &CPU6502::zpg_x, 4}, {"LDX", &CPU6502::LDX, &CPU6502::zpg_y, 4}, {"INI", &CPU6502::INI, nullptr, 0},
		{"CLV", &CPU6502::CLV, &CPU6502::impl, 2}, {"LDA", &CPU6502::LDA, &CPU6502::abs_y, 4}, {"TSX", &CPU6502::TSX, &CPU6502::impl, 2}, {"INI", &CPU6502::INI, nullptr, 0}, {"LDY", &CPU6502::LDY, &CPU6502::abs_x, 4}, {"LDA", &CPU6502::LDA, &CPU6502::abs_x, 4}, {"LDX", &CPU6502::LDX, &CPU6502::abs_y, 4}, {"INI", &CPU6502::INI, nullptr, 0},
		{"CPY", &CPU6502::CPY, &CPU6502::imm, 2}, {"CMP", &CPU6502::CMP, &CPU6502::ind_x, 6}, {"INI", &CPU6502::INI, nullptr, 0}, {"INI", &CPU6502::INI, nullptr, 0}, {"CPY", &CPU6502::CPY, &CPU6502::zpg, 3}, {"CMP", &CPU6502::CMP, &CPU6502::zpg, 3}, {"DEC", &CPU6502::DEC, &CPU6502::zpg, 5}, {"INI", &CPU6502::INI, nullptr, 0},
		{"INY", &CPU6502::INY, &CPU6502::impl, 2}, {"CMP", &CPU6502::CMP, &CPU6502::imm, 2}, {"DEX", &CPU6502::DEX, &CPU6502::impl, 2}, {"INI", &CPU6502::INI, nullptr, 0}, {"CPY", &CPU6502::CPY, &CPU6502::abs, 4}, {"CMP", &CPU6502::CMP, &CPU6502::abs, 4}, {"DEC", &CPU6502::DEC, &CPU6502::abs, 6}, {"INI", &CPU6502::INI, nullptr, 0},
		{"BNE", &CPU6502::BNE, &CPU6502::impl, 2}, {"CMP", &CPU6502::CMP, &CPU6502::ind_y, 5}, {"INI", &CPU6502::INI, nullptr, 0}, {"INI", &CPU6502::INI, nullptr, 0}, {"INI", &CPU6502::INI, nullptr, 0}, {"CMP", &CPU6502::CMP, &CPU6502::zpg_x, 4}, {"DEC", &CPU6502::DEC, &CPU6502::zpg_x, 6}, {"INI", &CPU6502::INI, nullptr, 0},
		{"CLD", &CPU6502::CLD, &CPU6502::impl, 2}, {"CMP", &CPU6502::CMP, &CPU6502::abs_y, 4}, {"INI", &CPU6502::INI, nullptr, 0}, {"INI", &CPU6502::INI, nullptr, 0}, {"INI", &CPU6502::INI, nullptr, 0}, {"CMP", &CPU6502::CMP, &CPU6502::abs_x, 4}, {"DEC", &CPU6502::DEC, &CPU6502::abs_x, 7}, {"INI", &CPU6502::INI, nullptr, 0},
		{"CPX", &CPU6502::CPX, &CPU6502::imm, 2}, {"SBC", &CPU6502::SBC, &CPU6502::ind_x, 6}, {"INI", &CPU6502::INI, nullptr, 0}, {"INI", &CPU6502::INI, nullptr, 0}, {"CPX", &CPU6502::CPX, &CPU6502::zpg, 3}, {"SBC", &CPU6502::SBC, &CPU6502::zpg, 3}, {"INC", &CPU6502::INC, &CPU6502::zpg, 5}, {"INI", &CPU6502::INI, nullptr, 0},
		{"INX", &CPU6502::INX, &CPU6502::impl, 2}, {"SBC", &CPU6502::SBC, &CPU6502::imm, 2}, {"NOP", &CPU6502::NOP, &CPU6502::impl, 2}, {"INI", &CPU6502::INI, nullptr, 0}, {"CPX", &CPU6502::CPX, &CPU6502::abs, 4}, {"SBC", &CPU6502::SBC, &CPU6502::abs, 4}, {"INC", &CPU6502::INC, &CPU6502::abs, 6}, {"INI", &CPU6502::INI, nullptr, 0},
		{"BEQ", &CPU6502::BEQ, &CPU6502::impl, 2}, {"SBC", &CPU6502::SBC, &CPU6502::ind_y, 5}, {"INI", &CPU6502::INI, nullptr, 0}, {"INI", &CPU6502::INI, nullptr, 0}, {"INI", &CPU6502::INI, nullptr, 0}, {"SBC", &CPU6502::SBC, &CPU6502::zpg_x, 4}, {"INC", &CPU6502::INC, &CPU6502::zpg_x, 6}, {"INI", &CPU6502::INI, nullptr, 0},
		{"SED", &CPU6502::SED, &CPU6502::impl, 2}, {"SBC", &CPU6502::SBC, &CPU6502::abs_y, 4}, {"INI", &CPU6502::INI, nullptr, 0}, {"INI", &CPU6502::INI, nullptr, 0}, {"INI", &CPU6502::INI, nullptr, 0}, {"SBC", &CPU6502::SBC, &CPU6502::abs_x, 4}, {"INC", &CPU6502::INC, &CPU6502::abs_x, 7}, {"INI", &CPU6502::INI, nullptr, 0},
	};

	//reset();
}

CPU6502::~CPU6502(){

}

/* Execute current instruction at PC */
void CPU6502::execute(){
	
	if(cycles <= 0){
		//getchar();		// REMOVE after debugging
		opcode = read(pc);
	
#ifdef DEBUG
		printf("  A=%d X=%d Y=%d SP=%d  C=%d Z=%d I=%d B=%d V=%d N=%d\n", a, x, y, sp, get_flag(C), 
			get_flag(Z), get_flag(I), get_flag(B), get_flag(V), get_flag(N));
		printf("0x%x  %x  ", pc, opcode);
	    printf("%s", instruction_table[opcode].name.c_str());
#endif
		cycles = (this->*instruction_table[opcode].operate)();

		cycles += instruction_table[opcode].cycles;
	}

	cycles--;
}

bool CPU6502::get_flag(flags_6502 flags){
	return ((p & flags) > 0)? 1 : 0;
}
void CPU6502::set_flag(flags_6502 flags, bool f){
	if(f){
		p = p | flags;
	}
	else{
		p = p & ~flags;
	}
}

void CPU6502::write(uint16_t addr, uint8_t data){
	machine->cpu_write(addr, data);
}
uint8_t CPU6502::read(uint16_t addr){
	return machine->cpu_read(addr);
}

void CPU6502::push_stack(uint8_t data){
	write(0x0100 + sp, data);
	--sp;
}
uint8_t CPU6502::pop_stack(){
	++sp;
	return read(0x0100 + sp);
}

void CPU6502::irq(){
	if(get_flag(I) == 0)
	{
		/* Disable interrupts */
		set_flag(I, 1);
		set_flag(B, 0);

		push_stack((pc >> 8) & 0xFF);
		push_stack(pc & 0xFF);

		push_stack(p);

		uint16_t new_pc = 0x0000;
		new_pc = new_pc | read(0xFFFE);
		new_pc = new_pc | (read(0xFFFF) << 8);

		pc = new_pc;
	}
	cycles -= 7;
}

void CPU6502::nmi(){
	/* Disable interrupts */
	set_flag(I, 1);
	set_flag(B, 0);

	push_stack((pc >> 8) & 0xFF);
	push_stack(pc & 0xFF);

	push_stack(p);

	uint16_t new_pc = 0x0000;
	new_pc = new_pc | read(0xFFFA);
	new_pc = new_pc | (read(0xFFFB) << 8);

	pc = new_pc;

	cycles -= 8;
}

void CPU6502::reset(){
	a = 0x00;
	x = 0x00;
	y = 0x00;
	p = 0x00;
	sp = 0xFD;

	uint16_t new_pc = 0x0000;
	new_pc = new_pc | read(0xFFFC);
	new_pc = new_pc | (read(0xFFFD) << 8);

	pc = new_pc;
	/* TODO: ROM test start address */
	//pc = 0xc000;

	cycles -= 7;
}

/* Addressing modes */
uint16_t CPU6502::ind_x(){
	uint16_t addr = 0x0000;
	uint8_t op = read(pc + 1);
	addr = ((op + x) & 0xFF) + (((op + x + 1) & 0xFF) << 8);
#ifdef DEBUG
    printf(" ($%02X,X)", op);
#endif
    pc += 2;
	return addr;
}

uint16_t CPU6502::ind_y(){
	uint16_t addr = 0x0000;
	uint8_t op = read(pc + 1);
	addr = (op + x) + (((op + 1) & 0xFF) << 8) + y;
#ifdef DEBUG
    printf(" ($%02X),X", op);
#endif
    pc += 2;
	return addr;
}

uint16_t CPU6502::ind(){
	uint16_t addr = read(pc + 1) + (read(pc + 2) << 8);
#ifdef DEBUG
    printf(" ($%04X),X", addr);
#endif
    pc += 3;

	/* Hardware bug - wraparound to page start */
	if(addr & 0x00FF == 0x00FF){
		return read(addr) + (read(addr & 0xFF00) << 8);
	}
	else{
		return read(addr) + (read(addr+1) << 8);
	}
}

uint16_t CPU6502::acc(){
#ifdef DEBUG
    printf(" A");
#endif
    pc += 1;
	return (uint16_t)a;
} 

uint16_t CPU6502::imm(){
	uint8_t op = read(pc + 1);
#ifdef DEBUG
    printf(" #%d", op);
#endif
    pc += 2;
	return (uint16_t)op;
} 

uint16_t CPU6502::zpg(){
	uint16_t addr = read(pc + 1);
#ifdef DEBUG
    printf(" $%02X", addr);
#endif
    pc += 2;
	return (uint16_t)addr;
}

uint16_t CPU6502::zpg_x(){
	uint16_t addr = read(pc + 1);
#ifdef DEBUG
    printf(" $%02X,X", addr);
#endif
    pc += 2;
	return (uint16_t)((addr + x) & 0xFF);
}

uint16_t CPU6502::zpg_y(){
	uint16_t addr = read(pc + 1);
#ifdef DEBUG
    printf(" $%02X,Y", addr);
#endif
    pc += 2;
	return (uint16_t)((addr + y) & 0xFF);
}

uint16_t CPU6502::rel(){
	uint16_t op = (uint16_t)read(pc + 1);
#ifdef DEBUG
    printf(" *%d", op);
#endif
    if(op & 0x80){
    	op |= 0xFF00;
    }
    pc += 2;
	return op;
}

uint16_t CPU6502::abs(){
	uint16_t addr = read(pc + 1) + (read(pc + 2) << 8);
#ifdef DEBUG
    printf(" $%04X", addr);
#endif
    pc += 3;
	return addr;
}

uint16_t CPU6502::abs_x(){
	uint16_t addr = read(pc + 1) + (read(pc + 2) << 8);
#ifdef DEBUG
    printf(" $%04X,X", addr);
#endif
    pc += 3;
	return addr + x;
}

uint16_t CPU6502::abs_y(){
	uint16_t addr = read(pc + 1) + (read(pc + 2) << 8);
#ifdef DEBUG
    printf(" $%04X,Y", addr);
#endif
    pc += 3;
	return addr + y;
}

uint16_t CPU6502::impl(){
	pc += 1;
	return 0;
}
/* END addressing modes */

/* Instructions */
uint8_t CPU6502::ADC(){
	uint16_t ans;
	uint8_t op = (this->*instruction_table[opcode].mode)();
	if(instruction_table[opcode].mode == &CPU6502::imm){
		ans = (uint16_t)a + (uint16_t)op + (uint16_t)get_flag(C);
		set_flag(V, (~((uint16_t)a ^ (uint16_t)op) & ((uint16_t)a ^ ans)) & 0x0080);  // Thanks to OLC for explanation
	}
	else{
		ans = (uint16_t)a + (uint16_t)read(op) + (uint16_t)get_flag(C);
		set_flag(V, (~((uint16_t)a ^ (uint16_t)read(op)) & ((uint16_t)a ^ ans)) & 0x0080);
	}
	set_flag(Z, 0 == (ans & 0xFF));
	set_flag(N, (ans & (1 << 7)) != 0);
	set_flag(C, ans > 255);
	a = ans & 0xFF;
	return 0;
}
uint8_t CPU6502::AND(){
	uint8_t ans;
	uint16_t op = (this->*instruction_table[opcode].mode)();
	if(instruction_table[opcode].mode == &CPU6502::imm){
		ans = a & (op & 0xFF);
	}
	else{
		ans = a & read(op);
	}
	set_flag(Z, (0 == ans));
	set_flag(N, ((ans & (1 << 7)) != 0));
	a = ans;
	return 0;
}
uint8_t CPU6502::ASL(){
	uint8_t ans;
	uint16_t op = (this->*instruction_table[opcode].mode)();
	if(instruction_table[opcode].mode == &CPU6502::acc){
		ans = op & 0xFF;
	}
	else{
		ans = read(op);
	}
	set_flag(C, ((ans & (1 << 7)) != 0));
	ans = ans << 1;
	set_flag(Z, (0 == ans));
	set_flag(N, ((ans & (1 << 7)) != 0));
	if(instruction_table[opcode].mode == &CPU6502::acc){
		a = ans;
	}
	else{
		write(op, ans);
	}
	return 0;
}
uint8_t CPU6502::BCC(){
	uint16_t offset = rel();
	if(get_flag(C) == 0){
		pc += offset;
	}
	return 0;
}
uint8_t CPU6502::BCS(){
	uint16_t offset = rel();
	if(get_flag(C)){
		pc += offset;
	}
	return 0;
}
uint8_t CPU6502::BEQ(){
	uint16_t offset = rel();
	if(get_flag(Z)){
		pc += offset;
	}
	return 0;
}
uint8_t CPU6502::BIT(){
	uint8_t ans = read((this->*instruction_table[opcode].mode)());
	set_flag(V, ((ans & V) != 0));
	set_flag(N, ((ans & N) != 0));
	set_flag(Z, ((ans & a) == 0));
	return 0;
}
uint8_t CPU6502::BMI(){
	uint16_t offset = rel();
	if(get_flag(N) == 0){
		pc += offset;
	}
	return 0;
}
uint8_t CPU6502::BNE(){
	uint16_t offset = rel();
	if(get_flag(Z) == 0){
		pc += offset;
	}
	return 0;
}
uint8_t CPU6502::BPL(){
	uint16_t offset = rel();
	if(get_flag(N)){
		pc += offset;
	}
	return 0;
}
uint8_t CPU6502::BRK(){
	impl();
	irq();
	set_flag(B, 1);
	return 0;
}
uint8_t CPU6502::BVC(){
	uint16_t offset = rel();
	if(get_flag(V) == 0){
		pc += offset;
	}
	return 0;
}
uint8_t CPU6502::BVS(){
	uint16_t offset = rel();
	if(get_flag(V)){
		pc += offset;
	}
	return 0;
}
uint8_t CPU6502::CLC(){
	impl();
	set_flag(C, 0);
	return 0;
}
uint8_t CPU6502::CLD(){
	impl();
	set_flag(D, 0);
	return 0;
}
uint8_t CPU6502::CLI(){
	impl();
	set_flag(I, 0);
	return 0;
}
uint8_t CPU6502::CLV(){
	impl();
	set_flag(V, 0);
	return 0;
}
uint8_t CPU6502::CMP(){
	uint8_t ans;
	uint16_t op = (this->*instruction_table[opcode].mode)();
	if(instruction_table[opcode].mode == &CPU6502::imm){
		ans = a - (op & 0xFF);
		set_flag(C, a >= (op & 0xFF));
	}
	else{
		ans = a - read(op);
		set_flag(C, a >= (read(op) & 0xFF));
	}
	set_flag(Z, (0 == ans));
	set_flag(N, ((ans & (1 << 7)) != 0));
	return 0;
}
uint8_t CPU6502::CPX(){
	uint8_t ans;
	uint16_t op = (this->*instruction_table[opcode].mode)();
	if(instruction_table[opcode].mode == &CPU6502::imm){
		ans = x - (op & 0xFF);
		set_flag(C, x >= (op & 0xFF));
	}
	else{
		ans = x - read(op);
		set_flag(C, x >= (read(op) & 0xFF));
	}
	set_flag(Z, (0 == ans));
	set_flag(N, ((ans & (1 << 7)) != 0));
	return 0;
}
uint8_t CPU6502::CPY(){
	uint8_t ans;
	uint16_t op = (this->*instruction_table[opcode].mode)();
	if(instruction_table[opcode].mode == &CPU6502::imm){
		ans = y - (op & 0xFF);
		set_flag(C, y >= (op & 0xFF));
	}
	else{
		ans = y - read(op);
		set_flag(C, y >= (read(op) & 0xFF));
	}
	set_flag(Z, (0 == ans));
	set_flag(N, ((ans & (1 << 7)) != 0));
	return 0;
}
uint8_t CPU6502::DEC(){
	uint8_t ans;
	uint16_t op = (this->*instruction_table[opcode].mode)();
	ans = read(op);
	ans -= 1;
	write(op, ans);
	set_flag(Z, ans == 0x00);
	set_flag(N, ans & 0x80);
	return 0;
}
uint8_t CPU6502::DEX(){
	impl();
	x -= 1;
	set_flag(Z, x == 0x00);
	set_flag(N, x & 0x80);
	return 0;
}
uint8_t CPU6502::DEY(){
	impl();
	y -= 1;
	set_flag(Z, y == 0x00);
	set_flag(N, y & 0x80);
	return 0;
}
uint8_t CPU6502::EOR(){
	uint8_t ans;
	uint16_t op = (this->*instruction_table[opcode].mode)();
	if(instruction_table[opcode].mode == &CPU6502::imm){
		ans = a ^ (op & 0xFF);
	}
	else{
		ans = a ^ read(op);
	}
	set_flag(Z, (0 == ans));
	set_flag(N, ((ans & (1 << 7)) != 0));
	a = ans;
	return 0;
}
uint8_t CPU6502::INC(){
	uint8_t ans;
	uint16_t op = (this->*instruction_table[opcode].mode)();
	ans = read(op);
	ans += 1;
	write(op, ans);
	set_flag(Z, ans == 0x00);
	set_flag(N, ans & 0x80);
	return 0;
}
uint8_t CPU6502::INX(){
	impl();
	x += 1;
	set_flag(Z, x == 0x00);
	set_flag(N, x & 0x80);
	return 0;
}
uint8_t CPU6502::INY(){
	impl();
	y += 1;
	set_flag(Z, y == 0x00);
	set_flag(N, y & 0x80);
	return 0;
}
uint8_t CPU6502::JMP(){
	pc = (this->*instruction_table[opcode].mode)();
	return 0;
}
uint8_t CPU6502::JSR(){
	push_stack(((pc+2) >> 8) & 0xFF);
	push_stack((pc+2) & 0xFF);
	pc = abs();
	return 0;
}
uint8_t CPU6502::LDA(){
	uint16_t op;
	if(instruction_table[opcode].mode == &CPU6502::imm){
		op = (this->*instruction_table[opcode].mode)();
	}
	else{
		op = read((this->*instruction_table[opcode].mode)());
	}
	set_flag(Z, (0 == op));
	set_flag(N, ((op & (1 << 7)) != 0));
	a = op;
	return 0;
}
uint8_t CPU6502::LDX(){
	uint16_t op;
	if(instruction_table[opcode].mode == &CPU6502::imm){
		op = (this->*instruction_table[opcode].mode)();
	}
	else{
		op = read((this->*instruction_table[opcode].mode)());
	}
	set_flag(Z, (0 == op));
	set_flag(N, ((op & (1 << 7)) != 0));
	x = op;
	return 0;
}
uint8_t CPU6502::LDY(){
	uint16_t op;
	if(instruction_table[opcode].mode == &CPU6502::imm){
		op = (this->*instruction_table[opcode].mode)();
	}
	else{
		op = read((this->*instruction_table[opcode].mode)());
	}
	set_flag(Z, (0 == op));
	set_flag(N, ((op & (1 << 7)) != 0));
	y = op;
	return 0;
}
uint8_t CPU6502::LSR(){
	uint8_t ans;
	uint16_t op = (this->*instruction_table[opcode].mode)();
	if(instruction_table[opcode].mode == &CPU6502::acc){
		ans = op & 0xFF;
	}
	else{
		ans = read(op);
	}
	set_flag(C, ((ans & 1) != 0));
	ans = ans >> 1;
	set_flag(Z, (0 == ans));
	set_flag(N, ((ans & (1 << 7)) != 0));
	if(instruction_table[opcode].mode == &CPU6502::acc){
		a = ans;
	}
	else{
		write(op, ans);
	}
	return 0;
}
uint8_t CPU6502::NOP(){
	impl();
	return 0;
}
uint8_t CPU6502::ORA(){
	uint8_t ans;
	uint16_t op = (this->*instruction_table[opcode].mode)();
	if(instruction_table[opcode].mode == &CPU6502::imm){
		ans = a | (op & 0xFF);
	}
	else{
		ans = a | read(op);
	}
	set_flag(Z, (0 == ans));
	set_flag(N, ((ans & (1 << 7)) != 0));
	a = ans;
	return 0;
}
uint8_t CPU6502::PHA(){
	impl();
	push_stack(a);
	return 0;
}
uint8_t CPU6502::PHP(){
	impl();
	push_stack(p);
	set_flag(B, 0);
	set_flag(U, 0);
	return 0;
}
uint8_t CPU6502::PLA(){
	impl();
	a = pop_stack();
	set_flag(Z, a = 0x0000);
	set_flag(N, a & 0x80);
	return 0;
}
uint8_t CPU6502::PLP(){
	impl();
	p = pop_stack();
	set_flag(U, 1);
	return 0;
}
uint8_t CPU6502::ROL(){
	uint8_t ans;
	uint16_t op = (this->*instruction_table[opcode].mode)();
	if(instruction_table[opcode].mode == &CPU6502::acc){
		ans = op & 0xFF;
	}
	else{
		ans = read(op);
	}
	uint8_t save_c = ((ans & (1 << 7)) != 0)? 1 : 0;
	set_flag(C, ((ans & (1 << 7)) != 0));
	ans = ans << 1;
	ans |= save_c;
	set_flag(Z, (0 == ans));
	set_flag(N, ((ans & (1 << 7)) != 0));
	if(instruction_table[opcode].mode == &CPU6502::acc){
		a = ans;
	}
	else{
		write(op, ans);
	}
	return 0;
}
uint8_t CPU6502::ROR(){
	uint8_t ans;
	uint16_t op = (this->*instruction_table[opcode].mode)();
	if(instruction_table[opcode].mode == &CPU6502::acc){
		ans = op & 0xFF;
	}
	else{
		ans = read(op);
	}
	uint8_t save_c = get_flag(C);
	set_flag(C, ans & 1);
	ans = ans >> 1;
	ans |= save_c << 7;
	set_flag(Z, (0 == ans));
	set_flag(N, ((ans & (1 << 7)) != 0));
	if(instruction_table[opcode].mode == &CPU6502::acc){
		a = ans;
	}
	else{
		write(op, ans);
	}
	return 0;
}
uint8_t CPU6502::RTI(){
	impl();
	uint16_t addr = 0x0000;
	p = pop_stack();
	addr += pop_stack();
	addr += pop_stack() << 8;
	pc = addr;
	return 0;
}
uint8_t CPU6502::RTS(){
	impl();
	uint16_t addr = 0x0000;
	addr += pop_stack();
	addr += pop_stack() << 8;
	pc = addr + 1;
	return 0;
}
uint8_t CPU6502::SBC(){
	uint16_t ans;
	uint8_t op = (this->*instruction_table[opcode].mode)();
	if(instruction_table[opcode].mode == &CPU6502::imm){
		ans = (uint16_t)a + ((uint16_t)op ^ 0x00FF)+ (uint16_t)get_flag(C);
		set_flag(V, (~((uint16_t)a ^ ((uint16_t)op ^ 0x00FF)) & ((uint16_t)a ^ ans)) & 0x0080);  // Thanks to OLC for explanation
	}
	else{
		ans = (uint16_t)a + ((uint16_t)read(op) ^ 0x00FF) + (uint16_t)get_flag(C);
		set_flag(V, (~((uint16_t)a ^ ((uint16_t)read(op) ^ 0x00FF)) & ((uint16_t)a ^ ans)) & 0x0080);
	}
	set_flag(Z, 0 == (ans & 0xFF));
	set_flag(N, (ans & (1 << 7)) != 0);
	set_flag(C, ans > 255);
	a = ans & 0xFF;
	return 0;
}
uint8_t CPU6502::SEC(){
	impl();
	set_flag(C, 1);
	return 0;
}
uint8_t CPU6502::SED(){
	impl();
	set_flag(D, 1);
	return 0;
}
uint8_t CPU6502::SEI(){
	impl();
	set_flag(I, 1);
	return 0;
}
uint8_t CPU6502::STA(){
	write((this->*instruction_table[opcode].mode)(), a);
	return 0;
}
uint8_t CPU6502::STX(){
	write((this->*instruction_table[opcode].mode)(), x);
	return 0;
}
uint8_t CPU6502::STY(){
	write((this->*instruction_table[opcode].mode)(), y);
	return 0;
}
uint8_t CPU6502::TAX(){
	impl();
	x = a;
	set_flag(Z, x = 0x00);
	set_flag(N, x & 0x80);
	return 0;
}
uint8_t CPU6502::TAY(){
	impl();
	y = a;
	set_flag(Z, y = 0x00);
	set_flag(N, y & 0x80);
	return 0;
}
uint8_t CPU6502::TSX(){
	impl();
	x = sp;
	set_flag(Z, x = 0x00);
	set_flag(N, x & 0x80);
	return 0;
}
uint8_t CPU6502::TXA(){
	impl();
	a = x;
	set_flag(Z, a = 0x00);
	set_flag(N, a & 0x80);
	return 0;
}
uint8_t CPU6502::TXS(){
	impl();
	sp = x;
	return 0;
}
uint8_t CPU6502::TYA(){
	impl();
	a = y;
	set_flag(Z, y = 0x00);
	set_flag(N, y & 0x80);
	return 0;
}


uint8_t CPU6502::INI(){
	/* Instruction not implemented */
	printf("Instruction not implemented.\n");
	return 0;
}

/* END instructions */