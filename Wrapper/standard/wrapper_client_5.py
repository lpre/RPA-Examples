#
# RPA - Tool for Rocket Propulsion Analysis
# RPA wrapper library
#
# Copyright 2009,2014 Alexander Ponomarenko.
# DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
#
# This software is a commercial product; you can use it either
# under the terms of the valid RPA SDK License as published
# at http://www.propulsion-analysis.com/sdk_eula.htm
# or as a complementary tool under the terms of valid RPA Standard Edition License
# as published at http://www.propulsion-analysis.com/lic_standard.htm
# or RPA Lite Edition License as published at http://www.propulsion-analysis.com/lic_lite.htm
#
# This program is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
# PURPOSE. See the applicable RPA SDK License, RPA Standard Edition License or
# RPA Lite Edition License for more details.
#
# You should have received a valid copy of the RPA SDK License, RPA Standard Edition License or
# RPA Lite Edition License along with this program; if not, write to author <contact@propulsion-analysis.com>
#
# Please contact author <contact@propulsion-analysis.com> or visit http://www.propulsion-analysis.com
# if you need additional information or have any questions.
#

import platform
import ctypes

# Load wrapper library 
if platform.system()=="Windows":
	rpa = ctypes.CDLL("libwrapper.dll");
elif platform.system()=="Linux":
	rpa = ctypes.CDLL("libwrapper.so");
else:
	rpa = ctypes.CDLL("libwrapper.dylib");

# Declare used functions
# Attention! 
# The list of functions is incomplete! Please reffer to C incluion files
# and declare other functions you need.
  
rpa.initializeWithPath.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_bool];

# config : Incomplete list.
rpa.configFileLoad.argtypes = [ctypes.c_void_p];

rpa.performanceCreate.argtypes = [ctypes.c_void_p, ctypes.c_bool, ctypes.c_bool];
rpa.performanceCreate.restype = ctypes.c_void_p;
rpa.performanceDelete.argtypes = [ctypes.c_void_p];
rpa.performanceSolve.argtypes = [ctypes.c_void_p, ctypes.c_bool];
rpa.performanceGetOF.argtypes = [ctypes.c_void_p];
rpa.performanceGetOF.restype = ctypes.c_double;
rpa.performanceGetDeliveredIsp.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_double, ctypes.c_char_p, ctypes.c_double];
rpa.performanceGetDeliveredIsp.restype = ctypes.c_double;
rpa.performanceGetDeliveredIspH.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_double, ctypes.c_char_p, ctypes.c_double];
rpa.performanceGetDeliveredIspH.restype = ctypes.c_double;
rpa.performanceGetDeliveredIspThrottled.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_double, ctypes.c_char_p, ctypes.c_double, ctypes.c_double];
rpa.performanceGetDeliveredIspThrottled.restype = ctypes.c_double;
rpa.performanceGetDeliveredIspHThrottled.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_double, ctypes.c_char_p, ctypes.c_double, ctypes.c_double];
rpa.performanceGetDeliveredIspHThrottled.restype = ctypes.c_double;
rpa.performanceGetIdealIsp.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_double, ctypes.c_char_p];
rpa.performanceGetIdealIsp.restype = ctypes.c_double;
rpa.performanceGetIdealIspH.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_double, ctypes.c_char_p];
rpa.performanceGetIdealIspH.restype = ctypes.c_double;
rpa.performanceGetIdealIspThrottled.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_double, ctypes.c_char_p, ctypes.c_double];
rpa.performanceGetIdealIspThrottled.restype = ctypes.c_double;
rpa.performanceGetIdealIspHThrottled.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_double, ctypes.c_char_p, ctypes.c_double];
rpa.performanceGetIdealIspHThrottled.restype = ctypes.c_double;
rpa.performancePrint.argtypes = [ctypes.c_void_p];

# propellant & reaction & derivatives : Incomplete list - see include/Equilibrium.h
rpa.propellantCreateOF.argtypes = [ctypes.c_double];
rpa.propellantCreateOF.restype = ctypes.c_void_p;
rpa.propellantDelete.argtypes = [ctypes.c_void_p];
rpa.propellantAddOxidizerComponent.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_double];
rpa.propellantAddFuelComponent.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_double];
rpa.propellantGetH.argtypes = [ctypes.c_void_p, ctypes.c_char_p];
rpa.propellantGetH.restype = ctypes.c_double;
rpa.propellantGetU.argtypes = [ctypes.c_void_p, ctypes.c_char_p];
rpa.propellantGetU.restype = ctypes.c_double;

rpa.equilibriumCreateFromPropellant.argtypes = [ctypes.c_void_p, ctypes.c_bool, ctypes.c_bool];
rpa.equilibriumCreateFromPropellant.restype = ctypes.c_void_p;
rpa.equilibriumDelete.argtypes = [ctypes.c_void_p];
rpa.equilibriumSetPT.argtypes = [ctypes.c_void_p, ctypes.c_double, ctypes.c_char_p, ctypes.c_double, ctypes.c_char_p];
rpa.equilibriumSetPH.argtypes = [ctypes.c_void_p, ctypes.c_double, ctypes.c_char_p, ctypes.c_double, ctypes.c_char_p];
rpa.equilibriumSetPS.argtypes = [ctypes.c_void_p, ctypes.c_double, ctypes.c_char_p, ctypes.c_double, ctypes.c_char_p];
rpa.equilibriumSetVT.argtypes = [ctypes.c_void_p, ctypes.c_double, ctypes.c_char_p, ctypes.c_double, ctypes.c_char_p];
rpa.equilibriumSetVU.argtypes = [ctypes.c_void_p, ctypes.c_double, ctypes.c_char_p, ctypes.c_double, ctypes.c_char_p];
rpa.equilibriumSetVS.argtypes = [ctypes.c_void_p, ctypes.c_double, ctypes.c_char_p, ctypes.c_double, ctypes.c_char_p];
rpa.equilibriumSolve.argtypes = [ctypes.c_void_p, ctypes.c_bool, ctypes.c_bool];
rpa.equilibriumIsSolved.argtypes = [ctypes.c_void_p];
rpa.equilibriumIsSolved.restype = ctypes.c_bool;
rpa.equilibriumGetT.argtypes = [ctypes.c_void_p, ctypes.c_char_p];
rpa.equilibriumGetT.restype = ctypes.c_double;
rpa.equilibriumGetP.argtypes = [ctypes.c_void_p, ctypes.c_char_p];
rpa.equilibriumGetP.restype = ctypes.c_double;
rpa.equilibriumGetV.argtypes = [ctypes.c_void_p, ctypes.c_char_p];
rpa.equilibriumGetV.restype = ctypes.c_double;
rpa.equilibriumGetH.argtypes = [ctypes.c_void_p, ctypes.c_char_p];
rpa.equilibriumGetH.restype = ctypes.c_double;
rpa.equilibriumGetU.argtypes = [ctypes.c_void_p, ctypes.c_char_p];
rpa.equilibriumGetU.restype = ctypes.c_double;
rpa.equilibriumGetS.argtypes = [ctypes.c_void_p, ctypes.c_char_p];
rpa.equilibriumGetS.restype = ctypes.c_double;

rpa.derivativesGetCp.argtypes = [ctypes.c_void_p, ctypes.c_char_p];
rpa.derivativesGetCp.restype = ctypes.c_double;
rpa.derivativesGetCv.argtypes = [ctypes.c_void_p, ctypes.c_char_p];
rpa.derivativesGetCv.restype = ctypes.c_double;
rpa.derivativesGetR.argtypes = [ctypes.c_void_p, ctypes.c_char_p];
rpa.derivativesGetR.restype = ctypes.c_double;
rpa.derivativesGetK.argtypes = [ctypes.c_void_p];
rpa.derivativesGetK.restype = ctypes.c_double;
rpa.derivativesGetGamma.argtypes = [ctypes.c_void_p];
rpa.derivativesGetGamma.restype = ctypes.c_double;
rpa.derivativesGetA.argtypes = [ctypes.c_void_p, ctypes.c_char_p];
rpa.derivativesGetA.restype = ctypes.c_double;
rpa.derivativesGetP.argtypes = [ctypes.c_void_p, ctypes.c_char_p];
rpa.derivativesGetP.restype = ctypes.c_double;
rpa.derivativesGetV.argtypes = [ctypes.c_void_p, ctypes.c_char_p];
rpa.derivativesGetV.restype = ctypes.c_double;
rpa.derivativesGetRho.argtypes = [ctypes.c_void_p, ctypes.c_char_p];
rpa.derivativesGetRho.restype = ctypes.c_double;
rpa.derivativesGetRhoGas.argtypes = [ctypes.c_void_p, ctypes.c_char_p];
rpa.derivativesGetRhoGas.restype = ctypes.c_double;
rpa.derivativesGetZ.argtypes = [ctypes.c_void_p, ctypes.c_char_p];
rpa.derivativesGetZ.restype = ctypes.c_double;
rpa.derivativesGetCpz_spec.argtypes = [ctypes.c_void_p, ctypes.c_char_p];
rpa.derivativesGetCpz_spec.restype = ctypes.c_double;
rpa.derivativesGetMm.argtypes = [ctypes.c_void_p, ctypes.c_char_p];
rpa.derivativesGetMm.restype = ctypes.c_double;
rpa.derivativesGetMm2.argtypes = [ctypes.c_void_p, ctypes.c_char_p];
rpa.derivativesGetMm2.restype = ctypes.c_double;
rpa.derivativesGetV.argtypes = [ctypes.c_void_p, ctypes.c_char_p];
rpa.derivativesGetV.restype = ctypes.c_double;
rpa.derivativesGetCR.argtypes = [ctypes.c_void_p, ctypes.c_char_p];
rpa.derivativesGetCR.restype = ctypes.c_double;
rpa.derivativesGetCFr.argtypes = [ctypes.c_void_p, ctypes.c_char_p];
rpa.derivativesGetCFr.restype = ctypes.c_double;
rpa.derivativesGetCEql.argtypes = [ctypes.c_void_p, ctypes.c_char_p];
rpa.derivativesGetCEql.restype = ctypes.c_double;
rpa.derivativesGetCpR.argtypes = [ctypes.c_void_p, ctypes.c_char_p];
rpa.derivativesGetCpR.restype = ctypes.c_double;
rpa.derivativesGetCpFr.argtypes = [ctypes.c_void_p, ctypes.c_char_p];
rpa.derivativesGetCpFr.restype = ctypes.c_double;
rpa.derivativesGetCpEql.argtypes = [ctypes.c_void_p, ctypes.c_char_p];
rpa.derivativesGetCpEql.restype = ctypes.c_double;
rpa.derivativesGetPrR.argtypes = [ctypes.c_void_p, ctypes.c_char_p];
rpa.derivativesGetPrR.restype = ctypes.c_double;
rpa.derivativesGetPrFr.argtypes = [ctypes.c_void_p, ctypes.c_char_p];
rpa.derivativesGetPrFr.restype = ctypes.c_double;
rpa.derivativesGetPrEql.argtypes = [ctypes.c_void_p, ctypes.c_char_p];
rpa.derivativesGetPrEql.restype = ctypes.c_double;

# Initialize RPA
rpa.initializeWithPath("../resources", None, 1);

# Define LOX+RP-1 propellant with O/F = 2.5 
prop = rpa.propellantCreateOF(2.5);
rpa.propellantAddOxidizerComponent(prop, "O2(L)", 1.0);
rpa.propellantAddFuelComponent(prop, "RP-1", 1.0);

# Obtain chemical equilibrium at different pressures (MPa)
p_list = [5, 10, 15, 20, 25, 30];
for p in p_list : 
	r = rpa.equilibriumCreateFromPropellant(prop, True, True);
	rpa.equilibriumSetPH(r, p, "MPa", rpa.propellantGetH(prop, "J/mol"), "J/mol");
	rpa.equilibriumSolve(r, False, True);
	T = rpa.equilibriumGetT(r, "K");
	H = rpa.equilibriumGetH(r, "kJ/kg");
	S = rpa.equilibriumGetS(r, "kJ/(kg K)");
	Cp = rpa.derivativesGetCp(r, "kJ/(kg K)");
	Cv = rpa.derivativesGetCv(r, "kJ/(kg K)");
	gamma = rpa.derivativesGetGamma(r);
	k = rpa.derivativesGetK(r);
	rho = rpa.derivativesGetRho(r, "kg/m^3")
	print "p=%6.2f MPa \n\t T=%6.2f K, \n\t H=%6.2f kJ/kg, S=%6.2f kJ/(kg K) \n\t Cp=%6.3f kJ/(kg K), Cv=%6.3f kJ/(kg K), gamma=%6.3f, k=%6.3f \n\t rho=%6.3f" % (p, T, H, S, Cp, Cv, gamma, k, rho);
	rpa.equilibriumDelete(r);

# Release created objects
rpa.propellantDelete(prop);

# Finalize RPA
rpa.finalize();