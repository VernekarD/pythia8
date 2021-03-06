// PythiaStdlib.h is a part of the PYTHIA event generator.
// Copyright (C) 2010 Torbjorn Sjostrand.
// PYTHIA is licenced under the GNU GPL version 2, see COPYING for details.
// Please respect the MCnet Guidelines, see GUIDELINES for details.

// Header file for functionality pulled in from Stdlib,
// plus a few useful utilities (small powers; positive square root,
// Gamma function).

#ifndef Pythia8_PythiaStdlib_H
#define Pythia8_PythiaStdlib_H

// Stdlib header files for mathematics.
#include <cmath>
#include <cstdlib>

// Stdlib header files for strings and containers.
#include <string>
#include <vector>
#include <map>
#include <deque>

// Stdlib header file for input and output.
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>

// Define pi if not yet done.
#ifndef M_PI
#define M_PI 3.1415926535897932385
#endif

// Alternatively you can specify exactly which std:: methods will be used.

namespace Pythia8 {
// Generic utilities and mathematical functions.
using std::swap;
using std::max;
using std::min; 
using std::abs; 
// Strings and containers.
using std::string; 
using std::vector; 
using std::map; 
using std::deque; 
using std::pair;
// Input/output streams.
using std::cin; 
using std::cout; 
using std::cerr; 
using std::istream; 
using std::ostream; 
using std::ifstream; 
using std::ofstream; 
using std::istringstream; 
using std::ostringstream; 
using std::fstream;
// Input/output formatting.
using std::endl; 
using std::fixed; 
using std::scientific; 
using std::left; 
using std::right; 
using std::setw; 
using std::setprecision; 
using std::ios;
} // end namespace Pythia8


namespace Pythia8 {

// Powers of small integers - for balance speed/code clarity.
inline double pow2(const double& x) {return std::pow(x, 2);}
inline double pow3(const double& x) {return std::pow(x, 3);}
inline double pow4(const double& x) {return std::pow(x, 4);}
inline double pow5(const double& x) {return std::pow(x, 5);}
inline double pow6(const double& x) {return std::pow(x, 6);}

// Avoid problem with negative square root argument (from roundoff).
inline double sqrtpos(const double& x) {return sqrt( max( 0., x));}

} // end namespace Pythia8

#endif // Pythia8_PythiaStdlib_H
