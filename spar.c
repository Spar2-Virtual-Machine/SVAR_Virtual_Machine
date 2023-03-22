#include <stdio.h>
#include "spar.h"
#include "platform.h"
#include "xil_printf.h"
#include <stdlib.h>
#include <stdint.h>
#include "platform.h"
#include "Bit_Serial.h"
#include "xparameters.h"
#include <inttypes.h>
#include "xbasic_types.h"
#include "xil_io.h"
#include "math.h"
#include "sleep.h"

int LEN = MAX_LEN << 24; //todo: see if this needs to be changed to account for spar size change
int ShL_cnt = 0;
int ShR_cnt = 0;
int ShN_cnt = 0;
int ShS_cnt = 0;
int mult_cnt = 0;
int add_cnt = 0;
int sub_cnt = 0;

int sparAdd_cnt=0;
int sparSub_cnt=0;
int sparMul_cnt=0;
int sparShN_cnt=0;
int sparShS_cnt=0;
int sparShE_cnt=0;
int sparShW_cnt=0;
int sparWR_cnt=0;
int sparRD_cnt = 0;

void ColumnToColumn(int rd, int rs, int copy){

	int i;
	usleep_A53(100);
	int instruction = (5<<26) + (rd<<21) + (rs<< 16) + (0 << 11);
	BIT_SERIAL_mWriteReg(XPAR_BIT_SERIAL_0_S00_AXI_BASEADDR, BIT_SERIAL_S00_AXI_SLV_REG0_OFFSET, instruction);//instruction
	BIT_SERIAL_mWriteReg(XPAR_BIT_SERIAL_0_S00_AXI_BASEADDR, BIT_SERIAL_S00_AXI_SLV_REG1_OFFSET, 0x00000003+LEN);//start = 1, reset = 1
	BIT_SERIAL_mWriteReg(XPAR_BIT_SERIAL_0_S00_AXI_BASEADDR, BIT_SERIAL_S00_AXI_SLV_REG1_OFFSET, 0x00000001+LEN);//start = 0, reset = 1
	usleep_A53(100);
	instruction = (5<<26) + (rd<<21) + (rs<< 16) + (1 << 11);
	BIT_SERIAL_mWriteReg(XPAR_BIT_SERIAL_0_S00_AXI_BASEADDR, BIT_SERIAL_S00_AXI_SLV_REG0_OFFSET, instruction);//instruction
	BIT_SERIAL_mWriteReg(XPAR_BIT_SERIAL_0_S00_AXI_BASEADDR, BIT_SERIAL_S00_AXI_SLV_REG1_OFFSET, 0x00000003+LEN);//start = 1, reset = 1
	BIT_SERIAL_mWriteReg(XPAR_BIT_SERIAL_0_S00_AXI_BASEADDR, BIT_SERIAL_S00_AXI_SLV_REG1_OFFSET, 0x00000001+LEN);//start = 0, reset = 1
	usleep_A53(100);

	for(i = 0; i < copy + 1; i++)
	{
		instruction = (11<<26) + (rd<<21) + (rs<< 16) + (0 << 11);
		BIT_SERIAL_mWriteReg(XPAR_BIT_SERIAL_0_S00_AXI_BASEADDR, BIT_SERIAL_S00_AXI_SLV_REG0_OFFSET, instruction);//instruction
		BIT_SERIAL_mWriteReg(XPAR_BIT_SERIAL_0_S00_AXI_BASEADDR, BIT_SERIAL_S00_AXI_SLV_REG1_OFFSET, 0x00000003+LEN);//start = 1, reset = 1
		BIT_SERIAL_mWriteReg(XPAR_BIT_SERIAL_0_S00_AXI_BASEADDR, BIT_SERIAL_S00_AXI_SLV_REG1_OFFSET, 0x00000001+LEN);//start = 0, reset = 1
		usleep_A53(100);
	}

}

void RowToColumn(int rd, int rs, int function){

	//function: 0 == noOp, 1 == sigmoid, 2 == tanh
	BIT_SERIAL_mWriteReg(XPAR_BIT_SERIAL_0_S00_AXI_BASEADDR, BIT_SERIAL_S00_AXI_SLV_REG6_OFFSET, function);
	execute(8, rs, rs, 0);
	ShN_cnt++;
	int instruction = (6<<26) + (rd<<21) + (rs << 16) + (0 << 11);
	BIT_SERIAL_mWriteReg(XPAR_BIT_SERIAL_0_S00_AXI_BASEADDR, BIT_SERIAL_S00_AXI_SLV_REG0_OFFSET, instruction);//instruction
	BIT_SERIAL_mWriteReg(XPAR_BIT_SERIAL_0_S00_AXI_BASEADDR, BIT_SERIAL_S00_AXI_SLV_REG1_OFFSET, 0x00000003+LEN);//start = 1, reset = 1
	BIT_SERIAL_mWriteReg(XPAR_BIT_SERIAL_0_S00_AXI_BASEADDR, BIT_SERIAL_S00_AXI_SLV_REG1_OFFSET, 0x00000001+LEN);//start = 0, reset = 1
	ShL_cnt++;
}

void ColumnToRow(int rd, int rs, int function){

	//function: 0 == noOp, 1 == sigmoid, 2 == tanh
	BIT_SERIAL_mWriteReg(XPAR_BIT_SERIAL_0_S00_AXI_BASEADDR, BIT_SERIAL_S00_AXI_SLV_REG6_OFFSET, function);
	execute(5, rs, rs, 0);
	ShR_cnt++; //increase the shift-right count. Not sure as to why this is used.
	int instruction = (7<<26) + (rd<<21) + (rd << 16) + (0 << 11);  //shift north? Documentation might be out of date
	BIT_SERIAL_mWriteReg(XPAR_BIT_SERIAL_0_S00_AXI_BASEADDR, BIT_SERIAL_S00_AXI_SLV_REG0_OFFSET, instruction);//instruction
	BIT_SERIAL_mWriteReg(XPAR_BIT_SERIAL_0_S00_AXI_BASEADDR, BIT_SERIAL_S00_AXI_SLV_REG1_OFFSET, 0x00000003+LEN);//start = 1, reset = 1
	BIT_SERIAL_mWriteReg(XPAR_BIT_SERIAL_0_S00_AXI_BASEADDR, BIT_SERIAL_S00_AXI_SLV_REG1_OFFSET, 0x00000001+LEN);//start = 0, reset = 1
	ShS_cnt++;
}

void RowToRow(int rd, int rs){

	BIT_SERIAL_mWriteReg(XPAR_BIT_SERIAL_0_S00_AXI_BASEADDR, BIT_SERIAL_S00_AXI_SLV_REG6_OFFSET, 0x6);
	execute(8, rs, rs, 0);
	ShN_cnt++;
	int instruction = (7<<26) + (rd<<21) + (rd << 16) + (0 << 11);

	BIT_SERIAL_mWriteReg(XPAR_BIT_SERIAL_0_S00_AXI_BASEADDR, BIT_SERIAL_S00_AXI_SLV_REG0_OFFSET, instruction);//instruction
	BIT_SERIAL_mWriteReg(XPAR_BIT_SERIAL_0_S00_AXI_BASEADDR, BIT_SERIAL_S00_AXI_SLV_REG1_OFFSET, 0x00000003+LEN);//start = 1, reset = 1
	BIT_SERIAL_mWriteReg(XPAR_BIT_SERIAL_0_S00_AXI_BASEADDR, BIT_SERIAL_S00_AXI_SLV_REG1_OFFSET, 0x00000001+LEN);//start = 0, reset = 1
	ShS_cnt++;
}

void print2DArray(int rows, int cols, int arr[][cols]){

	int i, j;
	for( i=0; i<rows; i++){
		for( j=0; j<cols; j++){
			printf("%08x\t", arr[i][j]);
		}
		printf("\n\r");
	}
}

inline fixed_point_t float_to_fixed(double input)
{
    return (fixed_point_t)(input * (1 << FIXED_POINT_FRACTIONAL_BITS));
}

void FxP_Conversion(int row, int col, float in[][col], int out[][col] ){

	int s, t;
	for( s=0; s<row; s++)
	{
		for( t=0; t<col; t++)
		{
//			out[s][t] = float_to_fixed(in[s][t]); //replaced because of an error
			out[s][t] = (fixed_point_t)(in[s][t] * (1 << FIXED_POINT_FRACTIONAL_BITS));
		}
	}
}


void SHIFT_NORTH(int rs, int rd){
	execute(8, rd, rs, 0);
}

void SHIFT_SOUTH(int rs, int rd){
	execute(7, rd, rs, 0);
}

void SHIFT_EAST(int rs, int rd){
	execute(5, rd, rs, 0);
}

void SHIFT_WEST(int rs, int rd){
	execute(6, rd, rs, 0);
}

void ELEMENTWISE_MULTIPLICATION(int matrixA_reg, int matrixB_reg, int Result_Reg){
	execute(2,Result_Reg,matrixA_reg,matrixB_reg);
//	mult_cnt++;
}

void MATRIX_ADDITION(int matrixA_reg, int matrixB_reg, int Result_Reg){

	execute(0,Result_Reg,matrixA_reg,matrixB_reg);
//	add_cnt++;
}

void MATRIX_SUBTRACTION(int matrixA_reg, int matrixB_reg, int Result_Reg){

	execute(1,Result_Reg,matrixA_reg,matrixB_reg);
//	sub_cnt++;
}

void MATRIX_MULTIPLICATION(int matrixA_reg, int matrixB_reg, int Result_Reg, int matrixA_col, int block_dimension){

	int i;
	execute(2,21,matrixB_reg,matrixA_reg);	//Mult
//	mult_cnt++;
	execute(5,27,21,0);						//ShiftEast
//	ShR_cnt++;
	execute(0,22,27,21);					//Add
//	add_cnt++;
	//shift and add
	for( i = 0; i < matrixA_col-2; i++)
	{
		execute(5,23,22,0);					//ShiftEast
//		ShR_cnt++;
		execute(0,22,23,21);				//Add
//		add_cnt++;
	}
	//shift till edge
	for( i = 0; i < (block_dimension*4) - matrixA_col; i++)
	{
		execute(5,22,22,0);					//ShiftEast
//		ShR_cnt++;
	}
	execute(0,Result_Reg,22,0);
//	add_cnt++;
	return;

}


unsigned int Log2n(unsigned int n)
{
    return (n > 1) ? 1 + Log2n(n / 2) : 0;
}

void MARTIX_MULTIPLICATION_Optimized(int matrixA_reg, int matrixB_reg, int Result_Reg, int matrixA_col, int block_dimension){

	//Optimized with BinTree accumulation
	unsigned int log_colA = Log2n(matrixA_col);
	int col_cnt = 0;
	execute(2,21,matrixB_reg,matrixA_reg);	//Mult
//	mult_cnt++;
	execute(0,23,21,0);						//Add
//	add_cnt++;
	int i, j;
	for( i=0; i<log_colA; i++){
		execute(5,22,21,0);						//ShiftEast
		col_cnt++;
//		ShR_cnt++;
		for( j=0; j<pow((double)2,(double)i)-(double)1; j++){
			execute(5,22,22,0);						//ShiftEast
			col_cnt++;
//			ShR_cnt++;
		}
		execute(0,21,22,21);					//Add
//		add_cnt++;
	}
	if(col_cnt != matrixA_col -1)
	{
		//shift and add
		for( i = col_cnt; i < matrixA_col; i++)
		{
			execute(5,22,21,0);					//ShiftEast
//			ShR_cnt++;
			execute(0,21,23,22);				//Add
//			add_cnt++;
		}
	}

	//shift till edge
	for( i = 0; i < (block_dimension*4) - matrixA_col; i++)
	{
		execute(5,21,21,0);					//ShiftEast
//		ShR_cnt++;
	}

	execute(0,Result_Reg,21,0);
//	add_cnt++;
	return;

}

void WRITE_Matrix(int row, int col, int W[][col], int reg, int copy){

	int s, t, i;
	if (copy == 1)		//write a 2D array, starts from the left-most PEs
	{
		for( s=0; s<row; s++)
		{
			for( t=0; t<col; t++)
			{
				//WRITE_REG(int Tile_i, int Tile_j, int BRAM_i, int BRAM_j, int PE, int reg, unsigned int data)
				WRITE_REG(s/(4 * Tile_dim), t/(4 * Tile_dim), (s/4)%Tile_dim, (t/4)%Tile_dim, ((s*4+t)%4+s*4)%16, reg, W[s][t]);
			}
		}
	}
	else if(copy == 0)	//write a 1D array into last column PEs
	{
		for( s=0; s<row; s++)
		{
			for( t=0; t<col; t++)
			{
				WRITE_REG(s/(4 * Tile_dim), Array_dim-1, (s/4)%Tile_dim, Tile_dim-1, (s*4+3)%16, reg, W[s][t]);
			}
		}
	}
	else if(copy == -1)	//write a 1D array into first column PEs
	{
		for( s=0; s<row; s++)
		{
			for( t=0; t<col; t++)
			{
				WRITE_REG(0, s/Array_dim, 0, (s/4)%Tile_dim, s%4, reg, W[s][t]);
			}
		}
	}
	else				//write a 1D array into first row, copies to the below PEs too
	{
		for( s=0; s<row; s++)
		{
			for( t=0; t<col; t++)
			{
				WRITE_REG(s/(4 * Tile_dim), Array_dim-1, (s/4)%Tile_dim, Tile_dim-1, (s*4+3)%16, reg, W[s][t]);
			}
		}
		ColumnToRow(reg, reg, 0);

		int instruction = (7<<26) + (reg<<21) + (reg << 16) + (0 << 11);
		BIT_SERIAL_mWriteReg(XPAR_BIT_SERIAL_0_S00_AXI_BASEADDR, BIT_SERIAL_S00_AXI_SLV_REG0_OFFSET, instruction);//instruction
		for( i = 0; i < copy-1; i++)
		{
			BIT_SERIAL_mWriteReg(XPAR_BIT_SERIAL_0_S00_AXI_BASEADDR, BIT_SERIAL_S00_AXI_SLV_REG1_OFFSET, 0x00000003+LEN);//start = 1, reset = 1
			BIT_SERIAL_mWriteReg(XPAR_BIT_SERIAL_0_S00_AXI_BASEADDR, BIT_SERIAL_S00_AXI_SLV_REG1_OFFSET, 0x00000001+LEN);//start = 0, reset = 1
		}

	}

}

//This actually just writes a section of a larger array into the registers.
///////////////////////// #of rows and cols, block row coordinates,     size of blocks                       the array     register, copy      ,does nothing in this context. Used to tell when end of matrix is reached in vector-matrix multiplication.
void WRITE_Matrix_Large(int row, int col, int block_row, int block_col, int row_blk_size, int col_blk_size, int W[][col], int reg, int copy, int block_dimension){


	int ss, tt, s, t, i;
	if (copy == 1)		//write a 2D array, starts from the left-most PEs
	{
		for( s = row_blk_size * block_row; s < ((block_row + 1) * row_blk_size); s++)
		{
			for( t = col_blk_size * block_col; t < ((block_col + 1) * col_blk_size); t++)
			{
				ss = s - (row_blk_size * block_row);
				tt = t - (col_blk_size * block_col);
				WRITE_REG(ss/(4 * Tile_dim), tt/(4 * Tile_dim), (ss/4)%Tile_dim, (tt/4)%Tile_dim, ((ss*4+tt)%4+ss*4)%16, reg, W[s][t]);

			}
		}
	}
	else if(copy == 0)	//write a 1D array into last column PEs
	{
		for( s = row_blk_size * block_row; s < ((block_row + 1) * row_blk_size); s++)
		{
			for( t = col_blk_size * block_col; t < col; t++)
			{
				ss = s - (row_blk_size * block_row);
				tt = t - (col_blk_size * block_col);
				WRITE_REG(ss/(4 * Tile_dim), Array_dim-1, (ss/4)%Tile_dim, Tile_dim-1, (ss*4+3)%16, reg, W[s][t]);

			}
		}
	}
	else if(copy == -1)	//write a 1D array into first column PEs
	{
		for( s = row_blk_size * block_row; s < ((block_row + 1) * row_blk_size); s++)
		{
			for( t = col_blk_size * block_col; t < col; t++)
			{
				ss = s - (row_blk_size * block_row);
				tt = t - (col_blk_size * block_col);
				WRITE_REG(0, ss/Array_dim, 0, (ss/4)%Tile_dim, ss%4, reg, W[s][t]);
			}
		}
	}
	else				//write a 1D array into first row, copies to the below PEs too
	{
		for( s = row_blk_size * block_row; s < ((block_row + 1) * row_blk_size); s++)
		{
			for( t = col_blk_size * block_col; t < col; t++)
			{
				ss = s - (row_blk_size * block_row);
				tt = t - (col_blk_size * block_col);
				WRITE_REG(ss/(4 * Tile_dim), Array_dim-1, (ss/4)%Tile_dim, Tile_dim-1, (ss*4+3)%16, reg, W[s][t]);
			}
		}
		ColumnToRow(reg, reg, 0);

		int instruction = (7<<26) + (reg<<21) + (reg << 16) + (0 << 11);
		BIT_SERIAL_mWriteReg(XPAR_BIT_SERIAL_0_S00_AXI_BASEADDR, BIT_SERIAL_S00_AXI_SLV_REG0_OFFSET, instruction);//instruction
		for( i = 0; i < copy-1; i++)
		{
			BIT_SERIAL_mWriteReg(XPAR_BIT_SERIAL_0_S00_AXI_BASEADDR, BIT_SERIAL_S00_AXI_SLV_REG1_OFFSET, 0x00000003+LEN);//start = 1, reset = 1
			BIT_SERIAL_mWriteReg(XPAR_BIT_SERIAL_0_S00_AXI_BASEADDR, BIT_SERIAL_S00_AXI_SLV_REG1_OFFSET, 0x00000001+LEN);//start = 0, reset = 1
		}

	}

}

int toggleBit(int n, int k){

    return (n ^ (1 << (k - 1)));
}

void WRITE_REG(int Tile_i, int Tile_j, int BRAM_i, int BRAM_j, int PE, int reg, unsigned int data){

	sparWR_cnt++;
	int base = reg*32;
	int i;
	for( i=base-2; i>=base-33; i-=2){

		unsigned int dia = ((data<<(31-i))>>31)<<PE;
		unsigned int dib = ((data<<(31-i-1))>>31)<<PE;
		unsigned int tmp, oldDia, oldDib, newDia, newDib;
		tmp = READ(Tile_i, Tile_j, BRAM_i, BRAM_j, 32+i, 32+i+1);
		oldDia = tmp >> 16;
		oldDib = (tmp << 16) >> 16;
		char oldBitA = getbit(oldDia, PE);
		char oldBitB = getbit(oldDib, PE);
		char BitA = getbit(dia, PE);
		char BitB = getbit(dib, PE);
		newDia = dia|oldDia;
		if(BitA==0 && oldBitA==1) newDia = toggleBit(newDia, PE+1);

		newDib = dib|oldDib;
		if(BitB==0 && oldBitB==1) newDib = toggleBit(newDib, PE+1);

		WRITE(Tile_i, Tile_j, BRAM_i, BRAM_j, 32+i, 32+i+1, newDia, newDib);
	}
}

unsigned int READ_REG(int Tile_i, int Tile_j, int BRAM_i, int BRAM_j, int PE, int reg){
//	static int sparRD_cnt = 0;

	sparRD_cnt++;
	int base = reg*32;
	unsigned int out;
	for(int i=base-2; i>=base-33; i-=2){

		unsigned int tmp, oldDia, oldDib;
		tmp = READ(Tile_i, Tile_j, BRAM_i, BRAM_j, 32+i, 32+i+1);
		oldDia = tmp >> 16;
		oldDib = (tmp << 16) >> 16;
		int oldBitA = getbit(oldDia, PE);
		int oldBitB = getbit(oldDib, PE);
		out = out << 1;
		out = out + oldBitB;
		out = out << 1;
		out = out + oldBitA;
	}
	return out;
}

void WRITE(int Tile_i, int Tile_j, int BRAM_i, int BRAM_j, int ADDRA, int ADDRB, int DIA, int DIB){

	int reg7_input = (Tile_i << 16) + Tile_j;
	int reg2_input = (BRAM_i << 16) + BRAM_j;
	int reg3_input = (ADDRA << 16) + ADDRB;
	int reg4_input = (DIA << 16) + DIB;

	//Tile_i = 0, Tile_j = 0
	BIT_SERIAL_mWriteReg(XPAR_BIT_SERIAL_0_S00_AXI_BASEADDR, BIT_SERIAL_S00_AXI_SLV_REG7_OFFSET, reg7_input);

	//BRAM_i = 0, BRAM_j = 0
	BIT_SERIAL_mWriteReg(XPAR_BIT_SERIAL_0_S00_AXI_BASEADDR, BIT_SERIAL_S00_AXI_SLV_REG2_OFFSET, reg2_input);

	//ADDRA = 32 , ADDRB = 64
	BIT_SERIAL_mWriteReg(XPAR_BIT_SERIAL_0_S00_AXI_BASEADDR, BIT_SERIAL_S00_AXI_SLV_REG3_OFFSET, reg3_input);

	//DIA = 0xFFFF, DIB = 0x1111
	BIT_SERIAL_mWriteReg(XPAR_BIT_SERIAL_0_S00_AXI_BASEADDR, BIT_SERIAL_S00_AXI_SLV_REG4_OFFSET, reg4_input);

	//external = 1, WEA = 1, WEB = 1
	BIT_SERIAL_mWriteReg(XPAR_BIT_SERIAL_0_S00_AXI_BASEADDR, BIT_SERIAL_S00_AXI_SLV_REG1_OFFSET, 0x0000001D+LEN);

	//external = 1, WEA = 0, WEB = 0
	BIT_SERIAL_mWriteReg(XPAR_BIT_SERIAL_0_S00_AXI_BASEADDR, BIT_SERIAL_S00_AXI_SLV_REG1_OFFSET, 0x00000001+LEN);
	BIT_SERIAL_mWriteReg(XPAR_BIT_SERIAL_0_S00_AXI_BASEADDR, BIT_SERIAL_S00_AXI_SLV_REG7_OFFSET, 0x00000000);
	BIT_SERIAL_mWriteReg(XPAR_BIT_SERIAL_0_S00_AXI_BASEADDR, BIT_SERIAL_S00_AXI_SLV_REG2_OFFSET, 0x00000000);
	BIT_SERIAL_mWriteReg(XPAR_BIT_SERIAL_0_S00_AXI_BASEADDR, BIT_SERIAL_S00_AXI_SLV_REG3_OFFSET, 0x00000000);
	BIT_SERIAL_mWriteReg(XPAR_BIT_SERIAL_0_S00_AXI_BASEADDR, BIT_SERIAL_S00_AXI_SLV_REG4_OFFSET, 0x00000000);

}

int READ(int Tile_i, int Tile_j, int BRAM_i, int BRAM_j, int ADDRA, int ADDRB){

	int reg7_input = (Tile_i << 16) + Tile_j;
	int reg2_input = (BRAM_i << 16) + BRAM_j;
	int reg3_input = (ADDRA << 16) + ADDRB;

	int DO;

	//Tile_i = 0, Tile_j = 0
	BIT_SERIAL_mWriteReg(XPAR_BIT_SERIAL_0_S00_AXI_BASEADDR, BIT_SERIAL_S00_AXI_SLV_REG7_OFFSET, reg7_input);

	//BRAM_i = 0, BRAM_j = 0
	BIT_SERIAL_mWriteReg(XPAR_BIT_SERIAL_0_S00_AXI_BASEADDR, BIT_SERIAL_S00_AXI_SLV_REG2_OFFSET, reg2_input);

	//ADDRA = 32 , ADDRB = 64
	BIT_SERIAL_mWriteReg(XPAR_BIT_SERIAL_0_S00_AXI_BASEADDR, BIT_SERIAL_S00_AXI_SLV_REG3_OFFSET, reg3_input);

	//external = 1, WEA = 0, WEB = 0
	BIT_SERIAL_mWriteReg(XPAR_BIT_SERIAL_0_S00_AXI_BASEADDR, BIT_SERIAL_S00_AXI_SLV_REG1_OFFSET, 0x00000005+LEN);

	//read
	DO = BIT_SERIAL_mReadReg(XPAR_BIT_SERIAL_0_S00_AXI_BASEADDR, BIT_SERIAL_S00_AXI_SLV_REG5_OFFSET);

	//external = 1, WEA = 0, WEB = 0
	BIT_SERIAL_mWriteReg(XPAR_BIT_SERIAL_0_S00_AXI_BASEADDR, BIT_SERIAL_S00_AXI_SLV_REG1_OFFSET, 0x00000001+LEN);
	BIT_SERIAL_mWriteReg(XPAR_BIT_SERIAL_0_S00_AXI_BASEADDR, BIT_SERIAL_S00_AXI_SLV_REG7_OFFSET, 0x00000000);
	BIT_SERIAL_mWriteReg(XPAR_BIT_SERIAL_0_S00_AXI_BASEADDR, BIT_SERIAL_S00_AXI_SLV_REG2_OFFSET, 0x00000000);
	BIT_SERIAL_mWriteReg(XPAR_BIT_SERIAL_0_S00_AXI_BASEADDR, BIT_SERIAL_S00_AXI_SLV_REG3_OFFSET, 0x00000000);

	return DO;
}

int printReg(int Tile_i, int Tile_j, int BRAM_i, int BRAM_j, int PE, int reg){

	int REG[32];
	int reg_out;
	int DO;
	reg = reg << 5;
	int i;
	for( i=0; i<16; i++){
		DO = READ (Tile_i, Tile_j, BRAM_i, BRAM_j, reg+i, reg+i+16);
		REG[i+16] = getbit(DO, PE);
		REG[i] = getbit(DO, PE+16);
	}

	reg_out = getTrans(REG);

	return reg_out;
}

void printRegFile(int Tile_i, int Tile_j, int BRAM_i, int BRAM_j, int number_of_regs){

	int PE, i;
	for( PE=0; PE<16; PE++){
		for( i=number_of_regs-1; i>=0; i--)
		{
			printf("%08x \t", printReg(Tile_i, Tile_j, BRAM_i, BRAM_j, PE, i));
		}
		printf("\n\r");
	}

}

int execute(int opcode, int rd, int rs1, int rs2)
{
	switch(opcode){
		case 0:
			sparAdd_cnt++;
			break;
		case 1:
			sparSub_cnt++;
			break;
		case 2:
			sparMul_cnt++;
			break;
		case 5:
			sparShE_cnt++;
			break;
		case 6:
			sparShW_cnt++;
			break;
		case 7:
			sparShS_cnt++;
			break;
		case 8:
			sparShN_cnt++;
			break;
	};

	int instruction = (opcode<<26) + (rd<<21) + (rs1 << 16) + (rs2 << 11);
	BIT_SERIAL_mWriteReg(XPAR_BIT_SERIAL_0_S00_AXI_BASEADDR, BIT_SERIAL_S00_AXI_SLV_REG1_OFFSET, 0x00000000+LEN);//start = 0, reset = 0
	BIT_SERIAL_mWriteReg(XPAR_BIT_SERIAL_0_S00_AXI_BASEADDR, BIT_SERIAL_S00_AXI_SLV_REG0_OFFSET, instruction);//instruction
	usleep_A53(12);
	BIT_SERIAL_mWriteReg(XPAR_BIT_SERIAL_0_S00_AXI_BASEADDR, BIT_SERIAL_S00_AXI_SLV_REG1_OFFSET, 0x00000001+LEN);//start = 0, reset = 1
	usleep_A53(12);
	BIT_SERIAL_mWriteReg(XPAR_BIT_SERIAL_0_S00_AXI_BASEADDR, BIT_SERIAL_S00_AXI_SLV_REG1_OFFSET, 0x00000003+LEN);//start = 1, reset = 1
	usleep_A53(12);
	BIT_SERIAL_mWriteReg(XPAR_BIT_SERIAL_0_S00_AXI_BASEADDR, BIT_SERIAL_S00_AXI_SLV_REG1_OFFSET, 0x00000001+LEN);//start = 0, reset = 1
	usleep_A53(24);
	return 0;
}

int getbit(int n, int k){

	return (n & ( 1 << k )) >> k;
}

void printArray(int* arr, int size){

	int i;
	for(i=0; i<size; i++){
		printf("%x\n\r",arr[i]);
	}
}

void getTranspose(int* ram, int* reg){

	int i, j;
	for(j=0; j<16; j++){
		for(i=31; i>=0; i--){
			reg[j] <<= 1;
			if(getbit(ram[i],j)==1) reg[j]++;
		}
	}
}

int getTrans(int* ram){

	int reg = 0;
	int i;
	for(i=31; i>=0; i--){
		reg <<= 1;
		getbit(ram[i],0)==1? reg++ : reg;
	}
	return reg;

}

void WEST_COLUMN_MOVE(int rs, int rd){
	for(int a = 0; a < Array_dim; a++)
	{
		for (int b = 0; b < Tile_dim; b++)
		{
			for(int j = 3; j < 16; j+=4)
			{
				int x = READ_REG(a, 0, b, 0, j-3, rs);
				WRITE_REG(a, Array_dim-1, b, Tile_dim-1, j, rd, x);
			}
		}
	}
}
