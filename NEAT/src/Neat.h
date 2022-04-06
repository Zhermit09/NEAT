#include <iostream>
#include <vector>
#include <numeric>
#include <string>
#include <cstdarg>
#define numOfDigits (std::to_string(abs(random)).length())
#define divide (random / (double)(pow(10.0, exp)))
//#define exp 2.71828182845904523536028747135266249775724709369995957496696762772407663035354759457138217852516642742746639193200305992181741359662

#pragma once 
int64_t Random();

#ifdef NEAT_AI_NEURALNETWORK
#define NEAT_AI_NEURALNETWORK

namespace neat {
	//           SETTINGS
	//------------------------------

	//# Network parameters
	const int Hidden_Layers = 1;

	//       # Neurons
	const int Num_Of_Inputs = 6;
	const int Num_Of_Hidden = 1;
	const int Num_Of_Outputs = 1;

	//# Wheight options
	const int Wheight_Range_Value = 30;
	//const int Wheight_Min_Value = -30;

	//# Bias options
	const int Bias_Range_Value = 30;
	//const int Bias_Min_Value = -30;

	//------------------------------

	double RandomDigits(int mod) {

		int64_t random = Random();
		double exp = (double)numOfDigits;
		double decimals = divide;

		return (Random() % mod) + decimals;

	}

	struct Neuron {
		std::vector<double> wheights;
		double bias{};
		long double value{};

		/*Neuron() {
			int64_t random;
			double exp;

			for (int i = 0; i < 3; i++) {
				random = Random();
				exp = (double)digits;
				wheights.push_back(divide);
			}

			random = Random();
			exp = (double)digits;
			bias = random / (double)(pow(10.0, exp - 1));

			value = 0;
		}*/


		double ActFunction(double x) {
			return tanh(x);
		}

		int Value(std::vector<double> m1, std::vector<double> m2) {

			double sum = 0;

			for (int i = 0; i < m1.size(); i++) {
				sum += m1[i] * m2[i];
			}
			value = ActFunction(sum + bias);
			return 0;
		}
	};

	struct Layer {

		std::vector<Neuron*> neurons;

		Layer(int count) {
			for (int i = 0; i < count; i++) {
				neurons.push_back(new Neuron());
			}
		}

		int InputLayer() {

			for (int i = 0; i < neurons.size(); i++) {
				neurons[i]->wheights.push_back(1);
			}

			return 0;
		}

		int ConnectLayer(int size) {

			for (int i = 0; i < neurons.size(); i++) {

				for (int j = 0; j < size; j++) {
					neurons[i]->wheights.push_back(RandomDigits(Wheight_Range_Value));
				}
				neurons[i]->bias = RandomDigits(Bias_Range_Value);	
			}

			return 0;
		}
	};

	struct Network {

		std::vector<Layer*> layers;

		Network() {
			CreateNeurons();
			FirstConnect();
		}

		int CreateNeurons() {

			layers.push_back(new Layer(Num_Of_Inputs));

			for (int i = 0; i < Hidden_Layers; i++) {
				layers.push_back(new Layer(Num_Of_Hidden));
			}

			layers.push_back(new Layer(Num_Of_Outputs));

			return 0;
		}

		int FirstConnect() {

			layers[0]->InputLayer();

			for (int i = 1; i < layers.size(); i++) {

				layers[i]->ConnectLayer(layers[i - 1]->neurons.size());
			}
			return 0;
		}

		static int ActivateNetwork(std::vector<double> inputs) {
			try {
				if (inputs.size() != Num_Of_Inputs) {
					throw std::runtime_error("ERROR: Wrong amout of inputs, ");
				}
			}
			catch (std::runtime_error e) {
				std::cout << e.what() << "Expected " << Num_Of_Inputs << " inputs, got " << inputs.size() << "\n";
				exit(1);

			}

			for (int i = 0; i < Num_Of_Inputs; i++)
			{
				std::cout << inputs[i] << "\n";
			}

			return 0;
		}

		/*static int ActivateNetwork(int count, ...) {
			va_list args;
			va_start(args, count);

			for (int i = 0; i < Num_Of_Inputs; i++)
			{
				std::cout << va_arg(args, double) << "\n";
			}

			va_end(args);
			return 0;
		}*/

	};
}
#endif