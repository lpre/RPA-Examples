/***************************************************
 RPA - Tool for Rocket Propulsion Analysis
 Copyright 2009-2014 Alexander Ponomarenko
 Please contact author <contact@propulsion-analysis.com> 
 or visit http://www.propulsion-analysis.com
 if you need additional information or have any questions.
 
 performance2.js
 
 This script loads existing configuration file, 
 solves the performance problem and prints out the 
 combustion parameters for each station "injector", 
 "nozzle inlet", "nozzle throat" and "nozzle exit" separately.
 
 ****************************************************/

// Load configuration file "examples/RD-275.cfg".
c = ConfigFile("examples/RD-275.cfg");
c.read();

// Create Performance object, initializing it with loaded configuration.
p = Performance(c);

// Solve the problem.
p.solve();

// Get the combustion chamber object.
chamber = p.getChamber();

// Get objects Reaction and Derivatives for injector station (0).
injector_r = chamber.getReaction(0);
injector_d = chamber.getDerivatives(0);

// Get objects Reaction and Derivatives for nozzle inlet station (1).
nozzleInlet_r = chamber.getReaction(1);
nozzleInlet_d = chamber.getDerivatives(1);

// Get objects Reaction and Derivatives for nozzle throat station (2).
throat_r = chamber.getReaction(2);
throat_d = chamber.getDerivatives(2);

// Get objects Reaction and Derivatives for nozzle exit station.
nozzleExit_r = p.getNozzleExitSection().getReaction();
nozzleExit_d = p.getNozzleExitSection().getDerivatives();

// Print out the results in US units.

print("***************************************************");
print("Injector");
print("***************************************************");
injector_r.print("US");
injector_d.print("US");

print("***************************************************");
print("Nozzle Inlet");
print("***************************************************");
nozzleInlet_r.print("US");
nozzleInlet_d.print("US");

print("***************************************************");
print("Nozzle Throat");
print("***************************************************");
throat_r.print("US");
throat_d.print("US");

print("***************************************************");
print("Nozzle Exit");
print("***************************************************");
if (null!=nozzleExit_r) {
	nozzleExit_r.print("US");
	nozzleExit_d.print("US");
} else {
	// Frozen flow model! 
	// Objects "Reaction" and "Derivatives" could not be obtained for nozzle exit station.
	s = p.getNozzleExitSection();
	s.printResults("US");
}
