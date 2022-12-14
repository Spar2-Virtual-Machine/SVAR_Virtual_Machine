#ifndef __VIRT_H_
#define __VIRT_H_
#include "AllocationTable.h"


void init_platform();
void cleanup_platform();

//Primitive functions.
void VIRT_Transpose(int regNum); //Gives the transpose of an array or rotates a vector. Essentially column-to-row
void VIRT_Activation_CtoR(int regSource, int regDes, int activationFunction); //activation function using column to row
void VIRT_Activation_CtoC(int regSource, int regDes, int activationFunction);

void Declare_M(Matrix *m, int rows, int cols);
void Deallocate_M(Matrix *m);
void Reset_Registers();

//
void VIRT_E_Add(int rs1, int rs2, int rd);
void VIRT_E_Sub(int rs1, int rs2, int rd);
void VIRT_E_Mul(int rs1, int rs2, int rd);
void VIRT_E_Relu(int rs1, int rs2, int rd);
void VIRT_North(int rs, int rd);
void VIRT_South(int rs, int rd);
void VIRT_East(int rs, int rd);
void VIRT_West(int rs, int rd);

void Store_M(Matrix* m, int rd, AllocationTable *table);
void Store_M_Transpose(Matrix *m, int rd, AllocationTable *table);
void Load_M(Matrix *m, int rs);

//compound




#endif
