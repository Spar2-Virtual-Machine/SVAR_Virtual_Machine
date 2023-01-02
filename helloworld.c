/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design*
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include "xil_printf.h"
#include <stdlib.h>
#include <stdint.h>
#include "platform.h"
#include "Bit_Serial.h"
#include "xparameters.h"
#include <inttypes.h>
#include "xil_types.h"
#include "xil_io.h"
#include "math.h"
#include "sleep.h"
#include "xtime_l.h"
//custom library
#include "spar.h"
#include "AllocationTable.h"
#include "virt.h"

#define MAX_LEN 32
#define FIXED_POINT_FRACTIONAL_BITS 16


#define divide_size_W 4
#define divide_size_U 4
#define m1 4						//size of hidden layers
#define p 4							//size of inputs
#define q 1							//size of outputs
#define epoch 1

#define colN 40
#define rowN 40


//Function prototypes
int execute(int opcode, int rd, int rs1, int rs2);
int readRAM(int address);
int getbit(int n, int k);
void getTranspose(int* ram, int* reg);
void printArray(int* arr, int size);
//void getData(int* arr, int address);
unsigned char Test(int64_t* rd_reg, int64_t* rs1_reg, int64_t* rs2_reg, char operation);
void WRITE(int Tile_i, int Tile_j, int BRAM_i, int BRAM_j, int ADDRA, int ADDRB, int DIA, int DIB);
int READ(int Tile_i, int Tile_j, int BRAM_i, int BRAM_j, int ADDRA, int ADDRB);
int printReg(int Tile_i, int Tile_j, int BRAM_i, int BRAM_j, int PE, int reg);
int getTrans(int* ram);
void printRegFile(int Tile_i, int Tile_j, int BRAM_i, int BRAM_j, int number_of_regs);
void WRITE_REG(int Tile_i, int Tile_j, int BRAM_i, int BRAM_j, int PE, int reg, unsigned int data);
unsigned int READ_REG(int Tile_i, int Tile_j, int BRAM_i, int BRAM_j, int PE, int reg);

//Allocation Table for the VM
static AllocationTable allocation_table;

void printPReg(int reg){
	int data = 0;
	for(int x=0; x<Array_dim; x++)
	{
		for(int y=0; y<Tile_dim; y++)
		{
			for(int pe=0; pe<16; pe+=4)
			{
				for(int i=0; i<Array_dim; i++)
				{
					for(int j=0; j<Tile_dim; j++)
					{
						data = READ_REG(i, x, j, y, pe, reg);
						printf("%d, ", data);
						data = READ_REG(i, x, j, y, pe+1, reg);
						printf("%d, ", data);
						data = READ_REG(i, x, j, y, pe+2, reg);
						printf("%d, ", data);
						data = READ_REG(i, x, j, y, pe+3, reg);
						printf("%d, ", data);
					}
				}
				printf("\n");
			}
		}
	}
}

int main()
{
	init_platform();
	cleanup_platform();
	//reset all the registers in the array to zero
	Reset_Registers();
	xil_printf("RESET REGISTERS IS DONE! \n");

	//setup allocation table
	resetTable(&allocation_table);
	printTableVReg(&allocation_table);

	printf("-------------------------------------------------------------------------\r\n");
//	printRegFile(0, 0, 0, 0, 32);

	int arr1[rowN][colN];
//	xil_printf("arr1%p\n", arr1);
	int arr2[rowN][colN];
//	xil_printf("arr2%p\n", arr2);
//	int arr3[rowN][colN];
//	int arr4[rowN][colN];
	//fill the arrays
	xil_printf("%p\n", allocation_table);
	for(int i=0; i < rowN; i++)
	{
		for(int j=0; j < colN; j++)
		{
			arr1[i][j] = j+i;
			arr2[i][j] = 0x00010000;
		}
	}
	xil_printf("here1\n");

	//setup the matrix
	Matrix m, n;
	Declare_M(&m, rowN, colN);
	m.memory = (int*)&arr1; //did not free memory, but I'm not being efficient here anyways
	Declare_M(&n, rowN, colN);
	n.memory = (int*)&arr2;
	xil_printf("here2\n");

	//"Store" the matrix into a virtual register (actually just allocate it to a virtual register in the allocation table)
	Store_M(&m, 1, &allocation_table);
	Store_M(&n, 2, &allocation_table);
	Store_M(&m, 3, &allocation_table);
	xil_printf("here3\n\n\n");

	//void safeAllocatePRegs(int vRegNum, int maxDim, int protectedVReg[], int numProtected, AllocationTable *table)
	int protectV[] = {1};
	safeAllocatePRegs(1, 24, protectV, 0, &allocation_table); //allocate pregs and move data to SPAR
	safeAllocatePRegs(2, 24, protectV, 0, &allocation_table);

	for(int a=0; a < rowN; a++)
	{
		for(int b=0; b < colN; b++)
		{
			printf("%.2f,", (float)n.memory[a*(n.cols)+b]/(1<<16));
		}
		usleep_A53(100);
		printf("\n");
	}

	//print allocation table vreg
	printTableVReg(&allocation_table);
	printTablePReg(&allocation_table);

	printRegFile(0,0,0,0,32);
    return 0;
}
