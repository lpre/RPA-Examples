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
# Attention! 
# The list of functions is incomplete! Please reffer to C incluion files
# and declare other functions you need.
if platform.system()=="Windows":
	rpa = ctypes.CDLL("libwrapper.dll");
elif platform.system()=="Linux":
	rpa = ctypes.CDLL("libwrapper.so");
else:
	rpa = ctypes.CDLL("libwrapper.dylib");

# Declare used functions
rpa.initializeWithPath.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_bool];

# config : Incomplete list.
rpa.configFileLoad.argtypes = [ctypes.c_void_p];
rpa.configFilePropellantSetRatio.argtypes = [ctypes.c_void_p, ctypes.c_double, ctypes.c_char_p];

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

# Initialize RPA
rpa.initializeWithPath("../resources", None, 1);

# Run analysis
conf = rpa.configFileLoad("RD-170.cfg");

# Calculate performance at given O/F ratios
of_ratios = [1.2, 1.5, 1.8, 2.0, 2.2, 2.4, 2.6, 2.8, 3.0];
for of in of_ratios : 
	rpa.configFilePropellantSetRatio(conf, of, "O/F");
	performance = rpa.performanceCreate(conf, False, False);
	rpa.performanceSolve(performance, False);
	# Get results
	Isp_SL = rpa.performanceGetDeliveredIspH(performance, "s", 0, "m", 0);
	Isp_vac = rpa.performanceGetDeliveredIsp(performance, "s", 0, "atm", 0);
	print "O/F=%3.1f: Isp(SL)=%6.2f s, Isp(vac)=%6.2f s" % (of, Isp_SL, Isp_vac);
	# Release created objects
	rpa.performanceDelete(performance)


# Release created objects
rpa.configFileDelete(conf)

# Finalize RPA
rpa.finalize();