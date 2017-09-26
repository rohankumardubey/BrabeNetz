#include "stdafx.h"
#include "Network.h"
using namespace std;


// ctor
Network::Network(initializer_list<int> initializerList)
{
	Init(&initializerList);
	RandomizeWeights(); // Calculate weights
}

Network::Network(initializer_list<int> initializerList, NetworkTopology* topology)
{
	Init(&initializerList);
	FillWeights(topology); // Calculate weights
}

// dector
Network::~Network()
{
	// cleanup
	delete this->hiddenNeuronsCount;
	delete this->layers;
	delete this->connectionWeights;
}

// Train network and adjust weights to expectedOutput
double Network::Train(double* inputValues, int length, double expectedOutput)
{
	double output = Feed(inputValues, length);

	if (output == expectedOutput)
		return 0.0; // it's trained good enough

	//TODO: Adjust network
	return output;
}

// Feed the network information and return the output
double Network::Feed(double* inputValues, int length)
{
	int lindex = length - 1; // Last index of inputValues (and eff. weights)

	double* values = inputValues; // Values of current layer
	int* valuesLength = &length;
	// Go through each hidden layer
	for (int hiddenIndex = 0; hiddenIndex < this->hiddenLayersCount; hiddenIndex++)
	{
		double* addr = values; // Cache temp variable
		values = ToNextLayer(values, *valuesLength, hiddenIndex, *valuesLength);

		if (addr != inputValues) // Don't delete parameter
			delete addr; // Delete the old pointer
	}

	// TODO: Calculate correct output layer (Squashing, ReLU, ..)
	double sum = 0;
	for (int i = 0; i < length; i++) // Loop through each neuron in output layer
	{
		double value = Rectify(values[i]); // ReLU it (keep if positive, 0 if negative; uint)
		sum += Squash(value); // Squash the result
	}

	// Cleanup
	return sum;
}

// This function focuses on only one layer, so in theory we have 1 input layer, the layer we focus on, and 1 output
double* Network::ToNextLayer(double* inputValues, int inputLength, int layerIndex, int& outLength)
{
	int nCount = this->hiddenNeuronsCount[layerIndex]; // Count of neurons in the given layer (w/ layerIndex)
	double* layer = this->layers[layerIndex]; // ptr to neurons in this layer
	double** weights = this->connectionWeights[layerIndex]; // ptr to weights of neurons in this layer
	double* output = new double[nCount];

	// Loop through each neuron on the given layer
	for (int n = 0; n < nCount; n++) // n = neuron
	{
		layer[n] = 0; // Reset layer's neuron at index n

		// Loop through each value in the inputs (every input broadcasts to all neurons in this layer)
		for (int ii = 0; ii < inputLength; ii++) // ii = input index
		{
			layer[n] += inputValues[ii] * weights[layerIndex][n]; // Add Value * Weight to that neuron
		}

		double neuron = layer[n]; // The current neuron's value
		neuron = Squash(neuron);
		output[n] = neuron; // Add value to output layer
	}

	outLength = nCount;
	return output;
}

void Network::RandomizeWeights()
{
	this->topology = new NetworkTopology();
	this->topology->AddLayer(new Layer()); // Add first layer (input layer)

	// Fill input layer -> first hidden layer
	for (int n = 0; n < inputNeuronsCount; n++) // Loop through each neuron
	{
		Neuron* neuron = new Neuron();
		int nextn = this->hiddenNeuronsCount[0]; // Count of neurons in first hidden layer

		for (int c = 0; c < nextn; c++)
		{
			Connection* connection = new Connection(); // Build up connection with random weight
			connection->Weight = (double(rand() % 200) / 100) - 1; // Random number between 0 and 2, minus 1 (so between -1 and 1)
			neuron->AddConnection(connection); // add connection
		}

		this->topology->Layers->at(0)->AddNeuron(neuron); // Add the neuron with connections to first hidden layer
	}
	
	// Fill all hidden layers
	for (int l = 0; l < hiddenLayersCount; l++) // Loop through each layer
	{
		Layer* layer = new Layer();
		
		for (int n = 0; n < hiddenNeuronsCount[l]; n++) // Loop through each neuron
		{
			Neuron* neuron = new Neuron();

			int nextNeurons = 0;
			int next = l + 1;
			if (next == hiddenLayersCount) // Check if out of bounds
				nextNeurons = outputNeuronsCount; // It's last layer; Use output layer
			else
				nextNeurons = hiddenNeuronsCount[next]; // Use next layer

			for (int c = 0; c < nextNeurons; c++) // Loop through each Connection
			{
				Connection* connection = new Connection();
				/*connection->From = neuron;
				connection->To = neuron;*/
				connection->Weight = (double(rand() % 200) / 100) - 1; // Random number between 0 and 2, minus 1 (so between -1 and 1)

				neuron->AddConnection(connection); // Add Connection from neuron `n`
			}

			layer->AddNeuron(neuron); // Add Neuron from layer `l`
		}

		topology->AddLayer(layer); // Add Layer
	}

	FillWeights(this->topology);
}

void Network::FillWeights(NetworkTopology* topology)
{
	this->topology = topology;
	// TODO: Check if this works

	// layer weights has a reference on the heap
	if (this->connectionWeights != nullptr)
	{
		// delete the reference
		delete this->connectionWeights;
	}

	int count = this->hiddenLayersCount + 1; // Count of layers with connections
	this->connectionWeights = new double**[count]; // init first dimension; count of layers with connections

												   // Fill input layer weights
	this->connectionWeights[0] = new double*[this->inputNeuronsCount]; // [0] is input layer
	int nextLayerNeuronCount = this->hiddenNeuronsCount[0]; // Count of neurons in first hidden layer
	for (int i = 0; i < this->inputNeuronsCount; i++) // Loop through each neuron in input layer
	{
		this->connectionWeights[0][i] = new double[nextLayerNeuronCount];

		for (int ni = 0; ni < nextLayerNeuronCount; ni++) // Loop through each connection on that neuron
		{
			// Set all layer weights on this layer to the number on our network topology
			this->connectionWeights[0][i][ni] = topology->Layers->at(0)->Neurons->at(i)->Connections->at(ni)->Weight;
		}
	}


	// Fill hidden layers weights
	for (int i = 0; i < this->hiddenLayersCount; i++)
	{
		int neuronsCount = this->hiddenNeuronsCount[i]; // Count of neurons on that layer
		int next = i + 1; // effectively next layer, connections are between layers
		int nextNeuronsCount;

		if (next < this->hiddenLayersCount) // Check if we're on the last layer; if yes -> last to output connections
			nextNeuronsCount = this->hiddenNeuronsCount[next]; // Count of neurons in next layer
		else
			nextNeuronsCount = this->outputNeuronsCount; // Count of neurons in next layer (output)

		this->connectionWeights[next] = new double*[neuronsCount]; // Init this layer's neurons []
		for (int ni = 0; ni < neuronsCount; ni++) // Loop through each neuron on this layer
		{
			this->connectionWeights[next][ni] = new double[neuronsCount]; // Init this neuron's connections []
			for (int nni = 0; nni < nextNeuronsCount; nni++) // Loop through each connection on this neuron
			{
				// Set all layer weights on this layer to the number on our network topology
				this->connectionWeights[next][ni][nni] = topology->Layers->at(next)->Neurons->at(ni)->Connections->at(nni)->Weight;
			}
		}
	}
}

void Network::Save(string path)
{
	// TODO
}

void Network::Load(string path)
{
	// TODO
}


void Network::Init(initializer_list<int>* initializerList)
{
	if (initializerList->size() < 3)
		throw "Initializer List can't contain less than 3 elements. E.g: { 2, 3, 4, 1 }: 2 Input, 3 Hidden, 4 Hidden, 1 Output";

	vector<int> inputVector; // clone initializer list to vector
	inputVector.insert(inputVector.end(), initializerList->begin(), initializerList->end());

	this->inputNeuronsCount = inputVector[0]; // First element in vector -> input
	this->outputNeuronsCount = inputVector.back(); // Last element in vector -> output
	this->hiddenLayersCount = inputVector.size() - 2; // Count of hidden layers = total items in vector minus end and start
	this->hiddenNeuronsCount = new int[hiddenLayersCount]; // elements except first and last = hidden layers
	this->layers = new double*[hiddenLayersCount]; // Init all hidden layers (between input & output)

	int hiddenIndex = 1; // index on input vector
	for (int i = 0; hiddenIndex <= hiddenLayersCount; i++) // Loop from [1] to [last-1] (all hidden layers)
	{
		int layerSize = inputVector[hiddenIndex]; // Layer size of this layer (Containing neurons)
		this->hiddenNeuronsCount[i] = layerSize; // Set neuron count on this hidden layer
		this->layers[i] = new double[layerSize]; // Create layer with neuron size in hidden-layers array

		hiddenIndex++;
	}
}