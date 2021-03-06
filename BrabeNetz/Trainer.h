#pragma once
#include "Network.h"

// Use a constant learn rate (LEARN_RATE) for training instead of small decreasing one
#define CONST_LEARN_RATE true
// Print the input, expected and actual output to console (that's hella slow!)
#define PRINT_OUTPUT true

/// <summary>
/// A Neuronal Network Trainer supporting bools
/// </summary>
class trainer
{
public:
	// Teach the network XOR switches
	static void train_xor(network& net, int train_times);
	// Train the network to recognize handwritten digits
	static void train_handwritten_digits(network& net, int train_times, string mnist_images, string mnist_labels);
};
