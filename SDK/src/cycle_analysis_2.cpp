/*
 * RPA - Tool for Rocket Propulsion Analysis
 * RPA Software Development Kit (SDK)
 *
 * Copyright 2009,2015 Alexander Ponomarenko.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This software is a commercial product; you can use it under the terms of the
 * RPA SDK License as published at http://www.propulsion-analysis.com/sdk_eula.htm
 *
 * This program is distributed WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the RPA SDK License for more details (a copy is included
 * in the sdk_eula.htm file that accompanied this program).
 *
 * You should have received a copy of the RPA SDK License along with this program;
 * if not, write to author <contact@propulsion-analysis.com>
 *
 * Please contact author <contact@propulsion-analysis.com> or visit http://www.propulsion-analysis.com
 * if you need additional information or have any questions.
 */

#include "utils/Util.hpp"

#include "math/Common.hpp"
#include "thermodynamics/input/Input.hpp"
#include "thermodynamics/thermo/Thermodynamics.hpp"
#include "thermodynamics/dll/Export.hpp"

#include "flow/IncludeThermo.hpp"

#include "flow/Component.hpp"
#include "flow/FlowElements.hpp"
#include "flow/Port.hpp"
#include "flow/FlowNet.hpp"
#include "flow/Solvers.hpp"
#include "flow/FeedSystem.hpp"
#include "flow/PowerSystem.hpp"
#include "flow/EngineCycle.hpp"
#include "flow/Parameters.hpp"
#include "flow/FlowException.hpp"

#include "common.hpp"

//**********************************************************************************

void parseFlowPath(design::ComponentFlowPath* path, bool isBranch=false) {

	printf("Path: %s\n", path->getName());

	if (isBranch) {
		design::MassFlowPort* in_port = path->getInletPort();
		for (int c=0; c<in_port->connectedSize(); ++c) {
			if (design::Port::out==in_port->getConnected(c)->getDirection()) {
				printf("\t Connected to discharge port of %s\n", in_port->getConnected(c)->getOwner()->getName());
			}
		}
	}

	for (unsigned int i=0; i<path->size(); ++i) {
		design::MassFlowElement* el = path->getElement(i);

		if (dynamic_cast<design::Combustor*>(el)) {
			printf("\t%2d %10s: mdot=%+5.3f kg/s p_in=%7.3f MPa --> mdot=%+5.3f kg/s p_out=%7.3f MPa (dp=%7.3f MPa) T=%8.3f K\n",
					i, el->getName(),
					el->getMDot(), el->inletPort()->getP()/1e6,
					el->outletPort()->getMDot(), el->outletPort()->getP()/1e6,
					(el->outletPort()->getP() - el->inletPort()->getP())/1e6,
					el->outletPort()->getT()
			);

		} else
		if (dynamic_cast<design::Turbine*>(el)) {
			printf("\t%2d %10s: mdot=%+5.3f kg/s p_in=%7.3f MPa --> mdot=%+5.3f kg/s p_out=%7.3f MPa (pi=%7.3f) T=%8.3f K N=%8.3f kW\n",
					i, el->getName(),
					el->getMDot(), el->inletPort()->getP()/1e6,
					el->outletPort()->getMDot(), el->outletPort()->getP()/1e6,
					(el->inletPort()->getP()/el->outletPort()->getP()),
					el->outletPort()->getT(),
					el->getPower()/1e3
			);

		} else
		if (dynamic_cast<design::Pump*>(el)) {
			printf("\t%2d %10s: mdot=%+5.3f kg/s p_in=%7.3f MPa --> mdot=%+5.3f kg/s p_out=%7.3f MPa (dp=%7.3f MPa) N=%8.3f kW\n",
					i, el->getName(),
					el->getMDot(), el->inletPort()->getP()/1e6,
					el->outletPort()->getMDot(), el->outletPort()->getP()/1e6,
					(el->outletPort()->getP() - el->inletPort()->getP())/1e6,
					el->getPower()/1e3
			);

		} else {
			printf("\t%2d %10s: mdot=%+5.3f kg/s p_in=%7.3f MPa --> mdot=%+5.3f kg/s p_out=%7.3f MPa (dp=%7.3f MPa)\n",
					i, el->getName(),
					el->getMDot(), el->inletPort()->getP()/1e6,
					el->outletPort()->getMDot(), el->outletPort()->getP()/1e6,
					(el->outletPort()->getP() - el->inletPort()->getP())/1e6
			);
		}
	}
}


void parseCycle(design::EngineCycle* cycle) {

	unsigned int fs_size = cycle->getComponentFeedSystemSize();
	unsigned int ps_size = cycle->getPowerSystemSize();

	for (unsigned int fsi=0; fsi<fs_size; ++fsi) {
		design::ComponentFeedSystem* fs = cycle->getComponentFeedSystem(fsi);

		parseFlowPath(fs->getFlowPath());

		unsigned int b_size = fs->sizeBranches();
		for (unsigned int bi=0; bi<b_size; ++bi) {
			design::ComponentFlowPath* b = fs->getBranch(bi);

			parseFlowPath(b, true);
		}

	}

	for (unsigned int psi=0; psi<ps_size; ++psi) {
		design::PowerSystem* ps = cycle->getPowerSystem(psi);

		parseFlowPath(ps->getFlowPath());

		unsigned int b_size = ps->sizeBranches();
		for (unsigned int bi=0; bi<b_size; ++bi) {
			design::ComponentFlowPath* b = ps->getBranch(bi);

			parseFlowPath(b, true);
		}
	}

}

//**********************************************************************************

void test1() {

	printf("Gas generator cycle\n");

	thermo::Mixture* ox = new thermo::Mixture();
	ox->addSpecies("O2(L)", 90, 3e5, 1.0);
	ox->checkFractions();

	thermo::Mixture* fuel = new thermo::Mixture();
	fuel->addSpecies("H2(L)", 20, 3e5, 1.0);
	fuel->checkFractions();

	double p_inlet = 0.3e6;
	double p_c = 7e6;
	double p_gg = p_c*2;
	double T_gg_max = 1000;

	design::ChamberFeedSubsystemParameters paramsOx("Oxidizer", ox, p_c);
	paramsOx.m_dot = 13;
	paramsOx.p_inlet = 0.3e6;
	paramsOx.w_inlet = 3;
	paramsOx.pump_eta = 0.7;
	paramsOx.p_outlet = p_c;
	paramsOx.valve_dp = 0.15*p_c;
	paramsOx.valve_zeta = 0;
	paramsOx.injector_dp = 1.5*design::ComponentFeedSystem::getInjectorPressureDrop(p_c);
	paramsOx.injector_mu = 0.7;

	design::BoostPumpParameters paramsOxBoost(p_c);
	paramsOxBoost.p_inlet = paramsOx.p_inlet;
	paramsOxBoost.p_outlet = paramsOx.p_inlet + 0.2e6;
	paramsOxBoost.pump_eta = 0.65;
	paramsOxBoost.hydraulicTurbine = true;
	paramsOxBoost.turb_eta = 0.7;

	design::GasGeneratorBranchParameters paramsOxGG(p_c);
	paramsOxGG.m_dot = paramsOx.m_dot * 0.04; // paramsOx.m_dot * r;
	paramsOxGG.valve_dp = 1.5e6;
	paramsOxGG.injector_dp = 1.5*design::ComponentFeedSystem::getInjectorPressureDrop(p_c);
	paramsOxGG.injector_mu = 0.7;
	paramsOxGG.connectTo = "pump";


	// Parameters of fuel subsystem

	design::ChamberFeedSubsystemParameters paramsFuel("Fuel", fuel, p_c);
	paramsFuel.m_dot = 2;
	paramsFuel.p_inlet = 0.3e6;
	paramsFuel.w_inlet = 5;
	paramsFuel.pump_eta = 0.7;
	paramsFuel.p_outlet = p_c;		// Fuel -> chamber
	paramsFuel.valve_dp = 0.15*p_c;
	paramsFuel.valve_zeta = 0;
	paramsFuel.cooling_dp = 1.5*design::ComponentFeedSystem::getCoolingPressureDrop(p_c);
	paramsFuel.injector_dp = 1.5*design::ComponentFeedSystem::getInjectorPressureDrop(p_c);
	paramsFuel.injector_mu = 0.65;

	design::GasGeneratorBranchParameters paramsFuelGG(p_c);
	paramsFuelGG.m_dot = paramsFuel.m_dot * 0.05;
	paramsFuelGG.valve_dp = 0;
	paramsFuelGG.valve_zeta = 50;
	paramsFuelGG.injector_dp = 1.5*design::ComponentFeedSystem::getInjectorPressureDrop(p_c);
	paramsFuelGG.injector_mu = 0.7;
	paramsFuelGG.connectTo = "pump";

	design::GasGeneratorParameters paramsGG(0.8*p_c, 0.97, 1000, design::GasGeneratorParameters::fuel_rich);

	design::TurbineParameters paramsT1(2.5, 0.75);
	design::TurbineParameters paramsT2(5.0, 0.75);

	design::BranchParameters branch("test");
	branch.m_dot = paramsOx.m_dot*0.1;
	branch.pump_eta = 0.7;
	branch.p_outlet = p_c*2;
	branch.fixed_dp = 1e6;
	branch.connectTo = "pump";

	design::GasGeneratorCycle* cycle = 0;

	try {
		time_ms start = util::System::currentTimeMillis();

		cycle = new design::GasGeneratorCycle(p_c, &paramsOx, &paramsFuel);
//		cycle->addOxBoostPump(&paramsOxBoost);
		cycle->addGasGenerator(&paramsOxGG, &paramsFuelGG, &paramsGG);
//		cycle->addGasGenerator(&paramsOxGG, &paramsFuelGG, &paramsGG);
		cycle->addTurbine(&paramsT1);
		cycle->addTurbine(&paramsT2, design::TurbineParameters::parallel);

		cycle->solve();

		time_ms stop = util::System::currentTimeMillis();
		printf("Time: %f sec\n", (stop-start)/1000.0);

		parseCycle(cycle);

	} catch (const runtime::Exception& ex) {
		printf("Could not solve!\n");
	}

	delete cycle;

	delete ox;
	delete fuel;

}

void test2() {

	printf("Full-flow staged-combustion cycle\n");

	thermo::Mixture* ox = new thermo::Mixture();
	ox->addSpecies("O2(L)", 90, 3e5, 1.0);
	ox->checkFractions();

	thermo::Mixture* fuel = new thermo::Mixture();
	fuel->addSpecies("H2(L)", 20, 3e5, 1.0);
	fuel->checkFractions();

	double p_inlet = 0.3e6;
	double p_c = 15e6;
	double p_gg = p_c*2;
	double T_gg_max = 1000;

	// Parameters of oxidizer subsystem

	design::ChamberFeedSubsystemParameters paramsOx("Oxidizer", ox, p_c);
	paramsOx.m_dot = 13;
	paramsOx.p_inlet = 0.3e6;
	paramsOx.w_inlet = 3;
	paramsOx.pump_eta = 0.7;
	paramsOx.p_outlet = p_c;
	paramsOx.valve_dp = 0.15*p_c;
	paramsOx.valve_zeta = 0;
	paramsOx.injector_dp = design::ComponentFeedSystem::getInjectorPressureDrop(p_c);
	paramsOx.injector_mu = 0.7;

	design::BoostPumpParameters paramsOxBoost(p_c);
	paramsOxBoost.p_inlet = paramsOx.p_inlet;
	paramsOxBoost.p_outlet = paramsOx.p_inlet + 0.5e6;
	paramsOxBoost.pump_eta = 0.65;
	paramsOxBoost.hydraulicTurbine = true;
	paramsOxBoost.turb_eta = 0.7;

	design::GasGeneratorBranchParameters paramsOxGG(p_c);
	paramsOxGG.m_dot = paramsOx.m_dot * 0.1;
	paramsOxGG.valve_dp = 0;
	paramsOxGG.valve_zeta = 50;
	paramsOxGG.injector_dp = 1.5*design::ComponentFeedSystem::getInjectorPressureDrop(p_c);
	paramsOxGG.injector_mu = 0.7;

	// Parameters of fuel subsystem

	design::ChamberFeedSubsystemParameters paramsFuel("Fuel", fuel, p_c);
	paramsFuel.m_dot = 2;
	paramsFuel.p_inlet = 0.3e6;
	paramsFuel.w_inlet = 5;
	paramsFuel.pump_eta = 0.7;
	paramsFuel.p_outlet = p_c;
	paramsFuel.valve_dp = 0.15*p_c;
	paramsFuel.valve_zeta = 0;
	paramsFuel.cooling_dp = 1.5*design::ComponentFeedSystem::getCoolingPressureDrop(p_c);
	paramsFuel.injector_dp = 1.5*design::ComponentFeedSystem::getInjectorPressureDrop(p_c);
	paramsFuel.injector_mu = 0.65;

	design::BoostPumpParameters paramsFuelBoost(p_c);
	paramsFuelBoost.p_inlet = paramsFuel.p_inlet;
	paramsFuelBoost.p_outlet = paramsFuel.p_inlet + 0.5e6;
	paramsFuelBoost.pump_eta = 0.65;
	paramsFuelBoost.hydraulicTurbine = true;
	paramsFuelBoost.turb_eta = 0.7;

	design::GasGeneratorBranchParameters paramsFuelGG(p_c);
	paramsFuelGG.m_dot = paramsFuel.m_dot * 0.1;
	paramsFuelGG.valve_dp = 0;
	paramsFuelGG.valve_zeta = 50;
	paramsFuelGG.injector_dp = 1.5*design::ComponentFeedSystem::getInjectorPressureDrop(p_c);
	paramsFuelGG.injector_mu = 0.7;


	design::GasGeneratorParameters paramsGG1(p_c, 0.97, 850, design::GasGeneratorParameters::oxidizer_rich);
	design::GasGeneratorParameters paramsGG2(p_c, 0.97, 1000, design::GasGeneratorParameters::fuel_rich);

	design::TurbineParameters paramsT1(2.5, 0.75);
	design::TurbineParameters paramsT2(5.0, 0.75);


	design::StagedCombustionCycle* cycle = 0;

	try {
		time_ms start = util::System::currentTimeMillis();

		cycle = new design::StagedCombustionCycle(p_c, &paramsOx, &paramsFuel);
		cycle->addPreburners(&paramsFuelGG, &paramsGG1, &paramsOxGG, &paramsGG2);
		cycle->addTurbine(&paramsT1);
		cycle->addTurbine(&paramsT2);
		cycle->solve();

		time_ms stop = util::System::currentTimeMillis();
		printf("Time: %f sec\n", (stop-start)/1000.0);

		parseCycle(cycle);

	} catch (const runtime::Exception& ex) {
		printf("Could not solve!\n");
	}

	delete cycle;

	delete ox;
	delete fuel;

}

void test3() {

	printf("Fuel-rich staged-combustion cycle: \n");

	thermo::Mixture* ox = new thermo::Mixture();
	ox->addSpecies("O2(L)", 90, 3e5, 1.0);
	ox->checkFractions();

	thermo::Mixture* fuel = new thermo::Mixture();
	fuel->addSpecies("H2(L)", 20, 3e5, 1.0);
	fuel->checkFractions();

	double p_inlet = 0.3e6;
	double p_c = 15e6;
	double p_gg = p_c*2;
	double T_gg_max = 1000;


	// Parameters of oxidizer subsystem

	design::ChamberFeedSubsystemParameters paramsOx("Oxidizer", ox, p_c);
	paramsOx.m_dot = 13;
	paramsOx.p_inlet = 0.3e6;
	paramsOx.w_inlet = 3;
	paramsOx.pump_eta = 0.7;
	paramsOx.p_outlet = p_c;		// Oxidizer -> chamber
	paramsOx.valve_dp = 0.15*p_c;
	paramsOx.valve_zeta = 0;
	paramsOx.injector_dp = design::ComponentFeedSystem::getInjectorPressureDrop(p_c);
	paramsOx.injector_mu = 0.7;

	design::BoostPumpParameters paramsOxBoost(p_c);
	paramsOxBoost.p_inlet = paramsOx.p_inlet;
	paramsOxBoost.p_outlet = paramsOx.p_inlet + 0.5e6;
	paramsOxBoost.pump_eta = 0.65;
	paramsOxBoost.hydraulicTurbine = true;
	paramsOxBoost.turb_eta = 0.7;

	design::GasGeneratorBranchParameters paramsOxGG(p_c);
	paramsOxGG.m_dot = paramsOx.m_dot * 0.1;
	paramsOxGG.pump_eta = 0.7;
	paramsOxGG.valve_dp = 0;
	paramsOxGG.valve_zeta = 50;
	paramsOxGG.injector_dp = 1.5*design::ComponentFeedSystem::getInjectorPressureDrop(p_c);
	paramsOxGG.injector_mu = 0.7;

	// Parameters of fuel subsystem


	design::ChamberFeedSubsystemParameters paramsFuel("Fuel", fuel, p_c);
	paramsFuel.m_dot = 2;
	paramsFuel.p_inlet = 0.3e6;
	paramsFuel.w_inlet = 5;
	paramsFuel.pump_eta = 0.7;
	paramsFuel.p_outlet = p_gg;			// Fuel -> preburner
	paramsFuel.valve_dp = 0.15*p_c;
	paramsFuel.valve_zeta = 0;
	paramsFuel.cooling_dp = 1.5*design::ComponentFeedSystem::getCoolingPressureDrop(p_c);
	paramsFuel.injector_dp = 1.5*design::ComponentFeedSystem::getInjectorPressureDrop(p_c);
	paramsFuel.injector_mu = 0.65;

	design::GasGeneratorParameters paramsGG(p_c, 0.97, 1000, design::GasGeneratorParameters::fuel_rich);

	design::TurbineParameters paramsT1(2.5, 0.75);
	design::TurbineParameters paramsT2(5.0, 0.75);

	design::StagedCombustionCycle* cycle = 0;
	try {
		time_ms start = util::System::currentTimeMillis();

		cycle = new design::StagedCombustionCycle(p_c, &paramsOx, &paramsFuel);
		cycle->addOxBoostPump(&paramsOxBoost);
		cycle->addPreburner(&paramsOxGG, &paramsGG);
		cycle->addTurbine(&paramsT1);
		cycle->addTurbine(&paramsT2, design::TurbineParameters::parallel);
		cycle->solve();

		time_ms stop = util::System::currentTimeMillis();
		printf("Time: %f sec\n", (stop-start)/1000.0);

		parseCycle(cycle);

	} catch (const runtime::Exception& ex) {
		printf("Could not solve!\n");
	}

	delete cycle;

	delete ox;
	delete fuel;

}

void test4() {

	printf("Fuel-rich staged-combustion cycle with 2 gas generators\n");

	thermo::Mixture* ox = new thermo::Mixture();
	ox->addSpecies("O2(L)", 90, 3e5, 1.0);
	ox->checkFractions();

	thermo::Mixture* fuel = new thermo::Mixture();
	fuel->addSpecies("H2(L)", 20, 3e5, 1.0);
	fuel->checkFractions();

	double p_inlet = 0.3e6;
	double p_c = 15e6;
	double p_gg = p_c*2;
	double T_gg_max = 1000;


	double r = 0.25;

	design::ChamberFeedSubsystemParameters paramsOx("Oxidizer", ox, p_c);
	paramsOx.m_dot = 13;
	paramsOx.p_inlet = 0.3e6;
	paramsOx.w_inlet = 3;
	paramsOx.pump_eta = 0.7;
	paramsOx.p_outlet = p_c;
	paramsOx.valve_dp = 0.15*p_c;
	paramsOx.valve_zeta = 0;
	paramsOx.injector_dp = 1.5*design::ComponentFeedSystem::getInjectorPressureDrop(p_c);
	paramsOx.injector_mu = 0.7;

	design::GasGeneratorBranchParameters paramsOxGG1(p_c);
	paramsOxGG1.m_dot = paramsOx.m_dot * 0.1; // paramsOx.m_dot * r;
	paramsOxGG1.pump_eta = 0.7;
	paramsOxGG1.valve_zeta = 50;
	paramsOxGG1.injector_dp = 1.5*design::ComponentFeedSystem::getInjectorPressureDrop(p_c);
	paramsOxGG1.injector_mu = 0.7;

	design::GasGeneratorBranchParameters paramsOxGG2(p_c);
	paramsOxGG2.m_dot = paramsOx.m_dot * 0.1; // paramsOx.m_dot * (1 - r);
	paramsOxGG2.pump_eta = 0.7;
	paramsOxGG2.valve_zeta = 50;
	paramsOxGG2.injector_dp = 1.5*design::ComponentFeedSystem::getInjectorPressureDrop(p_c);
	paramsOxGG2.injector_mu = 0.7;

	// Parameters of fuel subsystem

	design::ChamberFeedSubsystemParameters paramsFuel("Fuel", fuel, p_c);
	paramsFuel.m_dot = 2;
	paramsFuel.p_inlet = 0.3e6;
	paramsFuel.w_inlet = 5;
	paramsFuel.pump_eta = 0.7;
	paramsFuel.p_outlet = p_c;		// Fuel -> chamber
	paramsFuel.valve_dp = 0.15*p_c;
	paramsFuel.valve_zeta = 0;
	paramsFuel.injector_dp = 1.5*design::ComponentFeedSystem::getInjectorPressureDrop(p_c);
	paramsFuel.injector_mu = 0.65;

	design::GasGeneratorBranchParameters paramsFuelGG1(p_c);
	paramsFuelGG1.m_dot = paramsFuel.m_dot * r;
	paramsFuelGG1.pump_eta = 0.7;
	paramsFuelGG1.valve_dp = 0;
	paramsFuelGG1.valve_zeta = 50;
	paramsFuelGG1.injector_dp = 1.5*design::ComponentFeedSystem::getInjectorPressureDrop(p_c);
	paramsFuelGG1.injector_mu = 0.7;

	design::BranchParameters branch("cooling");
	branch.m_dot = paramsFuel.m_dot*0.2;
	branch.p_outlet = p_c;
	branch.cooling_dp = 0.15*p_c;
	branch.fixed_dp = 1.5*design::ComponentFeedSystem::getInjectorPressureDrop(p_c);
	branch.connectTo = "pump";


	design::GasGeneratorParameters paramsGG1(p_c, 0.97, 1000, design::GasGeneratorParameters::fuel_rich);
	design::GasGeneratorParameters paramsGG2(p_c, 0.97, 1200, design::GasGeneratorParameters::fuel_rich);

	design::TurbineParameters paramsT1(3.0, 0.75);
	design::TurbineParameters paramsT2(3.0, 0.75);


	design::StagedCombustionCycle* cycle = 0;

	try {
		time_ms start = util::System::currentTimeMillis();

		cycle = new design::StagedCombustionCycle(p_c, &paramsOx, &paramsFuel);

		cycle->addPreburners(&paramsFuelGG1, &paramsOxGG1, &paramsGG1, &paramsOxGG2, &paramsGG2);

		cycle->addTurbine(&paramsT1);
		cycle->addTurbine(&paramsT2);

		cycle->addFuelBranch(&branch);

		cycle->solve();

		time_ms stop = util::System::currentTimeMillis();
		printf("Time: %f sec\n", (stop-start)/1000.0);

		parseCycle(cycle);

	} catch (const runtime::Exception& ex) {
		printf("Could not solve!\n");
	}

	delete cycle;

	delete ox;
	delete fuel;

}

void test5() {

	printf("Oxidizer-rich staged-combustion cycle\n");

	thermo::Mixture* ox = new thermo::Mixture();
	ox->addSpecies("O2(L)", 90, 3e5, 1.0);
	ox->checkFractions();

	thermo::Mixture* fuel = new thermo::Mixture();
	fuel->addSpecies("RP-1", 293, 3e5, 1.0);
	fuel->checkFractions();

	double p_inlet = 0.3e6;
	double p_c = 25.6e6;
	double p_gg = p_c*2;
	double T_gg_max = 776.3;


	// Parameters of oxidizer subsystem

	design::ChamberFeedSubsystemParameters paramsOx("Oxidizer", ox, p_c);
	paramsOx.m_dot = 915.962;
	paramsOx.p_inlet = 0.834e6;
	paramsOx.w_inlet = 3;
	paramsOx.pump_eta = 0.8;
	paramsOx.p_outlet = p_gg;		// Oxidizer -> preburner
	paramsOx.valve_dp = 0.15*p_c;
	paramsOx.valve_zeta = 0;
	paramsOx.injector_dp = 1e6; //design::ComponentFeedSystem::getInjectorPressureDrop(p_c);
	paramsOx.injector_mu = 0.7;

	design::BoostPumpParameters paramsOxBoost(p_c);
	paramsOxBoost.p_inlet = paramsOx.p_inlet;
	paramsOxBoost.p_outlet = 1.2e6;
	paramsOxBoost.pump_eta = 0.8;
	paramsOxBoost.hydraulicTurbine = false;
	paramsOxBoost.turb_eta = 0.7;
	paramsOxBoost.turb_pi = 20;

	// Parameters of fuel subsystem

	design::ChamberFeedSubsystemParameters paramsFuel("Fuel", fuel, p_c);
	paramsFuel.m_dot = 336.8;
	paramsFuel.p_inlet = 0.745e6;
	paramsFuel.w_inlet = 5;
	paramsFuel.pump_eta = 0.8;
	paramsFuel.p_outlet = p_c;		// Fuel -> chamber
	paramsFuel.valve_dp = 0.15*p_c;
	paramsFuel.valve_zeta = 0;
	paramsFuel.cooling_dp = 1.5*design::ComponentFeedSystem::getCoolingPressureDrop(p_c);
	paramsFuel.injector_dp = 1.5*design::ComponentFeedSystem::getInjectorPressureDrop(p_c);
	paramsFuel.injector_mu = 0.65;

	design::BoostPumpParameters paramsFuelBoost(p_c);
	paramsFuelBoost.p_inlet = paramsFuel.p_inlet;
	paramsFuelBoost.p_outlet = 1.2e6;
	paramsFuelBoost.pump_eta = 0.8;
	paramsFuelBoost.turb_pi = 1.5;
	paramsFuelBoost.hydraulicTurbine = true;
	paramsFuelBoost.turb_eta = 0.7;

	design::GasGeneratorBranchParameters paramsFuelGG(p_c);
	paramsFuelGG.m_dot = paramsFuel.m_dot * 0.1;
	paramsFuelGG.pump_eta = 0.8;
	paramsFuelGG.valve_dp = 0;
	paramsFuelGG.valve_zeta = 50;
	paramsFuelGG.injector_dp = 20e6; //1.5*design::ComponentFeedSystem::getInjectorPressureDrop(p_c);
	paramsFuelGG.injector_mu = 0.7;

	design::GasGeneratorParameters paramsGG(p_c, 0.89, 776.3, design::GasGeneratorParameters::oxidizer_rich);

	design::TurbineParameters paramsT1(1.8, 0.78);

	design::StagedCombustionCycle* cycle = 0;

	try {

		time_ms start = util::System::currentTimeMillis();

		cycle = new design::StagedCombustionCycle(p_c, &paramsOx, &paramsFuel);

		cycle->addOxBoostPump(&paramsOxBoost);
		cycle->addFuelBoostPump(&paramsFuelBoost);
		cycle->addPreburner(&paramsFuelGG, &paramsGG);
		cycle->addTurbine(&paramsT1);
		cycle->solve();

		time_ms stop = util::System::currentTimeMillis();
		printf("Time: %f sec\n", (stop-start)/1000.0);

		parseCycle(cycle);

	} catch (const runtime::Exception& ex) {
		printf("Could not solve!\n");
	}

	delete cycle;

	delete ox;
	delete fuel;

}

void test6(bool oxBoost, bool fuelBoost, bool separateTurbines) {

	printf("Oxidizer-rich staged-combustion cycle:\n"
		   "ox booster pump: %s\n"
		   "fuel booster pump: %s\n"
		   "%d turbine/s\n", oxBoost?"yes":"no", fuelBoost?"yes":"no", separateTurbines?2:1);

	thermo::Mixture* ox = new thermo::Mixture();
	ox->addSpecies("O2(L)", 90, 3e5, 1.0);
	ox->checkFractions();

	thermo::Mixture* fuel = new thermo::Mixture();
	fuel->addSpecies("RP-1", 293, 3e5, 1.0);
	fuel->checkFractions();

	double p_inlet = 0.3e6;
	double p_c = 18e6;
	double p_gg = p_c*2;
	double T_gg_max = 1000;

	// Parameters of oxidizer subsystem

	design::ChamberFeedSubsystemParameters paramsOx("Oxidizer", ox, p_c);
	paramsOx.m_dot = 1728;
	paramsOx.p_inlet = 0.75e6;
	paramsOx.w_inlet = 5.1;
	paramsOx.pump_eta = 0.7;
	paramsOx.p_outlet = p_c; //p_gg;		// Oxidizer -> preburner
	paramsOx.valve_dp = 3e6; //.15*p_c;
	paramsOx.valve_zeta = 0;
	paramsOx.injector_dp = 2e6; // design::ComponentFeedSystem::getInjectorPressureDrop(p_c);

	design::BoostPumpParameters paramsOxBoost(p_c);
	paramsOxBoost.p_inlet = paramsOx.p_inlet;
	paramsOxBoost.p_outlet = 1.2e6;
	paramsOxBoost.pump_eta = 0.7;
	paramsOxBoost.hydraulicTurbine = false;
	paramsOxBoost.turb_eta = 0.7;
	paramsOxBoost.turb_pi = 15;

	// Parameters of fuel subsystem

	design::ChamberFeedSubsystemParameters paramsFuel("Fuel", fuel, p_c);
	paramsFuel.m_dot = 657;
	paramsFuel.p_inlet = 0.75e6;
	paramsFuel.w_inlet = 5;
	paramsFuel.pump_eta = 0.7;
	paramsFuel.p_outlet = p_c;		// Fuel -> chamber
	paramsFuel.valve_dp = 3e6; //0.15*p_c;
	paramsFuel.valve_zeta = 0;
	paramsFuel.cooling_dp = 7e6; //1.5*design::ComponentFeedSystem::getCoolingPressureDrop(p_c);
	paramsFuel.injector_dp = 2e6; //1.5*design::ComponentFeedSystem::getInjectorPressureDrop(p_c);

	design::BoostPumpParameters paramsFuelBoost(p_c);
	paramsFuelBoost.p_inlet = paramsFuel.p_inlet;
	paramsFuelBoost.p_outlet = 1.2e6;
	paramsFuelBoost.pump_eta = 0.7;
	paramsFuelBoost.hydraulicTurbine = true;
	paramsFuelBoost.turb_eta = 0.7;
	paramsFuelBoost.turb_pi = 1.2;

	design::GasGeneratorBranchParameters paramsFuelGG(p_c);
	paramsFuelGG.m_dot = paramsFuel.m_dot * 0.1;
	paramsFuelGG.pump_eta = 0.7;
	paramsFuelGG.valve_dp = 0.5e6;
	paramsFuelGG.injector_dp = 2e6; //1.5*design::ComponentFeedSystem::getInjectorPressureDrop(p_c);

	design::GasGeneratorParameters paramsGG(p_c, 0.90, 850, design::GasGeneratorParameters::oxidizer_rich);

	design::TurbineParameters paramsT1(2.5, 0.75);
	design::TurbineParameters paramsT2(5.0, 0.75);

	design::StagedCombustionCycle* cycle = 0;
	try {
		time_ms start = util::System::currentTimeMillis();

		cycle = new design::StagedCombustionCycle(p_c, &paramsOx, &paramsFuel);
		if (oxBoost) {
			cycle->addOxBoostPump(&paramsOxBoost);
		}
		if (fuelBoost) {
			cycle->addFuelBoostPump(&paramsFuelBoost);
		}

		cycle->addPreburner(&paramsFuelGG, &paramsGG);
		cycle->addTurbine(&paramsT1);

		if (separateTurbines) {
			cycle->addTurbine(&paramsT2, design::TurbineParameters::serial);
		}

		cycle->solve();

		time_ms stop = util::System::currentTimeMillis();
		printf("Time: %f sec\n", (stop-start)/1000.0);

		cycle->printDesignParameters();

		parseCycle(cycle);

	} catch (const runtime::Exception& ex) {
		printf("Could not solve!\n");
	}

	delete cycle;

	delete ox;
	delete fuel;

}


//**********************************************************************************

/**
 * This example performs cycle analysis for number of engines.
 */
int main(int argc, char* argv[]) {

	util::Log::createLog("ROOT")->
		addLogger(new util::FileLogger("", 10*1024));

	// Initialize thermodatabase
	initThermoDatabase();

	//------------

	printf("\n********************************************\n");

	test1();

	printf("\n********************************************\n");

	test2();

	printf("\n********************************************\n");

	test3();

	printf("\n********************************************\n");

	test4();

	printf("\n********************************************\n");

	test5();

	printf("\n********************************************\n");

	test6(false, false, false);

	printf("\n********************************************\n");

	test6(true, true, false);

	printf("\n********************************************\n");

	//------------

	util::Log::finalize();

	return 0;
}


