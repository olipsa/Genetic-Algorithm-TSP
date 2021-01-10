#include "Header.h"
using namespace std;
int main()
{
	unsigned int type;
	while (1)
	{
		cout << "Select 1 for Simulated Annealing or 2 for Genetical Algorithm: ";
		cin >> type;
		switch (type)
		{
		case 1:
			break;
		case 2:
			GeneticAlgorithm G;
			G.runIndependently(30);
			break;
		}

	}
}