load("utils.js");

// Read configuration file

conf = ConfigFile("examples/cycle_analysis/RD-275.cfg");
conf.read();

// Solve engine performance problem

perf = Performance(conf);
perf.solve();

//perf.printResults();

engineSizeConf = conf.getEngineSize();
chamberConf = conf.getChamberGeometry();

// Design combustion chamber 
chamber = CombustionChamberContour(perf, CorrectionFactors(perf));

// Configure the chamber

if (engineSizeConf.isThrust()) {
	chamber.setThrust(
		engineSizeConf.getThrust("N"), "N", 
		engineSizeConf.isAmbientPressure()?engineSizeConf.getAmbientPressure("Pa"):0, "Pa"
	);
}
if (engineSizeConf.isMdot()) {
	chamber.setMdot(engineSizeConf.getMdot("kg/s")/engineSizeConf.getChambersNo(), "kg/s");
}
if (engineSizeConf.isThroatD()) {
	chamber.setThroatD(engineSizeConf.getThroatD("m"), "m");
}

chamber.setContractionAngle(chamberConf.getContractionAngle("degrees"), "degrees");
chamber.setR1ToRtRatio(chamberConf.getR1ToRtRatio());
chamber.setR2toR2max(chamberConf.getR2ToR2maxRatio());
if (chamberConf.isCharacteristicChamberLength()) {
	chamber.setLstar(chamberConf.getChamberLength("m"), "m");
} else {
	chamber.setLc(chamberConf.getChamberLength("m"), "m");
}


chamber.calcGeometry();
Utils.printChamberParameters(chamber);

// Design nozzle contour 
nozzle = MocNozzleContour(chamber);
// Configure nozzle via parameters of the function:
nozzle.calcGeometryAtFixedArea(chamber.getFre(), chamber.getDc("m")/chamber.getDt("m"), chamberConf.getRnToRtRatio(), chamberConf.isTwToT0()?chamberConf.getTwToT0():0.3);
Utils.printNozzleParameters(chamber, nozzle, 0);

// Solve cycle analysis problem
cycle = EngineCycle(conf, nozzle);
cycle.print();

// Estimate engine performance
p = CyclePerformance(perf, chamber, cycle);
p.print();

// Estimate engine dry mass
m = MassEstimation(conf, cycle, nozzle);
m.print();





