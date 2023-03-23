/*
 * MLP_Benchmarks.c
 *
 *  Created on: Mar 22, 2023
 *      Author: Njfre
 */
#include "MLP_Benchmarks.h"

#define inputSize 100
#define l1Nodes 150
#define l2Nodes 100

extern inline void ResetCounts_m(){
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

extern inline void PrintCounts_m(){
	printf("Add Count: %d \n Sub Count: %d \n Mul Count %d\n", sparAdd_cnt, sparSub_cnt, sparMul_cnt);
	printf("North Count: %d \n South Count: %d \n East Count: %d \n West Count: %d\n", sparShN_cnt, sparShS_cnt, sparShE_cnt, sparShW_cnt);
	printf("Write Count: %d \n Read Count: %d\n", sparWR_cnt, sparRD_cnt);
}

void PopulateData(int input[inputSize], int w1[l1Nodes][inputSize], int b1[l1Nodes], int w2[l2Nodes][l1Nodes], int b2[l2Nodes]){
	for(int i=0; i<inputSize; i++)
	{
		input[i] = (2)<<16;
	}

	for(int i=0; i<l1Nodes; i++)
	{
		for(int j=0; j<inputSize; j++)
		{
			w1[i][j]=(1<<14);
		}
		b1[i]=0;
	}

	for(int i=0; i<l2Nodes; i++)
	{
		for(int j=0; j<l1Nodes; j++)
		{
			w2[i][j]=(1<<10);
		}
		b2[i]=1;
	}
}

void MLP_VM_For64x64_Count(AllocationTable *table){
	int input[inputSize];
	int weight1[l1Nodes][inputSize];
	int bias1[l1Nodes];
	int weight2[l2Nodes][l1Nodes];
	int bias2[l2Nodes];
	Vector in, b1, b2;
	Matrix w1, w2;
	PopulateData(input, weight1, bias1, weight2, bias2);
	Declare_V(&in, inputSize);
	Set_V_Data(&in, input);
	Declare_V(&b1, l1Nodes);
	Set_V_Data(&b1, bias1);
	Declare_V(&b2, l2Nodes);
	Set_V_Data(&b2, bias2);
	Declare_M(&w1, l1Nodes, inputSize);
	Set_M_Data(&w1, (int *)&weight1);
	Declare_M(&w2, l2Nodes, l1Nodes);
	Set_M_Data(&w2, (int *)&weight2);


	ResetCounts_m();
	Store_V(&in, 2, table);
	Store_M(&w1, 1, table);
	Mul_MV(1, 2, 3, table);

	Store_V(&b1, 4, table);
	E_Add_VV(3, 4, 3, table);

	Store_M(&w2, 5, table);
	Mul_MV(5, 3, 6, table);
	Store_V(&b2, 7, table);
	E_Add_VV(6, 7, 6, table);
//	PrintCounts_m();
}

void MLP_Native_For64x64_Count(AllocationTable *table){
	Reset_Registers();
	int input[inputSize];
	int weight1[l1Nodes][inputSize];
	int bias1[l1Nodes];
	int weight2[l2Nodes][l1Nodes];
	int bias2[l2Nodes];
	PopulateData(input, weight1, bias1, weight2, bias2);

	//write Matrix Into Memory
	WRITE_Matrix_Large(l1Nodes, inputSize, 0, 0, 64, 64, weight1, 1, 1, 0); //copy=1 for 2d array //goes into preg 1
	WRITE_Matrix_Large(l1Nodes, inputSize, 0, 1, 64, 64, weight1, 2, 1, 0); //copy=1 for 2d array //goes into preg 2
	WRITE_Matrix_Large(l1Nodes, inputSize, 1, 0, 64, 64, weight1, 3, 1, 0); //copy=1 for 2d array //goes into preg 3
	WRITE_Matrix_Large(l1Nodes, inputSize, 1, 1, 64, 64, weight1, 4, 1, 0); //copy=1 for 2d array //goes into preg 4
	WRITE_Matrix_Large(l1Nodes, inputSize, 2, 0, 64, 64, weight1, 5, 1, 0); //copy=1 for 2d array //goes into preg 5
	WRITE_Matrix_Large(l1Nodes, inputSize, 2, 1, 64, 64, weight1, 6, 1, 0); //copy=1 for 2d array //goes into preg 6

	//write Vector into Memory
	WRITE_Matrix_Large(1, inputSize, 0, 0, 1, 64, &input, 9, 1, 0);
	WRITE_Matrix_Large(1, inputSize, 0, 1, 1, 64, &input, 10, 1, 0);

	//Fill the PRegs for each vector
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

	//Multiply the vector and matrix segments
	ELEMENTWISE_MULTIPLICATION(1, 9, 17);
	ELEMENTWISE_MULTIPLICATION(2, 10, 18);
	ELEMENTWISE_MULTIPLICATION(3, 9, 19);
	ELEMENTWISE_MULTIPLICATION(4, 10, 20);
	ELEMENTWISE_MULTIPLICATION(5, 9, 21);
	ELEMENTWISE_MULTIPLICATION(6, 10, 22);


	//Accumulate results
	SHIFT_WEST(17, 25);
	WEST_COLUMN_MOVE(18, 25);
	SHIFT_WEST(18, 26);
	SHIFT_WEST(19, 27);
	WEST_COLUMN_MOVE(20, 27);
	SHIFT_WEST(20, 28);
	SHIFT_WEST(21, 29);
	WEST_COLUMN_MOVE(22, 29);
	SHIFT_WEST(22, 30);

	MATRIX_ADDITION(17, 25, 17);
	MATRIX_ADDITION(18, 26, 18);
	MATRIX_ADDITION(19, 27, 19);
	MATRIX_ADDITION(20, 28, 20);
	MATRIX_ADDITION(21, 29, 21);
	MATRIX_ADDITION(22, 30, 22);
	for(int i=0; i<inputSize-2; i++)
	{
		SHIFT_WEST(25, 25);
		WEST_COLUMN_MOVE(26, 25);
		SHIFT_WEST(26, 26);
		SHIFT_WEST(27, 27);
		WEST_COLUMN_MOVE(28, 27);
		SHIFT_WEST(28, 28);
		SHIFT_WEST(29, 29);
		WEST_COLUMN_MOVE(30, 29);
		SHIFT_WEST(30, 30);
		MATRIX_ADDITION(17, 25, 17);
		MATRIX_ADDITION(18, 26, 18);
		MATRIX_ADDITION(19, 27, 19);
		MATRIX_ADDITION(20, 28, 20);
		MATRIX_ADDITION(21, 29, 21);
		MATRIX_ADDITION(22, 30, 22);
	}

	//load in bias vector in
	//					row,   col, br, bc, brs, bcs, data, reg, copy, block dimension
	WRITE_Matrix_Large(l1Nodes, 1, 0, 0, 64, 1, &bias1, 1, 1, 0);
	WRITE_Matrix_Large(l1Nodes, 1, 1, 0, 64, 1, &bias1, 2, 1, 0);
	WRITE_Matrix_Large(l1Nodes, 1, 2, 0, 64, 1, &bias1, 3, 1, 0);

	//Add the bias (current vector is in 17, 19, and 21)
	MATRIX_ADDITION(1, 17, 17);
	MATRIX_ADDITION(2, 19, 19);
	MATRIX_ADDITION(3, 21, 21);

	//clear all but the first column
	SHIFT_WEST(17, 25);
	SHIFT_EAST(25, 25);
	MATRIX_SUBTRACTION(17, 25, 17);
	SHIFT_WEST(19, 25);
	SHIFT_EAST(25, 25);
	MATRIX_SUBTRACTION(19, 25, 19);
	SHIFT_WEST(21, 25);
	SHIFT_EAST(25, 25);
	MATRIX_SUBTRACTION(21, 25, 21);

	//fill all 3 registers for the vectors
	MATRIX_SUBTRACTION(1, 1, 0);
	SHIFT_EAST(17, 0);
	for(int x=0; x<SPAR_dimension-1; x++)
	{
		MATRIX_ADDITION(17, 0, 17);
		SHIFT_EAST(0, 0);
	}
	MATRIX_SUBTRACTION(1, 1, 0);
	SHIFT_EAST(19, 0);
	for(int x=0; x<SPAR_dimension-1; x++)
	{
		MATRIX_ADDITION(19, 0, 19);
		SHIFT_EAST(0, 0);
	}
	MATRIX_SUBTRACTION(1, 1, 0);
	SHIFT_EAST(21, 0);
	for(int x=0; x<SPAR_dimension-1; x++)
	{
		MATRIX_ADDITION(21, 0, 21);
		SHIFT_EAST(0, 0);
	}
	//load in the newest weight matrix
	WRITE_Matrix_Large(l2Nodes, l1Nodes, 0, 0, 64, 64, weight2, 1, 2, 0); //copy=2 for 2d array inverted//goes into preg 1
	WRITE_Matrix_Large(l2Nodes, l1Nodes, 0, 1, 64, 64, weight2, 2, 2, 0); //copy=2 for 2d array inverted//goes into preg 2
	WRITE_Matrix_Large(l2Nodes, l1Nodes, 0, 2, 64, 64, weight2, 3, 2, 0); //copy=2 for 2d array inverted//goes into preg 3
	WRITE_Matrix_Large(l2Nodes, l1Nodes, 1, 0, 64, 64, weight2, 4, 2, 0); //copy=2 for 2d array inverted//goes into preg 4
	WRITE_Matrix_Large(l2Nodes, l1Nodes, 1, 1, 64, 64, weight2, 5, 2, 0); //copy=2 for 2d array inverted//goes into preg 5
	WRITE_Matrix_Large(l2Nodes, l1Nodes, 1, 2, 64, 64, weight2, 6, 2, 0); //copy=2 for 2d array inverted //goes into preg 6

	//Multiply the vector and matrix segments
	ELEMENTWISE_MULTIPLICATION(1, 17, 25);
	ELEMENTWISE_MULTIPLICATION(2, 19, 26);
	ELEMENTWISE_MULTIPLICATION(3, 21, 27);
	ELEMENTWISE_MULTIPLICATION(4, 17, 28);
	ELEMENTWISE_MULTIPLICATION(5, 19, 29);
	ELEMENTWISE_MULTIPLICATION(6, 21, 30);

	//Accumulate products
	SHIFT_NORTH(25, 1);
	NORTH_COLUMN_MOVE(26, 1);
	SHIFT_NORTH(26, 2);
	NORTH_COLUMN_MOVE(27, 2);
	SHIFT_NORTH(27, 3);
	SHIFT_NORTH(28, 4);
	NORTH_COLUMN_MOVE(29, 4);
	SHIFT_NORTH(29, 5);
	NORTH_COLUMN_MOVE(30, 5);
	SHIFT_NORTH(30, 6);
	MATRIX_ADDITION(1, 25, 25);
	MATRIX_ADDITION(2, 26, 26);
	MATRIX_ADDITION(3, 27, 27);
	MATRIX_ADDITION(4, 28, 28);
	MATRIX_ADDITION(5, 29, 29);
	MATRIX_ADDITION(6, 30, 30);
	for(int i=0; i<l1Nodes-2; i++)//inputSize-2; i++)
	{
		SHIFT_NORTH(1, 1);
		NORTH_COLUMN_MOVE(2, 1);
		SHIFT_NORTH(2, 2);
		NORTH_COLUMN_MOVE(3, 2);
		SHIFT_NORTH(3, 3);
		SHIFT_NORTH(4, 4);
		NORTH_COLUMN_MOVE(5, 4);
		SHIFT_NORTH(5, 5);
		NORTH_COLUMN_MOVE(6, 5);
		SHIFT_NORTH(6, 6);
		MATRIX_ADDITION(1, 25, 25);
		MATRIX_ADDITION(2, 26, 26);
		MATRIX_ADDITION(3, 27, 27);
		MATRIX_ADDITION(4, 28, 28);
		MATRIX_ADDITION(5, 29, 29);
		MATRIX_ADDITION(6, 30, 30);
	}

	//Load bias vector
	WRITE_Matrix_Large(1, l2Nodes, 0, 0, 1, 64, &bias2, 1, 1, 0);
	WRITE_Matrix_Large(1, l2Nodes, 0, 0, 1, 64, &bias2, 2, 1, 0);
	//Add bias to current vector
	MATRIX_ADDITION(1, 25, 25);
	MATRIX_ADDITION(2, 28, 28);
	printPReg(25);
	printPReg(28);
}
