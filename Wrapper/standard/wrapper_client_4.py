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

# nozzle station
rpa.nozzleStationCreate.argtypes = [ctypes.c_void_p, ctypes.c_double, ctypes.c_char_p, ctypes.c_int, ctypes.c_bool, ctypes.c_bool, ctypes.c_double, ctypes.c_char_p, ctypes.c_bool];
rpa.nozzleStationCreate.restype = ctypes.c_void_p;
rpa.nozzleStationDelete.argtypes = [ctypes.c_void_p];
rpa.nozzleStationGetP.argtypes = [ctypes.c_void_p, ctypes.c_char_p];
rpa.nozzleStationGetP.restype = ctypes.c_double;
rpa.nozzleStationGetAr.restype = ctypes.c_double;
rpa.nozzleStationGetPi.restype = ctypes.c_double;
rpa.nozzleStationGetW.argtypes = [ctypes.c_void_p, ctypes.c_char_p];
rpa.nozzleStationGetW.restype = ctypes.c_double;
rpa.nozzleStationGetMach.argtypes = [ctypes.c_void_p];
rpa.nozzleStationGetMach.restype = ctypes.c_double;
rpa.nozzleStationGetF.restype = ctypes.c_double;
rpa.nozzleStationGetMr.argtypes = [ctypes.c_void_p, ctypes.c_char_p];
rpa.nozzleStationGetMr.restype = ctypes.c_double;
rpa.nozzleStationGetT.argtypes = [ctypes.c_void_p, ctypes.c_char_p];
rpa.nozzleStationGetT.restype = ctypes.c_double;
rpa.nozzleStationGetH.argtypes = [ctypes.c_void_p, ctypes.c_char_p];
rpa.nozzleStationGetH.restype = ctypes.c_double;
rpa.nozzleStationGetS.argtypes = [ctypes.c_void_p, ctypes.c_char_p];
rpa.nozzleStationGetS.restype = ctypes.c_double;
rpa.nozzleStationGetCp.argtypes = [ctypes.c_void_p, ctypes.c_char_p];
rpa.nozzleStationGetCp.restype = ctypes.c_double;
rpa.nozzleStationGetCv.argtypes = [ctypes.c_void_p, ctypes.c_char_p];
rpa.nozzleStationGetCv.restype = ctypes.c_double;
rpa.nozzleStationGetR.argtypes = [ctypes.c_void_p, ctypes.c_char_p];
rpa.nozzleStationGetR.restype = ctypes.c_double;
rpa.nozzleStationGetMm.argtypes = [ctypes.c_void_p, ctypes.c_char_p];
rpa.nozzleStationGetMm.restype = ctypes.c_double;
rpa.nozzleStationGetK.restype = ctypes.c_double;
rpa.nozzleStationGetA.argtypes = [ctypes.c_void_p, ctypes.c_char_p];
rpa.nozzleStationGetA.restype = ctypes.c_double;
rpa.nozzleStationGetRho.argtypes = [ctypes.c_void_p, ctypes.c_char_p];
rpa.nozzleStationGetRho.restype = ctypes.c_double;
rpa.nozzleStationGetV.argtypes = [ctypes.c_void_p, ctypes.c_char_p];
rpa.nozzleStationGetV.restype = ctypes.c_double;
rpa.nozzleStationGetCFr.argtypes = [ctypes.c_void_p, ctypes.c_char_p];
rpa.nozzleStationGetCFr.restype = ctypes.c_double;
rpa.nozzleStationGetCpFr.argtypes = [ctypes.c_void_p, ctypes.c_char_p];
rpa.nozzleStationGetCpFr.restype = ctypes.c_double;
rpa.nozzleStationGetPrFr.argtypes = [ctypes.c_void_p, ctypes.c_char_p];
rpa.nozzleStationGetPrFr.restype = ctypes.c_double;
rpa.nozzleStationGetCEql.argtypes = [ctypes.c_void_p, ctypes.c_char_p];
rpa.nozzleStationGetCEql.restype = ctypes.c_double;
rpa.nozzleStationGetCpEql.argtypes = [ctypes.c_void_p, ctypes.c_char_p];
rpa.nozzleStationGetCpEql.restype = ctypes.c_double;
rpa.nozzleStationGetPrEql.argtypes = [ctypes.c_void_p, ctypes.c_char_p];
rpa.nozzleStationGetPrEql.restype = ctypes.c_double;

# Initialize RPA
rpa.initializeWithPath("../resources", None, 1);

# Run analysis
conf = rpa.configFileLoad("RD-170.cfg");
performance = rpa.performanceCreate(conf, False, False);
rpa.performanceSolve(performance, False);

# Get results
of = rpa.performanceGetOF(performance);
Isp_SL = rpa.performanceGetDeliveredIspH(performance, "s", 0, "m", 0);
Isp_vac = rpa.performanceGetDeliveredIsp(performance, "s", 0, "atm", 0);

print "O/F=%2.1f" % of;
print "Isp(SL)=%6.2f s, Isp(vac)=%6.2f s" % (Isp_SL, Isp_vac);


station = rpa.nozzleStationCreate(performance, 0, None, 1, True, True, 1, "atm", False);
T = rpa.nozzleStationGetT(station, "K");
p = rpa.nozzleStationGetP(station, "Pa");
Mach = rpa.nozzleStationGetMach(station);
print "Nozzle Inlet:    p=%6.3f MPa T=%6.2f K, M=%6.2f" % (p/1e6, T, Mach);
rpa.nozzleStationDelete(station);

station = rpa.nozzleStationCreate(performance, 0, None, 2, True, True, 1, "atm", False);
T = rpa.nozzleStationGetT(station, "K");
p = rpa.nozzleStationGetP(station, "Pa");
Mach = rpa.nozzleStationGetMach(station);
print "Nozzle Throat:   p=%6.3f MPa T=%6.2f K, M=%6.2f" % (p/1e6, T, Mach);
rpa.nozzleStationDelete(station);

# Calculate parameters at different stations (A/At) of the nozzle 
area_ratio_list = [5, 10, 15, 20, 25, 30];
for AR in area_ratio_list : 
	station = rpa.nozzleStationCreate(performance, AR, None, 7, True, True, 1, "atm", False);
	T = rpa.nozzleStationGetT(station, "K");
	p = rpa.nozzleStationGetP(station, "Pa");
	Mach = rpa.nozzleStationGetMach(station);
	print "Nozzle A/At=%3.0f: p=%6.3f MPa T=%6.2f K, M=%6.2f" % (AR, p/1e6, T, Mach);
	rpa.nozzleStationDelete(station);

station = rpa.nozzleStationCreate(performance, 0, None, 3, True, True, 1, "atm", False);
T = rpa.nozzleStationGetT(station, "K");
p = rpa.nozzleStationGetP(station, "Pa");
Mach = rpa.nozzleStationGetMach(station);
print "Nozzle Exit:     p=%6.3f MPa T=%6.2f K, M=%6.2f" % (p/1e6, T, Mach);
rpa.nozzleStationDelete(station);

# Release created objects
rpa.performanceDelete(performance);
rpa.configFileDelete(conf);

# Finalize RPA
rpa.finalize();