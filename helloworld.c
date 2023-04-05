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
#include "Test.h"
#include "MLP_Benchmarks.h"

#define MAX_LEN 32
#define FIXED_POINT_FRACTIONAL_BITS 16


#define divide_size_W 4
#define divide_size_U 4
#define m1 4						//size of hidden layers
#define p 4							//size of inputs
#define q 1							//size of outputs
#define epoch 1

#define colN 41
#define rowN 40


//Allocation Table for the VM
static AllocationTable allocation_table;



int main()
{
	init_platform();
	cleanup_platform();
	//reset all the registers in the array to zero
	resetTable(&allocation_table);
	Reset_Registers();
	xil_printf("RESET REGISTERS IS DONE! \n");
	printf("-------------------------------------------------------------------------\r\n");
//	Reset_Registers();
//	Test_Elementwise_1Segment(&allocation_table);
//	Reset_Registers();
//	Test_Elementwise_2Segment(&allocation_table);
//	Reset_Registers();
//	Test_Elementwise_4Segment(&allocation_table);
//	Reset_Registers();
//	Test_Elementwise_8Segment(&allocation_table);
//	Reset_Registers();
//	Test_MulAcc_1Segment(&allocation_table);
//	Reset_Registers();
//	Test_MulAcc_2Segment(&allocation_table);
//	Reset_Registers();
//	Test_MulAcc_2Segment_T(&allocation_table);
//	Reset_Registers();
//	Test_MulAcc_4Segment(&allocation_table);
//	Reset_Registers();
//	Test_MulAcc_8Segment(&allocation_table);
//	Reset_Registers();
//	Test_MulAcc_8Segment_T(&allocation_table);

//	TestRELU2(&allocation_table);
//	Reset_Registers();
//	MLP_Native_For64x64_Count(&allocation_table);
//	Reset_Registers();
//	MLP_Native_For64x64_Time(&allocation_table);
//	Reset_Registers();
//	MLP_VM_For64x64_Count(&allocation_table);
//	Reset_Registers();
//	MLP_VM_For64x64_Time(&allocation_table);
	Test_RELU_1Segments(&allocation_table);
	return 0;
}
