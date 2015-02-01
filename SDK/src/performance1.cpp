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

	util::Log::createLog("ROOT")->
		addLogger(new util::FileLogger("", 10*1024));

	// Initialize thermodatabase
	initThermoDatabase();

	bool optimizePropellant = false;

	// Initialize configuration file object
	thermo::input::ConfigFile* data = new thermo::input::ConfigFile("examples/RD-170.cfg");

	// Read configuration file
	data->read();

	// Modify input data, if necessary
	// For instance:
	//
	// Set nozzle area ratio
	// data->getNozzleFlowOptions().setNozzleExitConditions().setAreaRatio(30, true);
	//
	// Set propellant O/F ratio (km==O/F):
	// data->getPropellant().setRatio(2.5, thermo::input::Ratio::km);
	//
	// Set chamber pressure:
	// data->getCombustionChamberConditions().setPressure(15, thermo::input::Pressure::MPa);
	//
	// etc.

	// Initialize performance solver
	performance::TheoreticalPerformance* performance = new performance::TheoreticalPerformance(data, false);

	if (optimizePropellant && thermo::input::Ratio::fractions!=data->getPropellant().getRatioType()) {
		// Find optimal mixture ratio for given propellant
		performance->optimizeForSpecificImpulse();
	} else {
		performance->solve();
	}
	// O/F ratio
	double r = performance->getPropellant()->getKm();

	// Get nozzle exit object
	performance::equilibrium::NozzleSectionConditions* nozzleExit = performance->getExitSection();

	double Is_v = nozzleExit->getIs_v();
	double Is = nozzleExit->getIs();
	double Is_SL = nozzleExit->getIs_H(CONST_ATM);

	printf("Is_SL=%8.3f m/s, Is_opt=%8.3f Is_vac=%8.3f\n", Is_SL/CONST_G, Is, Is_v);

	delete performance;
	delete data;

	util::Log::finalize();

	return 0;
}


