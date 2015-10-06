
Utils = {

	printChamberParameters : function (chamber, contourPoints) {
		printf("F=%f t", chamber.getThrust("kg")/1000);
		printf("m_dot=%f", chamber.getMdot("kg/s"));
		printf("Lstar=%f mm", chamber.getLstar("mm"));
		printf("Lc=%f mm", chamber.getLc("mm"));
		printf("Lcyl=%f mm", chamber.getLcyl("mm"));
		printf("Dt=%f mm", chamber.getDt("mm"));
		printf("Dc=%f mm", chamber.getDc("mm"));
		printf("R1=%f mm", chamber.getR1("mm"));
		printf("R2=%f mm", chamber.getR2("mm"));
		printf("b=%f deg", chamber.getB("deg"));
		printf("Ae/At=%f", chamber.getFre());

		if (contourPoints>0) {
			var con = chamber.getContour(contourPoints);
			if (con.x.length<2) {
			  print("Increase number of points");
			} else {
			  print(con.x);
			}
		}	
	},
	
	printNozzleParameters : function (chamber, nozzle, contourPoints) {
		printf("Le/Dt=%f", nozzle.getL("mm")/chamber.getDt("mm"));
		printf("Le=%f mm", nozzle.getL("mm"));
		printf("t_max=%f deg", nozzle.getThetaMax("deg"));
		printf("t_e=%f deg", nozzle.getThetaE("deg"));

		if (contourPoints>0) {
			var con = nozzle.getContour(false, contourPoints);
			if (con.x.length<2) {
			  print("Increase number of points");
			} else {
			  print(con1.x.length);
			}
		}	
	},

	printContour : function (chamber, chamberContourPoints, nozzle, nozzleContourPoints) {
		if (chamberContourPoints>0) {
			var con1 = chamber.getContour(chamberContourPoints);
			if (con1.x.length<2) {
			  print("Increase number of points: chamber");
			} else {
			  print(con1.x);
			}
		}
		
		if (nozzleContourPoints>0) {
			var con2 = nozzle.getContour(false, nozzleContourPoints);
			if (con2.x.length<2) {
			  print("Increase number of points: nozzle");
			} else {
			  print(con2.x.length);
			}
		}
	},

	Solver : {
		PERFORMANCE: 1,
		CHAMBER_CONTOUR: 2,
		NOZZLE_CONTOUR: 4,
		CYCLE: 8,
		ENGINE_PERFORMANCE: 16,
		ENGINE_MASS: 32,
		ALL: 1 | 2 | 4 | 8 | 16 | 32
	},

	load : function (confPath) {
		var solver = this;

		solver.conf = ConfigFile(confPath);
		solver.conf.read();

		solver.runPerformance = function() {
			solver.perf = Performance(solver.conf);
			solver.perf.solve();
		}

		solver.designChamber = function() {
			solver.chamber = CombustionChamberContour(solver.perf, CorrectionFactors(solver.perf), solver.conf);
		}

		solver.designNozzle = function() {
			solver.nozzle = NozzleContour(solver.conf, solver.chamber)
		}

		solver.solveCycle = function() {
			solver.cycle = EngineCycle(solver.conf, solver.nozzle);
		}

		solver.estimateEnginePerformance = function() {
			solver.cyclePerf = CyclePerformance(solver.perf, solver.chamber, solver.cycle);
		}

		solver.estimateEngineMass = function() {
			solver.mass = MassEstimation(solver.conf, solver.cycle, solver.nozzle);
		}

		solver.run = function(what) {
			what = what?what:Utils.Solver.ALL;
			if (what & Utils.Solver.PERFORMANCE) {
				solver.runPerformance();
			}
			if ((what & Utils.Solver.CHAMBER_CONTOUR) && solver.perf) {
				solver.designChamber();
			}
			if ((what & Utils.Solver.NOZZLE_CONTOUR) && solver.chamber) {
				solver.designNozzle();
			}
			if ((what & Utils.Solver.CYCLE) && solver.nozzle) {
				solver.solveCycle();
			}
			if ((what & Utils.Solver.ENGINE_PERFORMANCE) && solver.perf && solver.chamber && solver.cycle) {
				solver.estimateEnginePerformance();
			}
			if ((what & Utils.Solver.ENGINE_MASS) && solver.cycle & solver.nozzle) {
				solver.estimateEngineMass();
			}		
		}

		solver.print = function(what) {
			what = what?what:Utils.Solver.ALL;
			if ((what & Utils.Solver.PERFORMANCE) && solver.perf) {
				solver.perf.printResults();
			}
			if ((what & Utils.Solver.CHAMBER_CONTOUR) && solver.chamber) {
				Utils.printChamberParameters(solver.chamber);
			}
			if ((what & Utils.Solver.NOZZLE_CONTOUR) && solver.chamber && solver.nozzle) {
				Utils.printNozzleParameters(solver.chamber, solver.nozzle, false);
			}
			if ((what & Utils.Solver.CYCLE) && solver.cycle) {
				solver.cycle.print();
			}
			if ((what & Utils.Solver.ENGINE_PERFORMANCE) && solver.cyclePerf) {
				solver.cyclePerf.print();
			}
			if ((what & Utils.Solver.ENGINE_MASS) && solver.mass) {
				solver.mass.print();
			}
		}

		solver.printContour = function(chamberContourPoints, nozzleContourPoints) {
			if (solver.chamber && solver.nozzle) {
				if (!chamberContourPoints || chamberContourPoints<100) {
					chamberContourPoints = 100;
				}
				if (!nozzleContourPoints || nozzleContourPoints<200) {
					nozzleContourPoints = 200;
				}
				Utils.printContour(solver.chamber, chamberContourPoints, solver.nozzle, nozzleContourPoints);
			}
		}

		return solver;
	}
	
};
