/***************************************************
 RPA - Tool for Rocket Propulsion Analysis
 Copyright 2009-2014 Alexander Ponomarenko
 Please contact author <contact@propulsion-analysis.com> 
 or visit http://www.propulsion-analysis.com
 if you need additional information or have any questions.
 
 nested_analysis2.js
 
****************************************************/

load("resources/scripts/printf.js");

// Load configuration file "examples/RD-275.cfg".
c = ConfigFile("examples/RD-275.cfg");
c.read();

// Create Performance object, initializing it with loaded configuration.
p = Performance(c);

// Array with different values of O/F weight ratio
r = [2.4, 2.6, 2.8, 3.0, 3.2, 3.4];
r_unit = "O/F";

// Array with different values of combustion chamber pressure (MPa).
pc = [5, 10, 15, 20]; 
pc_unit = "MPa";

// Nozzle exit area ratio
Aexit = 26;

// Print out table header
printf("#%4s %6s %5s %8s %8s %8s", "r", "pc,MPa", "A/At", "Is_v,s", "Is_opt,s", "Is_sl,s");

// Solve the performance problem for each ratio in the array.
for (i=0; i<r.length; ++i) {
	// Assign new O/F weight ratio, replacing preconfigured one.
	p.getData().getPropellant().setRatio(r[i], r_unit);

	for (j=0; j<pc.length; ++j) {
		// Assign new chamber pressure
		p.getData().getCombustionChamberConditions().setPressure(pc[j], pc_unit);

		// Solve the problem.
		p.solve();

		s = p.solveNozzleSection(Aexit, "A/At");
	
		// Print out current O/F weight ratio and calculated specific impulse in vacuum, at optimum expansion, and at sea level.
		printf(" %4.2f %6.2f %8.2f %8.2f %8.2f", 
			r[i], pc[j],
			p.getNozzleExitSection().getIs_v("s"),
			p.getNozzleExitSection().getIs("s"),
			p.getNozzleExitSection().getIs_H(1, "atm", "s")
		);
		
		// Prepare the solver for restart.
		p.clearForRestart();
	}
}


