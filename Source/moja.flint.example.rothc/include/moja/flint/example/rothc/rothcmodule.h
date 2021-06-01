#pragma once

#include "moja/flint/example/rothc/_modules.rothc_exports.h"

#include <moja/flint/imodule.h>
#include <moja/flint/modulebase.h>

namespace moja::flint::example::rothc {

class ROTHC_API RothCModule : public ModuleBase {
   double evapoOpenRatio;

   double fracHumsToBios;
   double fracHumsToHums;

   double fracPbioToBiof;
   double fracPbioToHums;
   double fracSdcmToAtms;

   double maxTSMDBare;
   double maxTSMDCvrd;

   double rateModifyingFactor;

   /// <summary>
   /// Decomposition rate constants
   /// The decomposition rate constants (k), in years-1, for each compartment
   /// </summary>
   double sdcmRateMultDpm;
   double sdcmRateMultRpm;
   double sdcmRateMultBiof;
   double sdcmRateMultBios;
   double sdcmRateMultHums;

  public:
   RothCModule() = default;
   ~RothCModule() = default;

   void configure(const DynamicObject& config) override;
   void subscribe(NotificationCenter& notificationCenter) override;

   void onLocalDomainInit() override;
   void onTimingInit() override;
   void onTimingStep() override;

  private:
   void ComputeTSMDParams(double clayFrac, double sampleDepth, double bareToCvrdRatio);
   void InitializeForASimulation();
   void UpdateRateProperties();
   void SubmitMoves();

   /// <summary>
   ///  Soil organic carbon active compartments
   /// </summary>
   const IPool* _soilDPM;   // Decomposable Plant Material (DPM)
   const IPool* _soilRPM;   // Resistant Plant Material (RPM)
   const IPool* _soilBioF;  // Microbial Biomass (BIO) Fast
   const IPool* _soilBioS;  // Microbial Biomass (BIO) Slow
   const IPool* _soilHUM;   // Humified Organic Matter (HUM).
   const IPool* _soilIOM;
   const IPool* _atmosphere;
   const IPool* _initialValues;

   const IVariable* _rainfall;
   const IVariable* _avgAirTemp;
   const IVariable* _openPanEvap;
   const IVariable* _isSoilCovered;
   const IVariable* _clayFrac;
   const IVariable* _initSoil;
   const IVariable* _soil;

   IVariable* _TSMD;
};

}  // namespace moja::flint::example::rothc
