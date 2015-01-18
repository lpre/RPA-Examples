/***************************************************
 RPA - Tool for Rocket Propulsion Analysis
 Copyright 2009-2014 Alexander Ponomarenko
 Please contact author <contact@propulsion-analysis.com> 
 or visit http://www.propulsion-analysis.com
 if you need additional information or have any questions.
 
 reaction.js

 
****************************************************/

prop = Propellant();
prop.setRatio(6.0, "O/F");						// Set O/F weight ratio
prop.addOxidizer("O2(L)");						// Add oxidizer at it's normal temperature and atmospheric pressure
prop.addFuel("H2(L)", 0.8); 					// Add 1st fuel component at it's normal temperature and atmospheric pressure
prop.addFuel("RP-1", 0, "K", 3, "atm", 0.2);	// Add 2nd fuel component at it's normal temperature and pressure 3 atm 
// The sum "H2(L) mass fraction" (0.8) + "RP-1 mass frcation" (0.2) must be equal to 1.0

// Print out propellant information
prop.print("US");

print("***************************************************");
print("Problem (p,H)=const");
print("***************************************************");

r1 = Reaction(prop);
r1.setPH(10, "MPa", prop.getH("Btu/lb-mol"), "Btu/lb-mol");
r1.solve();

d1 = Derivatives(r1);

// Print out reaction information
r1.print("US");
d1.print("US");

print("***************************************************");
print("Problem (p,T)=const");
print("***************************************************");

r2 = Reaction(prop);
r2.setPT(10, "MPa", 6062.38174, "F", true);	// Set "true" to switch to isothermal problem
r2.solve();

d2 = Derivatives(r2);

// Print out reaction information
r2.print("US");
d2.print("US");

print("***************************************************");
print("Problem (p,S)=const");
print("***************************************************");

r3 = Reaction(prop);
r3.setPS(10, "MPa", 0.050, "Btu/(lb-mol R)");
r3.solve();

d3 = Derivatives(r3);

// Print out reaction information
r3.print("US");
d3.print("US");

