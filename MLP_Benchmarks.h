#ifndef __MLP_BENCHMARKS_H_
#define __MLP_BENCHMARKS_H_
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

void MLP_VM_For64x64_Count(AllocationTable *table);
void MLP_VM_For64x64_Time(AllocationTable *table);
void MLP_Native_For64x64_Count(AllocationTable *table);
void MLP_Native_For64x64_Time(AllocationTable *table);
#endif
