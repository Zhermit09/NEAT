#include <iostream>
#include <vector>
//#include <numeric>
//#include <string>
//#include <cstdarg>

#pragma once 
int64_t Random();

#ifdef NEAT_AI_NEURALNETWORK
//#define NEAT_AI_NEURALNETWORK

namespace neat {

	//----------SETTINGS----------

	//# Network parameters
	const int Num_Of_Inputs = 3;
	const int Num_Of_Hidden = 1;       //Good to start off with one hidden if output is more than one
	const int Num_Of_Outputs = 1;

	const int Population_Size = 15;

	//# Wheight options
	const int Wheight_Range_Value = 30; //20?

	//# Bias options
	const int Bias_Range_Value = 30;

	//const int Hidden_Layers = 1;
	//------------------------------


	double RandomDigits(int mod) {

		int64_t random = Random();
		double decimals = random / ((double)(pow(10, 19)));

		return (Random() % (mod)) + decimals;

	}

	/*
	struct Neuron {
		std::vector<double> wheights;
		double bias{};
		double value{};

		double ActFunction(double x) {
			return tanh(x);
		}

		int MakeValue(std::vector<double> inputs) {

			double sum = 0;

			for (int i = 0; i < inputs.size(); i++) {
				sum += inputs[i] * wheights[i];
				//std::cout<<inputs[i] * wheights[i]<<"       ";
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


		int ConnectInputLayer() {

			for (int i = 0; i < neurons.size(); i++) {
				neurons[i]->wheights.push_back(1);
			}

			return 0;
		}


		int CalculateInputValues(std::vector<double> inputs) {

			for (int i = 0; i < neurons.size(); i++) {

				neurons[i]->MakeValue({ inputs[i] });
				//std::cout << "\n";
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


		int CalculateValues(std::vector<double> inputs) {

			for (int i = 0; i < neurons.size(); i++) {
				neurons[i]->MakeValue(inputs);
				//std::cout << "\n";
			}

			return 0;
		}


		std::vector<double> GetValues() {

			std::vector<double> values;

			for (int i = 0; i < neurons.size(); i++) {
				values.push_back(neurons[i]->value);
			}
			return values;
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

			layers[0]->ConnectInputLayer();

			for (int i = 1; i < layers.size(); i++) {

				layers[i]->ConnectLayer((int)layers[i - 1]->neurons.size());
			}
			return 0;
		}

		std::vector<double> ActivateNetwork(std::vector<double> inputs) {
			try {
				if (inputs.size() != Num_Of_Inputs) {
					throw std::runtime_error("ERROR: Wrong amout of inputs for network");
				}
			}
			catch (std::runtime_error e) {
				std::cout << e.what() << ", Expected " << Num_Of_Inputs << " inputs, got " << inputs.size() << "\n";
				exit(1);

			}

			layers[0]->CalculateInputValues(inputs);
			//std::cout << "\n";

			for (int i = 1; i < layers.size(); i++)
			{
				layers[i]->CalculateValues(layers[i - 1]->GetValues());
				//std::cout << "\n";
			}

			auto neurons = layers.back()->neurons;
			std::vector<double> result;
			for (int i = 0; i < neurons.size(); i++)
			{
				result.push_back(neurons[i]->value);
			}

			return result;
		}


	};
	*/


	//----------------------------------------------------------------------------------------------------------

	enum class Node_Type
	{
		Input, Output, Hidden
	};


	struct Node_Gene {

		int node_ID{};
		Node_Type type{};
		int layer{};
		double bias{};//?

	};


	struct Link_Gene
	{
		int from_node_ID{};
		int to_node_ID{};

		double wheight{};
		bool enabled = true;
		//bool recurrent{};

		int unique_ID{};

	};


	struct Link_List {

		std::vector<Link_Gene> list{};

		int size() {
			return (int)list.size();
		}


		Link_Gene getInnovN(Link_Gene obj) {

			for (int i = 0; i < list.size(); i++)
			{
				if ((obj.from_node_ID == list[i].from_node_ID) and (obj.to_node_ID == list[i].to_node_ID)) {
					return  list[i];
				}
			}

			Add(obj);
			return list[size() - 1];
		}

		int Clear() {
			list.clear();
			return 0;
		}

		int Add(Link_Gene obj) {

			list.push_back(obj);
			list[size() - 1].unique_ID = size();

			return 0;
		}

		/*bool contains(Link_Gene obj) {

			for (int i = 0; i < list.size(); i++)
			{
				if ((obj.from_node == list[i].from_node) and (obj.to_node == list[i].to_node)) {
					return true;
				}
			}

			return false;
		}

		int Remove(int i) {
			if (i >= size() || i < 0) {
				return 1;
			}
			list.erase(list.begin() + i);
			return 0;
		}

		int Remove(Link_Gene obj) {

			//auto discard = remove(list.begin(), list.end(), obj);
			list.pop_back();
			return 0;
		}*/

	};


	struct Genome
	{

		std::vector<Node_Gene> node_genes{};
		std::vector<Link_Gene> link_genes{};
		//Problem with muliple of same ^connections

		Link_List* link_list{};

		Genome(Link_List* list) {
			link_list = list;
			Init();
		}

		int Init() {
			NodeInit();
			LinkInit();

			return 0;
		}

		int NodeInit() {

			for (int i = 0; i < Num_Of_Inputs; i++)
			{
				node_genes.push_back(Node_Gene(i + 1, Node_Type::Input, 1, 0));//RandomDigits(Bias_Range_Value)));				
			}
			for (int i = Num_Of_Inputs; i < Num_Of_Inputs + Num_Of_Outputs; i++)
			{
				node_genes.push_back(Node_Gene(i + 1, Node_Type::Output, (Num_Of_Hidden > 0) + 2, 0));//RandomDigits(Bias_Range_Value)));
			}
			for (int i = Num_Of_Inputs + Num_Of_Outputs; i < Num_Of_Inputs + Num_Of_Outputs + Num_Of_Hidden; i++)
			{
				node_genes.push_back(Node_Gene(i + 1, Node_Type::Hidden, 2 , 0));//RandomDigits(Bias_Range_Value)));
			}
			return 0;
		}

		int LinkInit() {

			if (Num_Of_Hidden <= 0) {
				for (Node_Gene node : node_genes)
				{
					if (node.type == Node_Type::Output) {
						for (int i = 0; i < Num_Of_Inputs; i++)
						{
							link_genes.push_back(link_list->getInnovN(Link_Gene(node_genes[i].node_ID, node.node_ID)));
							link_genes.back().wheight = RandomDigits(Wheight_Range_Value);
						}
					}
				}
			}
			else {

				for (Node_Gene node : node_genes)
				{
					if (node.type == Node_Type::Hidden) {
						for (int i = 0; i < Num_Of_Inputs; i++)
						{
							link_genes.push_back(link_list->getInnovN(Link_Gene(node_genes[i].node_ID, node.node_ID)));
							link_genes.back().wheight = RandomDigits(Wheight_Range_Value);
						}
					}
				}
				for (Node_Gene node : node_genes)
				{
					if (node.type == Node_Type::Output) {
						for (int i = Num_Of_Inputs + Num_Of_Outputs; i < Num_Of_Inputs + Num_Of_Outputs + Num_Of_Hidden; i++)
						{
							link_genes.push_back(link_list->getInnovN(Link_Gene(node_genes[i].node_ID, node.node_ID)));
							link_genes.back().wheight = RandomDigits(Wheight_Range_Value);
						}
					}
				}
			}

			return 0;
		}

	};


	struct Neuron {

		double bias{};
		//double sum_of_wheights{};
		//int layer{};
		double value = NAN;

	};


	struct Network
	{
		Genome* genome;
		std::vector<Neuron> neurons;

		Network(Genome* g) {
			genome = g;
			Init();
		}

		int Init() {
			for (Node_Gene node : genome->node_genes)
			{
				neurons.push_back(Neuron(node.bias));
			}
			return 0;
		}

		std::vector<double> Activate(std::vector<double> inputs) {

			fillInpuitNeurons(inputs);
			std::vector<double> results;

			for (Node_Gene node : genome->node_genes)
			{
				if (node.type == Node_Type::Output) {
					results.push_back(getNodeValue(node.node_ID));
				}
			}

			return results;
		}

		int fillInpuitNeurons(std::vector<double> inputs) {
			try {
				if (inputs.size() != Num_Of_Inputs) {
					throw std::runtime_error("ERROR: Wrong amout of inputs for network");
				}
			}
			catch (std::runtime_error e) {
				std::cout << e.what() << ", Expected " << Num_Of_Inputs << " inputs, got " << inputs.size() << "\n";
				exit(1);

			}

			for (Node_Gene node : genome->node_genes)
			{
				if (node.type == Node_Type::Input) {

					int i = node.node_ID - 1;
					neurons[i].value = inputs[i];
				}
			}
			return 0;
		}

		double ActFunction(double x) {
			return tanh(x);
		}

		double getNodeValue(int node_ID) {
			int i = node_ID - 1;

			if (isnan(neurons[i].value)) {

				double sum_of_wheights = 0;

				for (Link_Gene& link : genome->link_genes)
				{
					if (link.enabled) {
						if (/*(link.from_node == node_number) or*/ link.to_node_ID == node_ID) {
							sum_of_wheights += getNodeValue(link.from_node_ID) * link.wheight;
						}
					}
				}
				neurons[i].value = ActFunction(sum_of_wheights + neurons[i].bias);
				return neurons[i].value;
			}
			else
			{
				return neurons[i].value;
			}
		}

	};


	struct NEAT {

		Link_List* link_list = new Link_List();
		Genome* genome = new Genome(link_list);
		Network* netowk = new Network(genome);
		std::vector<Network*> networks;

		int Init() {
			for (int i = 0; i < Population_Size; i++)
			{
				networks.push_back(new Network(new Genome(link_list)));
			}
			return 0;
		}
	};

}
#endif