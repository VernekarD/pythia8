// SusyCouplings.cc is a part of the PYTHIA event generator.
// Copyright (C) 2010 Peter Skands, Torbjorn Sjostrand.
// PYTHIA is licenced under the GNU GPL version 2, see COPYING for details.
// Please respect the MCnet Guidelines, see GUIDELINES for details.

// Function definitions (not found in the header) for the 
// supersymmetric couplings class. 

#include "SusyCouplings.h"

namespace Pythia8 {

//==========================================================================

// The CoupSUSY class.

//--------------------------------------------------------------------------

// Constants: could be changed here if desired, but normally should not.
// These are of technical nature, as described for each.

// Allow verbose printout for debug purposes.
const bool CoupSUSY::DEBUG = false;

//--------------------------------------------------------------------------

// Initialize SM+SUSY couplings (only performed once).

void CoupSUSY::init (SusyLesHouches* slhaPtrIn, Settings* settingsPtrIn, 
  ParticleData* particleDataPtrIn, CoupSM* coupSMPtrIn) {

  // Save pointers..
  slhaPtr         = slhaPtrIn;
  settingsPtr     = settingsPtrIn;
  particleDataPtr = particleDataPtrIn;
  coupSMPtr       = coupSMPtrIn;
  
  // Is NMSSM switched on?
  isNMSSM = (slhaPtr->modsel(3) != 1 ? false : true);
  settingsPtr->flag("SLHA:NMSSM",isNMSSM);
  int nNeut = (isNMSSM ? 5 : 4);
  int nChar = 2;

  // Initialize pole masses 
  mZpole    = particleDataPtr->m0(23);
  wZpole    = particleDataPtr->mWidth(23);
  mWpole    = particleDataPtr->m0(24);
  wWpole    = particleDataPtr->mWidth(24);
  
  // Running masses and weak mixing angle 
  // (default to pole values if no running available)
  mW        = mWpole;
  mZ        = mZpole;
  sin2W     = 1.0 - pow(mW/mZ,2);  

  if (slhaPtr->gauge.exists(1) && slhaPtr->gauge.exists(2) 
      && slhaPtr->hmix.exists(3)) {
    double gp=slhaPtr->gauge(1);
    double g =slhaPtr->gauge(2);
    double v =slhaPtr->hmix(3);
    mW      = g * v / 2.0;
    mZ      = sqrt(pow(gp,2)+pow(g,2)) * v / 2.0;
    double tan2W   = pow2(gp)/pow2(g);
    if (DEBUG) cout << " tan2W = " << tan2W << endl;
    sin2W   = pow2(gp)/(pow2(g)+pow2(gp));  
  }
  sinW = sqrt(sin2W);
  cosW = sqrt(1.0-sin2W);

  // Tan(beta)
  // By default, use the running one in HMIX (if not found, use MINPAR)
  tanb = slhaPtr->hmix.exists(2) ? slhaPtr->hmix(2) : slhaPtr->minpar(3);
  cosb = sqrt( 1.0 / (1.0 + tanb*tanb) );
  sinb = sqrt(max(0.0,1.0-cosb*cosb));
  
  // tmp : verbose output
  if (DEBUG) {
    cout << " sin2W(Q) = " << sin2W << "  mW(Q) = " << mW 
         << "  mZ(Q) = " << mZ << endl;
    cout << " vev(Q) = " << slhaPtr->hmix(3) << " tanb(Q) = " << tanb
         << endl;
    for (int i=1;i<=3;i++) {
      for (int j=1;j<=3;j++) {
	cout << " VCKM  [" << i << "][" << j << "] = " 
             << scientific << setw(10) << coupSMPtr->VCKMgen(i,j) << endl;
      }
    }
  }  
  
  // Shorthand for squark mixing matrices 
  SusyLesHouches::matrixblock<6> Ru(slhaPtr->usqmix);
  SusyLesHouches::matrixblock<6> Rd(slhaPtr->dsqmix);
  SusyLesHouches::matrixblock<6> imRu(slhaPtr->imusqmix);
  SusyLesHouches::matrixblock<6> imRd(slhaPtr->imusqmix);
  
  // Construct ~g couplings
  for (int i=1 ; i<=6 ; i++) {
    for (int j=1 ; j<=3 ; j++) {
      LsddG[i][j] = complex( Rd(i,j)  ,  imRd(i,j));
      RsddG[i][j] = complex(-Rd(i,j+3), -imRd(i,j+3));
      LsuuG[i][j] = complex( Ru(i,j)  ,  imRu(i,j));
      RsuuG[i][j] = complex(-Ru(i,j+3), -imRu(i,j+3));
    }
  }
  
  // Construct qqZ couplings
  for (int i=1 ; i<=6 ; i++) {
    
    // q[i] q[i] Z (def with extra factor 2 compared to [Okun])
    LqqZ[i] = coupSMPtr->af(i) - 2.0*coupSMPtr->ef(i)*sin2W ;
    RqqZ[i] =                  - 2.0*coupSMPtr->ef(i)*sin2W ;

    // tmp: verbose output
    if (DEBUG) {
      cout << " LqqZ  [" << i << "][" << i << "] = " 
           << scientific << setw(10) << LqqZ[i] 
           << " RqqZ  [" << i << "][" << i  << "] = " 
           << scientific << setw(10) << RqqZ[i] << endl;
    }
  }

  // Construct ~q~qZ couplings
  for (int i=1 ; i<=6 ; i++) {

    // Squarks can have off-diagonal couplings as well
    for (int j=1 ; j<=6 ; j++) {
      
      // ~d[i] ~d[j] Z
      LsdsdZ[i][j] = 0.0;
      RsdsdZ[i][j] = 0.0;
      for (int k=1;k<=3;k++) {
	complex Rdik  = complex(Rd(i,k),  imRd(i,k)  );
	complex Rdjk  = complex(Rd(j,k),  imRd(j,k)  );
	complex Rdik3 = complex(Rd(i,k+3),imRd(i,k+3));
	complex Rdjk3 = complex(Rd(j,k+3),imRd(j,k+3));
	LsdsdZ[i][j] += LqqZ[1] * (Rdik*conj(Rdjk)); 
	RsdsdZ[i][j] += RqqZ[1] * (Rdik3*conj(Rdjk3)); 
      }
      
      // ~u[i] ~u[j] Z
      LsusuZ[i][j] = 0.0;
      RsusuZ[i][j] = 0.0; 
      for (int k=1;k<=3;k++) {
	complex Ruik  = complex(Ru(i,k)  ,imRu(i,k)  );
	complex Rujk  = complex(Ru(j,k)  ,imRu(j,k)  );
	complex Ruik3 = complex(Ru(i,k+3),imRu(i,k+3));
	complex Rujk3 = complex(Ru(j,k+3),imRu(j,k+3));
	LsusuZ[i][j] += LqqZ[2] * (Ruik*conj(Rujk)); 
	RsusuZ[i][j] += RqqZ[2] * (Ruik3*conj(Rujk3)); 
      }
      
    // tmp: verbose output
      if (DEBUG) {
	if (max(abs(LsdsdZ[i][j]),abs(RsdsdZ[i][j])) > 1e-6) {
	  cout << " LsdsdZ[" << i << "][" << j << "] = " 
               << scientific << setw(10) << LsdsdZ[i][j]
	       << " RsdsdZ[" << i << "][" << j << "] = " 
               << scientific << setw(10) << RsdsdZ[i][j] << endl;
	}
	if (max(abs(LsusuZ[i][j]),abs(RsusuZ[i][j]))> 1e-6) {
	  cout << " LsusuZ[" << i << "][" << j << "] = " 
               << scientific << setw(10) << LsusuZ[i][j]
	       << " RsusuZ[" << i << "][" << j << "] = " 
               << scientific << setw(10) << RsusuZ[i][j] << endl;
	}
      }
	
    }
    
  }
  
  // Construct udW couplings
  
  // Loop over up [i] and down [j] quark generation
  for (int i=1;i<=3;i++) {
    for (int j=1;j<=3;j++) {
      
      // CKM matrix (use Pythia one if no SLHA)
      // (NB: could also try input one if no running one found, but
      // would then need to compute from Wolfenstein)
      complex Vij=coupSMPtr->VCKMgen(i,j);
      if (slhaPtr->vckm.exists()) {
	Vij=complex(slhaPtr->vckm(i,j),slhaPtr->imvckm(i,j));
      }
      
      // u[i] d[j] W  
      LudW[i][j] = sqrt(2.0) * cosW * Vij;
      RudW[i][j] = 0.0;
            
      // tmp: verbose output
      if (DEBUG) {
	cout << " LudW  [" << i << "][" << j << "] = " 
             << scientific << setw(10) << LudW[i][j]
	     << " RudW  [" << i << "][" << j << "] = " 
             << scientific << setw(10) << RudW[i][j] << endl;
      }
    }
  }

  // Construct ~u~dW couplings

  // Loop over ~u[k] and ~d[l] flavours
  for (int k=1;k<=6;k++) {
    for (int l=1;l<=6;l++) {
	  
      LsusdW[k][l]=0.0; 
      RsusdW[k][l]=0.0;

      // Loop over u[i] and d[j] flavours
      for (int i=1;i<=3;i++) { 
	for (int j=1;j<=3;j++) {
	  
	  // CKM matrix (use Pythia one if no SLHA)
	  // (NB: could also try input one if no running one found, but
	  // would then need to compute from Wolfenstein)
	  complex Vij=coupSMPtr->VCKMgen(i,j);
	  if (slhaPtr->vckm.exists()) {
	    Vij=complex(slhaPtr->vckm(i,j),slhaPtr->imvckm(i,j));
	  }
      
	  // ~u[k] ~d[l] W (add one term for each quark flavour i,j)
	  complex Ruki = complex(Ru(k,i),imRu(k,i));
	  complex Rdlj = complex(Rd(l,j),imRd(l,j));
	  LsusdW[k][l] += sqrt(2.0) * cosW * Vij * Ruki * conj(Rdlj);
	  RsusdW[k][l] += 0.0;
	  
	}
      }

      // tmp: verbose output
      if (DEBUG) {
	if (max(abs(LsusdW[k][l]),abs(RsusdW[k][l]))> 1e-6) {
	  cout << " LsusdW[" << k << "][" << l << "] = " 
               << scientific << setw(10) << LsusdW[k][l]
	       << " RsusdW[" << k << "][" << l << "] = " 
               << scientific << setw(10) << RsusdW[k][l] << endl;
	}
      }

    }
  }
  
  // Now we come to the ones with really many indices
  
  // Construct ~chi0 couplings (allow for 5 neutralinos in NMSSM)
  for (int i=1;i<=nNeut;i++) {
    
    // Ni1, Ni2, Ni3, Ni4, Ni5
    complex ni1,ni2,ni3,ni4,ni5;
    if (not isNMSSM) {	
      ni1=complex( slhaPtr->nmix(i,1), slhaPtr->imnmix(i,1) );
      ni2=complex( slhaPtr->nmix(i,2), slhaPtr->imnmix(i,2) );
      ni3=complex( slhaPtr->nmix(i,3), slhaPtr->imnmix(i,3) );
      ni4=complex( slhaPtr->nmix(i,4), slhaPtr->imnmix(i,4) );
      ni5=complex( 0.0, 0.0);
    } else {
      ni1=complex( slhaPtr->nmnmix(i,1), slhaPtr->imnmnmix(i,1) );
      ni2=complex( slhaPtr->nmnmix(i,2), slhaPtr->imnmnmix(i,2) );
      ni3=complex( slhaPtr->nmnmix(i,3), slhaPtr->imnmnmix(i,3) );
      ni4=complex( slhaPtr->nmnmix(i,4), slhaPtr->imnmnmix(i,4) );
      ni5=complex( slhaPtr->nmnmix(i,5), slhaPtr->imnmnmix(i,5) );
    }
    
    // Change to positive mass convention
    complex iRot( 0., 1.);
    if (slhaPtr->mass(idNeut(i)) < 0.) {
      ni1 *= iRot;
      ni2 *= iRot;
      ni3 *= iRot;
      ni4 *= iRot;
      ni5 *= iRot;
    }
    
    // ~chi0 [i] ~chi0 [j] Z : loop over [j]
    for (int j=1; j<=nNeut; j++) {
      
      // neutralino [j] higgsino components
      complex nj3, nj4;
      if (not isNMSSM) {
	nj3=complex( slhaPtr->nmix(j,3), slhaPtr->imnmix(j,3) );
	nj4=complex( slhaPtr->nmix(j,4), slhaPtr->imnmix(j,4) );
      } else {
	nj3=complex( slhaPtr->nmnmix(j,3), slhaPtr->imnmnmix(j,3) );
	nj4=complex( slhaPtr->nmnmix(j,4), slhaPtr->imnmnmix(j,4) );
      }
      // Change to positive mass convention
      if (slhaPtr->mass(idNeut(j)) < 0.) {
	nj3 *= iRot;
	nj4 *= iRot;
      }
      
      // ~chi0 [i] ~chi0 [j] Z : couplings
      OLpp[i][j] = -0.5 * ni3 * conj(nj3) + 0.5 * ni4 * conj(nj4);
      ORpp[i][j] =  0.5 * conj(ni3) * nj3 - 0.5 * conj(ni4) * nj4;
      
    // tmp: verbose output
      if (DEBUG) {
	cout << " OL''  [" << i << "][" << j << "] = " 
             << scientific << setw(10) << OLpp[i][j]
	     << " OR''  [" << i << "][" << j << "] = " 
             << scientific << setw(10) << ORpp[i][j] << endl;
      }
	
    }
    
    // ~chi0 [i] ~chi+ [j] W : loop over [j]
    for (int j=1; j<=nChar; j++) {
      
      // Chargino mixing
      complex uj1, uj2, vj1, vj2;      
      uj1=complex( slhaPtr->umix(j,1), slhaPtr->imumix(j,1) );
      uj2=complex( slhaPtr->umix(j,2), slhaPtr->imumix(j,2) );
      vj1=complex( slhaPtr->vmix(j,1), slhaPtr->imvmix(j,1) );
      vj2=complex( slhaPtr->vmix(j,2), slhaPtr->imvmix(j,2) );
      
      // ~chi0 [i] ~chi+ [j] W : couplings
      OL[i][j] = -1.0/sqrt(2.0)*ni4*conj(vj2)+ni2*conj(vj1);
      OR[i][j] = 1.0/sqrt(2.0)*conj(ni3)*uj2+conj(ni2)*uj1;
      
      // tmp: verbose output
      if (DEBUG) {
	cout << " OL    [" << i << "][" << j << "] = " 
             << scientific << setw(10) << OL[i][j]
	     << " OR    [" << i << "][" << j << "] = " 
             << scientific << setw(10) << OR[i][j] << endl;
      }
    }
    
    // Charges
    double ed  = -1.0/3.0;
    double T3d = -0.5;
    double eu  =  2.0/3.0;
    double T3u =  0.5;
    
    // Loop over quark [k] generation
    for (int k=1;k<=3;k++) {
      
      // Set quark masses
      // Initial guess 0,0,0,mc,mb,mt with the latter from the PDT
      double mu = particleDataPtr->m0(2*k);
      double md = particleDataPtr->m0(2*k-1);
      if (k == 1) { mu=0.0 ; md=0.0; }
      if (k == 2) { md=0.0 ; mu=0.0; } 
      
      // Compute running mass from Yukawas and vevs if possible.
      if (slhaPtr->yd.exists() && slhaPtr->hmix.exists(3)) {
	double ykk=slhaPtr->yd(k,k);
	double v1=slhaPtr->hmix(3)/sqrt(1+pow(tanb,2));
	if (ykk > 0.0) md = ykk * v1 / sqrt(2.0) ;
      }
      if (slhaPtr->yu.exists() && slhaPtr->hmix.exists(3)) {
	double ykk=slhaPtr->yu(k,k);
	double v2=slhaPtr->hmix(3)/sqrt(1.0+1.0/pow(tanb,2));
	if (ykk > 0.0) mu = ykk * v2 / sqrt(2.0) ;
      }
      
      // tmp: verbose output
      if (DEBUG) {
	cout  <<  " Gen = " << k << " mu = " << mu << " md = " << md 
              << " yUU,DD = " << slhaPtr->yu(k,k) << "," 
              << slhaPtr->yd(k,k) << endl;
      }
      
      // Loop over squark [j] flavour
      for (int j=1;j<=6;j++) {
	
	// Squark mixing
	complex Rdjk  = complex(Rd(j,k),  imRd(j,k)  );
	complex Rdjk3 = complex(Rd(j,k+3),imRd(j,k+3));
	complex Rujk  = complex(Ru(j,k),  imRu(j,k)  );
	complex Rujk3 = complex(Ru(j,k+3),imRu(j,k+3));
	
	// ~d[j] d[k] ~chi0[i]
	double rt2 = sqrt(2.0);
	LsddX[j][k][i] = ((ed-T3d)*sinW/cosW*ni1 + T3d*ni2)*conj(Rdjk)/rt2
	  + md*ni3*conj(Rdjk3)/2.0/rt2/mW/cosb; 
	RsddX[j][k][i] = -ed*sinW/cosW*conj(ni1)*conj(Rdjk3)/rt2 
	  + md*conj(ni3)*conj(Rdjk)/2.0/rt2/mW/cosb;

	// ~u[j] u[k] ~chi0[i]
	LsuuX[j][k][i] = ((eu-T3u)*sinW/cosW*ni1 + T3u*ni2)*conj(Rujk)/rt2
	  + mu*ni4*conj(Rujk3)/2.0/rt2/mW/sinb;
	RsuuX[j][k][i] = -eu*sinW/cosW*conj(ni1)*conj(Rujk3)/rt2
	  + mu*conj(ni4)*conj(Rujk)/2.0/rt2/mW/sinb;

	if (DEBUG) {
	  if (abs(LsddX[j][k][i]) > 1e-6) {
	    // tmp: verbose output
	    cout << " LsddX[" << j << "][" << k << "][" << i << "] = "
		 << scientific << setw(10) << LsddX[j][k][i] << endl;
	  }
	  if (abs(RsddX[j][k][i]) > 1e-6) {
	    // tmp: verbose output
	    cout << " RsddX[" << j << "][" << k << "][" << i << "] = "
		 << scientific << setw(10) << RsddX[j][k][i] << endl;
	  }
	  if (abs(LsuuX[j][k][i]) > 1e-6) {
	    // tmp: verbose output
	    cout << " LsuuX[" << j << "][" << k << "][" << i << "] = "
		 << scientific << setw(10) << LsuuX[j][k][i] << endl;
	  }
	  if (abs(RsuuX[j][k][i]) > 1e-6) {
	    // tmp: verbose output
	    cout << " RsuuX[" << j << "][" << k << "][" << i << "] = "
		 << scientific << setw(10) << RsuuX[j][k][i] << endl;
	  }
	}
      }
      
    }
    
  }
  
  // Construct ~chi+ couplings 
  for (int i=1;i<=nChar;i++) {
    
    // Ui1, Ui2, Vi1, Vi2
    complex ui1,ui2,vi1,vi2;    
    ui1=complex( slhaPtr->umix(i,1), slhaPtr->imumix(i,1) );
    ui2=complex( slhaPtr->umix(i,2), slhaPtr->imumix(i,2) );
    vi1=complex( slhaPtr->vmix(i,1), slhaPtr->imvmix(i,1) );
    vi2=complex( slhaPtr->vmix(i,2), slhaPtr->imvmix(i,2) );
    
    // ~chi+ [i] ~chi- [j] Z : loop over [j]
    for (int j=1; j<=nChar; j++) {
      
      // Chargino mixing
      complex uj1, uj2, vj1, vj2;      
      uj1=complex( slhaPtr->umix(j,1), slhaPtr->imumix(j,1) );
      uj2=complex( slhaPtr->umix(j,2), slhaPtr->imumix(j,2) );
      vj1=complex( slhaPtr->vmix(j,1), slhaPtr->imvmix(j,1) );
      vj2=complex( slhaPtr->vmix(j,2), slhaPtr->imvmix(j,2) );
      
      // ~chi+ [i] ~chi- [j] Z : couplings
      OLp[i][j] = -vi1*conj(vj1) - 0.5*vi2*conj(vj2) 
	+ ( (i == j) ? sin2W : 0.0);
      ORp[i][j] = -conj(ui1)*uj1 - 0.5*conj(ui2)*uj2 
	+ ( (i == j) ? sin2W : 0.0);
      
      if (DEBUG) {
	// tmp: verbose output
	cout << " OL'   [" << i << "][" << j << "] = " 
             << scientific << setw(10) << OLp[i][j]
	     << " OR'   [" << i << "][" << j << "] = " 
             << scientific << setw(10) << ORp[i][j] << endl;
      }
    }
    
    // Loop over quark [l] flavour
    for (int l=1;l<=3;l++) {
      
      // Set quark [l] masses 
      // Initial guess 0,0,0,mc,mb,mt with the latter from the PDT
      double mul = particleDataPtr->m0(2*l);
      double mdl = particleDataPtr->m0(2*l-1);
      if (l == 1) { mul=0.0 ; mdl=0.0; }
      if (l == 2) { mdl=0.0 ; mul=0.0; } 
      
      // Compute running mass from Yukawas and vevs if possible.
      if (slhaPtr->yd.exists() && slhaPtr->hmix.exists(3)) {
	double yll=slhaPtr->yd(l,l);
	double v1=slhaPtr->hmix(3)/sqrt(1+pow(tanb,2));
	if (yll > 0.0) mdl = yll * v1 / sqrt(2.0) ;
      }
      if (slhaPtr->yu.exists() && slhaPtr->hmix.exists(3)) {
	double yll=slhaPtr->yu(l,l);
	double v2=slhaPtr->hmix(3)/sqrt(1.0+1.0/pow(tanb,2));
	if (yll > 0.0) mul = yll * v2 / sqrt(2.0) ;
      }
      
      // Loop over squark [j] flavour
      for (int j=1;j<=6;j++) {

	// Loop over off-diagonal quark [k] generation
	for (int k=1;k<=3;k++) {

	  // Set quark [k] masses 
	  // Initial guess 0,0,0,0,mb,mt with the latter from the PDT
	  double muk = particleDataPtr->m0(2*k);
	  double mdk = particleDataPtr->m0(2*k-1);
	  if (k == 1) { muk=0.0 ; mdk=0.0; }
	  if (k == 2) { mdk=0.0 ; muk=0.0; } 
      
	  // Compute running mass from Yukawas and vevs if possible.
	  if (slhaPtr->yd.exists() && slhaPtr->hmix.exists(3)) {
	    double ykk=slhaPtr->yd(k,k);
	    double v1=slhaPtr->hmix(3)/sqrt(1+pow(tanb,2));
	    if (ykk > 0.0) mdk = ykk * v1 / sqrt(2.0) ;
	  }
	  if (slhaPtr->yu.exists() && slhaPtr->hmix.exists(3)) {
	    double ykk=slhaPtr->yu(k,k);
	    double v2=slhaPtr->hmix(3)/sqrt(1.0+1.0/pow(tanb,2));
	    if (ykk > 0.0) muk = ykk * v2 / sqrt(2.0) ;
	  }	  

	  // CKM matrix (use Pythia one if no SLHA)
	  // (NB: could also try input one if no running one found, but
	  // would then need to compute from Wolfenstein)
	  complex Vlk=coupSMPtr->VCKMgen(l,k);
	  complex Vkl=coupSMPtr->VCKMgen(k,l);
	  if (slhaPtr->vckm.exists()) {
	    Vlk=complex(slhaPtr->vckm(l,k),slhaPtr->imvckm(l,k));
	    Vkl=complex(slhaPtr->vckm(k,l),slhaPtr->imvckm(k,l));
	  }
      
	  // Squark mixing
	  complex Rdjk  = complex(Rd(j,k),  imRd(j,k)  );
	  complex Rdjk3 = complex(Rd(j,k+3),imRd(j,k+3));
	  complex Rujk  = complex(Ru(j,k),  imRu(j,k)  );
	  complex Rujk3 = complex(Ru(j,k+3),imRu(j,k+3));

	  // sqrt(2)
	  double rt2 = sqrt(2.0);
	  
	  // ~d[j] u[l] ~chi+[i]
	  LsduX[j][l][i] += 0.5*(ui1*conj(Rdjk) 
				 - mdk*ui2*conj(Rdjk3)/rt2/mW/cosb)*Vlk;
	  RsduX[j][l][i] -= 0.5*mul*conj(vi2)*Vlk*Rdjk/rt2/mW/sinb; 

	  // ~u[j] d[l] ~chi+[i]
	  LsudX[j][l][i] += 0.5*(vi1*conj(Rujk)
				 - muk*vi2*conj(Rujk3)/rt2/mW/sinb)*conj(Vkl);
	  RsudX[j][l][i] -= 0.5*mdl*conj(ui2)*conj(Vkl)*conj(Rujk)/rt2/mW/cosb;

	}

	if (DEBUG) {
	  if (max(abs(LsduX[j][l][i]),abs(RsduX[j][l][i])) > 1e-6) {
	    // tmp: verbose output
	    cout << " LsduX[" << j << "][" << l << "][" << i << "] = "
		 << scientific << setw(10) << LsduX[j][l][i];
	    cout << " RsduX[" << j << "][" << l << "][" << i << "] = "
		 << scientific << setw(10) << RsduX[j][l][i] << endl;
	  }
	  if (max(abs(LsudX[j][l][i]),abs(RsudX[j][l][i])) > 1e-6) {
	    // tmp: verbose output
	    cout << " LsudX[" << j << "][" << l << "][" << i << "] = "
		 << scientific << setw(10) << LsudX[j][l][i];
	    cout << " RsudX[" << j << "][" << l << "][" << i << "] = "
		 << scientific << setw(10) << RsudX[j][l][i] << endl;
	  }
	}
	
      }
      
      
    }
    
  }

  // SLHA2 compatibility
  // Check whether scalar particle masses are ordered
  bool orderedQ = true;
  bool orderedL = true;
  for (int j=1;j<=5;j++) {
    if (particleDataPtr->m0(idSlep(j+1)) < particleDataPtr->m0(idSlep(j))) 
      orderedL  = false;
    if (particleDataPtr->m0(idSup(j+1)) < particleDataPtr->m0(idSup(j)))
      orderedQ  = false;
    if (particleDataPtr->m0(idSdown(j+1)) <particleDataPtr->m0(idSdown(j)))
      orderedQ  = false;
  }

  // If ordered, change sparticle labels to mass-ordered enumeration
  for (int i=1;i<=6;i++) {
    ostringstream indx;
    indx << i;
    string uName = "~u_"+indx.str();
    string dName = "~d_"+indx.str();
    string lName = "~e_"+indx.str();
    ParticleDataEntry* pdePtr;
    if (orderedQ) {
      pdePtr = particleDataPtr->particleDataEntryPtr(idSup(i));
      pdePtr->setNames(uName,uName+"bar");
      pdePtr = particleDataPtr->particleDataEntryPtr(idSdown(i));
      pdePtr->setNames(dName,dName+"bar");
    }
    if (orderedL) {
      pdePtr = particleDataPtr->particleDataEntryPtr(idSlep(i));
      pdePtr->setNames(lName,lName+"bar");
    }
  }
			
  // Let everyone know we are ready
  isInit = true;
  
}

//--------------------------------------------------------------------------

// Return neutralino flavour codes.

int CoupSUSY::idNeut(int idChi) {
  int id = 0;
  if      (idChi == 1) id = 1000022; 
  else if (idChi == 2) id = 1000023; 
  else if (idChi == 3) id = 1000025; 
  else if (idChi == 4) id = 1000035; 
  else if (idChi == 5) id = 1000045; 
  return id;
}

//--------------------------------------------------------------------------

// Return chargino flavour codes.

int CoupSUSY::idChar(int idChi) {
  int id = 0;
  if      (idChi ==  1) id =  1000024; 
  else if (idChi == -1) id = -1000024;     
  else if (idChi ==  2) id =  1000037; 
  else if (idChi == -2) id = -1000037; 
  return id;
}

//--------------------------------------------------------------------------

// Return sup flavour codes.

int CoupSUSY::idSup(int iSup) {
  int id = 0;
  int sgn = ( iSup > 0 ) ? 1 : -1;
  iSup = abs(iSup);
  if      (iSup ==  1) id =  1000002; 
  else if (iSup ==  2) id =  1000004;     
  else if (iSup ==  3) id =  1000006; 
  else if (iSup ==  4) id =  2000002; 
  else if (iSup ==  5) id =  2000004;     
  else if (iSup ==  6) id =  2000006; 
  return sgn*id;
}

//--------------------------------------------------------------------------

// Return sdown flavour codes

int CoupSUSY::idSdown(int iSdown) {
  int id = 0;
  int sgn = ( iSdown > 0 ) ? 1 : -1;
  iSdown = abs(iSdown);
  if      (iSdown ==  1) id =  1000001; 
  else if (iSdown ==  2) id =  1000003;     
  else if (iSdown ==  3) id =  1000005; 
  else if (iSdown ==  4) id =  2000001; 
  else if (iSdown ==  5) id =  2000003;     
  else if (iSdown ==  6) id =  2000005; 
  return sgn*id;
}

//--------------------------------------------------------------------------

// Function to return slepton flavour codes

int CoupSUSY::idSlep(int iSlep) {
  int id = 0;
  int sgn = ( iSlep > 0 ) ? 1 : -1;
  iSlep = abs(iSlep);
  if      (iSlep ==  1) id =  1000011; 
  else if (iSlep ==  2) id =  1000013;     
  else if (iSlep ==  3) id =  1000015; 
  else if (iSlep ==  4) id =  2000011; 
  else if (iSlep ==  5) id =  2000013;     
  else if (iSlep ==  6) id =  2000015; 
  return sgn*id;
}

//--------------------------------------------------------------------------

// Return a particle name, given the PDG code.

string CoupSUSY::getName(int pdgCode) {    

  // Absolute value and corresponding SM code
  int codeA = abs(pdgCode);
  int idSM  = codeA % 1000000;

  // Name
  string name;

  // Flag to indicate whether SLHA1 or SLHA2
  bool slha1 = false;

  // SM particles
  if (codeA == idSM) {
    // Neutrinos
    if (!slhaPtr->upmns.exists()) slha1=true;
    if (codeA == 12) name = (slha1) ? "nu_e" : "nu_1";
    if (codeA == 14) name = (slha1) ? "nu_mu" : "nu_2";
    if (codeA == 16) name = (slha1) ? "nu_tau" : "nu_3";
  }

  // Squarks
  else if ( idSM <= 6) {
    // up squarks
    if (idSM % 2 == 0) {
      // If SLHA1, return old PDG names
      if (slhaPtr->stopmix.exists()) slha1 = true;
      if (codeA == 1000002) name = (slha1) ? "~u_L" : "~u_1";
      if (codeA == 1000004) name = (slha1) ? "~c_L" : "~u_2";
      if (codeA == 1000006) name = (slha1) ? "~t_1" : "~u_3";
      if (codeA == 2000002) name = (slha1) ? "~u_R" : "~u_4";
      if (codeA == 2000004) name = (slha1) ? "~c_R" : "~u_5";
      if (codeA == 2000006) name = (slha1) ? "~t_2" : "~u_6";
    } 
    // down squarks
    else {
      // If SLHA1, return old PDG names
      if (slhaPtr->sbotmix.exists()) slha1 = true;
      if (codeA == 1000001) name = (slha1) ? "~d_L" : "~d_1";
      if (codeA == 1000003) name = (slha1) ? "~s_L" : "~d_2";
      if (codeA == 1000005) name = (slha1) ? "~b_1" : "~d_3";
      if (codeA == 2000001) name = (slha1) ? "~d_R" : "~d_4";
      if (codeA == 2000003) name = (slha1) ? "~s_R" : "~d_5";
      if (codeA == 2000005) name = (slha1) ? "~b_2" : "~d_6";
    }
    if (pdgCode < 0) name += "bar";
  } 

  // Sleptons
  else if ( idSM <= 19 ) {

    // Sneutrinos
   if (idSM % 2 == 0) {
      // If SLHA1, return old PDG names
      if (slhaPtr->staumix.exists()) slha1 = true;
      if (codeA == 1000012) name = (slha1) ? "~nu_eL" : "~nu_1";
      if (codeA == 1000014) name = (slha1) ? "~nu_muL" : "~nu_2";
      if (codeA == 1000016) name = (slha1) ? "~nu_tauL" : "~nu_3";
      if (codeA == 2000012) name = (slha1) ? "~nu_eR" : "~nu_4";
      if (codeA == 2000014) name = (slha1) ? "~nu_muR" : "~nu_5";
      if (codeA == 2000016) name = (slha1) ? "~nu_tauR" : "~nu_6";
      if (pdgCode < 0) name += "bar";
    }
    // charged sleptons
    else {
      // If SLHA1, return old PDG names
      if (slhaPtr->staumix.exists()) slha1 = true;
      if (codeA == 1000011) name = (slha1) ? "~e_L" : "~e_1";
      if (codeA == 1000013) name = (slha1) ? "~mu_L" : "~e_2";
      if (codeA == 1000015) name = (slha1) ? "~tau_1" : "~e_3";
      if (codeA == 2000011) name = (slha1) ? "~e_R" : "~e_4";
      if (codeA == 2000013) name = (slha1) ? "~mu_R" : "~e_5";
      if (codeA == 2000015) name = (slha1) ? "~tau_2" : "~e_6";
      if (pdgCode < 0) name += "-";
      else name += "+";
    }
  }

  else if (codeA == 1000021) name = "~g";
  else if (codeA == 1000022) name = "~chi_10";  
  else if (codeA == 1000023) name = "~chi_20";
  else if (codeA == 1000024) name = (pdgCode > 0) ? "~chi_1+" : "~chi_1-";
  else if (codeA == 1000025) name = "~chi_30";
  else if (codeA == 1000035) name = "~chi_40";
  else if (codeA == 1000037) name = (pdgCode > 0) ? "~chi_2+" : "~chi_2-";

  return name;

}
  
//==========================================================================

} // end namespace Pythia8


