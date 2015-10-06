// SSME

p_c = 22.6148; // MPa

ox = Mixture();
ox.addSpecies("O2(L)", 1.0);
ox.checkFractions(true);
ox.setT(ox.getSpecies(0).getT0(), "K");

fuel = Mixture();
fuel.addSpecies("H2(L)", 1.0);
fuel.checkFractions(true);
fuel.setT(fuel.getSpecies(0).getT0(), "K");


paramsOx = ChamberFeedSubsystemParameters("Oxidizer feed subsystem", ox, p_c, "MPa");
paramsOx.setMDot(440.374, "kg/s");
paramsOx.setPIn(0.7, "MPa");
paramsOx.setVIn(5.0, "m/s");
paramsOx.setPOut(p_c, "MPa");
paramsOx.setPumpEta(0.7);
paramsOx.setValveDeltaP(0.5, "MPa"); 
paramsOx.setInjectorDeltaP(1.0, "MPa"); 

paramsOxBoost = BoostPumpParameters();
paramsOxBoost.setPIn(paramsOx.getPIn());
paramsOxBoost.setPOut(3.0, "MPa");
paramsOxBoost.setPumpEta(0.7);
paramsOxBoost.setHydraulicTurbine(true);
paramsOxBoost.setTurbineEta(0.7);
paramsOxBoost.setTurbinePi(20);

paramsOxGG1 = GasGeneratorBranchParameters(p_c, "MPa");
paramsOxGG1.setABar(0.3);
paramsOxGG1.setPumpEta(0.7);
paramsOxGG1.setValveDeltaP(0.5, "MPa");
paramsOxGG1.setInjectorDeltaP(1.0, "MPa");
paramsOxGG1.setConnectToName("pump");

paramsOxGG2 = GasGeneratorBranchParameters(p_c, "MPa");
paramsOxGG2.setABar(0.3);
paramsOxGG2.setPumpEta(0.7);
paramsOxGG2.setValveDeltaP(0.5, "MPa");
paramsOxGG2.setInjectorDeltaP(1.0, "MPa");
paramsOxGG2.setConnectToName("pump");


paramsFuel = ChamberFeedSubsystemParameters("Fuel Feed Subsystem", fuel, p_c, "MPa");
paramsFuel.setMDot(73.396, "kg/s");
paramsFuel.setPIn(0.2, "MPa");
paramsFuel.setVIn(5.0, "m/s");
paramsFuel.setPOut(p_c, "MPa");
paramsFuel.setPumpEta(0.7);
paramsFuel.setValveDeltaP(0.5, "MPa");
paramsFuel.setInjectorDeltaP(1.0, "MPa");

paramsFuelBoost = BoostPumpParameters();
paramsFuelBoost.setPIn(paramsOx.getPIn());
paramsFuelBoost.setPOut(2.0, "MPa");
paramsFuelBoost.setPumpEta(0.7);
paramsFuelBoost.setHydraulicTurbine(true);
paramsFuelBoost.setTurbineEta(0.7);
paramsFuelBoost.setTurbinePi(20);

paramsFuelGG1 = GasGeneratorBranchParameters(p_c, "MPa");
paramsFuelGG1.setABar(0.3);
paramsFuelGG1.setMDot(paramsFuel.getMDot()*0.2, "kg/s");
paramsFuelGG1.setValveDeltaP(0.5, "MPa");
paramsFuelGG1.setInjectorDeltaP(1.0, "MPa");
paramsFuelGG1.setConnectToName("pump");

branch = BranchParameters("Chamber Cooling");
branch.setABar(0.3);
branch.setMDot(paramsFuel.getMDot()*0.2, "kg/s");
branch.setPumpEta(0.7);
branch.setPOut(p_c, "MPa");
branch.setValveDeltaP(0.5, "MPa");
branch.setCoolingDeltaP(7.0, "MPa");
branch.connectTo = "pump";


paramsGG1 = GasGeneratorParameters(p_c, "MPa", 0.95, 815, "K", "fuel_rich");
paramsGG2 = GasGeneratorParameters(p_c, "MPa", 0.95, 960, "K", "fuel_rich");

paramsT1 = TurbineParameters(2.0, 0.7);
paramsT2 = TurbineParameters(2.0, 0.7);


cycle = StagedCombustionCycle(p_c, "MPa", paramsOx, paramsFuel);
cycle.addOxBoostPump(paramsOxBoost);
cycle.addFuelBoostPump(paramsFuelBoost);
cycle.addPreburners(paramsFuelGG1, paramsOxGG1, paramsGG1, paramsOxGG2, paramsGG2);
cycle.addTurbine(paramsT1);
cycle.addTurbine(paramsT2);
cycle.addFuelBranch(branch);

cycle.setPrintLog(false);
cycle.solve();
cycle.setPrintLog(true);

cycle.print();


