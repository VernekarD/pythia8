// MultipleInteractions.h is a part of the PYTHIA event generator.
// Copyright (C) 2010 Torbjorn Sjostrand.
// PYTHIA is licenced under the GNU GPL version 2, see COPYING for details.
// Please respect the MCnet Guidelines, see GUIDELINES for details.

// This file contains the main classes for multiple interactions physics.
// SigmaMultiple stores allowed processes by in-flavour combination.
// MultipleInteractions: generates multiple parton-parton interactions.

#ifndef Pythia8_MultipleInteractions_H
#define Pythia8_MultipleInteractions_H

#include "Basics.h"
#include "BeamParticle.h"
#include "Event.h"
#include "Info.h"
#include "PartonSystems.h"
#include "PythiaStdlib.h"
#include "Settings.h"
#include "SigmaTotal.h"
#include "SigmaProcess.h"
#include "StandardModel.h"

namespace Pythia8 {
 
//==========================================================================

// SigmaMultiple is a helper class to MultipleInteractions.
// It packs pointers to the allowed processes for different 
// flavour combinations and levels of ambition.

class SigmaMultiple {

public:

  // Constructor.
  SigmaMultiple() {}
  
  // Destructor.
  ~SigmaMultiple() {
    for (int i = 0; i < int(sigmaT.size()); ++i) delete sigmaT[i];
    for (int i = 0; i < int(sigmaU.size()); ++i) delete sigmaU[i];}   

  // Initialize list of processes.
  bool init(int inState, int processLevel, Info* infoPtr, 
    Settings* settingsPtr, ParticleData* particleDataPtr, Rndm* rndmPtrIn, 
    BeamParticle* beamAPtr, BeamParticle* beamBPtr, CoupSM* coupSMPtr);

  // Calculate cross section summed over possibilities.
  double sigma( int id1, int id2, double x1, double x2, double sHat, 
    double tHat, double uHat, double alpS, double alpEM,
    bool restore = false, bool pickOtherIn = false);

  // Return whether the other, rare processes were selected.
  bool pickedOther() {return pickOther;} 

  // Return one subprocess, picked according to relative cross sections.
  SigmaProcess* sigmaSel();
  bool swapTU() {return pickedU;}

  // Return code or name of a specified process, for statistics table.
  int    nProc() const {return nChan;}
  int    codeProc(int iProc) const {return sigmaT[iProc]->code();}
  string nameProc(int iProc) const {return sigmaT[iProc]->name();}

private:

  // Constants: could only be changed in the code itself.
  static const double MASSMARGIN, OTHERFRAC;

  // Number of processes. Some use massive matrix elements.
  int            nChan;
  vector<bool>   needMasses;
  vector<double> m3Fix, m4Fix, sHatMin;

  // Vector of process list, one for t-channel and one for u-channel.
  vector<SigmaProcess*> sigmaT, sigmaU;

  // Values of cross sections in process list above.
  vector<double> sigmaTval, sigmaUval;
  double         sigmaTsum, sigmaUsum;
  bool           pickOther, pickedU;

  // Pointer to the random number generator.
  Rndm*          rndmPtr;
  
};
 
//==========================================================================

// The MultipleInteractions class contains the main methods for the 
// generation of multiple parton-parton interactions in hadronic collisions.

class MultipleInteractions {

public:

  // Constructor.
  MultipleInteractions() {}

  // Initialize the generation process for given beams.
  bool init( bool doMIinit, int diffractiveModeIn, Info* infoPtrIn, 
    Settings& settings, ParticleData* particleDataPtr, Rndm* rndmPtrIn, 
    BeamParticle* beamAPtrIn, BeamParticle* beamBPtrIn, CoupSM* coupSMPtrIn, 
    PartonSystems* partonSystemsPtrIn, SigmaTotal* sigmaTotPtrIn, 
    ostream& os = cout);

  // Reset impact parameter choice and update the CM energy.
  void reset();

  // Select first = hardest pT in minbias process.
  void pTfirst(); 

  // Set up kinematics for first = hardest pT in minbias process.
  void setupFirstSys( Event& process);

  // Find whether to limit maximum scale of emissions.
  bool limitPTmax( Event& event);

  // Prepare system for evolution.
  void prepare(double pTscale = 1000.) {
    if (!bSetInFirst) overlapNext(pTscale);}

  // Select next pT in downwards evolution.
  double pTnext( double pTbegAll, double pTendAll, Event& event);

  // Set up kinematics of acceptable interaction.
  void scatter( Event& event); 

  // Get some information on current interaction.
  double Q2Ren()     const {return pT2Ren;}
  double alphaSH()   const {return alpS;}
  double alphaEMH()  const {return alpEM;}
  double x1H()       const {return x1;} 
  double x2H()       const {return x2;} 
  double Q2Fac()     const {return pT2Fac;}
  double pdf1()      const {return xPDF1now;}
  double pdf2()      const {return xPDF2now;}
  double bMI()       const {return (bIsSet) ? bNow / bAvg : 0.;}
  double enhanceMI() const {return (bIsSet) ? enhanceB / zeroIntCorr : 1.;}

  // Update and print statistics on number of processes.
  void accumulate() { int iBeg = (infoPtr->isMinBias()) ? 0 : 1; 
    for (int i = iBeg; i < infoPtr->nMI(); ++i) ++nGen[ infoPtr->codeMI(i) ];}
  void statistics(bool resetStat = false, ostream& os = cout);
  
private: 

  // Constants: could only be changed in the code itself.
  static const bool   SHIFTFACSCALE, PREPICKRESCATTER;
  static const double SIGMAFUDGE, RPT20, PT0STEP, SIGMASTEP, PT0MIN,
                      EXPPOWMIN, PROBATLOWB, BSTEP, BMAX, EXPMAX, 
                      KCONVERGE, CONVERT2MB, ROOTMIN, ECMDEV;

  // Initialization data, read from Settings.
  bool   allowRescatter, allowDoubleRes;
  int    pTmaxMatch, alphaSorder, alphaEMorder, bProfile, processLevel, 
         rescatterMode, nQuarkIn, nSample, enhanceScreening;
  double alphaSvalue, Kfactor, pT0Ref, ecmRef, ecmPow, pTmin, coreRadius, 
         coreFraction, expPow, ySepResc, deltaYResc, sigmaPomP, 
         mMaxPertDiff, mMinPertDiff;

  // Other initialization data.
  bool   hasBaryonBeams, hasLowPow;
  int    diffractiveMode;
  double eCM, sCM, pT0, pT20, pT2min, pTmax, pT2max, pT20R, pT20minR, 
         pT20maxR, pT20min0maxR, pT2maxmin, sigmaND, pT4dSigmaMax, 
         pT4dProbMax, dSigmaApprox, sigmaInt, sudExpPT[101], 
         zeroIntCorr, normOverlap, nAvg, kNow, normPi, bAvg, bDiv, 
         probLowB, radius2B, radius2C, fracA, fracB, fracC, fracAhigh, 
         fracBhigh, fracChigh, fracABChigh, expRev, cDiv, cMax;

  // Properties specific to current system.
  bool   bIsSet, bSetInFirst, isAtLowB, pickOtherSel;
  int    id1, id2, i1Sel, i2Sel, id1Sel, id2Sel;
  double bNow, enhanceB, pT2, pT2shift, pT2Ren, pT2Fac, x1, x2, xT, xT2, 
         tau, y, sHat, tHat, uHat, alpS, alpEM, xPDF1now, xPDF2now,
         dSigmaSum, x1Sel, x2Sel, sHatSel, tHatSel, uHatSel;

  // Stored values for mass interpolation for diffractive systems.
  int    nStep, iStepFrom, iStepTo; 
  double eCMsave, eStepSize, eStepSave, eStepFrom, eStepTo, pT0Save[5], 
         pT4dSigmaMaxSave[5], pT4dProbMaxSave[5], sigmaIntSave[5], 
         sudExpPTSave[5][101], zeroIntCorrSave[5], normOverlapSave[5], 
         kNowSave[5], bAvgSave[5], bDivSave[5], probLowBSave[5], 
         fracAhighSave[5], fracBhighSave[5], fracChighSave[5], 
         fracABChighSave[5], cDivSave[5], cMaxSave[5];

  // Pointer to various information on the generation.
  Info*          infoPtr;

  // Pointer to the random number generator.
  Rndm*          rndmPtr;

  // Pointers to the two incoming beams.
  BeamParticle*  beamAPtr;
  BeamParticle*  beamBPtr;

  // Pointers to Standard Model couplings.
  CoupSM*        coupSMPtr;

  // Pointer to information on subcollision parton locations.
  PartonSystems* partonSystemsPtr;

  // Pointer to total cross section parametrization.
  SigmaTotal*    sigmaTotPtr;

  // Collections of parton-level 2 -> 2 cross sections. Selected one.
  SigmaMultiple  sigma2gg, sigma2qg, sigma2qqbarSame, sigma2qq;
  SigmaMultiple* sigma2Sel;
  SigmaProcess*  dSigmaDtSel;

  // Statistics on generated 2 -> 2 processes.
  map<int, int>  nGen;

  // alphaStrong and alphaEM calculations.
  AlphaStrong    alphaS;
  AlphaEM        alphaEM;

  // Scattered partons.
  vector<int>    scatteredA, scatteredB;

  // Determine constant in d(Prob)/d(pT2) < const / (pT2 + r * pT20)^2.  
  void upperEnvelope();

  // Integrate the parton-parton interaction cross section.
  void jetCrossSection();

  // Evaluate "Sudakov form factor" for not having a harder interaction.
  double sudakov(double pT2sud, double enhance = 1.);

  // Do a quick evolution towards the next smaller pT.
  double fastPT2( double pT2beg);

  // Calculate the actual cross section, either for the first interaction
  // (including at initialization) or for any subsequent in the sequence. 
  double sigmaPT2scatter(bool isFirst = false);

  // Find the partons that may rescatter.
  void findScatteredPartons( Event& event); 

  // Calculate the actual cross section for a rescattering. 
  double sigmaPT2rescatter( Event& event);

  // Calculate factor relating matter overlap and interaction rate.
  void overlapInit();

  // Pick impact parameter and interaction rate enhancement,
  // either before the first interaction (for minbias) or after it.
  void overlapFirst();
  void overlapNext(double pTscale);

};
 
//==========================================================================

} // end namespace Pythia8

#endif // Pythia8_MultipleInteractions_H
