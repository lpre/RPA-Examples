/***************************************************
 RPA - Tool for Rocket Propulsion Analysis
 Copyright 2009-2014 Alexander Ponomarenko
 Please contact author <contact@propulsion-analysis.com> 
 or visit http://www.propulsion-analysis.com
 if you need additional information or have any questions.
 
 propellant1.js
 
 
****************************************************/

load("resources/scripts/printf.js");


mix = Mixture();
mix.addSpecies("O2(L)", 0.8);					// Add 1st component at it's normal temperature and atmospheric pressure
mix.addSpecies("H2(L)", 0.02);					// Add 2nd component at it's normal temperature and atmospheric pressure
mix.addSpecies("RP-1", 0, "K", 3, "atm", 0.15);	// Add 3rd component at it's normal temperature and pressure 3 atm
mix.addSpecies("AL(cr)", 0.03);					// Add 4th component at it's normal temperature and atmospheric pressure
// The sum "O2(L) mass fraction" (0.8) + "H2(L) mass frcation" (0.02)  + "RP-1 mass fraction" (0.15) + "AL(cr) mass fcraction" (0.03) must be equal to 1.0

chamber = Chamber(mix);
chamber.setP(10, "MPa");	// Chamber pressure
chamber.setFcr(3);			// Nozzle inlet contraction area ratio
chamber.solve(true);		// finiteChamberSection=true


nozzleExit = NozzleSectionConditions(chamber, 40, "A/At", true);


// Get objects Reaction and Derivatives for injector station (0).
injector_r = chamber.getReaction(0);
injector_d = chamber.getDerivatives(0);

// Get objects Reaction and Derivatives for nozzle inlet station (1).
nozzleInlet_r = chamber.getReaction(1);
nozzleInlet_d = chamber.getDerivatives(1);

// Get objects Reaction and Derivatives for nozzle throat station (2).
throat_r = chamber.getReaction(2);
throat_d = chamber.getDerivatives(2);

// Get objects Reaction and Derivatives for nozzle exit.
nozzleExit_r = nozzleExit.getReaction();
nozzleExit_d = nozzleExit.getDerivatives();

// Print out propellant information
mix.print("US");

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
nozzleExit_r.print("US");
nozzleExit_d.print("US");

print("***************************************************");
print("Performance");
print("***************************************************");
printf(" Is_v=%8.2f s\n Is_opt=%8.2f s\n Is_sl=%8.2fs", 
	nozzleExit.getIs_v("s"),
	nozzleExit.getIs("s"),
	nozzleExit.getIs_H(1, "atm", "s")
);
