# RISC-V Processor Pipeline Simulator

## Description
This project is a simulator for the RISC-V processor pipeline. It reads a trace file containing the sequence of instructions executed by the processor and describes the state of the pipeline in each clock cycle.

## Submission Guidelines
1. The simulator should be written in the C language and submitted in a single file named `riscv.c`.
2. Additionally, copy the entire program to a single WORD file. Indicate the question number before each program.
3. The output of the simulator should be submitted in a separate file for each run.
4. All files should be compressed into a single ZIP file and uploaded to the website.
5. Document the code by adding comments.

## Project Requirements
1. **Input**: The simulator input is provided by a trace file describing the commands executed in the program. Each line in the file follows a specific format:
- `<instruction address>`: Memory address of the command
- `<opcode>`: Opcode of the command (e.g., add, sub, lw, sw)
- `<operand1-3>`: Operands of the command according to its format
2. **Simulation of the RISC-V pipeline**: The simulator displays the command in each stage of the RISC-V pipeline in each clock cycle, handling stalls. It calculates the CPI of the program at the end.
- Data and command memory accesses occur in the MEM and Fetch stages, respectively, each taking one clock cycle.
- Execution in the Ex stage requires one clock cycle.
- The processor uses static prediction of predict not taken.
- Command line parameters: First parameter for forwarding, second parameter for branch resolution.
3. **Output**: The output is formatted for each clock cycle, displaying the command in each stage and any stalls encountered. The final output includes the CPI of the program.
4. **Execution**: The simulator should be run with different parameters and trace files according to the provided table.

## Execution
The simulator should be run according to the following table:

| Run | Trace file | Forwarding | Branch resolution |
|-----|------------|------------|-------------------|
| 1   | trace1.txt | 0          | 0                 |
| 2   | trace1.txt | 1          | 0                 |
| 3   | trace1.txt | 0          | 1                 |
| 4   | trace1.txt | 1          | 1                 |
| 5   | trace2.txt | 1          | 1                 |

## Files Included
- `riscv.c`: Source code file for the RISC-V processor pipeline simulator.
- `trace1.txt`: Sample trace file for testing.
- `trace2.txt`: Additional trace file for testing.
- `README.md`: This README file providing information about the project.

## Usage
To run the simulator, compile `riscv.c` and execute it with the appropriate command line parameters and trace file.


## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
