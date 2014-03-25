/*
 *  defines.h
 *  asrTracer
 *
 *  Created by Petrik Clarberg on 2006-03-10.
 *  Copyright 2006 Lund University. All rights reserved.
 *
 */

#ifndef DEFINES_H
#define DEFINES_H

#include <stdexcept>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <limits>
#include <cmath>
#include "matrix.h"
#include "color.h"
#include "diffuse.h"

#ifndef M_PI
/// Define pi.
static const float M_PI = 3.14159265358979323846f;
#endif

/// Define positive infinity for floats.
static const float INF = std::numeric_limits<float>::infinity();	

/// Small value.
static const float epsilon = 1e-6f;

/// Default material is a gray diffuse material.
static Diffuse DEFAULT_MATERIAL = Diffuse(Color(0.7f,0.7f,0.7f));


/// Returns a uniform random number in the range [0,1).
static float uniform() { return (float)std::rand()/(1.f+RAND_MAX); }

/// Helper function for converting an int to a string.
static std::string int2str(int i)
{
	std::ostringstream oss;
	oss << i;
	return oss.str();
}

/// Swaps two floats.
static void swap(float& a, float& b) { float t=a; a=b; b=t; }

/// Round value to nearest integer.
#define ROUND(x) (std::floor((x)+0.5))

/// Returns the maximum of the two elements.
template<class T> T max(T a, T b) { return a>b ? a : b; }

/// Returns the minimum of the two elements.
template<class T> T min(T a, T b) { return a<b ? a : b; }

#endif
