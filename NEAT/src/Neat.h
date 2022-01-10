#include <iostream>
#include <vector>
#include <numeric>
//#define exp 2.71828182845904523536028747135266249775724709369995957496696762772407663035354759457138217852516642742746639193200305992181741359662
#ifdef NEAT_AI_NEURALNETWORK

namespace neat {

	struct Neuron {

	private:
		std::vector<Neuron*> links{};
		std::vector<double> wheights{};
		double bias{};

	protected:
		long double result{};

		long double Sigmoid(long double x) {
			return 1.0 / (1.0 + exp(-x));
		}

	private:
		double SumOfAll(std::vector<double> m1, std::vector<double> m2) {
			double sum = 0;

			for (int i = 0;i < m1.size(); i++) {
				sum += m1[i] * m2[i];
			}
			result = Sigmoid(sum + bias);
		}
	};

	struct InNeuron : Neuron {

		InNeuron(long double input) {
			result = Sigmoid(input);
		}
	public:
		long double get() {
			return result;
		}
	};

	struct OutNeuron : Neuron {

	};
}
#endif