/***************************************************
 RPA - Tool for Rocket Propulsion Analysis
 Copyright 2009-2015 Alexander Ponomarenko
 Please contact author <contact@propulsion-analysis.com> 
 or visit http://www.propulsion-analysis.com
 if you need additional information or have any questions.
 
 This example performes the following tasks:
  - solves engine performance problem
  - designs combustion chamber
  - designs nozzle contour
  - solves cycle analysis problem
  - estimates engine dry mass

 All parameters for engine performance problem are obtained from configuration
 file "examples/cycle_analysis/RD-275.cfg".

 All parameters for other problems are assigned directly in the script.

****************************************************/

// This script is used for printing out the chamber parameters.
load("utils.js");


//*********************************
// Read configuration file

conf = ConfigFile("examples/cycle_analysis/RD-275.cfg");
conf.read();


//*********************************
// Solve engine performance problem

perf = Performance(conf);
perf.solve();

//perf.printResults();


//*********************************
// Design combustion chamber 

engineSizeConf = conf.getEngineSize();
chamberConf = conf.getChamberGeometry();

chamber = CombustionChamberContour(perf, CorrectionFactors(perf));
chamber.setThrust(1747, "kN", 0, "atm");
chamber.setB(30, "degrees");
chamber.setR1toRt(1.5);
chamber.setR2toR2max(0.5);
chamber.setLstar(1, "m");
chamber.calcGeometry();

Utils.printChamberParameters(chamber);


//*********************************
// Design nozzle contour 

nozzle = MocNozzleContour(chamber);
nozzle.calcGeometryAtFixedArea(chamber.getFre(), 1.5, 0.382, 0.3);

Utils.printNozzleParameters(chamber, nozzle, 0);


//*********************************
// Solve cycle analysis problem

p_c = perf.getChamber().getReaction(0).getP("MPa");

ox = perf.getPropellant().getOxidizer(true);
ox.setT(298.15, "K");

fuel = perf.getPropellant().getFuel(true);
fuel.setT(298.15, "K");

paramsOx = ChamberFeedSubsystemParameters("Oxidizer feed subsystem", ox, p_c, "MPa");
paramsOx.setMDot(chamber.getMdotOx("kg/s"), "kg/s");
paramsOx.setPIn(0.3, "MPa");
paramsOx.setVIn(5.0, "m/s");
paramsOx.setPOut(p_c, "MPa");
paramsOx.setPumpEta(0.7);
paramsOx.setValveDeltaP(2.0, "MPa"); 
paramsOx.setInjectorDeltaP(1.0, "MPa"); 

paramsFuel = ChamberFeedSubsystemParameters("Fuel Feed Subsystem", fuel, p_c, "MPa");
paramsFuel.setMDot(chamber.getMdotF("kg/s"), "kg/s");
paramsFuel.setPIn(0.3, "MPa");
paramsFuel.setVIn(5.0, "m/s");
paramsFuel.setPOut(p_c, "MPa");
paramsFuel.setPumpEta(0.7);
paramsFuel.setValveDeltaP(2.0, "MPa");
paramsFuel.setCoolingDeltaP(5.0, "MPa");
paramsFuel.setInjectorDeltaP(1.0, "MPa");

paramsFuelGG = GasGeneratorBranchParameters(p_c, "MPa");
paramsFuelGG.setABar(0.3);
paramsFuelGG.setMDot(paramsFuel.getMDot()*0.05, "kg/s");
paramsFuelGG.setPumpEta(0.7);
paramsFuelGG.setValveDeltaP(1.0, "MPa");
paramsFuelGG.setInjectorDeltaP(0.5, "MPa");
paramsFuelGG.setConnectToName("pump");

paramsGG = GasGeneratorParameters(p_c, "MPa", 0.90, 800, "K", "oxidizer_rich");

paramsT = TurbineParameters(1.5, 0.74);

cycle = StagedCombustionCycle(p_c, "MPa", paramsOx, paramsFuel);
cycle.addPreburner(paramsFuelGG, paramsGG);
cycle.addTurbine(paramsT);

cycle.setPrintLog(false);
cycle.solve();
cycle.setPrintLog(true);

cycle.print();


//*********************************
// Estimate engine performance
p = CyclePerformance(perf, chamber, cycle);
p.print();

//*********************************
// Estimate engine dry mass
m = MassEstimation(cycle, nozzle);

oxFeedSystem = cycle.getComponentFeedSystem(0);
fuelFeedSystem = cycle.getComponentFeedSystem(1);

ox_p_in = oxFeedSystem.getFlowPath().getInletPort().getP("MPa");
ox_p_out = oxFeedSystem.getFlowPath().getElement("pump").getOutletPort().getP("MPa");

fuel_p_in = fuelFeedSystem.getFlowPath().getInletPort().getP("MPa");
fuel_p_out = fuelFeedSystem.getFlowPath().getElement("pump").getOutletPort().getP("MPa");

ox_bp_speed = fuel_bp_speed = 0;
ox_bp_in = ox_bp_out = 0;
fuel_bp_in = fuel_bp_out = 0;

oxBoostPump = oxFeedSystem.getFlowPath().getElement("pump_b");
if (oxBoostPump) {
	ox_bp_speed = 9000;
	ox_bp_in = ox_p_in; 
	ox_p_in = ox_bp_out = oxBoostPump.getOutletPort().getP("MPa");
}

fuelBoostPump = fuelFeedSystem.getFlowPath().getElement("pump_b");
if (fuelBoostPump) {
	fuel_bp_speed = 9000;
	fuel_bp_in = fuel_p_in; 
	fuel_p_in = fuel_bp_out = fuelBoostPump.getOutletPort().getP("MPa");
}

m.setTurbopum(
	13800, "rpm",
	ox_p_in, "MPa",
	ox_p_out, "MPa",
	ox.getRho("kg/m^3"), "kg/m^3",
	13800, "rpm",
	fuel_p_in, "MPa",
	fuel_p_out, "MPa",
	fuel.getRho("kg/m^3"), "kg/m^3",
	true
);

if (ox_bp_speed>0 || fuel_bp_speed>0) {
	m.setBoosterTurbopum(
		ox_bp_speed, "rpm",
		ox_bp_in, "MPa",
		ox_bp_out, "MPa",
		ox.getRho("kg/m^3"), "kg/m^3",
		fuel_bp_speed, "rpm",
		fuel_bp_in, "MPa",
		fuel_bp_out, "MPa",
		fuel.getRho("kg/m^3"), "kg/m^3"
	);
}

m.print();

