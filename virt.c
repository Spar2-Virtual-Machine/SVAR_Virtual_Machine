#include "virt.h"
#include "AllocationTable.h"
#include "spar.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "xil_printf.h"
#include "sleep.h"

extern inline void PrintCounts_v(){
	printf("Add Count: %d \n Sub Count: %d \n Mul Count %d\n", sparAdd_cnt, sparSub_cnt, sparMul_cnt);
	printf("North Count: %d \n South Count: %d \n East Count: %d \n West Count: %d\n", sparShN_cnt, sparShS_cnt, sparShE_cnt, sparShW_cnt);
	printf("Write Count: %d \n Read Count: %d\n", sparWR_cnt, sparRD_cnt);
}

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

extern inline void SafelyMoveToAnotherPREG(int preg, int reservedPregs[], int numberOfReserved, AllocationTable *table)
{
	//original point for the round robin pointer (may need to be changed)
	table->nextRegToUpdate %= Num_PREG; //safety check to make sure round robin pointer is in range
	int origin = table->nextRegToUpdate;
	do
	{
		if(table->preg[table->nextRegToUpdate] != table->preg[preg]) //make sure you aren't overriding another part of the source vreg
		{
			bool valid = true;
			for(int i = 0; i < numberOfReserved && valid == true; i++)
			{
				if(reservedPregs[i] == table->nextRegToUpdate) {valid = false; break;}
			}
			if(valid == true)
			{
				MoveToAnotherPREG(preg, table->nextRegToUpdate, table);
				table->nextRegToUpdate++;
				table->nextRegToUpdate%=Num_PREG;
				return;}
		}

		table->nextRegToUpdate++;
		table->nextRegToUpdate%=Num_PREG;
	} while(table->nextRegToUpdate != origin);
	printf("Error: Unable to move preg %d to another register\n", preg);
}

void MoveToAnotherPREG(int rs, int rd, AllocationTable *table){ //todo: test that this still works after changing
	int vreg = table->preg[rs];
	int replaced_vreg = table->preg[rd];
	if(replaced_vreg >=0) removeVRegFromPRegs(replaced_vreg, table);
	//Shift East, then Shift West
	execute(5, rd, rs, 0);
	execute(6, rd, rd, 0);
	//copy rightmost column
	for(int a = 0; a < Array_dim; a++)
	{
		for(int c = 0; c < Tile_dim; c++)
		{
			for(int pe = 3; pe < 16; pe+=4)
			{
				WRITE_REG(a, Array_dim, c, Tile_dim, pe, rd, READ_REG(a,Array_dim,c,Tile_dim,pe,rs));
			}
		}
	}

	for(int i=0; i<Max_PrForVr; i++) //todo: replace 6 with a variable
	{
		if(table->vreg[vreg].placement[i] == rs)
		{
			table->vreg[vreg].placement[i] = rd;
			break;
		}
	}
	table->preg[rd]=vreg;
	table->preg[rs]=-1;
}

extern inline void PrepareReg_E_MMM(int rs1, int rs2, int rd, AllocationTable *table){//not to be called outside of this file.
	//todo have registers in correct orientation. Account for size changes that may have occured.
	//check and set orientations
	int orient0 = 0; //count of registers already in SPAR with the orientation of 0
	int orient1 = 0;
	if(table->vreg[rs1].status==1)
	{
		if(table->vreg[rs1].orientation==0) orient0++;
		else orient1++;
	}
	if(table->vreg[rs2].status==1)
	{
		if(table->vreg[rs2].orientation==0) orient0++;
		else orient1++;
	}

	//check if destination register is ready
	if(table->vreg[rd].status==1)
	{
		//check validity; remove destination reference from pregs if it is not compatible dimensions
		if((table->vreg[rd].rows/SPAR_dimension) != (table->vreg[rs1].rows/SPAR_dimension) || (table->vreg[rd].cols/SPAR_dimension) != (table->vreg[rs1].cols/SPAR_dimension))
		{
			removeVRegFromPRegsNoData(rd, table); //todo: see if this change does anything
		}
		else if(table->vreg[rd].orientation==0) orient0++;
		else orient1++;
	}

	//determine the orientation.
	int orientation = 0;
	if(orient1>orient0) orientation = 1;

	//update orientation for each register.
	if(table->vreg[rs1].orientation!=orientation)
	{
		//todo: fix orientation more efficiently than removing and re-adding matrix
		if(table->vreg[rs1].status==1) removeVRegFromPRegs(rs1, table);
		table->vreg[rs1].orientation = orientation;
	}

	if(table->vreg[rs2].orientation!=orientation)
	{
		//todo: fix orientation more efficiently than removing and re-adding matrix
		if(table->vreg[rs2].status==1) removeVRegFromPRegs(rs2, table);
		table->vreg[rs2].orientation = orientation;
	}

	if(table->vreg[rd].orientation!=orientation)
	{
		//todo: fix orientation more efficiently than removing and re-adding matrix
		if(table->vreg[rd].status==1) removeVRegFromPRegs(rd, table);
		table->vreg[rd].orientation = orientation;
	}

	//set destination register values
	table->vreg[rd].cols = table->vreg[rs1].cols;
	table->vreg[rd].rows = table->vreg[rs1].rows;
	table->vreg[rd].orientation = orientation;
	table->vreg[rd].type = table->vreg[rs1].type;


	//now safely allocate the registers

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
		safeAllocatePRegs(rs1, reserveReg, numReserved, table);
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
		safeAllocatePRegs(rs2, reserveReg, numReserved, table);
	}

	if(table->vreg[rd].status != 1){
		//assume that rs1 and rs2 are in SPAR
		int reserveReg[2] = {rs1, rs2};
		safeAllocateEmptyPRegs(rd, reserveReg, 2, table);
	}
}

extern inline void PrepareReg_E_VVV(int rs1, int rs2, int rd, AllocationTable *table){//not to be called outside of this file.
	//todo have registers in correct orientation. Account for size changes that may have occured.
	//check and set orientations
	int orient0 = 0; //count of registers already in SPAR with the orientation of 0
	int orient1 = 0;
	if(table->vreg[rs1].status==1)
	{
		if(table->vreg[rs1].orientation==0) orient0++;
		else orient1++;
	}
	if(table->vreg[rs2].status==1)
	{
		if(table->vreg[rs2].orientation==0) orient0++;
		else orient1++;
	}

	//check if destination register is ready
	if(table->vreg[rd].status==1)
	{
		//check validity; remove destination reference from pregs if it is not compatible dimensions
		if((table->vreg[rd].rows/SPAR_dimension) != (table->vreg[rs1].rows/SPAR_dimension) || (table->vreg[rd].cols/SPAR_dimension) != (table->vreg[rs1].cols/SPAR_dimension))
		{
			removeVRegFromPRegs(rd, table);
		}
		else if(table->vreg[rd].orientation==0) orient0++;
		else orient1++;
	}

	//determine the orientation.
	int orientation = 0;
	if(orient1>orient0) orientation = 1;

	//update orientation for each register.
	if(table->vreg[rs1].orientation!=orientation)
	{
		//todo: fix orientation more efficiently than removing and re-adding matrix
		if(table->vreg[rs1].status==1) removeVRegFromPRegs(rs1, table);
		table->vreg[rs1].orientation = orientation;
	}

	if(table->vreg[rs2].orientation!=orientation)
	{
		//todo: fix orientation more efficiently than removing and re-adding matrix
		if(table->vreg[rs2].status==1) removeVRegFromPRegs(rs2, table);
		table->vreg[rs2].orientation = orientation;
	}

	if(table->vreg[rd].orientation!=orientation)
	{
		//todo: fix orientation more efficiently than removing and re-adding matrix
		if(table->vreg[rd].status==1) removeVRegFromPRegs(rd, table);
		table->vreg[rd].orientation = orientation;
	}

	//set destination register values
	table->vreg[rd].cols = table->vreg[rs1].cols;
	table->vreg[rd].rows = table->vreg[rs1].rows;
	table->vreg[rd].orientation = orientation;
	table->vreg[rd].type = table->vreg[rs1].type;


	//now safely allocate the registers
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
		safeAllocatePRegs(rs1, reserveReg, numReserved, table);
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
		safeAllocatePRegs(rs2, reserveReg, numReserved, table);
	}

	if(table->vreg[rd].status != 1){
		//assume that rs1 and rs2 are in SPAR
		int reserveReg[2] = {rs1, rs2};
		safeAllocateEmptyPRegs(rd, reserveReg, 2, table);
	}
}


extern inline void PrepareRegMM(int rs, int rd, AllocationTable *table){
	if(table->vreg[rs].status != 1)
	{
		int reserveReg[] = {-1, -1};
		if(table->vreg[rd].status==1 && rd != rs)
		{
			reserveReg[0] = rd;
			safeAllocatePRegs(rs, reserveReg, 1, table);
		}
		else
		{
			safeAllocatePRegs(rs, reserveReg, 0, table);
		}
	}

	if(table->vreg[rd].status != 1){
		int reserveReg[] = {rs};
		safeAllocateEmptyPRegs(rd, reserveReg, 1, table);
	}
}

extern inline void PrepareReg_Mul_MVM(int rsm, int rsv, int rd, AllocationTable *table){ //prepares registers for //rd is actually the temp
	//remove vector from preg if width > 1 preg and in SPAR
	if(table->vreg[rsv].status==1 && table->vreg[rsv].cols > SPAR_dimension) //todo: optimize this. May remove entirely
	{
		table->vreg[rsv].cols = 1;
		removeVRegFromPRegs(rsv, table);
	}

	//determine orientations
	//if matrix not in memory
	if(table->vreg[rsm].status==0)
	{
		if(table->vreg[rsv].status==0)
		{
			//default orientations
			table->vreg[rsm].orientation = 0;
			table->vreg[rsv].orientation = 1; //vector lays columnwise
		}
		else
		{
			if(table->vreg[rsv].orientation == 0) table->vreg[rsm].orientation = 1;
			else table->vreg[rsm].orientation = 0;
		}
	}
	else //if the matrix is in memory, it should take top priority since it is likely to have more data (harder to re-orient).
	{
		if(table->vreg[rsv].status==0)
		{
			//Set orientations; no need to worry
			if(table->vreg[rsm].orientation == 0) table->vreg[rsv].orientation=1;
			else table->vreg[rsv].orientation=0; //vector lays columnwise
		}
		else
		{
			//four cases; 2 don't require altering the vector's orientation
			//account for 2 that require removing the vector and reorienting it.
			if(table->vreg[rsm].orientation==0 && table->vreg[rsv].orientation==0){
				removeVRegFromPRegs(rsv, table);
				table->vreg[rsm].orientation = 1;
			}
			else if(table->vreg[rsm].orientation==1 && table->vreg[rsv].orientation==1){
				removeVRegFromPRegs(rsv, table);
				table->vreg[rsm].orientation = 0;
			}
		}
	}

	FillVector(rsv, rd, table); //fill in the vector to fit 1 reg
	//remove rd from preg if necessary //todo: can be optimized here to cast from vector to empty matrix somehow?
	removeVRegFromPRegsNoData(rd, table); //todo: see if this changes any of the results
	//set rd sizes and orientation
	table->vreg[rd].cols = table->vreg[rsm].cols;
	table->vreg[rd].rows = table->vreg[rsm].rows;
	table->vreg[rd].orientation = table->vreg[rsm].orientation;
	table->vreg[rd].type = table->vreg[rsm].type; //could be hard coded since we are setting it to a matrix

	//assign pregs for sources
	if(table->vreg[rsm].status != 1)
	{
		//void safeAllocatePRegs(int vRegNum, int maxDim, int protectedVReg[], int numProtected, AllocationTable *table);
		int reserveReg[] = {-1,-1};
		int numReserved = 0;

		if(table->vreg[rsv].status == 1 && rsm != rsv)
		{ //don't remove rsv if it is already in SPAR
			reserveReg[numReserved] = rsv;
			numReserved++;
		}
		if(table->vreg[rd].status == 1 && rd != rsm)
		{
			reserveReg[numReserved] = rd;
			numReserved++;
		}
		safeAllocatePRegs(rsm, reserveReg, numReserved, table);
	}

	if(table->vreg[rsv].status != 1){
		//assume that rsm is in SPAR already
		int reserveReg[2] = {-1, -1};
		reserveReg[0] = rsm;
		int numReserved = 1;
		if(table->vreg[rd].status == 1 && rd != rsv)
		{
			reserveReg[1] = rd;
			numReserved++;
		}
		safeAllocatePRegs(rsv, reserveReg, numReserved, table);
	}

	//assign pregs for rd
	if(table->vreg[rd].status != 1){
		//assume that rsm and rsv are in SPAR
		int reserveReg[2] = {rsm, rsv};
		safeAllocateEmptyPRegs(rd, reserveReg, 2, table);
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

	if(table->vreg[rs].orientation==0)
	{
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
					EastLoopAround(table->vreg[rs].placement[i-1], table->vreg[rd].placement[i], table);
				}
			}
		}
	}
	else
	{
		int colFol = ceil((float)table->vreg[rs].rows/(float)SPAR_dimension); //calculate the number of column segments
		int rowFol = ceil((float)table->vreg[rs].cols/(float)SPAR_dimension);

		//make sure that the registers are in the SPAR
		PrepareRegMM(rs, rd, table);

		int i = 0;
		for(int s=0; s<rowFol; s++)
		{
			for(int t=0; t<colFol; t++)
			{
				i = t*rowFol+s;
				execute(5, table->vreg[rd].placement[i], table->vreg[rs].placement[i], 0);
				if(t > 0)
				{
					EastLoopAround(table->vreg[rs].placement[i-rowFol], table->vreg[rd].placement[i], table);
				}
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

	if(table->vreg[rs].orientation==0)
	{
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
				execute(6, table->vreg[rd].placement[i], table->vreg[rs].placement[i], 0);
				if(t != 0)
				{
					WestLoopAround(table->vreg[rs].placement[i], table->vreg[rd].placement[i-1], table);
				}
			}
		}
	}
	else
	{
		int colFol = ceil((float)table->vreg[rs].rows/(float)SPAR_dimension); //calculate the number of column segments
		int rowFol = ceil((float)table->vreg[rs].cols/(float)SPAR_dimension);


		//make sure that the registers are in the SPAR
		PrepareRegMM(rs, rd, table);

		int i = 0;
		for(int s=0; s<rowFol; s++)
		{
			for(int t=0; t<colFol; t++)
			{
				i = t*rowFol+s;
				execute(6, table->vreg[rd].placement[i], table->vreg[rs].placement[i], 0);
				if(t > 0)
				{
					WestLoopAround(table->vreg[rs].placement[i], table->vreg[rd].placement[i-rowFol], table);
				}
			}
		}
	}
}

void SouthLoopAround(int prs, int prd, AllocationTable *table){
	for(int a = 0; a < Array_dim; a++)
	{
		for (int b = 0; b < Tile_dim; b++)
		{
			for(int j = 0; j < 4; j+=1)
			{
				int x = READ_REG(Array_dim-1, a, Tile_dim-1, b, j+12, prs);
						//READ_REG(0, a, 0, b, j, prs);//last 4 pes
				WRITE_REG(0, a, 0, b, j, prd, x);//first 4 pes
			}
		}
	}
}

void ShiftSouth_M(int rs, int rd, AllocationTable *table){
	table->vreg[rd].cols=table->vreg[rs].cols;
	table->vreg[rd].rows=table->vreg[rs].rows;
	table->vreg[rd].orientation = table->vreg[rs].orientation;
	table->vreg[rd].type = table->vreg[rs].type;
	//todo: see if there is a better way to calculate the num of row and column segments
	int colFol; //calculate the number of column segments
	int rowFol;
	if(table->vreg[rs].orientation==0)
	{
		colFol = ceil((float)table->vreg[rs].cols/(float)SPAR_dimension);
		rowFol = ceil((float)table->vreg[rs].rows/(float)SPAR_dimension);
		//make sure that the registers are in the SPAR
			PrepareRegMM(rs, rd, table);

			int i = 0;
			for(int s=0; s<rowFol; s++)
			{
				for(int t=0; t<colFol; t++)
				{
					i = t+(s*colFol);
					execute(7, table->vreg[rd].placement[i], table->vreg[rs].placement[i], 0);
					if(s != 0)//Do not need to do this for the first row. Will get from source preg above and copy first row to current destination
					{
						SouthLoopAround(table->vreg[rs].placement[i-colFol], table->vreg[rd].placement[i], table);
					}
				}
			}
	}
	else
	{
		colFol = ceil((float)table->vreg[rs].rows/(float)SPAR_dimension);
		rowFol = ceil((float)table->vreg[rs].cols/(float)SPAR_dimension);
		//make sure that the registers are in the SPAR
		PrepareRegMM(rs, rd, table);

		int i = 0;
		for(int s=0; s<rowFol; s++)
		{
			for(int t=0; t<colFol; t++)
			{
				i = t*rowFol+s;
				execute(7, table->vreg[rd].placement[i], table->vreg[rs].placement[i], 0);
				if(s > 0)//Do not need to do this for the first row. Will get from source preg above and copy first row to current destination
				{
					SouthLoopAround(table->vreg[rs].placement[i-1], table->vreg[rd].placement[i], table);
				}
			}
		}
	}

}


void NorthLoopAround(int prs, int prd, AllocationTable *table){
	for(int a = 0; a < Array_dim; a++)
	{
		for (int b = 0; b < Tile_dim; b++)
		{
			for(int j = 0; j < 4; j+=1)
			{
				int x = READ_REG(0, a, 0, b, j, prs);//last 4 pes
				WRITE_REG(Array_dim-1, a, Tile_dim-1, b, j+12, prd, x);//first 4 pes
			}
		}
	}
}

void ShiftNorth_M(int rs, int rd, AllocationTable *table){
	table->vreg[rd].cols=table->vreg[rs].cols;
	table->vreg[rd].rows=table->vreg[rs].rows;
	table->vreg[rd].orientation = table->vreg[rs].orientation;
	table->vreg[rd].type = table->vreg[rs].type;
	//todo: see if there is a better way to calculate the num of row and column segments
	int colFol; //calculate the number of column segments
	int rowFol;
	if(table->vreg[rs].orientation==0)
	{
		colFol = ceil((float)table->vreg[rs].cols/(float)SPAR_dimension);
		rowFol = ceil((float)table->vreg[rs].rows/(float)SPAR_dimension);
		//make sure that the registers are in the SPAR
		PrepareRegMM(rs, rd, table);

		int i = 0;
		for(int s=0; s<rowFol; s++)
		{
			for(int t=0; t<colFol; t++)
			{
				i = t+(s*colFol);
				execute(8, table->vreg[rd].placement[i], table->vreg[rs].placement[i], 0);
				if(s != 0)
				{
					NorthLoopAround(table->vreg[rs].placement[i], table->vreg[rd].placement[i-colFol], table);
				}
			}
		}
	}
	else
	{
		colFol = ceil((float)table->vreg[rs].rows/(float)SPAR_dimension);
		rowFol = ceil((float)table->vreg[rs].cols/(float)SPAR_dimension);
		//make sure that the registers are in the SPAR
		PrepareRegMM(rs, rd, table);

		int i = 0;
		for(int s=0; s<rowFol; s++)
		{
			for(int t=0; t<colFol; t++)
			{
				i = t*rowFol+s;
				execute(8, table->vreg[rd].placement[i], table->vreg[rs].placement[i], 0);
				if(s != 0)
				{
					NorthLoopAround(table->vreg[rs].placement[i], table->vreg[rd].placement[i-1], table);
				}
			}
		}
	}
}

void FillVector(int rs, int tempr, AllocationTable *table){ //copy vector except it wills up an entire preg of width. Needed for vector matrix multiplication
	//todo: remove unneccessary vector preg's if the width > 1 preg
	table->vreg[rs].cols = SPAR_dimension;
	table->vreg[tempr].cols = SPAR_dimension;
	table->vreg[tempr].rows = table->vreg[rs].rows;
	table->vreg[tempr].orientation = table->vreg[rs].orientation;
	table->vreg[tempr].type = 1;
	if(table->vreg[rs].orientation == 0)
	{
		//todo: make sure the vector is in SPAR and has enough registers for copying
		PrepareRegMM(rs, tempr, table);
		//clear all but first row of the source
		for(int i=0; i<Max_PrForVr; i++)
		{
			if(table->vreg[tempr].placement[i]>=0) {
				//shift West into temp
				execute(6, table->vreg[tempr].placement[i], table->vreg[rs].placement[i], 0);
				//Shift East
				execute(5, table->vreg[tempr].placement[i], table->vreg[tempr].placement[i], 0);
			}
			//set all other rows to 0
			execute(1, table->vreg[rs].placement[i], table->vreg[rs].placement[i], table->vreg[tempr].placement[i]);
		}
		//clear temp reg
		E_Sub_MM(rs, rs, tempr, table);
		ShiftEast_M(rs, tempr, table); //copy vector into the temp register
		for(int i=0; i<SPAR_dimension-1; i++)
		{
			E_Add_MM(tempr, rs, rs, table);
			ShiftEast_M(tempr, tempr, table);
		}
	}
	else
	{
		//todo: make sure the vector is in SPAR and has enough registers for copying
		PrepareRegMM(rs, tempr, table);
		//clear all but first row of the source
		for(int i=0; i<Max_PrForVr; i++)
		{
			if(table->vreg[tempr].placement[i]>=0) //make sure it is not negative
			{
				//shift north into temp
				execute(8, table->vreg[tempr].placement[i], table->vreg[rs].placement[i], 0);
				//Shift South
				execute(7, table->vreg[tempr].placement[i], table->vreg[tempr].placement[i], 0);
			}
			//set all other rows to 0
			if(table->vreg[tempr].placement[i]>=0){ //make sure it is not negative
				execute(1, table->vreg[rs].placement[i], table->vreg[rs].placement[i], table->vreg[tempr].placement[i]);
			}
		}
		//clear temp reg
		E_Sub_MM(rs, rs, tempr, table);
		ShiftSouth_M(rs, tempr, table);
		for(int i=0; i<SPAR_dimension-1; i++)
		{
			E_Add_MM(tempr, rs, rs, table);
			ShiftSouth_M(tempr, tempr, table);
		}

	}
}

void CastRegTo_V(int rs, int orientation, AllocationTable *table) { //orientation refers to how the vector is in relation to the matrix. May rename that.
	if(table->vreg[rs].type == 1) {return;} //leave if already a vector
	//if in memory. Move the first elements of each row in place
	if(orientation==0)
	{
		if(table->vreg[rs].status == 0)
		{
			for(int i=0; i < table->vreg[rs].rows; i++)
			{
				table->vreg[rs].data[i] = table->vreg[rs].data[i*table->vreg[rs].cols];
			}

			table->vreg[rs].type = 1;
			table->vreg[rs].cols = 1;
		}
		else
		{
			//remove all but the first reg of columns
			int horizontalLay = ceil((float)table->vreg[rs].cols/(float)SPAR_dimension); //how many regs are used for copying vector
			int newPlacements[Max_PrForVr] = {-1};
			int count = 0;
			for(int i=0; i<Max_PrForVr; i+=horizontalLay)
			{
				newPlacements[i/horizontalLay] = table->vreg[rs].placement[i];
				count++;
			}
			for(int i=0; i<Max_PrForVr && i<count; i++)
			{
				table->vreg[rs].placement[i] = newPlacements[i];
				if(i+1 < Max_PrForVr) table->vreg[rs].placement[i+1] = -1;
			}
			table->vreg[rs].type = 1;
			table->vreg[rs].cols = 1;
		}
	}
	else //turned orientation
	{
		if(table->vreg[rs].status == 0)
		{
			table->vreg[rs].rows = table->vreg[rs].cols;
			table->vreg[rs].type = 1;
			table->vreg[rs].cols = 1;
		}
		else
		{
			//remove all but the first reg of rows
			int horizontalLay = ceil((float)table->vreg[rs].cols/(float)SPAR_dimension); //how many regs are used for copying vector
			int newPlacements[Max_PrForVr] = {-1};
			int count = 0;
			for(int i=0; i<horizontalLay && i<Max_PrForVr; i+=1)//todo: replace 6 with macro or variable. and make it more efficient
			{
				newPlacements[i] = table->vreg[rs].placement[i];
				count++;
			}
			for(int i=0; i<Max_PrForVr && i<count; i++)
			{
				table->vreg[rs].placement[i] = newPlacements[i];
				if(i+1 < Max_PrForVr) table->vreg[rs].placement[i+1] = -1;
			}
			table->vreg[rs].rows = table->vreg[rs].cols;
			table->vreg[rs].type = 1;
			table->vreg[rs].cols = 1;
		}
	}

	//determine new orientation
	if(table->vreg[rs].orientation==0 && table->vreg[rs].status==1)
	{
		if(orientation==0){table->vreg[rs].orientation=0;}
		else {table->vreg[rs].orientation=1;}
	}
	else if (table->vreg[rs].status==1)
	{
		if(orientation==0){table->vreg[rs].orientation=1;}
		else {table->vreg[rs].orientation=0;}
	}

	//remove all other non-used pregs
	int negOneHit = 0;
	for(int i=0; i<Max_PrForVr; i++)
	{
		if(negOneHit == 0)
		{
			if(table->vreg[rs].placement[i]==-1)
			{
				negOneHit++;
			}
		}
		else
		{
			table->vreg[rs].placement[i]=-1;
		}
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
	if(rd==0) {return;} //canno write to reg 0
	if(rd < 0 || rd > Num_VREG) {printf("Error: Invalid VREG Number! Data will not be stored!\n"); return;}
	removeVRegFromPRegsNoData(rd, table); //remove the current vreg from all pregs
	int colFol = ceil((float)m->cols/(float)SPAR_dimension);
	int rowFol = ceil((float)m->rows/(float)SPAR_dimension);

	int copies = colFol * rowFol;
	if (copies > Max_PrForVr) //todo-->need to actually implement this if statement properly
	{
		printf("not enough registers to load array");
		return; //exits the function if there are not enough registers
	}
	allocateVRegM(m, rd, 0, table);
}

void Store_M_Transpose(Matrix *m, int rd, AllocationTable *table) { //not finished. Not necessary as of right now
	if(rd==0) {return;} //canno write to reg 0
	//do not needs to change "fold" calculations since the dimensions are square
	removeVRegFromPRegs(rd, table);
	int colFol = ceil((float)m->cols/(float)SPAR_dimension);
	int rowFol = ceil((float)m->rows/(float)SPAR_dimension);

	int copies = colFol * rowFol;
	if (copies*minimumNumOfVReg > numberofreg)
	{
		printf("not enough registers to load array\n");
		return; //exits the function if there are not enough registers
	}

	allocateVRegM_T(m, rd, 0, table);
	//todo: remove previous VReg data from PRegs
}

void Load_M(Matrix *m, int rs, AllocationTable *table){
	if(rs==0) {
		int rows = table->vreg[rs].rows;
		int cols = table->vreg[rs].cols;
		free(m->memory);
		m->memory = (int*)malloc(rows*cols*sizeof(int));
		for(int i=0; i<rows*cols; i++)
		{
			m->memory[i] = 0;
		}
		return;
	} //canno write to reg 0

	if(table->vreg[rs].status==1 ||table->vreg[rs].status==2)
	{
		copyFromPRegsToVRegData(rs, table);
	}

	int rows = table->vreg[rs].rows;
	int cols = table->vreg[rs].cols;
	free(m->memory);
	m->memory = (int*)malloc(rows*cols*sizeof(int));
	for(int i=0; i<rows; i++)
	{
		for(int j=0; j<cols; j++)
		m->memory[i*cols + j] = table->vreg[rs].data[i*cols + j];
	}
}

void Store_V(Vector* v, int rd, AllocationTable *table){
	if(rd==0) {return;} //cannot write to reg 0

	//todo: check if size is too big or not
	removeVRegFromPRegsNoData(rd, table);
	allocateVRegV(v, rd, table);
}

void Load_V(Vector* v, int rs, AllocationTable *table){
	if(rs==0) {
		int numElements = table->vreg[rs].rows;
		free(v->memory);
		v->memory = (int*)malloc(numElements*sizeof(int));
		for(int i=0; i<numElements; i++)
		{
			v->memory[i] = 0;
		}
		return;
	} //canno write to reg 0
	if(table->vreg[rs].status==1 ||table->vreg[rs].status==2)
	{
		copyFromPRegsToVRegData(rs, table);
	}
	int numElements = table->vreg[rs].rows;
	free(v->memory);
	v->memory = (int*)malloc(numElements*sizeof(int));
	for(int i=0; i<numElements; i++)
	{
		v->memory[i] = table->vreg[rs].data[i];
	}
}

///////////////////-----------------Math Operations-------------------------------//////////////////////////
void E_Add_MM(int rs1, int rs2, int rd, AllocationTable *table){
	//set result up //todo: move result setup to prepare
//	table->vreg[rd].cols = table->vreg[rs1].cols;
//	table->vreg[rd].rows = table->vreg[rs1].rows;
//	table->vreg[rd].orientation = table->vreg[rs1].orientation;
//	table->vreg[rd].type = table->vreg[rs1].type;
	PrepareReg_E_MMM(rs1, rs2, rd, table);
	for(int i = 0; i < Max_PrForVr; i++){//todo: replace 6 with a variable or macro
		if(table->vreg[rd].placement[i]>=0 && table->vreg[rs1].placement[i]>=0 && table->vreg[rs2].placement[i]>=0) execute(0, table->vreg[rd].placement[i], table->vreg[rs1].placement[i], table->vreg[rs2].placement[i]);
		else break; //break out of loop if one of the placements is -1
	}
}

void E_Sub_MM(int rs1, int rs2, int rd, AllocationTable *table)
{
	//set result up
	PrepareReg_E_MMM(rs1, rs2, rd, table);
	for(int i = 0; i < Max_PrForVr; i++){//todo: replace 6 with a variable or macro
		if(table->vreg[rd].placement[i]>=0 && table->vreg[rs1].placement[i]>=0 && table->vreg[rs2].placement[i]>=0) execute(1, table->vreg[rd].placement[i], table->vreg[rs1].placement[i], table->vreg[rs2].placement[i]);
		else break; //break out of loop if one of the placements is -1
	}
}

void E_Mul_MM(int rs1, int rs2, int rd, AllocationTable *table)
{
	//set result up
	PrepareReg_E_MMM(rs1, rs2, rd, table);
	//todo: make the placements execute in order of lowest number PE to highest in order to avoid messing up other rd pregs.
	int placementCopy[Max_PrForVr];
	int indices[Max_PrForVr];
	for(int i = 0; i<Max_PrForVr; i++)
	{
		placementCopy[i]=table->vreg[rd].placement[i];
		indices[i]=i;
	}

	quicksortModified(placementCopy, 0, 5, indices);

	for(int j=0; j<Max_PrForVr; j++)
	{
		int i = indices[j];
		if(table->vreg[rd].placement[i]>=0 && table->vreg[rs1].placement[i]>=0 && table->vreg[rs2].placement[i]>=0)
		{
			if(table->preg[table->vreg[rd].placement[i]+1]!=rd && table->preg[table->vreg[rd].placement[i]+1]>=0 && table->vreg[rd].placement[i]<Num_PREG-1) //account for register above the destination register being changed. Does not matter if the register is the last
			{
				if(table->preg[table->vreg[rd].placement[i]+1] == rs1) { //safely move a source register preg
					int reserved[Max_PrForVr*2]; //todo: replace reserved size to a variable or macro
					int reservedCount = 0;
					for(int plac = 0; plac < Max_PrForVr; plac++)
					{
						if(table->vreg[rd].placement[plac] >=0) {reserved[reservedCount] = table->vreg[rd].placement[plac]; reservedCount++;}
						if(table->vreg[rs2].placement[plac] >=0) {reserved[reservedCount] = table->vreg[rs2].placement[plac]; reservedCount++;}
					}
					SafelyMoveToAnotherPREG(table->vreg[rd].placement[i]+1, reserved, reservedCount, table);
				}
				else if(table->preg[table->vreg[rd].placement[i]+1] == rs2) { //safely move a source register preg
					int reserved[Max_PrForVr*2]; //todo: replace reserved size to a variable or macro
					int reservedCount = 0;
					for(int plac = 0; plac < Max_PrForVr; plac++)
					{
						if(table->vreg[rd].placement[plac] >=0) {reserved[reservedCount] = table->vreg[rd].placement[plac]; reservedCount++;}
						if(table->vreg[rs1].placement[plac] >=0) {reserved[reservedCount] = table->vreg[rs1].placement[plac]; reservedCount++;}
					}
					SafelyMoveToAnotherPREG(table->vreg[rd].placement[i]+1, reserved, reservedCount, table);
				}
				else {
					removeVRegFromPRegs(table->preg[table->vreg[rd].placement[i]+1], table);
				}
			}
			execute(2, table->vreg[rd].placement[i], table->vreg[rs1].placement[i], table->vreg[rs2].placement[i]);
		}
	}
}

void AccumulateColumns_M(int rs, int rd, int tempReg, AllocationTable *table) //todo: can be GREATLY optimized similarly to Atiyeh's design
{
	//clear tempReg and destination register. This also sets the tempReg and rd dimensions
	E_Sub_MM(rs, rs, tempReg, table);
	E_Sub_MM(rs, rs, rd, table);

	if(table->vreg[rs].orientation == 0)
	{
		//move data initially into tempReg
		ShiftWest_M(rs, tempReg, table);
		//add
		E_Add_MM(tempReg, rs, rd, table);
		for(int i = 0; i < (table->vreg[rs].cols)-2; i++)
		{
			ShiftWest_M(rd, tempReg, table);
			E_Add_MM(tempReg, rs, rd, table);
		}
	}
	else
	{
		ShiftNorth_M(rs, tempReg, table);
		E_Add_MM(tempReg, rs, rd, table);
		for(int i = 0; i < (table->vreg[rs].cols)-2; i++)
		{
			ShiftNorth_M(rd, tempReg, table);
			E_Add_MM(tempReg, rs, rd, table);
		}
	}
}

void E_Add_VV(int rs1, int rs2, int rd, AllocationTable *table){
	//todo: remove vreg copy data from pregs (resize vector and account for it)
//	table->vreg[rd].cols = 1;
//	table->vreg[rd].rows = table->vreg[rs1].rows;
//	table->vreg[rd].orientation = table->vreg[rs1].orientation;
//	table->vreg[rd].type = 1;
	PrepareReg_E_VVV(rs1, rs2, rd, table);
	for(int i=0; i<Max_PrForVr; i++)
	{
		if(table->vreg[rd].placement[i]>=0 && table->vreg[rs1].placement[i]>=0 && table->vreg[rs2].placement[i]>=0) execute(0, table->vreg[rd].placement[i], table->vreg[rs1].placement[i], table->vreg[rs2].placement[i]);
		else break;
	}
}

void E_Sub_VV(int rs1, int rs2, int rd, AllocationTable *table){
	//todo: remove vreg copy data from pregs (resize vector and account for it)
//	table->vreg[rd].cols = 1;
//	table->vreg[rd].rows = table->vreg[rs1].rows;
//	table->vreg[rd].orientation = table->vreg[rs1].orientation;
//	table->vreg[rd].type = 1;
	PrepareReg_E_MMM(rs1, rs2, rd, table);
	for(int i=0; i<Max_PrForVr; i++)
	{
		if(table->vreg[rd].placement[i]>=0 && table->vreg[rs1].placement[i]>=0 && table->vreg[rs2].placement[i]>=0) execute(1, table->vreg[rd].placement[i], table->vreg[rs1].placement[i], table->vreg[rs2].placement[i]);
		else break;
	}
}

void E_Mul_VV(int rs1, int rs2, int rd, AllocationTable *table){
	table->vreg[rd].cols = 1; // number of copies for vector
	table->vreg[rd].rows = table->vreg[rs1].rows;
	table->vreg[rd].orientation = table->vreg[rs1].orientation;
	table->vreg[rd].type = 1;
	//todo: remove vreg copy data from pregs (resize vector and account for it)
	//todo: account if registers need to be reoriented.
	PrepareReg_E_MMM(rs1, rs2, rd, table);
//	printTablePReg(table);

	int placementCopy[Max_PrForVr];
	int indices[Max_PrForVr];
	for(int i = 0; i<Max_PrForVr; i++)
	{
		placementCopy[i]=table->vreg[rd].placement[i];
		indices[i]=i;
	}

	quicksortModified(placementCopy, 0, 5, indices);

	for(int j=0; j<Max_PrForVr; j++)
	{
		int i = indices[j];
		if(table->vreg[rd].placement[i]>=0 && table->vreg[rs1].placement[i]>=0 && table->vreg[rs2].placement[i]>=0)
		{
//			printTablePReg(table);
			if(table->preg[table->vreg[rd].placement[i]+1]!=rd && table->preg[table->vreg[rd].placement[i]+1]>=0 && table->vreg[rd].placement[i]<Num_PREG-1) //account for register above the destination register being changed. Does not matter if the register is the last
			{
				if(table->preg[table->vreg[rd].placement[i]+1] == rs1) { //safely move a source register preg
					int reserved[Max_PrForVr*2]; //todo: replace reserved size to a variable or macro
					int reservedCount = 0;
					for(int plac = 0; plac < Max_PrForVr; plac++)
					{
						if(table->vreg[rd].placement[plac] >=0) {reserved[reservedCount] = table->vreg[rd].placement[plac]; reservedCount++;}
						if(table->vreg[rs2].placement[plac] >=0) {reserved[reservedCount] = table->vreg[rs2].placement[plac]; reservedCount++;}
					}
					SafelyMoveToAnotherPREG(table->vreg[rd].placement[i]+1, reserved, reservedCount, table);
				}
				else if(table->preg[table->vreg[rd].placement[i]+1] == rs2) { //safely move a source register preg
					int reserved[Max_PrForVr*2]; //todo: replace reserved size to a variable or macro
					int reservedCount = 0;
					for(int plac = 0; plac < Max_PrForVr; plac++)
					{
						if(table->vreg[rd].placement[plac] >=0) {reserved[reservedCount] = table->vreg[rd].placement[plac]; reservedCount++;}
						if(table->vreg[rs1].placement[plac] >=0) {reserved[reservedCount] = table->vreg[rs1].placement[plac]; reservedCount++;}
					}
					SafelyMoveToAnotherPREG(table->vreg[rd].placement[i]+1, reserved, reservedCount, table);
				}
				else {
					removeVRegFromPRegs(table->preg[table->vreg[rd].placement[i]+1], table);
				}
			}
			execute(2, table->vreg[rd].placement[i], table->vreg[rs1].placement[i], table->vreg[rs2].placement[i]);
			usleep_A53(500);
		}
	}
}

void Mul_MV(int rs_m, int rs_v, int rd, AllocationTable *table){

	int temp = 0;
	PrepareReg_Mul_MVM(rs_m, rs_v, temp, table);
	//organize the order of multiplication
	int placementCopy[Max_PrForVr]; //todo: replace 6 with macro or variable
	int indices[Max_PrForVr];
	int rs_v_preg_count = 0;
	for(int i = 0; i<Max_PrForVr; i++)
	{
		placementCopy[i]=table->vreg[temp].placement[i];
		indices[i]=i;
		if(table->vreg[rs_v].placement[i] >=0 && table->vreg[rs_v].placement[i]< Num_PREG) {rs_v_preg_count++;}
	}
	// printTablePReg(table);

	quicksortModified(placementCopy, 0, 5, indices);
	for(int j=0; j<Max_PrForVr; j++)
	{
		int i = indices[j];
		if(table->vreg[temp].placement[i]>=0 && table->vreg[rs_m].placement[i]>=0 && (table->vreg[rs_v].placement[i%rs_v_preg_count])>=0)
		{
//			printTablePReg(table);
			if(table->preg[table->vreg[temp].placement[i]+1]!=temp && table->preg[table->vreg[temp].placement[i]+1]>=0 && table->vreg[temp].placement[i]<Num_PREG-1) //account for register above the destination register being changed. Does not matter if the register is the last
			{
				if(table->preg[table->vreg[temp].placement[i]+1] == rs_m) { //safely move a source register preg
					int reserved[Max_PrForVr*2]; //todo: replace reserved size to a variable or macro
					int reservedCount = 0;
					for(int plac = 0; plac < Max_PrForVr; plac++)
					{
						if(table->vreg[temp].placement[plac] >=0) {reserved[reservedCount] = table->vreg[temp].placement[plac]; reservedCount++;}
						if(table->vreg[rs_v].placement[plac] >=0) {reserved[reservedCount] = table->vreg[rs_v].placement[plac]; reservedCount++;}
					}
					SafelyMoveToAnotherPREG(table->vreg[temp].placement[i]+1, reserved, reservedCount, table);
				}
				else if(table->preg[table->vreg[temp].placement[i]+1] == rs_v) { //safely move a source register preg
					int reserved[Max_PrForVr*2]; //todo: replace reserved size to a variable or macro
					int reservedCount = 0;
					for(int plac = 0; plac < Max_PrForVr; plac++)
					{
						if(table->vreg[temp].placement[plac] >=0) {reserved[reservedCount] = table->vreg[temp].placement[plac]; reservedCount++;}
						if(table->vreg[rs_m].placement[plac] >=0) {reserved[reservedCount] = table->vreg[rs_m].placement[plac]; reservedCount++;}
					}
					SafelyMoveToAnotherPREG(table->vreg[temp].placement[i]+1, reserved, reservedCount, table);
				}
				else {
					removeVRegFromPRegs(table->preg[table->vreg[temp].placement[i]+1], table);
				}
			}
			execute(2, table->vreg[temp].placement[i], table->vreg[rs_m].placement[i], (table->vreg[rs_v].placement[i%rs_v_preg_count]));
//			printf("Multiplying %d and %d to %d\n", table->vreg[rs_m].placement[i], table->vreg[rs_v].placement[i%rs_v_preg_count], table->vreg[temp].placement[i]);
		}
//		printf("\n J:%d\n", j);
//		PrintCounts_v();
	}
//	printTableVReg(table);
//	printPReg(table->vreg[1].placement[0]);
//	printPReg(table->vreg[2].placement[0]);
//	printPReg(table->vreg[0].placement[0]);
//	printVReg(rs_m, table);
//	printVReg(rs_v, table);
//	printVReg(temp, table);
	int old_vdata[SPAR_dimension*SPAR_dimension*8]; //todo: replace with macro or variable
	int old_vrows = table->vreg[rs_v].rows;
//	printVReg(rs_v, table);
	usleep(1); //idk why, but just this is enough to make it work. REMOVE WITH CAUTION

	removeVRegFromPRegs(rs_v, table);

	//copy vector data
	for(int it=0; it<table->vreg[rs_v].rows; it++)
	{
		old_vdata[it] = table->vreg[rs_v].data[it];
	}


	//accumulate the result matrix
	AccumulateColumns_M(temp, rd, rs_v, table);

	//cast the accumulation to a vector //(int rs, int orientation, AllocationTable *table)
	CastRegTo_V(rd, 0, table); //new orientation should be 1
//	printVReginPReg(rd, table);

	//restore vector data
	removeVRegFromPRegsNoData(rs_v, table);
	table->vreg[rs_v].rows = old_vrows;
	table->vreg[rs_v].cols = 1;
	table->vreg[rs_v].orientation = 0;
	table->vreg[rs_v].status = 0;
	table->vreg[rs_v].type = 1;
	for(int it=0; it<old_vrows; it++)
	{
		table->vreg[rs_v].data[it] = old_vdata[it];
	}
//	printVReg(rs_v, table);
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
