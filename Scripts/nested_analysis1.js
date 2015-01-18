
// Open the  file "log.txt" in the mode "w" ("write")
var f = new File("C:\path-to-output-logs\log.txt", "w");  

load("resources/scripts/printf.js");

// Load configuration file "examples/RD-275.cfg".
c = ConfigFile("examples/RD-275.cfg");
c.read();

f.printf("# Configuration file: %s\n\n", configName);

// Create Performance object, initializing it with loaded configuration.
p = Performance(c);

// Array of O/F weight ratios
r = [2.5, 2.6, 2.7, 2.8, 2.9, 3.0, 3.1];

// Define an array with different expansion area ratios.
a = [10, 20, 30, 40];

// Print out table header
f.printf("#%4s %5s %8s %8s %8s", "r", "A/At", "Is_v,s", "Is_opt,s", "Is_sl,s");

// Solve the performance problem for each ratio in the array.
for (i=0; i<r.length; ++i) {
	// Assign new O/F weight ratio, replacing preconfigured one.
    p.getPropellant().setRatio(r[i], "O/F");

	// Solve the combustion problem for given O/F ratio.
    p.solve();

	// Calculate performamce for each area ratio in the array.
	for (j=0; j<a.length; ++j) {
		s = p.solveNozzleSection(a[j], "A/At");
	
		// Print out current O/F weight ratio and calculated specific impulse in vacuum, at optimum expansion, and at sea level.
		f.printf(" %4.2f %5.2f %8.2f %8.2f %8.2f", 
			r[i], a[j],
			s.getIs_v("s"),
			s.getIs("s"),
			s.getIs_H(1, "atm", "s")
		);
	}
	
	// Prepare the solver for restart.
    p.clearForRestart();
}


// Close the file
f.close();
