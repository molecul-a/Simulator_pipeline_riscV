Project Description:
A simulator for the RISC-V processor pipeline is required. The simulator should read a trace file containing 
the sequence of instructions executed by the processor. The simulator should describe the state of 
the pipeline in each clock cycle as specified.

Submission Guidelines:

1. The simulator should be written in the C language and submitted in a single file named riscv.c.
2. Additionally, copy the entire program to a single WORD file. Indicate the question number before each program.
3. The output of the simulator should be submitted in a separate file for each run.
4. All files should be compressed into a single ZIP file and uploaded to the website.
5. Document the code by adding comments.

   
Project Requirements:

1. Input - The simulator input is provided by a trace file describing the commands executed in the program. The format of each line in the file is as follows:
<instruction address> <opcode> <operand1> <operand2> <operand3>
<instruction address> - Memory address of the command
<opcode> - The opcode of the command. The supported opcodes are:
add, sub, or, and, addi, subi, ori, andi, lw, sw, j, beq, bneq
<operand1-3> are the operands of the command according to its format. The leftmost operand is the one written by the command.
Each field in each row is separated by a space. For example:
100 add x1 x2 x3 //x1=x2+x3
104 lw x2 100 x4 //x2=Mem[100+x4]
108 subi x3 x10 1 //x3=x10-1
109 beq x3 x0 label //if(x3==x0) goto label

2. Simulation of the RISC-V pipeline - The simulator should display the command in each stage of the RISC-V pipeline in each clock cycle. 
If there is a stall, it should be displayed in the output. At the end of the simulation, the simulator should calculate the CPI of the program.
Data:
Accesses to data and command memory occur in the MEM and Fetch stages respectively, and each takes one clock cycle.
Executing commands in the Ex stage requires one clock cycle.
The processor uses static prediction of predict not taken.
The simulator receives 2 parameters on the command line:
First parameter: 0 run without forwarding, 1 processor supports all possible types of forwarding.
Second parameter: 0 branch resolution occurs in MEM stage, 1 branch resolution occurs in ID stage.
For example, running the command mips.exe 1 1 will run the simulator in forwarding support mode, and branch resolution will occur in the ID stage.

3. Output - The output should be formatted as follows for each clock cycle:
Cycle 30:
Fetch instruction: lw x10 2 x8
Decode instruction: sw x8 12 x0
Execute instruction: subi x9 x0 -25
Memory instruction: subi x8 x0 -10
Writeback instruction: stall
Cycle 31:
Fetch instruction: stall
Decode instruction: lw x10 2 x8
Execute instruction: sw x8 12 x0
Memory instruction: subi x9 x0 -25
Writeback instruction: subi x8 x0 -10
CPI: 1

4. Execution - The simulator should be run according to the following table:
Run	Trace file	Forwarding    	Branch resolution
1	  trace1.txt	    0	                0
2	  trace1.txt	    1                 0
4	  trace1.txt	    1	                1
3	  trace1.txt	    0	                1
5	  trace2.txt	    1	                1
