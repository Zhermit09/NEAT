#include <iostream>
#include <vector>
//#include <numeric>
//#include <string>
//#include <cstdarg>

#pragma once 
int64_t Random();


#ifdef NEAT_AI_NEURALNETWORK
#undef NEAT_AI_NEURALNETWORK

namespace neat {

	//----------SETTINGS----------

	//# Network parameters
	const int Num_Of_Inputs = 3;
	const int Num_Of_Hidden = 0;       //Good to start off with one hidden if output is more than one
	const int Num_Of_Outputs = 1;

	//const float Link_pct = 1;

	const int Population_Size = 15;

	//# Wheight options
	const int Wheight_Range_Value = 30; //20?


	//# Bias options
	const int Bias_Range_Value = 30;


	//# Compatibility
	const double c1 = 1;
	const double c2 = 2;
	const double c3 = 0.4;

	const double comp_thresh = 6.0;

	//------------------------------


	double RandomDigits(int mod) {

		int64_t random = Random();
		double decimals = random / ((double)(pow(10, 19)));

		return (Random() % (mod)) + decimals;

	}

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

		int Add(Link_Gene obj) {

			list.push_back(obj);
			list[size() - 1].unique_ID = size();

			return 0;
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
		double Fitness{};
		double Avg_fitness{};

		Link_List* global_link_list{};

		std::vector<Node_Gene> node_genes{};
		std::vector<Link_Gene> link_genes{};
		//Problem with muliple of same ^connections


		Genome(Link_List* list) {
			global_link_list = list;
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
				node_genes.push_back(Node_Gene(i + 1, Node_Type::Input, 1, 0 /*+ RandomDigits(Bias_Range_Value)*/));
			}
			for (int i = Num_Of_Inputs; i < Num_Of_Inputs + Num_Of_Outputs; i++)
			{
				node_genes.push_back(Node_Gene(i + 1, Node_Type::Output, (Num_Of_Hidden > 0) + 2, 0 /*+ RandomDigits(Bias_Range_Value)*/));
			}
			for (int i = Num_Of_Inputs + Num_Of_Outputs; i < Num_Of_Inputs + Num_Of_Outputs + Num_Of_Hidden; i++)
			{
				node_genes.push_back(Node_Gene(i + 1, Node_Type::Hidden, 2, 0 /*+ RandomDigits(Bias_Range_Value)*/));
			}
			return 0;
		}

		int LinkInit() {

			if (Num_Of_Hidden <= 0) {

				for (Node_Gene& node : node_genes)
				{
					if (node.type == Node_Type::Output) {
						for (int i = 0; i < Num_Of_Inputs; i++)
						{
							link_genes.push_back(global_link_list->getInnovN(Link_Gene(node_genes[i].node_ID, node.node_ID)));
							link_genes.back().wheight = RandomDigits(Wheight_Range_Value);
						}
					}
				}
			}
			else {

				for (Node_Gene& node : node_genes)
				{
					if (node.type == Node_Type::Hidden) {
						for (int i = 0; i < Num_Of_Inputs; i++)
						{
							link_genes.push_back(global_link_list->getInnovN(Link_Gene(node_genes[i].node_ID, node.node_ID)));
							link_genes.back().wheight = RandomDigits(Wheight_Range_Value);
						}
					}
				}
				for (Node_Gene& node : node_genes)
				{
					if (node.type == Node_Type::Output) {
						for (int i = Num_Of_Inputs + Num_Of_Outputs; i < Num_Of_Inputs + Num_Of_Outputs + Num_Of_Hidden; i++)
						{
							link_genes.push_back(global_link_list->getInnovN(Link_Gene(node_genes[i].node_ID, node.node_ID)));
							link_genes.back().wheight = RandomDigits(Wheight_Range_Value);
						}
					}
				}
			}

			return 0;
		}

		int ValidateLayers() {

			int highest_layer = 0;

			for (Node_Gene& node : node_genes)
			{
				if (node.node_ID > Num_Of_Inputs + Num_Of_Outputs) {
					int longest_path = GetPath(node.node_ID);
					node.layer = 1 + longest_path;

					if ((1 + longest_path) > highest_layer) {
						highest_layer = (1 + longest_path);
					}
				}
			}
			if (node_genes.size() > Num_Of_Inputs + Num_Of_Outputs) {
				for (int i = Num_Of_Inputs; i < Num_Of_Inputs + Num_Of_Outputs; i++)
				{
					node_genes[i].layer = highest_layer + 1;
				}
			}
			else {
				for (int i = Num_Of_Inputs; i < Num_Of_Inputs + Num_Of_Outputs; i++)
				{
					node_genes[i].layer = 2;
				}
			}

			return 0;
		}

		int GetPath(int node_ID) {
			int longest_path = 0;

			for (Link_Gene& link : link_genes)
			{
				if (link.to_node_ID == node_ID) {
					int path_counter = 1 + GetPath(link.from_node_ID);
					if (path_counter > longest_path) {
						longest_path = path_counter;
					}
				}
			}
			return longest_path;
		}

	};


	struct Neuron {

		double bias{};
		double sum_of_wheights{};
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
			for (Node_Gene& node : genome->node_genes)
			{
				neurons.push_back(Neuron(node.bias));
			}
			return 0;
		}

		int LoadInputs(std::vector<double> inputs) {
			try {
				if (inputs.size() != Num_Of_Inputs) {
					throw std::runtime_error("ERROR: Wrong amout of inputs for the network");
				}
			}
			catch (std::runtime_error e) {
				std::cout << e.what() << ", Expected " << Num_Of_Inputs << " inputs, got " << inputs.size() << "\n";
				exit(1);

			}

			for (Node_Gene& node : genome->node_genes)
			{
				if (node.type == Node_Type::Input) {

					int i = node.node_ID - 1;
					neurons[i].value = inputs[i];
				}
			}
			return 0;
		}

		std::vector<double> Activate(std::vector<double> inputs) {

			LoadInputs(inputs);
			std::vector<double> results;

			for (Node_Gene& node : genome->node_genes)
			{
				if (node.type == Node_Type::Output) {
					results.push_back(getNodeValue(node.node_ID));
				}
			}
			for (Neuron& neuron : neurons)
			{
				neuron.sum_of_wheights = 0;
				neuron.value = NAN;
			}

			return results;
		}

		double ActFunction(double x) {
			return tanh(x);
		}

		double getNodeValue(int node_ID) {
			int i = node_ID - 1;

			if (isnan(neurons[i].value)) {

				neurons[i].sum_of_wheights = 0;

				for (Link_Gene& link : genome->link_genes)
				{
					if (link.enabled) {
						if (/*(link.from_node == node_number) or*/ link.to_node_ID == node_ID) {
							neurons[i].sum_of_wheights += getNodeValue(link.from_node_ID) * link.wheight;
						}
					}
				}

				neurons[i].value = ActFunction(neurons[i].sum_of_wheights + neurons[i].bias);
				//std::cout << "Neuron ID " << node_ID << " " << neurons[i].sum_of_wheights << " " << neurons[i].value << "\n";
				return neurons[i].value;
			}
			else
			{
				//std::cout << "Neuron ID " << node_ID << " " << neurons[i].sum_of_wheights << " " << neurons[i].value << "\n";
				return neurons[i].value;
			}
		}

	};


	struct Spicie {

		std::vector<Genome*> members;
		double Avg_spicie_fitness{};
		double Adj_spicie_fitness{};
		int Offspring_allowed{};


		int AverageOutFitness() {

			for (Genome* member : members)
			{
				member->Avg_fitness = member->Fitness / members.size();
			}

			return 0;
		}

		int AverageSpicieFitness() {

			double sum = 0;

			for (Genome* member : members)
			{
				sum += member->Fitness;
			}
			Avg_spicie_fitness = sum / members.size();

			return 0;
		}

		int AdjustSpicieFitness() {
			Adj_spicie_fitness = Avg_spicie_fitness / members.size();

			return 0;
		}

	};


	struct NEAT {

		Link_List* global_link_list = new Link_List();
		std::vector<Genome*> genomes;

		double Global_spicie_avg{};
		std::vector<Spicie*> spicies;


		NEAT() {

			for (int i = 0; i < Population_Size; i++)
			{
				genomes.push_back(new Genome(global_link_list));
			}
		}

		std::vector<Network*> ConstructNets() {

			std::vector<Network*> networks;

			if (networks.empty()) {
				networks.clear();
				for (int i = 0; i < Population_Size; i++)
				{
					networks.push_back(new Network(genomes[i]));
				}
			}

			return networks;
		}

		int Evolve() {

			Speciate();
			std::cout << spicies.size()<<"\n";
			return 0;
		}


		int Speciate() {
			spicies.clear(); //Wrong, this is Gen 0 only!!!

			std::vector<neat::Genome*> genes = genomes;
			int k = 0;
			Genome* net_A;


			while (genes.size() != 0)
			{
				spicies.push_back(new Spicie());

				k = (int)(Random() % genes.size());
				net_A = genes[k];

				spicies[spicies.size() - 1]->members.push_back(net_A);
				genes.erase(genes.begin() + k);

				Genome* net_B;
				std::vector<int> remove;
				for (int i = 0; i < genes.size(); i++)
				{
					net_B = genes[i];

					if (CompatibilityCheck(net_A, net_B) <= comp_thresh) {
						spicies[spicies.size() - 1]->members.push_back(net_B);
						remove.push_back(i);
					}
				}

				for (int i = (int)remove.size() - 1; i >= 0; i--)
				{
					genes.erase(genes.begin() + i);

				}

			}
			return 0;
		}

		double CompatibilityCheck(Genome* net_A, Genome* net_B) {

			//Tips for future debug, CD might be wrong cuz i'm checking for enabled connections only

			int E = getExcess(net_B, getMaxID(net_A));

			int D = getDisjoint(net_A, net_B) + getDisjoint(net_B, net_A);

			double dW = getdW(net_A, net_B);

			int N = getN(net_A, net_B);

			double CD = (c1 * E) / N + (c2 * D) / N + (c3 * dW);

			return CD;
		}

		int getMaxID(Genome* genome) {

			int maxID = 0;

			for (Link_Gene& link : genome->link_genes) {
				if ((link.unique_ID > maxID) and link.enabled) {
					maxID = link.unique_ID;
				}
			}
			return maxID;
		}

		int getExcess(Genome* net_B, int maxID_A) {

			int E = 0;

			for (Link_Gene& link : net_B->link_genes) {

				if ((link.unique_ID > maxID_A) and link.enabled) {
					E += 1;
				}
			}

			return E;
		}

		int getDisjoint(Genome* net_A, Genome* net_B) {
			int D = 0;

			for (Link_Gene& link_A : net_A->link_genes) {

				if (link_A.enabled) {

					bool dissjoint = true;

					for (Link_Gene& link_B : net_B->link_genes) {

						if ((link_B.unique_ID == link_A.unique_ID) and link_B.enabled) {
							dissjoint = false;
							break;
						}
					}
					if (dissjoint) {
						D += 1;
					}

				}
			}

			return D;
		}

		double getdW(Genome* net_A, Genome* net_B) {

			double dW = 0;

			auto& links_A = net_A->link_genes;
			auto& links_B = net_B->link_genes;
			int size = 0;

			double sum = 0;
			double counter = 0;

			if (links_A.size() >= links_B.size()) {
				size = (int)links_B.size();
			}
			else { size = (int)links_A.size(); }

			for (int i = 0; i < size; i++)
			{
				if ((links_A[i].unique_ID == links_B[i].unique_ID) and links_A[i].enabled and links_B[i].enabled) {
					sum += abs(links_A[i].wheight - links_B[i].wheight);
					counter++;
				}
			}
			dW = sum / counter;

			return dW;
		}

		int getN(Genome* net_A, Genome* net_B) {

			auto& links_A = net_A->link_genes;
			auto& links_B = net_B->link_genes;
			auto& big_Link = links_A;
			int counter = 0;

			if (links_A.size() >= links_B.size()) {
				big_Link = links_A;
			}
			else { big_Link = links_B; }

			for (int i = 0; i < big_Link.size(); i++)
			{
				if (big_Link[i].enabled) {
					counter++;
				}
			}

			return counter;
		}

		int getAjustedGlobalSpicieAvg() {
			double sum = 0;

			for (Spicie* spicie : spicies)
			{
				sum += spicie->Avg_spicie_fitness;
			}
			Global_spicie_avg = sum / genomes.size();

			return 0;
		}

		int CalcOffspring() {

			int population = 0;

			for (Spicie* spicie : spicies)
			{
				spicie->Offspring_allowed = (int)round((spicie->Adj_spicie_fitness * spicie->members.size()) / Global_spicie_avg);
				population += spicie->Offspring_allowed;

			}

			/*if (population != Population_Size) {
				double fitcompare = 0;
				int i = 0;


			}*/

			return 0;
		}

	};

}
#endif