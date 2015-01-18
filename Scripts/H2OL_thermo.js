
// Enthalpy of propellant
var H = -9311.2836; // kJ/kg


mix = Mixture();
mix.addSpecies("H2O(L)");
// The sum "H2(L) mass fraction" (0.8) + "RP-1 mass frcation" (0.2) must be equal to 1.0

chamber = Chamber(mix);
chamber.setPH(100, "psi", H*mix.getM(), "J/mol");
chamber.setFcr(6);
chamber.solve(true);		

nozzleExit = NozzleSectionConditions(chamber, 100, "A/At", true);


// Print out propellant information
mix.print("SI");  // USe "US" to print out results in US/Imperial system


// Get objects Reaction and Derivatives for injector station (0).
injector_r = chamber.getReaction(0);
injector_d = chamber.getDerivatives(0);

print("***************************************************");
print("Injector");
print("***************************************************");
injector_r.print("SI");  // USe "US" to print out results in US/Imperial system
injector_d.print("SI");

/*
// Uncomment this section if you need tp know all parameters at 
// nozzle inlet, nozzle throat, and nozzle exit
  

// Get objects Reaction and Derivatives for nozzle inlet station (1).
nozzleInlet_r = chamber.getReaction(1);
nozzleInlet_d = chamber.getDerivatives(1);

print("***************************************************");
print("Nozzle Inlet");
print("***************************************************");
nozzleInlet_r.print("SI");  // USe "US" to print out results in US/Imperial system
nozzleInlet_d.print("SI");


// Get objects Reaction and Derivatives for nozzle throat station (2).
throat_r = chamber.getReaction(2);
throat_d = chamber.getDerivatives(2);

print("***************************************************");
print("Nozzle Throat");
print("***************************************************");
throat_r.print("SI");  // USe "US" to print out results in US/Imperial system
throat_d.print("SI");


// Get objects Reaction and Derivatives for nozzle exit.
nozzleExit_r = nozzleExit.getReaction();
nozzleExit_d = nozzleExit.getDerivatives();

print("***************************************************");
print("Nozzle Exit");
print("***************************************************");
nozzleExit_r.print("SI");   // USe "US" to print out results in US/Imperial system
nozzleExit_d.print("SI");

*/


print("***************************************************");
print("Ideal performance");
print("***************************************************");
printf(" %6s=%8.2f s, %6s=%8.4f\n %6s=%8.2f s, %6s=%8.4f\n", 
	"Is_v", nozzleExit.getIs_v("s"), 
	"Cf_v", nozzleExit.getCf_v(),
	"Is_opt", nozzleExit.getIs("s"), 
	"Cf_opt", nozzleExit.getCf()
);

