#ifndef SIM_PIPE_H_
#define SIM_PIPE_H_
#include "sp_registers.h"
#include <stdio.h>
#include <string>


using namespace std;

#define PROGRAM_SIZE 50
#define NUM_SP_REGISTERS 9
#define NUM_GP_REGISTERS 32
#define NUM_OPCODES 16
#define NUM_STAGES 5
class sim_pipe{

	/* Add the data members required by your simulator's implementation here */
  unsigned num_cycles;
  unsigned EOP_flag;
  unsigned stall_flag_DH;
  unsigned stall_flag_CH;
  unsigned CH_stall_count;
  unsigned stall_flag_SH;
  unsigned SH_stall_count;
  unsigned num_instructions;
  unsigned num_stalls;

  instruction_t stall;
  //instruction memory
  instruction_t instr_memory[PROGRAM_SIZE];

  //base address in the instruction memory where the program is loaded
  unsigned instr_base_address;

	//data memory - should be initialize to all 0xFF
	unsigned char *data_memory;

	//memory size in bytes
	unsigned data_memory_size;

	//memory latency in clock cycles
	unsigned data_memory_latency;

  //general purpose registers
  unsigned int* gp_registers;

  //special purpose registers
  sp_registers* sp_register_bank;
public:
	//instantiates the simulator with a data memory of given size (in bytes) and latency (in clock cycles)
	/* Note:
     - initialize the registers to UNDEFINED value
	   - initialize the data memory to all 0xFF values
	*/
	sim_pipe(unsigned data_mem_size, unsigned data_mem_latency);

	//de-allocates the simulator
	~sim_pipe();

	//loads the assembly program in file "filename" in instruction memory at the specified address
	void load_program(const char *filename, unsigned base_address=0x0);

	//runs the simulator for "cycles" clock cycles (run the program to completion if cycles=0)
	void run(unsigned cycles=0);
  void run_IF();
  void run_ID();
  void run_EXE();
  void run_MEM();
  void run_WB();
  unsigned get_condition(opcode_t op);

	//resets the state of the simulator
        /* Note:
	   - registers should be reset to UNDEFINED value
	   - data memory should be reset to all 0xFF values
	*/
	void reset();

	// returns value of the specified special purpose register for a given stage (at the "entrance" of that stage)
        // if that special purpose register is not used in that stage, returns UNDEFINED
        //
        // Examples (refer to page C-37 in the 5th edition textbook, A-32 in 4th edition of textbook)::
        // - get_sp_register(PC, IF) returns the value of PC
        // - get_sp_register(NPC, ID) returns the value of IF/ID.NPC
        // - get_sp_register(NPC, EX) returns the value of ID/EX.NPC
        // - get_sp_register(ALU_OUTPUT, MEM) returns the value of EX/MEM.ALU_OUTPUT
        // - get_sp_register(ALU_OUTPUT, WB) returns the value of MEM/WB.ALU_OUTPUT
	// - get_sp_register(LMD, ID) returns UNDEFINED
	/* Note: you are allowed to use a custom format for the IR register.
           Therefore, the test cases won't check the value of IR using this method.
	   You can add an extra method to retrieve the content of IR */
	unsigned get_sp_register(sp_register_t reg, stage_t stage);
	//returns value of the specified general purpose register
	int get_gp_register(unsigned reg);
	// set the value of the given general purpose register to "value"
	void set_gp_register(unsigned reg, unsigned value);
	//returns the number of instructions fully executed
	unsigned get_instructions_executed();
	//returns the number of clock cycles
	unsigned get_clock_cycles();
	//returns the number of stalls added by processor
	unsigned get_stalls();
  float get_IPC();
	//prints the content of the data memory within the specified address range
	void print_memory(unsigned start_address, unsigned end_address);
	// writes an integer value to data memory at the specified address (use little-endian format: https://en.wikipedia.org/wiki/Endianness)
	void write_memory(unsigned address, unsigned value);
  unsigned load_memory(unsigned address);
	//prints the values of the registers
	void print_registers();
  void check_insert_DH_stall(instruction_t, unsigned, unsigned);
  void insert_stall_fetch();
  instruction_t get_IR(stage_t);

};

#endif /*SIM_PIPE_H_*/
