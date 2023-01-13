#ifndef __VIRT_H_
#define __VIRT_H_
#include "AllocationTable.h"


void init_platform();
void cleanup_platform();

//hidden functions that the user should not call
void EastLoopAround(int prs, int prd, AllocationTable *table);
void WestLoopAround(int prs, int prd, AllocationTable *table);
void SouthLoopAround(int prs, int prd, AllocationTable *table);
void ShiftEast_M( int rs, int rd, AllocationTable *table);
void ShiftWest_M(int rs, int rd, AllocationTable *table);
void ShiftSouth_M(int rs, int rd, AllocationTable *table);
void CopyVector(int rs, int numberOfCopies, AllocationTable *table);

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
//Arithmetic
void E_Add_MM(int rs1, int rs2, int rd, AllocationTable *table);
void E_Sub_MM(int rs1, int rs2, int rd, AllocationTable *table);
void E_Mul_MM(int rs1, int rs2, int rd, AllocationTable *table);
void E_Add_VV(int rs1, int rs2, int rd, AllocationTable *table);
//compound
void Accumulate_M(int rs, int rd, int tempReg, AllocationTable *table);


#endif
