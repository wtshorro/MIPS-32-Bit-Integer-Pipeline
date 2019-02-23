#include "../include/sp_registers.h"
#include <string>

using namespace std;

sp_registers::sp_registers(){
  reset();
}

void sp_registers::reset(){
  IF.PC              = UNDEFINED;

  IFID.NPC           = UNDEFINED;
  IFID.IR.opcode     = NOP;
  IFID.IR.src1       = UNDEFINED;
  IFID.IR.src2       = UNDEFINED;
  IFID.IR.dest       = UNDEFINED;
  IFID.IR.immediate  = UNDEFINED;
  IFID.IR.label      = "";

  IDEX.A             = UNDEFINED;
  IDEX.B             = UNDEFINED;
  IDEX.IMM           = UNDEFINED;
  IDEX.NPC           = UNDEFINED;
  IDEX.IR.opcode     = NOP;
  IDEX.IR.src1       = UNDEFINED;
  IDEX.IR.src2       = UNDEFINED;
  IDEX.IR.dest       = UNDEFINED;
  IDEX.IR.immediate  = UNDEFINED;
  IDEX.IR.label      = "";

  EXMEM.IR.opcode    = NOP;
  EXMEM.IR.src1      = UNDEFINED;
  EXMEM.IR.src2      = UNDEFINED;
  EXMEM.IR.dest      = UNDEFINED;
  EXMEM.IR.immediate = UNDEFINED;
  EXMEM.IR.label     = "";
  EXMEM.B            = UNDEFINED;
  EXMEM.condition    = UNDEFINED;
  EXMEM.ALU_out      = UNDEFINED;

  MEMWB.ALU_out      = UNDEFINED;
  MEMWB.LMD          = UNDEFINED;
  MEMWB.IR.opcode    = NOP;
  MEMWB.IR.src1      = UNDEFINED;
  MEMWB.IR.src2      = UNDEFINED;
  MEMWB.IR.dest      = UNDEFINED;
  MEMWB.IR.immediate = UNDEFINED;
  MEMWB.condition    = UNDEFINED;
  MEMWB.IR.label     = "";
}
void sp_registers::print_sp_registers(){
    printf("IF current pc       %x\n", IF.PC);
    printf("-----------------------------------------------\n");
    printf("ID current NPC:     %x\n",IFID.NPC);
    printf("ID current opcode:  %s\n", opcodeToString(IFID.IR.opcode).c_str());
    printf("ID SRC 1:           %d\n", IFID.IR.src1);
    printf("ID SRC 2:           %d\n", IFID.IR.src2);
    printf("ID DEST:            %d\n", IFID.IR.dest);
    printf("ID IMM:             %d\n", IFID.IR.immediate);
    printf("-----------------------------------------------\n");
    printf("EXE current NPC:    %x\n",IDEX.NPC);
    printf("EXE current opcode: %s\n", opcodeToString(IDEX.IR.opcode).c_str());
    printf("EXE SRC 1:          %d\n", IDEX.IR.src1);
    printf("EXE SRC 2:          %d\n", IDEX.IR.src2);
    printf("EXE DEST:           %d\n", IDEX.IR.dest);
    printf("EXE IMM:            %d\n", IDEX.IR.immediate);
    printf("EXE A:              %d\n", IDEX.A);
    printf("EXE B:              %d\n", IDEX.B);
    printf("-----------------------------------------------\n");
    printf("MEM current opcode: %s\n", opcodeToString(EXMEM.IR.opcode).c_str());
    printf("MEM SRC 1:          %d\n", EXMEM.IR.src1);
    printf("MEM SRC 2:          %d\n", EXMEM.IR.src2);
    printf("MEM DEST:           %d\n", EXMEM.IR.dest);
    printf("MEM IMM:            %d\n", EXMEM.IR.immediate);
    printf("MEM B:              %d\n", EXMEM.B);
    printf("MEM ALU_OUT:        %d\n", EXMEM.ALU_out);
    printf("MEM COND:           %d\n", EXMEM.condition);
    printf("-----------------------------------------------\n");
    printf("WB current opcode:  %s\n", opcodeToString(MEMWB.IR.opcode).c_str());
    printf("WB SRC 1:           %d\n", MEMWB.IR.src1);
    printf("WB SRC 2:           %d\n", MEMWB.IR.src2);
    printf("WB DEST:            %d\n", MEMWB.IR.dest);
    printf("WB IMM:             %d\n", MEMWB.IR.immediate);
    printf("WB LMD:             %d\n", MEMWB.LMD);
    printf("WB ALU_OUT:         %d\n", MEMWB.ALU_out);
    printf("WB COND:            %d\n", MEMWB.condition);
    printf("-----------------------------------------------\n");
}
string sp_registers::opcodeToString(opcode_t op){
  string opcode;
  switch(op){
    case ADD:
      opcode = "ADD";
    case LW:
      opcode = "LW";
    break;
    case SW:
      opcode = "SW";
    break;
    case ADDI:
      opcode = "ADDI";
    break;
    case SUB:
      opcode = "SUB";
    break;
    case SUBI:
      opcode = "SUBI";
    break;
    case XOR:
      opcode = "XOR";
    break;
    case BEQZ:
      opcode = "BEQZ";
    break;
    case BNEZ:
      opcode = "BNEZ";
    break;
    case BLTZ:
      opcode = "BLTZ";
    break;
    case BGTZ:
      opcode = "BGTZ";
    break;
    case BLEZ:
      opcode = "BLEZ";
    break;
    case BGEZ:
      opcode = "BGEZ";
    break;
    case JUMP:
      opcode = "JUMP";
    break;
    case EOP:
      opcode = "EOP";
    break;
    case NOP:
      opcode = "NOP";
    break;
    default:
      opcode = "UNDEFINED OPCODE";
    break;
  }
  return opcode;
}
void sp_registers::set_IF(unsigned next_PC){
  IF.PC = next_PC;
}
unsigned sp_registers::get_IF(sp_register_t val){
  if(val == PC){
    return IF.PC;
  }
  else{
    return UNDEFINED;
  }
}

void sp_registers::set_IFID(unsigned npc, instruction_t new_instruction){
  IFID.NPC = npc;
  IFID.IR.opcode    = new_instruction.opcode;
  IFID.IR.src1      = new_instruction.src1;
  IFID.IR.src2      = new_instruction.src2;
  IFID.IR.dest      = new_instruction.dest;
  IFID.IR.immediate = new_instruction.immediate;
  IFID.IR.label     = new_instruction.label;
}
unsigned sp_registers::get_IFID(sp_register_t val){
  switch(val){
    case NPC:
      return IFID.NPC;
    break;
    default:
      return UNDEFINED;
    break;
  }
}

unsigned sp_registers::get_IDEX(sp_register_t val){
  switch(val){
    case A:
      return IDEX.A;
    break;
    case B:
      return IDEX.B;
    break;
    case IMM:
      return IDEX.IMM;
    break;
    case NPC:
      return IDEX.NPC;
    break;
    default:
      return UNDEFINED;
    break;
  }
}
void sp_registers::set_IDEX(unsigned a, unsigned b, unsigned imm, unsigned npc, instruction_t new_instruction){
  IDEX.A             = a;
  IDEX.B             = b;
  IDEX.IMM           = imm;
  IDEX.NPC           = npc;
  IDEX.IR.opcode     = new_instruction.opcode;
  IDEX.IR.src1       = new_instruction.src1;
  IDEX.IR.src2       = new_instruction.src2;
  IDEX.IR.dest       = new_instruction.dest;
  IDEX.IR.immediate  = new_instruction.immediate;
  IDEX.IR.label      = new_instruction.label;
}

unsigned sp_registers::get_EXMEM(sp_register_t val){
  switch(val){
    case B:
      return EXMEM.B;
    break;
    case ALU_OUTPUT:
      return EXMEM.ALU_out;
    break;
    case COND:
      return EXMEM.condition;
    break;
    default:
      return UNDEFINED;
    break;
  }
}
void sp_registers::set_EXMEM(unsigned b, unsigned alu_out, unsigned cond, instruction_t new_instruction){
  EXMEM.B            = b;
  EXMEM.ALU_out      = alu_out;
  EXMEM.condition    = cond;
  EXMEM.IR.opcode    = new_instruction.opcode;
  EXMEM.IR.src1      = new_instruction.src1;
  EXMEM.IR.src2      = new_instruction.src2;
  EXMEM.IR.dest      = new_instruction.dest;
  EXMEM.IR.immediate = new_instruction.immediate;
  EXMEM.IR.label     = new_instruction.label;
}

unsigned sp_registers::get_MEMWB(sp_register_t val){
  switch(val){
    case ALU_OUTPUT:
      return MEMWB.ALU_out;
    break;
    case LMD:
      return MEMWB.LMD;
    break;
    case COND:
      return MEMWB.condition;
    break;
    default:
    return UNDEFINED;
    break;
  }
}
void sp_registers::set_MEMWB(unsigned lmd, unsigned condition, instruction_t new_instruction, unsigned alu_out){
  MEMWB.ALU_out      = alu_out;
  MEMWB.LMD          = lmd;
  MEMWB.condition    = condition;
  MEMWB.IR.opcode    = new_instruction.opcode;
  MEMWB.IR.src1      = new_instruction.src1;
  MEMWB.IR.src2      = new_instruction.src2;
  MEMWB.IR.dest      = new_instruction.dest;
  MEMWB.IR.immediate = new_instruction.immediate;
  MEMWB.IR.label     = new_instruction.label;
}

bool sp_registers::get_IR(stage_t val, instruction_t& instruction){
  switch(val){
    case ID:
      instruction = IFID.IR;
      return true;
    break;
    case EXE:
      instruction = IDEX.IR;
      return true;
    break;
    case MEM:
      instruction = EXMEM.IR;
      return true;
    break;
    case WB:
      instruction = MEMWB.IR;
      return true;
    break;
    default:
      return false;
    break;
  }
}
