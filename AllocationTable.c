/*
 * AllocationTable.c
 *
 *  Created on: Nov 17, 2022
 *      Author: Njfre
 */
#include "AllocationTable.h"
#include "spar.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
void inline resetPRegs(AllocationTable *table)
{
	for(int i = 0; i<Num_PREG; i++)
	{
		table->preg[i] = -1;
	}
}

void inline resetVRegs(AllocationTable *table)
{
	for(int i = 0; i<Num_VREG; i++)
	{
		(table->vreg[i].status)=-1;
		xil_printf("vreg %d;\t%d\n", i, table->vreg[i].status);
	}
}

void inline resetTable(AllocationTable *table)
{
	xil_printf("%p\n", table);


	for(int i=0; i < Num_VREG; i++)
	{
		xil_printf("vreg %d;\t%p\n", i, table->vreg[i]);
	}
	resetPRegs(table);
	resetVRegs(table);
	for(int i = 0; i<Num_VREG; i++)
	{
		xil_printf("vreg %d;\tstatus: %d;\taddress %p\n", i, table->vreg[i].status, &(table->vreg[i]));
	}
	table->nextRegToUpdate = 0;
	xil_printf("%p\n", table);
}

void allocateVRegM(Matrix *m, int vRegNum, AllocationTable *table)
{
	printf("here2.3\n");
	xil_printf("VREG to allocate: %d;\ttable: %p\n", vRegNum, table);
	table->vreg[vRegNum].m = m;
	xil_printf("here2.4\n");
	table->vreg[vRegNum].status = 0; //say it is in memory
	xil_printf("new status: %d\n", table->vreg[vRegNum].status);
	xil_printf("here2.5\n");
	table->vreg[vRegNum].type = 0;
	//may have to initialize the placement
	xil_printf("here2.6\n");
}

void allocateVRegV(Vector *v, int vRegNum, AllocationTable *table)
{
	table->vreg[vRegNum].m = v;
	table->vreg[vRegNum].status = 0;
	table->vreg[vRegNum].type = 1;
}

void safeAllocatePRegs(int vRegNum, int cols, int rows, int orientation, int maxDim, int protectedVReg[], int numProtected, AllocationTable *table)
{
	//todo-- add in a counter or bool to make sure that all segments of a matrix/vector are written into the physical registers

	//if a matrix
	 if(table->vreg[vRegNum].type==0){
		 Matrix *m = (table->vreg[vRegNum].m);
		 int horizontalLay = cols/maxDim;
		 int verticalLay = rows/maxDim;
		 int numRegNeeded = horizontalLay*verticalLay; //todo: use ceil here

		 m->placement = (int*)malloc(numRegNeeded*sizeof(int)); //allocate an array to record the placement of each segment.

		 int pointerEnd = (table->nextRegToUpdate + Num_PREG -1)%Num_PREG; //indicates that it has looped through all of the physical registers
		 //loop through until all registers are written or the pointer loops all the way through all possible physical registers
		 for(int i = 0; i < numRegNeeded && table->nextRegToUpdate != pointerEnd; i++)
		 {
			int oldReg = table->preg[table->nextRegToUpdate];
			bool isProtected = 0;
			for(int j = 0; j < numProtected; j++)
			{
				if(oldReg == protectedVReg[j]) {isProtected = true; break;}
			}

			if(!isProtected) //replace data if the virtual register is not protected
			{
				if(oldReg!=-1) {removeVRegFromPRegs(oldReg, table);}
				//allocate the new virtual register to the physical register
				table->preg[table->nextRegToUpdate] = vRegNum;

				//write the data segment to the regsiter is SPAR

				//update the placement in the virtual register

				//update Round Robin pointer position
				table->nextRegToUpdate++;
				table->nextRegToUpdate %= Num_PREG;
			}
			else
			{
				//update pointer
				table->nextRegToUpdate++;
				table->nextRegToUpdate %= Num_PREG;
			}
		 }
		 //todo--> if the entire matrix is in SPAR, then update the status of the matrix
		 table->vreg[vRegNum].status = 1;
	 }
}

void loadToVRegDataToPReg(int vRegNum, int pRegNum, int startRow, int startCol, int endRow, int endCol, AllocationTable *table)
{
	//todo --> add compatibility with vectors and scalars in the form of another if statement or function
	//todo --> add ability to orient the way it should be
//	VReg *vreg = &table->vreg[vRegNum];
	Matrix *m = (table->vreg[vRegNum].m);
	for(int i = startRow; i <= endRow; i++)
	{
		int s = i-startRow;
		for(int j = startCol; i <= endCol; i++)
		{
			int t = i-endRow;
			//thinking that s and t should be replaced by i-startRow and j-endRow
			//WRITE_REG(int Tile_i, int Tile_j, int BRAM_i, int BRAM_j, int PE, int reg, unsigned int data);
			WRITE_REG((s/(4*Tile_dim))%Array_dim, (t/(4*Tile_dim))%Array_dim, (s/4)%Tile_dim, (t/4)%Tile_dim, ((s*4 +t)%4 + s*4)%16, pRegNum, m->memory[i + j*(m->cols)]);
		}
	}
}

void removeVRegFromPRegs(int vRegNum, AllocationTable *table)
{
	//todo --> store the data in the physical register to memory. Maybe overwrite everything with 0.
	for(int i = 0; i < Num_PREG; i++)
	{
		if(table->preg[i]==vRegNum)
		{
			table->preg[i]=-1;
		}
		table->vreg[vRegNum].status = 0;
	}
}

void printTableVReg(AllocationTable *table)
{
	for(int i=0; i<Num_VREG; i++)
	{
		printf("VReg: R%.2d,\t", i);
		printf("Status: %.2d,\t", table->vreg[i].status);
		printf("Address: %p,\n", table->vreg[i].m);
	}
}

