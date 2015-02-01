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

# chamber : Incomplete list.
rpa.chamberGetThroatDiameter.argtypes = [ctypes.c_void_p, ctypes.c_char_p];
rpa.chamberGetThroatDiameter.restype = ctypes.c_double;

# nozzle : Incomplete list.
rpa.nozzleGetExitDiameter.restype = ctypes.c_double;
rpa.nozzleGetLength.restype = ctypes.c_double;

# Initialize RPA
rpa.initializeWithPath("../resources", None, 1);

# Run analysis
conf = rpa.configFileLoad("RD-170.cfg");
performance = rpa.performanceCreate(conf, 0, 0);
rpa.performanceSolve(performance, 0);

# Get results
Isp_SL = rpa.performanceGetDeliveredIspH(performance, "s", 0, "m", 0);
Isp_vac = rpa.performanceGetDeliveredIsp(performance, "s", 0, "atm", 0);

print "Isp(SL)=%3.2f s, Isp(vac)=%3.2f s" % (Isp_SL, Isp_vac);

# Calculate Isp at given altitudes (m)
altitude_list = [0, 3e3, 5e3, 10e3, 20e3, 50e3];
for H in altitude_list : 
	Isp = rpa.performanceGetDeliveredIspH(performance, "s", H, "m", 0);
	print "H=%3.0f km: Isp=%3.2f s" % (H/1000, Isp);

# Release created objects
rpa.performanceDelete(performance);
rpa.configFileDelete(conf);

# Finalize RPA
rpa.finalize();