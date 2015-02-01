#include "wrapper.h"

#include <cstdio>

#include "utils/Util.hpp"
#include "thermodynamics/input/Input.hpp"
#include "thermodynamics/thermo/Thermodynamics.hpp"
#include "thermodynamics/dll/Export.hpp"
#include "thermodynamics/gasdynamics/StandardAtmosphere.hpp"

#include "nozzle/ConicalNozzle.hpp"
#include "nozzle/ParabolicNozzle.hpp"
#include "nozzle/MocNozzle.hpp"
#include "nozzle/digitized/NozzleContour.hpp"

#include "common.hpp"
#include "wrapper.h"

#ifdef __cplusplus
extern "C" {
#endif

//*****************************************************************************

void initialize(bool consoleOutput) {
	util::Log::createLog("ROOT")->
		addLogger(new util::FileLogger("", 10*1024));

	if (consoleOutput) {
		util::Log::getLog("ROOT")->
			addLogger(new util::ConsoleLogger());
	}

	// Initialize thermodatabase
	initThermoDatabase();
}

void finalize() {
	util::Log::finalize();
}

//*****************************************************************************

void* configFile() {
	thermo::input::ConfigFile* data = new thermo::input::ConfigFile();
	return data;
}

void* configFileLoad(const char* name) {
	thermo::input::ConfigFile* data = new thermo::input::ConfigFile(name);
	data->read();
	return data;
}

void* configFileClone(void* dataPtr) {
	if (dataPtr) {
		thermo::input::ConfigFile* data = new thermo::input::ConfigFile(*((thermo::input::ConfigFile*)dataPtr));
		return data;
	}
	return NULL;
}

void configFileSave(void* dataPtr) {
	if (dataPtr) {
		((thermo::input::ConfigFile*)dataPtr)->write(NULL, true) ;
	}
}

void configFileSaveAs(void* dataPtr, const char* name) {
	if (dataPtr) {
		((thermo::input::ConfigFile*)dataPtr)->write(name, true) ;
	}
}


void configFileDelete(void* dataPtr) {
	if (dataPtr) {
		delete dataPtr;
	}
}

bool configFileGeneralOptionsIsMultiphase(void* dataPtr) {
	if (dataPtr) {
		return ((thermo::input::ConfigFile*)dataPtr)->getGeneralOptions().isMultiphase();
	}
	return false;
}

void configFileGeneralOptionsSetMultiphase(void* dataPtr, bool value) {
	if (dataPtr) {
		((thermo::input::ConfigFile*)dataPtr)->getGeneralOptions().setMultiphase(value);
	}
}

bool configFileGeneralOptionsIsIons(void* dataPtr)  {
	if (dataPtr) {
		return ((thermo::input::ConfigFile*)dataPtr)->getGeneralOptions().isIons();
	}
	return false;
};

void configFileGeneralOptionsSetIons(void* dataPtr, bool value) {
	if (dataPtr) {
		((thermo::input::ConfigFile*)dataPtr)->getGeneralOptions().setIons(value);
	}
}

bool configFileGeneralOptionsIsFlowSeparation(void* dataPtr){
	if (dataPtr) {
		return ((thermo::input::ConfigFile*)dataPtr)->getGeneralOptions().isFlowSeparation();
	}
	return false;
};

void configFileGeneralOptionsSetFlowSeparation(void* dataPtr, bool value) {
	if (dataPtr) {
		((thermo::input::ConfigFile*)dataPtr)->getGeneralOptions().setFlowSeparation(value);
	}
}

double configFileCombustionChamberConditionsGetPressure(void* dataPtr, const char* units) {
	if (dataPtr) {
		return ((thermo::input::ConfigFile*)dataPtr)->getCombustionChamberConditions().getPressure(thermo::input::Pressure::rawToUnit(units));
	}
	return 0;
}

void configFileCombustionChamberConditionsSetPressure(void* dataPtr, double value, const char* units) {
	if (dataPtr) {
		((thermo::input::ConfigFile*)dataPtr)->getCombustionChamberConditions().setPressure(value, thermo::input::Pressure::rawToUnit(units), false);
	}
}

bool configFileNozzleFlowOptionsIsCalculateNozzleFlow(void* dataPtr) {
	if (dataPtr) {
		return ((thermo::input::ConfigFile*)dataPtr)->getNozzleFlowOptions().isCalculateNozzleFlow();
	}
	return false;
}

void configFileNozzleFlowOptionsSetCalculateNozzleFlow(void* dataPtr, bool value) {
	if (dataPtr) {
		((thermo::input::ConfigFile*)dataPtr)->getNozzleFlowOptions().setCalculateNozzleFlow(value);
	}
}

bool configFileNozzleFlowOptionsIsFreezingConditions(void* dataPtr) {
	if (dataPtr) {
		return ((thermo::input::ConfigFile*)dataPtr)->getNozzleFlowOptions().isFreezingConditionsSet();
	}
	return false;
}

void configFileNozzleFlowOptionsSetFreezingConditions(void* dataPtr, unsigned int type, double value, const char* units) {
	if (dataPtr) {
		((thermo::input::ConfigFile*)dataPtr)->getNozzleFlowOptions().setFreezingConditions().setCalculate(true);

		switch (type) {
		case 0:
			((thermo::input::ConfigFile*)dataPtr)->getNozzleFlowOptions().setFreezingConditions().setExpansionRatio(value);
			break;
		case 1:
			((thermo::input::ConfigFile*)dataPtr)->getNozzleFlowOptions().setFreezingConditions().setPressure(value, thermo::input::Pressure::rawToUnit(units), false);
			break;
		case 2:
			((thermo::input::ConfigFile*)dataPtr)->getNozzleFlowOptions().setFreezingConditions().setPressureRatio(value);
			break;
		}
	}
}

void configFileNozzleFlowOptionsDeleteFreezingConditions(void* dataPtr) {
	if (dataPtr) {
		((thermo::input::ConfigFile*)dataPtr)->getNozzleFlowOptions().deleteFreezingConditions();
	}
}

bool configFileNozzleFlowOptionsIsNozzleInletConditions(void* dataPtr) {
	if (dataPtr) {
		return ((thermo::input::ConfigFile*)dataPtr)->getNozzleFlowOptions().isNozzleInletConditionsSet();
	}
	return false;
}

void configFileNozzleFlowOptionsSetNozzleInletConditions(void* dataPtr, unsigned int type, double value, const char* units) {
	if (dataPtr) {
		switch (type) {
		case 0:
			((thermo::input::ConfigFile*)dataPtr)->getNozzleFlowOptions().setNozzleInletConditions().setContractionAreaRatio(value);
			break;
		case 1:
			((thermo::input::ConfigFile*)dataPtr)->getNozzleFlowOptions().setNozzleInletConditions().setMassFlux(value, thermo::input::MassFlux::rawToUnit(units), false);
			break;
		}
	}
}

void configFileNozzleFlowOptionsDeleteNozzleInletConditions(void* dataPtr) {
	if (dataPtr) {
		((thermo::input::ConfigFile*)dataPtr)->getNozzleFlowOptions().deleteNozzleInletConditions();
	}
}

bool configFileNozzleFlowOptionsIsAmbientConditions(void* dataPtr) {
	if (dataPtr) {
		return ((thermo::input::ConfigFile*)dataPtr)->getNozzleFlowOptions().isAmbientConditionsSet();
	}
	return false;
}

void configFileNozzleFlowOptionsSetAmbientConditions(void* dataPtr, double value, const char* units) {
	if (dataPtr) {
		((thermo::input::ConfigFile*)dataPtr)->getNozzleFlowOptions().setAmbientConditions().setFixedPressure(value, thermo::input::Pressure::rawToUnit(units), false);
	}
}

void configFileNozzleFlowOptionsSetAmbientConditionsRange(void* dataPtr, double lowerValue, const char* lowerUnits, double upperValue, const char* upperUnits) {
	if (dataPtr) {
		((thermo::input::ConfigFile*)dataPtr)->getNozzleFlowOptions().setAmbientConditions().setRangePressureMin(lowerValue, thermo::input::Pressure::rawToUnit(lowerUnits), false);
		((thermo::input::ConfigFile*)dataPtr)->getNozzleFlowOptions().setAmbientConditions().setRangePressureMax(upperValue, thermo::input::Pressure::rawToUnit(upperUnits), false);
	}
}

bool configFileNozzleFlowOptionsAmbientConditionsIsDeliveredPerformance(void* dataPtr) {
	if (dataPtr) {
		return ((thermo::input::ConfigFile*)dataPtr)->getNozzleFlowOptions().setAmbientConditions().isDeliveredPerformance();
	}
	return false;
}

void configFileNozzleFlowOptionsAmbientConditionsSetDeliveredPerformance(void* dataPtr, bool value) {
	if (dataPtr) {
		((thermo::input::ConfigFile*)dataPtr)->getNozzleFlowOptions().setAmbientConditions().setDeliveredPerformance(value);
	}
}

void configFileNozzleFlowOptionsDeleteAmbientConditions(void* dataPtr) {
	if (dataPtr) {
		((thermo::input::ConfigFile*)dataPtr)->getNozzleFlowOptions().deleteAmbientConditions();
	}
}

bool configFileNozzleFlowOptionsIsThrottlingConditions(void* dataPtr) {
	if (dataPtr) {
		return ((thermo::input::ConfigFile*)dataPtr)->getNozzleFlowOptions().isThrottlingConditionsSet();
	}
	return false;
}

void configFileNozzleFlowOptionsSetThrottlingConditions(void* dataPtr, double value) {
	if (dataPtr) {
		((thermo::input::ConfigFile*)dataPtr)->getNozzleFlowOptions().setThrottlingConditions().setFixedFlowrate(value);
	}
}

void configFileNozzleFlowOptionsSetThrottlingConditionsRange(void* dataPtr, double lowerValue, double upperValue) {
	if (dataPtr) {
		((thermo::input::ConfigFile*)dataPtr)->getNozzleFlowOptions().setThrottlingConditions().setRangeFlowrateMin(lowerValue);
		((thermo::input::ConfigFile*)dataPtr)->getNozzleFlowOptions().setThrottlingConditions().setRangeFlowrateMax(upperValue);
	}
}

bool configFileNozzleFlowOptionsThrottlingConditionsIsDeliveredPerformance(void* dataPtr) {
	if (dataPtr) {
		return ((thermo::input::ConfigFile*)dataPtr)->getNozzleFlowOptions().setThrottlingConditions().isDeliveredPerformance();
	}
	return false;
}

void configFileNozzleFlowOptionsThrottlingConditionsSetDeliveredPerformance(void* dataPtr, bool value) {
	if (dataPtr) {
		((thermo::input::ConfigFile*)dataPtr)->getNozzleFlowOptions().setThrottlingConditions().setDeliveredPerformance(value);
	}
}

void configFileNozzleFlowOptionsDeleteThrottlingConditions(void* dataPtr) {
	if (dataPtr) {
		((thermo::input::ConfigFile*)dataPtr)->getNozzleFlowOptions().deleteThrottlingConditions();
	}
}

bool configFileNozzleFlowOptionsIsEfficiencyFactors(void* dataPtr) {
	if (dataPtr) {
		return ((thermo::input::ConfigFile*)dataPtr)->getNozzleFlowOptions().isEfficiencyFactorsSet();
	}
	return false;
}

bool configFileNozzleFlowOptionsIsApplyEfficiencyFactors(void* dataPtr) {
	if (dataPtr) {
		return ((thermo::input::ConfigFile*)dataPtr)->getNozzleFlowOptions().isEfficiencyFactorsSet() &&
				((thermo::input::ConfigFile*)dataPtr)->getNozzleFlowOptions().getEfficiencyFactors().isApplyEfficiencyFactors();
	}
	return false;
}

void configFileNozzleFlowOptionsSetApplyEfficiencyFactors(void* dataPtr, bool value) {
	if (dataPtr) {
		((thermo::input::ConfigFile*)dataPtr)->getNozzleFlowOptions().setEfficiencyFactors().setApplyEfficiencyFactors(value);
	}
}

bool configFileNozzleFlowOptionsIsCycleEfficiency(void* dataPtr) {
	if (dataPtr) {
		return ((thermo::input::ConfigFile*)dataPtr)->getNozzleFlowOptions().setEfficiencyFactors().isCycleEfficiencySet();
	}
	return false;
}

void configFileNozzleFlowOptionsSetCycleEfficiency(void* dataPtr, double value) {
	if (dataPtr) {
		((thermo::input::ConfigFile*)dataPtr)->getNozzleFlowOptions().setEfficiencyFactors().setCycleEfficiency(value);
	}
}

double configFileNozzleFlowOptionsGetCycleEfficiency(void* dataPtr) {
	if (dataPtr) {
		return ((thermo::input::ConfigFile*)dataPtr)->getNozzleFlowOptions().setEfficiencyFactors().getCycleEfficiency();
	}
	return 0;
}

void configFileNozzleFlowOptionsDeleteCycleEfficiency(void* dataPtr) {
	if (dataPtr) {
		((thermo::input::ConfigFile*)dataPtr)->getNozzleFlowOptions().setEfficiencyFactors().deleteCycleEfficiency();
	}
}

bool configFileNozzleFlowOptionsIsReactionEfficiency(void* dataPtr) {
	if (dataPtr) {
		return ((thermo::input::ConfigFile*)dataPtr)->getNozzleFlowOptions().setEfficiencyFactors().isReactionEfficiencySet();
	}
	return false;
}

void configFileNozzleFlowOptionsSetReactionEfficiency(void* dataPtr, double value) {
	if (dataPtr) {
		((thermo::input::ConfigFile*)dataPtr)->getNozzleFlowOptions().setEfficiencyFactors().setReactionEfficiency(value);
	}
}

double configFileNozzleFlowOptionsGetReactionEfficiency(void* dataPtr) {
	if (dataPtr) {
		return ((thermo::input::ConfigFile*)dataPtr)->getNozzleFlowOptions().setEfficiencyFactors().getReactionEfficiency();
	}
	return 0;
}

void configFileNozzleFlowOptionsDeleteReactionEfficiency(void* dataPtr) {
	if (dataPtr) {
		((thermo::input::ConfigFile*)dataPtr)->getNozzleFlowOptions().setEfficiencyFactors().deleteCycleEfficiency();
	}
}

bool configFileNozzleFlowOptionsIsNozzleEfficiency(void* dataPtr) {
	if (dataPtr) {
		return ((thermo::input::ConfigFile*)dataPtr)->getNozzleFlowOptions().setEfficiencyFactors().isNozzleEfficiencySet();
	}
	return false;
}

void configFileNozzleFlowOptionsSetNozzleEfficiency(void* dataPtr, double value) {
	if (dataPtr) {
		((thermo::input::ConfigFile*)dataPtr)->getNozzleFlowOptions().setEfficiencyFactors().setNozzleEfficiency(value);
	}
}

double configFileNozzleFlowOptionsGetNozzleEfficiency(void* dataPtr) {
	if (dataPtr) {
		return ((thermo::input::ConfigFile*)dataPtr)->getNozzleFlowOptions().setEfficiencyFactors().getNozzleEfficiency();
	}
	return 0;
}

void configFileNozzleFlowOptionsDeleteNozzleEfficiency(void* dataPtr) {
	if (dataPtr) {
		((thermo::input::ConfigFile*)dataPtr)->getNozzleFlowOptions().setEfficiencyFactors().deleteNozzleEfficiency();
	}
}

bool configFileNozzleFlowOptionsIsNozzleLength(void* dataPtr) {
	if (dataPtr) {
		return ((thermo::input::ConfigFile*)dataPtr)->getNozzleFlowOptions().setEfficiencyFactors().isNozzleLengthSet();
	}
	return false;
}

void configFileNozzleFlowOptionsSetNozzleLength(void* dataPtr, double value) {
	if (dataPtr) {
		((thermo::input::ConfigFile*)dataPtr)->getNozzleFlowOptions().setEfficiencyFactors().setNozzleLength(value);
	}
}

double configFileNozzleFlowOptionsGetNozzleLength(void* dataPtr) {
	if (dataPtr) {
		return ((thermo::input::ConfigFile*)dataPtr)->getNozzleFlowOptions().setEfficiencyFactors().getNozzleLength();
	}
	return 0;
}

void configFileNozzleFlowOptionsDeleteNozzleLength(void* dataPtr) {
	if (dataPtr) {
		((thermo::input::ConfigFile*)dataPtr)->getNozzleFlowOptions().setEfficiencyFactors().deleteNozzleLength();
	}
}

bool configFileNozzleFlowOptionsIsConeHalfAngle(void* dataPtr) {
	if (dataPtr) {
		return ((thermo::input::ConfigFile*)dataPtr)->getNozzleFlowOptions().setEfficiencyFactors().isConeHalfAngleSet();
	}
	return false;
}

void configFileNozzleFlowOptionsSetConeHalfAngle(void* dataPtr, double value, const char* unit) {
	if (dataPtr) {
		((thermo::input::ConfigFile*)dataPtr)->getNozzleFlowOptions().setEfficiencyFactors().setConeHalfAngle(thermo::input::Angle::convert(value, thermo::input::Angle::rawToUnit(unit), thermo::input::Angle::degrees));
	}
}

double configFileNozzleFlowOptionsGetConeHalfAngle(void* dataPtr, const char* unit) {
	if (dataPtr) {
		return thermo::input::Angle::convert(((thermo::input::ConfigFile*)dataPtr)->getNozzleFlowOptions().setEfficiencyFactors().getConeHalfAngle(), thermo::input::Angle::degrees, thermo::input::Angle::rawToUnit(unit));
	}
	return 0;
}

void configFileNozzleFlowOptionsDeleteConeHalfAngle(void* dataPtr) {
	if (dataPtr) {
		((thermo::input::ConfigFile*)dataPtr)->getNozzleFlowOptions().setEfficiencyFactors().deleteConeHalfAngle();
	}
}

void configFileNozzleFlowOptionsDeleteEfficiencyFactors(void* dataPtr) {
	if (dataPtr) {
		((thermo::input::ConfigFile*)dataPtr)->getNozzleFlowOptions().deleteEfficiencyFactors();
	}
}

double configFileNozzlePropellantGetRatio(void* dataPtr) {
	if (dataPtr) {
		return ((thermo::input::ConfigFile*)dataPtr)->getPropellant().getRatio();
	}
	return 0;
}

const char* configFileNozzlePropellantGetRatioType(void* dataPtr) {
	if (dataPtr) {
		thermo::input::Ratio::unitToRaw(((thermo::input::ConfigFile*)dataPtr)->getPropellant().getRatioType());
	}
	return NULL;
}

void configFileNozzlePropellantSetRatio(void* dataPtr, double value, const char* type) {
	if (dataPtr) {
		((thermo::input::ConfigFile*)dataPtr)->getPropellant().setRatio(value, thermo::input::Ratio::rawToUnit(type));
	}
}

unsigned int configFileNozzlePropellantGetOxidizerListSize(void* dataPtr) {
	if (dataPtr) {
		return ((thermo::input::ConfigFile*)dataPtr)->getPropellant().getOxidizerListSize();
	}
	return 0;
}

const char* configFileNozzlePropellantGetOxidizerName(void* dataPtr, unsigned int n) {
	if (dataPtr) {
		return ((thermo::input::ConfigFile*)dataPtr)->getPropellant().getOxidizer(n).getName().c_str();
	}
	return NULL;
}

double configFileNozzlePropellantGetOxidizerMassFraction(void* dataPtr, unsigned int n) {
	if (dataPtr) {
		return ((thermo::input::ConfigFile*)dataPtr)->getPropellant().getOxidizer(n).getMf();
	}
	return 0;
}

double configFileNozzlePropellantGetOxidizerPressure(void* dataPtr, unsigned int n, const char* units) {
	if (dataPtr) {
		return ((thermo::input::ConfigFile*)dataPtr)->getPropellant().getOxidizer(n).getP(thermo::input::Pressure::rawToUnit(units));
	}
	return 0;
}

double configFileNozzlePropellantGetOxidizerTemperature(void* dataPtr, unsigned int n, const char* units) {
	if (dataPtr) {
		return ((thermo::input::ConfigFile*)dataPtr)->getPropellant().getOxidizer(n).getT(thermo::input::Temperature::rawToUnit(units));
	}
	return 0;
}

void configFileNozzlePropellantAddOxidizer(void* dataPtr, const char* name, double massFraction, double pressure, const char* pressureUnits, double temperature, const char* temperatureUnits) {
	if (dataPtr) {
		((thermo::input::ConfigFile*)dataPtr)->getPropellant().addOxidizer(new thermo::input::Component(name, massFraction, temperature, thermo::input::Temperature::rawToUnit(temperatureUnits), pressure, thermo::input::Pressure::rawToUnit(pressureUnits)));
	}
}

unsigned int configFileNozzlePropellantGetFuelListSize(void* dataPtr) {
	if (dataPtr) {
		return ((thermo::input::ConfigFile*)dataPtr)->getPropellant().getFuelListSize();
	}
	return 0;
}

const char* configFileNozzlePropellantGetFuelName(void* dataPtr, unsigned int n) {
	if (dataPtr) {
		return ((thermo::input::ConfigFile*)dataPtr)->getPropellant().getFuel(n).getName().c_str();
	}
	return NULL;
}

double configFileNozzlePropellantGetFuelMassFraction(void* dataPtr, unsigned int n) {
	if (dataPtr) {
		return ((thermo::input::ConfigFile*)dataPtr)->getPropellant().getFuel(n).getMf();
	}
	return 0;
}

double configFileNozzlePropellantGetFuelPressure(void* dataPtr, unsigned int n, const char* units) {
	if (dataPtr) {
		return ((thermo::input::ConfigFile*)dataPtr)->getPropellant().getFuel(n).getP(thermo::input::Pressure::rawToUnit(units));
	}
	return 0;
}

double configFileNozzlePropellantGetFuelTemperature(void* dataPtr, unsigned int n, const char* units) {
	if (dataPtr) {
		((thermo::input::ConfigFile*)dataPtr)->getPropellant().getFuel(n).getT(thermo::input::Temperature::rawToUnit(units));
	}
	return 0;
}

void configFileNozzlePropellantAddFuel(void* dataPtr, const char* name, double massFraction, double pressure, const char* pressureUnits, double temperature, const char* temperatureUnits) {
	if (dataPtr) {
		((thermo::input::ConfigFile*)dataPtr)->getPropellant().addFuel(new thermo::input::Component(name, massFraction, temperature, thermo::input::Temperature::rawToUnit(temperatureUnits), pressure, thermo::input::Pressure::rawToUnit(pressureUnits)));
	}
}

unsigned int configFileNozzlePropellantGetSpeciesListSize(void* dataPtr) {
	if (dataPtr) {
		return ((thermo::input::ConfigFile*)dataPtr)->getPropellant().getSpeciesListSize();
	}
	return 0;
}

const char* configFileNozzlePropellantGetSpeciesName(void* dataPtr, unsigned int n) {
	if (dataPtr) {
		return ((thermo::input::ConfigFile*)dataPtr)->getPropellant().getSpecies(n).getName().c_str();;
	}
	return NULL;
}

double configFileNozzlePropellantGetSpeciesMassFraction(void* dataPtr, unsigned int n) {
	if (dataPtr) {
		((thermo::input::ConfigFile*)dataPtr)->getPropellant().getSpecies(n).getMf();
	}
	return 0;
}

double configFileNozzlePropellantGetSpeciesPressure(void* dataPtr, unsigned int n, const char* units) {
	if (dataPtr) {
		((thermo::input::ConfigFile*)dataPtr)->getPropellant().getSpecies(n).getP(thermo::input::Pressure::rawToUnit(units));
	}
	return 0;
}

double configFileNozzlePropellantGetSpeciesTemperature(void* dataPtr, unsigned int n, const char* units) {
	if (dataPtr) {
		((thermo::input::ConfigFile*)dataPtr)->getPropellant().getSpecies(n).getT(thermo::input::Temperature::rawToUnit(units));
	}
	return 0;
}

void configFileNozzlePropellantAddSpecies(void* dataPtr, const char* name, double massFraction, double pressure, const char* pressureUnits, double temperature, const char* temperatureUnits) {
	if (dataPtr) {
		((thermo::input::ConfigFile*)dataPtr)->getPropellant().addSpecies(new thermo::input::Component(name, massFraction, temperature, thermo::input::Temperature::rawToUnit(temperatureUnits), pressure, thermo::input::Pressure::rawToUnit(pressureUnits)));
	}
}

bool configFileEngineSizeIsThrust(void* dataPtr) {
	if (dataPtr) {
		return ((thermo::input::ConfigFile*)dataPtr)->getEngineSize().isThrustSet();
	}
	return false;
}

double configFileEngineSizeGetThrust(void* dataPtr, const char* units) {
	if (dataPtr) {
		return ((thermo::input::ConfigFile*)dataPtr)->getEngineSize().getThrust(thermo::input::Force::rawToUnit(units));
	}
	return 0;
}

void configFileEngineSizeSetThrust(void* dataPtr, double value, const char* units) {
	if (dataPtr) {
		((thermo::input::ConfigFile*)dataPtr)->getEngineSize().setThrust(value, thermo::input::Force::rawToUnit(units), false);
	}
}

void configFileEngineSizeDeleteThrust(void* dataPtr) {
	if (dataPtr) {
		((thermo::input::ConfigFile*)dataPtr)->getEngineSize().deleteThrust();
	}
}

bool configFileEngineSizeIsAmbientPressure(void* dataPtr) {
	if (dataPtr) {
		return ((thermo::input::ConfigFile*)dataPtr)->getEngineSize().isAmbientPressureSet();
	}
	return false;
}

double configFileEngineSizeGetAmbientPressure(void* dataPtr, const char* units) {
	if (dataPtr) {
		return ((thermo::input::ConfigFile*)dataPtr)->getEngineSize().getAmbientPressure(thermo::input::Pressure::rawToUnit(units));
	}
	return 0;
}

void configFileEngineSizeSetAmbientPressure(void* dataPtr, double value, const char* units) {
	if (dataPtr) {
		((thermo::input::ConfigFile*)dataPtr)->getEngineSize().setAmbientPressure(value, thermo::input::Pressure::rawToUnit(units), false);
	}
}

void configFileEngineSizeDeleteAmbientPressure(void* dataPtr) {
	if (dataPtr) {
		((thermo::input::ConfigFile*)dataPtr)->getEngineSize().deleteAmbientPressure();
	}
}

bool configFileEngineSizeIsMdot(void* dataPtr) {
	if (dataPtr) {
		return ((thermo::input::ConfigFile*)dataPtr)->getEngineSize().isMdotSet();
	}
	return false;
}

double configFileEngineSizeGetMdot(void* dataPtr, const char* units) {
	if (dataPtr) {
		return ((thermo::input::ConfigFile*)dataPtr)->getEngineSize().getMdot(thermo::input::MassFlowRate::rawToUnit(units));
	}
	return 0;
}

void configFileEngineSizeSetMdot(void* dataPtr, double value, const char* units)  {
	if (dataPtr) {
		((thermo::input::ConfigFile*)dataPtr)->getEngineSize().setMdot(value, thermo::input::MassFlowRate::rawToUnit(units), false);
	}
}

void configFileEngineSizeDeleteMdot(void* dataPtr) {
	if (dataPtr) {
		((thermo::input::ConfigFile*)dataPtr)->getEngineSize().deleteMdot();
	}
}

bool configFileEngineSizeIsThroatDiameter(void* dataPtr) {
	if (dataPtr) {
		return ((thermo::input::ConfigFile*)dataPtr)->getEngineSize().isThroatDSet();
	}
	return false;
}

double configFileEngineSizeGetThroatDiameter(void* dataPtr, const char* units) {
	if (dataPtr) {
		return ((thermo::input::ConfigFile*)dataPtr)->getEngineSize().getThroatD(thermo::input::Length::rawToUnit(units));
	}
	return 0;
}

void configFileEngineSizeSetThroatDiameter(void* dataPtr, double value, const char* units) {
	if (dataPtr) {
		((thermo::input::ConfigFile*)dataPtr)->getEngineSize().setThroatD(value, thermo::input::Length::rawToUnit(units), false);
	}
}

void configFileEngineSizeDeleteThroatDiameter(void* dataPtr) {
	if (dataPtr) {
		((thermo::input::ConfigFile*)dataPtr)->getEngineSize().deleteThroatD();
	}
}

unsigned int configFileEngineSizeGetChambersNo(void* dataPtr) {
	if (dataPtr) {
		return ((thermo::input::ConfigFile*)dataPtr)->getEngineSize().getChambersNo();
	}
	return 0;
}

void configFileEngineSizeSetChambersNo(void* dataPtr, unsigned int value) {
	if (dataPtr) {
		((thermo::input::ConfigFile*)dataPtr)->getEngineSize().setChambersNo(value);
	}
}

double configFileEngineSizeGetChamberLength(void* dataPtr, const char* units) {
	if (dataPtr) {
		return ((thermo::input::ConfigFile*)dataPtr)->getEngineSize().getChamberGeometry().getChamberLength(thermo::input::Length::rawToUnit(units));
	}
	return 0;
}

void configFileEngineSizeSetChamberLength(void* dataPtr, double value, const char* units, unsigned int type) {
	if (dataPtr) {
		((thermo::input::ConfigFile*)dataPtr)->getEngineSize().getChamberGeometry().setChamberLength(value, thermo::input::Length::rawToUnit(units), false);
		((thermo::input::ConfigFile*)dataPtr)->getEngineSize().getChamberGeometry().setCharacteristicLength(1==type);
	}
}

unsigned int configFileEngineSizeGetChamberLengthType(void* dataPtr) {
	if (dataPtr) {
		return ((thermo::input::ConfigFile*)dataPtr)->getEngineSize().getChamberGeometry().isCharacteristicLength()?1:2;
	}
	return 0;
}

double configFileEngineSizeGetContractionAngle(void* dataPtr, const char* unit) {
	if (dataPtr) {
		return thermo::input::Angle::convert(((thermo::input::ConfigFile*)dataPtr)->getEngineSize().getChamberGeometry().getContractionAngle(), thermo::input::Angle::degrees, thermo::input::Angle::rawToUnit(unit));
	}
	return 0;
}

void configFileEngineSizeSetContractionAngle(void* dataPtr, double value, const char* unit) {
	if (dataPtr) {
		((thermo::input::ConfigFile*)dataPtr)->getEngineSize().getChamberGeometry().setContractionAngle(thermo::input::Angle::convert(value, thermo::input::Angle::rawToUnit(unit), thermo::input::Angle::degrees));
	}
}

double configFileEngineSizeGetR1ToRtRatio(void* dataPtr) {
	if (dataPtr) {
		return ((thermo::input::ConfigFile*)dataPtr)->getEngineSize().getChamberGeometry().getR1ToRtRatio();
	}
	return 0;
}

void configFileEngineSizeSetR1ToRtRatio(void* dataPtr, double value) {
	if (dataPtr) {
		((thermo::input::ConfigFile*)dataPtr)->getEngineSize().getChamberGeometry().setR1ToRtRatio(value);
	}
}

double configFileEngineSizeGetRnToRtRatio(void* dataPtr) {
	if (dataPtr) {
		return ((thermo::input::ConfigFile*)dataPtr)->getEngineSize().getChamberGeometry().getRnToRtRatio();
	}
	return 0;
}

void configFileEngineSizeSetRnToRtRatio(void* dataPtr, double value) {
	if (dataPtr) {
		((thermo::input::ConfigFile*)dataPtr)->getEngineSize().getChamberGeometry().setRnToRtRatio(value);
	}
}

double configFileEngineSizeGetR2ToR2maxRatio(void* dataPtr) {
	if (dataPtr) {
		return ((thermo::input::ConfigFile*)dataPtr)->getEngineSize().getChamberGeometry().getR2ToR2maxRatio();
	}
	return 0;
}

void configFileEngineSizeSetR2ToR2maxRatio(void* dataPtr, double value) {
	if (dataPtr) {
		((thermo::input::ConfigFile*)dataPtr)->getEngineSize().getChamberGeometry().setR2ToR2maxRatio(value);
	}
}

bool configFileEngineSizeIsTIC(void* dataPtr) {
	if (dataPtr) {
		return ((thermo::input::ConfigFile*)dataPtr)->getEngineSize().getChamberGeometry().isTOC();
	}
	return false;
}

void configFileEngineSizeSetTIC(void* dataPtr, bool value) {
	if (dataPtr) {
		((thermo::input::ConfigFile*)dataPtr)->getEngineSize().getChamberGeometry().setTOC(value);
	}
}

void configFileEngineSizeDeleteTIC(void* dataPtr) {
	if (dataPtr) {
		((thermo::input::ConfigFile*)dataPtr)->getEngineSize().getChamberGeometry().deleteTOC();
	}
}

bool configFileEngineSizeIsTwToT0(void* dataPtr) {
	if (dataPtr) {
		return ((thermo::input::ConfigFile*)dataPtr)->getEngineSize().getChamberGeometry().isTw_to_T0();
	}
	return false;
}

double configFileEngineSizeGetTwToT0(void* dataPtr) {
	if (dataPtr) {
		return ((thermo::input::ConfigFile*)dataPtr)->getEngineSize().getChamberGeometry().getTwToT0();
	}
	return 0;
}

void configFileEngineSizeSetTwToT0(void* dataPtr, double value) {
	if (dataPtr) {
		((thermo::input::ConfigFile*)dataPtr)->getEngineSize().getChamberGeometry().setTwToT0(value);
	}
}

void configFileEngineSizeDeleteTwToT0(void* dataPtr) {
	if (dataPtr) {
		((thermo::input::ConfigFile*)dataPtr)->getEngineSize().getChamberGeometry().deleteTwToT0();
	}
}

bool configFileEngineSizeIsParabolicInitialAngle(void* dataPtr) {
	if (dataPtr) {
		return ((thermo::input::ConfigFile*)dataPtr)->getEngineSize().getChamberGeometry().isParabolicInitialAngle();
	}
	return false;
}

double configFileEngineSizeGetParabolicInitialAngle(void* dataPtr, const char* units) {
	if (dataPtr) {
		return thermo::input::Angle::convert(((thermo::input::ConfigFile*)dataPtr)->getEngineSize().getChamberGeometry().getParabolicInitialAngle(), thermo::input::Angle::degrees, thermo::input::Angle::rawToUnit(units));
	}
	return 0;
}

void configFileEngineSizeSetParabolicInitialAngle(void* dataPtr, double value, const char* units) {
	if (dataPtr) {
		((thermo::input::ConfigFile*)dataPtr)->getEngineSize().getChamberGeometry().setParabolicInitialAngle(thermo::input::Angle::convert(value, thermo::input::Angle::rawToUnit(units), thermo::input::Angle::degrees));
	}
}

void configFileEngineSizeDeleteParabolicInitialAngle(void* dataPtr) {
	if (dataPtr) {
		((thermo::input::ConfigFile*)dataPtr)->getEngineSize().getChamberGeometry().deleteParabolicInitialAngle();
	}
}

bool configFileEngineSizeIsParabolicExitAngle(void* dataPtr) {
	if (dataPtr) {
		((thermo::input::ConfigFile*)dataPtr)->getEngineSize().getChamberGeometry().isParabolicExitAngle();
	}
	return false;
}

double configFileEngineSizeGetParabolicExitAngle(void* dataPtr, const char* units) {
	if (dataPtr) {
		return thermo::input::Angle::convert(((thermo::input::ConfigFile*)dataPtr)->getEngineSize().getChamberGeometry().getParabolicExitAngle(), thermo::input::Angle::degrees, thermo::input::Angle::rawToUnit(units));
	}
	return 0;
}

void configFileEngineSizeSetParabolicExitAngle(void* dataPtr, double value, const char* units) {
	if (dataPtr) {
		((thermo::input::ConfigFile*)dataPtr)->getEngineSize().getChamberGeometry().setParabolicExitAngle(thermo::input::Angle::convert(value, thermo::input::Angle::rawToUnit(units), thermo::input::Angle::degrees));
	}
}

void configFileEngineSizeDeleteParabolicExitAngle(void* dataPtr) {
	if (dataPtr) {
		((thermo::input::ConfigFile*)dataPtr)->getEngineSize().getChamberGeometry().deleteParabolicExitAngle();
	}
}

//*****************************************************************************

void* performanceCreate(void* dataPtr, bool solve, bool optimizePropellant) {
	thermo::input::ConfigFile* data = reinterpret_cast<thermo::input::ConfigFile*>(dataPtr);
	if (data) {
		performance::TheoreticalPerformance* performance = new performance::TheoreticalPerformance(data, false);
		if (solve) {
			performanceSolve(performance, optimizePropellant);
		}

		return performance;
	}
	return NULL;
}

void performanceDelete(void* performancePtr) {
	if (performancePtr) {
		delete performancePtr;
	}
}

void performanceSolve(void* performancePtr, bool optimizePropellant) {
	performance::TheoreticalPerformance* performance = reinterpret_cast<performance::TheoreticalPerformance*>(performancePtr);
	if (performance) {
		if (optimizePropellant && thermo::input::Ratio::fractions!=performance->getData()->getPropellant().getRatioType()) {
			// Find optimal mixture ratio for given propellant
			performance->optimizeForSpecificImpulse();
		} else {
			performance->solve();
		}
	}
}

double performanceGetDeliveredIsp(void* performancePtr, const char* IspUnits, double pa, const char* pressureUnits, double phi) {
	performance::TheoreticalPerformance* performance = reinterpret_cast<performance::TheoreticalPerformance*>(performancePtr);
	if (performance) {
		double checkFlowSeparation = performance->getData()->getGeneralOptions().isFlowSeparation();

		pa = thermo::input::Pressure::convert(pa, thermo::input::Pressure::rawToUnit(pressureUnits), thermo::input::Pressure::Pa);

		// Get nozzle exit object
		performance::equilibrium::NozzleSectionConditions* exitSection = performance->getExitSection();
		performance::equilibrium::NozzleSectionConditions* flowSeparationSection = NULL;

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

				flowSeparationSection = performance->solveNozzleSection(condition, conditionType, checkForFreezing, true, pa, false);
			}

		}

		if (phi<=0) {
			// Get performance correction factor
			performance::efficiency::CorrectionFactors* correctionFactors = new performance::efficiency::CorrectionFactors(performance);
			phi = correctionFactors->getOverallEfficiency();
			delete correctionFactors;
		}

		// Specific impulse at defined altitude, m/s
		double IsH = 0;

		if (flowSeparationSection) {
			// Calculate Isp for nozzle with flow separation.
			double p2 = flowSeparationSection->getP();
			double IsH2 = 0.3*(pa - p2)*(flowSeparationSection->getF() - exitSection->getF());
			IsH = flowSeparationSection->getIs_v()*phi - flowSeparationSection->getF()*pa + IsH2;
		} else {
			// Calculate Isp for nozzle without flow separation.
			IsH = exitSection->getIs_v()*phi - exitSection->getF()*pa;
		}

		delete flowSeparationSection;

		if (0==strcmp(IspUnits, "m/s")) {
			return IsH;
		} else if (0==strcmp(IspUnits, "ft/s")) {
			return thermo::input::Length::convert(IsH, thermo::input::Length::m, thermo::input::Length::ft);
		} else if (0==strcmp(IspUnits, "s")) {
			return IsH/CONST_G;
		}
	}

	return 0;
}

double performanceGetDeliveredIspH(void* performancePtr, const char* IspUnits, double H, const char* altitudeUnits, double phi) {
	StandardAtmosphere sa;
	sa.setAltitude(thermo::input::Length::convert(H, thermo::input::Length::rawToUnit(altitudeUnits), thermo::input::Length::m));
	return performanceGetDeliveredIsp(performancePtr, IspUnits, sa.getPressure(), "Pa", phi);
}

double performanceGetIdealIsp(void* performancePtr, const char* IspUnits, double pa, const char* pressureUnits) {
	return performanceGetDeliveredIsp(performancePtr, IspUnits, pa, pressureUnits, 1.0);
}

double performanceGetIdealIspH(void* performancePtr, const char* IspUnits, double H, const char* altitudeUnits) {
	StandardAtmosphere sa;
	sa.setAltitude(thermo::input::Length::convert(H, thermo::input::Length::rawToUnit(altitudeUnits), thermo::input::Length::m));
	return performanceGetDeliveredIsp(performancePtr, IspUnits, sa.getPressure(), "Pa", 1.0);
}


double performanceGetOF(void* performancePtr) {
	performance::TheoreticalPerformance* performance = reinterpret_cast<performance::TheoreticalPerformance*>(performancePtr);
	if (performance) {
		return performance->getPropellant()->getKm();
	}
	return 0;
}

void performancePrint(void* performancePtr) {
	performance::TheoreticalPerformance* performance = reinterpret_cast<performance::TheoreticalPerformance*>(performancePtr);
	if (performance) {
		performance->printResults(0);
	}
}

//*****************************************************************************

void* chamberCreate(void* performancePtr, bool applyCorrectionFactors) {
	performance::TheoreticalPerformance* performance = reinterpret_cast<performance::TheoreticalPerformance*>(performancePtr);
	if (performance) {
		thermo::input::ConfigFile* data = performance->getData();

		if (data->getEngineSize().isThrustSet() || data->getEngineSize().isMdotSet() || data->getEngineSize().isThroatDSet()) {

			// Design parameters of combustion chamber
			double b = data->getEngineSize().getChamberGeometry().getContractionAngle() * M_PI / 180.;
			double R1 = data->getEngineSize().getChamberGeometry().getR1ToRtRatio();
			double R2 = data->getEngineSize().getChamberGeometry().getR2ToR2maxRatio();
			double Lstar = data->getEngineSize().getChamberGeometry().getChamberLength();
			bool isLstar = data->getEngineSize().getChamberGeometry().isCharacteristicLength();


			performance::efficiency::CorrectionFactors* correctionFactors = NULL;
			if (applyCorrectionFactors) {
				correctionFactors = new performance::efficiency::CorrectionFactors(performance);
			}

			design::Chamber* chamber = new design::Chamber(performance, correctionFactors);

			if (data->getEngineSize().isThrustSet()) {
				chamber->setThrust(data->getEngineSize().getThrust(true) / (double)data->getEngineSize().getChambersNo(), data->getEngineSize().getAmbientPressure(true));
			} else
			if (data->getEngineSize().isMdotSet()) {
				chamber->setMdot(data->getEngineSize().getMdot(true) / (double)data->getEngineSize().getChambersNo());
			} else
			if (data->getEngineSize().isThroatDSet()) {
				chamber->setDt(data->getEngineSize().getThroatD(true));
			}

			chamber->setB(b);
			chamber->setR1toRt(R1);
			chamber->setR2toR2max(R2);
			if (isLstar) {
				chamber->setLstar(Lstar);
			} else {
				// Lstar is actually L'
				chamber->setLc(Lstar);
			}
			chamber->calcGeometry();

			return chamber;
		}
	}
	return NULL;
}

void chamberDelete(void* chamberPtr) {
	if (chamberPtr) {
		delete chamberPtr;
	}
}

double chamberGetThroatDiameter(void* chamberPtr, const char* units) {
	design::Chamber* chamber = reinterpret_cast<design::Chamber*>(chamberPtr);
	if (chamber) {
printf("aaaa %f\n", chamber->getDt());
		return thermo::input::Length::convert(chamber->getDt(), thermo::input::Length::m, thermo::input::Length::rawToUnit(units));
	}
	return 0;
}

double chamberGetThrust(void* chamberPtr, const char* units) {
	design::Chamber* chamber = reinterpret_cast<design::Chamber*>(chamberPtr);
	if (chamber) {
		return thermo::input::Force::convert(chamber->getThrust(), thermo::input::Force::N, thermo::input::Force::rawToUnit(units));
	}
	return 0;
}

double chamberGetMdot(void* chamberPtr, const char* units) {
	design::Chamber* chamber = reinterpret_cast<design::Chamber*>(chamberPtr);
	if (chamber) {
		return thermo::input::MassFlowRate::convert(chamber->getMdot(), thermo::input::MassFlowRate::kg_over_s, thermo::input::MassFlowRate::rawToUnit(units));
	}
	return 0;
}

double chamberGetMdotOxidizer(void* chamberPtr, const char* units) {
	design::Chamber* chamber = reinterpret_cast<design::Chamber*>(chamberPtr);
	if (chamber) {
		return thermo::input::MassFlowRate::convert(chamber->getMdotOx(), thermo::input::MassFlowRate::kg_over_s, thermo::input::MassFlowRate::rawToUnit(units));
	}
	return 0;
}

double chamberGetMdotFuel(void* chamberPtr, const char* units) {
	design::Chamber* chamber = reinterpret_cast<design::Chamber*>(chamberPtr);
	if (chamber) {
		return thermo::input::MassFlowRate::convert(chamber->getMdotF(), thermo::input::MassFlowRate::kg_over_s, thermo::input::MassFlowRate::rawToUnit(units));
	}
	return 0;
}

//*****************************************************************************

void* nozzleCreate(void* chamberPtr, bool applyCorrectionFactors) {
	design::Chamber* chamber = reinterpret_cast<design::Chamber*>(chamberPtr);
	if (chamber) {
		design::Nozzle* nozzle = NULL;

		performance::TheoreticalPerformance* performance = chamber->getPerformance();
		thermo::input::ConfigFile* data = performance->getData();

		double Rn = data->getEngineSize().getChamberGeometry().getRnToRtRatio();
		double R1 = data->getEngineSize().getChamberGeometry().getR1ToRtRatio();

		if (data->getNozzleFlowOptions().isEfficiencyFactorsSet() && data->getNozzleFlowOptions().getEfficiencyFactors().isConeHalfAngleSet()) {
			double te = !data->getNozzleFlowOptions().getEfficiencyFactors().isConeHalfAngleSet()?7.5:data->getNozzleFlowOptions().getEfficiencyFactors().getConeHalfAngle();

			nozzle = new design::ConicalNozzle(chamber);
			static_cast<design::ConicalNozzle*>(nozzle)->calcGeometry(te, Rn);

		} else if (data->getEngineSize().getChamberGeometry().isTOC()) {

			// MOC TIC
			nozzle = new design::MocNozzle(chamber);

			double Tw_bar = data->getEngineSize().getChamberGeometry().isTw_to_T0()?data->getEngineSize().getChamberGeometry().getTwToT0():0.2;

			static_cast<design::MocNozzle*>(nozzle)->calcGeometryMaxThrust(R1, Rn, Tw_bar);

		} else {
			// Parabolic nozzle

			reaction::Derivatives *throatDerivatives = performance->getChamber()->getDerivatives(performance::equilibrium::Chamber::THROAT);

			performance::equilibrium::NozzleSectionConditions *exitSection = dynamic_cast<performance::equilibrium::NozzleSectionConditions*>(performance->getExitSection());
			double Me = exitSection->getMach();
			double rho_e = exitSection->getRho();
			double w_e = exitSection->getW();
			double p_e = exitSection->getP();
			double k_e = exitSection->getK();

			// Ambient pressure
			double p_a = 0;

			double mu_e = asin(1./Me);

			// Nozzle initial angle (degrees)
			double tn = data->getEngineSize().getChamberGeometry().isParabolicInitialAngle()?data->getEngineSize().getChamberGeometry().getParabolicInitialAngle():-1.0;
			// Estimate nozzle initial angle
			if (tn<0) {
				double k = throatDerivatives->getK();
				double K = sqrt((k+1.)/(k-1.));
				tn = 60.*( K*atan(sqrt(Me*Me-1.)/K) - atan(Me*Me-1.) ) / M_PI;
			}

			// Nozzle exit angle (degrees)
			double te = data->getEngineSize().getChamberGeometry().isParabolicExitAngle()?data->getEngineSize().getChamberGeometry().getParabolicExitAngle():-1.0;
			// Estimate nozzle exit angle
			if (te<0) {
				te = 90.*asin(2.*(p_e-p_a)/(rho_e*w_e*w_e*tan(mu_e)))/M_PI;
				if (te<8.) {
					te = 8.;
				}
				if (te>tn) {
					te = 0.9*tn;
				}
			}

			double l = 0.8;
			if (data->getNozzleFlowOptions().isEfficiencyFactorsSet() && data->getNozzleFlowOptions().getEfficiencyFactors().isNozzleLengthSet()) {
				l = data->getNozzleFlowOptions().getEfficiencyFactors().getNozzleLength()/100.;
			}

			nozzle = new design::ParabolicNozzle(chamber);
			static_cast<design::ParabolicNozzle*>(nozzle)->calcGeometry(tn, te, R1, Rn, l);
		}

		return nozzle;
	}
	return NULL;
}

void nozzleDelete(void* nozzlePtr) {
	if (nozzlePtr) {
		delete nozzlePtr;
	}
}

double nozzleGetExitDiameter(void* nozzlePtr, const char* units) {
	design::Nozzle* nozzle = reinterpret_cast<design::Nozzle*>(nozzlePtr);
	if (nozzle) {
		return thermo::input::Length::convert(nozzle->getChamber()->getDe(), thermo::input::Length::m, thermo::input::Length::rawToUnit(units));
	}
	return 0;
}

double nozzleGetLength(void* nozzlePtr, const char* units) {
	design::Nozzle* nozzle = reinterpret_cast<design::Nozzle*>(nozzlePtr);
	if (nozzle) {
		double Le = 0;
		if (dynamic_cast<design::ConicalNozzle*>(nozzle)) {
			Le = static_cast<design::ConicalNozzle*>(nozzle)->getL();
		} else
		if (dynamic_cast<design::ParabolicNozzle*>(nozzle)) {
			Le = static_cast<design::ParabolicNozzle*>(nozzle)->getL();
		} else
		if (dynamic_cast<design::MocNozzle*>(nozzle)) {
			Le = static_cast<design::MocNozzle*>(nozzle)->getLe();
		}
		return thermo::input::Length::convert(Le, thermo::input::Length::m, thermo::input::Length::rawToUnit(units));
	}
	return 0;
}


//*****************************************************************************

#ifdef __cplusplus
}
#endif
