/***************************************************
 RPA - Tool for Rocket Propulsion Analysis
 Copyright 2009-2014 Alexander Ponomarenko
 Please contact author <contact@propulsion-analysis.com> 
 or visit http://www.propulsion-analysis.com
 if you need additional information or have any questions.
 
 frozen.js
 
****************************************************/

load("resources/scripts/printf.js");


prop = Propellant();
prop.setRatio(6.0, "O/F");	// Set O/F weight ratio
prop.addOxidizer("O2(L)");	// Add oxidizer at it's normal temperature and atmospheric pressure
prop.addFuel("H2(L)"); 		// Add fuel at it's normal temperature and atmospheric pressure

// Define chamber to calculate performance with frozen equilibrium flow,
// specifying  nozzle area ratio where shifting equilibrium model swtitched to frozen one
chamber = ChamberFr(prop, true, true, 10, "A/At");
chamber.setP(10, "MPa");	// Chamber pressure
chamber.setFcr(3);			// Nozzle inlet contraction area ratio
chamber.solve(true);		// finiteChamberSection=true

// Get nozzle area ratio where shifting equilibrium model swtitched to frozen one
frozenAt = chamber.getEquilibriumSection().getFr();

// Define an array with different expansion area ratios.
r = [2, 5, 10, 20, 26.2, 40];

// Print out table header
printf("#%5s %8s %8s %8s", "A/At", "Is_v,s", "Is_v,m/s", "Is,ft/s");

// Optional: pre-initialize the inner A/At table for better performance 
NozzleSectionConditions(chamber, frozenAt, "A/At", true)

// Calculate performamce for each area ratio in the array.
for (i=0; i<r.length; ++i) {
	
	s = r[i]>frozenAt ? NozzleSectionConditionsFr(chamber, r[i], "A/At", true) : NozzleSectionConditions(chamber, r[i], "A/At", true);

	// Print out current area  ratio and calculated vacuum specific impulse in s, m/s and ft/s.
	printf(" %5.2f %8.2f %8.2f %8.2f",
		r[i],
		s.getIs_v("s"),
		s.getIs_v("m/s"),
		s.getIs_v("ft/s")
	);
	
}




