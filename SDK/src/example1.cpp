/*
 * RPA - Tool for Rocket Propulsion Analysis
 * RPA Software Development Kit (SDK)
 *
 * Copyright 2009,2015 Alexander Ponomarenko.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This software is a commercial product; you can use it under the terms of the
 * RPA SDK License as published at http://www.propulsion-analysis.com/sdk_eula.htm
 *
 * This program is distributed WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the RPA SDK License for more details (a copy is included
 * in the sdk_eula.htm file that accompanied this program).
 *
 * You should have received a copy of the RPA SDK License along with this program;
 * if not, write to author <contact@propulsion-analysis.com>
 *
 * Please contact author <contact@propulsion-analysis.com> or visit http://www.propulsion-analysis.com
 * if you need additional information or have any questions.
 */

#include "utils/Util.hpp"
#include "thermodynamics/input/Input.hpp"
#include "thermodynamics/thermo/Thermodynamics.hpp"
#include "thermodynamics/dll/Export.hpp"
#include "thermodynamics/gasdynamics/StandardAtmosphere.hpp"

#include "common.hpp"

/**
 * This example calculates the performance of rocket engine using pre-defined configuration file.
 */
int main(int argc, char* argv[]) {

	double throttleValue = 1.0;

	util::Log::createLog("ROOT")->
		addLogger(new util::FileLogger("", 10*1024));

	// Initialize thermodatabase
	initThermoDatabase();

	// Initialize configuration file object
	thermo::input::ConfigFile* data = new thermo::input::ConfigFile("examples/RD-170.cfg");

	// Read configuration file
	data->read();

	// Initialize performance solver
	performance::TheoreticalPerformance* performance = new performance::TheoreticalPerformance(data, false);
	performance::ThrottlingPerformance* throttlingPerformance = NULL;

	// Solve
	performance->solve();
	if (throttleValue!=1.0) {
		throttlingPerformance = new performance::ThrottlingPerformance(performance, throttleValue);
	}

	// Get nozzle exit object
	performance::equilibrium::NozzleSectionConditions* nozzleExit = (throttlingPerformance?throttlingPerformance:performance)->getExitSection();

	double Is_v = nozzleExit->getIs_v();
	double Is = nozzleExit->getIs();
	double Is_SL = nozzleExit->getIs_H(CONST_ATM);

	printf("Is_SL=%8.3f m/s, Is_opt=%8.3f Is_vac=%8.3f, r=%4.2f\n", Is_SL, Is, Is_v, throttleValue);

	// Do not delete nozzleExit
	delete performance;
	delete throttlingPerformance;
	delete data;

	util::Log::finalize();

	return 0;
}


