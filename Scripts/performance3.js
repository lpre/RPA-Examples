/***************************************************
 RPA - Tool for Rocket Propulsion Analysis
 Copyright 2009-2014 Alexander Ponomarenko
 Please contact author <contact@propulsion-analysis.com> 
 or visit http://www.propulsion-analysis.com
 if you need additional information or have any questions.
 
 performance2.js
 
 This script loads existing configuration file, and runs a number
 of problems,  replacing the preconfigured O/F ratio with values
 from array.
 
****************************************************/

load("resources/scripts/printf.js");

// Load configuration file "examples/RD-275.cfg".
c = ConfigFile("examples/RD-275.cfg");
c.read();

// Create Performance object, initializing it with loaded configuration.
p = Performance(c);

// Array of O/F weight ratios
r = [2.5, 2.6, 2.7, 2.8, 2.9, 3.0, 3.1];


// Print out table header
printf("#%4s %8s %8s %8s", "r", "Is_v,s", "Is_opt,s", "Is_sl,s");


// Solve the performance problem for each ratio in the array.
for (i=0; i<r.length; ++i) {
	// Assign new O/F weight ratio, replacing preconfigured one.
    p.getPropellant().setRatio(r[i], "O/F");

	// Solve the problem.
    p.solve();

	// Print out current O/F weight ratio and calculated specific impulse in vacuum, at optimum expansion, and at sea level.
	printf(" %4.2f %8.2f %8.2f %8.2f", 
		p.getPropellant().getRatio("O/F"), 
		p.getNozzleExitSection().getIs_v("s"),
		p.getNozzleExitSection().getIs("s"),
		p.getNozzleExitSection().getIs_H(1, "atm", "s")
	);

	// Prepare the solver for restart.
    p.clearForRestart();
}

