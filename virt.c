#include "virt.h"
#include "AllocationTable.h"
#include "spar.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "xil_printf.h"

#define numberofreg 16 //todo-->have this redefined to use a more global variable
#define SPAR_dimension (4*Array_dim*Tile_dim) //number of PE's found either horizontally or vertically
#define minimumNumOfVReg 4 //minimum number of virtual registers that need to fit in spar to perform operations
int usedRegisters = 0;

extern inline void SetUp3Registers(int rs1, int rs2, int rd, AllocationTable *table){//not to be called outside of this file
	if(table->vreg[rs1].status != 1)
	{
		//void safeAllocatePRegs(int vRegNum, int maxDim, int protectedVReg[], int numProtected, AllocationTable *table);
		int reserveReg[] = {-1,-1};
		int numReserved = 0;
		if(table->vreg[rs2].status == 1 && rs1 != rs2)
		{ //don't remove rs2 if it is already in SPAR
			reserveReg[numReserved] = rs2;
			numReserved++;
		}
		if(table->vreg[rd].status == 1 && rd != rs1)
		{
			reserveReg[numReserved] = rd;
			numReserved++;
		}
		safeAllocatePRegs(rs1, SPAR_dimension, reserveReg, numReserved, table);
	}

	if(table->vreg[rs2].status != 1){
		//assume that rs1 is in SPAR already
		int reserveReg[2] = {-1, -1};
		reserveReg[0] = rs1;
		int numReserved = 1;
		if(table->vreg[rd].status == 1 && rd != rs2)
		{
			reserveReg[1] = rd;
			numReserved++;
		}
		safeAllocatePRegs(rs2, SPAR_dimension, reserveReg, numReserved, table);
	}

	if(table->vreg[rd].status != 1){
		//assume that rs1 and rs2 are in SPAR
		int reserveReg[2] = {rs1, rs2};
		safeAllocateEmptyPRegs(rd, SPAR_dimension, reserveReg, 2, table);
	}
}


void Declare_M(Matrix *m, int rows, int cols){
	//allocate enough space in memory
	m->memory = (int*)malloc(rows*cols*sizeof(int));
	m->rows = rows;
	m->cols = cols;
	//leave placement alone. Placement is only to be used when storing a matrix to keep track of the individual segments.
}

void Deallocate_M(Matrix *m){
	free(m->memory);
}

void Store_M(Matrix* m, int rd, AllocationTable *table) { //does not actually store a matrix. Just allocates it in the allocation table
	int colFol = ceil((float)m->cols/(float)SPAR_dimension);
	int rowFol = ceil((float)m->rows/(float)SPAR_dimension);

	int copies = colFol * rowFol;
	if (copies > numberofreg/minimumNumOfVReg) //todo-->need to actually implement this if statement properly
	{
		printf("not enough registers to load array");
		return; //exits the function if there are not enough registers
	}

	allocateVRegM(m, rd, 0, table);
	//todo: remove previous VReg data from PRegs
	printf("here5\n");
}

void Store_M_Transpose(Matrix *m, int rd, AllocationTable *table) {
	//do not needs to change "fold" calculations since the dimensions are square
	int colFol = ceil((float)m->cols/(float)SPAR_dimension);
	int rowFol = ceil((float)m->rows/(float)SPAR_dimension);

	int copies = colFol * rowFol;
	printf("copies: %d, dimension: %d\n", copies, SPAR_dimension);
	if (copies*minimumNumOfVReg > numberofreg)
	{
		printf("not enough registers to load array\n");
		return; //exits the function if there are not enough registers
	}

	allocateVRegM(m, rd, 1, table);
	//todo: remove previous VReg data from PRegs
}

void Load_M(Matrix *m, int rs) { //load matrix from spar to memory
	//read data to SPAR
	int colFol = ceil((float)m->cols/(float)SPAR_dimension); //may come up with another way to get this value from the Matrix struct
	int s = 0;
	int t = 0;
	for (t = 0; t < m->rows; t += 1)
	{
		int offset = t*(m->cols);
		for (s = 0; s < m->cols; s += 1)
		{
			//go back and check the *memory accesss before running
			m->memory[offset+s] = READ_REG((s/(4*Tile_dim))%Array_dim, (t/(4*Tile_dim))%Array_dim, (s/4)%Tile_dim, (t/4)%Tile_dim, ((s*4 +t)%4 + s*4)%16, rs + (minimumNumOfVReg * ((s/SPAR_dimension)*colFol + (t/SPAR_dimension))));
//				xil_printf("Read Value: %d\n", READ_REG((s/(4*Tile_dim))%Array_dim, (t/(4*Tile_dim))%Array_dim, (s/4)%Tile_dim, (t/4)%Tile_dim, ((s*4 +t)%4 + s*4)%16, rs + (numberOfVirtualRegisters * ((s/dimension)*colFol + (t/dimension)))));
		}
	}
}

void E_Add_MM(int rs1, int rs2, int rd, AllocationTable *table){
	//set result up
	table->vreg[rd].cols = table->vreg[rs1].cols;
	table->vreg[rd].rows = table->vreg[rs1].rows;
	table->vreg[rd].orientation = table->vreg[rs1].orientation;
	table->vreg[rd].type = table->vreg[rs1].type;
	//todo: remove placements in VReg[rd]
	SetUp3Registers(rs1, rs2, rd, table);
	for(int i = 0; i < 6; i++){//todo: replace 6 with a variable or macro
//		printf("i: %d, rd: %d, rs1: %d, rs2: %d\n", i, table->vreg[rd].placement[i], table->vreg[rs1].placement[i], table->vreg[rs2].placement[i]);
		if(table->vreg[rd].placement[i]>=0 && table->vreg[rs1].placement[i]>=0 && table->vreg[rs2].placement[i]>=0) execute(0, table->vreg[rd].placement[i], table->vreg[rs1].placement[i], table->vreg[rs2].placement[i]);
		else break; //break out of loop if one of the placements is -1
	}
}

void Reset_Registers(){
	int i, j, k, l, addra;
	for( i = 0; i < Array_dim; i++){
		for( j = 0; j < Array_dim; j++){
			for( k = 0; k < Tile_dim; k++){
				for( l = 0; l < Tile_dim; l++){
					for( addra = 0; addra < 32 * 32; addra++){
						WRITE(i, j, k, l, addra, 0, 0, 0);
					}
				}
			}
		}
	}
};
