/*
 * AllocationTable.h
 *
 *  Created on: Nov 17, 2022
 *      Author: Njfre
 */

//todo--> need ot capitalize the first letter of each function. I am too used to Kotlin standards

#ifndef SRC_ALLOCATIONTABLE_H_
#define SRC_ALLOCATIONTABLE_H_

#define Num_VREG 8
#define Num_PREG 16
#define Max_Num_PREG_Per_VREG 6
#define VREG_Data_Size 3200 //arbitrarily chosen for now

typedef struct Matrix{
	int *memory; //type subject to change. May convert to the fixed point type
	int rows; //type subject to change
	int cols;
} Matrix;


typedef struct Vector{
	int *memory;
	int size;
} Vector;

typedef struct VReg{
	void *m; //void pointer to a matrix or vector. This is just kept here to load the data in. Not used again after loading.

	int data[VREG_Data_Size]; //data that will be kept in memory
	int rows;
	int cols;
	int orientation; //0=regular 1=transpose. Need to know orientation before multiplication and other operations. -1 if just in memory (not spar)

	int type; //0 means matrix. 1 means vector. 2 means scalar
	int placement[Max_Num_PREG_Per_VREG]; //an array that keeps track of the different parts of a matrix. //placement should only be touched inside of the allocation table functions
	int status; //more general placement of the data. -1 = empty. 0 means in memory. 1 means in SPAR. 2 means in spar not matching memory. 3 invalid in spar.  5 Means the data is set in m but not in the VReg data
}VReg;

typedef struct AllocationTable
{
	VReg vreg[Num_VREG]; //pointers to matrix vector structures.
	int preg[Num_PREG]; //number here represents what vreg is taking up that preg; -1 means empty
	int nextRegToUpdate; //round robin counter.
	//todo--> add a way to store temporary vreg data in memory so that it is not lost.
}AllocationTable;

void resetPRegs(AllocationTable *table);
void resetVRegs(AllocationTable *table);
void resetTable(AllocationTable *table);

void allocateVRegM(Matrix *m, int vRegNum, int orientation, AllocationTable *table);
void allocateVRegM_T(Matrix *m, int vRegNum, int orientation, AllocationTable *table);
void allocateVRegV(Vector *v, int vRegNum, AllocationTable *table);
void safeAllocatePRegs(int vRegNum, int maxDim, int protectedVReg[], int numProtected, AllocationTable *table);
void safeAllocateEmptyPRegs(int vRegNum, int maxDim, int protectedVReg[], int numProtected, AllocationTable *table); //allocate without loading new data to SPAR

void removeVRegFromPRegs(int vRegNum, AllocationTable *table);
void loadVRegDataToPReg_M(int vRegNum, int pRegNum, int startRow, int startCol, int endRow, int endCol, AllocationTable *table);
void loadVRegDataToPReg_V(int vRegNum, int pRegNum, int startRow, int endRow, AllocationTable *table);
void copyFromPRegsToVRegData(int vRegNum, AllocationTable *table); //move data from SPAR to memory

void printTableVReg(AllocationTable *table);
void printTablePReg(AllocationTable *table);
void printVRegData(int reg, AllocationTable *table);
void printVReginPReg(int reg, AllocationTable *table); //prints off the data in the PREGS as one ginat 2d array


#endif /* SRC_ALLOCATIONTABLE_H_ */
