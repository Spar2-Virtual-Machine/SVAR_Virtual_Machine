/*
 * AllocationTable.c
 *
 *  Created on: Nov 17, 2022
 *      Author: Njfre
 */
#include "AllocationTable.h"
#include "spar.h"
#include "sleep.h"
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
//		printf("here1\n");
		(table->vreg[i].status)=-1;
		xil_printf("vreg %d;\t%d\n", i, table->vreg[i].status);
		for(int j = 0; j<6; j++) //todo: change the 6 to a variable
		{
//			printf("here2\n");

			table->vreg[i].placement[j] = -1;
		}
//		for(int j = 0; j<VREG_Data_Size/2; j++)
//		{
//			table->vreg[i].data[j]=0;
//		}
//		printf("here3\n");
		table->vreg[i].cols=0;
		table->vreg[i].rows=0;
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

void allocateVRegM(Matrix *m, int vRegNum, int orientation, AllocationTable *table)
{
	xil_printf("VREG to allocate: %d;\ttable: %p\n", vRegNum, table);
	printf("here1\n");
	table->vreg[vRegNum].orientation = orientation;
	table->vreg[vRegNum].type = 0; //set type to specify matrix
	table->vreg[vRegNum].cols = m->cols; //set the dimensions of the VReg data
	table->vreg[vRegNum].rows = m->rows;
	printf("here2\n");

	//put matrix data into the VReg data. Does not need to take orientation into account
	for(int i=0; i<m->rows; i++)
	{
		for(int j=0; j<m->cols; j++)
		{
			table->vreg[vRegNum].data[i*m->cols + j] = m->memory[i*m->cols + j];
		}
	}
	printf("here3\n");

	table->vreg[vRegNum].status = 0;
	table->vreg[vRegNum].type = 0;
	printf("here4\n");

	return;
}

void allocateVRegV(Vector *v, int vRegNum, AllocationTable *table)
{

}

void safeAllocatePRegs(int vRegNum, int maxDim, int protectedVReg[], int numProtected, AllocationTable *table)
{
	printf("begin safe allocate\n");
	//todo-- add in a counter or bool to make sure that all segments of a matrix/vector are written into the physical registers before changing status
	//todo-- add in ability to change according to orientation

	//if a matrix
	 if(table->vreg[vRegNum].type==0){
		 int horizontalLay = ceil((float)table->vreg[vRegNum].cols/(float)maxDim); //maxdim = 4PE's * Tile_Dim * Array_Dim
		 int verticalLay = ceil((float)table->vreg[vRegNum].rows/(float)maxDim);
		 int numRegNeeded = horizontalLay*verticalLay; //todo: use ceil here

		 int pointerEnd = (table->nextRegToUpdate + Num_PREG)%Num_PREG; //indicates that it has looped through all of the physical registers
		 int i = 0;
		 //loop through until all registers are written or the pointer loops all the way through all possible physical registers
		 do
		 {
			int oldReg = table->preg[table->nextRegToUpdate];
			bool isProtected = 0;
			for(int j = 0; j < numProtected; j++)
			{
				if(oldReg == protectedVReg[j]) {isProtected = true; break;}
			}

			if(!isProtected) //reallocate the preg if the virtual register is not protected
			{
				if(oldReg!=-1) {removeVRegFromPRegs(oldReg, table);}
				//todo: remove all other PReg allocations for old VREG and move data to memory

				//assign the current virtual register to the physical register
				table->preg[table->nextRegToUpdate] = vRegNum;

				//update the placement in the virtual register
				table->vreg[vRegNum].placement[i] = table->nextRegToUpdate;

				//write the data segment to the register in SPAR
				int startRow = (i/horizontalLay)*maxDim;
				int startCol = (i%horizontalLay)*maxDim;
				int endRow = startRow+maxDim-1;
				if(endRow > table->vreg[vRegNum].rows) {endRow = table->vreg[vRegNum].rows - 1;}
				int endCol = startCol+maxDim-1;
				if(endCol > table->vreg[vRegNum].cols) {endCol = table->vreg[vRegNum].cols - 1;}
//				printf("StartRow: %d, StartCol: %d, EndRow: %d, EndCol: %d\n", startRow, startCol, endRow, endCol);
				loadVRegDataToPReg(vRegNum, table->nextRegToUpdate, startRow, startCol, endRow, endCol, table); //todo: just a placeholder for now

				//increment the number registers that have been assigned
				i++;
			}
			//update pointer
			table->nextRegToUpdate++;
			table->nextRegToUpdate %= Num_PREG;
		 } while ((i < numRegNeeded) && (table->nextRegToUpdate != pointerEnd));
		 //if the entire matrix is in SPAR, then update the status of the matrix
		 table->vreg[vRegNum].status = 1;
	 }
}

void safeAllocateEmptyPRegs(int vRegNum, int maxDim, int protectedVReg[], int numProtected, AllocationTable *table)
{
	printf("begin safe allocate\n");
	//todo-- add in a counter or bool to make sure that all segments of a matrix/vector are written into the physical registers before changing status
	//todo-- add in ability to change according to orientation

	//if a matrix
	 if(table->vreg[vRegNum].type==0){
		 int horizontalLay = ceil((float)table->vreg[vRegNum].cols/(float)maxDim); //maxdim = 4PE's * Tile_Dim * Array_Dim
		 int verticalLay = ceil((float)table->vreg[vRegNum].rows/(float)maxDim);
		 int numRegNeeded = horizontalLay*verticalLay; //todo: use ceil here

		 int pointerEnd = (table->nextRegToUpdate + Num_PREG)%Num_PREG; //indicates that it has looped through all of the physical registers
		 int i = 0;
		 //loop through until all registers are written or the pointer loops all the way through all possible physical registers
		 do
		 {
			int oldReg = table->preg[table->nextRegToUpdate];
			bool isProtected = 0;
			for(int j = 0; j < numProtected; j++)
			{
				if(oldReg == protectedVReg[j]) {isProtected = true; break;}
			}

			if(!isProtected) //reallocate the preg if the virtual register is not protected
			{
				if(oldReg!=-1) {removeVRegFromPRegs(oldReg, table);}
				//todo: remove all other PReg allocations for old VREG and move data to memory

				//assign the current virtual register to the physical register
				table->preg[table->nextRegToUpdate] = vRegNum;

				//update the placement in the virtual register
				table->vreg[vRegNum].placement[i] = table->nextRegToUpdate;

				//write the data segment to the register in SPAR
				int startRow = (i/horizontalLay)*maxDim;
				int startCol = (i%horizontalLay)*maxDim;
				int endRow = startRow+maxDim-1;
				if(endRow > table->vreg[vRegNum].rows) {endRow = table->vreg[vRegNum].rows - 1;}
				int endCol = startCol+maxDim-1;
				if(endCol > table->vreg[vRegNum].cols) {endCol = table->vreg[vRegNum].cols - 1;}
				//increment the number registers that have been assigned
				i++;
			}
			//update pointer
			table->nextRegToUpdate++;
			table->nextRegToUpdate %= Num_PREG;
		 } while ((i < numRegNeeded) && (table->nextRegToUpdate != pointerEnd));
		 //if the entire matrix is in SPAR, then update the status of the matrix
		 table->vreg[vRegNum].status = 1;
	 }
}

void loadVRegDataToPReg(int vRegNum, int pRegNum, int startRow, int startCol, int endRow, int endCol, AllocationTable *table)
{
	//todo --> add compatibility with vectors and scalars in the form of another if statement or function
	//todo --> add ability to orient the way it should be
//	Matrix *m = (table->vreg[vRegNum].m);
	for(int i = startRow; i <= endRow; i++)
	{
		int s = i-startRow;
		for(int j = startCol; j <= endCol; j++)
		{
			int t = j-startCol;
			//thinking that s and t should be replaced by i-startRow and j-endRow
			WRITE_REG((s/(4*Tile_dim))%Array_dim, (t/(4*Tile_dim))%Array_dim, (s/4)%Tile_dim, (t/4)%Tile_dim, ((s*4 +t)%4 + s*4)%16, pRegNum, table->vreg[vRegNum].data[i + j*(table->vreg[vRegNum].cols)]);
//			xil_printf("s: %d, t: %d;  ", s, t);
//			xil_printf("Array: %d, %d", (s/(4*Tile_dim))%Array_dim,(t/(4*Tile_dim))%Array_dim);
//			xil_printf(" Tile: %d, %d", (s/4)%Tile_dim, (t/4)%Tile_dim);
//			xil_printf(" PE: %d \n", ((s*4 +t)%4 + s*4)%16);
		}
	}
}

void storePRegToMem(int vRegNum, int pRegNum, int startRow, int startCol, int endRow, int endCol, AllocationTable *table) 	//move data from SPAR to memory
{
	for(int placement=0; placement<6 && table->vreg->placement[placement] != -1; placement++){ //todo replace 6 with a variable
		//todo change for each orientation
		//todo account for the type in the vReg

		//default orientation
		int* data;
		int colOffset = 0;
		if(table->vreg[vRegNum].type==0){
			Matrix *m = table->vreg[vRegNum].m;
			data = m->memory;
			colOffset = m->cols;
		}

		for(int i = startRow; i < endRow; i++)
		{
			int s = i-startRow;
			for(int j = startCol; j <= endCol; j++)
			{
				int t = j-startCol;
				data[i + j*(colOffset)] = READ_REG((s/(4*Tile_dim))%Array_dim, (t/(4*Tile_dim))%Array_dim, (s/4)%Tile_dim, (t/4)%Tile_dim, ((s*4 +t)%4 + s*4)%16, pRegNum);
			}
		}
	}
}


void removeVRegFromPRegs(int vRegNum, AllocationTable *table)
{
	//todo --> store the data in the physical register to memory. Maybe overwrite everything with 0.
	//todo: use placement in VREG instead of looping through all of the PE's
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

void printTablePReg(AllocationTable *table)
{
	printf("___________PReg__________\n");
	for(int i=0; i<Num_PREG; i++)
	{
		printf("PReg: R%.2d, \t", i);
		printf("Assigned V: %.2d, \n", table->preg[i]);
	}
}

void printVRegData(int reg, AllocationTable *table)
{
	printf("VReg Data\n");
	for(int i=0; i<table->vreg[reg].rows; i++)
	{
		for(int j=0; j<table->vreg[reg].cols; j++)
		{
			printf("%d, ", table->vreg[reg].data[j + i*(table->vreg[reg].cols)]);
		}
		printf("\n");
		usleep_A53(10);
	}
}
