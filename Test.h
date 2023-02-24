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

int TestFillVector(AllocationTable *table);
int TestFillVectorTurned(AllocationTable *table);
int TestFillVectorMixedOrientation(AllocationTable *table);
int ConvertMatrixToVectorTest(AllocationTable *table);
int ConvertMatrixToVectorTurnedTest(AllocationTable *table);
#endif /* SRC_TEST_H_ */
