Overall Setup:
--Spar.h: Includes the basic library created by Atiyeh
--AllocationTable.h: Includes all the functionality for the allocation table and virtual registers. Includes loading and storing data
--Virt.h: All of the virtual machine instructions.
--Test.h: includes different tests for functionality. None of the tests self-check, so outputs have to be verified by the user with a serial terminal.
--MLP_Benchmarks.h: set of functions for testing MLP network on specifically sized SPAR configurations.

To start:
First create a single instance of AllocationTable.
Reset the table using resetTable();
Reset registers to clear the physical registers.

Things to note:
--Only functions in virt.h and AllocationTable.h will be tracked by the virtual machine. Any other function calls will not and may lead to unintentional behavior.
--The total number of virtual registers is defined as Num_VREG
--The total number of physical registers is defined as Num_PREG
--The Maximum numer of physical registers any virtual register can use is Max_PrForVr (currently 8)
--VReg_Data_Size is the maximum number of elements that can be stored in a virtual register. Going over this will results in altering the data of other virtual registers. Increasing this number will also increase the amount of memory reserved for virtual register data.
--minimumNumOfVReg is the minimum number of virtual registers that need to be in the SPAR physical registers to complete any given instruction.
--multiplicationOverFlow tells the system that multiplication will result in the register above the destination being affected.
--All data in the virtual registers is stored linearly in memory. Alterations to the 'rows' or 'cols' of a virtual register will result in the data being read out incorrectly.
--Placement[] in the virtual registers are the physical register numbers that a segment of data lies in. -1 means invalid (no segment being used).
--Status represents the current state of data. -1 is empty/invalid. 0 means that virtual register data is in memory. 1 means that virtual register data is in SPAR registers.
--Data is first moved to reserved memory. Only when a function needs to use a virtual register is it loaded to the physical registers on SPAR.
