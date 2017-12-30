/**
* @class GeneticAlgorithm.h
* @author Bryan Franz
* @date December 26, 2017
* @brief Contains a templated class for running a genetic algorithm on a chromosome class
*
* Description...
* @see (link to GitHub)
*/

#ifndef GENETIC_ALGORITHM_H_
#define GENETIC_ALGORITHM_H_

#ifdef _MSC_VER
#pragma once
#endif	// _MSC_VER

#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <iostream>
#include <fstream>

#include "GeneticAlgorithmTechniques.h"
#include "ChromoSerialization.h"

namespace ga
{
	template <typename C>
	class GeneticAlgorithm
	{
	public:
		explicit GeneticAlgorithm(const std::string t_name, const size_t t_generationSize, const int t_initialStateId, int (*t_randomGenerator)(void))
			: m_name(t_name),
			m_generationSize(t_generationSize),
			m_randomGenerator(t_randomGenerator),
			m_mutationSelection(MutationSelection::pureRandom),
			m_mutationCountMax(static_cast<size_t>(0)),
			m_mutationBitWidth(static_cast<size_t>(8)),
			m_mutationChanceIn100(static_cast<short int>(100)),
			m_numCrossoverSplits(static_cast<size_t>(1)),
			m_currentGeneration(static_cast<size_t>(0))
		{
			for (size_t i = 0; i < t_generationSize; i++) {
				m_chromo.push_back(new C(t_initialStateId, m_randomGenerator));
			}
			C::getSerialItemIndices(m_serialItemIndices, m_mutationLimits);
		};

		~GeneticAlgorithm()
		{
			// Cleanup
			for (auto p : m_chromo)
				delete p;
			for (auto p : m_competition)
				delete p;
		};

		void setCompetition(const std::vector<C*> t_competition);

		// The magic happens here
		void advanceGeneration(const bool showDebugMessages = false);

		// Translate Chromo data to and from strings
		// These are public to help with testing
		void serializeChromos();
		void deserializeChromos();
		
		// Algorithm settings
		void setMutationSelection(const MutationSelection);
		void setMutationCountMax(const size_t);
		void setMutationCountFunction(size_t(*)(const size_t, const double, int(*)(void)));
		void setMutationBitWidth(const size_t);
		void setMutationChanceIn100(const short int);
		void setNumberToCopy(const size_t);
		void setNumberToShuffle(const size_t);
		void setNumberToCrossover(const size_t);
		void setNumberToMutate(const size_t);
		void setNumberOfCrossoverSplits(const size_t);

		unsigned long int getGeneration() const;

		// File I/O
		void writeToFileAsBinary(const std::string);
		void readFromFileAsBinary(const std::string);
		void writeToFileAsCSV(const std::string);
		void readFromFileAsCSV(const std::string);

		// Print to console
		friend std::ostream &operator<<(std::ostream& output, const GeneticAlgorithm& self) {
			// Output header info
			output << "GA " << self.m_name << "\nGeneration #" << self.getGeneration() << "\n{\n";
			// Output chromos
			size_t id{ 0 };
			for (typename std::vector<C*>::const_iterator it { std::begin(self.m_chromo) }; it != std::end(self.m_chromo); ++it)
			{
				output << "ID [" << id << "] = ";
				output << **it << "\n";
				++id;
			}
			output << "}\n\n";
			return output;
		}

	protected:
		// Chromosomes
		std::vector<C*> m_chromo;
		// Any competing chromosomes
		// (usually from other GAs)
		std::vector<C*> m_competition;

		// Number of chromosomes
		size_t m_generationSize;

		// Controls bit selection during mutations
		MutationSelection m_mutationSelection;
		// When a Chromo is selected for mutation,
		// create mutations in range [1, m_mutationCountMax]
		size_t m_mutationCountMax;
		// Each mutation modifies a block of bits of this width
		size_t m_mutationBitWidth;
		// Within mutation blocks, this controls the
		// chance of any bit being toggled
		short int m_mutationChanceIn100;

		// Variables which control the number of
		// Chromos to run different crossover processes on
		size_t m_numIdealSafe{ 1 };
		size_t m_numEvolveSafe{ 0 };
		size_t m_numEvolveCopy{ 0 };
		size_t m_numEvolveCopyExtra{ 0 };
		size_t m_numEvolveShuffle{ 0 };
		size_t m_numEvolveCrossover{ 0 };
		
		// In a crossover, split the serialized data
		// this many times
		size_t m_numCrossoverSplits{ 1 };

		// Number of Chromos to mutate
		size_t m_numEvolveMutate{ 0 };

		// Indices recalculated each generation
		size_t m_firstIdEvolveCopy;
		size_t m_firstIdEvolveShuffle;
		size_t m_firstIdEvolveCrossover;
		size_t m_firstIdEvolveMutate;

		unsigned long int m_currentGeneration{ 0 };

	private:
		// Evolution phases
		bool runSelectionPhase(const bool);
		void runCrossoverPhase(const bool);
		void runMutationPhase(const bool);
		
		// Selection processes:
		void getFitnessScores();
		void sortChromoByScore();
		
		// Helper functions:
		
		// Ensure GA settings will actually work
		// ie. At least 2 Chromos not in crossover phase
		bool areBoundsValid();
		// Use GA settings to calculate the number
		// of Chromos to mark "safe"
		void determineIdealSafeChromos();
		// Check fitness scores to ensure that invalid Chromos
		// are always marked "unsafe"
		void determineSafeChromos();
		// Calculate indices for crossover + mutation processes
		void determineEvolutionRanges();

		// Crossover processes:
		void doCopies();
		void doShuffles();
		void doCrossovers();

		// Mutation processes:
		void doMutations();
		void doCustomMutations();

		// Utility functions
		size_t pickRandomUnsafeChromo();
		size_t pickRandomSafeChromo();
		void pickTwoRandomSafeChromos(size_t&, size_t&);

		// Information about serialized string partitions
		std::vector<SerialPartition> m_serialItemIndices;
		// The extent to which we are allowed to mutate serialized strings
		// This allows data to be serialized while only being mutated by
		// customMutations() method
		MutationLimits m_mutationLimits;

		// My name
		std::string m_name;

		// Random number generator
		int (*m_randomGenerator)(void);
	};





	/**
	*	@brief  Calls methods in order to perform selection, crossover, and mutation.
	*	Also handles errors due to incorrect bounds.
	*
	*	@return void
	*/
	template <typename C>
	void GeneticAlgorithm<C>::advanceGeneration(const bool showDebugMessages)
	{
		// Preparation phase
		if (!areBoundsValid()) {
			// Error message has already been displayed
			std::cout << "advanceGeneration() terminating.\n";
			return;
		}

		// Selection phase
		if (!runSelectionPhase(showDebugMessages)) {
			// Error message has already been displayed
			std::cout << "advanceGeneration() terminating.\n";
			return;
		}
		if (showDebugMessages) {
			std::cout << "Selection phase complete...\n";
		}

		// Crossover phase
		runCrossoverPhase(showDebugMessages);
		if (showDebugMessages) {
			std::cout << "Crossover phase complete...\n";
		}

		// Mutation phase
		runMutationPhase(showDebugMessages);
		if (showDebugMessages) {
			std::cout << "Mutation phase complete...\n";
		}

		m_currentGeneration++;
	}
	
	/**
	*	@brief  Outputs GA settings and Chromo data to specified file.
	*	The Chromo data written is the serialized string data, not the actual
	*	variables. Therefore, the Chromo's serialize() function must be
	*	called before this function is called.
	*	
	*	@param  t_filename specifies the path and filename of the output file
	*	@return void
	*/
	template <typename C>
	void GeneticAlgorithm<C>::writeToFileAsBinary(const std::string t_filename)
	{
		// Attempt to open file
		std::ofstream oStream(t_filename, std::ios::out | std::ios::binary);

		if (oStream.is_open())
		{
			int tempInt;

			// Write GA settings
			oStream.write((char*)&m_generationSize, sizeof(size_t));
			oStream.write((char*)&m_mutationCountMax, sizeof(size_t));
			oStream.write((char*)&m_mutationBitWidth, sizeof(size_t));
			oStream.write((char*)&m_mutationChanceIn100, sizeof(size_t));
			oStream.write((char*)&m_numEvolveCopy, sizeof(size_t));
			oStream.write((char*)&m_numEvolveShuffle, sizeof(size_t));
			oStream.write((char*)&m_numEvolveCrossover, sizeof(size_t));
			oStream.write((char*)&m_numEvolveMutate, sizeof(size_t));
			oStream.write((char*)&m_currentGeneration, sizeof(unsigned long int));

			tempInt = static_cast<int>(m_mutationSelection);
			oStream.write((char*)&tempInt, sizeof(int));

			// Write each Chromo
			for (auto it{ std::begin(m_chromo) }; it != std::end(m_chromo); ++it)
			{
				(*it)->writeToFileAsBinary(oStream);
			}

			oStream.close();

		}
		else std::cout << "\nERROR: Unable to open file " + t_filename + "\n\n";
	}

	/**
	*	@brief  Opens file and reads GA settings and Chromo data. Then deserializes Chromo data.
	*	Since the Chromo data in the file has been serialized to string format,
	*	this method deserializes it.
	*
	*	@param  t_filename specifies the path and filename of the file to read from
	*	@return void
	*/
	template <typename C>
	void GeneticAlgorithm<C>::readFromFileAsBinary(const std::string t_filename)
	{
		// Attempt to open file
		std::ifstream iStream(t_filename, std::ios::in | std::ios::binary | std::ios::ate);
		
		if (iStream.is_open())
		{
			// Temporary vars
			int tempInt;
			size_t newGenerationSize;

			char * readMemSizeT;
			char * readMemInt;
			char * readMemULongInt;
			memSize = sizeof(size_t);
			readMemSizeT = new char[memSize];
			readMemInt = new char[sizeof(int)];
			readMemULongInt = new char[sizeof(unsigned long int)];


			iStream.seekg(0, ios::beg);
			iStream.read(readMemSizeT, memSize);
			newGenerationSize = *((size_t*) readMemSizeT);

			iStream.read(readMemSizeT, memSize);
			m_mutationCountMax = *((size_t*)readMemSizeT);

			iStream.read(readMemSizeT, memSize);
			m_mutationBitWidth = *((size_t*)readMemSizeT);

			iStream.read(readMemSizeT, memSize);
			m_mutationChanceIn100 = *((size_t*)readMemSizeT);

			iStream.read(readMemSizeT, memSize);
			m_numEvolveCopy = *((size_t*)readMemSizeT);

			iStream.read(readMemSizeT, memSize);
			m_numEvolveShuffle = *((size_t*)readMemSizeT);

			iStream.read(readMemSizeT, memSize);
			m_numEvolveCrossover = *((size_t*)readMemSizeT);

			iStream.read(readMemSizeT, memSize);
			m_numEvolveMutate = *((size_t*)readMemSizeT);

			iStream.read(readMemULongInt, sizeof(unsigned long int));
			m_currentGeneration = *((unsigned long int*)readMemULongInt);
			
			iStream.read(readMemInt, sizeof(int));
			tempInt = *((int*)readMemInt);
			m_mutationSelection = static_cast<MutationSelection>(tempInt);

			// Apply GA setting - Generation Size
			if (m_generationSize != newGenerationSize) {
				m_generationSize = newGenerationSize;
				m_chromo.resize(m_generationSize, new C(0, m_randomGenerator));
			}
			
			delete[] readMemSizeT;
			delete[] readMemInt;
			delete[] readMemULongInt;

			// Read each Chromo
			for (auto it{ std::begin(m_chromo) }; it != std::end(m_chromo); ++it)
			{
				// Read serialized data
				(*it)->readFromFileAsBinary(iStream);
				// Translate to variables and limit
				(*it)->deserialize();
				(*it)->applyLimits();
				// Serialize again, in case limits were applied
				(*it)->serialize();
			}

			iStream.close();
			
		}
		else std::cout << "\nERROR: Unable to open file " + t_filename + "\n\n";
	}

	/**
	*	@brief  Outputs GA settings and Chromo data to specified CSV file.
	*	As the actual Chromo variables are written, rather than the serialized strings,
	*	the inherited Chromo method writeToFileAsCSV() is called.
	*
	*	@param  t_filename specifies the path and filename of the output file
	*	@return void
	*/
	template <typename C>
	void GeneticAlgorithm<C>::writeToFileAsCSV(const std::string t_filename)
	{
		// Attempt to open file
		std::ofstream oStream(t_filename, std::ios::out);
		
		if (oStream.is_open())
		{
			// Write header for GA settings
			oStream << "CurrentGen,GenSize,MutateMax,MutateBitWi,MutateIn100,Copy,Shuffle,Crossover,Mutate,MutationSelect\n";

			// Write GA settings
			oStream << m_currentGeneration << "," <<
				m_generationSize << "," <<
				m_mutationCountMax << "," <<
				m_mutationBitWidth << "," <<
				m_mutationChanceIn100 << "," <<
				m_numEvolveCopy << "," <<
				m_numEvolveShuffle << "," <<
				m_numEvolveCrossover << "," <<
				m_numEvolveMutate << "," <<
				static_cast<int>(m_mutationSelection) <<
				"\n\n";

			// Write header for chromos
			oStream << "ChromoID,Score";
			for (size_t i{ 0 }; i < m_serialItemIndices.size(); ++i) {
				if (m_serialItemIndices.at(i).type == SerialPartitionType::eachBitUnique) {
					// Boolean vectors require columns for each bit
					for (size_t bit{ 0 }; bit < m_serialItemIndices.at(i).uniqueBits; ++bit) {
						oStream << "," << m_serialItemIndices.at(i).name << bit;
					}
				}
				else {
					oStream << "," << m_serialItemIndices.at(i).name;
				}
			}
			oStream << "\n";

			// Write each Chromo
			size_t id{ 0 };
			for (auto it{ std::begin(m_chromo) }; it != std::end(m_chromo); ++it)
			{
				(*it)->writeToFileAsCSV(id++, oStream);
			}

			oStream.close();

		}
		else std::cout << "\nERROR: Unable to open file " + t_filename + "\n\n";
	}

	/**
	*	@brief  Opens file and reads GA settings and Chromo data.
	*	As the actual Chromo variables are written, rather than the serialized strings,
	*	the inherited Chromo method readFromFileAsCSV() is called.
	*
	*	@param  t_filename specifies the path and filename of the file to read from
	*	@return void
	*/
	template <typename C>
	void GeneticAlgorithm<C>::readFromFileAsCSV(const std::string t_filename)
	{
		// Attempt to open file
		std::ifstream iStream(t_filename, std::ios::in);

		if (iStream.is_open())
		{
			// Temporary vars
			int tempInt;
			std::string line;
			size_t newGenerationSize;

			// Read GA header line
			std::getline(iStream, line);
			// Read GA settings
			std::getline(iStream, line);
			std::vector<std::string> gaSettings = bif::Import::parseString(line, ",", bif::Import::CleanTokenBy::none);
			stringToNumber(gaSettings.at(0), m_currentGeneration);
			stringToNumber(gaSettings.at(1), newGenerationSize);
			stringToNumber(gaSettings.at(2), m_mutationCountMax);
			stringToNumber(gaSettings.at(3), m_mutationBitWidth);
			stringToNumber(gaSettings.at(4), m_mutationChanceIn100);
			stringToNumber(gaSettings.at(5), m_numEvolveCopy);
			stringToNumber(gaSettings.at(6), m_numEvolveShuffle);
			stringToNumber(gaSettings.at(7), m_numEvolveCrossover);
			stringToNumber(gaSettings.at(8), m_numEvolveMutate);
			stringToNumber(gaSettings.at(9), tempInt);
			m_mutationSelection = static_cast<MutationSelection>(tempInt);

			// Apply GA setting - Generation Size
			if (m_generationSize != newGenerationSize) {
				m_generationSize = newGenerationSize;
				m_chromo.resize(m_generationSize, new C(0, m_randomGenerator));
			}

			// Read blank line
			std::getline(iStream, line);

			// Read chromo header line
			std::getline(iStream, line);

			// Read each Chromo
			for (auto it{ std::begin(m_chromo) }; it != std::end(m_chromo); ++it)
			{
				// Read variables directly and limit
				(*it)->readFromFileAsCSV(iStream);
				(*it)->applyLimits();
				// Update serialized strings
				(*it)->serialize();
			}

			iStream.close();
		}
		else std::cout << "\nERROR: Unable to open file " + t_filename + "\n\n";
	}

	/**
	*	@brief  (Upcoming Feature!)
	*
	*	@param  t_competition is ...
	*	@return void
	*/
	template <typename C>
	void GeneticAlgorithm<C>::setCompetition(std::vector<C*> t_competition)
	{
		m_competition = t_competition;
	}

	/**
	*	@brief  Calls selection methods and ensures that there are enough valid Chromos for the rest of the generation.
	*	Runs fitness function on the Chromos to get their fitness scores
	*	and sorts them by those scores in order to mark the best scoring Chromos
	*	as "safe".
	*	
	*	"Safe" Chromos are not modified during the crossover and mutation phases,
	*	and are used as sources for other Chromos.
	*	
	*	Since we only want valid Chromos to be used as sources, this method also
	*	calls determineIdealSafeChromos(), which detects invalid "safe" Chromos
	*	and marks them as "unsafe".
	*
	*	@return true if there are enough "safe" Chromos, false otherwise
	*/
	template <typename C>
	bool GeneticAlgorithm<C>::runSelectionPhase(const bool showDebugMessages)
	{
		getFitnessScores();
		sortChromoByScore();

		// Calculate indices for crossover and mutation processes
		determineIdealSafeChromos();
		determineSafeChromos();
		determineEvolutionRanges();

		// Ensure there are at least 2 "safe" Chromos
		if (m_numEvolveSafe < 2) {
			std::cout << "\nERROR: Number of valid Chromos is less than 2!\n";
			std::cout << "Review initial conditions to insure they are valid according to the fitness function.\n";
			return false;
		}

		// Results are valid
		return true;
	}

	/**
	*	@brief  Serializes Chromo data and calls all crossover methods.
	*	Crossover methods are run on individual Chromos marked as "unsafe"
	*	due to having either invalid or low fitness scores
	*	Methods used:
	*		COPIES - Choose a random "safe" Chromo and copy data from it.
	*		SHUFFLES - Choose 2 random "safe" Chromos and randomly copy data from them.
	*		CROSSOVERS - Choose 2 random "safe" Chromos and uUse n-Split crossover
	*	method to copy data from them.
	*
	*	@return void
	*/
	template <typename C>
	void GeneticAlgorithm<C>::runCrossoverPhase(const bool showDebugMessages)
	{
		// Convert from variables to serialized strings
		serializeChromos();

		// Crossover phase
		doCopies();
		doShuffles();
		doCrossovers();
	}

	/**
	*	@brief  Mutates serialized data and calls any custom mutations.
	*	@note   The data should have already been serialized in the crossover
	*	phase.
	*
	*	Calls doMutations() to mutate the serialize data, then deserializes
	*	all Chromos. If applicable, runs custom mutations which are performed
	*	by the inherited Chromo class.
	*
	*	@return void
	*/
	template <typename C>
	void GeneticAlgorithm<C>::runMutationPhase(const bool showDebugMessages)
	{
		// Mutation phase (part 1)
		// Mutate the serialized data
		doMutations();
		if (showDebugMessages) {
			std::cout << "doMutation() complete...\n";
		}
		
		// Convert from serialized strings back to variables
		deserializeChromos();
		if (showDebugMessages) {
			std::cout << "deserializeChromos() complete...\n";
		}
		
		if (C::hasCustomMutations()) {
			// Mutation phase (part 2)
			// Mutate the variables directly
			doCustomMutations();

			// Update serialized string with the new mutations
			serializeChromos();
		}
	}

	/**
	*	@brief  Sets m_mutationSelection member variable
	*
	*	@param  t_mutationSelection is an enum setting that can put limitations on data mutation
	*	@return void
	*/
	template <typename C>
	void GeneticAlgorithm<C>::setMutationSelection(const MutationSelection t_mutationSelection)
	{
		m_mutationSelection = t_mutationSelection;
	}

	/**
	*	@brief  Sets m_mutationCountMax, the maximum number of mutations to any given Chromo during a generation
	*	If you want to disable mutations, use setNumberToMutate(0)
	*
	*	@param  t_mutationCountMax defines the new maximum mutation count. Must be at least 1.
	*	@return void
	*/
	template <typename C>
	void GeneticAlgorithm<C>::setMutationCountMax(const size_t t_mutationCountMax)
	{
		if (t_mutationCountMax >= 1) {
			m_mutationCountMax = t_mutationCountMax;
		}
		else {
			m_mutationCountMax = static_cast<size_t>(1);
			std::cout << "\nERROR: Mutation count max must be at least 1. Setting to default value of 1. If you want to disable mutations, please call setNumberToMutate(0);\n\n";
		}
	}

	/**
	*	@brief  Sets m_mutationBitWidth, the number of bits in mutation blocks.
	*
	*	@param  t_mutationBitWidth, must be at least 1.
	*	@return void
	*/
	template <typename C>
	void GeneticAlgorithm<C>::setMutationBitWidth(const size_t t_mutationBitWidth)
	{
		if (t_mutationBitWidth >= 1) {
			m_mutationBitWidth = t_mutationBitWidth;
		}
		else {
			m_mutationBitWidth = static_cast<size_t>(1);
			std::cout << "\nERROR: Mutation bit width must be at least 1. Setting to default value of 1.\n\n";
		}
	}

	/**
	*	@brief  Sets m_mutationChanceIn100, the chance of any bit within a mutation block being toggled
	*	If you want to disable mutations, use setNumberToMutate(0)
	*
	*	@param  t_mutationChanceIn100 is the new mutation chance, out of 100. Must be in the range [1, 100].
	*	@return void
	*/
	template <typename C>
	void GeneticAlgorithm<C>::setMutationChanceIn100(const short int t_mutationChanceIn100)
	{
		if (t_mutationChanceIn100 > 0 && t_mutationChanceIn100 <= 100) {
			m_mutationChanceIn100 = t_mutationChanceIn100;
		}
		else {
			if (t_mutationChanceIn100 == 0) {
				m_mutationChanceIn100 = 1;
				std::cout << "\nERROR: Mutation chance in 100 must be between 1 and 100, inclusive. Setting to 1.\n\n";
			} else {
				m_mutationChanceIn100 = 100;
				std::cout << "\nERROR: Mutation chance in 100 must be between 1 and 100, inclusive. Setting to 100.\n\n";
			}
			
		}
	}

	/**
	*	@brief  Sets m_numEvolveCopy, the number of Chromos to be directly copied in the crossover phase
	*	This function does not perform bounds checking, as that is done
	*	when advanceGeneration() is called.
	*
	*	@param  t_numCopy must be at least 0.
	*	@return void
	*/
	template <typename C>
	void GeneticAlgorithm<C>::setNumberToCopy(const size_t t_numCopy)
	{
		m_numEvolveCopy = t_numCopy;
	}

	/**
	*	@brief  Sets m_numEvolveShuffle, the number of Chromos to be shuffled from parents in the crossover phase
	*	This function does not perform bounds checking, as that is done
	*	when advanceGeneration() is called.
	*
	*	@param  t_numShuffle must be at least 0.
	*	@return void
	*/
	template <typename C>
	void GeneticAlgorithm<C>::setNumberToShuffle(const size_t t_numShuffle)
	{
		m_numEvolveShuffle = t_numShuffle;
	}

	/**
	*	@brief  Sets m_numEvolveCrossover, the number of Chromos to be combined from parents using n-Split crossovers.
	*	This function does not perform bounds checking, as that is done
	*	when advanceGeneration() is called.
	*
	*	@param  t_numCrossover must be at least 0.
	*	@return void
	*/
	template <typename C>
	void GeneticAlgorithm<C>::setNumberToCrossover(const size_t t_numCrossover)
	{
		m_numEvolveCrossover = t_numCrossover;
	}

	/**
	*	@brief  Sets m_numEvolveMutate, the number of Chromos to be mutated each generation.
	*	This function does not perform bounds checking, as that is done
	*	when advanceGeneration() is called.
	*
	*	@param  t_numMutate must be at least 0.
	*	@return void
	*/
	template <typename C>
	void GeneticAlgorithm<C>::setNumberToMutate(const size_t t_numMutate)
	{
		m_numEvolveMutate = t_numMutate;
	}

	/**
	*	@brief  Sets m_numCrossoverSplits, the number of splits used in n-split crossovers.
	*
	*	@param  t_numCrossoverSplits must be at least 1.
	*	@return void
	*/
	template <typename C>
	void GeneticAlgorithm<C>::setNumberOfCrossoverSplits(const size_t t_numCrossoverSplits)
	{
		if (t_numCrossoverSplits >= 1) {
			m_numCrossoverSplits = t_numCrossoverSplits;
		}
		else {
			m_numCrossoverSplits = static_cast<size_t>(1);
			std::cout << "\nERROR: Number of crossover splits must be at least 1. Setting to 1.\n\n";
		}
	}

	/**
	*	@brief  Utility function that ensures GA settings for crossovers and mutations are within the bounds of the generation size.
	*	Tests for minimum generation size, having at least 2 "safe" Chromos during
	*	the crossover phase, and having at least 2 Chromos not used in mutation phase.
	*
	*	@return true if there were no unresolvable issues, false otherwise
	*/
	template <typename C>
	bool GeneticAlgorithm<C>::areBoundsValid()
	{
		// Generation must have at least 3 Chromos
		// Due to needing 2 "safe" Chromos and at least 1 for the crossover and mutation phases
		if (m_generationSize < 3)
		{
			std::cout << "\nERROR: Generation size must be at least 3!\n\n";
			return false;
		}
		// This tests whether there are too many crossover processes (copies, shuffles, n-split crossovers)
		// for the generation size.
		// There must be at least 2 Chromos used as parents for the crossover phase,
		// and they must not be crossed over themselves.
		else if (m_numEvolveCopy + m_numEvolveShuffle + m_numEvolveCrossover + 2 > m_generationSize)
		{
			std::cout << "\nERROR: Generation must have at least 2 Chromosomes unused in crossover phase.\n";
			std::cout << "Increase generation size or reduce the number of copies/shuffles/crossovers.\n";
			return false;
		}
		// This limits the number of mutations to the generation size minus 2,
		// as at least 2 valid Chromos must be kept as parents for the crossover phase.
		else if (m_numEvolveMutate + 2 > m_generationSize)
		{
			// Limit number of mutations
			m_numEvolveMutate = m_generationSize - 2;
			std::cout << "\nWARNING: Generation must have at least 2 Chromosomes unused in mutation phase.\n";
			std::cout << "Reducing mutation count to " << m_numEvolveMutate << " to compensate.\n";
			return true;
		}
		// No problems detected
		return true;
	}

	/**
	*	@brief  Returns the current generation.
	*
	*	@return m_currentGeneration
	*/
	template <typename C>
	unsigned long int GeneticAlgorithm<C>::getGeneration() const
	{
		return m_currentGeneration;
	}

	/**
	*	@brief  A crossover method that replaces data in "unsafe" Chromos with direct copies of serialized data in "safe" Chromos.
	*
	*	@return void
	*/
	template <typename C>
	void GeneticAlgorithm<C>::doCopies()
	{
		// Calculate last ID, based on the user-specified number to copy
		// Chromos which would normally be "safe" yet have invalid scores are also copied
		const size_t lastIdToCopy{ m_firstIdEvolveCopy + m_numEvolveCopy + m_numEvolveCopyExtra };

		for (size_t i = m_firstIdEvolveCopy; i < lastIdToCopy; ++i)
		{
			// One parent is randomly chosen from best chromos
			size_t parentId{ pickRandomSafeChromo() };
			C* parent = m_chromo.at(parentId);

			// Replace unworthy chromo
			m_chromo.at(i)->copyParent(*parent);
		}
	}

	/**
	*	@brief  A crossover method that replaces data in "unsafe" Chromos with shuffled data from two random "safe" Chromos.
	*	Each byte in the serialized data is chosen randomly from the
	*	corresponding bytes of the parent Chromos.
	*
	*	@return void
	*/
	template <typename C>
	void GeneticAlgorithm<C>::doShuffles()
	{
		if (m_numEvolveShuffle > 0)
		{
			for (size_t i = m_firstIdEvolveShuffle; i < m_firstIdEvolveShuffle + m_numEvolveShuffle; ++i)
			{
				// Two parents are randomly chosen from best chromos
				size_t parentId1{ 0 };
				size_t parentId2{ 0 };
				getTwoUniqueRandomNumbers(parentId1, parentId2, static_cast<size_t>(0), m_numEvolveSafe, m_randomGenerator );

				// Replace unworthy chromo
				m_chromo.at(i)->shuffleFromParents(*m_chromo.at(parentId1), *m_chromo.at(parentId2));

			}
		}
	}

	/**
	*	@brief  A crossover method that replaces data in "unsafe" Chromos with data crossed over from two random "safe" Chromos.
	*	The default crossover used is n-Split, found in Chromo.h.
	*
	*	@return void
	*/
	template <typename C>
	void GeneticAlgorithm<C>::doCrossovers()
	{
		for (size_t i{ m_firstIdEvolveCrossover }; i < m_firstIdEvolveCrossover + m_numEvolveCrossover; ++i)
		{
			// Two parents are randomly chosen from best chromos
			size_t parentId1{ 0 };
			size_t parentId2{ 0 };
			pickTwoRandomSafeChromos(parentId1, parentId2);

			// Replace unworthy chromo
			m_chromo.at(i)->crossoverFromParents(*m_chromo.at(parentId1), *m_chromo.at(parentId2), m_numCrossoverSplits);
		}
	}

	/**
	*	@brief  Finds a random "unsafe" Chromo
	*
	*	@return Index of random "unsafe" Chromo
	*/
	template <typename C>
	size_t GeneticAlgorithm<C>::pickRandomUnsafeChromo()
	{
		return static_cast<size_t>(m_randomGenerator() % (m_generationSize - m_numEvolveSafe) + m_numEvolveSafe);
	}

	/**
	*	@brief  Finds a random "safe" Chromo
	*
	*	@return Index of random "safe" Chromo
	*/
	template <typename C>
	size_t GeneticAlgorithm<C>::pickRandomSafeChromo()
	{
		if (m_numEvolveSafe <= 1) {
			return static_cast<size_t>(0);
		}
		else {
			return static_cast<size_t>(m_randomGenerator() % m_numEvolveSafe);
		}
	}

	/**
	*	@brief  Finds two random "safe" Chromos. They will not have the same index.
	*
	*	@param  t_safeId1 will be set to the index of a random "safe" Chromo
	*	@param  t_safeId2 will be set to the index of a random "safe" Chromo
	*	@return void
	*/
	template <typename C>
	void GeneticAlgorithm<C>::pickTwoRandomSafeChromos(size_t& t_safeId1, size_t& t_safeId2)
	{
		if (m_numEvolveSafe <= 1) {
			t_safeId1 = 0;
			t_safeId2 = 0;
		}
		else {
			t_safeId1 = m_randomGenerator() % m_numEvolveSafe;
			t_safeId2 = t_safeId1;
			while (t_safeId1 == t_safeId2) {
				t_safeId2 = m_randomGenerator() % m_numEvolveSafe;
			}
		}
	}

	/**
	*	@brief  Chooses random Chromos to mutate and calls their mutate() method.
	*	Chromos will not be mutated twice.
	*
	*	@return void
	*/
	template <typename C>
	void GeneticAlgorithm<C>::doMutations()
	{
		std::vector<size_t> mutationList;
		if (m_numEvolveMutate < m_generationSize - m_numEvolveSafe) {
			// Pick random unsafe Chromos to mutate,
			// ensuring that none are picked twice
			getUniqueRandomNumbers(mutationList, m_numEvolveMutate, m_numEvolveSafe, m_generationSize, m_randomGenerator);
			for (size_t i{ 0 }; i < m_numEvolveMutate; ++i)
			{
				m_chromo.at(mutationList.at(i))->mutate(m_serialItemIndices, m_mutationLimits, m_mutationSelection,
					m_mutationCountMax, m_mutationBitWidth, m_mutationChanceIn100);
			}
		}
		else {
			// If there are not enough unsafe Chromos to choose from,
			// instead mutate all unsafe
			for (size_t i{ m_firstIdEvolveMutate }; i< m_generationSize; ++i)
			{
				m_chromo.at(i)->mutate(m_serialItemIndices, m_mutationLimits, m_mutationSelection,
					m_mutationCountMax, m_mutationBitWidth, m_mutationChanceIn100);
			}
		}
	}

	/**
	*	@brief  Iterates through all Chromos, calling their mutateCustom() method.
	*	This is mostly used for user-defined mutations such as adding/subtracting within
	*	a random range. 
	*	
	*	It is often more effective to mutate floats this way than to include them
	*	in the serialized mutation.
	*
	*	@return void
	*/
	template <typename C>
	void GeneticAlgorithm<C>::doCustomMutations()
	{
		// Call mutateCustom() for all unsafe chromos
		// Mutation chance is handled by mutateCustom()
		for (size_t i{ m_firstIdEvolveMutate }; i< m_generationSize; ++i)
		{
			m_chromo.at(i)->mutateCustom();
		}
	}

	/**
	*	@brief  Iterates through all Chromos, using their fitness function to set their scores.
	*
	*	@return void
	*/
	template <typename C>
	void GeneticAlgorithm<C>::getFitnessScores()
	{
		for (auto it{ std::begin(m_chromo) }; it != std::end(m_chromo); ++it)
		{
			// This function will set the Chromo score
			(*it)->runFitnessFunctionLocal();
		}
	}

	/**
	*	@brief  Runs a partial sort on Chromos to find the top scoring Chromos.
	*
	*	@return void
	*/
	template <typename C>
	void GeneticAlgorithm<C>::sortChromoByScore()
	{
		// Use only a partial sort because
		// low scoring Chromos will be overwritten
		// and their order does not matter.
		std::partial_sort(m_chromo.begin(), m_chromo.begin() + m_numIdealSafe, m_chromo.end(), [](C* x, C* y) { return x->getScore() > y->getScore(); });
	}

	/**
	*	@brief  Utility function that calculates the number of "best" Chromos to be marked as "safe"
	*	"Safe" Chromos are omitted from the crossover and mutation phases.
	*
	*	@return void
	*/
	template <typename C>
	void GeneticAlgorithm<C>::determineIdealSafeChromos()
	{
		// Bounds have already been checked in areBoundsValid(),
		// so this should return a positive number
		m_numIdealSafe = m_generationSize - m_numEvolveCopy - m_numEvolveShuffle - m_numEvolveCrossover;
	}

	/**
	*	@brief  Iterates through top-scoring Chromos, ensuring only valid Chromos are kept "safe".
	*	Invalid Chromos are no longer "safe", and will be copied from "safe" Chromos.
	*
	*	@return void
	*/
	template <typename C>
	void GeneticAlgorithm<C>::determineSafeChromos()
	{
		// Ensure that all safe chromos are valid
		m_numEvolveSafe = static_cast<size_t>(0);
		for (size_t i{ m_numIdealSafe }; i --> 0; )
		{
			std::cout << " " << i << " ";
			if (m_chromo.at(i)->getScore() > 0)
			{
				m_numEvolveSafe = i + 1;
				break;
			}
		}
		
		// Copy chromos determined as unsafe
		m_numEvolveCopyExtra = m_numIdealSafe - m_numEvolveSafe;
	}

	/**
	*	@brief  Sets indices for the crossover and mutation methods using GA settings and the result of determineSafeChromos().
	*
	*	@return void
	*/
	template <typename C>
	void GeneticAlgorithm<C>::determineEvolutionRanges()
	{
		// Unsafe chromos are split up into different crossover groups
		m_firstIdEvolveCopy = m_numEvolveSafe;
		m_firstIdEvolveShuffle = m_firstIdEvolveCopy + m_numEvolveCopy + m_numEvolveCopyExtra;
		m_firstIdEvolveCrossover = m_firstIdEvolveShuffle + m_numEvolveShuffle;

		// Mutation is allowed on every chromo that is unsafe
		m_firstIdEvolveMutate = m_numEvolveSafe;
	}

	/**
	*	@brief  Iterates through all Chromos, calling their serialize() method.
	*
	*	@return void
	*/
	template <typename C>
	void GeneticAlgorithm<C>::serializeChromos()
	{
		for (size_t i{ 0 }; i < m_generationSize; ++i)
		{
			m_chromo.at(i)->serialize();
		}
	}

	/**
	*	@brief  Iterates through all Chromos, calling their deserialize() method.
	*
	*	@return void
	*/
	template <typename C>
	void GeneticAlgorithm<C>::deserializeChromos()
	{
		for (size_t i{ m_numEvolveSafe }; i < m_generationSize; ++i)
		{
			m_chromo.at(i)->deserialize();
			m_chromo.at(i)->applyLimits();
		}
	}

} // namespace ga

#endif	// GENETIC_ALGORITHM_H_