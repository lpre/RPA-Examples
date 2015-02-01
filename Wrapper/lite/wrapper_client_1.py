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
rpa.configFileLoad.argtypes = [ctypes.c_void_p];
rpa.performanceGetOF.argtypes = [ctypes.c_void_p];
rpa.performanceGetOF.restype = ctypes.c_double;
rpa.performanceGetDeliveredIspH.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_double, ctypes.c_char_p, ctypes.c_double];
rpa.performanceGetDeliveredIspH.restype = ctypes.c_double;
rpa.performanceGetDeliveredIsp.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_double, ctypes.c_char_p, ctypes.c_double];
rpa.performanceGetDeliveredIsp.restype = ctypes.c_double;
rpa.chamberGetThroatDiameter.argtypes = [ctypes.c_void_p, ctypes.c_char_p];
rpa.chamberGetThroatDiameter.restype = ctypes.c_double;
rpa.nozzleGetExitDiameter.restype = ctypes.c_double;
rpa.nozzleGetLength.restype = ctypes.c_double;

# Initialize RPA
rpa.initializeWithPath("../resources", None, 1);

# Run analysis
conf = rpa.configFileLoad("RD-170.cfg");
performance = rpa.performanceCreate(conf, 0, 0);
rpa.performanceSolve(performance, 0);

# Calculate combustion chamber and nozzle geometry
chamber = rpa.chamberCreate(performance, 1);
nozzle = rpa.nozzleCreate(chamber, 1);

# Get results
of = rpa.performanceGetOF(performance);
Isp_SL = rpa.performanceGetDeliveredIspH(performance, "s", 0, "m", 0);
Isp_vac = rpa.performanceGetDeliveredIsp(performance, "s", 0, "atm", 0);
Dt = rpa.chamberGetThroatDiameter(chamber, "mm");
De = rpa.nozzleGetExitDiameter(nozzle, "mm");
Le = rpa.nozzleGetLength(nozzle, "mm");

print "O/F=%2.1f" % of;
print "Isp(SL)=%3.2f s, Isp(vac)=%3.2f s" % (Isp_SL, Isp_vac);
print "Dt=%4.2f mm, De=%4.2f mm Le=%4.2f mm" % (Dt, De, Le);

# Release created objects
rpa.nozzleDelete(nozzle);
rpa.chamberDelete(chamber);
rpa.performanceDelete(performance);
rpa.configFileDelete(conf);

# Finalize RPA
rpa.finalize();