Emulates a simplified ARM assembler and instruction set, taking a text file with LEGv8 “ARM” derivative instructions, and performing the operations to support function calls. It was written in a text file and compiled using a GCC compiler on an Ubuntu build inside Windows Subsystem Linux.

The available instructions are: ADD, ADDI, SUB, SUBI, LDUR, STUR, LSL, CBZ, CBNZ, BR, BL, B. Uses a multi-dimensional array to emulate stack memory as well as an array for registers.

The included instruction file populates registers then calls a function that completes what would roughly translate to the following C code:

if (n > 0) 
  z = z << 2;
  n--

return z;

Useful, I know! But was fun learning.
