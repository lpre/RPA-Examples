/*
 * RPA - Tool for Rocket Propulsion Analysis
 * RPA wrapper library
 *
 * Copyright 2009,2014 Alexander Ponomarenko.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This software is a commercial product; you can use it either
 * under the terms of the valid RPA SDK License as published
 * at http://www.propulsion-analysis.com/sdk_eula.htm
 * or as a complementary tool under the terms of valid RPA Standard Edition License
 * as published at http://www.propulsion-analysis.com/lic_standard.htm
 * or RPA Lite Edition License as published at http://www.propulsion-analysis.com/lic_lite.htm
 *
 * This program is distributed WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the applicable RPA SDK License, RPA Standard Edition License or
 * RPA Lite Edition License for more details.
 *
 * You should have received a valid copy of the RPA SDK License, RPA Standard Edition License or
 * RPA Lite Edition License along with this program; if not, write to author <contact@propulsion-analysis.com>
 *
 * Please contact author <contact@propulsion-analysis.com> or visit http://www.propulsion-analysis.com
 * if you need additional information or have any questions.
 */


/*
 * This example demonstrates how to use the RPA wrapper within C/C++ programs.
 *
 * Compilation on Linux:
 *
 * make -f Makefile_gcc
 *
 * Compilation on Windows:
 *
 * make -f Makefile_win
 *
 */

#include "../include/Wrapper.h"
#include "../include/ConfigFile.h"
#include "../include/Performance.h"

#ifdef __cplusplus
extern "C" {
#endif

int main(int argc, char* argv[]) {
	initializeWithPath("../resources", 0, 1);

	void* c = configFileLoad("RD-170.cfg");

	void* p = performanceCreate(c, 0, 0);

	performanceSolve(p, 0);
	performancePrint(p);

	performanceDelete(p);
	configFileDelete(c);

	finalize();
}

#ifdef __cplusplus
}
#endif
