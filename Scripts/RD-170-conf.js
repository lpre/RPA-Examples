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

 All parameters are obtained from configuration file "examples/cycle_analysis/RD-170.cfg".

****************************************************/

// This script is used for printing out the chamber parameters.
load("utils.js");

//*********************************
// Read configuration file

conf = ConfigFile("examples/cycle_analysis/RD-170.cfg");
conf.read();

//*********************************
// Solve engine performance problem

perf = Performance(conf);
perf.solve();
perf.printResults();

//*********************************
// Design combustion chamber 
chamber = CombustionChamberContour(perf, CorrectionFactors(perf), conf);
Utils.printChamberParameters(chamber);

//*********************************
// Design nozzle contour 
nozzle = NozzleContour(conf, chamber);
Utils.printNozzleParameters(chamber, nozzle, 0);

//*********************************
// Solve cycle analysis problem
cycle = EngineCycle(conf, nozzle);
cycle.print();

p = CyclePerformance(perf, chamber, cycle);
p.print();

//*********************************
// Estimate engine dry mass
m = MassEstimation(conf, cycle, nozzle);
m.print();

