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

void initThermoDatabase() {
	try {
		loadThermodynamicsDatabase("resources/thermo.inp", true);
	} catch (const std::exception& ex) {
		util::Log::warnf("THERMO", "Could not load thermodynamics data from resources/thermo.inp\n");
	}

	try {
		loadThermodynamicsDatabase("resources/usr_thermo.inp", false);
	} catch (const std::exception& ex) {
		util::Log::warnf("THERMO", "Could not load user-defined thermodynamics data from resources/usr_thermo.inp\n");
	}

	try {
		loadPropertiesDatabase("resources/properties.inp");
	} catch (const std::exception& ex) {
		util::Log::warnf("THERMO", "Could not load properties data from resources/properties.inp\n");
	}

	try {
		loadPropertiesDatabase("resources/usr_properties.inp");
	} catch (const std::exception& ex) {
		util::Log::warnf("THERMO", "Could not load user-defined properties data from resources/usr_properties.inp\n");
	}

	try {
		loadTransportPropertiesDatabase("resources/trans.inp");
	} catch (const std::exception& ex) {
		util::Log::warnf("THERMO", "Could not load transport properties data from resources/trans.inp\n");
	}
}



