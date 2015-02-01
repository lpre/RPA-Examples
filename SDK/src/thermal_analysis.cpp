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

#include <map>

#include "utils/Util.hpp"
#include "thermodynamics/input/Input.hpp"
#include "thermodynamics/thermo/Thermodynamics.hpp"
#include "thermodynamics/dll/Export.hpp"
#include "thermodynamics/gasdynamics/StandardAtmosphere.hpp"

#include "nozzle/ConicalNozzle.hpp"
#include "nozzle/ParabolicNozzle.hpp"
#include "nozzle/MocNozzle.hpp"
#include "nozzle/digitized/NozzleContour.hpp"

#include "heat_transfer/Nozzle.hpp"


#include "common.hpp"

struct ChamberMassFlowRate {
	double mdot;
	double mdot_ox;
	double mdot_f;
};

struct RPAData {
	thermo::input::ConfigFile* data;
	performance::TheoreticalPerformance* performance;
	performance::ThrottlingPerformance* throttlingPerformance;
	performance::efficiency::CorrectionFactors* correctionFactors;
	design::Chamber* chamber;
	design::Nozzle* nozzle;
	ChamberMassFlowRate* chamberMassFlowRate;
	design::thermal::Nozzle* t_nozzle;

	RPAData(const char* configFile) :
		data(0),
		performance(0), throttlingPerformance(0), correctionFactors(0),
		chamber(0), nozzle(0), chamberMassFlowRate(0),
		t_nozzle(0) {

		// Initialize configuration file object
		data = new thermo::input::ConfigFile(configFile);

		// Read configuration file
		data->read();
	}

	~RPAData() {
		delete t_nozzle;
		delete chamberMassFlowRate;
		delete nozzle;
		delete chamber;
		delete correctionFactors;
		delete throttlingPerformance;
		delete performance;
		delete data;
	}

	void chamberPerformance(bool optimizePropellant=false) {

		delete performance;

		// Initialize performance solver
		performance = new performance::TheoreticalPerformance(data, false);


		if (optimizePropellant && thermo::input::Ratio::fractions!=data->getPropellant().getRatioType()) {
			// Find optimal mixture ratio for given propellant
			performance->optimizeForSpecificImpulse();
		} else {
			performance->solve();
		}

	}

	void chamberGeometry(bool applyCorrectionFactor=false) {

		delete chamber;
		delete nozzle;

		chamber = 0;
		nozzle = 0;

		if (data->getEngineSize().isThrustSet() || data->getEngineSize().isMdotSet() || data->getEngineSize().isThroatDSet()) {

			// Design parameters of combustion chamber
			double b = data->getEngineSize().getChamberGeometry().getContractionAngle() * M_PI / 180.;
			double R1 = data->getEngineSize().getChamberGeometry().getR1ToRtRatio();
			double R2 = data->getEngineSize().getChamberGeometry().getR2ToR2maxRatio();
			double Rn = data->getEngineSize().getChamberGeometry().getRnToRtRatio();
			double Lstar = data->getEngineSize().getChamberGeometry().getChamberLength();
			bool isLstar = data->getEngineSize().getChamberGeometry().isCharacteristicLength();


			bool parabolicNozzle = !data->getEngineSize().getChamberGeometry().isTOC();

			if (!correctionFactors && applyCorrectionFactor) {
				// Get performance correction factor
				correctionFactors = new performance::efficiency::CorrectionFactors(performance);
			}

			chamber = new design::Chamber(performance, correctionFactors);

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

			delete chamberMassFlowRate;
			chamberMassFlowRate = new ChamberMassFlowRate();

			chamberMassFlowRate->mdot = chamber->getMdot();
			chamberMassFlowRate->mdot_ox = chamber->getMdotOx();
			chamberMassFlowRate->mdot_f = chamber->getMdotF();

			double F_opt = Is_opt*chamberMassFlowRate->mdot;
			double F_v = Is_v*chamberMassFlowRate->mdot;

			if (data->getNozzleFlowOptions().isEfficiencyFactorsSet() && data->getNozzleFlowOptions().getEfficiencyFactors().isConeHalfAngleSet()) {
				double te = !data->getNozzleFlowOptions().getEfficiencyFactors().isConeHalfAngleSet()?7.5:data->getNozzleFlowOptions().getEfficiencyFactors().getConeHalfAngle();

				nozzle = new design::ConicalNozzle(chamber);
				static_cast<design::ConicalNozzle*>(nozzle)->calcGeometry(te, Rn);

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
					double k = throatDerivatives->getK();
					double K = sqrt((k+1.)/(k-1.));
					tn = 60.*( K*atan(sqrt(Me*Me-1.)/K) - atan(Me*Me-1.) ) / M_PI;
				}

				// Nozzle exit angle (degrees)
				double te = 0.;
				if (data->getEngineSize().getChamberGeometry().isParabolicExitAngle()) {
					te = data->getEngineSize().getChamberGeometry().getParabolicExitAngle();
				} else {
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

			}

		}

	}

	void chamberThermalAnalysis(bool applyCooling=false, bool applyFilmCooling=false, bool simplifiedChamberContour=false) {

		if (data->isChamberCooling() && nozzle) {

			double Twg = 1000;
			double wallLayer = 0.05;

			bool blc = data->getChamberCooling().getHeatTransferParameters().isApplyBLC();
			bool withRadiativeHeatTransfer = data->getChamberCooling().getHeatTransferParameters().isRadiationHeatTransfer();

			int points = data->getChamberCooling().getHeatTransferParameters().getNumberOfStations();
			if (points<=0) {
				points = 50;
			}

			t_nozzle = new design::thermal::Nozzle(nozzle, blc, points, simplifiedChamberContour);

			switch(data->getChamberCooling().getHeatTransferParameters().getRelationsType()) {
				case thermo::input::HeatTransferParameters::Ievlev:
					util::Log::printf("THERMO", "Using Ievlev approach.%s", CR);
					t_nozzle->setIevlevApproach();
					break;
				case thermo::input::HeatTransferParameters::Bartz:
					util::Log::printf("THERMO", "Using Bartz approach.%s", CR);
					t_nozzle->setBartzApproach();
					break;
				case thermo::input::HeatTransferParameters::combined:
					util::Log::printf("THERMO", "Using Mixed approach.%s", CR);
					t_nozzle->setMixedApproach();
					break;
			}

			t_nozzle->setEmissivity(data->getChamberCooling().getHeatTransferParameters().getWallSurfaceEmissivity());


			if (applyCooling) {

				if (data->getChamberCooling().getFilmSlotsListSize()>0) {

					if (!withRadiativeHeatTransfer) {
						util::Log::errorf("THERMO", "Film cooling can only be applied with activated radiation heat transfer calculation.%s", CR);
						throw thermo::Exception(thermo::Exception::INVALID_STATE, "Film cooling can only be applied with activated radiation heat transfer calculation.");
					}

					if (0==data->getChamberCooling().getSectionListSize()) {
						util::Log::errorf("THERMO", "Film cooling can only be applied as an complementary cooling. Please define either regenerative or radiation cooling.%s", CR);
						throw thermo::Exception(thermo::Exception::INVALID_STATE, "Film cooling can only be applied as an complementary cooling. Please define either regenerative or radiation cooling as well.");
					}
				}

				std::map<design::thermal::RegenerativeCooling*, thermo::input::ConvectiveCooling*> sections;

				for (int i=0, size=data->getChamberCooling().getSectionListSize(); i<size; ++i) {
					thermo::input::CoolingSection& s = data->getChamberCooling().getSection(i);

					double location = s.getLocation(true);
					bool oppositeFlow = true;
					double mdot = 0;

					thermo::Mixture* mix = NULL;
					if (dynamic_cast<thermo::input::ConvectiveCooling*>(&s)) {
						// Prepare cooling mixture definition
						thermo::input::ConvectiveCooling& s2 = dynamic_cast<thermo::input::ConvectiveCooling&>(s);

						oppositeFlow = s2.getOppositeFlow();

						if (!s2.isCoolantFromSet()) {
							mdot = s2.getMdot();

							mix = new thermo::Mixture();
							for (int j=0, size2=s2.getCoolantListSize(); j<size2; ++j) {

								try {
									mix->addSpecies(s2.getCoolant(j).getName().c_str(), s2.getCoolant(j).getT(true), s2.getCoolant(j).getP(true), s2.getCoolant(j).getMf());
								} catch (const thermo::Exception& ex) {
									util::Log::errorf("THERMO", "Regenerative cooling: unknown species %s or invalid parameters.%s", s2.getCoolant(j).getName().c_str(), CR);
									throw thermo::Exception(thermo::Exception::INVALID_STATE, "Regenerative cooling: unknown species.");
								}

								if (0==s2.getCoolant(j).getT(true) || 0==s2.getCoolant(j).getP(true)) {
									util::Log::errorf("THERMO", "Regenerative cooling: invalid coolant definition.%s", CR);
									throw thermo::Exception(thermo::Exception::INVALID_STATE, "Regenerative cooling: invalid coolant definition.");
								}

								if (!mix->getSpecies(mix->size()-1)->hasMuTable()) {
									util::Log::errorf("THERMO", "Regenerative cooling: invalid coolant definition.%s", CR);
									throw thermo::Exception(thermo::Exception::INVALID_STATE, "Regenerative cooling: invalid coolant definition.");
								}
							}
						}
					}

					if (dynamic_cast<thermo::input::TubularJacketDesign*>(&s)) {
						thermo::input::TubularJacketDesign& s2 = dynamic_cast<thermo::input::TubularJacketDesign&>(s);

						design::thermal::TubularWallDesign* rt = NULL;
						if (mix) {
							rt = new design::thermal::TubularWallDesign(mix, mdot, oppositeFlow);
							delete mix;
							mix = NULL;
						} else {
							rt = new design::thermal::TubularWallDesign(oppositeFlow);
						}

						rt->setH(s2.getH(true));
						rt->setD(s2.getD(true), s2.getN());
						rt->setHelix(s2.isHelix());

						if (s.isLambdaSet()) {
							rt->setLambda(s.getLambda(true));
						} else {
							util::Log::warnf("THERMO", "Unknown wall thermal conductivity. Using default value 270 W/(m K).%s", CR);
							rt->setLambda(270.0);
						}

						if (s.isInsulationLambdaSet() && s.isInsulationHSet()) {
							rt->setInsulationLambda(s.getInsulationLambda(true));
							rt->setInsulationH(s.getInsulationH(true));
						} else  {
							rt->setInsulationLambda(0.0);
							rt->setInsulationH(0.0);
						}

						if (s.isIdSet()) {
							rt->setId(s.getId());
						}

						t_nozzle->addCooling(rt, 0, location);

						if (s2.isCoolantFromSet()) {
							sections[rt] = &s2;
						}

					} else
					if (dynamic_cast<thermo::input::ChannelJacketDesign*>(&s)) {
						thermo::input::ChannelJacketDesign& s2 = dynamic_cast<thermo::input::ChannelJacketDesign&>(s);

						design::thermal::ChannelWallDesign* rc = NULL;
						if (mix) {
							rc = new design::thermal::ChannelWallDesign(mix, mdot, oppositeFlow);
							delete mix;
							mix = NULL;
						} else {
							rc = new design::thermal::ChannelWallDesign(oppositeFlow);
						}

						rc->setH(s2.getH(true));
						rc->setHc(s2.getHc1(true), s2.getHc2(true));
						rc->setA(s2.getA1(true), s2.getA2(true));
						if (s2.isNSet()) {
							rc->setN(s2.getN());
						} else {
							rc->setB(s2.getB1(true), s2.getB2(true));
						}
						rc->setGamma(thermo::input::Angle::convert(90.0-s2.getGamma(true), thermo::input::Angle::degrees, thermo::input::Angle::radians));

						if (s.isLambdaSet()) {
							rc->setLambda(s.getLambda(true));
						} else {
							util::Log::warnf("THERMO", "Unknown wall thermal conductivity. Using default value 270 W/(m K).%s", CR);
							rc->setLambda(270.0);
						}

						if (s.isInsulationLambdaSet() && s.isInsulationHSet()) {
							rc->setInsulationLambda(s.getInsulationLambda(true));
							rc->setInsulationH(s.getInsulationH(true));
						} else  {
							rc->setInsulationLambda(0.0);
							rc->setInsulationH(0.0);
						}

						if (s.isIdSet()) {
							rc->setId(s.getId());
						}

						t_nozzle->addCooling(rc, 0, location);

						if (s2.isCoolantFromSet()) {
							sections[rc] = &s2;
						}

					} else
					if (dynamic_cast<thermo::input::SlotJacketDesign*>(&s)) {
						thermo::input::SlotJacketDesign& s2 = dynamic_cast<thermo::input::SlotJacketDesign&>(s);

						design::thermal::CoaxialShellDesign* c = NULL;
						if (mix) {
							c = new design::thermal::CoaxialShellDesign(mix, mdot, oppositeFlow);
							delete mix;
							mix = NULL;
						} else {
							c = new design::thermal::CoaxialShellDesign(oppositeFlow);
						}

						c->setHc(s2.getHc(true));
						c->setH(s2.getH(true));

						if (s.isLambdaSet()) {
							c->setLambda(s.getLambda(true));
						} else {
							util::Log::warnf("THERMO", "Unknown wall thermal conductivity. Using default value 270 W/(m K).%s", CR);
							c->setLambda(270.0);
						}

						if (s.isInsulationLambdaSet() && s.isInsulationHSet()) {
							c->setInsulationLambda(s.getInsulationLambda(true));
							c->setInsulationH(s.getInsulationH(true));
						} else  {
							c->setInsulationLambda(0.0);
							c->setInsulationH(0.0);
						}

						if (s.isIdSet()) {
							c->setId(s.getId());
						}

						t_nozzle->addCooling(c, 0, location);

						if (s2.isCoolantFromSet()) {
							sections[c] = &s2;
						}

					} else
					if (dynamic_cast<thermo::input::RadiationCooling*>(&s)) {
						thermo::input::RadiationCooling& s2 = dynamic_cast<thermo::input::RadiationCooling&>(s);

						design::thermal::RadiativeCooling* rc = new design::thermal::RadiativeCooling(s2.getSurfaceEmissivity());

						rc->setH(s2.getH(true));
						if (s.isLambdaSet()) {
							rc->setLambda(s.getLambda(true));
						} else {
							util::Log::warnf("THERMO", "Unknown wall thermal conductivity. Using default value 270 W/(m K).%s", CR);
							rc->setLambda(270.0);
						}

						if (s.isInsulationLambdaSet() && s.isInsulationHSet()) {
							rc->setInsulationLambda(s.getInsulationLambda(true));
							rc->setInsulationH(s.getInsulationH(true));
						} else  {
							rc->setInsulationLambda(0.0);
							rc->setInsulationH(0.0);
						}

						if (s.isIdSet()) {
							rc->setId(s.getId());
						}

						t_nozzle->addCooling(rc, 0, location);

					}

					if (mix) {
						delete mix;
						mix = NULL;
						util::Log::errorf("THERMO", "Unknown type of cooling section. Please verify input parameters.%s", CR);
						throw thermo::Exception(thermo::Exception::INVALID_STATE, "Unknown type of cooling section. Please verify input parameters.");
					}

				}

				for (int i=0, size=t_nozzle->getNumberOfWallCoolings(); i<size; ++i) {
					// Ignore all radiation cooling sections
					design::thermal::RegenerativeCooling* c = dynamic_cast<design::thermal::RegenerativeCooling*>(t_nozzle->getWallCooling(i));
					if (c) {
						std::map<design::thermal::RegenerativeCooling*, thermo::input::ConvectiveCooling*>::iterator s = sections.find(c);
						if (sections.end()!=s) {
							std::string coolantFrom = s->second->getCoolantFrom();
							if (!coolantFrom.empty()) {
								design::thermal::RegenerativeCooling* from = dynamic_cast<design::thermal::RegenerativeCooling*>(t_nozzle->getWallCooling(coolantFrom));
								if (from) {
									c->connectInputTo(from);
								} else {
									util::Log::errorf("THERMO", "Could not find regenerative cooling section with ID=%s. Please verify input parameters.%s", coolantFrom.c_str(), CR);
									throw thermo::Exception(thermo::Exception::INVALID_STATE, "Could not find regenerative cooling section with specified ID. Please verify input parameters.");
								}
							}
						}
					}
				}
				sections.clear();

				t_nozzle->fillGapsWithRadiativeCooling(0.002, 80, 0.85);

				int size2 = data->getChamberCooling().getSectionListSize();
				for (int i=0, size=data->getChamberCooling().getFilmSlotsListSize(); i<size; ++i) {

					double location = data->getChamberCooling().getFilmSlot(i).getLocation(true);
					double mdot = data->getChamberCooling().getFilmSlot(i).getMdot();

					// Do not add film belt with mdot~0
					if (mdot>1e-6) {

						thermo::Mixture* mix = new thermo::Mixture();
						for (int j=0, size2=data->getChamberCooling().getFilmSlot(i).getCoolantListSize(); j<size2; ++j) {
							try {
								mix->addSpecies(
									data->getChamberCooling().getFilmSlot(i).getCoolant(j).getName().c_str(),
									data->getChamberCooling().getFilmSlot(i).getCoolant(j).getT(true),
									data->getChamberCooling().getFilmSlot(i).getCoolant(j).getP(true),
									data->getChamberCooling().getFilmSlot(i).getCoolant(j).getMf()
								);
							} catch (const thermo::Exception& ex) {
								util::Log::errorf("THERMO", "Film cooling: unknown species %s or invalid parameters.%s", data->getChamberCooling().getFilmSlot(i).getCoolant(j).getName().c_str(), CR);
								throw thermo::Exception(thermo::Exception::INVALID_STATE, "Film cooling: unknown species.");
							}

							if (0==data->getChamberCooling().getFilmSlot(i).getCoolant(j).getT(true)) {
								util::Log::errorf("THERMO", "Film cooling: invalid coolant definition.%s", CR);
								throw thermo::Exception(thermo::Exception::INVALID_STATE, "Film cooling: invalid coolant definition.");
							}

							if (!mix->getSpecies(mix->size()-1)->hasMuTable()) {
								util::Log::errorf("THERMO", "Film cooling: invalid coolant definition.%s", CR);
								throw thermo::Exception(thermo::Exception::INVALID_STATE, "Film cooling: invalid coolant definition.");
							}
						}

						t_nozzle->addFilmCoolingSlot(mix, mdot, location);
						delete mix;

					}
				}

				t_nozzle->solve(Twg, wallLayer, true, applyFilmCooling, withRadiativeHeatTransfer);

			} else {
				// applyCooling is false

				t_nozzle->solve(Twg, wallLayer, false, applyFilmCooling, withRadiativeHeatTransfer);
			}

			util::Log::printf("THERMO", "%s***************************************************%sThermal Analysis%s---------------------------------------------------%s", CR, CR, CR, CR);
			util::Log::printf("THERMO", "%10s %10s %15s %15s %15s %10s%s",
					"x, mm",
					"r, mm",
					"q_rad, kW/m^2",
					"q_conv, kW/m^2",
					"q_total, kW/m^2",
					"Twg, K",
					CR);

			for (int i=0; i<t_nozzle->getNumberOfSections(); ++i){
				design::thermal::NozzleSection* section = t_nozzle->getSection(i);

				util::Log::printf("THERMO", "%10.3f %10.3f %15.3f %15.3f %15.3f %10.3f%s",
						section->getX()*1000., 	// convert to mm
						section->getR()*1000.,	// convert to mm
						section->getQRadiative()/1000.,	// convert to kW/m^2
						section->getQ(t_nozzle->getApproach(), design::thermal::NozzleSection::CONVECTIVE)/1000., 	// convert to kW/m^2
						section->getQ(t_nozzle->getApproach(), design::thermal::NozzleSection::TOTAL)/1000.,		// convert to kW/m^2
						section->getTwg(),	// K
						CR);


			}

			std::vector<design::thermal::RegenerativeCooling*> icc;
			t_nozzle->getIndependentConvectiveCooling(icc);
			for (std::vector<design::thermal::RegenerativeCooling*>::iterator ic=icc.begin(), end=icc.end(); ic<end; ++ic) {
				design::thermal::RegenerativeCooling* c = (*ic);
				util::Log::printf("THERMO", "Location=%10.3f Tolerance=%e%s", c->getLocation(), c->getTolerance(), CR);
			}


		}

	}

};


int main(int argc, char* argv[]) {

	util::Log::createLog("ROOT")->
//		addLogger(new util::ConsoleLogger())->
		addLogger(new util::FileLogger("", 10*1024));

	// Initialize thermodatabase
	initThermoDatabase();


	RPAData rpaData("examples/thermal/Aestus.cfg");

	rpaData.chamberPerformance();
	rpaData.chamberGeometry(true);

	rpaData.chamberThermalAnalysis(true, true);


	util::Log::finalize();

	return 0;
}


