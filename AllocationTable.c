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
	table->vreg[vRegNum].orientation = orientation; //setting orientation here actually doesn't matter that much
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

void allocateVRegM_T(Matrix *m, int vRegNum, int orientation, AllocationTable *table)
{
	xil_printf("VREG to allocate: %d;\ttable: %p\n", vRegNum, table);
	printf("here1\n");
	table->vreg[vRegNum].orientation = orientation;
	table->vreg[vRegNum].type = 0; //set type to specify matrix
	table->vreg[vRegNum].cols = m->rows; //set the dimensions of the VReg data
	table->vreg[vRegNum].rows = m->cols;
	printf("here2\n");

	//put matrix data into the VReg data. Does not need to take orientation into account
	for(int i=0; i<m->cols; i++)
	{
		for(int j=0; j<m->rows; j++)
		{
			printf("%d, ",m->memory[j*m->cols + i]);
		}
		printf("\n");
	}
	printf("here3\n");

	table->vreg[vRegNum].status = 0;
	table->vreg[vRegNum].type = 0;
	printf("here4\n");

	return;
}

void allocateVRegV(Vector *v, int vRegNum, AllocationTable *table)
{
	table->vreg[vRegNum].orientation = 0;
	table->vreg[vRegNum].type = 1;
	table->vreg[vRegNum].cols = 1; //This will count the number of times the vector is copied
	table->vreg[vRegNum].rows = v->size; //This will count the size of the vector

	for(int i=0; i<v->size; i++)
	{
		table->vreg[vRegNum].data[i] = v->memory[i];
	}
	table->vreg[vRegNum].status = 0;
}

void safeAllocatePRegs(int vRegNum, int maxDim, int protectedVReg[], int numProtected, AllocationTable *table)
{
	printf("begin safe allocate for %d\n", vRegNum);
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
				if(table->nextRegToUpdate==8){
					printf("hitting 8 here\n");
				}
				if(oldReg>=0) {removeVRegFromPRegs(oldReg, table);} //remove VReg data from PRegs. Copy into memory. unassign all of the other PRegs
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
				loadVRegDataToPReg_M(vRegNum, table->nextRegToUpdate, startRow, startCol, endRow, endCol, table);

				//increment the number registers that have been assigned
				i++;
			}
			//update pointer
			table->nextRegToUpdate++;
			table->nextRegToUpdate %= Num_PREG;
			table->vreg[vRegNum].placement[i] = -1; //mark the end of placement
		} while ((i < numRegNeeded) && (table->nextRegToUpdate != pointerEnd));
		//if the entire matrix is in SPAR, then update the status of the matrix
		table->vreg[vRegNum].status = 1;
	}
	//vector in standard orientation 
	else if(table->vreg[vRegNum].type==1 && table->vreg[vRegNum].orientation==0){
		printf("safe allocating vector\n");
		int horizontalLay = ceil((float)table->vreg[vRegNum].cols/(float)maxDim); //assume the number of rows=number of copies is already 
		int verticalLay = ceil((float)table->vreg[vRegNum].rows/(float)maxDim);
		int numRegNeeded = horizontalLay*verticalLay;
		int pointerEnd = (table->nextRegToUpdate + Num_PREG)%Num_PREG;
		int i = 0;
		printf("hL: %d, vL: %d\n", horizontalLay, verticalLay);
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
				if(table->nextRegToUpdate==8){
					printf("hitting 8 here\n");
				}
				if(oldReg>=0) {removeVRegFromPRegs(oldReg, table);} //remove VReg data from PRegs. Copy into memory. unassign all of the other PRegs
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
				loadVRegDataToPReg_V(vRegNum, table->nextRegToUpdate, startRow, endRow, table);

				//increment the number registers that have been assigned
				i++;
			}
			//update pointer
			table->nextRegToUpdate++;
			table->nextRegToUpdate %= Num_PREG;
			table->vreg[vRegNum].placement[i] = -1; //mark the end of placement
		} while ((i < numRegNeeded) && (table->nextRegToUpdate != pointerEnd));
		//if the entire matrix is in SPAR, then update the status of the matrix
		table->vreg[vRegNum].status = 1;
	}
}

void safeAllocateEmptyPRegs(int vRegNum, int maxDim, int protectedVReg[], int numProtected, AllocationTable *table)
{
	printf("begin safe allocate for %d\n", vRegNum);
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
				if(table->nextRegToUpdate==8){
					printf("hitting 8 here\n");
				}

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
			table->vreg[vRegNum].placement[i] = -1; //mark the end of placement
		 } while ((i < numRegNeeded) && (table->nextRegToUpdate != pointerEnd));
		 //if the entire matrix is in SPAR, then update the status of the matrix
		 table->vreg[vRegNum].status = 1;
	 }
	 else if(table->vreg[vRegNum].type==1 && table->vreg[vRegNum].orientation==0){
	 		printf("safe allocating vector\n");
	 		int horizontalLay = ceil((float)table->vreg[vRegNum].cols/(float)maxDim); //assume the number of rows=number of copies is already
	 		int verticalLay = ceil((float)table->vreg[vRegNum].rows/(float)maxDim);
	 		int numRegNeeded = horizontalLay*verticalLay;
	 		int pointerEnd = (table->nextRegToUpdate + Num_PREG)%Num_PREG;
	 		int i = 0;
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
	 				if(table->nextRegToUpdate==8){
	 					printf("hitting 8 here\n");
	 				}
	 				if(oldReg!=-1) {removeVRegFromPRegs(oldReg, table);} //remove VReg data from PRegs. Copy into memory. unassign all of the other PRegs
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
//	 				loadVRegDataToPReg_V(vRegNum, table->nextRegToUpdate, startRow, endRow, table);

	 				//increment the number registers that have been assigned
	 				i++;
	 			}
	 			//update pointer
	 			table->nextRegToUpdate++;
	 			table->nextRegToUpdate %= Num_PREG;
	 			table->vreg[vRegNum].placement[i] = -1; //mark the end of placement
	 		} while ((i < numRegNeeded) && (table->nextRegToUpdate != pointerEnd));
	 		//if the entire matrix is in SPAR, then update the status of the matrix
	 		table->vreg[vRegNum].status = 1;
	 	}
}

void loadVRegDataToPReg_M(int vRegNum, int pRegNum, int startRow, int startCol, int endRow, int endCol, AllocationTable *table)
{
	//todo --> add compatibility with vectors and scalars in the form of another if statement or function
	//todo --> add ability to orient the way it should be
	//////Matrices
	printf("Load: startRow: %d, endRow: %d, startCol: %d, endCol: %d, pRegNum: %d\n", startRow, endRow, startCol, endCol, pRegNum);
	if(table->vreg[vRegNum].type==0 && table->vreg[vRegNum].orientation==0)
	{
		int s, t;
		for(int i = startRow; i <= endRow; i++)
		{
			s = i-startRow;
			for(int j = startCol; j <= endCol; j++)
			{
				t = j-startCol;
				WRITE_REG((s/(4*Tile_dim))%Array_dim, (t/(4*Tile_dim))%Array_dim, (s/4)%Tile_dim, (t/4)%Tile_dim, ((s*4 +t)%4 + s*4)%16, pRegNum, table->vreg[vRegNum].data[j + i*(table->vreg[vRegNum].cols)]);
			}
		}
	} else if(table->vreg[vRegNum].type==0 && table->vreg[vRegNum].orientation==1)
	{
		int s, t;
		for(int i = startRow; i <= endRow; i++)
		{
			s = i-startRow;
			for(int j = startCol; j <= endCol; j++)
			{
				t = j-startCol;
				WRITE_REG((s/(4*Tile_dim))%Array_dim, (t/(4*Tile_dim))%Array_dim, (s/4)%Tile_dim, (t/4)%Tile_dim, ((s*4 +t)%4 + s*4)%16, pRegNum, table->vreg[vRegNum].data[i + j*(table->vreg[vRegNum].cols)]);
			}
		}
	}
}

void loadVRegDataToPReg_V(int vRegNum, int pRegNum, int startRow, int endRow, AllocationTable *table){
	//just put data into the first row of PE. Then you can use ShiftEast to copy rows later
	printf("Load: startRow: %d, endRow: %dpRegNum: %d\n", startRow, endRow, pRegNum);
	if(table->vreg[vRegNum].orientation==0)
	{
		int s;
		for(int i = startRow; i<=endRow; i++)
		{
			s = i-startRow;
			WRITE_REG((s/(4*Tile_dim))%Array_dim, 0, (s/4)%Tile_dim, 0, (s*4)%16, pRegNum, table->vreg[vRegNum].data[i]);
			printf("%d, ", READ_REG((s/(4*Tile_dim))%Array_dim, 0, (s/4)%Tile_dim, 0, (s*4)%16, pRegNum));
		}
		printf("\n");
	}
}

void copyFromPRegsToVRegData(int vRegNum, AllocationTable *table) 	//move data from SPAR to VReg memory
{
	if(table->vreg[vRegNum].status == 0) {printf("No Need to Copy vreg %d\n", vRegNum);return;}
	int horizontalLay = ceil((float)table->vreg[vRegNum].cols/(float)24);
	int* data;
	int colOffset = 0;
	if(table->vreg[vRegNum].type==0 && table->vreg[vRegNum].orientation==0)
	{
		for(int placement=0; placement<6 && table->vreg[vRegNum].placement[placement] != -1; placement++){ //todo replace 6 with a variable
			//todo change for each orientation
			//todo account for the type in the vReg

			//default orientation for matrix
			if(table->vreg[vRegNum].type==0){
				data = table->vreg[vRegNum].data;
				colOffset = table->vreg[vRegNum].cols;
			}

			printf("colOffset/24: %d\n", (int)ceil((float)colOffset/24));
			int startRow = 24*(placement/horizontalLay); //todo: replace 24 with variables/macros
			int endRow = startRow + 24;
			if(endRow > table->vreg[vRegNum].rows) {endRow = table->vreg[vRegNum].rows;}
			int startCol = 24*(placement%horizontalLay);
			int endCol = startCol + 24;
			if(endCol > table->vreg[vRegNum].cols) {endCol = table->vreg[vRegNum].cols;}
			int pRegNum = table->vreg[vRegNum].placement[placement];
			printf("startRow: %d, endRow: %d, startCol: %d, endCol: %d, pRegNum: %d\n", startRow, endRow, startCol, endCol, pRegNum);
			int s, t;
			for(int i = startRow; i < endRow; i++)
			{
				s = i-startRow;
				for(int j = startCol; j < endCol; j++)
				{
					t = j-startCol;
					data[j + i*(colOffset)] = READ_REG((s/(4*Tile_dim))%Array_dim, (t/(4*Tile_dim))%Array_dim, (s/4)%Tile_dim, (t/4)%Tile_dim, ((s*4 +t)%4 + s*4)%16, pRegNum);
				}
			}
		}
	}

	if(table->vreg[vRegNum].type==1 && table->vreg[vRegNum].orientation==0)
	{
		//default orientation for vector
		printf("Copying vector vreg %d\n", vRegNum);
		for(int placement=0; placement<6 && table->vreg[vRegNum].placement[placement] != -1; placement+=horizontalLay){ //todo replace 6 with a variable
			if(table->vreg[vRegNum].type==0){
				colOffset = table->vreg[vRegNum].cols;
			}
			int pRegNum = table->vreg[vRegNum].placement[placement];
			int startRow = 24*(placement/horizontalLay);
			int endRow = startRow + 24;
			if(endRow > table->vreg[vRegNum].rows) {endRow = table->vreg[vRegNum].rows;}
			int s;
			for(int i = startRow; i < endRow; i++)
			{
				s = i-startRow;
				table->vreg[vRegNum].data[i] = READ_REG((s/(4*Tile_dim))%Array_dim, 0, (s/4)%Tile_dim, 0, (s*4)%16, pRegNum);
				printf("%d, ", table->vreg[vRegNum].data[i]);
			}
		}
	}
}

void removeVRegFromPRegs(int vRegNum, AllocationTable *table)
{
	printf("Removing VReg %d from PRegisters\n", vRegNum);
	//todo --> store the data in the physical register to memory. Maybe overwrite everything with 0.
	copyFromPRegsToVRegData(vRegNum, table);
	//todo: use placement in VREG instead of looping through all of the PE's
	for(int i=0; i < 6; i++) //todo: replace 6 with a variable or macro
	{
		if(table->vreg[vRegNum].placement[i]==-1){continue;}
		else
		{
			 table->preg[table->vreg[vRegNum].placement[i]] = -1;
		}
	}
	table->vreg[vRegNum].status = 0;
}

void convertMtoV(int vRegNum, AllocationTable *table)
{
	if(table->vreg[vRegNum].orientation==0)
	{
		//convert if in memory
		if(table->vreg[vRegNum].status == 0)
		{

		}
		//convert if in SPAR already. Do not need to copy anything
		if(table->vreg[vRegNum].status==1)
		{
			table->vreg[vRegNum].cols=table->vreg[vRegNum].rows; //cols act as the only direction
			table->vreg[vRegNum].rows=0;

		}
	}
}

void printTableVReg(AllocationTable *table)
{
	for(int i=0; i<Num_VREG; i++)
	{
		printf("VReg: R%.2d,\t", i);
		printf("Status: %.2d,\t", table->vreg[i].status);
		printf("type: ");
		if(table->vreg[i].type==0) {printf("Matrix");}
		else if(table->vreg[i].type==1) {printf("Vector");}
		printf("\t");
		printf("orientation: %d,\n", table->vreg[i].orientation);
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
	printf("VReg Data for %d\nRegisters: ", reg);
	for(int i=0; i<6; i++)
	{
		printf("%d,", table->vreg[reg].placement[i]);
	}
	printf("\n");
	if(table->vreg[reg].type==0)
	{
		for(int i=0; i<table->vreg[reg].rows; i++)
		{
			for(int j=0; j<table->vreg[reg].cols; j++)
			{
				printf("%d, ", table->vreg[reg].data[j + i*(table->vreg[reg].cols)]);
			}
			printf("\n");
			usleep_A53(5);
		}
	}
	else if(table->vreg[reg].type==1)
	{
		for(int i=0; i<table->vreg[reg].rows; i++)
		{
			printf("%d, ", table->vreg[reg].data[i]);
		}
		printf("\n");
	}
}

void printVReginPReg(int reg, AllocationTable *table)
{
	printf("VReg %d being printed\n", reg);
	for(int i=0; i<6; i++) //todo: replace 6 with macro or variable
	{
		printf("%d, ", table->vreg[reg].placement[i]);
	}
	printf("\n");

	//just print off the data raw from spar
	int horizontalLay = ceil((float)table->vreg[reg].cols/(float)24); //todo: replace 24 with macro
	for(int i=0; i<table->vreg[reg].rows; i++)
	{
		for(int j=0; j<table->vreg[reg].cols; j++)
		{
			int pcount = (i/24)*horizontalLay + (j/24); //columns/maxdim
//			printf("pcount: %d\n", pcount);
			int preg = table->vreg[reg].placement[pcount];
			int s = i%24;
			int t = j%24;
			int x = READ_REG((s/(4*Tile_dim))%Array_dim, (t/(4*Tile_dim))%Array_dim, (s/4)%Tile_dim, (t/4)%Tile_dim, ((s*4 +t)%4 + s*4)%16, preg);
			if(x==0)printf("x: %d, pcount: %d, preg: %d, i,j: %d,%d, s,t: %d,%d\n", x, pcount, preg, i, j, s, t);
			printf("%d, ", x);
		}
		printf("\n");
		usleep_A53(5);
	}
}
