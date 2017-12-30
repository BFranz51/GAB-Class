/**
* @file main.cpp
* @author Bryan Franz
* @date December 30, 2017
* @brief Just a quick script to test out different GAs
*
* @see (link to GitHub)
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <random>

#include "GeneticAlgorithm.h"
#include "Chromo.h"
#include "ChromoTestFeatures.h"
#include "ChromoKnapsack.h"

using namespace std;

int main()
{
	
	cout << "Genetic Algorithm Testing\n\n";

	srand(static_cast<unsigned int>(time(NULL)));

	//ga::runTestGenerations<ga::ChromoTestFeatures>(&rand);
	//cin.get();
	
	/*
	vector<int> a{ 0 };
	vector<float> b{ 0.0 };
	vector<bool> bl{ true, false, true, false, true, false, true, true };
	vector<float> dd{ 0.0 };
	string s{ "" };
	for (int i{ 0 }; i < 1000; ++i) {
		//dd.at(0) += ga::randomRangeDouble(-5.0, 5.0, &rand);
		//dd.at(0) = 0;
		cout << dd.at(0) << " [" << ga::serializeVector(dd) << "]\n";
		
		s = "";
		s += ga::serializeVector(a);
		s += ga::serializeVector(b);
		s += ga::serializeBoolVector(bl);
		s += ga::serializeVector(dd);
		
		size_t l{ 0 };
		l = ga::deserializeVector(a, s, l);
		l = ga::deserializeVector(b, s, l);
		l = ga::deserializeBoolVector(bl, s, l);
		l = ga::deserializeVector(dd, s, l);
		
		cout << dd.at(0) << " [" << ga::serializeVector(dd) << "]\n";
	}

	cin.get();*/

	/*string s{ "1234567890" };
	vector<ga::SerialPartition> sp;
	sp.push_back(ga::SerialPartition(0, 5));
	sp.push_back(ga::SerialPartition(5, 5));
	for (int iii{ 0 }; iii < 5; ++iii)
	{
		s = "1234567890";
		cout << s << endl;
		ga::mutateRandomBits(s, sp, ga::MutationSelection::entirePartition, 1, 1, 100, &rand);
		cout << s << endl << endl;
	}

	cin.get();*/

	//ChromoMaxSum* test = new ChromoMaxSum(static_cast<size_t> (0), static_cast<int>(0));
	//cout << *test;
	ga::GeneticAlgorithm<ga::ChromoKnapsack>* ga1 = new ga::GeneticAlgorithm<ga::ChromoKnapsack>("Test A", 10, 0, &rand);
	
	ga1->setNumberToCopy(2);
	ga1->setNumberToShuffle(2);
	ga1->setNumberToCrossover(2);
	ga1->setNumberToMutate(6);

	ga1->setMutationSelection(ga::MutationSelection::entirePartition);
	ga1->setMutationCountMax(3);
	ga1->setMutationBitWidth(8);
	ga1->setMutationChanceIn100(30);

	cout << *ga1;

	for (int g{ 0 }; g < 3; g++) {
		ga1->advanceGeneration(false);
		cout << *ga1;
		cout << "\n\n";
	}
	cout << *ga1;
	cout << "\nWRITING TO FILE, GEN = " << ga1->getGeneration() << "\n";
	ga1->writeToFileAsCSV("TestFile1.csv");

	for (int g{ 0 }; g < 1; g++) {
		ga1->advanceGeneration();
		cout << *ga1;
		cout << "\n\n";
	}

	cout << "\nREADING FROM FILE, GEN = " << ga1->getGeneration() << "\n";
	ga1->readFromFileAsCSV("TestFile1.csv");
	cout << "NOW GEN = " << ga1->getGeneration() << "\n";
	cout << *ga1;
	

	cout << "Destructing...";
	delete ga1;
	cin.get();
	cin.get();
	return 0;
}