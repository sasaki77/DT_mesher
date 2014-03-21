#ifndef PREPROCESS_H
#define PREPROCESS_H

#ifdef MAIN
#define EXTERN
#else
#define EXTERN extern
#endif

#define _USE_MATH_DEFINES // for C++,cmath
#define CALC_EPS (1e-15)
#define OUT_EPS (1e-13)

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <vector>
#include <stack>
#include <list>
#include <iomanip>
#include <time.h>
#include <stdlib.h>
#include <float.h>

#define DEBUG 0
#define EQDBL(x,y) ( (fabs((double)(x))-fabs((double)(y))) < OUT_EPS )

using namespace std;

#include "class.h"

EXTERN std::ostream& operator<<(std::ostream& os,/*const*/ Node& obj);
EXTERN std::ostream& operator<<(std::ostream& os,/*const*/ Element& obj);
EXTERN std::ostream& operator<<(std::ostream& os,/*const*/ Edge& obj);

#endif PREPROCESS_H
