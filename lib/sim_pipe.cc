#include "../include/sim_pipe.h"
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <iomanip>
#include <map>

//#define DEBUG

using namespace std;

//used for debugging purposes
static const char *reg_names[NUM_SP_REGISTERS] = {"PC", "NPC", "IR", "A", "B", "IMM", "COND", "ALU_OUTPUT", "LMD"};
static const char *stage_names[NUM_STAGES] = {"IF", "ID", "EX", "MEM", "WB"};
static const char *instr_names[NUM_OPCODES] = {"LW", "SW", "ADD", "ADDI", "SUB", "SUBI", "XOR", "BEQZ", "BNEZ", "BLTZ", "BGTZ", "BLEZ", "BGEZ", "JUMP", "EOP", "NOP"};

/* =============================================================
   HELPER FUNCTIONS
   ============================================================= */

/* converts integer into array of unsigned char - little indian */
inline void int2char(unsigned value, unsigned char *buffer){
	memcpy(buffer, &value, sizeof value);
}

/* converts array of char into integer - little indian */
inline unsigned char2int(unsigned char *buffer){
	unsigned d;
	memcpy(&d, buffer, sizeof d);
	return d;
}
/* implements the ALU operations */
unsigned alu(unsigned opcode, unsigned a, unsigned b, unsigned imm, unsigned npc){
	switch(opcode){
			case ADD:
				return (a+b);
			case ADDI:
				return(a+imm);
			case SUB:
				return(a-b);
			case SUBI:
				return(a-imm);
			case XOR:
				return(a ^ b);
			case LW:
			case SW:
				return(a + imm);
			case BEQZ:
			case BNEZ:
			case BGTZ:
			case BGEZ:
			case BLTZ:
			case BLEZ:
			case JUMP:
				return(npc+imm);
			default:
				return (-1);
	}
}

/* =============================================================

   CODE PROVIDED - NO NEED TO MODIFY FUNCTIONS BELOW

   ============================================================= */

/* loads the assembly program in file "filename" in instruction memory at the specified address */

void sim_pipe::load_program(const char *filename, unsigned base_address){
   /* initializing the base instruction address */
   instr_base_address = base_address;
   sp_register_bank->set_IF(base_address);

   /* creating a map with the valid opcodes and with the valid labels */
   map<string, opcode_t> opcodes; //for opcodes
   map<string, unsigned> labels;  //for branches
   for (int i=0; i<NUM_OPCODES; i++)
	 opcodes[string(instr_names[i])]=(opcode_t)i;

   /* opening the assembly file */
   ifstream fin(filename, ios::in | ios::binary);
   if (!fin.is_open()) {
      cerr << "error: open file " << filename << " failed!" << endl;
      exit(-1);
   }

   /* parsing the assembly file line by line */
   string line;
   unsigned instruction_nr = 0;
   while (getline(fin,line)){
	// set the instruction field
	char *str = const_cast<char*>(line.c_str());

  	// tokenize the instruction
	char *token = strtok (str," \t");
	map<string, opcode_t>::iterator search = opcodes.find(token);
        if (search == opcodes.end()){
		// this is a label for a branch - extract it and save it in the labels map
		string label = string(token).substr(0, string(token).length() - 1);
		labels[label]=instruction_nr;
                // move to next token, which must be the instruction opcode
		token = strtok (NULL, " \t");
		search = opcodes.find(token);
		if (search == opcodes.end()) cout << "ERROR: invalid opcode: " << token << " !" << endl;
	}
	instr_memory[instruction_nr].opcode = search->second;

	//reading remaining parameters
	char *par1;
	char *par2;
	char *par3;
	switch(instr_memory[instruction_nr].opcode){
		case ADD:
		case SUB:
		case XOR:
			par1 = strtok (NULL, " \t");
			par2 = strtok (NULL, " \t");
			par3 = strtok (NULL, " \t");
			instr_memory[instruction_nr].dest = atoi(strtok(par1, "R"));
			instr_memory[instruction_nr].src1 = atoi(strtok(par2, "R"));
			instr_memory[instruction_nr].src2 = atoi(strtok(par3, "R"));
			break;
		case ADDI:
		case SUBI:
			par1 = strtok (NULL, " \t");
			par2 = strtok (NULL, " \t");
			par3 = strtok (NULL, " \t");
			instr_memory[instruction_nr].dest = atoi(strtok(par1, "R"));
			instr_memory[instruction_nr].src1 = atoi(strtok(par2, "R"));
			instr_memory[instruction_nr].immediate = strtoul (par3, NULL, 0);
			break;
		case LW:
			par1 = strtok (NULL, " \t");
			par2 = strtok (NULL, " \t");
			instr_memory[instruction_nr].dest = atoi(strtok(par1, "R"));
			instr_memory[instruction_nr].immediate = strtoul(strtok(par2, "()"), NULL, 0);
			instr_memory[instruction_nr].src1 = atoi(strtok(NULL, "R"));
			break;
		case SW:
			par1 = strtok (NULL, " \t");
			par2 = strtok (NULL, " \t");
			instr_memory[instruction_nr].src1 = atoi(strtok(par1, "R"));
			instr_memory[instruction_nr].immediate = strtoul(strtok(par2, "()"), NULL, 0);
			instr_memory[instruction_nr].src2 = atoi(strtok(NULL, "R"));
			break;
		case BEQZ:
		case BNEZ:
		case BLTZ:
		case BGTZ:
		case BLEZ:
		case BGEZ:
			par1 = strtok (NULL, " \t");
			par2 = strtok (NULL, " \t");
			instr_memory[instruction_nr].src1 = atoi(strtok(par1, "R"));
			instr_memory[instruction_nr].label = par2;
			break;
		case JUMP:
			par2 = strtok (NULL, " \t");
			instr_memory[instruction_nr].label = par2;
		default:
			break;

	}

	/* increment instruction number before moving to next line */
	instruction_nr++;
   }
   //reconstructing the labels of the branch operations
   int i = 0;
   while(true){
   	instruction_t instr = instr_memory[i];
  	if (instr.opcode == EOP) break;
  	if (instr.opcode == BLTZ || instr.opcode == BNEZ ||
        instr.opcode == BGTZ || instr.opcode == BEQZ ||
        instr.opcode == BGEZ || instr.opcode == BLEZ ||
        instr.opcode == JUMP
  	 ){
  		instr_memory[i].immediate = (labels[instr.label] - i - 1) << 2;
  	}
    i++;
  }
}

/* writes an integer value to data memory at the specified address (use little-endian format: https://en.wikipedia.org/wiki/Endianness) */
void sim_pipe::write_memory(unsigned address, unsigned value){
	int2char(value,data_memory+address);
}
unsigned sim_pipe::load_memory(unsigned address){
  	unsigned d;
  	memcpy(&d, data_memory + address, sizeof d);
  	return d;
}

/* prints the content of the data memory within the specified address range */
void sim_pipe::print_memory(unsigned start_address, unsigned end_address){
	cout << "data_memory[0x" << hex << setw(8) << setfill('0') << start_address << ":0x" << hex << setw(8) << setfill('0') <<  end_address << "]" << endl;
	for (unsigned i=start_address; i<end_address; i++){
		if (i%4 == 0) cout << "0x" << hex << setw(8) << setfill('0') << i << ": ";
		cout << hex << setw(2) << setfill('0') << int(data_memory[i]) << " ";
		if (i%4 == 3) cout << endl;
	}
}

/* prints the values of the registers */
void sim_pipe::print_registers(){
        cout << "Special purpose registers:" << endl;
        unsigned i, s;
        for (s=0; s<NUM_STAGES; s++){
                cout << "Stage: " << stage_names[s] << endl;
                for (i=0; i< NUM_SP_REGISTERS; i++)
                        if ((sp_register_t)i != IR && (sp_register_t)i != COND && get_sp_register((sp_register_t)i, (stage_t)s)!=UNDEFINED) cout << reg_names[i] << " = " << dec <<  get_sp_register((sp_register_t)i, (stage_t)s) << hex << " / 0x" << get_sp_register((sp_register_t)i, (stage_t)s) << endl;
        }
        cout << "General purpose registers:" << endl;
        for (i=0; i< NUM_GP_REGISTERS; i++)
                if (get_gp_register(i)!=UNDEFINED) cout << "R" << dec << i << " = " << get_gp_register(i) << hex << " / 0x" << get_gp_register(i) << endl;
}

/* initializes the pipeline simulator */
sim_pipe::sim_pipe(unsigned mem_size, unsigned mem_latency){
	data_memory_size = mem_size;
	data_memory_latency = mem_latency;
	data_memory = new unsigned char[data_memory_size];
  gp_registers = (unsigned int*)malloc(sizeof(unsigned int) * NUM_GP_REGISTERS);
  sp_register_bank = new sp_registers;
	reset();
}

/* deallocates the pipeline simulator */
sim_pipe::~sim_pipe(){
  free(gp_registers);
  delete sp_register_bank;
	delete [] data_memory;
}

void sim_pipe::run(unsigned cycles){
  if(cycles > 0){
    while(cycles > 0){
      run_WB();
      run_MEM();
      if(!stall_flag_SH){
        run_EXE();
      }
      if(!stall_flag_SH ){
        run_ID();
      }
      if(!stall_flag_DH){
        if(!stall_flag_CH){
          if(!stall_flag_SH ){
            run_IF();
          }
        }else{
          if(CH_stall_count > 0){
            insert_stall_fetch();
            CH_stall_count--;
          }else{
            insert_stall_fetch();
            CH_stall_count = 1;
            stall_flag_CH = 0;
          }
        }
      }
      cycles--;
      num_cycles++;
      #ifdef DEBUG
        sp_register_bank->print_sp_registers();
      #endif
    }
  }
  else if(cycles == 0){
    instruction_t check_EOP = get_IR(WB);
    while(check_EOP.opcode != EOP){
      run_WB();
      run_MEM();
      if(!stall_flag_SH){
        run_EXE();
      }
      if(!stall_flag_SH){
        run_ID();
      }
      if(!stall_flag_DH){
        if(!stall_flag_CH){
          if(!stall_flag_SH ){
            run_IF();
          }
        }else{
          if(CH_stall_count > 0){
            insert_stall_fetch();
            CH_stall_count--;
          }else{
            insert_stall_fetch();
            CH_stall_count = 1;
            stall_flag_CH = 0;
          }
        }
      }
      num_cycles++;
      check_EOP = get_IR(WB);
    }
  }
}
void sim_pipe::run_IF(){
  unsigned current_pc, npc, op;
  instruction_t current_instruction, next_instruction;
  current_pc = get_sp_register(PC, IF);
  npc = current_pc + 4;
  sp_register_bank->get_IR(WB, current_instruction);
  op = current_instruction.opcode;
  if(op == BEQZ || op == BNEZ || op == BLTZ || op == BGTZ || op == BLEZ || op == BGEZ || op == JUMP){
    if(get_sp_register(COND, WB)){
      current_pc = get_sp_register(ALU_OUTPUT, WB);
      npc = current_pc + 4;
    }
  }
  next_instruction = instr_memory[ ((current_pc - instr_base_address)/4)];
  if(next_instruction.opcode == EOP){
    npc = current_pc;
  }
  sp_register_bank->set_IFID(npc, next_instruction);
  sp_register_bank->set_IF(npc);
}
void sim_pipe::run_ID(){
  unsigned temp_A, temp_B, npc, ID_src1, ID_src2;
  opcode_t op;
  instruction_t ID_instruction;
  sp_register_bank->get_IR(ID, ID_instruction);
  ID_src1 = ID_instruction.src1;
  ID_src2 = ID_instruction.src2;
  check_insert_DH_stall(ID_instruction, ID_src1, ID_src2);
  op = ID_instruction.opcode;
  switch(op){
    case ADD:
    case SUB:
    case XOR:
      temp_A = get_gp_register(ID_instruction.src1);
      temp_B = get_gp_register(ID_instruction.src2);
      ID_instruction.immediate = UNDEFINED;
    break;
    case ADDI:
    case SUBI:
    case LW:
      temp_A = get_gp_register(ID_instruction.src1);
      temp_B = UNDEFINED;
      break;
    case SW:
      temp_A = get_gp_register(ID_instruction.src2);
      temp_B = get_gp_register(ID_instruction.src1);
      ID_instruction.dest = UNDEFINED;
    break;
    case BEQZ:
		case BNEZ:
		case BLTZ:
		case BGTZ:
		case BLEZ:
		case BGEZ:
		case JUMP:
      temp_A = get_gp_register(ID_instruction.src1);
      temp_B = UNDEFINED;
      ID_instruction.dest = UNDEFINED;
      stall_flag_CH = 1;
    break;
    default:
      temp_A = UNDEFINED;
      temp_B = UNDEFINED;
      ID_instruction.immediate = UNDEFINED;
      ID_instruction.dest = UNDEFINED;
    break;
  }
  if(!stall_flag_DH){
    npc = get_sp_register(NPC,ID);
    sp_register_bank->set_IDEX(temp_A, temp_B, ID_instruction.immediate, npc, ID_instruction);
  }
  else{
    npc = UNDEFINED;
    sp_register_bank->set_IDEX(UNDEFINED, UNDEFINED, stall.immediate, npc, stall);
  }
}
void sim_pipe::run_EXE(){
  unsigned temp_cond, temp_A, temp_B, temp_IMM, npc, alu_temp;
  opcode_t op;
  instruction_t temp_instruction;
  temp_A = get_sp_register(A,EXE);
  temp_B = get_sp_register(B,EXE);
  temp_IMM = get_sp_register(IMM,EXE);
  npc = get_sp_register(NPC,EXE);
  sp_register_bank->get_IR(EXE, temp_instruction);
  op = temp_instruction.opcode;
  alu_temp = alu(op, temp_A, temp_B, temp_IMM, npc);
  temp_cond = get_condition(op);
  sp_register_bank->set_EXMEM(temp_B, alu_temp, temp_cond, temp_instruction);
}
void sim_pipe::run_MEM(){
  unsigned alu_temp;
  unsigned condition;
  instruction_t temp_instruction;
  unsigned memory = UNDEFINED;
  sp_register_bank->get_IR(MEM, temp_instruction);
  alu_temp = get_sp_register(ALU_OUTPUT, MEM);
  if(temp_instruction.opcode == LW || temp_instruction.opcode == SW){
    if(SH_stall_count < data_memory_latency){
      SH_stall_count++;
      stall_flag_SH = 1;
      temp_instruction = stall;
      alu_temp = UNDEFINED;
    }
    else{
      stall_flag_SH = 0;
      SH_stall_count = 0;
      if(temp_instruction.opcode == LW){
        memory = load_memory(alu_temp);
      }else if(temp_instruction.opcode == SW){
        write_memory(alu_temp, get_sp_register(B, MEM));
      }
    }
  }
  condition = get_sp_register(COND,MEM);
  sp_register_bank->set_MEMWB(memory, condition, temp_instruction, alu_temp);
}
void sim_pipe::run_WB(){
  instruction_t temp_instruction;
  sp_register_bank->get_IR(WB, temp_instruction);
  if( SW < temp_instruction.opcode && temp_instruction.opcode < BEQZ){
    set_gp_register(temp_instruction.dest, get_sp_register(ALU_OUTPUT, WB));
  }else if(temp_instruction.opcode == LW){
    set_gp_register(temp_instruction.dest, get_sp_register(LMD, WB));
  }
  if(temp_instruction.opcode != EOP && temp_instruction.opcode != NOP){
    num_instructions++;
  }
  if(temp_instruction.opcode == NOP){
    num_stalls++;
  }
}

unsigned sim_pipe::get_condition(opcode_t op){
  unsigned temp_cond, temp_A;
  int temp;
  instruction_t temp_instruction;
  sp_register_bank->get_IR(EXE,temp_instruction);

  temp_A = get_sp_register(A,EXE);
  if(temp_A == UNDEFINED || temp_instruction.opcode < BEQZ || temp_instruction.opcode > JUMP){
    return 0;
  }else{
    //temp_A = get_gp_register(temp_A);
    temp = (int) temp_A;
    switch(op){
      case BEQZ:
        temp_cond = (temp == 0);
      break;
      case BNEZ:
        temp_cond = (temp != 0);
      break;
      case BLTZ:
        temp_cond = (temp < 0);
      break;
      case BGTZ:
        temp_cond = (temp > 0);
      break;
      case BLEZ:
        temp_cond = (temp <= 0);
      break;
      case BGEZ:
          temp_cond = (temp >= 0);
      break;
      case JUMP:
        temp_cond = 0x00000001;
      break;
      default:
        temp_cond = 0x00000000;
      break;
    }
    return temp_cond;
  }
}


/* reset the state of the pipeline simulator */
/* Clear out all of the register values and make them zero */
void sim_pipe::reset(){
  for(int i = 0; i < NUM_GP_REGISTERS; i++){
    gp_registers[i] = UNDEFINED;
  }
  for(int i = 0; i < data_memory_size; i++){
    data_memory[i] = -1;
  }
  num_cycles = 0;
  num_stalls = -4;
  num_instructions = 0;
  EOP_flag = 0;
  stall_flag_DH = 0;
  stall_flag_CH = 0;
  CH_stall_count = 1;
  stall_flag_SH = 0;
  SH_stall_count = 0;
  stall.opcode = NOP;
  stall.src1 = UNDEFINED;
  stall.src2 = UNDEFINED;
  stall.dest = UNDEFINED;
  stall.immediate = UNDEFINED;
  stall.label = "";

}

//return value of special purpose register
unsigned sim_pipe::get_sp_register(sp_register_t reg, stage_t s){
  switch(s){
    case IF:
      return sp_register_bank->get_IF(reg);
    break;
    case ID:
      return sp_register_bank->get_IFID(reg);
    break;
    case EXE:
      return sp_register_bank->get_IDEX(reg);
    break;
    case MEM:
      return sp_register_bank->get_EXMEM(reg);
    break;
    case WB:
      return sp_register_bank->get_MEMWB(reg);
    break;
    default:
      return UNDEFINED;
    break;
  }
}

//returns value of general purpose register
int sim_pipe::get_gp_register(unsigned reg){
	return gp_registers[reg]; //please modify
}

void sim_pipe::set_gp_register(unsigned reg, unsigned value){
  gp_registers[reg] = value;
}

float sim_pipe::get_IPC(){
        float ipc = (float)(num_instructions)/(float)(num_cycles);
        return ipc; //please modify
}

unsigned sim_pipe::get_instructions_executed(){
        return num_instructions; //please modify
}

unsigned sim_pipe::get_stalls(){
        return num_stalls;
}

unsigned sim_pipe::get_clock_cycles(){
        return num_cycles; //please modify
}

void sim_pipe::check_insert_DH_stall(instruction_t ID_instruction, unsigned ID_src1, unsigned ID_src2){
  instruction_t MEM_instruction, WB_instruction;
  sp_register_bank->get_IR(MEM, MEM_instruction);
  sp_register_bank->get_IR(WB, WB_instruction);
  if(ID_instruction.opcode != NOP && ID_instruction.opcode != EOP){
    if(((ID_src1 == MEM_instruction.dest || ID_src2 == MEM_instruction.dest) && MEM_instruction.dest != UNDEFINED) ||
       ((ID_src1 == WB_instruction.dest  || ID_src2 == WB_instruction.dest ) && WB_instruction.dest  != UNDEFINED)){
      stall_flag_DH = 1;
    }else{
      stall_flag_DH = 0;
    }
  }
  else{
    stall_flag_DH = 0;
  }
}

void sim_pipe::insert_stall_fetch(){
  unsigned current_pc;
  current_pc = get_sp_register(PC,IF);
  sp_register_bank->set_IFID(UNDEFINED, stall);
}

instruction_t sim_pipe::get_IR(stage_t s){
  instruction_t temp;
  sp_register_bank->get_IR(s,temp);
  return temp;
}
