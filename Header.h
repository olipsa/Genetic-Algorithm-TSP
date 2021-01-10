#pragma once
#include<vector>
#include<iostream>
#include<fstream>
#include<vector>
#include <algorithm> // std::shuffle
#include <chrono> // std::chrono::system_clock
#include <random> // std::default_random_engine
using namespace std;
ifstream fin("instances.txt");
ofstream fout("rezultate.txt");
class GeneticAlgorithm {
	unsigned int popSize, bestChromosome, generations, pathMin, bestSol2, bestChromosome2, noImprovements;
	double MUTATION_PROBABILITY = 0.007, CROSSOVER_PROBABILITY = 0.7;
	vector<vector<int>> population;
	vector<double> g, q;
	vector<int>selected;
	void generatePopulation()
	{
		population.clear();
		population.resize(popSize);

		for (int i = 0; i < popSize; i++)
		{
			for (int j = 0; j < nrCities; j++)
				population[i].push_back(j + 1);

			unsigned seed = chrono::system_clock::now().time_since_epoch().count();
			shuffle(population[i].begin(), population[i].end(), default_random_engine(seed));
		}
	}
	void evaluatePopulation()
	{
		int pathLength, city1, city2;
		double result, T = 0;
		vector<double>p;
		//Get the fitness of each chromosome and  store it in g array
		g.clear();
		q.clear();
		for (int i = 0; i < population.size(); i++)
		{
			pathLength = 0;
			for (int j = 0; j < nrCities; j++)
			{
				city1 = population[i][j];
				if (j == nrCities - 1)
					city2 = population[i][0];
				else
					city2 = population[i][j + 1];

				city1--; city2--;
				pathLength += cities[city1][city2];
			}

			if (pathLength < bestSol)
			{
				bestSol = pathLength;
				bestChromosome = i;
			}
			else if (pathLength < bestSol2)
			{
				bestSol2 = pathLength;
				bestChromosome2 = i;
			}
			result = (double)1 / pathLength;
			g.push_back(result);
			T += result;

		}
		//Get individual probability of each chromosome and store it in p array
		for (int i = 0; i < population.size(); i++)
			p.push_back(g[i] / T);

		//Get accumulated probability and store it in q array
		q.push_back(0);
		for (int i = 1; i <= population.size(); i++)
			q.push_back(q[i - 1] + p[i - 1]);

	}
	void selectBest()
	{
		int pathLength, city1, city2;
		vector<double>pathLen;
		for (int i = 0; i < population.size(); i++)
		{
			pathLength = 0;
			for (int j = 0; j < nrCities; j++)
			{
				city1 = population[i][j];
				if (j == nrCities - 1)
					city2 = population[i][0];
				else
					city2 = population[i][j + 1];

				city1--; city2--;
				pathLength += cities[city1][city2];
			}
			pathLen.push_back(pathLength);

		}

		for (int i = 0; i < population.size() - 1; i++)
			for (int j = i + 1; j < population.size(); j++)
				if (pathLen[i] > pathLen[j])
				{
					swap(pathLen[i], pathLen[j]);
					swap(population[i], population[j]);
				}

		//eliminate duplicates
		/*int n = population.size();
		for(int i=n-1;i>0;i--)
			if (pathLen[i] == pathLen[i - 1]&&population.size()>popSize)
			{
				population.erase(population.begin() + i);
				pathLen.erase(pathLen.begin() + i);
			}

		if (pathLen[0] < bestSol)
		{
			noImprovements = 0;
			//fout << "Generation number " << generations << " with best sol " << bestSol << " and pm="<<MUTATION_PROBABILITY<<endl;
			//MUTATION_PROBABILITY = 0.007;
		}
		else
			noImprovements++;

		if (noImprovements > 100 && MUTATION_PROBABILITY<0.05)
			MUTATION_PROBABILITY *= 1.01;*/

		bestSol = pathLen[0];
		bestChromosome = 0;

		int i = 1;
		while (pathLen[i] == bestSol && i < pathLen.size() - 1) i++;
		if (i == pathLen.size() - 1)
		{
			bestSol2 = bestSol;
			bestChromosome2 = bestChromosome;
		}
		else
		{
			bestSol2 = pathLen[i];
			bestChromosome2 = i;
		}

		while (population.size() != popSize)
			population.erase(population.begin() + popSize);
		//for (int i = 0; i < population.size(); i++)
			//cout << pathLen[i] << ' ';
		//cout << endl;

	}
	void rouletteWheel()
	{
		double randNo;
		vector<int> bestChr, bestChr2;
		unsigned int iterations = 0;
		bestChr = population[bestChromosome];
		bestChr2 = population[bestChromosome2];
		int nr = population.size();
		while (iterations < popSize - 2)
		{
			//spin the roulette wheel popSize times
			randNo = (double)rand() / RAND_MAX;
			for (unsigned int j = 0; j < nr; j++)
				if (q[j] < randNo && randNo <= q[j + 1])
				{
					iterations++;
					population.push_back(population[j]);
					break;
				}
			//98 chromosomes are selected for the next generation
		}
		//Remove chromosomes that were not selected
		while (population.size() != popSize - 2)
			population.erase(population.begin()); //remove previous chromosomes from population

		randNo = rand() % (popSize - 1);
		population.insert(population.begin() + randNo, bestChr); //elitism
		bestChromosome = randNo;
		randNo = rand() % popSize;
		population.insert(population.begin() + randNo, bestChr2);
		bestChromosome2 = randNo;

	}
	void crossover1()
	{
		int parent1, parent2, cutPoint1, cutPoint2, city, foundAtPosition;
		double randNo;
		vector<int>::iterator it;
		randNo = rand() % selected.size();
		parent1 = selected[randNo];
		selected.erase(selected.begin() + randNo);

		randNo = rand() % selected.size();
		parent2 = selected[randNo];
		selected.erase(selected.begin() + randNo);

		population.resize(population.size() + 2);

		population[population.size() - 1].resize(nrCities);
		population[population.size() - 2].resize(nrCities);

		cutPoint1 = rand() % nrCities;
		cutPoint2 = rand() % nrCities;
		if (cutPoint1 > cutPoint2)
			swap(cutPoint1, cutPoint2);

		for (int i = cutPoint1; i <= cutPoint2; i++)
		{
			population[population.size() - 1][i] = population[parent2][i];
			population[population.size() - 2][i] = population[parent1][i];
		}

		for (int i = 0; i < nrCities; i++)
			if (i<cutPoint1 || i>cutPoint2)
			{
				//for first child
				city = population[parent1][i];
				it = find(population[population.size() - 1].begin(), population[population.size() - 1].end(), city);
				while (it != population[population.size() - 1].end()) //while element is already in child chromosome
				{
					foundAtPosition = it - population[population.size() - 1].begin();
					city = population[population.size() - 2][foundAtPosition];
					it = find(population[population.size() - 1].begin(), population[population.size() - 1].end(), city);
				}
				population[population.size() - 1][i] = city;

				//for second child
				city = population[parent2][i];
				it = find(population[population.size() - 2].begin(), population[population.size() - 2].end(), city);
				while (it != population[population.size() - 2].end()) //while element is already in child chromosome
				{
					foundAtPosition = it - population[population.size() - 2].begin();
					city = population[population.size() - 1][foundAtPosition];
					it = find(population[population.size() - 2].begin(), population[population.size() - 2].end(), city);
				}
				population[population.size() - 2][i] = city;

			}

	}
	void crossover()
	{
		double chance;
		selected.clear();
		for (int i = 0; i < population.size(); i++)
		{
			chance = (double)rand() / RAND_MAX;
			if (chance < CROSSOVER_PROBABILITY)
				selected.push_back(i);
		}
		if (selected.size() % 2 == 1)
			selected.pop_back();


		while (selected.size() != 0)
			crossover1();

		selectBest();

	}
	void mutation1(int chromosomePosition, int selectedGene)
	{
		unsigned int selectedGene2 = rand() % (nrCities);
		swap(population[chromosomePosition][selectedGene], population[chromosomePosition][selectedGene2]);
	}
	void mutation2(int chromosomePosition, int selectedCutPoint)
	{
		int selectedCutPoint2 = rand() % (nrCities);
		if (selectedCutPoint > selectedCutPoint2) swap(selectedCutPoint, selectedCutPoint2);

		for (int i = selectedCutPoint, j = selectedCutPoint2; i < j; i++, j--)
			swap(population[chromosomePosition][i], population[chromosomePosition][j]);


	}
	void mutation()
	{
		double chance;
		int randNo;
		vector<int> result1, result2;
		for (int i = 0; i < population.size(); i++)
			if (i != bestChromosome && i != bestChromosome2)
				for (int j = 0; j < nrCities; j++)
				{
					chance = (double)rand() / RAND_MAX;
					if (chance < MUTATION_PROBABILITY)
					{
						randNo = rand() % 2;
						switch (randNo + 1)
						{
						case 1:
							mutation1(i, j);
							break;
						case 2:
							mutation2(i, j);
							break;

						}
					}

				}

	}
public:
	unsigned int bestSol, nrCities;
	vector<vector<int>> cities;
	void run()
	{
		generations = 0;
		popSize = 100;
		bestSol = 9999999999;
		bestSol2 = 99999999999;
		noImprovements = 0;
		generatePopulation(); //generate an initial population with random paths
		evaluatePopulation();
		while (generations < 10000)
		{
			rouletteWheel();
			crossover();
			mutation();
			evaluatePopulation();
			generations++;
			/*for (int i = 0; i < population.size(); i++)
			{
				for (int j = 0; j < nrCities; j++)
					cout << population[i][j] << ' ';
				cout << endl;
			}*/
		}
	}
	void runIndependently(int sampleSize)
	{
		fin >> nrCities;
		cities.resize(nrCities);
		for (int i = 0; i < nrCities; i++)
		{
			cities[i].resize(nrCities);
			for (int j = 0; j < nrCities; j++)
				fin >> cities[i][j];
		}
		srand(time(NULL));
		int Min = 9999999999999, Maximum = -99999999999999;
		double Avg = 0, stDev = 0;
		vector<int>results;
		cout << "Please wait a looot...\n";
		for (int i = 0; i < sampleSize; i++)
		{
			run();
			Avg += bestSol;
			fout << "Solution for test no." << i + 1 << " is: " << bestSol << endl;
			results.push_back(bestSol);

			if (bestSol < Min)
				Min = bestSol;
		}
		Avg /= sampleSize;

		for (int i = 0; i < sampleSize; i++)
		{
			stDev += pow(results[i] - Avg, 2);
			if (results[i] > Maximum)
				Maximum = results[i];
		}
		stDev = sqrt(stDev / sampleSize);


		fout << "Avg: " << Avg << "\nMin: " << Min << "\nMax: " << Maximum << "\nstDev: " << stDev << endl;

	}
};