#include <iostream>
#include <vector>
#include <numeric>
#include <string>
#define digits (std::to_string(abs(random)).length())
#define divide (random / (double)(pow(10.0, exp)))
//#define exp 2.71828182845904523536028747135266249775724709369995957496696762772407663035354759457138217852516642742746639193200305992181741359662

#pragma once 
int64_t Random();

#ifdef NEAT_AI_NEURALNETWORK
#define NEAT_AI_NEURALNETWORK

namespace neat {

	struct Neuron {
		std::vector<double> wheights;
		double bias{};
		double value{};

		Neuron() {
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
		}


		double ActFunction(double x) {
			double n = tanh(x);

			//std::string s = std::to_string(n);

			return n;
		}

		/*double SumOfAll(std::vector<double> m1, std::vector<double> m2) {
			double sum = 0;

			for (int i = 0;i < m1.size(); i++) {
				sum += m1[i] * m2[i];
			}
			value = Sigmoid(sum + bias);
		}*/
	};

	struct Layer {

		std::vector<Neuron*> neurons;
		Layer() {
			for (int i = 0; i < 3; i++) {
				neurons.push_back(new Neuron());
			}
		}
	};

	struct Network {

		std::vector<Layer*> layers;
		Network() {
			for (int i = 0; i < 3; i++) {
				layers.push_back(new Layer());
			}
		}
	};
}
#endif