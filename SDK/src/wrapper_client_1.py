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