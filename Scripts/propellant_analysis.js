/***************************************************
 RPA - Tool for Rocket Propulsion Analysis
 Copyright 2009-2014 Alexander Ponomarenko
 Please contact author <contact@propulsion-analysis.com> 
 or visit http://www.propulsion-analysis.com
 if you need additional information or have any questions.
 
 propellant_analysis.js
 
****************************************************/

load("resources/scripts/printf.js");

mix = Mixture();
mix.addSpecies("O2(L)", 0.8);					// Add 1st component at it's normal temperature and atmospheric pressure
mix.addSpecies("H2(L)", 0.15);					// Add 2nd component at it's normal temperature and atmospheric pressure
mix.addSpecies("RP-1", 0, "K", 3, "atm", 0.03);	// Add 3rd component at it's normal temperature and pressure 3 atm
mix.addSpecies("AL(cr)", 0.02);					// Add 4th component at it's normal temperature and atmospheric pressure

// Total mass fraction of components #2 (RP-1) and #3 (AL(cr))
sf = mix.getFraction(2) + mix.getFraction(3);

// Array with different values of AL(cr) mass fraction
m = Array();
for (i=0; i<=1.0; i+=0.2) {
	m[m.length] = sf*i;
}

// Print out table header
printf("#%6s %6s %8s %8s %8s", "RP-1", "AL(cr)", "Is_v,s", "Is_opt,s", "Is_sl,s");

for (i=0; i<m.length; ++i) {

	// Change mas fractions of components #2 (RP-1) and #3 (AL(cr))
	mix.setFraction(2, sf - m[i]);
	mix.setFraction(3, m[i]);

	chamber = Chamber(mix);
	chamber.setP(10, "MPa");	// Chamber pressure
	chamber.setFcr(3);			// Nozzle inlet contraction area ratio
	chamber.solve(true);		// finiteChamberSection=true

	nozzleExit = NozzleSectionConditions(chamber, 40, "A/At", true);

	printf(" %6.3f %6.3f %8.2f %8.2f %8.2f", 
		mix.getFraction(2),
		mix.getFraction(3),
		nozzleExit.getIs_v("s"),
		nozzleExit.getIs("s"),
		nozzleExit.getIs_H(1, "atm", "s")
	);
	
}

