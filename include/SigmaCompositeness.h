// SigmaCompositeness.h is a part of the PYTHIA event generator.
// Copyright (C) 2010 Torbjorn Sjostrand.
// PYTHIA is licenced under the GNU GPL version 2, see COPYING for details.
// Please respect the MCnet Guidelines, see GUIDELINES for details.

// Header file for compositiness-process differential cross sections.
// Contains classes derived from SigmaProcess via Sigma(1/2)Process.

#ifndef Pythia8_SigmaCompositeness_H
#define Pythia8_SigmaCompositeness_H

#include "SigmaProcess.h"

namespace Pythia8 {
 
//==========================================================================

// A derived class for q g -> q^* (excited quark state).

class Sigma1qg2qStar : public Sigma1Process {

public:

  // Constructor.
  Sigma1qg2qStar(int idqIn) : idq(idqIn) {}

  // Initialize process. 
  virtual void initProc(); 

  // Calculate flavour-independent parts of cross section.
  virtual void sigmaKin();

  // Evaluate sigmaHat(sHat). 
  virtual double sigmaHat();

  // Select flavour, colour and anticolour.
  virtual void setIdColAcol();

  // Evaluate weight for q* decay angles (else inactive).
  virtual double weightDecay( Event& process, int iResBeg, int iResEnd); 

  // Info on the subprocess.
  virtual string name()       const {return nameSave;}
  virtual int    code()       const {return codeSave;}
  virtual string inFlux()     const {return "qg";}
  virtual int    resonanceA() const {return idRes;}

private:

  // Parameters set at initialization or for current kinematics. 
  int    idq, idRes, codeSave;
  string nameSave;
  double mRes, GammaRes, m2Res, GamMRat, Lambda, coupFcol, widthIn, sigBW;

  // Pointer to properties of the particle species, to access decay channels.
  ParticleDataEntry* qStarPtr;

};
 
//==========================================================================

// A derived class for l gamma -> q^* (excited lepton state).

class Sigma1lgm2lStar : public Sigma1Process {

public:

  // Constructor.
  Sigma1lgm2lStar(int idlIn) : idl(idlIn) {}

  // Initialize process. 
  virtual void initProc(); 

  // Calculate flavour-independent parts of cross section.
  virtual void sigmaKin();

  // Evaluate sigmaHat(sHat). 
  virtual double sigmaHat();

  // Select flavour, colour and anticolour.
  virtual void setIdColAcol();

  // Evaluate weight for l* decay angles (else inactive).
  virtual double weightDecay( Event& process, int iResBeg, int iResEnd); 

  // Info on the subprocess.
  virtual string name()       const {return nameSave;}
  virtual int    code()       const {return codeSave;}
  virtual string inFlux()     const {return "fgm";}
  virtual int    resonanceA() const {return idRes;}

private:

  // Parameters set at initialization or for current kinematics. 
  int    idl, idRes, codeSave;
  string nameSave;
  double mRes, GammaRes, m2Res, GamMRat, Lambda, coupChg, widthIn, sigBW;

  // Pointer to properties of the particle species, to access decay channels.
  ParticleDataEntry* qStarPtr;

};
 
//==========================================================================

// A derived class for q q' -> q^* q' (excited quark state).

class Sigma2qq2qStarq : public Sigma2Process {

public:

  // Constructor.
  Sigma2qq2qStarq(int idqIn) : idq(idqIn) {}

  // Initialize process. 
  virtual void initProc(); 

  // Calculate flavour-independent parts of cross section.
  virtual void sigmaKin();

  // Evaluate sigmaHat(sHat). 
  virtual double sigmaHat();

  // Select flavour, colour and anticolour.
  virtual void setIdColAcol();

  // Info on the subprocess.
  virtual string name()       const {return nameSave;}
  virtual int    code()       const {return codeSave;}
  virtual string inFlux()     const {return "qq";}
  virtual int    id3Mass()    const {return idRes;}

private:

  // Parameters set at initialization or for current kinematics. 
  int    idq, idRes, codeSave;
  string nameSave;
  double Lambda, preFac, openFracPos, openFracNeg, sigmaA, sigmaB;

};
 
//==========================================================================

// A derived class for q qbar -> l^* lbar (excited lepton state).

class Sigma2qqbar2lStarlbar : public Sigma2Process {

public:

  // Constructor.
  Sigma2qqbar2lStarlbar(int idlIn) : idl(idlIn) {}

  // Initialize process. 
  virtual void initProc(); 

  // Calculate flavour-independent parts of cross section.
  virtual void sigmaKin();

  // Evaluate sigmaHat(sHat). 
  virtual double sigmaHat() {return sigma;}

  // Select flavour, colour and anticolour.
  virtual void setIdColAcol();

  // Info on the subprocess.
  virtual string name()       const {return nameSave;}
  virtual int    code()       const {return codeSave;}
  virtual string inFlux()     const {return "qqbarSame";}
  virtual int    id3Mass()    const {return idRes;}

private:

  // Parameters set at initialization or for current kinematics. 
  int    idl, idRes, codeSave;
  string nameSave;
  double Lambda, preFac, openFracPos, openFracNeg, sigma;

};

//==========================================================================

// A derived class for q q -> q q (quark contact interactions).
// Based on, Sigma2qq2qq (QCD).

class Sigma2QCqq2qq : public Sigma2Process {

public:

  // Constructor.
  Sigma2QCqq2qq(){}

  // Initialize process. 
  virtual void initProc(); 

  // Calculate flavour-independent parts of cross section.
  virtual void sigmaKin();

  // Evaluate d(sigmaHat)/d(tHat). 
  virtual double sigmaHat();

  // Select flavour, colour and anticolour.
  virtual void setIdColAcol();

  // Info on the subprocess.
  virtual string name()   const {return "q q(bar)' -> (QC) -> q q(bar)'";}
  virtual int    code()   const {return 4201;}
  virtual string inFlux() const {return "qq";}

 private:

  // Values stored for colour flow selection.
  double sigT, sigU, sigTU, sigST, sigSum, sigQCSTU, sigQCUTS;

  // Compositeness parameters.
  double m_Lambda2;
  int    m_etaLL, m_etaRR, m_etaLR;

};

//==========================================================================

// A derived class for q qbar -> q' qbar' (quark contact interactions).
// Based on, Sigma2qqbar2qqbarNew(QCD). 
// Note: This process give the same contributions for q == q' and q != q'.

class Sigma2QCqqbar2qqbar : public Sigma2Process {

public:

  // Constructor.
  Sigma2QCqqbar2qqbar(){}

  // Initialize process. 
  virtual void initProc(); 

  // Calculate flavour-independent parts of cross section.
  virtual void sigmaKin();

  // Evaluate d(sigmaHat)/d(tHat). 
  virtual double sigmaHat() {return sigma;}

  // Select flavour, colour and anticolour.
  virtual void setIdColAcol();

  // Info on the subprocess.
  virtual string name()   const {return "q qbar -> (QC) -> q' qbar' (uds)";}
  virtual int    code()   const {return 4202;}
  virtual string inFlux() const {return "qqbarSame";}

 private:

  // Number of outgoing quark flavours to be considered, given that
  // matrix elements are calculated in the massless approximation.
  int    m_nQuarkNew;

  // Values stored for colour flow selection.
  int    idNew;
  double mNew, m2New, sigS, sigma;

  // Compositeness parameters.
  double m_Lambda2;
  int    m_etaLL, m_etaRR, m_etaLR;

};

//==========================================================================

} // end namespace Pythia8

#endif // Pythia8_SigmaCompositeness_H
