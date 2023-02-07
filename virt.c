#include "virt.h"
#include "AllocationTable.h"
#include "spar.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "xil_printf.h"

#define numberofreg 16 //todo-->have this redefined to use a more global variable
#define SPAR_dimension (4*Array_dim*Tile_dim) //number of PE's found either horizontally or vertically
#define minimumNumOfVReg 4 //minimum number of virtual registers that need to fit in spar to perform operations
#define multiplicationOverFlow true //indicate that multiplication changes the values in the physical register above the result register

void quicksortModified(int number[],int first,int last, int indices[]){
	int i, j, pivot, temp;
	if(first<last){
		pivot=first;
		i=first;
		j=last;

		while(i<j){

			while(number[i]<=number[pivot]&&i<last)
				i++;

			while(number[j]>number[pivot])
				j--;

			if(i<j){ //swap
				temp=number[i];
				number[i]=number[j];
				number[j]=temp;

				temp=indices[i];
				indices[i]=indices[j];
				indices[j]=temp;
			}
		}
		temp=number[pivot];
		number[pivot]=number[j];
		number[j]=temp;
		temp=indices[pivot];
		indices[pivot]=indices[j];
		indices[j]=temp;
		quicksortModified(number, first, j-1, indices);
		quicksortModified(number, j+1, last, indices);
	}
}

extern inline void PrepareRegMMM(int rs1, int rs2, int rd, AllocationTable *table){//not to be called outside of this file
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

extern inline void PrepareRegMM(int rs, int rd, AllocationTable *table){
	if(table->vreg[rs].status != 1)
	{
		int reserveReg[] = {-1, -1};
		if(table->vreg[rd].status==1 && rd != rs)
		{
			reserveReg[0] = rd;
			safeAllocatePRegs(rs, SPAR_dimension, reserveReg, 1, table);
		}
		else
		{
			safeAllocatePRegs(rs, SPAR_dimension, reserveReg, 0, table);
		}
	}

	if(table->vreg[rd].status != 1){
		int reserveReg[] = {rs};
		safeAllocateEmptyPRegs(rd, SPAR_dimension, reserveReg, 1, table);
	}
}

void EastLoopAround(int prs, int prd, AllocationTable *table){
	for(int a = 0; a < Array_dim; a++)
	{
		for (int b = 0; b < Tile_dim; b++)
		{
			for(int j = 3; j < 16; j+=4)
			{
				int x = READ_REG(a, Array_dim-1, b, Tile_dim-1, j, prs);//READ_REG(a, Array_dim-1, b, Tile_dim-1, j, rs);
				WRITE_REG(a, 0, b, 0, j-3, prd, x);
			}
		}
	}
}
void ShiftEast_M( int rs, int rd, AllocationTable *table){
	table->vreg[rd].cols=table->vreg[rs].cols;
	table->vreg[rd].rows=table->vreg[rs].rows;
	table->vreg[rd].orientation = table->vreg[rs].orientation;
	table->vreg[rd].type = table->vreg[rs].type;

	//todo: see if there is a better way to calculate the num of row and column segments
	int colFol = ceil((float)table->vreg[rs].cols/(float)SPAR_dimension); //calculate the number of column segments
	int rowFol = ceil((float)table->vreg[rs].rows/(float)SPAR_dimension);

	//make sure that the registers are in the SPAR
	PrepareRegMM(rs, rd, table);

	int i = 0;
	for(int s=0; s<rowFol; s++)
	{
		for(int t=0; t<colFol; t++)
		{
			i = t+(s*colFol);
			execute(5, table->vreg[rd].placement[i], table->vreg[rs].placement[i], 0);
			if(t != 0)
			{
				printf("About to east loop aroud; \t prs: %d; prd: %d;\n", table->vreg[rs].placement[i-1], table->vreg[rd].placement[i]);
				EastLoopAround(table->vreg[rs].placement[i-1], table->vreg[rd].placement[i], table);
			}
		}
	}
}

void WestLoopAround(int prs, int prd, AllocationTable *table){
	for(int a = 0; a < Array_dim; a++)
	{
		for (int b = 0; b < Tile_dim; b++)
		{
			for(int j = 3; j < 16; j+=4)
			{
				int x = READ_REG(a, 0, b, 0, j-3, prs);//READ_REG(a, Array_dim-1, b, Tile_dim-1, j, rs);
				WRITE_REG(a, Array_dim-1, b, Tile_dim-1, j, prd, x);
			}
		}
	}
}
void ShiftWest_M(int rs, int rd, AllocationTable *table){//moves the matrix in orientation 0 west in virtual array
	table->vreg[rd].cols=table->vreg[rs].cols;
	table->vreg[rd].rows=table->vreg[rs].rows;
	table->vreg[rd].orientation = table->vreg[rs].orientation;
	table->vreg[rd].type = table->vreg[rs].type;

	//todo: see if there is a better way to calculate the num of row and column segments
	int colFol = ceil((float)table->vreg[rs].cols/(float)SPAR_dimension); //calculate the number of column segments
	int rowFol = ceil((float)table->vreg[rs].rows/(float)SPAR_dimension);

	//make sure that the registers are in the SPAR
	PrepareRegMM(rs, rd, table);

	int i = 0;
//	for(int s = rowFol-1; s>=0; s--)
//	{
//		for(int t = colFol-1; t>=0; t--)
//		{
//			execute(8, table->vreg[rd].placement[i], table->vreg[rs].placement[i], 0);
//			if(t != colFol-1)
//			{
//				printf("About to west loop aroud; \t prs: %d; prd: %d;\n", table->vreg[rs].placement[i-1], table->vreg[rd].placement[i]);
//				WestLoopAround(table->vreg[rs].placement[i-1], table->vreg[rd].placement[i], table);
//			}
//			i++;
//		}
//	}
	for(int s=0; s<rowFol; s++)
	{
		for(int t=0; t<colFol; t++)
		{
			i = t+(s*colFol);
			execute(6, table->vreg[rd].placement[i], table->vreg[rs].placement[i], 0);
			if(t == colFol-1)
			{
				printf("About to west loop aroud; \t prs: %d; prd: %d;\n", table->vreg[rs].placement[i-1], table->vreg[rd].placement[i]);
				WestLoopAround(table->vreg[rs].placement[i], table->vreg[rd].placement[i-1], table);
			}
		}
	}
}

void CopyVector(int rs, int numberOfCopies, AllocationTable *table){
	if(table->vreg[rs].orientation == 1)
	{
		//todo: make sure the vector is in SPAR
		for(int i=0; i<numberOfCopies; i++)
		{
			ShiftEast_M(rs, rs, table);
		}
	}
	else
	{
		//todo: make sure the vector is in SPAR
		for(int i=0; i<numberOfCopies; i++)
		{
//			ShiftSouth_M(rs, rs, table);
		}
	}
}

void CastReg_V(int rs, int orientation, AllocationTable *table) {
	if(table->vreg[rs].type == 1) {return;} //leave if already a vector
	if(orientation==0)
	{

	}
	else
	{

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

void Declare_V(Vector *v, int size){
	v->size = size;
	v->memory = (int*)malloc(size*sizeof(int));
}

void Deallocate_V(Vector *v){
	free(v->memory);
}

////////////////////////////----------------Load and Store Operations----------------/////////////////////////
void Store_M(Matrix* m, int rd, AllocationTable *table) { //does not actually store a matrix. Just allocates it in the allocation table
	removeVRegFromPRegs(rd, table); //remove the current vreg from all pregs
	int colFol = ceil((float)m->cols/(float)SPAR_dimension);
	int rowFol = ceil((float)m->rows/(float)SPAR_dimension);

	int copies = colFol * rowFol;
	if (copies > numberofreg/minimumNumOfVReg) //todo-->need to actually implement this if statement properly
	{
		printf("not enough registers to load array");
		return; //exits the function if there are not enough registers
	}

	allocateVRegM(m, rd, 0, table);
}

//not done. Ignore
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

	allocateVRegM_T(m, rd, 0, table);
	//todo: remove previous VReg data from PRegs
}

void Store_V(Vector* v, int rd, AllocationTable *table){
	//todo: check if size is too big or not
	allocateVRegV(v, rd, table);
}

void Load_V(Vector* v, int rs, AllocationTable *table){
	if(table->vreg[rs].status==1 ||table->vreg[rs].status==2)
	{
		copyFromPRegsToVRegData(rs, table);
	}
	int numElements = table->vreg[rs].rows;
	free(v->memory);
	for(int i=0; i<numElements; i++)
	{
		printf("%d, ", table->vreg[rs].data[i]);
		v->memory[i] = table->vreg[rs].data[i];
	}
	printf("\n");
}

///////////////////-----------------Math Operations-------------------------------//////////////////////////
void E_Add_MM(int rs1, int rs2, int rd, AllocationTable *table){
	//set result up
	table->vreg[rd].cols = table->vreg[rs1].cols;
	table->vreg[rd].rows = table->vreg[rs1].rows;
	table->vreg[rd].orientation = table->vreg[rs1].orientation;
	table->vreg[rd].type = table->vreg[rs1].type;
	printf("here b4 freez1\n");
	PrepareRegMMM(rs1, rs2, rd, table);
	for(int i = 0; i < 6; i++){//todo: replace 6 with a variable or macro
//		printf("i: %d, rd: %d, rs1: %d, rs2: %d\n", i, table->vreg[rd].placement[i], table->vreg[rs1].placement[i], table->vreg[rs2].placement[i]);
		if(table->vreg[rd].placement[i]>=0 && table->vreg[rs1].placement[i]>=0 && table->vreg[rs2].placement[i]>=0) execute(0, table->vreg[rd].placement[i], table->vreg[rs1].placement[i], table->vreg[rs2].placement[i]);
		else break; //break out of loop if one of the placements is -1
	}
}

void E_Sub_MM(int rs1, int rs2, int rd, AllocationTable *table)
{
	//set result up
	table->vreg[rd].cols = table->vreg[rs1].cols;
	table->vreg[rd].rows = table->vreg[rs1].rows;
	table->vreg[rd].orientation = table->vreg[rs1].orientation;
	table->vreg[rd].type = table->vreg[rs1].type;
	PrepareRegMMM(rs1, rs2, rd, table);
	for(int i = 0; i < 6; i++){//todo: replace 6 with a variable or macro
		if(table->vreg[rd].placement[i]>=0 && table->vreg[rs1].placement[i]>=0 && table->vreg[rs2].placement[i]>=0) execute(1, table->vreg[rd].placement[i], table->vreg[rs1].placement[i], table->vreg[rs2].placement[i]);
		else break; //break out of loop if one of the placements is -1
	}
}

void E_Mul_MM(int rs1, int rs2, int rd, AllocationTable *table)
{
	//set result up
	table->vreg[rd].cols = table->vreg[rs1].cols;
	table->vreg[rd].rows = table->vreg[rs1].rows;
	table->vreg[rd].orientation = table->vreg[rs1].orientation;
	table->vreg[rd].type = table->vreg[rs1].type;
	PrepareRegMMM(rs1, rs2, rd, table);
	//todo: make the placements execute in order of lowest number PE to highest in order to avoid messing up other rd pregs.
	int placementCopy[6];
	int indices[6];
	for(int i = 0; i<6; i++)
	{
		placementCopy[i]=table->vreg[rd].placement[i];
		indices[i]=i;
	}

	quicksortModified(placementCopy, 0, 5, indices);

	for(int j=0; j<6; j++)
	{
		int i = indices[j];
		if(table->vreg[rd].placement[i]>=0 && table->vreg[rs1].placement[i]>=0 && table->vreg[rs2].placement[i]>=0)
		{
			printf("here %d\n", table->preg[table->vreg[rd].placement[i]+1]);
			if(table->preg[table->vreg[rd].placement[i]+1]!=rd) //account for register above the destination register being changed.
			{
				printf("removing vreg from preg: %d", table->preg[table->vreg[rd].placement[i]+1]);
				removeVRegFromPRegs(table->preg[table->vreg[rd].placement[i]+1], table);
			}
			execute(2, table->vreg[rd].placement[i], table->vreg[rs1].placement[i], table->vreg[rs2].placement[i]);
		}
	}

//	for(int i = 0; i < 6; i++){//todo: replace 6 with a variable or macro
//		if(table->vreg[rd].placement[i]>=0 && table->vreg[rs1].placement[i]>=0 && table->vreg[rs2].placement[i]>=0)
//		{
//			printf("here %d\n", table->preg[table->vreg[rd].placement[i]+1]);
//			if(table->preg[table->vreg[rd].placement[i]+1]!=rd) //account for register above the destination register being changed.
//			{
//				printf("removing vreg from preg: %d", table->preg[table->vreg[rd].placement[i]+1]);
//				removeVRegFromPRegs(table->preg[table->vreg[rd].placement[i]+1], table);
//			}
//			execute(2, table->vreg[rd].placement[i], table->vreg[rs1].placement[i], table->vreg[rs2].placement[i]);
//		}
//		else break; //break out of loop if one of the placements is -1
//	}
}

void Accumulate_M(int rs, int rd, int tempReg, AllocationTable *table)
{
	//clear tempReg and destination register
	E_Sub_MM(rs, rs, tempReg, table);
	E_Sub_MM(rs, rs, rd, table);

	//move data initially into tempReg
	ShiftWest_M(rs, tempReg, table);
	//add
	E_Add_MM(tempReg, rs, rd, table);
	for(int i = 0; i < (table->vreg[rs].cols)-1; i++)
	{
		ShiftWest_M(rd, tempReg, table);
		E_Add_MM(tempReg, rs, rd, table);
	}
	//cast result to vector
}

void E_Add_VV(int rs1, int rs2, int rd, AllocationTable *table){
	//todo: remove vreg copy data from pregs (resize vector and account for it)
	//todo: account if registers need to be reoriented.
	table->vreg[rd].cols = 1;
	table->vreg[rd].rows = table->vreg[rs1].rows;
	table->vreg[rd].orientation = table->vreg[rs1].orientation;
	table->vreg[rd].type = 1;
	PrepareRegMMM(rs1, rs2, rd, table);
	for(int i=0; i<6; i++)
	{
		if(table->vreg[rd].placement[i]>=0 && table->vreg[rs1].placement[i]>=0 && table->vreg[rs2].placement[i]>=0) execute(0, table->vreg[rd].placement[i], table->vreg[rs1].placement[i], table->vreg[rs2].placement[i]);
		else break;
	}
}

void E_Sub_VV(int rs1, int rs2, int rd, AllocationTable *table){
	//todo: remove vreg copy data from pregs (resize vector and account for it)
	//todo: account if registers need to be reoriented.
	table->vreg[rd].cols = 1;
	table->vreg[rd].rows = table->vreg[rs1].rows;
	table->vreg[rd].orientation = table->vreg[rs1].orientation;
	table->vreg[rd].type = 1;
	PrepareRegMMM(rs1, rs2, rd, table);
	for(int i=0; i<6; i++)
	{
		if(table->vreg[rd].placement[i]>=0 && table->vreg[rs1].placement[i]>=0 && table->vreg[rs2].placement[i]>=0) execute(1, table->vreg[rd].placement[i], table->vreg[rs1].placement[i], table->vreg[rs2].placement[i]);
		else break;
	}
}

void E_Mul_VV(int rs1, int rs2, int rd, AllocationTable *table){
	table->vreg[rd].cols = 1;
	table->vreg[rd].rows = table->vreg[rs1].rows;
	table->vreg[rd].orientation = table->vreg[rs1].orientation;
	table->vreg[rd].type = 1;
	//todo: remove vreg copy data from pregs (resize vector and account for it)
	//todo: account if registers need to be reoriented.
	PrepareRegMMM(rs1, rs2, rd, table);
	printTablePReg(table);

	int placementCopy[6];
	int indices[6];
	for(int i = 0; i<6; i++)
	{
		placementCopy[i]=table->vreg[rd].placement[i];
		indices[i]=i;
	}

	quicksortModified(placementCopy, 0, 5, indices);

	for(int j=0; j<6; j++)
	{
		int i = indices[j];
		printf("%d\n", i);
		if(table->vreg[rd].placement[i]>=0 && table->vreg[rs1].placement[i]>=0 && table->vreg[rs2].placement[i]>=0)
		{
			printf("here %d\n", table->preg[table->vreg[rd].placement[i]+1]);
			if(table->preg[table->vreg[rd].placement[i]+1]!=rd && table->preg[table->vreg[rd].placement[i]+1]>=0) //account for register above the destination register being changed.
			{
				printf("removing vreg from preg: %d\n", table->preg[table->vreg[rd].placement[i]+1]);
				removeVRegFromPRegs(table->preg[table->vreg[rd].placement[i]+1], table);
			}
			printf("Multiplying %d x %d = %d\n",  table->vreg[rs1].placement[i], table->vreg[rs2].placement[i], table->vreg[rd].placement[i]);
			execute(2, table->vreg[rd].placement[i], table->vreg[rs1].placement[i], table->vreg[rs2].placement[i]);
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
