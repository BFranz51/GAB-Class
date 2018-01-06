/**
* @class ChromoTestFeatures.h
* @author Bryan Franz
* @date December 29, 2017
* @brief A chromosome implementation that uses all GA features
* GeneticAlgorithm.h contains 
*
* @see (link to GitHub)
*/

#ifndef CHROMO_TEST_FEATURES_H_
#define CHROMO_TEST_FEATURES_H_

#ifdef _MSC_VER
#pragma once
#endif	// _MSC_VER

#include <unordered_map>

#include "Chromo.h"

namespace ga
{
	class ChromoTestFeatures : public Chromo
	{
	public:
		using Chromo::setScore;
		explicit ChromoTestFeatures(const int t_initialStateId, int(*t_randomGenerator)(void))
			: Chromo(t_initialStateId, t_randomGenerator)
		{
			initializeValues(t_initialStateId);
		};
		~ChromoTestFeatures() {};

		void initializeValues(const int);
		void runFitnessFunctionLocal();

		// For data that needs custom mutations
		// (most commonly adding/subtracting by random amount rather than flipping bits)
		void mutateCustom();
		const static bool hasCustomMutations() { return true; };

		// Data processing
		void encode();
		void decode();
		void applyLimits();

		// Extra info to be used during crossovers and mutations
		static void getEncodedPartitions(std::vector<EncodedPartition>&, MutationLimits&);

		// Custom output
		friend std::ostream& operator<<(std::ostream&, const ChromoTestFeatures&);

	protected:
		// Input/output
		void writeDataToCSV(std::ostream&);
		void readDataFromCSV(std::vector<std::string>&);

	private:
		std::vector<short int> sudoku;
		std::vector<short int> num;
		std::vector<int> num2;
		std::vector<bool> bools;
		std::vector<float> floats;
		std::vector<float> betterFloats;
	};

} // namespace ga

#endif	// CHROMO_MAX_SUM_H_