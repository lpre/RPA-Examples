load("utils.js");

solver = Utils.load("examples/cycle_analysis/RD-275.cfg");
solver.run(Utils.Solver.ALL);
solver.print(Utils.Solver.ALL);

