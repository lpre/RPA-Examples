#
# RPA - Tool for Rocket Propulsion Analysis
# RPA wrapper library
#
# Copyright 2009,2015 Alexander Ponomarenko.
#
#
# This is an example Octave file which loads and uses RPA Wrapper functions provided via rpa.oct (compiled from rpa.cpp).
#

autoload("rpaInit", "rpa.oct")
autoload("rpaFinalize", "rpa.oct")    

autoload("rpaConfigOpen", "rpa.oct")  
autoload("rpaConfigClose", "rpa.oct")  
autoload("rpaConfigGetPc", "rpa.oct")  
autoload("rpaConfigSetPc", "rpa.oct")  

autoload("rpaPerformance", "rpa.oct")  
autoload("rpaPerformanceDelete", "rpa.oct")  
autoload("rpaPerformanceGetIsp", "rpa.oct")  
autoload("rpaPerformanceGetIdealIsp", "rpa.oct")  

rpaInit(false);

name = "examples/RD-170.cfg";

disp("Loading "), disp(name) 
c = rpaConfigOpen(name);

disp(""), disp("Create performance object from loaded configuration file") 
p = rpaPerformance(c);

disp(""), disp("Get Isp (vac)") 
[Is_v, Is_v_units] = rpaPerformanceGetIsp(p)

disp(""), disp("Get Isp (SL) at throttleLevel=0.75") 
[Is_a, Is_a_units, p_a, p_a_units, t, phi] = rpaPerformanceGetIsp(p, 1, "atm", 0.75)
rpaPerformanceDelete(p);

disp(""), disp("Modify pc in configuration file") 

rpaConfigSetPc(c, rpaConfigGetPc(c)*0.75)

disp(""), disp("Create performance object from modified configuration file") 
p = rpaPerformance(c);
disp(""), disp("Get Isp (vac)") 
[Is_v, Is_v_units] = rpaPerformanceGetIsp(p, "s")
rpaPerformanceDelete(p);

disp(""), disp("Close configuration file and finalize RPA") 
rpaConfigClose(c);
rpaFinalize();
