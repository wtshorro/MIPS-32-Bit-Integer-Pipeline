#ifndef SP_REGISTERS_H
#define SP_REGISTERS_H
#include <stdio.h>
#include <string>

using namespace std;

#define UNDEFINED 0xFFFFFFFF
#define ZERO      0x00000000
typedef enum {IF, ID, EXE, MEM, WB} stage_t;
typedef enum {PC, NPC, IR, A, B, IMM, COND, ALU_OUTPUT, LMD} sp_register_t;
typedef enum {LW, SW, ADD, ADDI, SUB, SUBI, XOR, BEQZ, BNEZ, BLTZ, BGTZ, BLEZ, BGEZ, JUMP, EOP, NOP} opcode_t;
typedef struct{
        opcode_t opcode; //opcode
        unsigned src1; //first source register in the assembly instruction (for SW, register to be written to memory)
        unsigned src2; //second source register in the assembly instruction
        unsigned dest; //destination register
        unsigned immediate; //immediate field
        string label; //for conditional branches, label of the target instruction - used only for parsing/debugging purposes
} instruction_t;


typedef struct{
  unsigned PC;
} fetch_registers;

typedef struct{
  unsigned NPC;
  instruction_t IR;
} pipe_stage_one;

typedef struct{
  unsigned A;
  unsigned B;
  unsigned IMM;
  unsigned NPC;
  instruction_t IR;
} pipe_stage_two;

typedef struct{
  unsigned ALU_out;
  unsigned B;
  unsigned condition;
  instruction_t IR;
} pipe_stage_three;

typedef struct{
  unsigned LMD;
  instruction_t IR;
  unsigned ALU_out;
  unsigned condition;
} pipe_stage_four;



class sp_registers{
  fetch_registers IF;
  pipe_stage_one IFID;
  pipe_stage_two IDEX;
  pipe_stage_three EXMEM;
  pipe_stage_four MEMWB;
public:
  sp_registers();
  void reset();
  //IF stage accessor methods
  unsigned get_IF(sp_register_t);
  void set_IF(unsigned);
  //ID stage accessor methods
  unsigned get_IFID(sp_register_t);
  void set_IFID(unsigned, instruction_t);
  //EXE stage accessor methods
  unsigned get_IDEX(sp_register_t);
  void set_IDEX(unsigned, unsigned, unsigned, unsigned, instruction_t);
  //MEM stage accessor methods
  unsigned get_EXMEM(sp_register_t);
  void set_EXMEM(unsigned, unsigned, unsigned, instruction_t);
  //WB stage accessor methods
  unsigned get_MEMWB(sp_register_t);

  void set_MEMWB(unsigned, unsigned, instruction_t, unsigned);

  string opcodeToString(opcode_t op);
  void print_sp_registers();

  bool get_IR(stage_t, instruction_t&);
};
#endif
