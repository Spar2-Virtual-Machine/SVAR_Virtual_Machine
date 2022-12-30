#ifndef __VIRT_H_
#define __VIRT_H_
#include "AllocationTable.h"


void init_platform();
void cleanup_platform();

//hidden functions that the user should not call
void MoveDataToSPAR(int vReg);

//Primitive functions.
void Declare_M(Matrix *m, int rows, int cols);
void Deallocate_M(Matrix *m);
void Reset_Registers();

//
void Store_M(Matrix* m, int rd, AllocationTable *table);
void Store_M_Transpose(Matrix *m, int rd, AllocationTable *table);
void Load_M(Matrix *m, int rs);

//compound




#endif
