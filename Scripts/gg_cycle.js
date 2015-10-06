// Example of solving the GG cycle with direct configuration of all parameters


printDesignParams = false;

p_c = 7; // MPa

ox = Mixture();
ox.addSpecies("O2(L)", 1.0);
ox.checkFractions(true);
ox.setT(90, "K");

fuel = Mixture();
fuel.addSpecies("H2(L)", 1.0);
fuel.checkFractions(true);
fuel.setT(21, "K");


paramsOx = ChamberFeedSubsystemParameters("Oxidizer", ox, p_c, "MPa");
paramsOx.setMDot(13, "kg/s");
paramsOx.setPIn(0.7, "MPa");
paramsOx.setVIn(3.0, "m/s");
paramsOx.setPOut(p_c, "MPa");
paramsOx.setPumpEta(0.7);
paramsOx.setValveDeltaP(0.15*p_c, "MPa");
paramsOx.setValveZeta(0);
paramsOx.setInjectorDeltaP(1.5*ComponentFeedSystem.getInjectorPressureDrop(p_c, "MPa"), "MPa");
paramsOx.setInjectorMu(0);

paramsOxBoost = BoostPumpParameters();
paramsOxBoost.setPIn(paramsOx.getPIn("MPa"), "MPa");
paramsOxBoost.setPOut(paramsOx.getPIn("MPa")+0.2, "MPa");
paramsOxBoost.setPumpEta(0.65);
paramsOxBoost.setGasTurbine();
paramsOxBoost.setTurbineEta(0.7);

paramsOxGG = GasGeneratorBranchParameters(p_c, "MPa");
paramsOxGG.setMDot(paramsOx.getMDot()*0.05, "kg/s");
paramsOxGG.setValveDeltaP(1.5, "MPa");
paramsOxGG.setInjectorDeltaP(1.5*ComponentFeedSystem.getInjectorPressureDrop(p_c, "MPa"), "MPa");
paramsOxGG.setInjectorMu(0.7);
paramsOxGG.setConnectToName("pump");


paramsFuel = ChamberFeedSubsystemParameters("Fuel", fuel, p_c, "MPa");
paramsFuel.setMDot(2, "kg/s");
paramsFuel.setPIn(0.7, "MPa");
paramsFuel.setVIn(5.0, "m/s");
paramsFuel.setPOut(p_c, "MPa");
paramsFuel.setPumpEta(0.7);
paramsFuel.setValveDeltaP(0.15*p_c, "MPa");
paramsFuel.setValveZeta(0);
paramsFuel.setCoolingDeltaP(1.5*ComponentFeedSystem.getCoolingPressureDrop(p_c, "MPa"), "MPa");
paramsFuel.setInjectorDeltaP(1.5*ComponentFeedSystem.getInjectorPressureDrop(p_c, "MPa"), "MPa");
paramsFuel.setInjectorMu(0);


paramsFuelGG = GasGeneratorBranchParameters(p_c, "MPa");
paramsFuelGG.setMDot(paramsFuel.getMDot()*0.05, "kg/s");
paramsFuelGG.setValveDeltaP(1.5, "MPa");
paramsFuelGG.setInjectorDeltaP(1.5*ComponentFeedSystem.getInjectorPressureDrop(p_c, "MPa"), "MPa");
paramsFuelGG.setInjectorMu(0.7);
paramsFuelGG.setConnectToName("pump");

paramsGG = GasGeneratorParameters(0.8*p_c, "MPa", 0.97, 1000, "K", "fuel_rich");

paramsT1 = TurbineParameters(2.5, 0.75);
paramsT2 = TurbineParameters(5.0, 0.75);

branch = BranchParameters("test");
branch.setMDot(paramsOx.getMDot("kg/s")*0.1, "kg/s");
branch.setPumpEta(0.7);
branch.setPOut(p_c*2, "MPa");
branch.setFixedDeltaP(1, "MPa");
branch.setConnectToName("pump");

cycle = GasGeneratorCycle(p_c, "MPa", paramsOx, paramsFuel);
cycle.addOxBoostPump(paramsOxBoost);
cycle.addGasGenerator(paramsOxGG, paramsFuelGG, paramsGG);
cycle.addTurbine(paramsT1);
cycle.addTurbine(paramsT2, "parallel");
cycle.addOxBranch(branch);
cycle.setPrintLog(true);

cycle.setPrintLog(false);
cycle.solve();
cycle.setPrintLog(true);

cycle.print();
