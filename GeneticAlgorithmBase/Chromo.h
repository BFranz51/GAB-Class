/**
* @class Chromo.h
* @author Bryan Franz
* @date December 26, 2017
* @brief Contains a base class for chromosomes to be used by the GeneticAlgorithm class
*
* @see (link to GitHub)
*/

#ifndef CHROMO_H_
#define CHROMO_H_

#ifdef _MSC_VER
#pragma once
#endif	// _MSC_VER

#include <vector>
#include <iostream>
#include "GeneticAlgorithm.h"
#include "GeneticAlgorithmTechniques.h"
#include "ChromoSerialization.h"
#include "ImportData.h"

namespace ga
{
	class Chromo
	{
	public:
		explicit Chromo(const int t_initialStateId, int(*t_randomGenerator)(void))
			: m_randomGenerator(t_randomGenerator)
		{ };
		virtual ~Chromo() {};

		double getScore() const;
		std::string getEncoding() const;

		// Evolutionary default functions
		// Often overidden in derived classes
		template <typename C>
		void copyParent(const C&);
		template <typename C>
		void shuffleFromParents(const C&, const C&);
		template <typename C>
		void crossoverFromParents(const C&, const C&, const size_t);
		void mutate(std::vector<EncodedPartition>&, const MutationLimits, const MutationSelection, const size_t, const size_t, const short int);
		virtual void mutateCustom() = 0;

		// Pure virtual functions, required to be overridden by derived classes
		virtual void encode() = 0;
		virtual void decode() = 0;

		// Don't require limits to be applied
		void applyLimits() {};

		void writeToFileAsBinary(std::ofstream&);
		void readFromFileAsBinary(std::ifstream&);
		void writeToFileAsCSV(const size_t, std::ofstream&);
		void readFromFileAsCSV(std::ifstream&);

		friend std::ostream &operator<<(std::ostream& output, const Chromo& self) {
			output << "Chromo or << operator not overridden";
			return output;
		}

		static void addItemIndicesOfVector(std::vector<EncodedPartition>&, MutationLimits&, size_t&, const size_t, const size_t, const std::string, const bool);
		static void addItemIndicesOfBoolVector(std::vector<EncodedPartition>&, MutationLimits&, size_t&, const size_t, const std::string, const bool);

	protected:
		void setScore(double);
		virtual void writeDataToCSV(std::ostream&) = 0;
		virtual void readDataFromCSV(std::vector<std::string>&) = 0;

		std::string m_encoded;
		int(*m_randomGenerator)(void);

	private:
		double m_score{ 0.0 };
	};

	// Implement templated functions here, rather than in the cpp file

	/**
	*	@brief  Copies the encoded data from another chromo.
	*
	*	@param  t_parent specifies the Chromo to copy from
	*	@return void
	*/
	template <typename C>
	void Chromo::copyParent(const C& t_parent)
	{
		m_encoded = t_parent.m_encoded;
	}

	/**
	*	@brief  Copies the encoded data from two sources, randomly choosing each byte from the sources.
	*
	*	@param  t_parent1 specifies the first parent to copy from
	*	@param  t_parent2 specifies the second parent to copy from
	*	@return void
	*/
	template <typename C>
	void Chromo::shuffleFromParents(const C& t_parent1, const C& t_parent2)
	{
		m_encoded = shuffleEncodedData(t_parent1.m_encoded, t_parent2.m_encoded, m_randomGenerator);
	}

	/**
	*	@brief  Copies the encoded data from two sources using the n-Split method.
	*	This is done by dividing up the encoded string into (N + 1) partitions
	*	of random size, and selecting a different source parent for each partition.
	*
	*	@param  t_parent1 specifies the first parent to copy from
	*	@param  t_parent2 specifies the second parent to copy from
	*	@param  t_numSplits specifies the number of splits
	*	@return void
	*/
	template <typename C>
	void Chromo::crossoverFromParents(const C& t_parent1, const C& t_parent2, const size_t t_numSplits)
	{
		m_encoded = nSplitEncodedData(t_parent1.m_encoded, t_parent2.m_encoded, t_numSplits, m_randomGenerator);
	}

} // namespace ga

#endif	// CHROMO_H_