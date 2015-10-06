// Example of solving GG-cycle (Vulcaun-2) with
// direct configuration of all parameters

p_c = 11.73; // MPa

ox = Mixture();
ox.addSpecies("O2(L)", 1.0);
ox.checkFractions(true);
ox.setT(90.17, "K");

fuel = Mixture();
fuel.addSpecies("H2(L)", 1.0);
fuel.checkFractions(true);
fuel.setT(20.27, "K");

paramsOx = ChamberFeedSubsystemParameters("Oxidizer feed subsystem", ox, p_c, "MPa");
paramsOx.setMDot(280.74, "kg/s");
paramsOx.setPIn(0.7, "MPa");
paramsOx.setVIn(5.0, "m/s");
paramsOx.setPOut(p_c, "MPa");
paramsOx.setPumpEta(0.7);
paramsOx.setValveDeltaP(0.5, "MPa"); 
paramsOx.setInjectorDeltaP(0.5, "MPa"); 

paramsOxGG = GasGeneratorBranchParameters(p_c, "MPa");
paramsOxGG.setValveDeltaP(1.0, "MPa");
paramsOxGG.setInjectorDeltaP(0.5, "MPa");
//ramsOxGG.setConnectToName("pump");


paramsFuel = ChamberFeedSubsystemParameters("Fuel Feed Subsystem", fuel, p_c, "MPa");
paramsFuel.setMDot(41.90, "kg/s");
paramsFuel.setPIn(0.3, "MPa");
paramsFuel.setVIn(5.0, "m/s");
paramsFuel.setPOut(p_c, "MPa");
paramsFuel.setPumpEta(0.7);
paramsFuel.setValveDeltaP(0.5, "MPa");
paramsFuel.setCoolingDeltaP(5, "MPa"); // 1.5*ComponentFeedSystem.getCoolingPressureDrop(p_c, "MPa")
paramsFuel.setInjectorDeltaP(0.5, "MPa");


paramsFuelGG = GasGeneratorBranchParameters(p_c, "MPa");
paramsFuelGG.setValveDeltaP(1.0, "MPa");
paramsFuelGG.setInjectorDeltaP(0.5, "MPa");
paramsFuelGG.setConnectToName("pump");

paramsGG = GasGeneratorParameters(10.1, "MPa", 0.90, 875, "K", "fuel_rich");

paramsT1 = TurbineParameters(12.0, 0.7);
paramsT2 = TurbineParameters(18.0, 0.7);


cycle = GasGeneratorCycle(p_c, "MPa", paramsOx, paramsFuel);
cycle.addGasGenerator(paramsOxGG, paramsFuelGG, paramsGG);
cycle.addTurbine(paramsT1);
cycle.addTurbine(paramsT2, "parallel");
cycle.setPrintLog(false);

cycle.setPrintLog(false);
cycle.solve();
cycle.setPrintLog(true);
cycle.print();

