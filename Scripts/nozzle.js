c = ConfigFile("examples/RD-275.cfg");
c.read();

p = Performance(c);
p.solve();

//p.printResults();

chamber = CombustionChamberContour(p, CorrectionFactors(p));

chamber.setThrust(1747, "kN", 0, "atm");
chamber.setB(30, "degrees");
chamber.setR1toRt(1.5);
chamber.setR2toR2max(0.5);
chamber.setLstar(1, "m");
chamber.calcGeometry();

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
printf("Fr=%f", chamber.getFre());

nozzle = MocNozzleContour(chamber);

nozzle.calcGeometryAtFixedArea(chamber.getFre(), 1.5, 0.382, 0.3);

printf("Le=%f mm", nozzle.getL("mm"));
printf("t_max=%f deg", nozzle.getThetaMax("deg"));
printf("t_e=%f deg", nozzle.getThetaE("deg"));

con1 = chamber.getContour(100);
if (con1.x.length<2) {
  print("Increase number of points");
} else {
  print(con1.x);
}

con2 = nozzle.getContour(false, 200);
if (con2.x.length<2) {
  print("Increase number of points");
} else {
  print(con2.x.length);
}


