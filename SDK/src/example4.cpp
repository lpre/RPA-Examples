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
 * This example calculates the performance of rocket engine at several values of ambient pressure using pre-defined configuration file,
 * considering performance losses.
 */
int main(int argc, char* argv[]) {

	bool applyCorrectionFactor = true;

	double nominalMdot = 10; 				// kg/s
	double throttleValue = 1.0;
	double mdot = throttleValue*nominalMdot;

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

	// Performance correction factor
	double phi = 1.0;
	if (applyCorrectionFactor) {
		// Get performance correction factor
		performance::efficiency::CorrectionFactors* correctionFactors = new performance::efficiency::CorrectionFactors(throttlingPerformance?throttlingPerformance:performance);
		phi = correctionFactors->getOverallEfficiency();
		delete correctionFactors;
	}

	int n = 50;
	double p1 = CONST_ATM;			// Ambient pressure initial value
	double p2 = CONST_ATM*0.0001;	// Ambient pressure final value
	for (int i=0; i<=n; ++i) {
		double pa = p1 + (p2-p1)*(double)i/(double)n;	// Ambient pressure current value
		double Is = nozzleExit->getIs_v()*phi - nozzleExit->getF()*pa;
		double F = Is*mdot;
		printf("pa=%8.3f atm, Is=%8.3f m/s, F=%8.3f kN, r=%4.2f\n", pa/CONST_ATM, Is, F/1000., throttleValue);
	}

	delete performance;
	delete throttlingPerformance;
	delete data;

	util::Log::finalize();

	return 0;
}


