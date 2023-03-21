#ifndef __SPAR_H_
#define __SPAR_H_

#include <stdint.h>

//define values
#define MAX_LEN 32
#define FIXED_POINT_FRACTIONAL_BITS 16
#define Array_dim  4
#define Tile_dim  4
#define divide_size_W 4
#define divide_size_U 4
#define m1 4						//size of hidden layers
#define p 4							//size of inputs
#define q 1							//size of outputs
#define epoch 1
extern int sparAdd_cnt;
extern int sparSub_cnt;
extern int sparMul_cnt;
extern int sparShN_cnt;
extern int sparShS_cnt;
extern int sparShE_cnt;
extern int sparShW_cnt;
extern int sparWR_cnt;
extern int sparRD_cnt;
//define types
typedef int32_t fixed_point_t;

//define prototypes; ordered as they appear in the source file
void ColumnToColumn(int rd, int rs, int copy);
void RowToColumn(int rd, int rs, int function);
void ColumnToRow(int rd, int rs, int function);
void RowToRow(int rd, int rs);
void print2DArray(int rows, int cols, int arr[][cols]);
fixed_point_t float_to_fixed(double input); //may need to change this as it is an inline function
void FxP_Conversion(int row, int col, float in[][col], int out[][col] );
void ELEMENTWISE_MULTIPLICATION(int matrixA_reg, int matrixB_reg, int Result_Reg);
void SHIFT_NORTH(int rs, int rd);
void SHIFT_SOUTH(int rs, int rd);
void SHIFT_EAST(int rs, int rd);
void SHIFT_WEST(int rs, int rd);

void MATRIX_ADDITION(int matrixA_reg, int matrixB_reg, int Result_Reg);
void MATRIX_SUBTRACTION(int matrixA_reg, int matrixB_reg, int Result_Reg);
void MATRIX_MULTIPLICATION(int matrixA_reg, int matrixB_reg, int Result_Reg, int matrixA_col, int block_dimension);
unsigned int Log2n(unsigned int n);
void MARTIX_MULTIPLICATION_Optimized(int matrixA_reg, int matrixB_reg, int Result_Reg, int matrixA_col, int block_dimension);
void WRITE_Matrix(int row, int col, int W[][col], int reg, int copy);
void WRITE_Matrix_Large(int row, int col, int block_row, int block_col, int row_blk_size, int col_blk_size, int W[][col], int reg, int copy, int block_dimension);
int toggleBit(int n, int k);
void WRITE_REG(int Tile_i, int Tile_j, int BRAM_i, int BRAM_j, int PE, int reg, unsigned int data);
unsigned int READ_REG(int Tile_i, int Tile_j, int BRAM_i, int BRAM_j, int PE, int reg);
void WRITE(int Tile_i, int Tile_j, int BRAM_i, int BRAM_j, int ADDRA, int ADDRB, int DIA, int DIB);
int READ(int Tile_i, int Tile_j, int BRAM_i, int BRAM_j, int ADDRA, int ADDRB);
int printReg(int Tile_i, int Tile_j, int BRAM_i, int BRAM_j, int PE, int reg);
void printRegFile(int Tile_i, int Tile_j, int BRAM_i, int BRAM_j, int number_of_regs);
int execute(int opcode, int rd, int rs1, int rs2);
int getbit(int n, int k);
void printArray(int* arr, int size);
void getTranspose(int* ram, int* reg);
int getTrans(int* ram);

#endif
