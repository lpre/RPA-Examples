import ctypes

# Load CDLL
rpa = ctypes.CDLL("wrapper.dll");

# Declare used functions
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
rpa.initialize(1);

# Run analysis
conf = rpa.configFileLoad("examples\\RD-170.cfg");
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