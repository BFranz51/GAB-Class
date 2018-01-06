/**
* @class ChromoKnapsack.h
* @author Bryan Franz
* @date December 26, 2017
* @brief Attempts to fill a "bag" of items, maximizing their value while staying under a weight limit
*
* @see (link to GitHub)
*/

#ifndef CHROMO_KNAPSACK_H_
#define CHROMO_KNAPSACK_H_

#ifdef _MSC_VER
#pragma once
#endif	// _MSC_VER

#include "Chromo.h"

namespace ga
{
	class ChromoKnapsack : public Chromo
	{
	public:
		explicit ChromoKnapsack(const int t_initialStateId, int(*t_randomGenerator)(void))
			: Chromo(t_initialStateId, t_randomGenerator)
		{
			initializeValues(t_initialStateId);
		};
		~ChromoKnapsack() {};

		void initializeValues(const int);

		// Run the fitness function on only this chromosome,
		// rather than in a larger simulation
		void runFitnessFunctionLocal();

		// Custom mutations for certain data
		// Most commonly adding/subtracting by random amount rather than flipping bits
		// Floats/doubles are often easier to mutate this way
		void mutateCustom();
		const static bool hasCustomMutations() { return false; };

		// Data processing that enables
		// crossover and mutation phases,
		// as well as file I/O
		void encode();
		void decode();
		void applyLimits();

		// Extra info to be used during crossovers and mutations
		static void getEncodedPartitions(std::vector<EncodedPartition>&, MutationLimits&);

		// Custom output
		friend std::ostream& operator<<(std::ostream&, const ChromoKnapsack&);

	protected:
		// Input/output
		void writeDataToCSV(std::ostream&);
		void readDataFromCSV(std::vector<std::string>&);

	private:
		// Data used in fitness function
		std::vector<bool> hasItem;
	};

} // namespace ga

#endif	// CHROMO_KNAPSACK_H_