#ifndef __VIRT_H_
#define __VIRT_H_
#include "AllocationTable.h"


void init_platform();
void cleanup_platform();

//hidden functions that the user should not call
void SafelyMoveToAnotherPREG(int preg, int reservedPregs[], int numberOfReserved, AllocationTable *table);
void MoveToAnotherPREG(int rs, int rd, AllocationTable *table);
void EastLoopAround(int prs, int prd, AllocationTable *table);
void WestLoopAround(int prs, int prd, AllocationTable *table);
void SouthLoopAround(int prs, int prd, AllocationTable *table);
void ShiftEast_M( int rs, int rd, AllocationTable *table);
void ShiftWest_M(int rs, int rd, AllocationTable *table);
void ShiftSouth_M(int rs, int rd, AllocationTable *table);
//void CopyVector(int rs, int tempr, int numberOfCopies, AllocationTable *table); //DEPRECATED. Not Necessary //copying vector for vector-matrix multiplication
void FillVector(int rs, int tempr, AllocationTable *table); //copy vector except it wills up an entire preg of width. Needed for vector matrix multiplication
void CastRegTo_V(int rs, int orientation, AllocationTable *table); //cast a matrix to vector
void PrepareReg_Mul_MVM(int rsm, int rsv, int rd, AllocationTable *table);//prepares registers for

//Primitive functions.
void Declare_M(Matrix *m, int rows, int cols);
void Declare_V(Vector *v, int size);
void Deallocate_M(Matrix *m);
void Deallocate_V(Vector *v);

void Reset_Registers();
void Store_M(Matrix* m, int rd, AllocationTable *table);
void Store_M_Transpose(Matrix *m, int rd, AllocationTable *table);
void Load_M(Matrix *m, int rs, AllocationTable *table);
void Store_V(Vector* v, int rd, AllocationTable *table);
void Load_V(Vector* v, int rs, AllocationTable *table);
//Arithmetic
void E_Add_MM(int rs1, int rs2, int rd, AllocationTable *table);
void E_Sub_MM(int rs1, int rs2, int rd, AllocationTable *table);
void E_Mul_MM(int rs1, int rs2, int rd, AllocationTable *table);
void E_Add_VV(int rs1, int rs2, int rd, AllocationTable *table);
void E_Sub_VV(int rs1, int rs2, int rd, AllocationTable *table);
void E_Mul_VV(int rs1, int rs2, int rd, AllocationTable *table);
//compound
void AccumulateColumns_M(int rs, int rd, int tempReg, AllocationTable *table);
void Mul_MV(int rs_m, int rs_v, int rd, int temp, AllocationTable *table);

#endif
