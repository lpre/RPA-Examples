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

#include "design/MassEstimation.hpp"
#include "design/CycleAnalysis/Chamber.hpp"
#include "design/CycleAnalysis/GasGenerator.hpp"
#include "design/CycleAnalysis/Turbopump.hpp"
#include "design/CycleAnalysis/Cycles.hpp"


#include "common.hpp"

struct ChamberMassFlowRate {
	double mdot;
	double mdot_ox;
	double mdot_f;
};

struct FeedSubsystem {
	design::ChamberFeedSubsystemParameters* main;
	design::BoostPumpParameters* boostPump;
	std::vector<design::BranchParameters*> branches;
	design::GasGeneratorBranchParameters* gg1;
	design::GasGeneratorBranchParameters* gg2;

	FeedSubsystem()
	: main(0), boostPump(0), gg1(0), gg2(0) {
	}

	~FeedSubsystem() {
		if (main && main->c) {
			delete main->c;
			main->c = 0;
		}
		delete main;
		delete boostPump;
		delete gg1;
		delete gg2;
		for (unsigned int i=0; i<branches.size(); ++i) {
			delete branches[i];
		}
		branches.clear();
	}
};

struct PowerSubsystem {
	design::GasGeneratorParameters* gg1;
	design::GasGeneratorParameters* gg2;
	design::TurbineParameters* turbine1;
	design::TurbineParameters* turbine2;
	design::TurbineParameters::TYPE type;

	PowerSubsystem()
	: gg1(0), gg2(0), turbine1(0), turbine2(0), type(design::TurbineParameters::serial) {
	}

	~PowerSubsystem() {
		delete gg1;
		delete gg2;
		delete turbine1;
		delete turbine2;
	}
};


struct CyclePerformance {
	// Chamber performance
	double Is_c_v;
	double Is_c_opt;
	double Is_c_SL;

	double T_c_v;
	double T_c_opt;
	double T_c_SL;

	// Engine performance
	double Is_e_v;
	double Is_e_opt;
	double Is_e_SL;

	double T_e_v;
	double T_e_opt;
	double T_e_SL;

	double phi_e;
	CyclePerformance() {
		Is_c_v = Is_c_opt = Is_c_SL = 0;
		T_c_v = T_c_opt = T_c_SL = 0;
		Is_e_v = Is_e_opt = Is_e_SL = 0;
		T_e_v = T_e_opt = T_e_SL = 0;
		phi_e = 1;
	}
};


struct RPAData {
	thermo::input::ConfigFile* data;
	performance::TheoreticalPerformance* performance;
	performance::ThrottlingPerformance* throttlingPerformance;
	performance::efficiency::CorrectionFactors* correctionFactors;
	design::Chamber* chamber;
	design::Nozzle* nozzle;
	ChamberMassFlowRate* chamberMassFlowRate;
	design::EngineCycle* cycle;
	design::MassEstimation* mass;
	CyclePerformance* cyclePerformance;


	RPAData(const char* configFile) :
		data(0),
		performance(0), throttlingPerformance(0), correctionFactors(0),
		chamber(0), nozzle(0), chamberMassFlowRate(0),
		cycle(0), mass(0), cyclePerformance(0) {

		// Initialize configuration file object
		data = new thermo::input::ConfigFile(configFile);

		// Read configuration file
		data->read();
	}

	~RPAData() {
		delete cycle;
		delete mass;
		delete cyclePerformance;
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

	void engineCycleAnalysis() {

		delete cycle;
		delete mass;
		delete cyclePerformance;

		cycle = 0;
		mass = 0;
		cyclePerformance = 0;


		if (data->getPropellantFeedSystem().isPressurizedFeedSystemSet() || data->getPropellantFeedSystem().isTurbopumpFeedSystemSet()) {

			// Total mass flow rate through all thrust chambers
			double mdot_ox = 0;
			double mdot_f = 0;

			double p_c = data->getCombustionChamberConditions().getPressure();

			// Configuration parameters - feed subsystems
			FeedSubsystem paramsOx, paramsFuel;

			// Configuration parameters - power subsystems
			PowerSubsystem paramsPower;

			try {

				thermo::input::Propellant& prop = data->getPropellant();
				switch (prop.getRatioType()) {
					case thermo::input::Ratio::km:
					case thermo::input::Ratio::alpha:
					case thermo::input::Ratio::optimal:
					{
						// Bipropellant

						mdot_ox = nozzle->getChamber()->getMdotOx() * data->getEngineSize().getChambersNo();
						mdot_f = nozzle->getChamber()->getMdotF() * data->getEngineSize().getChambersNo();

						thermo::Mixture* oxMixture = new thermo::Mixture();
						for (size_t i=0, size=prop.getOxidizerListSize(); i<size; ++i) {
							thermo::input::Component& ox = prop.getOxidizer(i);
							if (ox.getMf()>0) {

								double T = ox.getT();
								if (0==T) {
									const thermo::Species* s = thermo::Database::getInstance()->find(ox.getName());
									T = s->getT0();
								}

								double p = ox.getP();
								if (0==p) {
									p = CONST_P0;
								}

								oxMixture->addSpecies(ox.getName(), T, p, ox.getMf());
							} else {
								util::Log::warnf("THERMO", "Skipping %s with mass fraction r=0%s", ox.getName().c_str(), CR);
							}
						}
						oxMixture->checkFractions();

						thermo::Mixture* fMixture = new thermo::Mixture();
						for (size_t i=0, size=prop.getFuelListSize(); i<size; ++i) {
							thermo::input::Component& f = prop.getFuel(i);
							if (f.getMf()>0) {

								double T = f.getT();
								if (0==T) {
									const thermo::Species* s = thermo::Database::getInstance()->find(f.getName());
									T = s->getT0();
								}

								double p = f.getP();
								if (0==p) {
									p = CONST_P0;
								}

								fMixture->addSpecies(f.getName(), T, p, f.getMf());
							} else {
								util::Log::warnf("THERMO", "Skipping %s with mass fraction r=0%s", f.getName().c_str(), CR);
							}
						}
						fMixture->checkFractions();

						paramsOx.main = new design::ChamberFeedSubsystemParameters("Oxidizer Feed Subsystem", oxMixture, p_c);
						paramsFuel.main = new design::ChamberFeedSubsystemParameters("Fuel Feed Subsystem", fMixture, p_c);

						break;
					}
					case thermo::input::Ratio::fractions:
					{
						// Monopropellant

						mdot_f = nozzle->getChamber()->getMdotF() * data->getEngineSize().getChambersNo();

						thermo::Mixture* mixture = new thermo::Mixture();
						for (size_t i=0, size=prop.getSpeciesListSize(); i<size; ++i) {
							thermo::input::Component& c = prop.getSpecies(i);
							if (c.getMf()>0) {

								double T = c.getT();
								if (0==T) {
									const thermo::Species* s = thermo::Database::getInstance()->find(c.getName());
									T = s->getT0();
								}

								double p = c.getP();
								if (0==p) {
									p = CONST_P0;
								}

								mixture->addSpecies(c.getName(), T, p, c.getMf());
							} else {
								util::Log::warnf("THERMO", "Skipping %s with mass fraction r=0%s", c.getName().c_str(), CR);
							}
						}
						mixture->checkFractions();

						paramsFuel.main = new design::ChamberFeedSubsystemParameters("Propellant Feed Subsystem", mixture, p_c);

						break;
					}
				}


				thermo::input::PropellantFeedSystem& propellantFeedSystem = data->getPropellantFeedSystem();

				if (propellantFeedSystem.isPressurizedFeedSystemSet()) {
					// Pressurized feed system
					// TODO not yet implemented

					throw thermo::Exception(thermo::Exception::INVALID_STATE,
							"Improperly configured fuel feed subsystem.\n\n"
							"Pressurized feed system is not supported in this version.");

				} else if (propellantFeedSystem.isTurbopumpFeedSystemSet()) {
					// Turbopump feed system

					thermo::input::TurbopumpFeedSystem& tpSystem = propellantFeedSystem.getTurbopumpFeedSystem();

					if (paramsOx.main && tpSystem.isOxidizerFeedSystem()) {

						configureComponentSubsystem(p_c, mdot_ox,
							&paramsOx,
							&tpSystem.getOxidizerFeedSystem(),
							tpSystem.isOxidizerGGBranch1()?&tpSystem.getOxidizerGGBranch1():0,
							tpSystem.isOxidizerGGBranch2()?&tpSystem.getOxidizerGGBranch2():0,
							tpSystem.isOxidizerBoostPump()?&tpSystem.getOxidizerBoostPump():0,
							&tpSystem.getOxidizerBranches()
						);

					} else if (
						(!paramsOx.main && tpSystem.isOxidizerFeedSystem())
						||
						(paramsOx.main && !tpSystem.isOxidizerFeedSystem())
					) {
						throw thermo::Exception(thermo::Exception::INVALID_STATE,
								"Improperly configured oxidizer main feed subsystem");
					}

					if (paramsFuel.main && tpSystem.isFuelFeedSystem()) {

						configureComponentSubsystem(p_c, mdot_f,
							&paramsFuel,
							&tpSystem.getFuelFeedSystem(),
							tpSystem.isFuelGGBranch1()?&tpSystem.getFuelGGBranch1():0,
							tpSystem.isFuelGGBranch2()?&tpSystem.getFuelGGBranch2():0,
							tpSystem.isFuelBoostPump()?&tpSystem.getFuelBoostPump():0,
							&tpSystem.getFuelBranches()
						);

					} else if (
						(!paramsFuel.main && tpSystem.isFuelFeedSystem())
						||
						(paramsFuel.main && !tpSystem.isFuelFeedSystem())
					) {
						throw thermo::Exception(thermo::Exception::INVALID_STATE,
								"Improperly configured fuel main feed subsystem.");
					}


					if (tpSystem.isGG1()) {
						thermo::input::GasGeneratorParameters& gg = tpSystem.getGG1();

						if (!gg.isTmaxSet()) {
							throw thermo::Exception(thermo::Exception::INVALID_STATE,
									"Improperly configured fuel feed subsystem.\n\n"
									"Gas generator/preburner (#1) temperature Tmax has to be specified.");
						}

						design::GasGeneratorParameters::TYPE type =
							thermo::input::GasGeneratorParameters::oxidizer_rich==gg.getType()?
							design::GasGeneratorParameters::oxidizer_rich : design::GasGeneratorParameters::fuel_rich;

						paramsPower.gg1 = new design::GasGeneratorParameters(
							gg.isPressureSet()?gg.getPressure():p_c,
							gg.isSigmaSet()?gg.getSigma():1.0,
							gg.getTmax(),
							type
						);


						// GG2 requires GG1 to be configured
						if (tpSystem.isGG2()) {
							thermo::input::GasGeneratorParameters& gg = tpSystem.getGG2();

							if (!gg.isTmaxSet()) {
								throw thermo::Exception(thermo::Exception::INVALID_STATE,
										"Improperly configured fuel feed subsystem.\n\n"
										"Gas generator/preburner (#2) temperature Tmax has to be specified.");
							}

							design::GasGeneratorParameters::TYPE type =
								thermo::input::GasGeneratorParameters::oxidizer_rich==gg.getType()?
								design::GasGeneratorParameters::oxidizer_rich : design::GasGeneratorParameters::fuel_rich;

							paramsPower.gg2 = new design::GasGeneratorParameters(
								gg.isPressureSet()?gg.getPressure():p_c,
								gg.isSigmaSet()?gg.getSigma():1.0,
								gg.getTmax(),
								type
							);
						}

					}


					if (tpSystem.isTurbine1()) {
						thermo::input::TurbineParameters& t = tpSystem.getTurbine1();

						paramsPower.turbine1 = new design::TurbineParameters(
							t.isPiSet()?t.getPi():0,
							t.isEtaSet()?t.getEta():0.7
						);


						// Turbine2 requires Turbine1 to be configured
						if (tpSystem.isTurbine2()) {
							thermo::input::TurbineParameters& t = tpSystem.getTurbine2();

							paramsPower.turbine2 = new design::TurbineParameters(
								t.isPiSet()?t.getPi():0,
								t.isEtaSet()?t.getEta():0.7
							);

							switch(t.getType()) {
							case thermo::input::TurbineParameters::parallel:
								paramsPower.type = design::TurbineParameters::parallel;
								break;
							case thermo::input::TurbineParameters::serial_ox_f:
								paramsPower.type = design::TurbineParameters::serial_ox_f;
								break;
							case thermo::input::TurbineParameters::serial_f_ox:
								paramsPower.type = design::TurbineParameters::serial_f_ox;
								break;
							case thermo::input::TurbineParameters::serial:
							default:
								paramsPower.type = design::TurbineParameters::serial;
								break;
							}

	//						paramsPower.type = thermo::input::TurbineParameters::serial==t.getType()?
	//							design::TurbineParameters::serial : design::TurbineParameters::parallel;
						}


					}


					switch (data->getPropellantFeedSystem().getTurbopumpFeedSystemCycle()) {
						case thermo::input::TurbopumpFeedSystem::gas_generator: {
							//*** Gas-generator cycle

							if (!paramsOx.main || !paramsFuel.main || !paramsOx.gg1 || !paramsFuel.gg1 || !paramsPower.gg1) {
								throw thermo::Exception(thermo::Exception::INVALID_STATE,
										"Improperly configured engine cycle.\n\n"
										"The following subsystems have to be configured:\n"
										"- oxidizer main feed branch\n"
										"- oxidizer GG feed branch\n"
										"- fuel main feed branch\n"
										"- fuel GG feed branch");
							}

							paramsOx.main->p_outlet = p_c;
							paramsFuel.main->p_outlet = p_c;

							if (0==paramsPower.gg1->p) {
								paramsPower.gg1->p = 0.8*p_c;
							}
							if (paramsPower.gg2 && 0==paramsPower.gg2->p) {
								paramsPower.gg2->p = paramsPower.gg1->p;
							}

							design::GasGeneratorCycle* _cycle = new design::GasGeneratorCycle(p_c, paramsOx.main, paramsFuel.main);

							_cycle->addGasGenerator(paramsOx.gg1, paramsFuel.gg1, paramsPower.gg1);

							cycle = _cycle;

							break;
						}
						case thermo::input::TurbopumpFeedSystem::staged_combustion: {
							//*** Staged-combustion cycle

							design::StagedCombustionCycle* _cycle = 0;

							if (paramsOx.main && paramsFuel.main) {
								paramsOx.main->p_outlet = p_c;
								paramsFuel.main->p_outlet = p_c;
								_cycle = new design::StagedCombustionCycle(p_c, paramsOx.main, paramsFuel.main);
							} else if (paramsFuel.main) {
								paramsFuel.main->p_outlet = p_c;
								_cycle = new design::StagedCombustionCycle(p_c, paramsFuel.main);
							} else {
								throw thermo::Exception(thermo::Exception::INVALID_STATE,
										"Improperly configured engine cycle.\n\n"
										"The following subsystems have to be configured:\n"
										"- oxidizer main feed branch\n"
										"- fuel main feed branch\n"
										"- preburner feed branch/es");
							}

							if (paramsPower.gg1) {
								if (paramsPower.gg2) {
									// Staged-combustion cycle with two preburners
									design::GasGeneratorBranchParameters* ggBranch0 = 0;
									design::GasGeneratorBranchParameters* ggBranch1 = 0;
									design::GasGeneratorBranchParameters* ggBranch2 = 0;

									if (design::GasGeneratorParameters::oxidizer_rich==paramsPower.gg1->type) {
										ggBranch0 = paramsOx.gg1;
										ggBranch1 = paramsFuel.gg1;
										ggBranch2 = paramsFuel.gg2;
									} else {
										ggBranch0 = paramsFuel.gg1;
										ggBranch1 = paramsOx.gg1;
										ggBranch2 = paramsOx.gg2;
									}

									if (!ggBranch0 || !ggBranch1 || !ggBranch2) {
										if (design::GasGeneratorParameters::oxidizer_rich==paramsPower.gg1->type) {
											throw thermo::Exception(thermo::Exception::INVALID_STATE,
													"Improperly configured engine cycle.\n\n"
													"The following subsystems have to be configured:\n"
													"- oxidizer main feed branch\n"
													"- fuel main feed branch\n"
													"- oxidizer preburner feed branch #1 with assigned relative mass flow rate\n"
													"- fuel preburner feed branch #1\n"
													"- fuel preburner feed branch #2");
										} else {
											throw thermo::Exception(thermo::Exception::INVALID_STATE,
													"Improperly configured engine cycle.\n\n"
													"The following subsystems have to be configured:\n"
													"- oxidizer main feed branch\n"
													"- fuel main feed branch\n"
													"- fuel preburner feed branch #1 with assigned relative mass flow rate\n"
													"- oxidizer preburner feed branch #1\n"
													"- oxidizer preburner feed branch #2");
										}
									}

									if (0==paramsPower.gg1->p) {
										paramsPower.gg1->p = p_c;
									}
									if (0==paramsPower.gg2->p) {
										paramsPower.gg2->p = p_c;
									}

									_cycle->addPreburners(ggBranch0, ggBranch1, paramsPower.gg1, ggBranch2, paramsPower.gg2);

								} else {
									// Staged-combustion cycle with one preburner
									design::GasGeneratorBranchParameters* ggBranch = 0;
									if (design::GasGeneratorParameters::oxidizer_rich==paramsPower.gg1->type) {
										ggBranch = paramsFuel.gg1;
									} else {
										ggBranch = paramsOx.gg1;
									}

									if (!ggBranch) {
										if (design::GasGeneratorParameters::oxidizer_rich==paramsPower.gg1->type) {
											throw thermo::Exception(thermo::Exception::INVALID_STATE,
													"Improperly configured engine cycle.\n\n"
													"The following subsystems have to be configured:\n"
													"- oxidizer main feed branch\n"
													"- fuel main feed branch\n"
													"- fuel preburner feed branch #1");
										} else {
											throw thermo::Exception(thermo::Exception::INVALID_STATE,
													"Improperly configured engine cycle.\n\n"
													"The following subsystems have to be configured:\n"
													"- oxidizer main feed branch\n"
													"- fuel main feed branch\n"
													"- oxidizer preburner feed branch #1");
										}
									}

									if (0==paramsPower.gg1->p) {
										paramsPower.gg1->p = p_c;
									}

									_cycle->addPreburner(ggBranch, paramsPower.gg1);
								}
							}

							cycle = _cycle;

							break;
						}
						case thermo::input::TurbopumpFeedSystem::full_flow_staged_combustion: {
							//*** Full-flow staged-combustion cycle

							if (!paramsOx.main || !paramsFuel.main || !paramsOx.gg1 || !paramsFuel.gg1) {
								throw thermo::Exception(thermo::Exception::INVALID_STATE, ""
										"Improperly configured engine cycle.\n\n"
										"The following subsystems have to be configured:\n"
										"- oxidizer main feed branch\n"
										"- fuel main feed branch\n"
										"- fuel preburner feed branch #1\n"
										"- oxidizer preburner feed branch #1");
							}

							if (0==paramsPower.gg1->p) {
								paramsPower.gg1->p = p_c;
							}
							if (0==paramsPower.gg2->p) {
								paramsPower.gg2->p = p_c;
							}

							design::StagedCombustionCycle* _cycle = new design::StagedCombustionCycle(p_c, paramsOx.main, paramsFuel.main);

							_cycle->addPreburners(paramsFuel.gg1, paramsPower.gg1, paramsOx.gg1, paramsPower.gg2);

							cycle = _cycle;

							break;
						}
						case thermo::input::TurbopumpFeedSystem::expander: {
							//*** Expander cycle

							// TODO Not yet implemented
							throw thermo::Exception(thermo::Exception::INVALID_STATE,
									"Improperly configured fuel feed subsystem.\n\n"
									"Expander cycle is not supported in this version.");
						}
					}


					if (paramsOx.boostPump) {
						cycle->addOxBoostPump(paramsOx.boostPump);
					}
					if (paramsFuel.boostPump) {
						cycle->addFuelBoostPump(paramsFuel.boostPump);
					}

					for (unsigned int i=0; i<paramsOx.branches.size(); ++i) {
						cycle->addOxBranch(paramsOx.branches[i]);
					}

					for (unsigned int i=0; i<paramsFuel.branches.size(); ++i) {
						cycle->addFuelBranch(paramsFuel.branches[i]);
					}

					if (paramsPower.turbine1) {
						cycle->addTurbine(paramsPower.turbine1);
						if (paramsPower.turbine2) {
							cycle->addTurbine(paramsPower.turbine2, paramsPower.type);
						}
					} else {
						throw thermo::Exception(thermo::Exception::INVALID_STATE,
								"Improperly configured engine cycle.\n\n"
								"Turbine parameters have to be specified.");
					}

	//				cycle->print();

					// Solve the configured cycle
					cycle->solve();

				}

			} catch (const runtime::Exception& ex) {
				util::Log::errorf("THERMO", "Could not run cycle analysis: %s.%s", ex.what(), CR);
				throw;
			}

		}

	}

	void configureComponentSubsystem(
			double p_c, double m_dot,
			FeedSubsystem* params,
			thermo::input::ChamberFeedSubsystemParameters* feedSystem,
			thermo::input::GasGeneratorBranchParameters* gg1,
			thermo::input::GasGeneratorBranchParameters* gg2,
			thermo::input::BoostPumpParameters* boosterPump,
			std::vector<thermo::input::BranchParameters*>* branches
	) {

		params->main->m_dot = m_dot;

		params->main->p_inlet = feedSystem->getInletPressure();
		params->main->w_inlet = feedSystem->getInletVelocity();
		params->main->pump_eta = feedSystem->isPumpEtaSet()?feedSystem->getPumpEta():0;
		params->main->valve_dp = feedSystem->isValveDpSet()?feedSystem->getValveDp():0;
		params->main->valve_zeta = feedSystem->isValveZetaSet()?feedSystem->getValveZeta():0;
		params->main->cooling_dp = feedSystem->isCoolingDpSet()?feedSystem->getCoolingDp():0;
		params->main->cooling_dT = feedSystem->isCoolingDtSet()?feedSystem->getCoolingDt():0;
		params->main->injector_dp = feedSystem->isInjectorDpSet()?feedSystem->getInjectorDp():0;
		params->main->injector_mu = feedSystem->isInjectorMuSet()?feedSystem->getInjectorMu():0;

		params->main->p_outlet = p_c;

		if (params->main->c) {
			params->main->c->setP(params->main->p_inlet);
		}

		if (boosterPump) {
			params->boostPump = new design::BoostPumpParameters(p_c);

			params->boostPump->p_inlet = params->main->p_inlet;
			params->boostPump->p_outlet = boosterPump->getDischargePressure();
			params->boostPump->pump_eta = boosterPump->isPumpEtaSet()?boosterPump->getPumpEta():0.7;

			if (boosterPump->isHydraulicTurbineSet() && boosterPump->getHydraulicTurbine()) {
				// dedicated hydraulic turbine
				params->boostPump->hydraulicTurbine = true;
				params->boostPump->turb_pi = boosterPump->isTurbinePiSet()?boosterPump->getTurbinePi():0;
				params->boostPump->turb_eta = boosterPump->isTurbineEtaSet()?boosterPump->getTurbineEta():0;
			} else if (boosterPump->isTurbinePiSet() || boosterPump->isTurbineEtaSet()) {
				// dedicated gas turbine
				params->boostPump->hydraulicTurbine = false;
				params->boostPump->turb_pi = boosterPump->isTurbinePiSet()?boosterPump->getTurbinePi():0;
				params->boostPump->turb_eta = boosterPump->isTurbineEtaSet()?boosterPump->getTurbineEta():0;
			} else {
				// main gas turbine
				params->boostPump->hydraulicTurbine = false;
				params->boostPump->turb_pi = 0;
				params->boostPump->turb_eta = 0;
			}
		}

		auto configureBranch = [&m_dot](design::BranchParameters* branch, thermo::input::BranchParameters* b) {
			branch->m_dot = b->isMdotSet()? b->getMdot()*m_dot : 0.01*m_dot;
			branch->p_outlet = b->isDischargePressureSet()?b->getDischargePressure():0;
			branch->A_bar = b->isAbarSet()?b->getAbar():0;
			branch->pump_eta = b->isPumpEtaSet()?b->getPumpEta():0;
			branch->valve_dp = b->isValveDpSet()?b->getValveDp():0;
			branch->valve_zeta = b->isValveZetaSet()?b->getValveZeta():0;
			branch->cooling_dp = b->isCoolingDpSet()?b->getCoolingDp():0;
			branch->cooling_dT = b->isCoolingDtSet()?b->getCoolingDt():0;
			branch->fixed_dp = b->isFixedDpSet()?b->getFixedDp():0;
			branch->fixed_pi = b->isFixedPiSet()?b->getFixedPi():0;
			branch->connectTo = b->isConnectToSet()?b->getConnectTo().c_str():0;
			branch->dischargeTo = b->isDischargeToSet()?b->getDischargeTo().c_str():0;
		};

		if (gg1) {
			params->gg1 = new design::GasGeneratorBranchParameters(p_c);
			configureBranch(params->gg1, gg1);
			params->gg1->pipe_dp = gg1->isPipeDpSet()?gg1->getPipeDp():0;
			params->gg1->injector_dp = gg1->isInjectorDpSet()?gg1->getInjectorDp():0;
			params->gg1->injector_mu = gg1->isInjectorMuSet()?gg1->getInjectorMu():0;
		}

		if (gg2) {
			params->gg2 = new design::GasGeneratorBranchParameters(p_c);
			configureBranch(params->gg2, gg1);
			params->gg2->pipe_dp = gg2->isPipeDpSet()?gg2->getPipeDp():0;
			params->gg2->injector_dp = gg2->isInjectorDpSet()?gg2->getInjectorDp():0;
			params->gg2->injector_mu = gg2->isInjectorMuSet()?gg2->getInjectorMu():0;
		}

		for (unsigned int i=0; i<branches->size(); ++i) {
			thermo::input::BranchParameters* b = branches->at(i);
			design::BranchParameters* branch = new design::BranchParameters(b->getName().c_str());
			configureBranch(branch, b);
			params->branches.push_back(branch);
		}

	}

	void estimateCyclePerformance() {
		if (!chamber || !nozzle || !cycle) {
			return;
		}

		delete cyclePerformance;
		cyclePerformance = new CyclePerformance();

		performance::equilibrium::NozzleSectionConditions* exitSection = dynamic_cast<performance::equilibrium::NozzleSectionConditions*>(performance->getExitSection());

		performance::equilibrium::NozzleSectionConditions *overexpExitSection = performance->getOverExpansionSection();

		double phi =  1.0;
		performance::efficiency::CorrectionFactors* correctionFactors = getCorrectionFactors(performance);
		if (correctionFactors) {
			phi = correctionFactors->getOverallEfficiency();
			delete correctionFactors;
		}

		cyclePerformance->Is_c_v = exitSection->getIs_v() * phi;
		cyclePerformance->Is_c_opt = cyclePerformance->Is_c_v - exitSection->getF()*exitSection->getP();

		if (overexpExitSection) {
			// Dobrovolky, p.57, (2.27), (2.28)
			cyclePerformance->Is_c_SL = overexpExitSection->getIs_v()*phi - overexpExitSection->getF()*CONST_ATM;
			double p2 = overexpExitSection->getP();
			double IsH2 = 0.3*(CONST_ATM - p2)*(overexpExitSection->getF() - exitSection->getF());
			cyclePerformance->Is_c_SL += IsH2;
		} else {
			cyclePerformance->Is_c_SL = cyclePerformance->Is_c_v - exitSection->getF()*CONST_ATM;
		}

		// Total mass flow rate through the engine
		double mdot_e = 0;
		for (unsigned int i=0, size=cycle->getComponentFeedSystemSize(); i<size; ++i) {
			mdot_e += cycle->getComponentFeedSystem(i)->getFlowPath()->getInletPort()->getMDot();
		}

		// Total mass flow rate through the chambers
		double mdot_c = chamber->getMdot() * data->getEngineSize().getChambersNo();

		// Thrust of chamber
		cyclePerformance->T_c_v = cyclePerformance->Is_c_v * mdot_c / data->getEngineSize().getChambersNo();
		cyclePerformance->T_c_opt = cyclePerformance->Is_c_opt * mdot_c / data->getEngineSize().getChambersNo();
		cyclePerformance->T_c_SL = cyclePerformance->Is_c_SL * mdot_c / data->getEngineSize().getChambersNo();

		// Thrust of GG nozzles
		double T_gg_v = 0;
		double T_gg_opt = 0;
		double T_gg_SL = 0;

		if (dynamic_cast<design::GasGeneratorCycle*>(cycle)) {
			// Gas-generator

			design::GasGeneratorCycle* _cycle = dynamic_cast<design::GasGeneratorCycle*>(cycle);

			cyclePerformance->phi_e = 1.;

			for (unsigned int i=0, size=_cycle->getPowerSystemSize(); i<size; ++i) {
				double mdot_gg_i = cycle->getPowerSystem(i)->getFlowPath()->getOutletPort()->getMDot();

				double Is_gg_v = _cycle->getIs(i, 0, phi*0.7);
				double Is_gg_opt = _cycle->getIs(i, CONST_ATM*2., phi*0.7);	// Just and 0>pa>CONST_ATM
				double Is_gg_SL = _cycle->getIs(i, CONST_ATM, phi*0.7);

				T_gg_v += Is_gg_v * mdot_gg_i;
				T_gg_opt += Is_gg_opt * mdot_gg_i;
				T_gg_SL += Is_gg_SL * mdot_gg_i;
			}

		} else {

			// Correction factor due to mass flow rate through branches (bypassing thrust chambers)
			cyclePerformance->phi_e = mdot_c/mdot_e;

			// Engine performance
			cyclePerformance->Is_e_v = cyclePerformance->Is_c_v * cyclePerformance->phi_e;

		}

		cyclePerformance->T_e_v = cyclePerformance->T_c_v*data->getEngineSize().getChambersNo() + T_gg_v;
		cyclePerformance->T_e_opt = cyclePerformance->T_c_opt*data->getEngineSize().getChambersNo() + T_gg_opt;
		cyclePerformance->T_e_SL = cyclePerformance->T_c_SL*data->getEngineSize().getChambersNo() + T_gg_SL;

		cyclePerformance->Is_e_v = cyclePerformance->T_e_v / mdot_e;
		cyclePerformance->Is_e_opt = cyclePerformance->T_e_opt / mdot_e;
		cyclePerformance->Is_e_SL = cyclePerformance->T_e_SL / mdot_e;

		cyclePerformance->phi_e = cyclePerformance->Is_e_v / cyclePerformance->Is_c_v;

	}

	void estimateEngineMass() {
		if (!chamber || !nozzle || !cycle /*|| !data->getPropellantFeedSystem().isEstimateDryMass()*/) {
			return;
		}

		delete mass;
		mass = 0;

		try {

			thermo::input::PropellantFeedSystem& propellantFeedSystem = data->getPropellantFeedSystem();

			if (propellantFeedSystem.isPressurizedFeedSystemSet()) {
				mass = new design::MassEstimation(design::MassEstimation::PRESSURE_FED, nozzle);

			} else if (propellantFeedSystem.isTurbopumpFeedSystemSet()) {

				thermo::input::TurbopumpFeedSystem& tpSystem = data->getPropellantFeedSystem().getTurbopumpFeedSystem();

				switch (data->getPropellantFeedSystem().getTurbopumpFeedSystemCycle()) {
					case thermo::input::TurbopumpFeedSystem::gas_generator: {
						mass = new design::MassEstimation(design::MassEstimation::GAS_GENERATOR, nozzle);
						break;
					}
					case thermo::input::TurbopumpFeedSystem::staged_combustion: {
						mass = new design::MassEstimation(design::MassEstimation::STAGED_COMBUSTION, nozzle);
						break;
					}
					case thermo::input::TurbopumpFeedSystem::full_flow_staged_combustion: {
						mass = new design::MassEstimation(design::MassEstimation::STAGED_COMBUSTION, nozzle);
						break;
					}
					case thermo::input::TurbopumpFeedSystem::expander: {
						mass = new design::MassEstimation(design::MassEstimation::EXPANDER, nozzle);
						break;
					}
				}


				bool singleShaft = true;

				double boost_speed1=0, boost_p1_in=0, boost_p1_out=0;
				double speed1=0, p1_in=0, p1_out=0, rho1=0;

				double boost_speed2=0, boost_p2_in=0, boost_p2_out=0;
				double speed2=0, p2_in=0, p2_out=0, rho2=0;


				thermo::input::Propellant& prop = data->getPropellant();
				switch (prop.getRatioType()) {
					case thermo::input::Ratio::km:
					case thermo::input::Ratio::alpha:
					case thermo::input::Ratio::optimal:
					{
						// Bipropellant

						if (tpSystem.isTurbine1() && tpSystem.getTurbine1().isRotationalSpeedSet()) {
							speed1 = tpSystem.getTurbine1().getRotationalSpeed();

							if (tpSystem.isTurbine2() && tpSystem.getTurbine2().isRotationalSpeedSet()) {
								speed2 = tpSystem.getTurbine2().getRotationalSpeed();
								singleShaft = false;
							} else {
								speed2 = speed1;
							}
						}


						if (tpSystem.isOxidizerBoostPump() && tpSystem.getOxidizerBoostPump().isRotationalSpeedSet()) {
							boost_speed1 = tpSystem.getOxidizerBoostPump().getRotationalSpeed();
						} else {
							boost_speed1 = speed1;
						}
						if (tpSystem.isFuelBoostPump() && tpSystem.getFuelBoostPump().isRotationalSpeedSet()) {
							boost_speed2 = tpSystem.getFuelBoostPump().getRotationalSpeed();
						} else {
							boost_speed2 = speed2;
						}

						thermo::Mixture* oxMixture = new thermo::Mixture();
						for (size_t i=0, size=prop.getOxidizerListSize(); i<size; ++i) {
							thermo::input::Component& ox = prop.getOxidizer(i);
							if (ox.getMf()>0) {

								double T = ox.getT();
								if (0==T) {
									const thermo::Species* s = thermo::Database::getInstance()->find(ox.getName());
									T = s->getT0();
								}

								double p = ox.getP();
								if (0==p) {
									p = CONST_P0;
								}

								oxMixture->addSpecies(ox.getName(), T, p, ox.getMf());
							} else {
								util::Log::warnf("THERMO", "Skipping %s with mass fraction r=0%s", ox.getName().c_str(), CR);
							}
						}

						thermo::Mixture* fMixture = new thermo::Mixture();
						for (size_t i=0, size=prop.getFuelListSize(); i<size; ++i) {
							thermo::input::Component& f = prop.getFuel(i);
							if (f.getMf()>0) {
								double T = f.getT();
								if (0==T) {
									const thermo::Species* s = thermo::Database::getInstance()->find(f.getName());
									T = s->getT0();
								}

								double p = f.getP();
								if (0==p) {
									p = CONST_P0;
								}

								fMixture->addSpecies(f.getName(), T, p, f.getMf());
							} else {
								util::Log::warnf("THERMO", "Skipping %s with mass fraction r=0%s", f.getName().c_str(), CR);
							}
						}

						rho1 = design::Fluid::getRho(oxMixture);
						rho2 = design::Fluid::getRho(fMixture);

						delete oxMixture;
						delete fMixture;

						break;
					}
					case thermo::input::Ratio::fractions:
					{
						// Monopropellant

						if (tpSystem.isTurbine1() && tpSystem.getTurbine1().isRotationalSpeedSet()) {
							speed1 = tpSystem.getTurbine1().getRotationalSpeed();
						}

						if (tpSystem.isFuelBoostPump() && tpSystem.getFuelBoostPump().isRotationalSpeedSet()) {
							boost_speed1 = tpSystem.getFuelBoostPump().getRotationalSpeed();
						} else {
							boost_speed1 = speed1;
						}

						thermo::Mixture* mixture = new thermo::Mixture();
						for (size_t i=0, size=prop.getSpeciesListSize(); i<size; ++i) {
							thermo::input::Component& c = prop.getSpecies(i);
							if (c.getMf()>0) {

								double T = c.getT();
								if (0==T) {
									const thermo::Species* s = thermo::Database::getInstance()->find(c.getName());
									T = s->getT0();
								}

								double p = c.getP();
								if (0==p) {
									p = CONST_P0;
								}

								mixture->addSpecies(c.getName(), T, p, c.getMf());
							} else {
								util::Log::warnf("THERMO", "Skipping %s with mass fraction r=0%s", c.getName().c_str(), CR);
							}
						}

						rho1 = design::Fluid::getRho(mixture);

						delete mixture;

						break;
					}
				}


				for (unsigned int fsi=0, fs_size=cycle->getComponentFeedSystemSize(); fsi<fs_size; ++fsi) {
					design::ComponentFlowPath* fp = cycle->getComponentFeedSystem(fsi)->getFlowPath();

					if (0==fsi) {
						p1_in = fp->getInletPort()->getP();
					} else {
						p2_in = fp->getInletPort()->getP();
					}


					for (unsigned int i=0, path_size=fp->size(); i<path_size; ++i) {
						design::MassFlowElement* el = fp->getElement(i);

						if (dynamic_cast<design::Pump*>(el)) {

							if (0==strcmp("pump", el->getName())) {

								if (0==fsi) {
									p1_out = el->outletPort()->getP();
								} else {
									p2_out = el->outletPort()->getP();
								}

							} else
							if (0==strcmp("pump_b", el->getName())) {

								if (0==fsi) {
									boost_p1_in = p1_in;
									p1_in = boost_p1_out = el->outletPort()->getP();
								} else {
									boost_p2_in = p2_in;
									p2_in = boost_p2_out = el->outletPort()->getP();
								}

							}

						}

					}

				}

				if (speed1>0 && speed2>0) {

					mass->setTurbopum(
						speed1, p1_in, p1_out, rho1,
						speed2, p2_in, p2_out, rho2,
						singleShaft
					);


					if (boost_speed1>0 || boost_speed2>0) {
						mass->setBoosterTurbopum(
							boost_speed1, boost_p1_in, boost_p1_out, rho1,
							boost_speed2, boost_p2_in, boost_p2_out, rho2
						);
					}

				} else
				if (speed1>0) {

					mass->setTurbopum(
						speed1, p1_in, p1_out, rho1,
						0, 0, 0, 0,
						true
					);


					if (boost_speed1>0) {
						mass->setBoosterTurbopum(
							boost_speed1, boost_p1_in, boost_p1_out, rho1,
							0, 0, 0, 0
						);
					}

				} else {
					throw thermo::Exception(thermo::Exception::INVALID_STATE, "Improperly configured engine cycle: specify turbopump rotational speed.");
				}

			}

			// TODO
			double Fr_ext = 0.0;
			double nozzle_factor = 1.0;

			if (Fr_ext>0 && nozzle_factor<1.0) {
				mass->setNozzleExtension(chamber->getFre()*Fr_ext, nozzle_factor);
			}

			mass->getMass();

		} catch (const runtime::Exception& ex) {
			util::Log::errorf("THERMO", "Could not run cycle analysis: %s.%s", ex.what(), CR);
			throw;
		}

	}

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


	void parseCycle() {

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

	void parseMass() {
		const char* format1 = "%25s: %8.2f %s\n";

		printf("\nEngine dry mass estimation\n");
		printf(  "--------------------------\n");

		double m = mass->getMass(design::MassEstimation::SINGLE_CHAMBER) * mass->getNumberOfChambers();
		printf(format1, "chamber/s", m, "kg");

		m = mass->getMass(design::MassEstimation::TURBOPUMP);
		printf(format1, "turbopump/s", m, "kg");

		m = mass->getMass(design::MassEstimation::BOOSTER_TURBOPUMP);
		printf(format1, "booster turbopump/s", m, "kg");

		m = mass->getMass(design::MassEstimation::OTHER_COMPONENTS);
		printf(format1, "other components", m, "kg");

		m = mass->getMass(design::MassEstimation::TOTAL);
		printf(format1, "total", m, "kg");

		if (cyclePerformance) {
			double w = mass->getMass(design::MassEstimation::TOTAL)*CONST_G;

			double T2W_v = cyclePerformance->T_e_v / w;
			double T2W_opt = cyclePerformance->T_e_opt / w;
			double T2W_SL = cyclePerformance->T_e_SL / w;

			printf("\nThrust-to-Weight ratio estimation\n");
			printf(  "---------------------------------\n");

			printf(format1, "in vacuum", T2W_v, "");
			printf(format1, "at optimum expansion", T2W_opt, "");
			printf(format1, "at sea level", T2W_SL, "");
		}

	}

	void parseCyclePerformance() {
		const char* format1 = "%-25s\n";
		const char* format2 = "%25s: %8.2f %s\n";
		const char* format3 = "%25s: %8.2f\n";
		const char* format4 = "%25s: %8.2f %s (x%1d)\n";

		int n = data->getEngineSize().getChambersNo();

		printf("\nEngine performance estimation\n");
		printf(  "-----------------------------\n");

		printf(format1, "Chamber performance");

		printf(format2, "specific impulse (vac)", cyclePerformance->Is_c_v/CONST_G, "s");
		printf(format2, "specific impulse (opt)", cyclePerformance->Is_c_opt/CONST_G, "s");
		printf(format2, "specific impulse (SL)", cyclePerformance->Is_c_SL/CONST_G, "s");

		printf(format4, "thrust (vac)", cyclePerformance->T_c_v/1000., "kN", n);
		printf(format4, "thrust (opt)", cyclePerformance->T_c_opt/1000., "kN", n);
		printf(format4, "thrust (SL)", cyclePerformance->T_c_SL/1000., "kN", n);

		printf(format1, "Engine performance");

		printf(format2, "specific impulse (vac)", cyclePerformance->Is_e_v/CONST_G, "s");
		printf(format2, "specific impulse (opt)", cyclePerformance->Is_e_opt/CONST_G, "s");
		printf(format2, "specific impulse (SL)", cyclePerformance->Is_e_SL/CONST_G, "s");
		printf(format3, "correction factor (vac)", cyclePerformance->phi_e);

		printf(format2, "thrust (vac)", cyclePerformance->T_e_v/1000., "kN");
		printf(format2, "thrust (opt)", cyclePerformance->T_e_opt/1000., "kN");
		printf(format2, "thrust (SL)", cyclePerformance->T_e_SL/1000., "kN");
	}

};


int main(int argc, char* argv[]) {

	util::Log::createLog("ROOT")->
//		addLogger(new util::ConsoleLogger())->
		addLogger(new util::FileLogger("", 10*1024));

	// Initialize thermodatabase
	initThermoDatabase();


	RPAData rpaData("examples/cycle_analysis/RD-275.cfg");

	rpaData.chamberPerformance();
	rpaData.chamberGeometry(true);

	rpaData.engineCycleAnalysis();
	rpaData.estimateCyclePerformance();
	rpaData.estimateEngineMass();

	// Print out the results
	rpaData.parseCycle();
	rpaData.parseCyclePerformance();
	rpaData.parseMass();

	util::Log::finalize();

	return 0;
}


