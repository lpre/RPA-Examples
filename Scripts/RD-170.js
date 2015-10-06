// Example of solving ox-rich staged combustion cycle (RD-170) 
// with direct configuration of all parameters

p_c = 24.5; // MPa

ox = Mixture();
ox.addSpecies("O2(L)", 1.0);
ox.checkFractions(true);
ox.setT(80.0, "K");

fuel = Mixture();
fuel.addSpecies("RP-1", 1.0);
fuel.checkFractions(true);
fuel.setT(298.15, "K");

paramsOx = ChamberFeedSubsystemParameters("Oxidizer feed subsystem", ox, p_c, "MPa");
paramsOx.setMDot(1728.686, "kg/s");
paramsOx.setPIn(0.8, "MPa");
paramsOx.setVIn(5.0, "m/s");
paramsOx.setPOut(p_c, "MPa");
paramsOx.setPumpEta(0.74);
paramsOx.setValveDeltaP(7.0, "MPa"); 
paramsOx.setInjectorDeltaP(3.0, "MPa"); 

paramsOxBoost = BoostPumpParameters();
paramsOxBoost.setPIn(paramsOx.getPIn());
paramsOxBoost.setPOut(1.2, "MPa");
paramsOxBoost.setPumpEta(0.7);
paramsOxBoost.setGasTurbine(true);
paramsOxBoost.setTurbineEta(0.7);
paramsOxBoost.setTurbinePi(15);


paramsFuel = ChamberFeedSubsystemParameters("Fuel Feed Subsystem", fuel, p_c, "MPa");
paramsFuel.setMDot(657.295, "kg/s");
paramsFuel.setPIn(0.75, "MPa");
paramsFuel.setVIn(5.0, "m/s");
paramsFuel.setPOut(p_c, "MPa");
paramsFuel.setPumpEta(0.74);
paramsFuel.setValveDeltaP(7.0, "MPa");
paramsFuel.setCoolingDeltaP(15.0, "MPa");
paramsFuel.setInjectorDeltaP(2.5, "MPa");

paramsFuelBoost = BoostPumpParameters();
paramsFuelBoost.setPIn(paramsFuel.getPIn());
paramsFuelBoost.setPOut(1.2, "MPa");
paramsFuelBoost.setPumpEta(0.8);
paramsFuelBoost.setHydraulicTurbine(true);
paramsFuelBoost.setTurbineEta(0.7);
paramsFuelBoost.setTurbinePi(1.3);

paramsFuelGG = GasGeneratorBranchParameters(p_c, "MPa");
paramsFuelGG.setABar(0.3);
paramsFuelGG.setMDot(paramsFuel.getMDot()*0.05, "kg/s");
paramsFuelGG.setPumpEta(0.7);
paramsFuelGG.setValveDeltaP(3.0, "MPa");
paramsFuelGG.setInjectorDeltaP(3.0, "MPa");
paramsFuelGG.setFixedDeltaP(5.0, "MPa");
paramsFuelGG.setConnectToName("pump");

paramsGG = GasGeneratorParameters(p_c, "MPa", 0.95, 772, "K", "oxidizer_rich");

paramsT = TurbineParameters(1.5, 0.79);


cycle = StagedCombustionCycle(p_c, "MPa", paramsOx, paramsFuel);
cycle.addOxBoostPump(paramsOxBoost);
cycle.addFuelBoostPump(paramsFuelBoost);
cycle.addPreburner(paramsFuelGG, paramsGG);
cycle.addTurbine(paramsT);

cycle.setPrintLog(false);
cycle.solve();
cycle.setPrintLog(true);

cycle.print();


