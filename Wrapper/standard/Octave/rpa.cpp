/*
 * RPA - Tool for Rocket Propulsion Analysis
 * RPA wrapper library
 *
 * Copyright 2009,2015 Alexander Ponomarenko.
 */


/*
 * This example demonstrates how to create extension for GNU Octave using RPA wrapper.
 *
 * Compilation on Linux
 * 
 * Use program mkoctfile provided by Octave:
 *
 * 	mkoctfile -L<path-to-wrapper> -lwrapper ./rpa.cpp
 * 
 * You might need to specify the path to Octave inclusion files and libraries, for example:
 * 
 * 	mkocfile -I/usr/include/octave-3.8.2/ -I/usr/include/octave-3.8.2/octave/ -L/usr/lib64/octave/3.8.2/ -L<path-to-wrapper> -lwrapper ./rpa.cpp
 * 
 * Running on Linux
 * 
 * Copy the compiled file rpa.oct into directory with wrapper libraries, and execute the command:
 * 
 * 	LD_LIBRARY_PATH=./:$LD_LIBRARY_PATH octave --path ./ ./test-rpa.m
 * 
 * where test-rpa.m is Octave file which loads and uses RPA Wrapper functions.
 * 
 * To get more information about programming Octave extensions, please look at:
 * http://octave.sourceforge.net/octave/function/mkoctfile.html
 * https://www.gnu.org/software/octave/doc/interpreter/External-Code-Interface.html#External-Code-Interface
 * http://octave.org/doxygen/3.8/
 * 
 */

#include <octave/oct.h>
#include <octave/parse.h>

#include "../include/Wrapper.h"
#include "../include/ConfigFile.h"
#include "../include/Performance.h"

/**
 * Initialize RPA 
 **/
DEFUN_DLD (rpaInit, args, nargout, "Initialize RPA") {
  int nargin = args.length();

  std::string respath = "./resources";
  std::string logpath = "./";
  bool consoleOutput = true;

  int nextIdx = 0;
  if (nargin>0 && args(0).is_string()) {
    respath = args(0).string_value();
     nextIdx++;
    
    if (nargin>1 && args(1).is_string()) {
      logpath = args(1).string_value();
      nextIdx++;
    }
  }
  if (nargin>nextIdx && args(nextIdx).is_bool_scalar()) {
    consoleOutput = args(nextIdx).bool_value();
  }
  
  initializeWithPath(respath.c_str(), logpath.c_str(), consoleOutput);
  
  return octave_value_list ();
}

/**
 * Finalize RPA 
 **/
DEFUN_DLD (rpaFinalize, args, nargout, "Finalize RPA") {
  finalize();
  return octave_value_list ();
}

/**
 * Open configuration file 
 **/
DEFUN_DLD (rpaConfigOpen, args, nargout, "Open configuration file") {
  octave_value retval;
  if (args.length()>0 && args(0).is_string()) {
    std::string cfgpath = args(0).string_value();
    void* c = configFileLoad(cfgpath.c_str());
    retval = (unsigned long int)c;
  }
  return retval;
}

/**
 * Close configuration file 
 **/
DEFUN_DLD (rpaConfigClose, args, nargout, "Close configuration file") {
  if (args.length()>0) {
    void* c  = (void*)args(0).ulong_value();
    if (c) {
      configFileDelete(c);
    }
  }
  return octave_value();
}

/**
 * Return configured combustion chamber pressure
 * 
 * This examples does not provide any further functions for manipulating the configuration file.
 * You may add new functions using this one as an example.
 **/
DEFUN_DLD (rpaConfigGetPc, args, nargout, "Close configuration file") {
  int nargin = args.length();
  
  octave_value_list retval;
  if (nargin>0) {
    void* c  = (void*)args(0).ulong_value();
    if (c) {
      std::string units = "MPa";
      
      if (nargin>1 && args(1).is_string()) {
	units = args(1).string_value();
      }

      retval(0) = configFileCombustionChamberConditionsGetPressure(c, units.c_str());
      retval(1) = units;
    }
  }
  return retval;
}

/**
 * Set combustion chamber pressure
 * 
 * This examples does not provide any further functions for manipulating the configuration file.
 * You may add new functions using this one as an example.
 **/
DEFUN_DLD (rpaConfigSetPc, args, nargout, "Close configuration file") {
  int nargin = args.length();
  
  if (nargin>0) {
    void* c  = (void*)args(0).ulong_value();
    if (c) {
      std::string units = "MPa";
      
      if (nargin>1 && args(1).is_numeric_type()) {
	double pc = args(1).double_value();
	if (nargin>2 && args(2).is_string()) {
	  units = args(2).string_value();
	}
	
	configFileCombustionChamberConditionsSetPressure(c, pc, units.c_str());
      }
    }
  }
  return octave_value();
}

/**
 * Create and run performance object 
 **/
DEFUN_DLD (rpaPerformance, args, nargout, "Create and run performance object") {
  octave_value retval;
  if (args.length()>0) {
    void* c  = (void*)args(0).ulong_value();
    if (c) {
      void* p = performanceCreate(c, 0, 0);
      performanceSolve(p, 0);
      retval = (unsigned long int)p;
    }
  }
  return retval;
}

/**
 * Delete performance object
 **/
DEFUN_DLD (rpaPerformanceDelete, args, nargout, "Delete performance object") {
  if (args.length()>0) {
    void* p  = (void*)args(0).ulong_value();
    if (p) {
      performanceDelete(p);
    }
  }
  return octave_value();
}

/**
 * Get delivered specific impulse 
 * Parameters:
 * p - pointer to performance object; required
 * units - specific impulse units; optional; default value is "m/s"
 * pa - ambient pressure; optional; default value is 0 (vacuum)
 * pa_units - pressure units; optionsl; default value is "atm"
 * throttleValue - throttle value (throttleValue=1.0 at nominal thrust); optional; default value is 1 
 * phi - correction factor; if phi==0, uses correction factors defined in configuration file, or estimated factor, if not defined in configuration file; optionsl; default value is 0
 **/
DEFUN_DLD (rpaPerformanceGetIsp, args, nargout, "Get delivered specific impulse") {
  int nargin = args.length();
  octave_value_list retval;
  
  void* p = 0;
  std::string units = "m/s";
  double pa = 0;
  std::string p_units = "atm";
  double throttleValue = 1;
  double phi = 0;

  if (nargin>0) {
    p = (void*)args(0).ulong_value();
    
    int nextIdx = 1;
    
    if (nargin>nextIdx && args(nextIdx).is_string()) {
      units = args(nextIdx).string_value();
      nextIdx++;
    }

    if (nargin>nextIdx && args(nextIdx).is_numeric_type()) {
      pa = args(nextIdx).double_value();
      nextIdx++;
      if (nargin>nextIdx && args(nextIdx).is_string()) {
	p_units = args(nextIdx).string_value();
	nextIdx++;
      }
    }

    if (nargin>nextIdx && args(nextIdx).is_numeric_type()) {
      throttleValue = args(nextIdx).double_value();
	nextIdx++;
    }

    if (nargin>nextIdx && args(nextIdx).is_numeric_type()) {
      phi = args(nextIdx).double_value();
	nextIdx++;
    }

    retval(0) = performanceGetDeliveredIspThrottled(p, units.c_str(), pa, p_units.c_str(), throttleValue, phi);
    retval(1) = units;
    retval(2) = pa;
    retval(3) = p_units;
    retval(4) = throttleValue;
    retval(5) = phi;
  }
  
  return retval;
}

/**
 * Get ideal specific impulse 
 * Parameters:
 * p - pointer to performance object; required
 * units - specific impulse units; optional; default value is "m/s"
 * pa - ambient pressure; optional; default value is 0 (vacuum)
 * pa_units - pressure units; optionsl; default value is "atm"
 * throttleValue - throttle value (throttleValue=1.0 at nominal thrust); optional; default value is 1 
 **/
DEFUN_DLD (rpaPerformanceGetIdealIsp, args, nargout, "Get delivered specific impulse") {
  int nargin = args.length();
  octave_value_list retval;
  
  void* p = 0;
  std::string units = "m/s";
  double pa = 0;
  std::string p_units = "atm";
  double throttleValue = 1;

  if (nargin>0) {
    p = (void*)args(0).ulong_value();
    
    int nextIdx = 1;
    
    if (nargin>nextIdx && args(nextIdx).is_string()) {
      units = args(nextIdx).string_value();
      nextIdx++;
    }

    if (nargin>nextIdx && args(nextIdx).is_numeric_type()) {
      pa = args(nextIdx).double_value();
      nextIdx++;
      if (nargin>nextIdx && args(nextIdx).is_string()) {
	p_units = args(nextIdx).string_value();
	nextIdx++;
      }
    }

    if (nargin>nextIdx && args(nextIdx).is_numeric_type()) {
      throttleValue = args(nextIdx).double_value();
	nextIdx++;
    }
    
    retval(0) = performanceGetIdealIspThrottled(p, units.c_str(), pa, p_units.c_str(), throttleValue);
    retval(1) = units;
    retval(2) = pa;
    retval(3) = p_units;
    retval(4) = throttleValue;
  }
  
  return retval;
}

