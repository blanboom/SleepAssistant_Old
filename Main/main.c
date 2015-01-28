/**
 * SleepAssistant
 */

#include <stdio.h>
#include "diag/Trace.h"
#include "Motion.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

int main(int argc, char* argv[])
{
	motionInit();
	while (1)
	{
		trace_printf("%d\n", detectMove());
	}
}

#pragma GCC diagnostic pop
