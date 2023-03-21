/*
 * Test.h
 *
 *  Created on: Feb 19, 2023
 *      Author: Njfre
 */

#ifndef SRC_TEST_H_
#define SRC_TEST_H_

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
#include "xtime_l.h"

void MultiplyMV(int matrix[], int vector[], int rows, int cols, int result[]);
int TestShiftNorth(AllocationTable *table);
int TestShiftSouth(AllocationTable *table);
int TestShiftEast(AllocationTable *table);
int TestShiftWest(AllocationTable *table);
int TestTurnedColumnAccumulation(AllocationTable *table);
int TestFillVector(AllocationTable *table);
int TestFillVectorTurned(AllocationTable *table);
int TestFillVectorMixedOrientation(AllocationTable *table);
int AccumulateTest(AllocationTable * table);
int ConvertMatrixToVectorTest(AllocationTable *table);
int ConvertMatrixToVectorTurnedTest(AllocationTable *table);
int MultiplyAccumulateMatrixVectorSetupTest(AllocationTable *table);
int MultiplyAccumulateMatrixVectorTest(AllocationTable *table);

int MLP_Benchmark(AllocationTable *table);

int Test_Elementwise_1Segment(AllocationTable *table);
int Test_Elementwise_2Segment(AllocationTable *table);
int Test_Elementwise_4Segment(AllocationTable *table);
int Test_Elementwise_8Segment(AllocationTable *table);

int Test_MulAcc_1Segment(AllocationTable *table);
int Test_MulAcc_2Segment(AllocationTable *table);
int Test_MulAcc_4Segment(AllocationTable *table);
int Test_MulAcc_8Segment(AllocationTable *table);

#endif /* SRC_TEST_H_ */
