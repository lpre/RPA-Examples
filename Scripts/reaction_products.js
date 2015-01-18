/***************************************************
 RPA - Tool for Rocket Propulsion Analysis
 Copyright 2009-2014 Alexander Ponomarenko
 Please contact author <contact@propulsion-analysis.com> 
 or visit http://www.propulsion-analysis.com
 if you need additional information or have any questions.
 
 reaction.js
 
****************************************************/

load("resources/scripts/printf.js");


prop = Propellant();
prop.setRatio(6.0, "O/F");	// Set O/F weight ratio
prop.addOxidizer("O2(L)");	// Add oxidizer at it's normal temperature and atmospheric pressure
prop.addFuel("H2(L)");		// Add fuel at it's normal temperature and atmospheric pressure

// Print out propellant information
prop.print("US");

r = Reaction(prop);
r.setPH(10, "MPa", prop.getH("J/mol"), "J/mol");
r.solve();

d = Derivatives(r);

products = r.getResultingMixture();

// Reaction products total number of moles
// The absolute number does not matter, and only used for calculation of mole fraction
totalMoles = 0;
for (i=0; i<products.size(); ++i) {
	totalMoles += products.getSpecies(i).getN();
}

// Reaction products total mass (kg)
// The absolute number does not matter, and only used for calculation of mass fraction
totalMass = totalMoles*d.getM()/1000;

printf("%15s %9s %9s %4s", "Name", "Mass Frac", "Mole Frac", "Cond");

sum1 = 0;
sum2 = 0; 

for (i=0; i<products.size(); ++i) {
	// Reaction product
	s = products.getSpecies(i);

	// Mass of reaction product (kg)
	// The absolute number does not matter, and only used for calculation of mass fraction 
	mass = s.getN()*s.getM()/1000;
	
	massFraction = mass/totalMass; 
	moleFraction = s.getN()/totalMoles; 
	
	sum1 += massFraction;
	sum2 += moleFraction;
	
	// We are printing out mass fraction in  format "%9.7f",
	// so skip all products with massFraction<1e-7
	if (massFraction<1e-7) {
		continue;
	}
	
	printf(
		"%15s %9.7f %9.7f %4d",
		s.getName(),
		massFraction,
		moleFraction,
		s.getCondensed()
	);

}

printf(
	"%15s %9.7f %9.7f",
	"Summ:",
	sum1,
	sum2
);

