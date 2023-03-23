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
	Reset_Registers();
	resetTable(table);
	srand(12890);
	int rowN = 40; //1+rand()%100;
	int colN = 80; //1+rand()%100;

	int arr1[rowN][colN];
	xil_printf("%p\n", table);
	for(int i=0; i < rowN; i++)
	{
		for(int j=0; j < colN; j++)
		{
			arr1[i][j] = j+100*i;
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
	safeAllocatePRegs(1, NULL, 0, table);
	safeAllocatePRegs(2, NULL, 0, table);


	ShiftNorth_M(1, 3, table);
	ShiftNorth_M(2, 4, table);
	printTablePReg(table);
	printVReg(3, table);
	printVReg(4, table);
	return 0; //fail by default
}

int TestShiftSouth(AllocationTable *table)
{
	Reset_Registers();
	resetTable(table);
	srand(12890);
	int rowN = 80; //1+rand()%100;
	int colN = 20; //1+rand()%100;

	int arr1[rowN][colN];
	xil_printf("%p\n", table);
	for(int i=0; i < rowN; i++)
	{
		for(int j=0; j < colN; j++)
		{
			arr1[i][j] = j+100*i;
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
	safeAllocatePRegs(1, NULL, 0, table);
	safeAllocatePRegs(2, NULL, 0, table);


	ShiftSouth_M(1, 3, table);
	ShiftSouth_M(2, 4, table);
	printTablePReg(table);
	printVReg(3, table);
	printVReg(4, table);
	return 0; //fail by default
}

int TestShiftEast(AllocationTable *table)
{
	Reset_Registers();
	resetTable(table);
	srand(12890);
	int rowN = 40; //1+rand()%100;
	int colN = 80; //1+rand()%100;

	int arr1[rowN][colN];
	xil_printf("%p\n", table);
	for(int i=0; i < rowN; i++)
	{
		for(int j=0; j < colN; j++)
		{
			arr1[i][j] = j+100*i;
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
	safeAllocatePRegs(1, NULL, 0, table);
	safeAllocatePRegs(2, NULL, 0, table);


	ShiftEast_M(1, 3, table);
	ShiftEast_M(2, 4, table);
	printTablePReg(table);
	printVReg(3, table);
	printVReg(4, table);
	return 0; //fail by default
}

int TestShiftWest(AllocationTable *table)
{
	Reset_Registers();
	resetTable(table);
	srand(12890);
	int rowN = 20; //1+rand()%100;
	int colN = 40; //1+rand()%100;

	int arr1[rowN][colN];
//	int arr2[rowN][colN];
	xil_printf("%p\n", table);
	for(int i=0; i < rowN; i++)
	{
		for(int j=0; j < colN; j++)
		{
			arr1[i][j] = j+100*i;
//			arr2[i][j] = (1)<<16;
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
	safeAllocatePRegs(1, NULL, 0, table);
	safeAllocatePRegs(2, NULL, 0, table);


	ShiftWest_M(1, 3, table);
	ShiftWest_M(2, 4, table);
	printTablePReg(table);
	printVReg(3, table);
	printVReg(4, table);
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
	safeAllocatePRegs(1, NULL, 0, table);
	safeAllocatePRegs(2, NULL, 0, table);


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
	resetTable(table);
	srand(12890);
	int rowN = 90;
	int colN = 128;

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
	safeAllocatePRegs(1, NULL, 0, table);
	safeAllocatePRegs(2, NULL, 0, table);
	printVReg(1, table);
	E_Add_VV(1,2,4, table);

	//todo: add in some random noise after the first column


	FillVector(4, 0, table);
//	printVReg(4, table);
	printTableVReg(table);
	printTablePReg(table);

	printVReg(1, table);
//	copyFromPRegsToVRegData(2, table);
	printVReg(2, table);
//

//
	printVReg(4, table);
	printRegFile(0,0,0,0,9);
	return 0; //fail by default
}

int TestFillVectorTurned(AllocationTable *table)
{
	resetTable(table);
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
	safeAllocatePRegs(1, NULL, 0, table);
	safeAllocatePRegs(2, NULL, 0, table);

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
	safeAllocatePRegs(1, NULL, 0, table);
	safeAllocatePRegs(2, NULL, 0, table);

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
	safeAllocatePRegs(1, NULL, 0, table);
	safeAllocatePRegs(2, NULL, 0, table);
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
		safeAllocatePRegs(1, NULL, 0, table);
		safeAllocatePRegs(2, NULL, 0, table);
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

int AccumulateTest(AllocationTable * table)
{
	resetTable(table);
	int rowN = 40; //1+rand()%100;
	int colN = 20; //1+rand()%100;

	int arr1[rowN][colN];
//	int arr2[rowN][colN];
	xil_printf("%p\n", table);
	for(int i=0; i < rowN; i++)
	{
		for(int j=0; j < colN; j++)
		{
			arr1[i][j] = 40;
//			arr2[i][j] = (1)<<16;
		}
	}
	xil_printf("Accumulation turned and normal orientation\n", rowN, colN);

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
	safeAllocatePRegs(1, NULL, 0, table);
	safeAllocatePRegs(2, NULL, 0, table);

	AccumulateColumns_M(1, 3, 0, table);
	AccumulateColumns_M(2, 4, 0, table);
	printVReg(3, table);
	printVReg(4, table);
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
		if(i%8==7)safeAllocatePRegs(1, NULL, 0, table);
		if(i%16==15)safeAllocatePRegs(2, NULL, 0, table);
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
	int rowN = 118;
	int colN = 70;
	int arr1[rowN][colN];
	int arr2[rowN][colN];
	for(int i=0; i < rowN; i++)
	{
		for(int j=0; j < colN; j++)
		{
			arr1[i][j] = 256;//j+256+(4*i);
			arr2[i][j] = (2)<<16;
		}
	}
//	int testResult[rowN];
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

	MoveToAnotherPREG(1, 14, table);
	Mul_MV(1, 2, 3, table);

//	printVReg(3, table); //ha=3
	Vector bias2;
	Declare_V(&bias2, rowN);
	for(int i=0; i<rowN; i++)
	{
		bias2.memory[i] = -28000;//5*(-1*(i%2));
	}
	Store_V(&bias2, 4, table);
	E_Add_VV(3,4,3, table); //ha+b in vreg3

	Matrix weight2;
	Declare_M(&weight2, colN, rowN);
	for(int i=0; i<rowN; i++)
	{
		for (int j=0; j<colN; j++)
		{
			weight2.memory[j*rowN+i] = 1<<16;
		}
	}

	Store_M(&weight2, 4, table);
//	printVReg(4, table);
	Mul_MV(4, 3, 5, table);
//	printVReg(3, table);
//	printVReg(4, table);
	printVReg(5, table);
	return 0;
}

void Test_VReg0(AllocationTable *table){ //todo: not done since I tested it before.
	resetTable(table);

//	printf("-------------------------------------------------------------------------\r\n");
//	srand(12890);
//	int rowN = 48;
//	int colN = 48;
//	int arr1[rowN][colN];
//	int arr2[rowN][colN];
//	for(int i=0; i < rowN; i++)
//	{
//		for(int j=0; j < colN; j++)
//		{
//			arr1[i][j] = j+256+(4*i);
//			arr2[i][j] = (2)<<16;
//		}
//	}
////	int testResult[rowN];
//	//declare and setup matrix and vector
//	Matrix matrix1;
//	Declare_M(&matrix1, rowN, colN);
//	free(matrix1.memory);
//	matrix1.memory=(int*)&arr1;
//	Vector vector1;
//	Declare_V(&vector1, colN); //needs to be column length
//	vector1.memory=(int*)&arr2;
//	Store_M(&matrix1, 0, table);
//
//	Load_M(&matrix1, 0, table);
//	Store_V(&vector1, 0, table);
//


	return;
}

int MLP_Benchmark(AllocationTable *table){
	resetTable(table);
	printf("-------------------------------------------------------------------------\r\n");
	int repetitions = 4;
//	int rowN = 40;
	int srV[repetitions]; //predetermine the placements to reduce time
	int wV[repetitions]; //weight and bias will just share the same work register
	int dV[repetitions];
	int regLoop = 1;

	for(int i=0; i<repetitions; i++)
	{
		if(i>0){
			srV[i] = dV[i-1];
		}
		else{
			srV[i]=regLoop;
		}
		regLoop++;
		regLoop%=Num_VREG;
		if(regLoop==0){regLoop++;}

		wV[i]=regLoop;
		regLoop++;
		regLoop%=Num_VREG;
		if(regLoop==0){regLoop++;}

		dV[i]=regLoop;
		regLoop++;
		regLoop%=Num_VREG;
		if(regLoop==0){regLoop++;}
	}

//	int input[rowN];
//	int weight[rowN][rowN];
//	int bias[rowN];
//	Store_V(&input, srV[0], table);
//	Store_M(&weight, wV[0], table);
	Mul_MV(wV[0], srV[0], dV[0], table);
//	Store_V(&bias, wV[0], table);
	E_Add_VV(wV[0], srV[0], srV[0], table);
	for(int i=1; i<repetitions; i++)
	{

	}
	return 0;
}


extern inline void ResetCounts(){
	sparAdd_cnt=0;
	sparSub_cnt=0;
	sparMul_cnt=0;
	sparShN_cnt=0;
	sparShS_cnt=0;
	sparShE_cnt=0;
	sparShW_cnt=0;
	sparWR_cnt=0;
	sparRD_cnt=0;
}

extern inline void PrintCounts(){
	printf("Add Count: %d \n Sub Count: %d \n Mul Count %d\n", sparAdd_cnt, sparSub_cnt, sparMul_cnt);
	printf("North Count: %d \n South Count: %d \n East Count: %d \n West Count: %d\n", sparShN_cnt, sparShS_cnt, sparShE_cnt, sparShW_cnt);
	printf("Write Count: %d \n Read Count: %d\n", sparWR_cnt, sparRD_cnt);
}
int Test_Elementwise_1Segment(AllocationTable *table){
	printf("\n1 Segment Elementwise\n");
	resetTable(table);
	printf("-------------------------------------------------------------------------\r\n");

	//time variable
	XTime tStart, tEnd;
	double ElapsedTime;

	//define sample data
	int rowN = 64;
	int colN = 64;
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
	Matrix matrix1, matrix2;
	Declare_M(&matrix1, rowN, colN);
	Declare_M(&matrix2, rowN, colN);
	free(matrix1.memory);
	free(matrix2.memory);
	matrix1.memory=(int*)&arr1;
	matrix2.memory=(int*)&arr2;



	//addition
	//void WRITE_Matrix_Large(int row, int col, int block_row, int block_col, int row_blk_size, int col_blk_size, int W[][col], int reg, int copy, int block_dimension){
	WRITE_Matrix_Large(rowN, colN, 0, 0, 64, 64, arr1, 1, 1, 0); //copy=1 for 2d array //goes into preg 1
	WRITE_Matrix_Large(rowN, colN, 0, 0, 64, 64, arr2, 2, 1, 0); //goes into preg 2.

	ResetCounts();
	MATRIX_ADDITION(1,2,3);
	printf("Native Matrix Addition---------------------------------------------------\n");
	PrintCounts();
//	printPReg(3);

	XTime_GetTime(&tStart);
	for(int i=0; i<100; i++)
	{
		MATRIX_ADDITION(1,2,3);
	}
	XTime_GetTime(&tEnd);
	ElapsedTime = (1.0 * (tEnd - tStart) / (COUNTS_PER_SECOND));
	printf("Addition x100 Time: %lf Seconds\n", ElapsedTime);

	printf("VM Matrix Addition-------------------------------------------------------\n");
	resetTable(table);
	Store_M(&matrix1, 1, table);
	Store_M(&matrix2, 2, table);
	safeAllocatePRegs(1, NULL, 0, table);
	int protectedVReg[] = {1};
	safeAllocatePRegs(2, protectedVReg, 1, table);
	ResetCounts();
	E_Add_MM(1,2,3, table);
	PrintCounts();
//	printVReg(3, table);

	XTime_GetTime(&tStart);
	for(int i=0; i<100; i++)
	{
		E_Add_MM(1,2,3, table);
	}
	XTime_GetTime(&tEnd);
	ElapsedTime = (1.0 * (tEnd - tStart) / (COUNTS_PER_SECOND));
	printf("Addition x100 Time: %lf Seconds\n", ElapsedTime);







	//subtraction-------------WRITE_Matrix_Large(0, 0, 0, 0, 64, 64, arr1, 1, 1, 0); //copy=1 for 2d array //goes into preg 1
	printf("/////////////////////////Subtraction---------------------------------------------------\n");
	WRITE_Matrix_Large(0, 0, 0, 0, 64, 64, arr2, 2, 1, 0); //goes into preg 2.

	ResetCounts();
	MATRIX_SUBTRACTION(1,2,3);
	printf("Native Matrix Subtraction---------------------------------------------------\n");
	PrintCounts();
	XTime_GetTime(&tStart);
	for(int i=0; i<100; i++)
	{
		MATRIX_SUBTRACTION(1,2,3);
	}
	XTime_GetTime(&tEnd);
	ElapsedTime = (1.0 * (tEnd - tStart) / (COUNTS_PER_SECOND));
	printf("Subtraction x100 Time: %lf Seconds\n", ElapsedTime);

	printf("VM Matrix Subtraction-------------------------------------------------------\n");
	resetTable(table);
	Store_M(&matrix1, 1, table);
	Store_M(&matrix2, 2, table);
	safeAllocatePRegs(1, NULL, 0, table);
	safeAllocatePRegs(2, protectedVReg, 1, table);
	ResetCounts();
	E_Sub_MM(1,2,3, table);
	PrintCounts();
//	printVReg(3, table);

	XTime_GetTime(&tStart);
	for(int i=0; i<100; i++)
	{
		E_Sub_MM(1,2,3, table);
	}
	XTime_GetTime(&tEnd);
	ElapsedTime = (1.0 * (tEnd - tStart) / (COUNTS_PER_SECOND));
	printf("Subtraction x100 Time: %lf Seconds\n", ElapsedTime);



	//Multiplication_E-------------WRITE_Matrix_Large(0, 0, 0, 0, 64, 64, arr1, 1, 1, 0); //copy=1 for 2d array //goes into preg 1
	printf("/////////////////////////Multiplication_E---------------------------------------------------\n");
	WRITE_Matrix_Large(0, 0, 0, 0, 64, 64, arr2, 2, 1, 0); //goes into preg 2.

	ResetCounts();
	ELEMENTWISE_MULTIPLICATION(1,2,3); //here
	printf("Native Matrix Multiplication---------------------------------------------------\n");
	PrintCounts();
	XTime_GetTime(&tStart);
	for(int i=0; i<100; i++)
	{
		ELEMENTWISE_MULTIPLICATION(1,2,3); //here
	}
	XTime_GetTime(&tEnd);
	ElapsedTime = (1.0 * (tEnd - tStart) / (COUNTS_PER_SECOND));
	printf("E_Multiplication x100 Time: %lf Seconds\n", ElapsedTime); //here

	printf("VM Matrix Multiplication-------------------------------------------------------\n");
	resetTable(table);
	Store_M(&matrix1, 1, table);
	Store_M(&matrix2, 2, table);
	safeAllocatePRegs(1, NULL, 0, table);
	safeAllocatePRegs(2, protectedVReg, 1, table);
	ResetCounts();
	E_Mul_MM(1,2,3, table); //here
	PrintCounts();
//	printVReg(3, table);

	XTime_GetTime(&tStart);
	for(int i=0; i<100; i++)
	{
		E_Mul_MM(1,2,3, table); //here
	}
	XTime_GetTime(&tEnd);
	ElapsedTime = (1.0 * (tEnd - tStart) / (COUNTS_PER_SECOND));
	printf("E_Multiplication x100 Time: %lf Seconds\n", ElapsedTime);
	return 1;
}

int Test_Elementwise_2Segment(AllocationTable *table){
	printf("\n2 Segment Elementwise\n");
	resetTable(table);
	printf("-------------------------------------------------------------------------\r\n");

	//time variable
	XTime tStart, tEnd;
	double ElapsedTime;

	//define sample data
	int rowN = 128;
	int colN = 64;
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
	Matrix matrix1, matrix2;
	Declare_M(&matrix1, rowN, colN);
	Declare_M(&matrix2, rowN, colN);
	free(matrix1.memory);
	free(matrix2.memory);
	matrix1.memory=(int*)&arr1;
	matrix2.memory=(int*)&arr2;
	int protectedVReg[] = {1};



	//addition
	//void WRITE_Matrix_Large(int row, int col, int block_row, int block_col, int row_blk_size, int col_blk_size, int W[][col], int reg, int copy, int block_dimension){
	WRITE_Matrix_Large(rowN, colN, 0, 0, 64, 64, arr1, 1, 1, 0); //copy=1 for 2d array //goes into preg 1
	WRITE_Matrix_Large(rowN, colN, 1, 0, 64, 64, arr1, 2, 1, 0); //copy=1 for 2d array //goes into preg 2

	WRITE_Matrix_Large(rowN, colN, 0, 0, 64, 64, arr2, 3, 1, 0); //goes into preg3
	WRITE_Matrix_Large(rowN, colN, 1, 0, 64, 64, arr2, 4, 1, 0); //goes into preg4

	ResetCounts();
	MATRIX_ADDITION(1,3,5);
	MATRIX_ADDITION(2,4,6);
	printf("Native Matrix Addition---------------------------------------------------\n");
	PrintCounts();


	XTime_GetTime(&tStart);
	for(int i=0; i<100; i++)
	{
		MATRIX_ADDITION(1,3,5);
		MATRIX_ADDITION(2,4,6);
	}
	XTime_GetTime(&tEnd);
	ElapsedTime = (1.0 * (tEnd - tStart) / (COUNTS_PER_SECOND));
	printf("Addition x100 Time: %lf Seconds\n", ElapsedTime);

	printf("VM Matrix Addition-------------------------------------------------------\n");
	resetTable(table);
	Store_M(&matrix1, 1, table);
	Store_M(&matrix2, 2, table);
	safeAllocatePRegs(1, NULL, 0, table);
	safeAllocatePRegs(2, protectedVReg, 1, table);
	ResetCounts();
	E_Add_MM(1,2,3, table);
	PrintCounts();
//	printVReg(3, table);

	XTime_GetTime(&tStart);
	for(int i=0; i<100; i++)
	{
		E_Add_MM(1,2,3, table);
	}
	XTime_GetTime(&tEnd);
	ElapsedTime = (1.0 * (tEnd - tStart) / (COUNTS_PER_SECOND));
	printf("Addition x100 Time: %lf Seconds\n", ElapsedTime);


	//subtraction
	//void WRITE_Matrix_Large(int row, int col, int block_row, int block_col, int row_blk_size, int col_blk_size, int W[][col], int reg, int copy, int block_dimension){
	WRITE_Matrix_Large(rowN, colN, 0, 0, 64, 64, arr1, 1, 1, 0); //copy=1 for 2d array //goes into preg 1
	WRITE_Matrix_Large(rowN, colN, 1, 0, 64, 64, arr1, 2, 1, 0); //copy=1 for 2d array //goes into preg 2

	WRITE_Matrix_Large(rowN, colN, 0, 0, 64, 64, arr2, 3, 1, 0); //goes into preg3
	WRITE_Matrix_Large(rowN, colN, 1, 0, 64, 64, arr2, 4, 1, 0); //goes into preg4

	ResetCounts();
	MATRIX_SUBTRACTION(1,3,5); //here
	MATRIX_SUBTRACTION(2,4,6); //here
	printf("Native Matrix Subtraction---------------------------------------------------\n"); //here
	PrintCounts();


	XTime_GetTime(&tStart);
	for(int i=0; i<100; i++)
	{
		MATRIX_SUBTRACTION(1,3,5); //here
		MATRIX_SUBTRACTION(2,4,6); //here
	}
	XTime_GetTime(&tEnd);
	ElapsedTime = (1.0 * (tEnd - tStart) / (COUNTS_PER_SECOND));
	printf("Subtraction x100 Time: %lf Seconds\n", ElapsedTime);

	printf("VM Matrix Subtraction-------------------------------------------------------\n"); //here
	resetTable(table);
	Store_M(&matrix1, 1, table);
	Store_M(&matrix2, 2, table);
	safeAllocatePRegs(1, NULL, 0, table);
	safeAllocatePRegs(2, protectedVReg, 1, table);
	ResetCounts();
	E_Sub_MM(1,2,3, table); //here
	PrintCounts();
//	printVReg(3, table);

	XTime_GetTime(&tStart);
	for(int i=0; i<100; i++)
	{
		E_Sub_MM(1,2,3, table); //here
	}
	XTime_GetTime(&tEnd);
	ElapsedTime = (1.0 * (tEnd - tStart) / (COUNTS_PER_SECOND));
	printf("Subtraction x100 Time: %lf Seconds\n", ElapsedTime);

	//================================================================================================================================
	//Multiplication
	//void WRITE_Matrix_Large(int row, int col, int block_row, int block_col, int row_blk_size, int col_blk_size, int W[][col], int reg, int copy, int block_dimension){
	WRITE_Matrix_Large(rowN, colN, 0, 0, 64, 64, arr1, 1, 1, 0); //copy=1 for 2d array //goes into preg 1
	WRITE_Matrix_Large(rowN, colN, 1, 0, 64, 64, arr1, 2, 1, 0); //copy=1 for 2d array //goes into preg 2

	WRITE_Matrix_Large(rowN, colN, 0, 0, 64, 64, arr2, 3, 1, 0); //goes into preg3
	WRITE_Matrix_Large(rowN, colN, 1, 0, 64, 64, arr2, 4, 1, 0); //goes into preg4

	ResetCounts();
	ELEMENTWISE_MULTIPLICATION(1,3,5); //here
	ELEMENTWISE_MULTIPLICATION(2,4,6); //here
	printf("Native Matrix Multiplication---------------------------------------------------\n"); //here
	PrintCounts();


	XTime_GetTime(&tStart);
	for(int i=0; i<100; i++)
	{
		ELEMENTWISE_MULTIPLICATION(1,3,5); //here
		ELEMENTWISE_MULTIPLICATION(2,4,6); //here
	}
	XTime_GetTime(&tEnd);
	ElapsedTime = (1.0 * (tEnd - tStart) / (COUNTS_PER_SECOND));
	printf("Multiplication x100 Time: %lf Seconds\n", ElapsedTime);

	printf("VM Matrix Subtraction-------------------------------------------------------\n"); //here
	resetTable(table);
	Store_M(&matrix1, 1, table);
	Store_M(&matrix2, 2, table);
	safeAllocatePRegs(1, NULL, 0, table);
	safeAllocatePRegs(2, protectedVReg, 1, table);
	ResetCounts();
	E_Mul_MM(1,2,3, table); //here
	PrintCounts();

	XTime_GetTime(&tStart);
	for(int i=0; i<100; i++)
	{
		E_Mul_MM(1,2,3, table); //here
	}
	XTime_GetTime(&tEnd);
	ElapsedTime = (1.0 * (tEnd - tStart) / (COUNTS_PER_SECOND));
	printf("Multiplication x100 Time: %lf Seconds\n", ElapsedTime);
	return 1;
}

int Test_Elementwise_4Segment(AllocationTable *table){
	printf("\n4 Segment Elementwise\n");
	resetTable(table);
	printf("-------------------------------------------------------------------------\r\n");

	//time variable
	XTime tStart, tEnd;
	double ElapsedTime;

	//define sample data
	int rowN = 128;
	int colN = 128;
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
	Matrix matrix1, matrix2;
	Declare_M(&matrix1, rowN, colN);
	Declare_M(&matrix2, rowN, colN);
	free(matrix1.memory);
	free(matrix2.memory);
	matrix1.memory=(int*)&arr1;
	matrix2.memory=(int*)&arr2;
	int protectedVReg[] = {1};



	//addition
	//void WRITE_Matrix_Large(int row, int col, int block_row, int block_col, int row_blk_size, int col_blk_size, int W[][col], int reg, int copy, int block_dimension){
	WRITE_Matrix_Large(rowN, colN, 0, 0, 64, 64, arr1, 1, 1, 0); //copy=1 for 2d array //goes into preg 1
	WRITE_Matrix_Large(rowN, colN, 0, 1, 64, 64, arr1, 2, 1, 0); //copy=1 for 2d array //goes into preg 2
	WRITE_Matrix_Large(rowN, colN, 1, 0, 64, 64, arr1, 3, 1, 0); //copy=1 for 2d array //goes into preg 2
	WRITE_Matrix_Large(rowN, colN, 1, 1, 64, 64, arr1, 4, 1, 0); //copy=1 for 2d array //goes into preg 2

	WRITE_Matrix_Large(rowN, colN, 0, 0, 64, 64, arr2, 7, 1, 0); //goes into preg7
	WRITE_Matrix_Large(rowN, colN, 0, 1, 64, 64, arr2, 8, 1, 0); //goes into preg8
	WRITE_Matrix_Large(rowN, colN, 1, 0, 64, 64, arr2, 9, 1, 0); //goes into preg9
	WRITE_Matrix_Large(rowN, colN, 1, 1, 64, 64, arr2, 10, 1, 0); //goes into preg10

	ResetCounts();
	MATRIX_ADDITION(1,7,15);
	MATRIX_ADDITION(2,8,16);
	MATRIX_ADDITION(3,9,17);
	MATRIX_ADDITION(4,10,18);
	printf("Native Matrix Addition---------------------------------------------------\n");
	PrintCounts();


	XTime_GetTime(&tStart);
	for(int i=0; i<100; i++)
	{
		MATRIX_ADDITION(1,7,15);
		MATRIX_ADDITION(2,8,16);
		MATRIX_ADDITION(3,9,17);
		MATRIX_ADDITION(4,10,18);
	}
	XTime_GetTime(&tEnd);
	ElapsedTime = (1.0 * (tEnd - tStart) / (COUNTS_PER_SECOND));
	printf("Addition x100 Time: %lf Seconds\n", ElapsedTime);

	printf("VM Matrix Addition-------------------------------------------------------\n");
	resetTable(table);
	Store_M(&matrix1, 1, table);
	Store_M(&matrix2, 2, table);
	safeAllocatePRegs(1, NULL, 0, table);
	safeAllocatePRegs(2, protectedVReg, 1, table);
	ResetCounts();
	E_Add_MM(1,2,3, table);
	PrintCounts();
//	printVReg(3, table);

	XTime_GetTime(&tStart);
	for(int i=0; i<100; i++)
	{
		E_Add_MM(1,2,3, table);
	}
	XTime_GetTime(&tEnd);
	ElapsedTime = (1.0 * (tEnd - tStart) / (COUNTS_PER_SECOND));
	printf("Addition x100 Time: %lf Seconds\n", ElapsedTime);


	//subtraction
	//void WRITE_Matrix_Large(int row, int col, int block_row, int block_col, int row_blk_size, int col_blk_size, int W[][col], int reg, int copy, int block_dimension){
	WRITE_Matrix_Large(rowN, colN, 0, 0, 64, 64, arr1, 1, 1, 0); //copy=1 for 2d array //goes into preg 1
	WRITE_Matrix_Large(rowN, colN, 0, 1, 64, 64, arr1, 2, 1, 0); //copy=1 for 2d array //goes into preg 2
	WRITE_Matrix_Large(rowN, colN, 1, 0, 64, 64, arr1, 3, 1, 0); //copy=1 for 2d array //goes into preg 2
	WRITE_Matrix_Large(rowN, colN, 1, 1, 64, 64, arr1, 4, 1, 0); //copy=1 for 2d array //goes into preg 2

	WRITE_Matrix_Large(rowN, colN, 0, 0, 64, 64, arr2, 7, 1, 0); //goes into preg7
	WRITE_Matrix_Large(rowN, colN, 0, 1, 64, 64, arr2, 8, 1, 0); //goes into preg8
	WRITE_Matrix_Large(rowN, colN, 1, 0, 64, 64, arr2, 9, 1, 0); //goes into preg9
	WRITE_Matrix_Large(rowN, colN, 1, 1, 64, 64, arr2, 10, 1, 0); //goes into preg10


	ResetCounts();
	MATRIX_SUBTRACTION(1,7,15);
	MATRIX_SUBTRACTION(2,8,16);
	MATRIX_SUBTRACTION(3,9,17);
	MATRIX_SUBTRACTION(4,10,18);
	printf("Native Matrix Subtraction---------------------------------------------------\n"); //here
	PrintCounts();


	XTime_GetTime(&tStart);
	for(int i=0; i<100; i++)
	{
		MATRIX_SUBTRACTION(1,7,15);
		MATRIX_SUBTRACTION(2,8,16);
		MATRIX_SUBTRACTION(3,9,17);
		MATRIX_SUBTRACTION(4,10,18);
	}
	XTime_GetTime(&tEnd);
	ElapsedTime = (1.0 * (tEnd - tStart) / (COUNTS_PER_SECOND));
	printf("Subtraction x100 Time: %lf Seconds\n", ElapsedTime);

	printf("VM Matrix Subtraction-------------------------------------------------------\n"); //here
	resetTable(table);
	Store_M(&matrix1, 1, table);
	Store_M(&matrix2, 2, table);
	safeAllocatePRegs(1, NULL, 0, table);
	safeAllocatePRegs(2, protectedVReg, 1, table);
	ResetCounts();
	E_Sub_MM(1,2,3, table); //here
	PrintCounts();
//	printVReg(3, table);

	XTime_GetTime(&tStart);
	for(int i=0; i<100; i++)
	{
		E_Sub_MM(1,2,3, table); //here
	}
	XTime_GetTime(&tEnd);
	ElapsedTime = (1.0 * (tEnd - tStart) / (COUNTS_PER_SECOND));
	printf("Subtraction x100 Time: %lf Seconds\n", ElapsedTime);

	//================================================================================================================================
	//Multiplication
	//void WRITE_Matrix_Large(int row, int col, int block_row, int block_col, int row_blk_size, int col_blk_size, int W[][col], int reg, int copy, int block_dimension){
	WRITE_Matrix_Large(rowN, colN, 0, 0, 64, 64, arr1, 1, 1, 0); //copy=1 for 2d array //goes into preg 1
	WRITE_Matrix_Large(rowN, colN, 0, 1, 64, 64, arr1, 2, 1, 0); //copy=1 for 2d array //goes into preg 2
	WRITE_Matrix_Large(rowN, colN, 1, 0, 64, 64, arr1, 3, 1, 0); //copy=1 for 2d array //goes into preg 2
	WRITE_Matrix_Large(rowN, colN, 1, 1, 64, 64, arr1, 4, 1, 0); //copy=1 for 2d array //goes into preg 2

	WRITE_Matrix_Large(rowN, colN, 0, 0, 64, 64, arr2, 7, 1, 0); //goes into preg7
	WRITE_Matrix_Large(rowN, colN, 0, 1, 64, 64, arr2, 8, 1, 0); //goes into preg8
	WRITE_Matrix_Large(rowN, colN, 1, 0, 64, 64, arr2, 9, 1, 0); //goes into preg9
	WRITE_Matrix_Large(rowN, colN, 1, 1, 64, 64, arr2, 10, 1, 0); //goes into preg10


	ResetCounts();
	ELEMENTWISE_MULTIPLICATION(1,7,15); //here
	ELEMENTWISE_MULTIPLICATION(2,8,16);
	ELEMENTWISE_MULTIPLICATION(3,9,17);
	ELEMENTWISE_MULTIPLICATION(4,10,18);
	printf("Native Matrix Multiplication---------------------------------------------------\n"); //here
	PrintCounts();


	XTime_GetTime(&tStart);
	for(int i=0; i<100; i++)
	{
		ELEMENTWISE_MULTIPLICATION(1,7,15); //here
		ELEMENTWISE_MULTIPLICATION(2,8,16);
		ELEMENTWISE_MULTIPLICATION(3,9,17);
		ELEMENTWISE_MULTIPLICATION(4,10,18);
	}
	XTime_GetTime(&tEnd);
	ElapsedTime = (1.0 * (tEnd - tStart) / (COUNTS_PER_SECOND));
	printf("Multiplication x100 Time: %lf Seconds\n", ElapsedTime);

	printf("VM Matrix Subtraction-------------------------------------------------------\n"); //here
	resetTable(table);
	Store_M(&matrix1, 1, table);
	Store_M(&matrix2, 2, table);
	safeAllocatePRegs(1, NULL, 0, table);
	safeAllocatePRegs(2, protectedVReg, 1, table);
	ResetCounts();
	E_Mul_MM(1,2,3, table); //here
	PrintCounts();

	XTime_GetTime(&tStart);
	for(int i=0; i<100; i++)
	{
		E_Mul_MM(1,2,3, table); //here
	}
	XTime_GetTime(&tEnd);
	ElapsedTime = (1.0 * (tEnd - tStart) / (COUNTS_PER_SECOND));
	printf("Multiplication x100 Time: %lf Seconds\n", ElapsedTime);
	return 1;
}

int Test_Elementwise_8Segment(AllocationTable *table){
	printf("\n8 Segment Elementwise\n");
	resetTable(table);
	printf("-------------------------------------------------------------------------\r\n");

	//time variable
	XTime tStart, tEnd;
	double ElapsedTime;

	//define sample data
	int rowN = 256;
	int colN = 128;
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
	Matrix matrix1, matrix2;
	Declare_M(&matrix1, rowN, colN);
	Declare_M(&matrix2, rowN, colN);
	free(matrix1.memory);
	free(matrix2.memory);
	matrix1.memory=(int*)&arr1;
	matrix2.memory=(int*)&arr2;
	int protectedVReg[] = {1};



	//addition
	//void WRITE_Matrix_Large(int row, int col, int block_row, int block_col, int row_blk_size, int col_blk_size, int W[][col], int reg, int copy, int block_dimension){
	WRITE_Matrix_Large(rowN, colN, 0, 0, 64, 64, arr1, 1, 1, 0); //copy=1 for 2d array
	WRITE_Matrix_Large(rowN, colN, 0, 1, 64, 64, arr1, 2, 1, 0); //copy=1 for 2d array
	WRITE_Matrix_Large(rowN, colN, 1, 0, 64, 64, arr1, 3, 1, 0); //copy=1 for 2d array
	WRITE_Matrix_Large(rowN, colN, 1, 1, 64, 64, arr1, 4, 1, 0); //copy=1 for 2d array
	WRITE_Matrix_Large(rowN, colN, 2, 0, 64, 64, arr1, 5, 1, 0); //copy=1 for 2d array
	WRITE_Matrix_Large(rowN, colN, 2, 1, 64, 64, arr1, 6, 1, 0); //copy=1 for 2d array
	WRITE_Matrix_Large(rowN, colN, 3, 0, 64, 64, arr1, 7, 1, 0); //copy=1 for 2d array
	WRITE_Matrix_Large(rowN, colN, 3, 1, 64, 64, arr1, 8, 1, 0); //copy=1 for 2d array

	WRITE_Matrix_Large(rowN, colN, 0, 0, 64, 64, arr2, 9, 1, 0); //goes into preg7
	WRITE_Matrix_Large(rowN, colN, 0, 1, 64, 64, arr2, 10, 1, 0); //goes into preg8
	WRITE_Matrix_Large(rowN, colN, 1, 0, 64, 64, arr2, 11, 1, 0); //goes into preg9
	WRITE_Matrix_Large(rowN, colN, 1, 1, 64, 64, arr2, 12, 1, 0); //goes into preg10
	WRITE_Matrix_Large(rowN, colN, 2, 0, 64, 64, arr2, 13, 1, 0); //goes into preg7
	WRITE_Matrix_Large(rowN, colN, 2, 1, 64, 64, arr2, 14, 1, 0); //goes into preg8
	WRITE_Matrix_Large(rowN, colN, 3, 0, 64, 64, arr2, 15, 1, 0); //goes into preg9
	WRITE_Matrix_Large(rowN, colN, 3, 1, 64, 64, arr2, 16, 1, 0); //goes into preg10


	ResetCounts();
	MATRIX_ADDITION(1, 9, 17);
	MATRIX_ADDITION(2, 10, 18);
	MATRIX_ADDITION(3, 11, 19);
	MATRIX_ADDITION(4, 12, 20);
	MATRIX_ADDITION(5, 13, 21);
	MATRIX_ADDITION(6, 14, 22);
	MATRIX_ADDITION(7, 15, 23);
	MATRIX_ADDITION(8, 16, 24);

	printf("Native Matrix Addition---------------------------------------------------\n");
	PrintCounts();


	XTime_GetTime(&tStart);
	for(int i=0; i<100; i++)
	{
		MATRIX_ADDITION(1, 9, 17);
		MATRIX_ADDITION(2, 10, 18);
		MATRIX_ADDITION(3, 11, 19);
		MATRIX_ADDITION(4, 12, 20);
		MATRIX_ADDITION(5, 13, 21);
		MATRIX_ADDITION(6, 14, 22);
		MATRIX_ADDITION(7, 15, 23);
		MATRIX_ADDITION(8, 16, 24);
	}
	XTime_GetTime(&tEnd);
	ElapsedTime = (1.0 * (tEnd - tStart) / (COUNTS_PER_SECOND));
	printf("Addition x100 Time: %lf Seconds\n", ElapsedTime);

	printf("VM Matrix Addition-------------------------------------------------------\n");
	resetTable(table);
	Store_M(&matrix1, 1, table);
	Store_M(&matrix2, 2, table);
	safeAllocatePRegs(1, NULL, 0, table);
	safeAllocatePRegs(2, protectedVReg, 1, table);
	ResetCounts();
	E_Add_MM(1,2,3, table);
	PrintCounts();
//	printVReg(3, table);

	XTime_GetTime(&tStart);
	for(int i=0; i<100; i++)
	{
		E_Add_MM(1,2,3, table);
	}
	XTime_GetTime(&tEnd);
	ElapsedTime = (1.0 * (tEnd - tStart) / (COUNTS_PER_SECOND));
	printf("Addition x100 Time: %lf Seconds\n", ElapsedTime);


	//subtraction
	//void WRITE_Matrix_Large(int row, int col, int block_row, int block_col, int row_blk_size, int col_blk_size, int W[][col], int reg, int copy, int block_dimension){
	WRITE_Matrix_Large(rowN, colN, 0, 0, 64, 64, arr1, 1, 1, 0); //copy=1 for 2d array
	WRITE_Matrix_Large(rowN, colN, 0, 1, 64, 64, arr1, 2, 1, 0); //copy=1 for 2d array
	WRITE_Matrix_Large(rowN, colN, 1, 0, 64, 64, arr1, 3, 1, 0); //copy=1 for 2d array
	WRITE_Matrix_Large(rowN, colN, 1, 1, 64, 64, arr1, 4, 1, 0); //copy=1 for 2d array
	WRITE_Matrix_Large(rowN, colN, 2, 0, 64, 64, arr1, 5, 1, 0); //copy=1 for 2d array
	WRITE_Matrix_Large(rowN, colN, 2, 1, 64, 64, arr1, 6, 1, 0); //copy=1 for 2d array
	WRITE_Matrix_Large(rowN, colN, 3, 0, 64, 64, arr1, 7, 1, 0); //copy=1 for 2d array
	WRITE_Matrix_Large(rowN, colN, 3, 1, 64, 64, arr1, 8, 1, 0); //copy=1 for 2d array

	WRITE_Matrix_Large(rowN, colN, 0, 0, 64, 64, arr2, 9, 1, 0); //goes into preg7
	WRITE_Matrix_Large(rowN, colN, 0, 1, 64, 64, arr2, 10, 1, 0); //goes into preg8
	WRITE_Matrix_Large(rowN, colN, 1, 0, 64, 64, arr2, 11, 1, 0); //goes into preg9
	WRITE_Matrix_Large(rowN, colN, 1, 1, 64, 64, arr2, 12, 1, 0); //goes into preg10
	WRITE_Matrix_Large(rowN, colN, 2, 0, 64, 64, arr2, 13, 1, 0); //goes into preg7
	WRITE_Matrix_Large(rowN, colN, 2, 1, 64, 64, arr2, 14, 1, 0); //goes into preg8
	WRITE_Matrix_Large(rowN, colN, 3, 0, 64, 64, arr2, 15, 1, 0); //goes into preg9
	WRITE_Matrix_Large(rowN, colN, 3, 1, 64, 64, arr2, 16, 1, 0); //goes into preg10


	ResetCounts();
	MATRIX_SUBTRACTION(1, 9, 17);
	MATRIX_SUBTRACTION(2, 10, 18);
	MATRIX_SUBTRACTION(3, 11, 19);
	MATRIX_SUBTRACTION(4, 12, 20);
	MATRIX_SUBTRACTION(5, 13, 21);
	MATRIX_SUBTRACTION(6, 14, 22);
	MATRIX_SUBTRACTION(7, 15, 23);
	MATRIX_SUBTRACTION(8, 16, 24);
	printf("Native Matrix Subtraction---------------------------------------------------\n"); //here
	PrintCounts();


	XTime_GetTime(&tStart);
	for(int i=0; i<100; i++)
	{
		MATRIX_SUBTRACTION(1, 9, 17);
		MATRIX_SUBTRACTION(2, 10, 18);
		MATRIX_SUBTRACTION(3, 11, 19);
		MATRIX_SUBTRACTION(4, 12, 20);
		MATRIX_SUBTRACTION(5, 13, 21);
		MATRIX_SUBTRACTION(6, 14, 22);
		MATRIX_SUBTRACTION(7, 15, 23);
		MATRIX_SUBTRACTION(8, 16, 24);
	}
	XTime_GetTime(&tEnd);
	ElapsedTime = (1.0 * (tEnd - tStart) / (COUNTS_PER_SECOND));
	printf("Subtraction x100 Time: %lf Seconds\n", ElapsedTime);

	printf("VM Matrix Subtraction-------------------------------------------------------\n"); //here
	resetTable(table);
	Store_M(&matrix1, 1, table);
	Store_M(&matrix2, 2, table);
	safeAllocatePRegs(1, NULL, 0, table);
	safeAllocatePRegs(2, protectedVReg, 1, table);
	ResetCounts();
	E_Sub_MM(1,2,3, table); //here
	PrintCounts();
//	printVReg(3, table);

	XTime_GetTime(&tStart);
	for(int i=0; i<100; i++)
	{
		E_Sub_MM(1,2,3, table); //here
	}
	XTime_GetTime(&tEnd);
	ElapsedTime = (1.0 * (tEnd - tStart) / (COUNTS_PER_SECOND));
	printf("Subtraction x100 Time: %lf Seconds\n", ElapsedTime);

	//================================================================================================================================
	//Multiplication
	//void WRITE_Matrix_Large(int row, int col, int block_row, int block_col, int row_blk_size, int col_blk_size, int W[][col], int reg, int copy, int block_dimension){
	WRITE_Matrix_Large(rowN, colN, 0, 0, 64, 64, arr1, 1, 1, 0); //copy=1 for 2d array
	WRITE_Matrix_Large(rowN, colN, 0, 1, 64, 64, arr1, 2, 1, 0); //copy=1 for 2d array
	WRITE_Matrix_Large(rowN, colN, 1, 0, 64, 64, arr1, 3, 1, 0); //copy=1 for 2d array
	WRITE_Matrix_Large(rowN, colN, 1, 1, 64, 64, arr1, 4, 1, 0); //copy=1 for 2d array
	WRITE_Matrix_Large(rowN, colN, 2, 0, 64, 64, arr1, 5, 1, 0); //copy=1 for 2d array
	WRITE_Matrix_Large(rowN, colN, 2, 1, 64, 64, arr1, 6, 1, 0); //copy=1 for 2d array
	WRITE_Matrix_Large(rowN, colN, 3, 0, 64, 64, arr1, 7, 1, 0); //copy=1 for 2d array
	WRITE_Matrix_Large(rowN, colN, 3, 1, 64, 64, arr1, 8, 1, 0); //copy=1 for 2d array

	WRITE_Matrix_Large(rowN, colN, 0, 0, 64, 64, arr2, 9, 1, 0); //goes into preg7
	WRITE_Matrix_Large(rowN, colN, 0, 1, 64, 64, arr2, 10, 1, 0); //goes into preg8
	WRITE_Matrix_Large(rowN, colN, 1, 0, 64, 64, arr2, 11, 1, 0); //goes into preg9
	WRITE_Matrix_Large(rowN, colN, 1, 1, 64, 64, arr2, 12, 1, 0); //goes into preg10
	WRITE_Matrix_Large(rowN, colN, 2, 0, 64, 64, arr2, 13, 1, 0); //goes into preg7
	WRITE_Matrix_Large(rowN, colN, 2, 1, 64, 64, arr2, 14, 1, 0); //goes into preg8
	WRITE_Matrix_Large(rowN, colN, 3, 0, 64, 64, arr2, 15, 1, 0); //goes into preg9
	WRITE_Matrix_Large(rowN, colN, 3, 1, 64, 64, arr2, 16, 1, 0); //goes into preg10


	ResetCounts();
	ELEMENTWISE_MULTIPLICATION(1, 9, 17);
	ELEMENTWISE_MULTIPLICATION(2, 10, 18);
	ELEMENTWISE_MULTIPLICATION(3, 11, 19);
	ELEMENTWISE_MULTIPLICATION(4, 12, 20);
	ELEMENTWISE_MULTIPLICATION(5, 13, 21);
	ELEMENTWISE_MULTIPLICATION(6, 14, 22);
	ELEMENTWISE_MULTIPLICATION(7, 15, 23);
	ELEMENTWISE_MULTIPLICATION(8, 16, 24);
	printf("Native Matrix Multiplication---------------------------------------------------\n"); //here
	PrintCounts();


	XTime_GetTime(&tStart);
	for(int i=0; i<100; i++)
	{
		ELEMENTWISE_MULTIPLICATION(1, 9, 17);
		ELEMENTWISE_MULTIPLICATION(2, 10, 18);
		ELEMENTWISE_MULTIPLICATION(3, 11, 19);
		ELEMENTWISE_MULTIPLICATION(4, 12, 20);
		ELEMENTWISE_MULTIPLICATION(5, 13, 21);
		ELEMENTWISE_MULTIPLICATION(6, 14, 22);
		ELEMENTWISE_MULTIPLICATION(7, 15, 23);
		ELEMENTWISE_MULTIPLICATION(8, 16, 24);
	}
	XTime_GetTime(&tEnd);
	ElapsedTime = (1.0 * (tEnd - tStart) / (COUNTS_PER_SECOND));
	printf("Multiplication x100 Time: %lf Seconds\n", ElapsedTime);

	printf("VM Matrix Multiplication-------------------------------------------------------\n"); //here
	resetTable(table);
	Store_M(&matrix1, 1, table);
	Store_M(&matrix2, 2, table);
	safeAllocatePRegs(1, NULL, 0, table);
	safeAllocatePRegs(2, protectedVReg, 1, table);
	ResetCounts();
	E_Mul_MM(1,2,3, table); //here
	PrintCounts();

	XTime_GetTime(&tStart);
	for(int i=0; i<100; i++)
	{
		E_Mul_MM(1,2,3, table); //here
	}
	XTime_GetTime(&tEnd);
	ElapsedTime = (1.0 * (tEnd - tStart) / (COUNTS_PER_SECOND));
	printf("Multiplication x100 Time: %lf Seconds\n", ElapsedTime);
	return 1;
}

int Test_MulAcc_1Segment(AllocationTable *table){
	printf("\n1 Segment MV Multiply Accumulate\n");
	//time variable
	XTime tStart, tEnd;
	double ElapsedTime;

	//define sample data
	int rowN = 64;
	int colN = 64;
	int arr1[rowN][colN];
	int arr2[rowN][colN];
	for(int i=0; i < rowN; i++)
	{
		for(int j=0; j < colN; j++)
		{
			arr1[i][j] = 2*j+256+(4*i);
			arr2[i][j] = (2)<<14;
		}
	}
	Matrix matrix1, matrix2;
	Vector vector1;
	Declare_M(&matrix1, rowN, colN);
	Declare_M(&matrix2, rowN, colN);
	Declare_V(&vector1, colN);
	free(matrix1.memory);
	free(matrix2.memory);
	free(vector1.memory);
	matrix1.memory=(int*)&arr1;
	matrix2.memory=(int*)&arr2;
	vector1.memory=(int*)&arr2;
	//Native approach
	//void WRITE_Matrix(int row, int col, int W[][col], int reg, int copy);
	printf("Native Matrix-Vector Multiplication-------------------------------------------------------\n");
	WRITE_Matrix(rowN, colN, arr1, 1, 1); //store the matrix
	MATRIX_SUBTRACTION(1, 1, 9);
	WRITE_Matrix(1, colN, arr2, 9, 1); //store the vector
	ResetCounts();

	//fill entire preg for vector
	//clear preg 0
	MATRIX_SUBTRACTION(1, 1, 0);
	//shift into register 0 the vector
	SHIFT_SOUTH(9, 0);
	for(int x=0; x<SPAR_dimension-1; x++)
	{
		MATRIX_ADDITION(9, 0, 9);
		SHIFT_SOUTH(0,0);
	}

	//Multiply
	ELEMENTWISE_MULTIPLICATION(1, 9, 17); //17 is the destination

	//accumulate
	SHIFT_WEST(17, 25);
	MATRIX_ADDITION(17, 25, 17);
	for(int x=0; x<colN-2; x++)
	{
		SHIFT_WEST(25, 25);
		//add into result
		MATRIX_ADDITION(17, 25, 17);
	}
	PrintCounts();

	//get time for 100 iterations
	XTime_GetTime(&tStart);
	for(int i=0; i<100; i++)
	{
		//fill entire preg for vector
		//clear preg 0
		MATRIX_SUBTRACTION(1, 1, 0);
		//shift into register 0 the vector
		SHIFT_SOUTH(9, 0);
		for(int x=0; x<SPAR_dimension-1; x++)
		{
			MATRIX_ADDITION(9, 0, 9);
			SHIFT_SOUTH(0,0);
		}

		//Multiply
		ELEMENTWISE_MULTIPLICATION(1, 9, 17); //17 is the destination

		//accumulate
		SHIFT_WEST(17, 25);
		MATRIX_ADDITION(17, 25, 17);
		for(int x=0; x<colN-2; x++)
		{
			SHIFT_WEST(25, 25);
			//add into result
			MATRIX_ADDITION(17, 25, 17);
		}
	}

	XTime_GetTime(&tEnd);
	ElapsedTime = (1.0 * (tEnd - tStart) / (COUNTS_PER_SECOND));
	printf("Matrix-Vector Multiplication x100 Time: %lf Seconds\n", ElapsedTime);

	printf("VM Matrix-Vector Multiplication-------------------------------------------------------\n"); //here
	resetTable(table);
	Store_M(&matrix1, 1, table);
	Store_V(&vector1, 2, table);
	Mul_MV(1,2,3, table); //run it once just to setup the data

	ResetCounts();
	Mul_MV(1,2,3, table); //here
	PrintCounts();

	XTime_GetTime(&tStart);
	for(int i=0; i<100; i++)
	{
		Mul_MV(1,2,3, table); //here
	}
	XTime_GetTime(&tEnd);
	ElapsedTime = (1.0 * (tEnd - tStart) / (COUNTS_PER_SECOND));
	printf("Matrix-Vector Multiplication x100 Time: %lf Seconds\n", ElapsedTime);


	return 1; //I'm not sure why I even had this return anything
}

int Test_MulAcc_2Segment(AllocationTable *table){
	printf("\n2 Segment MV Multiply Accumulate\n");
	//time variable
	XTime tStart, tEnd;
	double ElapsedTime;

	//define sample data
	int rowN = 128;
	int colN = 64;
	int arr1[rowN][colN];
	int arr2[rowN][colN];
	for(int i=0; i < rowN; i++)
	{
		for(int j=0; j < colN; j++)
		{
			arr1[i][j] = 2*j+256+(4*i);
			arr2[i][j] = (2)<<14;
		}
	}
	Matrix matrix1, matrix2;
	Vector vector1;
	Declare_M(&matrix1, rowN, colN);
	Declare_M(&matrix2, rowN, colN);
	Declare_V(&vector1, colN);
	free(matrix1.memory);
	free(matrix2.memory);
	free(vector1.memory);
	matrix1.memory=(int*)&arr1;
	matrix2.memory=(int*)&arr2;
	vector1.memory=(int*)&arr2;
	//Native approach
	//void WRITE_Matrix(int row, int col, int W[][col], int reg, int copy);
	printf("Native Matrix-Vector Multiplication-------------------------------------------------------\n");
	WRITE_Matrix(rowN, colN, arr1, 1, 1); //store the matrix
	WRITE_Matrix_Large(rowN, colN, 0, 0, 64, 64, arr1, 1, 1, 0); //copy=1 for 2d array //goes into preg 1
	WRITE_Matrix_Large(rowN, colN, 1, 0, 64, 64, arr1, 2, 1, 0); //copy=1 for 2d array //goes into preg 2

	execute(1, 9, 1, 1);
	WRITE_Matrix(1, colN, arr2, 9, 1); //store the vector
	ResetCounts();

	//fill entire preg 9 for vector
	MATRIX_SUBTRACTION(1, 1, 0);
	SHIFT_SOUTH(9, 0);
	for(int x=0; x<SPAR_dimension-1; x++)
	{
		MATRIX_ADDITION(9, 0, 9);
		SHIFT_SOUTH(0,0);
	}

	//Multiply Vector and Matrix
	ELEMENTWISE_MULTIPLICATION(1, 9, 17);
	ELEMENTWISE_MULTIPLICATION(2, 9, 18);
	//shift into the temp register 0
	SHIFT_WEST(17, 25);
	SHIFT_WEST(18, 26);
	MATRIX_ADDITION(17, 25, 17);
	MATRIX_ADDITION(18, 26, 18);	
	
	//accumulate
	for(int x=0; x<colN-2; x++)
	{
		SHIFT_WEST(25, 25);
		SHIFT_WEST(26, 26);
		//add into result
		MATRIX_ADDITION(17, 25, 17);
		MATRIX_ADDITION(18, 26, 18);	
	}
	PrintCounts();

	XTime_GetTime(&tStart);
	for(int i=0; i<100; i++)
	{
		//fill entire preg 9 for vector
		MATRIX_SUBTRACTION(1, 1, 0);
		SHIFT_SOUTH(9, 0);
		for(int x=0; x<SPAR_dimension-1; x++)
		{
			MATRIX_ADDITION(9, 0, 9);
			SHIFT_SOUTH(0,0);
		}

		//Multiply Vector and Matrix
		ELEMENTWISE_MULTIPLICATION(1, 9, 17);
		ELEMENTWISE_MULTIPLICATION(2, 9, 18);
		//shift into the temp register 0
		SHIFT_WEST(17, 25);
		SHIFT_WEST(18, 26);
		MATRIX_ADDITION(17, 25, 17);
		MATRIX_ADDITION(18, 26, 18);	
		
		//accumulate
		for(int x=0; x<colN-2; x++)
		{
			SHIFT_WEST(25, 25);
			SHIFT_WEST(26, 26);
			//add into result
			MATRIX_ADDITION(17, 25, 17);
			MATRIX_ADDITION(18, 26, 18);	
		}
	}
	XTime_GetTime(&tEnd);
	ElapsedTime = (1.0 * (tEnd - tStart) / (COUNTS_PER_SECOND));
	printf("Matrix-Vector Multiplication x100 Time: %lf Seconds\n", ElapsedTime);

	printf("VM Matrix-Vector Multiplication-------------------------------------------------------\n"); //here
	resetTable(table);
	Store_M(&matrix1, 1, table);
	Store_V(&vector1, 2, table);
	Mul_MV(1,2,3, table); //run it once just to load the data into spar. This will eliminate initial loading times

	ResetCounts();
	Mul_MV(1,2,3, table); //here
	PrintCounts();

	XTime_GetTime(&tStart);
	for(int i=0; i<100; i++)
	{
		Mul_MV(1,2,3, table); //here
	}
	XTime_GetTime(&tEnd);
	ElapsedTime = (1.0 * (tEnd - tStart) / (COUNTS_PER_SECOND));
	printf("Matrix-Vector Multiplication x100 Time: %lf Seconds\n", ElapsedTime);


	return 1;
}
int Test_MulAcc_2Segment_T(AllocationTable *table){
	printf("\n2 Segment MV Multiply Accumulate\n");
	//time variable
	XTime tStart, tEnd;
	double ElapsedTime;

	//define sample data
	int rowN = 64;
	int colN = 128;
	int arr1[rowN][colN];
	int arr2[rowN][colN];
	for(int i=0; i < rowN; i++)
	{
		for(int j=0; j < colN; j++)
		{
			arr1[i][j] = 2*j+256+(4*i);
			arr2[i][j] = (2)<<14;
		}
	}
	Matrix matrix1, matrix2;
	Vector vector1;
	Declare_M(&matrix1, rowN, colN);
	Declare_M(&matrix2, rowN, colN);
	Declare_V(&vector1, colN);
	free(matrix1.memory);
	free(matrix2.memory);
	free(vector1.memory);
	matrix1.memory=(int*)&arr1;
	matrix2.memory=(int*)&arr2;
	vector1.memory=(int*)&arr2;
	//Native approach
	//void WRITE_Matrix(int row, int col, int W[][col], int reg, int copy);
	printf("Native Matrix-Vector Multiplication-------------------------------------------------------\n");
	WRITE_Matrix(rowN, colN, arr1, 1, 1); //store the matrix
	WRITE_Matrix_Large(rowN, colN, 0, 0, 64, 64, arr1, 1, 1, 0); //copy=1 for 2d array //goes into preg 1
	WRITE_Matrix_Large(rowN, colN, 0, 1, 64, 64, arr1, 2, 1, 0); //copy=1 for 2d array //goes into preg 2

	//store vector
	//could have used subtraction method, but this is just for clearing the pregisters. Not timeed or anything.
	execute(1, 9, 1, 1);
	execute(1, 10, 1, 1);
	WRITE_Matrix_Large(1, colN, 0, 0, 1, 64, arr2, 9, 1, 0); //goes into preg9
	//WRITE_Matrix_Large(int row, int col, int block_row, int block_col, int row_blk_size, int col_blk_size, int W[][col], int reg, int copy, int block_dimension)
	WRITE_Matrix_Large(1, colN, 0, 1, 1, 64, arr2, 10, 1, 0);
	ResetCounts();

	//fill entire preg 9 then preg10 for vector
	MATRIX_SUBTRACTION(1, 1, 0);
	SHIFT_SOUTH(9, 0);
	for(int x=0; x<SPAR_dimension-1; x++)
	{
		MATRIX_ADDITION(9, 0, 9);
		SHIFT_SOUTH(0, 0);
	}
	MATRIX_SUBTRACTION(1, 1, 0);
	SHIFT_SOUTH(10, 0);
	for(int x=0; x<SPAR_dimension-1; x++)
	{
		MATRIX_ADDITION(10, 0, 10);
		SHIFT_SOUTH(0, 0);
	}

	//Multiply Vector and Matrix
	ELEMENTWISE_MULTIPLICATION(1, 9, 17);
	ELEMENTWISE_MULTIPLICATION(2, 10, 18);
	//shift into the temp register 0
	SHIFT_WEST(17, 25);
	WEST_COLUMN_MOVE(18,25);
	SHIFT_WEST(18, 26);
	MATRIX_ADDITION(17, 25, 17);
	MATRIX_ADDITION(18, 26, 18);

	//accumulate
	for(int x=0; x<colN-2; x++)
	{
		SHIFT_WEST(25, 25);
		WEST_COLUMN_MOVE(26,25);
		SHIFT_WEST(26, 26);
		//add into result
		MATRIX_ADDITION(17, 25, 17);
		MATRIX_ADDITION(18, 26, 18);
	}
	PrintCounts();

	XTime_GetTime(&tStart);
	for(int i=0; i<100; i++)
	{
		//fill entire preg 9 then preg10 for vector
		MATRIX_SUBTRACTION(1, 1, 0);
		SHIFT_SOUTH(9, 0);
		for(int x=0; x<SPAR_dimension-1; x++)
		{
			MATRIX_ADDITION(9, 0, 9);
			SHIFT_SOUTH(0, 0);
		}
		MATRIX_SUBTRACTION(1, 1, 0);
		SHIFT_SOUTH(10, 0);
		for(int x=0; x<SPAR_dimension-1; x++)
		{
			MATRIX_ADDITION(10, 0, 10);
			SHIFT_SOUTH(0, 0);
		}

		//Multiply Vector and Matrix
		ELEMENTWISE_MULTIPLICATION(1, 9, 17);
		ELEMENTWISE_MULTIPLICATION(2, 10, 18);
		//shift into the temp register 0
		SHIFT_WEST(17, 25);
		WEST_COLUMN_MOVE(18,25);
		SHIFT_WEST(18, 26);
		MATRIX_ADDITION(17, 25, 17);
		MATRIX_ADDITION(18, 26, 18);

		//accumulate
		for(int x=0; x<colN-2; x++)
		{
			SHIFT_WEST(25, 25);
			WEST_COLUMN_MOVE(26,25);
			SHIFT_WEST(26, 26);
			//add into result
			MATRIX_ADDITION(17, 25, 17);
			MATRIX_ADDITION(18, 26, 18);
		}
	}
	XTime_GetTime(&tEnd);
	ElapsedTime = (1.0 * (tEnd - tStart) / (COUNTS_PER_SECOND));
	printf("Matrix-Vector Multiplication x100 Time: %lf Seconds\n", ElapsedTime);

	printf("VM Matrix-Vector Multiplication-------------------------------------------------------\n"); //here
	resetTable(table);
	Store_M(&matrix1, 1, table);
	Store_V(&vector1, 2, table);
	Mul_MV(1,2,3, table); //run it once just to setup the data

	ResetCounts();
	Mul_MV(1,2,3, table); //here
	PrintCounts();
//	printVReg(3, table);

	XTime_GetTime(&tStart);
	for(int i=0; i<100; i++)
	{
		Mul_MV(1,2,3, table); //here
	}
	XTime_GetTime(&tEnd);
	ElapsedTime = (1.0 * (tEnd - tStart) / (COUNTS_PER_SECOND));
	printf("Matrix-Vector Multiplication x100 Time: %lf Seconds\n", ElapsedTime);
	return 1;
}

int Test_MulAcc_4Segment(AllocationTable *table){
	printf("\n4 Segment MV Multiply Accumulate\n");
	//time variable
	XTime tStart, tEnd;
	double ElapsedTime;

	//define sample data
	int rowN = 128;
	int colN = 128;
	int arr1[rowN][colN];
	int arr2[rowN][colN];
	for(int i=0; i < rowN; i++)
	{
		for(int j=0; j < colN; j++)
		{
			arr1[i][j] = 2*j+256+(4*i);
			arr2[i][j] = (2)<<14;
		}
	}
	Matrix matrix1, matrix2;
	Vector vector1;
	Declare_M(&matrix1, rowN, colN);
	Declare_M(&matrix2, rowN, colN);
	Declare_V(&vector1, colN);
	free(matrix1.memory);
	free(matrix2.memory);
	free(vector1.memory);
	matrix1.memory=(int*)&arr1;
	matrix2.memory=(int*)&arr2;
	vector1.memory=(int*)&arr2;
	//Native approach
	//void WRITE_Matrix(int row, int col, int W[][col], int reg, int copy);
	printf("Native Matrix-Vector Multiplication-------------------------------------------------------\n");
	WRITE_Matrix(rowN, colN, arr1, 1, 1); //store the matrix
	WRITE_Matrix_Large(rowN, colN, 0, 0, 64, 64, arr1, 1, 1, 0); //copy=1 for 2d array //goes into preg 1
	WRITE_Matrix_Large(rowN, colN, 0, 1, 64, 64, arr1, 2, 1, 0); //copy=1 for 2d array //goes into preg 2
	WRITE_Matrix_Large(rowN, colN, 1, 0, 64, 64, arr1, 3, 1, 0); //copy=1 for 2d array //goes into preg 3
	WRITE_Matrix_Large(rowN, colN, 1, 1, 64, 64, arr1, 4, 1, 0); //copy=1 for 2d array //goes into preg 4

	//could have used subtraction method, but this is just for clearing the pregisters. Not timeed or anything.
	execute(1, 9, 1, 1);
	execute(1, 10, 1, 1);
	WRITE_Matrix_Large(1, colN, 0, 0, 1, 64, arr2, 9, 1, 0); //goes into preg9
	//WRITE_Matrix_Large(int row, int col, int block_row, int block_col, int row_blk_size, int col_blk_size, int W[][col], int reg, int copy, int block_dimension)
	WRITE_Matrix_Large(1, colN, 0, 1, 1, 64, arr2, 10, 1, 0); //goes into preg10

	ResetCounts();
	//fill entire preg 9 and then 10 for vector
	MATRIX_SUBTRACTION(1, 1, 0);
	SHIFT_SOUTH(9, 0);
	for(int x=0; x<SPAR_dimension-1; x++)
	{
		MATRIX_ADDITION(9, 0, 9);
		SHIFT_SOUTH(0, 0);
	}
	MATRIX_SUBTRACTION(1, 1, 0);
	SHIFT_SOUTH(10, 0);
	for(int x=0; x<SPAR_dimension-1; x++)
	{
		MATRIX_ADDITION(10, 0, 10);
		SHIFT_SOUTH(0, 0);
	}


	//Multiply Vector and Matrix
	ELEMENTWISE_MULTIPLICATION(1, 9, 17);
	ELEMENTWISE_MULTIPLICATION(2, 10, 18);
	ELEMENTWISE_MULTIPLICATION(3, 9, 19);
	ELEMENTWISE_MULTIPLICATION(4, 10, 20);

	//accumulate
	SHIFT_WEST(17, 25);
	//move West Edge from 18 to 25
	WEST_COLUMN_MOVE(18, 25);
	SHIFT_WEST(18, 26);
	SHIFT_WEST(19, 27);
	//move West Edge from 20 to 27
	WEST_COLUMN_MOVE(20, 27);
	SHIFT_WEST(20, 28);
	MATRIX_ADDITION(17, 25, 17);
	MATRIX_ADDITION(18, 26, 18);
	MATRIX_ADDITION(19, 27, 19);
	MATRIX_ADDITION(20, 28, 20);
	for(int x=0; x<colN-2; x++)
	{
		SHIFT_WEST(25, 25);
		//move West Edge from 26 to 25
		WEST_COLUMN_MOVE(26, 25);
		SHIFT_WEST(26, 26);
		SHIFT_WEST(27, 27);
		//move West Edge from 28 to 27
		WEST_COLUMN_MOVE(28, 27);
		SHIFT_WEST(28, 28);
		//add into result
		MATRIX_ADDITION(17, 25, 17);
		MATRIX_ADDITION(18, 26, 18);
		MATRIX_ADDITION(19, 27, 19);
		MATRIX_ADDITION(20, 28, 20);
	}
	PrintCounts();

//	printf("here1\n");
//	printPReg(17);
//	printf("\n");
//	printPReg(19);
//	printf("\n");

	XTime_GetTime(&tStart);
	for(int i=0; i<100; i++)
	{
		//fill entire preg 9 and then 10 for vector
		MATRIX_SUBTRACTION(1, 1, 0);
		SHIFT_SOUTH(9, 0);
		for(int x=0; x<SPAR_dimension-1; x++)
		{
			MATRIX_ADDITION(9, 0, 9);
			SHIFT_SOUTH(0, 0);
		}
		MATRIX_SUBTRACTION(1, 1, 0);
		SHIFT_SOUTH(10, 0);
		for(int x=0; x<SPAR_dimension-1; x++)
		{
			MATRIX_ADDITION(10, 0, 10);
			SHIFT_SOUTH(0, 0);
		}


		//Multiply Vector and Matrix
		ELEMENTWISE_MULTIPLICATION(1, 9, 17);
		ELEMENTWISE_MULTIPLICATION(2, 10, 18);
		ELEMENTWISE_MULTIPLICATION(3, 9, 19);
		ELEMENTWISE_MULTIPLICATION(4, 10, 20);

		//accumulate
		SHIFT_WEST(17, 25);
		SHIFT_WEST(18, 26);
		SHIFT_WEST(19, 27);
		SHIFT_WEST(20, 28);
		MATRIX_ADDITION(17, 25, 17);
		MATRIX_ADDITION(18, 26, 18);
		MATRIX_ADDITION(19, 27, 19);
		MATRIX_ADDITION(20, 28, 20);
		for(int x=0; x<colN-2; x++)
		{
			SHIFT_WEST(25, 25);
			//move West Edge from 26 to 25
			WEST_COLUMN_MOVE(26, 25);
			SHIFT_WEST(26, 26);
			SHIFT_WEST(27, 27);
			//move West Edge from 28 to 27
			WEST_COLUMN_MOVE(28, 27);
			SHIFT_WEST(28, 28);
			//add into result
			MATRIX_ADDITION(17, 25, 17);
			MATRIX_ADDITION(18, 26, 18);
			MATRIX_ADDITION(19, 27, 19);
			MATRIX_ADDITION(20, 28, 20);
		}
	}
	XTime_GetTime(&tEnd);
	ElapsedTime = (1.0 * (tEnd - tStart) / (COUNTS_PER_SECOND));
	printf("Matrix-Vector Multiplication x100 Time: %lf Seconds\n", ElapsedTime);


	printf("VM Matrix-Vector Multiplication-------------------------------------------------------\n"); //here
	resetTable(table);
	Store_M(&matrix1, 1, table);
	Store_V(&vector1, 2, table);
	Mul_MV(1,2,3, table); //run it once just to setup the data

	ResetCounts();
	Mul_MV(1,2,3, table); //here
	PrintCounts();
//	printVReg(3, table);

	XTime_GetTime(&tStart);
	for(int i=0; i<100; i++)
	{
		Mul_MV(1,2,3, table); //here
	}
	XTime_GetTime(&tEnd);
	ElapsedTime = (1.0 * (tEnd - tStart) / (COUNTS_PER_SECOND));
	printf("Matrix-Vector Multiplication x100 Time: %lf Seconds\n", ElapsedTime);
	return 1;
}

int Test_MulAcc_8Segment(AllocationTable *table){
	printf("\n8 Segment MV Multiply Accumulate\n");
	//time variable
	XTime tStart, tEnd;
	double ElapsedTime;

	//define sample data
	int rowN = 256;
	int colN = 128;
	int arr1[rowN][colN];
	int arr2[rowN][colN];
	for(int i=0; i < rowN; i++)
	{
		for(int j=0; j < colN; j++)
		{
			arr1[i][j] = 2*j+256+(4*i);
			arr2[i][j] = (2)<<14;
		}
	}
	Matrix matrix1, matrix2;
	Vector vector1;
	Declare_M(&matrix1, rowN, colN);
	Declare_M(&matrix2, rowN, colN);
	Declare_V(&vector1, colN);
	free(matrix1.memory);
	free(matrix2.memory);
	free(vector1.memory);
	matrix1.memory=(int*)&arr1;
	matrix2.memory=(int*)&arr2;
	vector1.memory=(int*)&arr2;
	//Native approach
	//void WRITE_Matrix(int row, int col, int W[][col], int reg, int copy);
	printf("Native Matrix-Vector Multiplication-------------------------------------------------------\n");
	WRITE_Matrix(rowN, colN, arr1, 1, 1); //store the matrix
	WRITE_Matrix_Large(rowN, colN, 0, 0, 64, 64, arr1, 1, 1, 0); //copy=1 for 2d array //goes into preg 1
	WRITE_Matrix_Large(rowN, colN, 0, 1, 64, 64, arr1, 2, 1, 0); //copy=1 for 2d array //goes into preg 2
	WRITE_Matrix_Large(rowN, colN, 1, 0, 64, 64, arr1, 3, 1, 0); //copy=1 for 2d array //goes into preg 3
	WRITE_Matrix_Large(rowN, colN, 1, 1, 64, 64, arr1, 4, 1, 0); //copy=1 for 2d array //goes into preg 4
	WRITE_Matrix_Large(rowN, colN, 2, 0, 64, 64, arr1, 5, 1, 0); //copy=1 for 2d array //goes into preg 5
	WRITE_Matrix_Large(rowN, colN, 2, 1, 64, 64, arr1, 6, 1, 0); //copy=1 for 2d array //goes into preg 6
	WRITE_Matrix_Large(rowN, colN, 3, 0, 64, 64, arr1, 7, 1, 0); //copy=1 for 2d array //goes into preg 7
	WRITE_Matrix_Large(rowN, colN, 3, 1, 64, 64, arr1, 8, 1, 0); //copy=1 for 2d array //goes into preg 8

	execute(1, 9, 1, 1);
//	MATRIX_SUBTRACTION(1, 1, 9);
	execute(1, 10, 1, 1);
//	MATRIX_SUBTRACTION(1, 1, 10);
	WRITE_Matrix_Large(1, colN, 0, 0, 1, 64, arr2, 9, 1, 0); //goes into preg9
	//WRITE_Matrix_Large(int row, int col, int block_row, int block_col, int row_blk_size, int col_blk_size, int W[][col], int reg, int copy, int block_dimension)
	WRITE_Matrix_Large(1, colN, 0, 1, 1, 64, arr2, 10, 1, 0); //goes into preg10

	ResetCounts();
	//fill entire preg 9 and then 10 for vector
	MATRIX_SUBTRACTION(1, 1, 0);
	SHIFT_SOUTH(9, 0);
	for(int x=0; x<SPAR_dimension-1; x++)
	{
		MATRIX_ADDITION(9, 0, 9);
		SHIFT_SOUTH(0, 0);
	}
	MATRIX_SUBTRACTION(1, 1, 0);
	SHIFT_SOUTH(10, 0);
	for(int x=0; x<SPAR_dimension-1; x++)
	{
		MATRIX_ADDITION(10, 0, 10);
		SHIFT_SOUTH(0, 0);
	}


	//Multiply Vector and Matrix
	ELEMENTWISE_MULTIPLICATION(1, 9, 17);
	ELEMENTWISE_MULTIPLICATION(2, 10, 18);
	ELEMENTWISE_MULTIPLICATION(3, 9, 19);
	ELEMENTWISE_MULTIPLICATION(4, 10, 20);
	ELEMENTWISE_MULTIPLICATION(5, 9, 21);
	ELEMENTWISE_MULTIPLICATION(6, 10, 22);
	ELEMENTWISE_MULTIPLICATION(7, 9, 23);
	ELEMENTWISE_MULTIPLICATION(8, 10, 24);

	//accumulate
	SHIFT_WEST(17, 25);
	//move West Edge from 18 to 25
	WEST_COLUMN_MOVE(18, 25);
	SHIFT_WEST(18, 26);
	SHIFT_WEST(19, 27);
	//move West Edge from 20 to 27
	WEST_COLUMN_MOVE(20, 27);
	SHIFT_WEST(20, 28);
	SHIFT_WEST(21, 29);
	//move West Edge from 22 to 29
	WEST_COLUMN_MOVE(22, 29);
	SHIFT_WEST(22, 30);
	SHIFT_WEST(23, 31);
	//move West Edge from 24 to 31
	WEST_COLUMN_MOVE(24, 31);
	SHIFT_WEST(24, 0);

	MATRIX_ADDITION(17, 25, 17);
	MATRIX_ADDITION(18, 26, 18);
	MATRIX_ADDITION(19, 27, 19);
	MATRIX_ADDITION(20, 28, 20);
	MATRIX_ADDITION(21, 29, 21);
	MATRIX_ADDITION(22, 30, 22);
	MATRIX_ADDITION(23, 31, 23);
	MATRIX_ADDITION(24, 0, 24);
	for(int x=0; x<colN-2; x++)
	{
		SHIFT_WEST(25, 25);
		//move West Edge from 26 to 25
		WEST_COLUMN_MOVE(26, 25);
		SHIFT_WEST(26, 26);
		SHIFT_WEST(27, 27);
		//move West Edge from 28 to 27
		WEST_COLUMN_MOVE(28, 27);
		SHIFT_WEST(28, 28);
		SHIFT_WEST(29, 29);
		//move West Edge from 30 to 29
		WEST_COLUMN_MOVE(30, 29);
		SHIFT_WEST(30, 30);
		SHIFT_WEST(31, 31);
		//move West Edge from 0 to 31
		WEST_COLUMN_MOVE(0, 31);
		SHIFT_WEST(0, 0);
		//add into result
		MATRIX_ADDITION(17, 25, 17);
		MATRIX_ADDITION(18, 26, 18);
		MATRIX_ADDITION(19, 27, 19);
		MATRIX_ADDITION(20, 28, 20);
		MATRIX_ADDITION(21, 29, 21);
		MATRIX_ADDITION(22, 30, 22);
		MATRIX_ADDITION(23, 31, 23);
		MATRIX_ADDITION(24, 0, 24);
	}
	PrintCounts();
//	printPReg(17);

	XTime_GetTime(&tStart);
	for(int i=0; i<100; i++)
	{
		//fill entire preg 9 and then 10 for vector
		MATRIX_SUBTRACTION(1, 1, 0);
		SHIFT_SOUTH(9, 0);
		for(int x=0; x<SPAR_dimension-1; x++)
		{
			MATRIX_ADDITION(9, 0, 9);
			SHIFT_SOUTH(0, 0);
		}
		MATRIX_SUBTRACTION(1, 1, 0);
		SHIFT_SOUTH(10, 0);
		for(int x=0; x<SPAR_dimension-1; x++)
		{
			MATRIX_ADDITION(10, 0, 10);
			SHIFT_SOUTH(0, 0);
		}


		//Multiply Vector and Matrix
		ELEMENTWISE_MULTIPLICATION(1, 9, 17);
		ELEMENTWISE_MULTIPLICATION(2, 10, 18);
		ELEMENTWISE_MULTIPLICATION(3, 9, 19);
		ELEMENTWISE_MULTIPLICATION(4, 10, 20);
		ELEMENTWISE_MULTIPLICATION(5, 9, 21);
		ELEMENTWISE_MULTIPLICATION(6, 10, 22);
		ELEMENTWISE_MULTIPLICATION(7, 9, 23);
		ELEMENTWISE_MULTIPLICATION(8, 10, 24);

		//accumulate
		SHIFT_WEST(17, 25);
		//move West Edge from 18 to 25
		WEST_COLUMN_MOVE(18, 25);
		SHIFT_WEST(18, 26);
		SHIFT_WEST(19, 27);
		//move West Edge from 20 to 27
		WEST_COLUMN_MOVE(20, 27);
		SHIFT_WEST(20, 28);
		SHIFT_WEST(21, 29);
		//move West Edge from 22 to 29
		WEST_COLUMN_MOVE(22, 29);
		SHIFT_WEST(22, 30);
		SHIFT_WEST(23, 31);
		//move West Edge from 24 to 31
		WEST_COLUMN_MOVE(24, 31);
		SHIFT_WEST(24, 0);

		MATRIX_ADDITION(17, 25, 17);
		MATRIX_ADDITION(18, 26, 18);
		MATRIX_ADDITION(19, 27, 19);
		MATRIX_ADDITION(20, 28, 20);
		MATRIX_ADDITION(21, 29, 21);
		MATRIX_ADDITION(22, 30, 22);
		MATRIX_ADDITION(23, 31, 23);
		MATRIX_ADDITION(24, 0, 24);
		for(int x=0; x<colN-2; x++)
		{
			SHIFT_WEST(25, 25);
			//move West Edge from 26 to 25
			WEST_COLUMN_MOVE(26, 25);
			SHIFT_WEST(26, 26);
			SHIFT_WEST(27, 27);
			//move West Edge from 28 to 27
			WEST_COLUMN_MOVE(28, 27);
			SHIFT_WEST(28, 28);
			SHIFT_WEST(29, 29);
			//move West Edge from 30 to 29
			WEST_COLUMN_MOVE(30, 29);
			SHIFT_WEST(30, 30);
			SHIFT_WEST(31, 31);
			//move West Edge from 0 to 31
			WEST_COLUMN_MOVE(0, 31);
			SHIFT_WEST(0, 0);
			//add into result
			MATRIX_ADDITION(17, 25, 17);
			MATRIX_ADDITION(18, 26, 18);
			MATRIX_ADDITION(19, 27, 19);
			MATRIX_ADDITION(20, 28, 20);
			MATRIX_ADDITION(21, 29, 21);
			MATRIX_ADDITION(22, 30, 22);
			MATRIX_ADDITION(23, 31, 23);
			MATRIX_ADDITION(24, 0, 24);
		}
	}
	XTime_GetTime(&tEnd);
	ElapsedTime = (1.0 * (tEnd - tStart) / (COUNTS_PER_SECOND));

	printf("Matrix-Vector Multiplication x100 Time: %lf Seconds\n", ElapsedTime);
	printf("VM Matrix-Vector Multiplication-------------------------------------------------------\n"); //here
	Reset_Registers();
	resetTable(table);
	Store_M(&matrix1, 1, table);
	Store_V(&vector1, 2, table);
	Mul_MV(1,2,3, table); //run it once just to setup the data


	ResetCounts();
	printf("\n\n");
	Mul_MV(1,2,3, table); //here
	PrintCounts();

	XTime_GetTime(&tStart);
	for(int i=0; i<100; i++)
	{
		Mul_MV(1,2,3, table); //here
	}
	XTime_GetTime(&tEnd);
	ElapsedTime = (1.0 * (tEnd - tStart) / (COUNTS_PER_SECOND));
	printf("Matrix-Vector Multiplication x100 Time: %lf Seconds\n", ElapsedTime);
	return 1;
}
int Test_MulAcc_8Segment_T(AllocationTable *table){
	printf("\n8 Segment MV Multiply Accumulate\n");
	//time variable
	XTime tStart, tEnd;
	double ElapsedTime;

	//define sample data
	int rowN = 128;
	int colN = 256;
	int arr1[rowN][colN];
	int arr2[rowN][colN];
	for(int i=0; i < rowN; i++)
	{
		for(int j=0; j < colN; j++)
		{
			arr1[i][j] = 2*j+256+(4*i);
			arr2[i][j] = (2)<<14;
		}
	}
	Matrix matrix1, matrix2;
	Vector vector1;
	Declare_M(&matrix1, rowN, colN);
	Declare_M(&matrix2, rowN, colN);
	Declare_V(&vector1, colN);
	free(matrix1.memory);
	free(matrix2.memory);
	free(vector1.memory);
	matrix1.memory=(int*)&arr1;
	matrix2.memory=(int*)&arr2;
	vector1.memory=(int*)&arr2;
	//Native approach
	//void WRITE_Matrix(int row, int col, int W[][col], int reg, int copy);
	printf("Native Matrix-Vector Multiplication-------------------------------------------------------\n");
	WRITE_Matrix_Large(rowN, colN, 0, 0, 64, 64, arr1, 1, 1, 0); //copy=1 for 2d array //goes into preg 1
	WRITE_Matrix_Large(rowN, colN, 0, 1, 64, 64, arr1, 2, 1, 0); //copy=1 for 2d array //goes into preg 2
	WRITE_Matrix_Large(rowN, colN, 0, 2, 64, 64, arr1, 3, 1, 0); //copy=1 for 2d array //goes into preg 3
	WRITE_Matrix_Large(rowN, colN, 0, 3, 64, 64, arr1, 4, 1, 0); //copy=1 for 2d array //goes into preg 4
	WRITE_Matrix_Large(rowN, colN, 1, 0, 64, 64, arr1, 5, 1, 0); //copy=1 for 2d array //goes into preg 5
	WRITE_Matrix_Large(rowN, colN, 1, 1, 64, 64, arr1, 6, 1, 0); //copy=1 for 2d array //goes into preg 6
	WRITE_Matrix_Large(rowN, colN, 1, 2, 64, 64, arr1, 7, 1, 0); //copy=1 for 2d array //goes into preg 7
	WRITE_Matrix_Large(rowN, colN, 1, 3, 64, 64, arr1, 8, 1, 0); //copy=1 for 2d array //goes into preg 8

	execute(1, 9, 1, 1);
	execute(1, 10, 1, 1);
	execute(1, 11, 1, 1);
	execute(1, 12, 1, 1);
	WRITE_Matrix_Large(1, colN, 0, 0, 1, 64, arr2, 9, 1, 0); //goes into preg9
	//WRITE_Matrix_Large(int row, int col, int block_row, int block_col, int row_blk_size, int col_blk_size, int W[][col], int reg, int copy, int block_dimension)
	WRITE_Matrix_Large(1, colN, 0, 1, 1, 64, arr2, 10, 1, 0); //goes into preg10
	WRITE_Matrix_Large(1, colN, 0, 2, 1, 64, arr2, 11, 1, 0); //goes into preg11
	WRITE_Matrix_Large(1, colN, 0, 3, 1, 64, arr2, 12, 1, 0); //goes into preg12

	ResetCounts();
	//fill entire preg 9 and then 10,11,12 for vector
	MATRIX_SUBTRACTION(1, 1, 0);
	SHIFT_SOUTH(9, 0);
	for(int x=0; x<SPAR_dimension-1; x++)
	{
		MATRIX_ADDITION(9, 0, 9);
		SHIFT_SOUTH(0, 0);
	}
	MATRIX_SUBTRACTION(1, 1, 0);
	SHIFT_SOUTH(10, 0);
	for(int x=0; x<SPAR_dimension-1; x++)
	{
		MATRIX_ADDITION(10, 0, 10);
		SHIFT_SOUTH(0, 0);
	}
	MATRIX_SUBTRACTION(1, 1, 0);
	SHIFT_SOUTH(11, 0);
	for(int x=0; x<SPAR_dimension-1; x++)
	{
		MATRIX_ADDITION(11, 0, 11);
		SHIFT_SOUTH(0, 0);
	}
	MATRIX_SUBTRACTION(1, 1, 0);
	SHIFT_SOUTH(12, 0);
	for(int x=0; x<SPAR_dimension-1; x++)
	{
		MATRIX_ADDITION(12, 0, 12);
		SHIFT_SOUTH(0, 0);
	}



	//Multiply Vector and Matrix
	ELEMENTWISE_MULTIPLICATION(1, 9, 17);
	ELEMENTWISE_MULTIPLICATION(2, 10, 18);
	ELEMENTWISE_MULTIPLICATION(3, 11, 19);
	ELEMENTWISE_MULTIPLICATION(4, 12, 20);
	ELEMENTWISE_MULTIPLICATION(5, 9, 21);
	ELEMENTWISE_MULTIPLICATION(6, 10, 22);
	ELEMENTWISE_MULTIPLICATION(7, 11, 23);
	ELEMENTWISE_MULTIPLICATION(8, 12, 24);

	//accumulate
	SHIFT_WEST(17, 25);
	WEST_COLUMN_MOVE(18, 25);
	SHIFT_WEST(18, 26);
	WEST_COLUMN_MOVE(19, 26);
	SHIFT_WEST(19, 27);
	WEST_COLUMN_MOVE(20, 27);
	SHIFT_WEST(20, 28);
	SHIFT_WEST(21, 29);
	WEST_COLUMN_MOVE(22,29);
	SHIFT_WEST(22, 30);
	WEST_COLUMN_MOVE(23,29);
	SHIFT_WEST(23, 31);
	WEST_COLUMN_MOVE(24,31);
	SHIFT_WEST(24, 0);

	MATRIX_ADDITION(17, 25, 17);
	MATRIX_ADDITION(18, 26, 18);
	MATRIX_ADDITION(19, 27, 19);
	MATRIX_ADDITION(20, 28, 20);
	MATRIX_ADDITION(21, 29, 21);
	MATRIX_ADDITION(22, 30, 22);
	MATRIX_ADDITION(23, 31, 23);
	MATRIX_ADDITION(24, 0, 24);
	for(int x=0; x<colN-2; x++)
	{
		SHIFT_WEST(25, 25);
		WEST_COLUMN_MOVE(26, 25);
		SHIFT_WEST(26, 26);
		WEST_COLUMN_MOVE(27,26);
		SHIFT_WEST(27, 27);
		WEST_COLUMN_MOVE(28, 27);
		SHIFT_WEST(28, 28);
		SHIFT_WEST(29, 29);
		WEST_COLUMN_MOVE(30, 29);
		SHIFT_WEST(30, 30);
		WEST_COLUMN_MOVE(31,30);
		SHIFT_WEST(31, 31);
		WEST_COLUMN_MOVE(0, 31);
		SHIFT_WEST(0, 0);
		//add into result
		MATRIX_ADDITION(17, 25, 17);
		MATRIX_ADDITION(18, 26, 18);
		MATRIX_ADDITION(19, 27, 19);
		MATRIX_ADDITION(20, 28, 20);
		MATRIX_ADDITION(21, 29, 21);
		MATRIX_ADDITION(22, 30, 22);
		MATRIX_ADDITION(23, 31, 23);
		MATRIX_ADDITION(24, 0, 24);
	}
	PrintCounts();

	XTime_GetTime(&tStart);
	for(int i=0; i<100; i++)
	{
		//fill entire preg 9 and then 10,11,12 for vector
		MATRIX_SUBTRACTION(1, 1, 0);
		SHIFT_SOUTH(9, 0);
		for(int x=0; x<SPAR_dimension-1; x++)
		{
			MATRIX_ADDITION(9, 0, 9);
			SHIFT_SOUTH(0, 0);
		}
		MATRIX_SUBTRACTION(1, 1, 0);
		SHIFT_SOUTH(10, 0);
		for(int x=0; x<SPAR_dimension-1; x++)
		{
			MATRIX_ADDITION(10, 0, 10);
			SHIFT_SOUTH(0, 0);
		}
		MATRIX_SUBTRACTION(1, 1, 0);
		SHIFT_SOUTH(11, 0);
		for(int x=0; x<SPAR_dimension-1; x++)
		{
			MATRIX_ADDITION(11, 0, 11);
			SHIFT_SOUTH(0, 0);
		}
		MATRIX_SUBTRACTION(1, 1, 0);
		SHIFT_SOUTH(12, 0);
		for(int x=0; x<SPAR_dimension-1; x++)
		{
			MATRIX_ADDITION(12, 0, 12);
			SHIFT_SOUTH(0, 0);
		}



		//Multiply Vector and Matrix
		ELEMENTWISE_MULTIPLICATION(1, 9, 17);
		ELEMENTWISE_MULTIPLICATION(2, 10, 18);
		ELEMENTWISE_MULTIPLICATION(3, 11, 19);
		ELEMENTWISE_MULTIPLICATION(4, 12, 20);
		ELEMENTWISE_MULTIPLICATION(5, 9, 21);
		ELEMENTWISE_MULTIPLICATION(6, 10, 22);
		ELEMENTWISE_MULTIPLICATION(7, 11, 23);
		ELEMENTWISE_MULTIPLICATION(8, 12, 24);

		//accumulate
		SHIFT_WEST(17, 25);
		WEST_COLUMN_MOVE(18, 25);
		SHIFT_WEST(18, 26);
		WEST_COLUMN_MOVE(19, 26);
		SHIFT_WEST(19, 27);
		WEST_COLUMN_MOVE(20, 27);
		SHIFT_WEST(20, 28);
		SHIFT_WEST(21, 29);
		WEST_COLUMN_MOVE(22,29);
		SHIFT_WEST(22, 30);
		WEST_COLUMN_MOVE(23,29);
		SHIFT_WEST(23, 31);
		WEST_COLUMN_MOVE(24,31);
		SHIFT_WEST(24, 0);

		MATRIX_ADDITION(17, 25, 17);
		MATRIX_ADDITION(18, 26, 18);
		MATRIX_ADDITION(19, 27, 19);
		MATRIX_ADDITION(20, 28, 20);
		MATRIX_ADDITION(21, 29, 21);
		MATRIX_ADDITION(22, 30, 22);
		MATRIX_ADDITION(23, 31, 23);
		MATRIX_ADDITION(24, 0, 24);
		for(int x=0; x<colN-2; x++)
		{
			SHIFT_WEST(25, 25);
			WEST_COLUMN_MOVE(26, 25);
			SHIFT_WEST(26, 26);
			WEST_COLUMN_MOVE(27,26);
			SHIFT_WEST(27, 27);
			WEST_COLUMN_MOVE(28, 27);
			SHIFT_WEST(28, 28);
			SHIFT_WEST(29, 29);
			WEST_COLUMN_MOVE(30, 29);
			SHIFT_WEST(30, 30);
			WEST_COLUMN_MOVE(31,30);
			SHIFT_WEST(31, 31);
			WEST_COLUMN_MOVE(0, 31);
			SHIFT_WEST(0, 0);
			//add into result
			MATRIX_ADDITION(17, 25, 17);
			MATRIX_ADDITION(18, 26, 18);
			MATRIX_ADDITION(19, 27, 19);
			MATRIX_ADDITION(20, 28, 20);
			MATRIX_ADDITION(21, 29, 21);
			MATRIX_ADDITION(22, 30, 22);
			MATRIX_ADDITION(23, 31, 23);
			MATRIX_ADDITION(24, 0, 24);
		}
	}
	XTime_GetTime(&tEnd);
	ElapsedTime = (1.0 * (tEnd - tStart) / (COUNTS_PER_SECOND));

	printf("Matrix-Vector Multiplication x100 Time: %lf Seconds\n", ElapsedTime);
	printf("VM Matrix-Vector Multiplication-------------------------------------------------------\n"); //here
	Reset_Registers();
	resetTable(table);
	Store_M(&matrix1, 1, table);
	Store_V(&vector1, 2, table);
	Mul_MV(1,2,3, table); //run it once just to setup the data


	ResetCounts();
	printf("\n\n");
	Mul_MV(1,2,3, table); //here
	PrintCounts();

	XTime_GetTime(&tStart);
	for(int i=0; i<100; i++)
	{
		Mul_MV(1,2,3, table); //here
	}
	XTime_GetTime(&tEnd);
	ElapsedTime = (1.0 * (tEnd - tStart) / (COUNTS_PER_SECOND));
	printf("Matrix-Vector Multiplication x100 Time: %lf Seconds\n", ElapsedTime);
	return 1;
}
