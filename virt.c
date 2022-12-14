#include "virt.h"
#include "AllocationTable.h"
#include "spar.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "xil_printf.h"

#define numberofreg 16 //todo-->have this redefined to use a more global variable
#define dimension (4*Array_dim*Tile_dim) //number of PE's found either horizontally or vertically
#define minimumNumOfVReg 4 //minimum number of virtual registers that need to fit in spar to perform operations
int usedRegisters = 0;


void Declare_M(Matrix *m, int rows, int cols){
	//allocate enough space in memory
	m->memory = (int*)malloc(rows*cols*sizeof(int));
	m->rows = rows;
	m->cols = cols;
	m->orientation = -1; //do this to specify that it is initially in memory
	//leave placement alone. Placement is only to be used when storing a matrix to keep track of the individual segments.
}

void Deallocate_M(Matrix *m){
	free(m->memory);
}

void Store_M(Matrix* m, int rd, AllocationTable *table) { //does not actually store a matrix. Just allocates it in the allocation table
	int colFol = ceil((float)m->cols/(float)dimension);
	int rowFol = ceil((float)m->rows/(float)dimension);

	int copies = colFol * rowFol;
	printf("copies: %d, dimension: %d\n", copies, dimension);
	if (copies > numberofreg/minimumNumOfVReg) //todo-->need to actually implement this if statement properly
	{
		printf("not enough registers to load array");
		return; //exits the function if there are not enough registers
	}

	xil_printf("here2.1\n");
	m->placement = malloc(copies*sizeof(int)); //allocate enough memory for the array
	m->orientation = 0;
	xil_printf("here2.2\n");
	allocateVRegM(m, rd, table);
	xil_printf("new status: %d\n", table->vreg[2].status);
}

void Store_M_Transpose(Matrix *m, int rd, AllocationTable *table) {
	//do not needs to change "fold" calculations since the dimensions are square
	int colFol = ceil((float)m->cols/(float)dimension);
	int rowFol = ceil((float)m->rows/(float)dimension);

	int copies = colFol * rowFol;
	printf("copies: %d, dimension: %d\n", copies, dimension);
	if (copies*minimumNumOfVReg > numberofreg)
	{
		printf("not enough registers to load array\n");
		return; //exits the function if there are not enough registers
	}

	xil_printf("here2.1\n");
	m->placement = malloc(copies*sizeof(int)); //allocate enough memory for the array
	m->orientation = 1; //set orientation as 1
	xil_printf("here2.2\n");

	allocateVRegM(m, rd, table);
}

void Load_M(Matrix *m, int rs) { //load matrix from spar to memory
	//read data to array
	int colFol = ceil((float)m->cols/(float)dimension); //may come up with another way to get this value from the Matrix struct
	int s = 0;
	int t = 0;
	for (t = 0; t < m->rows; t += 1)
	{
		int offset = t*(m->cols);
		for (s = 0; s < m->cols; s += 1)
		{
			//go back and check the *memory accesss before running
			m->memory[offset+s] = READ_REG((s/(4*Tile_dim))%Array_dim, (t/(4*Tile_dim))%Array_dim, (s/4)%Tile_dim, (t/4)%Tile_dim, ((s*4 +t)%4 + s*4)%16, rs + (minimumNumOfVReg * ((s/dimension)*colFol + (t/dimension))));
//				xil_printf("Read Value: %d\n", READ_REG((s/(4*Tile_dim))%Array_dim, (t/(4*Tile_dim))%Array_dim, (s/4)%Tile_dim, (t/4)%Tile_dim, ((s*4 +t)%4 + s*4)%16, rs + (numberOfVirtualRegisters * ((s/dimension)*colFol + (t/dimension)))));
		}
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
