# PAGsuite

This is a `git` mirror of:
https://digidev.digi.e-technik.uni-kassel.de/pagsuite/

Copyright of the code belongs to its original authors/contributors and/or Kassel university.

Contacts:
 - Martin Kumm <kumm(at)uni-kassel.de>
 - Konrad Möller <konrad.moeller(at)uni-kassel.de> 

The suite contains optimization tools for the pipelined multiple constant multiplication (PMCM) problem, i.e., the multiplication of a single variable with multiple constants using bit-shifts, registered adders/subtractors and registers, such that a register is placed after each addition/subtraction and the pipeline is valid. Including the pipeline registers in the optimization is extremly usefull for FPGA-based designs but is also benefically for ASIC designs [1-3]. As PMCM is a generalization of the MCM problem (without pipelining), it can also be used to find MCM solutions with minimal adder depth. 

Three algorithms are provided with PAGSuite, the heuristic RPAG, the optimal ILP-based tool OPAG and PAG fusion
