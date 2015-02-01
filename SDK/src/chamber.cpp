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
#include "thermodynamics/input/Input.hpp"
#include "thermodynamics/thermo/Thermodynamics.hpp"
#include "thermodynamics/dll/Export.hpp"
#include "thermodynamics/gasdynamics/StandardAtmosphere.hpp"

#include "nozzle/ConicalNozzle.hpp"
#include "nozzle/ParabolicNozzle.hpp"
#include "nozzle/MocNozzle.hpp"
#include "nozzle/digitized/NozzleContour.hpp"

#include "common.hpp"

/**
 * This example calculated geometry of combustion chamber and nozzle.
 */
int main(int argc, char* argv[]) {

	util::Log::createLog("ROOT")->
		addLogger(new util::FileLogger("", 10*1024));

	// Initialize thermodatabase
	initThermoDatabase();

	bool applyCorrectionFactor = true;
	bool optimizePropellant = false;

	// Initialize configuration file object
	thermo::input::ConfigFile* data = new thermo::input::ConfigFile("examples/RD-170.cfg");

	// Read configuration file
	data->read();

	// Modify input data, if necessary
	// For instance:
	//
	// Set nozzle area ratio
	// data->getNozzleFlowOptions().setNozzleExitConditions().setAreaRatio(30, true);
	//
	// Set propellant O/F ratio (km==O/F):
	// data->getPropellant().setRatio(2.5, thermo::input::Ratio::km);
	//
	// Set chamber pressure:
	// data->getCombustionChamberConditions().setPressure(15, thermo::input::Pressure::MPa);
	//
	// etc.


	// Initialize performance solver
	performance::TheoreticalPerformance* performance = new performance::TheoreticalPerformance(data, false);

	if (optimizePropellant && thermo::input::Ratio::fractions!=data->getPropellant().getRatioType()) {
		// Find optimal mixture ratio for given propellant
		performance->optimizeForSpecificImpulse();
	} else {
		performance->solve();
	}

	if (data->getEngineSize().isThrustSet() || data->getEngineSize().isMdotSet() || data->getEngineSize().isThroatDSet()) {

		// Design parameters of combustion chamber
		double b = data->getEngineSize().getChamberGeometry().getContractionAngle() * M_PI / 180.;
		double R1 = data->getEngineSize().getChamberGeometry().getR1ToRtRatio();
		double R2 = data->getEngineSize().getChamberGeometry().getR2ToR2maxRatio();
		double Rn = data->getEngineSize().getChamberGeometry().getRnToRtRatio();
		double Lstar = data->getEngineSize().getChamberGeometry().getChamberLength();
		bool isLstar = data->getEngineSize().getChamberGeometry().isCharacteristicLength();

		// Alternatively define parameters directly
//		double b = 30 * M_PI / 180.;
//		double R1 = 0.5;
//		double R2 = 0.5;
//		double Rn = 0.2;
//		double Lstar = 1.2;
//		bool isLstar = true;


		bool parabolicNozzle = !data->getEngineSize().getChamberGeometry().isTOC();

		performance::efficiency::CorrectionFactors* correctionFactors = NULL;

		if (applyCorrectionFactor) {
			// Get performance correction factor
			correctionFactors = new performance::efficiency::CorrectionFactors(performance);
		}


		design::Chamber* chamber = new design::Chamber(performance, correctionFactors);
		design::Nozzle* nozzle = NULL;

		if (data->getEngineSize().isThrustSet()) {
			chamber->setThrust(data->getEngineSize().getThrust(true) / (double)data->getEngineSize().getChambersNo(), data->getEngineSize().getAmbientPressure(true));
		} else
		if (data->getEngineSize().isMdotSet()) {
			chamber->setMdot(data->getEngineSize().getMdot(true) / (double)data->getEngineSize().getChambersNo());
		} else
		if (data->getEngineSize().isThroatDSet()) {
			chamber->setDt(data->getEngineSize().getThroatD(true));
		}

		chamber->setB(b);
		chamber->setR1toRt(R1);
		chamber->setR2toR2max(R2);
		if (isLstar) {
			chamber->setLstar(Lstar);
		} else {
			// Lstar is actually L'
			chamber->setLc(Lstar);
		}
		chamber->calcGeometry();

		// Calculate thrust and mass flow rate
		performance::equilibrium::NozzleSectionConditions* exitSection = dynamic_cast<performance::equilibrium::NozzleSectionConditions*>(performance->getExitSection());

		double Is_v = exitSection->getIs_v();
		if (correctionFactors) {
			Is_v *= correctionFactors->getOverallEfficiency();
		}
		double Is_opt = Is_v - exitSection->getF()*exitSection->getP();

		double mdot = chamber->getMdot();
		double mdot_ox = chamber->getMdotOx();
		double mdot_f = chamber->getMdotF();

		double F_opt = Is_opt*mdot;
		double F_v = Is_v*mdot;



		if (correctionFactors) {
			printf("%25s: %e\n", "Reaction efficiency", correctionFactors->getReactionEfficiency());
			printf("%25s: %e\n", "Overall efficiency", correctionFactors->getOverallEfficiency());
		}

		printf("%25s: %10.5f %6s\n", "Chamber thrust (vac)", F_v/1000., "kN");
		printf("%25s: %10.5f %6s\n", "Specific impulse (vac)", Is_v/CONST_G, "s");

		printf("%25s: %10.5f %6s\n", "Chamber thrust (opt)", F_opt/1000., "kN");
		printf("%25s: %10.5f %6s\n", "Specific impulse (opt)", Is_opt/CONST_G, "s");

		printf("%25s: %10.5f %6s\n", "Total mass flow rate", mdot, "kg/s");
		printf("%25s: %10.5f %6s\n", "Oxidizer mass flow rate", mdot_ox, "kg/s");
		printf("%25s: %10.5f %6s\n", "Fuel mass flow rate", mdot_f, "kg/s");

		printf("Dc = %7.2f mm  b = %7.2f deg\n", chamber->getDc()*1000., chamber->getB()*180./M_PI);
		printf("R2 = %7.2f mm  R1 = %7.2f mm\n", chamber->getR2()*1000, chamber->getR1()*1000.);
		printf("L* = %7.2f mm\n", chamber->getLstar()*1000.);
		printf("Lc = %7.2f mm  Lcyl = %7.2f mm\n", chamber->getLc()*1000., chamber->getLcyl()*1000);
		printf("Dt = %7.2f mm\n", chamber->getDt()*1000.);


		if (data->getNozzleFlowOptions().isEfficiencyFactorsSet() && data->getNozzleFlowOptions().getEfficiencyFactors().isConeHalfAngleSet()) {
			double te = !data->getNozzleFlowOptions().getEfficiencyFactors().isConeHalfAngleSet()?7.5:data->getNozzleFlowOptions().getEfficiencyFactors().getConeHalfAngle();

			nozzle = new design::ConicalNozzle(chamber);
			static_cast<design::ConicalNozzle*>(nozzle)->calcGeometry(te, Rn);

			printf("Rn = %7.2f mm, Te = %7.2f deg\n", static_cast<design::ConicalNozzle*>(nozzle)->getRn()*1000., te);
			printf("Le = %7.2f mm", static_cast<design::ConicalNozzle*>(nozzle)->getL()*1000.);

		} else if (data->getEngineSize().getChamberGeometry().isTOC()) {
			// MOC TIC

			nozzle = new design::MocNozzle(chamber);

			double Tw_bar = data->getEngineSize().getChamberGeometry().isTw_to_T0()?data->getEngineSize().getChamberGeometry().getTwToT0():0.3;

			// TODO: configuration file does not provide this parameter
			bool fixedAreaAnaLength = true;

			if (data->getEngineSize().getChamberGeometry().isTOC_L()) {
				double Le_bar = data->getEngineSize().getChamberGeometry().getLe();

				if (fixedAreaAnaLength) {
					double Fr = chamber->getFre();
					static_cast<design::MocNozzle*>(nozzle)->calcGeometryAtFixedAreaAndLength(Fr, Le_bar, R1, Rn);
				} else {
					static_cast<design::MocNozzle*>(nozzle)->calcGeometryAtFixedLength(Le_bar, R1, Rn, Tw_bar);
				}

			} else {
				double Fr = chamber->getFre();
				static_cast<design::MocNozzle*>(nozzle)->calcGeometryAtFixedArea(Fr, R1, Rn, Tw_bar);
			}

			double Tn_max = 0;
			double dt = 0.005;
			for (double t=0; t<=1.0; t+=dt) {
				if ((t+dt)>1.0) {
					t = 1.0;
				}

				design::moc::Point* p = static_cast<design::MocNozzle*>(nozzle)->getPoint(t);
				double Tn_t = p->theta();
				delete p;

				if (Tn_t<Tn_max) {
					break;
				}
				Tn_max = Tn_t;
			}

			printf("Rn = %7.2f mm, Tn (max) = %7.2f deg\n", Rn*chamber->getDt()*1000., Tn_max*180./M_PI);
			printf("Le = %7.2f mm, Te = %7.2f deg\n", static_cast<design::MocNozzle*>(nozzle)->getLe()*1000., static_cast<design::MocNozzle*>(nozzle)->getTe());

		} else {
			// Parabolic nozzle

			reaction::Derivatives *throatDerivatives = performance->getChamber()->getDerivatives(performance::equilibrium::Chamber::THROAT);

			performance::equilibrium::NozzleSectionConditions *exitSection = dynamic_cast<performance::equilibrium::NozzleSectionConditions*>(performance->getExitSection());
			double Me = exitSection->getMach();
			double rho_e = exitSection->getRho();
			double w_e = exitSection->getW();
			double p_e = exitSection->getP();
			double k_e = exitSection->getK();

			// Ambient pressure
			double p_a = 0;

			double mu_e = asin(1./Me);

			// Nozzle initial angle (degrees)
			double tn = 0.;
			if (data->getEngineSize().getChamberGeometry().isParabolicInitialAngle()) {
				tn = data->getEngineSize().getChamberGeometry().getParabolicInitialAngle();
			} else {
				// Estimate
				// Vasiljev, p.352 (10.63)
				double k = throatDerivatives->getK();
				double K = sqrt((k+1.)/(k-1.));
				tn = 60.*( K*atan(sqrt(Me*Me-1.)/K) - atan(Me*Me-1.) ) / M_PI;
			}

			// Nozzle exit angle (degrees)
			double te = 0.;
			if (data->getEngineSize().getChamberGeometry().isParabolicExitAngle()) {
				te = data->getEngineSize().getChamberGeometry().getParabolicExitAngle();
			} else {
				// Estimate
				// Dobrovolsky p.50 (2.23); Dorofeev, p.343 (28.1)
				te = 90.*asin(2.*(p_e-p_a)/(rho_e*w_e*w_e*tan(mu_e)))/M_PI;
				if (te<8.) {
					te = 8.;
				}
			}
			if (te>tn) {
				te = 0.9*tn;
			}

			nozzle = new design::ParabolicNozzle(chamber);
			static_cast<design::ParabolicNozzle*>(nozzle)->calcGeometry2(tn, te, R1, Rn);

			printf("Rn = %7.2f mm, Tn = %7.2f deg\n", static_cast<design::ParabolicNozzle*>(nozzle)->getRn()*1000., tn);
			printf("Le = %7.2f mm, Te = %7.2f deg\n", static_cast<design::ParabolicNozzle*>(nozzle)->getL()*1000., te);

		}

		printf("De = %7.2f mm\n", chamber->getDe()*1000.);

		delete nozzle;
		delete chamber;
		delete correctionFactors;

	} else {
		printf("Engine size design parameters not defined!\n");
	}

	delete performance;
	delete data;

	util::Log::finalize();

	return 0;
}


