import ctypes

# Load CDLL
rpa = ctypes.CDLL("wrapper.dll");

# Declare used functions
rpa.configFileLoad.argtypes = [ctypes.c_void_p];
rpa.configFileNozzlePropellantSetRatio.argtypes = [ctypes.c_void_p, ctypes.c_double, ctypes.c_char_p];
rpa.performanceGetOF.argtypes = [ctypes.c_void_p];
rpa.performanceGetOF.restype = ctypes.c_double;
rpa.performanceGetDeliveredIspH.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_double, ctypes.c_char_p, ctypes.c_double]
rpa.performanceGetDeliveredIspH.restype = ctypes.c_double;
rpa.performanceGetDeliveredIsp.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_double, ctypes.c_char_p, ctypes.c_double]
rpa.performanceGetDeliveredIsp.restype = ctypes.c_double;

# Initialize RPA
rpa.initialize(1);

# Run analysis
conf = rpa.configFileLoad("examples\\RD-170.cfg");

# Calculate performance at given O/F ratios
of_ratios = [1.2, 1.5, 1.8, 2.0, 2.2, 2.4, 2.6, 2.8, 3.0];
for of in of_ratios : 
	rpa.configFileNozzlePropellantSetRatio(conf, of, "O/F");
	performance = rpa.performanceCreate(conf, 0, 0);
	rpa.performanceSolve(performance, 0);
	# Get results
	Isp_SL = rpa.performanceGetDeliveredIspH(performance, "s", 0, "m", 0);
	Isp_vac = rpa.performanceGetDeliveredIsp(performance, "s", 0, "atm", 0);
	print "O/F=%2.1f: Isp(SL)=%3.2f s, Isp(vac)=%3.2f s" % (of, Isp_SL, Isp_vac);
	# Release created objects
	rpa.performanceDelete(performance)


# Release created objects
rpa.configFileDelete(conf)

# Finalize RPA
rpa.finalize();