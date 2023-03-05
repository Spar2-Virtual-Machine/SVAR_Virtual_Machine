/*
 * Test.c
 *
 *  Created on: Feb 19, 2023
 *      Author: Njfre
 */
#include "Test.h"

void MultiplyMV(int *matrix, int *vector, int rows, int cols, int *result) //todo: not working correctly
{
	int temp = 0;
	for(int x=0; x<rows; x++)
	{
		temp = 0;
		for(int y=0; y<cols; y++)
		{
			float mf = ((float)matrix[(x*cols) + y])/((float)(2<<16));
			float vf = ((float)vector[y])/((float)(2<<16));
			float tempf = mf*vf*(float)(2<<16);
			temp += (int) tempf;
//			temp += ((matrix[(x*cols) + y]) * vector[y]);
		}
		result[x] = temp;
	}
	return;
}

int TestShiftNorth(AllocationTable *table)
{
	for(int x=24; x<29024; x+=1000)
	{
		int row = x%1000;
		int col = x/1000;
		int rowA = (row/(4*Tile_dim))%Array_dim;
		int colA = (col/(4*Tile_dim))%Array_dim;
		int rowT = (row/4)%Tile_dim;
		int colT = (col/4)%Tile_dim;
		int pe = ((col*4 + row)%4 + row*4)%16;
		printf("x: %d, A: %d, %d, T: %d,%d, pe: %d\n", x, rowA, colA, rowT, colT, pe);
	}
	Reset_Registers();
	resetTable(table);
	srand(12890);
	int rowN = 30; //1+rand()%100;
	int colN = 30; //1+rand()%100;

	int arr1[rowN][colN];
	int arr2[rowN][colN];
	xil_printf("%p\n", table);
	for(int i=0; i < rowN; i++)
	{
		for(int j=0; j < colN; j++)
		{
			arr1[i][j] = j+1000*i;
			arr2[i][j] = (1)<<16;
		}
	}
	xil_printf("%d Rows and %d Columns \n", rowN, colN);

	//setup the matrix
	Matrix matrix1;
	Declare_M(&matrix1, rowN, colN);
	matrix1.memory = (int*)&arr1;
	Matrix matrix2;
	Declare_M(&matrix2, rowN, colN);
	matrix2.memory = (int*)&arr1;
	Store_M(&matrix1, 1, table);
	Store_M(&matrix2, 2, table);
	table->vreg[1].orientation=1;
	safeAllocatePRegs(1, 24, NULL, 0, table);
	safeAllocatePRegs(2, 24, NULL, 0, table);


	ShiftNorth_M(1, 3, table);
	ShiftNorth_M(2, 4, table);
	printTablePReg(table);
	printVReg(1, table);
	printVReg(3, table);
	printVReg(4, table);
	int s=23;
	int t=0;
	printf("array %d, %d; tile %d, %d; pe %d\n", (s/(4*Tile_dim))%Array_dim, (t/(4*Tile_dim))%Array_dim, (s/4)%Tile_dim, (t/4)%Tile_dim, ((s*4 +t)%4 + s*4)%16);
	return 0; //fail by default
}

int TestShiftSouth(AllocationTable *table)
{
	Reset_Registers();
	resetTable(table);
	srand(12890);
	int rowN = 30; //1+rand()%100;
	int colN = 30; //1+rand()%100;

	int arr1[rowN][colN];
	int arr2[rowN][colN];
	xil_printf("%p\n", table);
	for(int i=0; i < rowN; i++)
	{
		for(int j=0; j < colN; j++)
		{
			arr1[i][j] = j+1000*i;
			arr2[i][j] = (1)<<16;
		}
	}
	xil_printf("%d Rows and %d Columns \n", rowN, colN);

	//setup the matrix
	Matrix matrix1;
	Declare_M(&matrix1, rowN, colN);
	matrix1.memory = (int*)&arr1;
	Matrix matrix2;
	Declare_M(&matrix2, rowN, colN);
	matrix2.memory = (int*)&arr1;
	Store_M(&matrix1, 1, table);
	Store_M(&matrix2, 2, table);
	table->vreg[1].orientation=1;
	safeAllocatePRegs(1, 24, NULL, 0, table);
	safeAllocatePRegs(2, 24, NULL, 0, table);


	ShiftSouth_M(1, 3, table);
	ShiftSouth_M(2, 4, table);
	printTablePReg(table);
	printVReg(3, table);
	printVReg(4, table);
	int s=23;
	int t=0;
	printf("array %d, %d; tile %d, %d; pe %d\n", (s/(4*Tile_dim))%Array_dim, (t/(4*Tile_dim))%Array_dim, (s/4)%Tile_dim, (t/4)%Tile_dim, ((s*4 +t)%4 + s*4)%16);
	return 0; //fail by default
}


int TestTurnedColumnAccumulation(AllocationTable *table)
{
	Reset_Registers();
	resetTable(table);
	srand(12890);
	int rowN = 4; //1+rand()%100;
	int colN = 49; //1+rand()%100;

	int arr1[rowN][colN];
	int arr2[rowN][colN];
	xil_printf("%p\n", table);
	for(int i=0; i < rowN; i++)
	{
		for(int j=0; j < colN; j++)
		{
			arr1[i][j] = j+256;
			arr2[i][j] = (1)<<16;
		}
	}
	xil_printf("%d Rows and %d Columns \n", rowN, colN);

	//setup the matrix
	Matrix matrix1;
	Declare_M(&matrix1, rowN, colN);
	matrix1.memory = (int*)&arr1;
	Matrix matrix2;
	Declare_M(&matrix2, rowN, colN);
	matrix2.memory = (int*)&arr2;
	Store_M(&matrix1, 1, table);
	Store_M(&matrix2, 2, table);
	table->vreg[1].orientation = 1;
	table->vreg[2].orientation = 1;
	safeAllocatePRegs(1, 24, NULL, 0, table);
	safeAllocatePRegs(2, 24, NULL, 0, table);


//	AccumulateColumns_M(1, 3, 0, table);
	AccumulateColumns_M(2, 4, 0, table);

//	printVReg(1, table);

//	printVReg(3, table);
	printVReg(2, table);
	printVReg(4, table);

	return 0; //fail by default
}

int TestFillVector(AllocationTable *table)
{
	srand(12890);
	int rowN = 1+rand()%100;
	int colN = 1+rand()%100;

	int arr1[rowN][colN];
	int arr2[rowN][colN];
	xil_printf("%p\n", table);
	for(int i=0; i < rowN; i++)
	{
		for(int j=0; j < colN; j++)
		{
			arr1[i][j] = j+256;
			arr2[i][j] = (1)<<16;
		}
	}
	xil_printf("%d Rows and %d Columns \n", rowN, colN);

	//setup the matrix
	Vector v1, v2;
	Declare_V(&v1, rowN);
	v1.memory=(int*)&arr1;
	Declare_V(&v2, rowN);
	v2.memory=(int*)&arr2;

	Store_V(&v1, 1, table);
	Store_V(&v2, 2, table);
	table->vreg[1].orientation = 0;
	table->vreg[2].orientation = 0;
	safeAllocatePRegs(1, 24, NULL, 0, table);
	safeAllocatePRegs(2, 24, NULL, 0, table);

	E_Add_VV(1,2,4, table);

	//todo: add in some random noise after the first column


	FillVector(4, 0, table);
	printVReginPReg(4, table);

	copyFromPRegsToVRegData(1, table);
	printVRegData(1, table);
	copyFromPRegsToVRegData(2, table);
	printVRegData(2, table);

	printTableVReg(table);
	printTablePReg(table);


	copyFromPRegsToVRegData(4, table);
	printVRegData(4, table);
	printRegFile(0,0,0,0,9);
	return 0; //fail by default
}

int TestFillVectorTurned(AllocationTable *table)
{
	srand(12890);
	int rowN = 1+rand()%100;
	int colN = 1+rand()%100;

	int arr1[rowN][colN];
	int arr2[rowN][colN];
	xil_printf("%p\n", table);
	for(int i=0; i < rowN; i++)
	{
		for(int j=0; j < colN; j++)
		{
			arr1[i][j] = j+256;
			arr2[i][j] = (1)<<16;
		}
	}
	xil_printf("%d Rows and %d Columns \n", rowN, colN);

	//setup the matrix
	Vector v1, v2;
	Declare_V(&v1, rowN);
	v1.memory=(int*)&arr1;
	Declare_V(&v2, rowN);
	v2.memory=(int*)&arr2;

	Store_V(&v1, 1, table);
	Store_V(&v2, 2, table);
	table->vreg[1].orientation = 1;
	table->vreg[2].orientation = 1;
	safeAllocatePRegs(1, 24, NULL, 0, table);
	safeAllocatePRegs(2, 24, NULL, 0, table);

	E_Add_VV(1,2,4, table);

	FillVector(4, 0, table);
	printVReginPReg(4, table);

	copyFromPRegsToVRegData(1, table);
	printVRegData(1, table);
	copyFromPRegsToVRegData(2, table);
	printVRegData(2, table);

	printTableVReg(table);
	printTablePReg(table);


	copyFromPRegsToVRegData(4, table);
	printVRegData(4, table);
	printRegFile(0,0,0,0,9);
	return 0; //fail by default
}

int TestFillVectorMixedOrientation(AllocationTable *table)
{
	srand(1283420);
	int rowN = 1+rand()%100;
	int colN = 1+rand()%100;

	int arr1[rowN][colN];
	int arr2[rowN][colN];
	xil_printf("%p\n", table);
	for(int i=0; i < rowN; i++)
	{
		for(int j=0; j < colN; j++)
		{
			arr1[i][j] = j+256+rand()%(2<<30);
			arr2[i][j] = ((1)<<16)+rand()%(2<<30);
		}
	}
	xil_printf("%d Rows and %d Columns \n", rowN, colN);

	//setup the matrix
	Vector v1, v2;
	Declare_V(&v1, rowN);
	v1.memory=(int*)&arr1;
	Declare_V(&v2, rowN);
	v2.memory=(int*)&arr2;

	Store_V(&v1, 1, table);
	Store_V(&v2, 2, table);
	table->vreg[1].orientation = 0;
	table->vreg[2].orientation = 1;
	safeAllocatePRegs(1, 24, NULL, 0, table);
	safeAllocatePRegs(2, 24, NULL, 0, table);

	E_Add_VV(1,2,4, table);

	FillVector(4, 0, table);
	printVReginPReg(4, table);

	copyFromPRegsToVRegData(1, table);
	printVRegData(1, table);
	copyFromPRegsToVRegData(2, table);
	printVRegData(2, table);

	printTableVReg(table);
	printTablePReg(table);

	for(int reg = 0; reg < 6; reg++)
	{
		for(int row = 0; row < rowN; row++)
		{

		}
	}


	copyFromPRegsToVRegData(4, table);
	printVRegData(4, table);
	printRegFile(0,0,0,0,9);
	return 0; //fail by default
}

int ConvertMatrixToVectorTest(AllocationTable *table){
	Reset_Registers();
	xil_printf("RESET REGISTERS IS DONE! \n");

	//setup allocation table
	resetTable(table);

	printf("-------------------------------------------------------------------------\r\n");
	srand(12890);
	int rowN = 30;
	int colN = 40;

	int arr1[rowN][colN];
//	int arr2[rowN][colN];

	for(int i=0; i < rowN; i++)
	{
		for(int j=0; j < colN; j++)
		{
			arr1[i][j] = j+256+(4*i);
//			arr2[i][j] = (2)<<16;
		}
	}

	Matrix matrix1; //test for both orientations and both statuses.
	Declare_M(&matrix1, rowN, colN);
	matrix1.memory=(int*)&arr1;

	Store_M(&matrix1, 1, table);
	Store_M(&matrix1, 2, table);
	Store_M(&matrix1, 6, table);
	Store_M(&matrix1, 7, table);
//	printVRegData(1, table);
	safeAllocatePRegs(1, 24, NULL, 0, table);
	safeAllocatePRegs(2, 24, NULL, 0, table);
	printTableVReg(table);
	CastRegTo_V(1, 0, table);
	CastRegTo_V(2, 1, table);
	CastRegTo_V(6, 0, table);
	CastRegTo_V(7, 1, table);

	printf("\nResults---------------------------------\n");
	copyFromPRegsToVRegData(1, table);
	printVRegData(1, table);
	printf("\n");
	copyFromPRegsToVRegData(2, table);
	printVRegData(2, table);
	printf("\n");
	copyFromPRegsToVRegData(6, table);
	printVRegData(6, table);
	printf("\n");
	copyFromPRegsToVRegData(7, table);
	printVRegData(7, table);
	printf("\n");
	printf("end\n");
	return 0;
}

int ConvertMatrixToVectorTurnedTest(AllocationTable *table){
	Reset_Registers();
		xil_printf("RESET REGISTERS IS DONE! \n");

		//setup allocation table
		resetTable(table);

		printf("-------------------------------------------------------------------------\r\n");
		srand(12890);
		int rowN = 30;
		int colN = 40;

		int arr1[rowN][colN];
	//	int arr2[rowN][colN];

		for(int i=0; i < rowN; i++)
		{
			for(int j=0; j < colN; j++)
			{
				arr1[i][j] = j+256+(4*i);
	//			arr2[i][j] = (2)<<16;
			}
		}

		Matrix matrix1; //test for both orientations and both statuses.
		Declare_M(&matrix1, rowN, colN);
		matrix1.memory=(int*)&arr1;

		Store_M(&matrix1, 1, table);
		Store_M(&matrix1, 2, table);
		Store_M(&matrix1, 6, table);
		Store_M(&matrix1, 7, table);
		table->vreg[1].orientation=1;
		table->vreg[2].orientation=1;
		table->vreg[6].orientation=1;
		table->vreg[7].orientation=1;
		safeAllocatePRegs(1, 24, NULL, 0, table);
		safeAllocatePRegs(2, 24, NULL, 0, table);
		CastRegTo_V(1, 0, table);
		CastRegTo_V(2, 1, table);
		CastRegTo_V(6, 0, table);
		CastRegTo_V(7, 1, table);

		printf("\nResults---------------------------------------------------------------------------\n");
		copyFromPRegsToVRegData(1, table);
		printVRegData(1, table);
		printf("\n");
		copyFromPRegsToVRegData(2, table);
		printVRegData(2, table);
		printf("\n");
		copyFromPRegsToVRegData(6, table);
		printVRegData(6, table);
		printf("\n");
		copyFromPRegsToVRegData(7, table);
		printVRegData(7, table);
		printf("\n");
		printf("end\n");
		return 0;
}

int MultiplyAccumulateMatrixVectorSetupTest(AllocationTable *table)
{
	int valid = 1;
	for(int i=0; i<32; i++) //idk why i chose 20
	{
		resetTable(table);

		printf("-------------------------------------------------------------------------\r\n");
		int rowN = 30;
		int colN = 40;
		int arr1[rowN][colN];
		for(int i=0; i < rowN; i++)
		{
			for(int j=0; j < colN; j++)
			{
				arr1[i][j] = j+256+(4*i);
	//			arr2[i][j] = (2)<<16;
			}
		}
		//declare and setup matrix and vector
		Matrix matrix1;
		Declare_M(&matrix1, rowN, colN);
		free(matrix1.memory);
		matrix1.memory=(int*)&arr1;
		Vector vector1;
		Declare_V(&vector1, colN); //needs to be column length
		vector1.memory=(int*)&arr1;

		Store_M(&matrix1, 1, table);
		if(i%2==1){table->vreg[1].orientation=1;}
		Store_V(&vector1, 2, table);
		if(i%4==3){table->vreg[2].orientation=1;}
//		PrepareReg_Mul_MVM(1, 2, 3, table);//prepares registers for //todo: account for change

		//randomly store ahead of time
		if(i%8==7)safeAllocatePRegs(1, 24, NULL, 0, table);
		if(i%16==15)safeAllocatePRegs(2, 24, NULL, 0, table);
		//check orientations
		if(table->vreg[1].orientation == table->vreg[2].orientation)
		{
			printf("Error: source registers incorrectly oriented\n");
			valid = 0;
		}

		if(table->vreg[1].orientation != table->vreg[3].orientation)
		{
			printf("Error: destination register incorrectly oriented\n");
			valid = 0;
		}
		if(table->vreg[1].status==0)
		{
			printf("Error: matrix not stored\n");
			valid=0;
		}
		if(table->vreg[2].status==0)
		{
			printf("Error: vector not stored\n");
			valid=0;
		}
		if(table->vreg[3].status==0)
		{
			printf("Error: destination has no pregs allocated\n");
			valid=0;
		}
		if(table->vreg[3].type==1)
		{
			printf("Error: destination is vector and not matrix\n");
			valid=0;
		}
	}

	if(valid==0){printf("not passed\n");}
	else printf("passed\n");
	return valid;
}

int MultiplyAccumulateMatrixVectorTest(AllocationTable *table){
	resetTable(table);

	printf("-------------------------------------------------------------------------\r\n");
	srand(12890);
	int rowN = 20;
	int colN = 80;
	int arr1[rowN][colN];
	int arr2[rowN][colN];
	for(int i=0; i < rowN; i++)
	{
		for(int j=0; j < colN; j++)
		{
			arr1[i][j] = j+256+(4*i);
			arr2[i][j] = (2)<<16;
		}
	}
	int testResult[rowN];
	//declare and setup matrix and vector
	Matrix matrix1;
	Declare_M(&matrix1, rowN, colN);
	free(matrix1.memory);
	matrix1.memory=(int*)&arr1;
	Vector vector1;
	Declare_V(&vector1, colN); //needs to be column length
	vector1.memory=(int*)&arr2;
	Store_M(&matrix1, 1, table);
	Store_V(&vector1, 2, table);

	Mul_MV(1, 2, 3, 0, table);
	printVReg(0, table);
	printf("\n");
	printVReg(1, table);
	printf("\n");
	printVReg(2, table);
	printf("\n");
	printVReg(3, table);

	return 0;
}

