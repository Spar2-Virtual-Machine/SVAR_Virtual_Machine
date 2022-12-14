/*
 * AllocationTable.h
 *
 *  Created on: Nov 17, 2022
 *      Author: Njfre
 */

//todo--> need ot capitalize the first letter of each function. I am too used to Kotlin standards

#ifndef SRC_ALLOCATIONTABLE_H_
#define SRC_ALLOCATIONTABLE_H_

#define Num_VREG 16
#define Num_PREG 16

typedef struct Matrix{
	int *memory; //type subject to change. May convert to the fixed point type
	int rows; //type subject to change
	int cols;
	int orientation; //0=regular 1=transpose. Need to know orientation before multiplication and other operations. -1 if just in memory (not spar)
	int *placement;
} Matrix;


typedef struct Vector{
	int *vector;
	int *placement; //placement should only be touched inside of the allocation table functions
	int size;
	int orientation; //0 = stored in columns, 1 = stored in rows
} Vector;

typedef struct VRegData{ //raw data representation of a virtual register (made of data from multiple p reg). used to hold temp data that is not declared as a variable yet.
	int *data;
	int rows;
	int cols;
}VRegData;

typedef struct VReg{
	void *m; //void pointer to a matrix or vector
	int type; //0 means matrix. 1 means vector. 2 means scalar. 10 means VRegData
	int *placement; //an array that keeps track of the different parts of a matrix. //placement should only be touched inside of the allocation table functions
	int status; //more general placement of the data. -1 = empty. 0 means in memory. 1 means in SPAR. 2 means in spar not matching memory. 3 invalid in spar.
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

void allocateVRegM(Matrix *m, int vRegNum, AllocationTable *table);
void allocateVRegV(Vector *v, int vRegNum, AllocationTable *table);
void safeAllocateReg(int vRegNum, int maxArraySize, int protectedVReg[], int numProtected, AllocationTable *table);

void removeVRegFromPRegs(int vRegNum, AllocationTable *table);
void loadToVRegDataToPReg(int vRegNum, int pRegNum, int startRow, int startCol, int endRow, int endCol, AllocationTable *table);
void storePRegToMem(int pRegNum, int startRow, int startCol, int endRow, int endCol, Matrix *m); //move data from a

void printTableVReg(AllocationTable *table);
void printTablePReg(AllocationTable *table);

#endif /* SRC_ALLOCATIONTABLE_H_ */
