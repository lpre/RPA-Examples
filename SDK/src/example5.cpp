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

performance::equilibrium::NozzleSectionConditions* getFlowSeparation(performance::TheoreticalPerformance* performance, double pa, bool checkFlowSeparation)  {

	if (checkFlowSeparation) {

		// Critical ambient pressure
		double pa_crit = performance->getPaCrit();

		if (pa>pa_crit) {
			// If pa>pa_crit, there is a flow separation in the nozzle
			// Find the location of the flow separation

			thermo::input::NozzleFlowOptions* pFlowOptions = new thermo::input::NozzleFlowOptions(performance->getData()->getNozzleFlowOptions());

			bool checkForFreezing = pFlowOptions->isFreezingConditionsSet() && pFlowOptions->getFreezingConditions().isCalculate();

			thermo::input::NozzleSectionConditions& nozzleSectionConditions = pFlowOptions->getNozzleExitConditions();
			double condition;
			performance::frozen::NozzleSectionConditions::CONDITION_TYPE conditionType;
			if (nozzleSectionConditions.isAreaRatio()) {
				condition = nozzleSectionConditions.getAreaRatio();
				conditionType = performance::frozen::NozzleSectionConditions::FR;
			} else if (nozzleSectionConditions.isPressureRatio()) {
				condition = nozzleSectionConditions.getPressureRatio();
				conditionType = performance::frozen::NozzleSectionConditions::pp;
			} else if (nozzleSectionConditions.isPressure()) {
				condition = nozzleSectionConditions.getPressure();
				conditionType = performance::frozen::NozzleSectionConditions::P;
			}

			return performance->solveNozzleSection(condition, conditionType, checkForFreezing, true, pa, false);
		}

	}

	return NULL;
}


/**
 * This example calculates the performance of rocket engine at several values of ambient pressure using pre-defined configuration file,
 * considering nozzle flow separation effects.
 */
int main(int argc, char* argv[]) {

	bool checkFlowSeparation = true;
	bool applyCorrectionFactor = true;

	double nominalMdot = 10; 				// kg/s
	double throttleValue = 1.0;
	double mdot = throttleValue*nominalMdot;

	util::Log::createLog("ROOT")->
		addLogger(new util::FileLogger("", 10*1024));

	// Initialize thermodatabase
	initThermoDatabase();

	// Initialize configuration file object
	thermo::input::ConfigFile* data = new thermo::input::ConfigFile("examples/Aestus.cfg");

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
	performance::equilibrium::NozzleSectionConditions* flowSeparationSection = NULL;

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

		flowSeparationSection = getFlowSeparation(throttlingPerformance?throttlingPerformance:performance, pa, checkFlowSeparation);

		double Is = 0;
		if (flowSeparationSection) {
			// Calculate Isp for nozzle with flow separation.
			double p2 = flowSeparationSection->getP();
			double IsH2 = 0.3*(pa - p2)*(flowSeparationSection->getF() - nozzleExit->getF());
			Is = flowSeparationSection->getIs_v()*phi - flowSeparationSection->getF()*pa + IsH2;
		} else {
			// Calculate Isp for nozzle without flow separation.
			Is = nozzleExit->getIs_v()*phi - nozzleExit->getF()*pa;
		}

		double F = Is*mdot;
		printf("pa=%8.3f atm,  Is=%8.3f m/s, F=%8.3f kN, r=%4.2f %s\n", pa/CONST_ATM, Is, F/1000., throttleValue, NULL!=flowSeparationSection?"(flow separation)":"");

		delete flowSeparationSection;
	}

	delete performance;
	delete throttlingPerformance;
	delete data;

	util::Log::finalize();

	return 0;
}


