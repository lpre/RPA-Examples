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

 All parameters are obtained from configuration file "examples/cycle_analysis/Vulcain-2.cfg".

****************************************************/

load("utils.js");

solver = Utils.load("examples/cycle_analysis/Vulcain-2.cfg");
solver.run();
solver.print();
