
#ifndef RPA_API_PYTHON_THERMO_H_
#define RPA_API_PYTHON_THERMO_H_

#ifdef __cplusplus
extern "C" {
#endif

//*****************************************************************************

__declspec(dllexport)
	void initialize(bool consoleOutput);

__declspec(dllexport)
	void finalize();

//*****************************************************************************

__declspec(dllexport)
	void* configFile();

__declspec(dllexport)
	void* configFileLoad(const char* name);

__declspec(dllexport)
	void* configFileClone(void* dataPtr);

__declspec(dllexport)
	void configFileDelete(void* dataPtr);

__declspec(dllexport)
	void configFileSave(void* dataPtr);

__declspec(dllexport)
	void configFileSaveAs(void* dataPtr, const char* name);

__declspec(dllexport)
	bool configFileGeneralOptionsIsMultiphase(void* dataPtr);

__declspec(dllexport)
	void configFileGeneralOptionsSetMultiphase(void* dataPtr, bool value);

__declspec(dllexport)
	bool configFileGeneralOptionsIsIons(void* dataPtr);

__declspec(dllexport)
	void configFileGeneralOptionsSetIons(void* dataPtr, bool value);

__declspec(dllexport)
	bool configFileGeneralOptionsIsFlowSeparation(void* dataPtr);

__declspec(dllexport)
	void configFileGeneralOptionsSetFlowSeparation(void* dataPtr, bool value);

__declspec(dllexport)
	double configFileCombustionChamberConditionsGetPressure(void* dataPtr, const char* units);

__declspec(dllexport)
	void configFileCombustionChamberConditionsSetPressure(void* dataPtr, double value, const char* units);

__declspec(dllexport)
	bool configFileNozzleFlowOptionsIsCalculateNozzleFlow(void* dataPtr);

__declspec(dllexport)
	void configFileNozzleFlowOptionsSetCalculateNozzleFlow(void* dataPtr, bool value);

__declspec(dllexport)
	bool configFileNozzleFlowOptionsIsFreezingConditions(void* dataPtr);

__declspec(dllexport)
	void configFileNozzleFlowOptionsSetFreezingConditions(void* dataPtr, unsigned int type, double value, const char* units);

__declspec(dllexport)
	void configFileNozzleFlowOptionsDeleteFreezingConditions(void* dataPtr);

__declspec(dllexport)
	bool configFileNozzleFlowOptionsIsNozzleInletConditions(void* dataPtr);

__declspec(dllexport)
	void configFileNozzleFlowOptionsSetNozzleInletConditions(void* dataPtr, unsigned int type, double value, const char* units);

__declspec(dllexport)
	void configFileNozzleFlowOptionsDeleteNozzleInletConditions(void* dataPtr);

__declspec(dllexport)
	bool configFileNozzleFlowOptionsIsAmbientConditions(void* dataPtr);

__declspec(dllexport)
	void configFileNozzleFlowOptionsSetAmbientConditions(void* dataPtr, double value, const char* units);

__declspec(dllexport)
	void configFileNozzleFlowOptionsSetAmbientConditionsRange(void* dataPtr, double lowerValue, const char* lowerUnits, double upperValue, const char* upperUnits);

__declspec(dllexport)
	bool configFileNozzleFlowOptionsAmbientConditionsIsDeliveredPerformance(void* dataPtr);

__declspec(dllexport)
	void configFileNozzleFlowOptionsAmbientConditionsSetDeliveredPerformance(void* dataPtr, bool value);

__declspec(dllexport)
	void configFileNozzleFlowOptionsDeleteAmbientConditions(void* dataPtr);

__declspec(dllexport)
	bool configFileNozzleFlowOptionsIsThrottlingConditions(void* dataPtr);

__declspec(dllexport)
	void configFileNozzleFlowOptionsSetThrottlingConditions(void* dataPtr, double value);

__declspec(dllexport)
	void configFileNozzleFlowOptionsSetThrottlingConditionsRange(void* dataPtr, double lowerValue, double upperValue);

__declspec(dllexport)
	bool configFileNozzleFlowOptionsThrottlingConditionsIsDeliveredPerformance(void* dataPtr);

__declspec(dllexport)
	void configFileNozzleFlowOptionsThrottlingConditionsSetDeliveredPerformance(void* dataPtr, bool value);

__declspec(dllexport)
	void configFileNozzleFlowOptionsDeleteThrottlingConditions(void* dataPtr);

__declspec(dllexport)
	bool configFileNozzleFlowOptionsIsEfficiencyFactors(void* dataPtr);

__declspec(dllexport)
	bool configFileNozzleFlowOptionsIsApplyEfficiencyFactors(void* dataPtr);

__declspec(dllexport)
	void configFileNozzleFlowOptionsSetApplyEfficiencyFactors(void* dataPtr, bool value);

__declspec(dllexport)
	bool configFileNozzleFlowOptionsIsCycleEfficiency(void* dataPtr);

__declspec(dllexport)
	void configFileNozzleFlowOptionsSetCycleEfficiency(void* dataPtr, double value);

__declspec(dllexport)
	double configFileNozzleFlowOptionsGetCycleEfficiency(void* dataPtr);

__declspec(dllexport)
	void configFileNozzleFlowOptionsDeleteCycleEfficiency(void* dataPtr);

__declspec(dllexport)
	bool configFileNozzleFlowOptionsIsReactionEfficiency(void* dataPtr);

__declspec(dllexport)
	void configFileNozzleFlowOptionsSetReactionEfficiency(void* dataPtr, double value);

__declspec(dllexport)
	double configFileNozzleFlowOptionsGetReactionEfficiency(void* dataPtr);

__declspec(dllexport)
	void configFileNozzleFlowOptionsDeleteReactionEfficiency(void* dataPtr);

__declspec(dllexport)
	bool configFileNozzleFlowOptionsIsNozzleEfficiency(void* dataPtr);

__declspec(dllexport)
	void configFileNozzleFlowOptionsSetNozzleEfficiency(void* dataPtr, double value);

__declspec(dllexport)
	double configFileNozzleFlowOptionsGetNozzleEfficiency(void* dataPtr);

__declspec(dllexport)
	void configFileNozzleFlowOptionsDeleteNozzleEfficiency(void* dataPtr);

__declspec(dllexport)
	bool configFileNozzleFlowOptionsIsNozzleLength(void* dataPtr);

__declspec(dllexport)
	void configFileNozzleFlowOptionsSetNozzleLength(void* dataPtr, double value);

__declspec(dllexport)
	double configFileNozzleFlowOptionsGetNozzleLength(void* dataPtr);

__declspec(dllexport)
	void configFileNozzleFlowOptionsDeleteNozzleLength(void* dataPtr);

__declspec(dllexport)
	bool configFileNozzleFlowOptionsIsConeHalfAngle(void* dataPtr);

__declspec(dllexport)
	void configFileNozzleFlowOptionsSetConeHalfAngle(void* dataPtr, double value, const char* unit);

__declspec(dllexport)
	double configFileNozzleFlowOptionsGetConeHalfAngle(void* dataPtr, const char* unit);

__declspec(dllexport)
	void configFileNozzleFlowOptionsDeleteConeHalfAngle(void* dataPtr);

__declspec(dllexport)
	void configFileNozzleFlowOptionsDeleteEfficiencyFactors(void* dataPtr);

__declspec(dllexport)
	double configFileNozzlePropellantGetRatio(void* dataPtr);

__declspec(dllexport)
	const char* configFileNozzlePropellantGetRatioType(void* dataPtr);

__declspec(dllexport)
	void configFileNozzlePropellantSetRatio(void* dataPtr, double value, const char* type);

__declspec(dllexport)
	unsigned int configFileNozzlePropellantGetOxidizerListSize(void* dataPtr);

__declspec(dllexport)
	const char* configFileNozzlePropellantGetOxidizerName(void* dataPtr, unsigned int n);

__declspec(dllexport)
	double configFileNozzlePropellantGetOxidizerMassFraction(void* dataPtr, unsigned int n);

__declspec(dllexport)
	double configFileNozzlePropellantGetOxidizerPressure(void* dataPtr, unsigned int n, const char* units);

__declspec(dllexport)
	double configFileNozzlePropellantGetOxidizerTemperature(void* dataPtr, unsigned int n, const char* units);

__declspec(dllexport)
	void configFileNozzlePropellantAddOxidizer(void* dataPtr, const char* name, double massFraction, double pressure, const char* pressureUnits, double temperature, const char* temperatureUnits);

__declspec(dllexport)
	unsigned int configFileNozzlePropellantGetFuelListSize(void* dataPtr);

__declspec(dllexport)
	const char* configFileNozzlePropellantGetFuelName(void* dataPtr, unsigned int n);

__declspec(dllexport)
	double configFileNozzlePropellantGetFuelMassFraction(void* dataPtr, unsigned int n);

__declspec(dllexport)
	double configFileNozzlePropellantGetFuelPressure(void* dataPtr, unsigned int n, const char* units);

__declspec(dllexport)
	double configFileNozzlePropellantGetFuelTemperature(void* dataPtr, unsigned int n, const char* units);

__declspec(dllexport)
	void configFileNozzlePropellantAddFuel(void* dataPtr, const char* name, double massFraction, double pressure, const char* pressureUnits, double temperature, const char* temperatureUnits);

__declspec(dllexport)
	unsigned int configFileNozzlePropellantGetSpeciesListSize(void* dataPtr);

__declspec(dllexport)
	const char* configFileNozzlePropellantGetSpeciesName(void* dataPtr, unsigned int n);

__declspec(dllexport)
	double configFileNozzlePropellantGetSpeciesMassFraction(void* dataPtr, unsigned int n);

__declspec(dllexport)
	double configFileNozzlePropellantGetSpeciesPressure(void* dataPtr, unsigned int n, const char* units);

__declspec(dllexport)
	double configFileNozzlePropellantGetSpeciesTemperature(void* dataPtr, unsigned int n, const char* units);

__declspec(dllexport)
	void configFileNozzlePropellantAddSpecies(void* dataPtr, const char* name, double massFraction, double pressure, const char* pressureUnits, double temperature, const char* temperatureUnits);

__declspec(dllexport)
	bool configFileEngineSizeIsThrust(void* dataPtr);

__declspec(dllexport)
	double configFileEngineSizeGetThrust(void* dataPtr, const char* units);

__declspec(dllexport)
	void configFileEngineSizeSetThrust(void* dataPtr, double value, const char* units);

__declspec(dllexport)
	void configFileEngineSizeDeleteThrust(void* dataPtr);

__declspec(dllexport)
	bool configFileEngineSizeIsAmbientPressure(void* dataPtr);

__declspec(dllexport)
	double configFileEngineSizeGetAmbientPressure(void* dataPtr, const char* units);

__declspec(dllexport)
	void configFileEngineSizeSetAmbientPressure(void* dataPtr, double value, const char* units);

__declspec(dllexport)
	void configFileEngineSizeDeleteAmbientPressure(void* dataPtr);

__declspec(dllexport)
	bool configFileEngineSizeIsMdot(void* dataPtr);

__declspec(dllexport)
	double configFileEngineSizeGetMdot(void* dataPtr, const char* units);

__declspec(dllexport)
	void configFileEngineSizeSetMdot(void* dataPtr, double value, const char* units);

__declspec(dllexport)
	void configFileEngineSizeDeleteMdot(void* dataPtr);

__declspec(dllexport)
	bool configFileEngineSizeIsThroatDiameter(void* dataPtr);

__declspec(dllexport)
	double configFileEngineSizeGetThroatDiameter(void* dataPtr, const char* units);

__declspec(dllexport)
	void configFileEngineSizeSetThroatDiameter(void* dataPtr, double value, const char* units);

__declspec(dllexport)
	void configFileEngineSizeDeleteThroatDiameter(void* dataPtr);

__declspec(dllexport)
	unsigned int configFileEngineSizeGetChambersNo(void* dataPtr);

__declspec(dllexport)
	void configFileEngineSizeSetChambersNo(void* dataPtr, unsigned int value);

__declspec(dllexport)
	double configFileEngineSizeGetChamberLength(void* dataPtr, const char* units);

__declspec(dllexport)
	void configFileEngineSizeSetChamberLength(void* dataPtr, double value, const char* units, unsigned int type);

__declspec(dllexport)
	unsigned int configFileEngineSizeGetChamberLengthType(void* dataPtr);

__declspec(dllexport)
	double configFileEngineSizeGetContractionAngle(void* dataPtr, const char* unit);

__declspec(dllexport)
	void configFileEngineSizeSetContractionAngle(void* dataPtr, double value, const char* unit);

__declspec(dllexport)
	double configFileEngineSizeGetR1ToRtRatio(void* dataPtr);

__declspec(dllexport)
	void configFileEngineSizeSetR1ToRtRatio(void* dataPtr, double value);

__declspec(dllexport)
	double configFileEngineSizeGetRnToRtRatio(void* dataPtr);

__declspec(dllexport)
	void configFileEngineSizeSetRnToRtRatio(void* dataPtr, double value);

__declspec(dllexport)
	double configFileEngineSizeGetR2ToR2maxRatio(void* dataPtr);

__declspec(dllexport)
	void configFileEngineSizeSetR2ToR2maxRatio(void* dataPtr, double value);

__declspec(dllexport)
	bool configFileEngineSizeIsTIC(void* dataPtr);

__declspec(dllexport)
	void configFileEngineSizeSetTIC(void* dataPtr, bool value);

__declspec(dllexport)
	void configFileEngineSizeDeleteTIC(void* dataPtr);

__declspec(dllexport)
	bool configFileEngineSizeIsTwToT0(void* dataPtr);

__declspec(dllexport)
	double configFileEngineSizeGetTwToT0(void* dataPtr);

__declspec(dllexport)
	void configFileEngineSizeSetTwToT0(void* dataPtr, double value);

__declspec(dllexport)
	void configFileEngineSizeDeleteTwToT0(void* dataPtr);

__declspec(dllexport)
	bool configFileEngineSizeIsParabolicInitialAngle(void* dataPtr);

__declspec(dllexport)
	double configFileEngineSizeGetParabolicInitialAngle(void* dataPtr, const char* units);

__declspec(dllexport)
	void configFileEngineSizeSetParabolicInitialAngle(void* dataPtr, double value, const char* units);

__declspec(dllexport)
	void configFileEngineSizeDeleteParabolicInitialAngle(void* dataPtr);

__declspec(dllexport)
	bool configFileEngineSizeIsParabolicExitAngle(void* dataPtr);

__declspec(dllexport)
	double configFileEngineSizeGetParabolicExitAngle(void* dataPtr, const char* units);

__declspec(dllexport)
	void configFileEngineSizeSetParabolicExitAngle(void* dataPtr, double value, const char* units);

__declspec(dllexport)
	void configFileEngineSizeDeleteParabolicExitAngle(void* dataPtr);

//*****************************************************************************

__declspec(dllexport)
	void* performanceCreate(void* dataPtr, bool solve, bool optimizePropellant);

__declspec(dllexport)
	void performanceDelete(void* performancePtr);

__declspec(dllexport)
	void performanceSolve(void* performancePtr, bool optimizePropellant);

__declspec(dllexport)
	double performanceGetOF(void* performancePtr);

/**
 * @param performancePtr pointer to performance object
 * @param IspUnits return Isp using given units: "m/s", "ft/s" or "s"
 * @param pa ambient pressure in defined units
 * @param pressureUnits units of ambient pressure
 * @param phi correction factor; if phi==0, use correction factors defined in configuration file, or estimated factor, if not defined in configuration file.
 */
__declspec(dllexport)
	double performanceGetDeliveredIsp(void* performancePtr, const char* IspUnits, double pa, const char* pressureUnits, double phi);

__declspec(dllexport)
	double performanceGetDeliveredIspH(void* performancePtr, const char* IspUnits, double H, const char* altitudeUnits, double phi);

__declspec(dllexport)
	double performanceGetIdealIsp(void* performancePtr, const char* IspUnits, double pa, const char* units);

__declspec(dllexport)
	double performanceGetIdealIspH(void* performancePtr, const char* IspUnits, double H, const char* altitudeUnits);

__declspec(dllexport)
	void performancePrint(void* performancePtr);

//*****************************************************************************

__declspec(dllexport)
	void* chamberCreate(void* performancePtr, bool applyCorrectionFactors);

__declspec(dllexport)
	void chamberDelete(void* chamberPtr);

__declspec(dllexport)
	double chamberGetThroatDiameter(void* chamberPtr, const char* units);

__declspec(dllexport)
	double chamberGetThrust(void* chamberPtr, const char* units);

__declspec(dllexport)
	double chamberGetMdot(void* chamberPtr, const char* units);

__declspec(dllexport)
	double chamberGetMdotOxidizer(void* chamberPtr, const char* units);

__declspec(dllexport)
	double chamberGetMdotFuel(void* chamberPtr, const char* units);

//*****************************************************************************

__declspec(dllexport)
	void* nozzleCreate(void* chamberPtr, bool applyCorrectionFactors);

__declspec(dllexport)
	void nozzleDelete(void* nozzlePtr);

__declspec(dllexport)
	double nozzleGetExitDiameter(void* nozzlePtr, const char* units);

__declspec(dllexport)
	double nozzleGetLength(void* nozzlePtr, const char* units);

//*****************************************************************************


#ifdef __cplusplus
}
#endif

#endif /* RPA_API_PYTHON_THERMO_H_ */
