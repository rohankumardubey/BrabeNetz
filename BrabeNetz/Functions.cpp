#include "stdafx.h"
#include "Functions.h"
#include <cmath>
#include <vector>
using namespace std;

// The Euler's constant (e) calculated with exp(1)
const DECIMAL euler_constant = exp(1.0);
// Minimum value that's possible as a Squash(..) input parameter
const DECIMAL min_value = 0;
// Maximum value that's possible as a Squash(..) input parameter
const DECIMAL max_value = 1;

/// <summary>
/// Squash a value with the Sigmoid
/// function (https://en.wikipedia.org/wiki/Sigmoid_function) using
/// doubles or floats (depending on compile-architecture)
/// </summary>
DECIMAL squash(const DECIMAL value)
{
	return 1 / (1 + exp(-value));
}

/// <summary>
/// Rectify a value with the rectified linear unit (ReLU)
/// function (https://en.wikipedia.org/wiki/Rectifier_(neural_networks)) using
/// doubles or floats (depending on compile-architecture)
/// </summary>
DECIMAL rectify(const DECIMAL value)
{
	return fmax(value, 0);
}

DECIMAL get_error(const DECIMAL expected, const DECIMAL actual)
{
	return actual * (1 - actual) * (expected - actual);
}

DECIMAL get_error(const DECIMAL output_error, const DECIMAL neuron_value, const DECIMAL total_weights)
{
	return neuron_value * (1 - neuron_value) * total_weights;
}

/// <summary>
/// Rectify a value with the rectified linear unit (ReLU)
/// function (https://en.wikipedia.org/wiki/Rectifier_(neural_networks)) using
/// doubles (because doubles are being Hardware-implemented -> faster)
/// </summary>
DECIMAL sum(vector<DECIMAL>* values)
{
	DECIMAL sum = 0;
	for (int i = 0; i < values->size(); i++)
	{
		sum += values->at(i);
	}
	return sum;
}


// LOGISTIC:
//   float exp = std::pow(EulerConstant, value);
//   return exp / (exp + 1);

// HYPERBOLIC TAN:
//   float expP = std::pow(EulerConstant, value);
//   float expN = std::pow(EulerConstant, -value);
//   return (expP - expN) / (expP + expN);

// SPECIFIC ALGEBRAIC
//   return (value) / std::sqrt(1 + std::pow(value, 2));

// BRAH
// ((value - min_A) / max_A - min_A) * (new_max_A - new_min_A) + new_min_A