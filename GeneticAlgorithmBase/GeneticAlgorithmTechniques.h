#ifndef GENETIC_ALGORITHM_TECHNIQUES_H_
#define GENETIC_ALGORITHM_TECHNIQUES_H_

#ifdef _MSC_VER
#pragma once
#endif	// _MSC_VER

#include <iostream>
#include <vector>
#include <string>
#include <sstream>

namespace ga
{
	enum class MutationSelection { pureRandom, randomByte, entirePartition };
	enum class MutationMode { value, byte, bit };
	enum class MutationTechnique { randomize, offset };
	enum class ShuffleMode { value, byte };
	enum class EncodedPartitionType { normal, eachBitUnique };

	struct EncodedPartition
	{
	public:
		std::string name;
		std::size_t location;
		std::size_t bytes;
		EncodedPartitionType type;
		std::size_t uniqueBits;
		EncodedPartition()
			: location(0),
			bytes(0)
		{};
		EncodedPartition(const std::string t_name, const std::size_t t_location, const std::size_t t_bytes, const EncodedPartitionType t_EncodedPartitionType, const std::size_t t_uniqueBits = 0)
			: name(t_name),
			location(t_location),
			bytes(t_bytes),
			type(t_EncodedPartitionType),
			uniqueBits(t_uniqueBits)
		{};
	};

	struct MutationLimits
	{
	public:
		std::size_t bytes;
		std::size_t partitions;
		MutationLimits()
			: bytes(0), partitions(0) {};
		MutationLimits(const std::size_t t_bytes, const std::size_t t_partitions)
			: bytes(t_bytes), partitions(t_partitions) {};
	};

	/**
	*	@brief  Runs a series of tests on a GA. Works best with ChromoTestFeatures.
	*
	*	@return void
	*/
	template <typename C>
	static void runTestGenerations(int(*t_randomGenerator)(void))
	{
		std::cout << "Beginning GA tests...\n";
		ga::GeneticAlgorithm<C>* gaTest = new ga::GeneticAlgorithm<C>("Test run", 12, 1, t_randomGenerator);
		std::cout << "\nTesting encoding...\n";
		gaTest->setNumberToCopy(1);
		gaTest->setNumberToShuffle(0);
		gaTest->setNumberToCrossover(0);
		gaTest->setNumberToMutate(0);
		gaTest->advanceGeneration();
		std::cout << *gaTest;

		std::cout << "\nAll values should be copied below!\n";
		for (int i{ 0 }; i < 4; ++i) {
			gaTest->advanceGeneration();
			gaTest->encodeChromos();
			gaTest->decodeChromos();
			std::cout << *gaTest;
		}
		
		std::cout << "\n\n\nTesting crossover phase...\n";
		gaTest->setNumberToCopy(3);
		gaTest->setNumberToShuffle(3);
		gaTest->setNumberToCrossover(3);
		gaTest->setNumberToMutate(0);
		
		for (int g{ 0 }; g < 2; g++) {
			gaTest->advanceGeneration();
			cout << *gaTest;
			cout << "\n\n";
		}

		std::cout << "\n\n\nTesting mutation phase - entire partition...\n";
		
		gaTest->setMutationSelection(ga::MutationSelection::entirePartition);
		gaTest->setNumberToCopy(10);
		gaTest->setNumberToShuffle(0);
		gaTest->setNumberToCrossover(0);
		gaTest->setNumberToMutate(10);
		gaTest->setMutationCountMax(1);
		gaTest->setMutationChanceIn100(50);

		std::cout << "Only 1 value should change...\n";
		for (int g{ 0 }; g < 2; g++) {
			gaTest->advanceGeneration();
			cout << *gaTest;
			cout << "\n\n";
		}
		
		std::cout << "\n\n\nTesting mutation phase - random byte...\n";
		gaTest->setMutationSelection(ga::MutationSelection::randomByte);
		gaTest->setMutationCountMax(1);
		gaTest->setMutationBitWidth(8);
		gaTest->setMutationChanceIn100(50);
		for (int g{ 0 }; g < 2; g++) {
			gaTest->advanceGeneration();
			cout << *gaTest;
			cout << "\n\n";
		}

		std::cout << "\n\n\nTesting mutation phase - random bit...\n";
		gaTest->setMutationSelection(ga::MutationSelection::pureRandom);
		gaTest->setMutationCountMax(1);
		gaTest->setMutationBitWidth(1);
		gaTest->setMutationChanceIn100(100);
		for (int g{ 0 }; g < 2; g++) {
			gaTest->advanceGeneration();
			cout << *gaTest;
			cout << "\n\n";
		}

		cout << "Calling destructor for GA...\n";
		delete gaTest;
		cout << "Destructor successful!\n\n";
	}

	/**
	*	@brief  Iterates through a vector, limiting the items
	*
	*	@param  t_vec specifies the vector to modify
	*	@param  t_min specifies the minimum value to allow
	*	@param  t_max specifies the maximum value to allow
	*	@return void
	*/
	template <typename T>
	static void limitVector(std::vector<T>& t_vec, T const t_min, T const t_max)
	{
		for (std::size_t i{ 0 }; i < t_vec.size(); ++i)
		{
			if (t_vec.at(i) < t_min) {
				t_vec.at(i) = t_min;
			}
			else if (t_vec.at(i) > t_max) {
				t_vec.at(i) = t_max;
			}
		}
	}

	/**
	*	@brief  Iterates through a vector, limiting the items to a range via modulus
	*	This is useful for data that should be limited to a
	*	range without having the min and max values be
	*	overrepresented.
	*
	*	@param  t_vec specifies the vector to modify
	*	@param  t_min specifies the minimum value to allow
	*	@param  t_max specifies the maximum value to allow
	*	@return void
	*/
	template <typename T>
	static void limitVectorModulo(std::vector<T>& t_vec, T const t_min, T const t_max)
	{
		const T range{ t_max - t_min + static_cast<T>(1) };
		for (std::size_t i{ 0 }; i < t_vec.size(); ++i)
		{
			const T relativeVal = t_vec.at(i) - t_min;
			T newVal = (relativeVal + range) % range + t_min;

			if (newVal < t_min) {
				newVal += range;
			}

			t_vec.at(i) = newVal;
		}
	}

	/**
	*	@brief  Iterates through a vector of singles or doubles, limiting the items to a range via modulus
	*	This is useful for data that should be limited to a
	*	range without having the min and max values be
	*	overrepresented.
	*
	*	@param  t_vec specifies the vector to modify
	*	@param  t_min specifies the minimum value to allow
	*	@param  t_max specifies the maximum value to allow
	*	@return void
	*/
	template <typename T>
	static void limitVectorModuloDouble(std::vector<T>& t_vec, double const t_min, double const t_max)
	{
		const double range{ t_max - t_min + static_cast<T>(1) };
		for (std::size_t i{ 0 }; i < t_vec.size(); ++i)
		{
			const double relativeVal = static_cast<double>(t_vec.at(i)) - t_min;
			double newVal = fmod(relativeVal + range, range) + t_min;

			if (newVal < t_min) {
				newVal += range;
			}

			t_vec.at(i) = static_cast<T>(newVal);
		}
	}

	/**
	*	@brief  Fills a vector with random unique numbers according to Knuth algorithm
	*
	*	@param  t_result specifies the output vector
	*	@param  t_randomIndexCount specifies the number of random numbers to generate
	*	@param  t_min specifies the minimum value to allow
	*	@param  t_max specifies the maximum value to allow
	*	@param  t_randomGenerator specifies the random number generator
	*	@return void
	*/
	template <typename T>
	static void getUniqueRandomNumbers(std::vector<T>& t_result, std::size_t t_randomIndexCount, const T t_min, const T t_max, int(*t_randomGenerator)(void))
	{
		// Using Knuth algorithm
		// Complexity: O(range) = O(N)
		const T range{ t_max - t_min };

		if (t_randomIndexCount == 1) {
			// Special case where only 1 number is required
			t_result.push_back(t_randomGenerator() % range + t_min);
		}
		else {
			
			std::size_t numbersNeeded{ t_randomIndexCount };

			// Iterate through all possible numbers
			for (std::size_t i{ 0 }; i < range; ++i) {
				std::size_t numbersLeft = range - i;
				// Probability = numbersNeeded / numbersLeft
				if (t_randomGenerator() % numbersLeft < numbersNeeded) {
					t_result.push_back(i + t_min);
					--numbersNeeded;
					if (numbersNeeded == 0) {
						break;
					}
				}
			}
		}
	}

	/**
	*	@brief  Finds two random unique numbers
	*
	*	@param  t_result1 specifies the 1st random number
	*	@param  t_result1 specifies the 2nd random number
	*	@param  t_min specifies the minimum value to allow
	*	@param  t_max specifies the maximum value to allow
	*	@param  t_randomGenerator specifies the random number generator
	*	@return void
	*/
	template <typename T>
	static void getTwoUniqueRandomNumbers(T& t_result1, T& t_result2, T t_min, T t_max, int(*t_randomGenerator)(void))
	{
		if (t_max == t_min) {
			// Special case where only 1 number is available
			t_result1 = t_min;
			t_result2 = t_min;
		}
		else {
			const T range{ t_max - t_min };

			// Assign 1st number
			t_result1 = t_randomGenerator() % range + t_min;

			// 2nd number has (range - 1) possibilities
			// Increment if result >= t_result1
			// This removes t_result1 from possible results,
			// while keeping other possibilities at an equal probability
			
			t_result2 = t_randomGenerator() % (range - 1) + t_min;
			if (t_result2 >= t_result1) {
				++t_result2;
			}
		}
	}

	/**
	*	@brief  Overloaded << operator for vectors
	*
	*	@param  t_oStream specifies the output stream to write to
	*	@param  t_vec specifies the vector to write
	*	@return output stream
	*/
	template <typename T>
	static std::ostream& operator << (std::ostream& t_oStream, const std::vector<T>& t_vec)
	{
		t_oStream << "<";
		bool isFirst = true;
		for (typename std::vector<T>::const_iterator ii = t_vec.begin(); ii != t_vec.end(); ++ii)
		{
			if (!isFirst) {
				t_oStream << ", ";
			}
			t_oStream << *ii;
			isFirst = false;
		}
		t_oStream << ">";
		return t_oStream;
	}

	/**
	*	@brief  Converts a string to a number of a given type
	*
	*	@param  t_str specifies the string to convert
	*	@param  t_num specifies the number to modify
	*	@return void
	*/
	template <typename T>
	static void stringToNumber(std::string t_str, T& t_num)
	{
		// Very inefficient method for converting a string to a number
		// TODO: Replace with something better
		std::stringstream soSlow(t_str);
		soSlow >> t_num;
	}

	/**
	*	@brief  Constrains a value between a minimum and maximum
	*
	*	@param  t_value specifies the input value
	*	@param  t_min specifies the minimum value to allow
	*	@param  t_max specifies the maximum value to allow
	*	@return constrained value
	*/
	template <typename T>
	inline T clamp(const T t_value, const T t_min, const T t_max)
	{
		return t_value < t_min ? t_min : (t_value > t_max ? t_max : t_value);
	}

	/**
	*	@brief  Returns a random double in a range
	*
	*	@param  t_min specifies the minimum value to allow
	*	@param  t_max specifies the maximum value to allow
	*	@param  t_randomGenerator specifies the random number generator
	*	@return random double within the range
	*/
	inline double randomRangeDouble(const double t_min, const double t_max, int(*t_randomGenerator)(void))
	{
		return static_cast<double>(t_randomGenerator()) / RAND_MAX * (t_max - t_min) + t_min;
	}

	/**
	*	@brief  Returns a random number in a range
	*
	*	@param  t_min specifies the minimum value to allow
	*	@param  t_max specifies the maximum value to allow
	*	@param  t_randomGenerator specifies the random number generator
	*	@return random number within the range
	*/
	template <typename T>
	inline T randomRange(const T t_min, const T t_max, int(*t_randomGenerator)(void))
	{
		return static_cast<T>(t_randomGenerator()) % (t_max - t_min) + t_min;
	}

} // namespace ga

#endif	// GENETIC_ALGORITHM_TECHNIQUES_H_