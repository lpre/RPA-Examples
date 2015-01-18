/***************************************************
 RPA - Tool for Rocket Propulsion Analysis
 Copyright 2009-2014 Alexander Ponomarenko
 Please contact author <contact@propulsion-analysis.com> 
 or visit http://www.propulsion-analysis.com
 if you need additional information or have any questions.
 
 performance4.js
 
 This script loads existing configuration file, solves the main problem to obtain chamber conditions,
 and then calculates the performance for nozzles with different expansion area ratio.
 
****************************************************/

load("resources/scripts/printf.js");

// Load configuration file "examples/RD-275.cfg".
c = ConfigFile("examples/RD-275.cfg");
c.read();

// Create Performance object, initializing it with loaded configuration, 
// and then solve the problem to get chamber/throat conditions. 
p = Performance(c);
p.solve();

// Print out configured area ratio and corresponding vacuum specific impulse
print("#Configured A/At = "+p.getNozzleExitSection().getFr().toFixed(2)+"  Is_v = "+p.getNozzleExitSection().getIs_v("m/s").toPrecision(7)+" m/s");

// Define an array with different expansion area ratios.
// Note that area ratio 26.2 is equal to the preconfigured one for RD-275.
r = [10, 20, 26.2, 40];

// Print out table header
printf("#%5s %8s %8s %8s", "A/At", "Is_v,s", "Is_v,m/s", "Is,ft/s");

// Calculate performamce for each area ratio in the array.
for (i=0; i<r.length; ++i) {
	s = p.solveNozzleSection(r[i], "A/At");

	// Print out current area  ratio and calculated vacuum specific impulse in s, m/s and ft/s.
	printf(" %5.2f %8.2f %8.2f %8.2f",
		r[i],
		s.getIs_v("s"),
		s.getIs_v("m/s"),
		s.getIs_v("ft/s")
	);
	
}

