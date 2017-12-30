/**
* @class ChromoSerialization.h
* @author Bryan Franz
* @date December 26, 2017
* @brief Contains static functions for working with serialized data
* Includes:
*   - Conversion to/from primitives
*   - Conversion to/from vectors
*   - Crossover methods
*   - Mutation
*
* @see (link to GitHub)
*/

#ifndef CHROMO_SERIALIZATION_H_
#define CHROMO_SERIALIZATION_H_

#ifdef _MSC_VER
#pragma once
#endif	// _MSC_VER

#include <iostream>
#include <vector>
#include <string>

#include "GeneticAlgorithmTechniques.h"

namespace ga
{
	/**
	*	@brief  Takes in a value and returns a short string containing the serialized value
	*	The length of the string is the number of bytes in the primitive
	*
	*	@param  t_data specifies the value to be converted
	*	@return string containing the serialized data
	*/
	template <typename T>
	static std::string serializePrimitive(T t_data)
	{
		string serializedValue = "";
		serializedValue.reserve(sizeof(T) + 1);

		T* pointerToData = &t_data;
		char *charPointer = reinterpret_cast<char*>(pointerToData);
		for (size_t i{ 0 }; i < sizeof(T); ++i)
		{
			char ch = *charPointer;
			serializedValue += ch;
			charPointer++;
		}
		return serializedValue;
	}

	/**
	*	@brief  Modifies a primitive to be the value of a serialized string
	*
	*	@param  t_data specifies the value to be modified
	*   @param  t_serialized is the serialized value
	*	@return void
	*/
	template <typename T>
	static size_t deserializePrimitive(T& t_data, std::string& t_serialized)
	{
		// First, convert to a character array
		char* serializedChars = new char[sizeof(T)];
		for (size_t i{ 0 }; i < sizeof(T); ++i) {
			serializedChars[i] = t_serialized.at(i);
		}

		T number = *((T*)serializedChars);
		t_data = number;
		free(serializedChars);
		return sizeof(T);
	}

	/**
	*	@brief  Takes in a vector and returns a short string containing the serialized vector
	*	The length of the string is the number of bytes in the primitive multiplied by the size of the vector
	*
	*	@param  t_vec specifies the value to be converted
	*	@return string containing the serialized vector
	*/
	template <typename T>
	static std::string serializeVector(std::vector<T>& t_vec)
	{
		std::string serializedVector = "";
		serializedVector.reserve(sizeof(T)  * t_vec.size() + 1);

		for (size_t i{ 0 }; i < t_vec.size(); ++i)
		{
			T* pointerToData = &t_vec.at(i);
			char *charPointer = reinterpret_cast<char*>(pointerToData);
			for (size_t ii = 0; ii < sizeof(T); ++ii)
			{
				char ch = *charPointer;
				serializedVector += ch;
				charPointer++;
			}
		}

		return serializedVector;
	}

	/**
	*	@brief  Modifies a vector to pull values from a serialized string
	*
	*	@param  t_vec specifies the vector to be modified
	*   @param  t_serialized is the serialized value
	*   @param  t_strFirstIndex is the location where the vector data begins
	*	@return size_t indicating where the next data in the serialized string is location
	*/
	template <typename T>
	static size_t deserializeVector(std::vector<T>& t_vec, const std::string& t_serialized, const size_t t_strFirstIndex)
	{
		// Character buffer for each value
		char* buffer = new char[sizeof(T)];

		// Iterate through each item
		for (size_t i{ 0 }; i < t_vec.size(); ++i)
		{
			// Items can be contained within multiple characters
			// So we iterate through each character
			for (size_t ii{ 0 }; ii < sizeof(T); ++ii) {
				buffer[ii] = t_serialized.at(t_strFirstIndex + i * sizeof(T) + ii);
			}

			// Get value from buffer
			T number = *((T*)buffer);
			t_vec.at(i) = number;
		}

		// Cleanup
		free(buffer);

		// Return final index so that more data can be read
		return (t_strFirstIndex + sizeof(T) * t_vec.size());
	}

	/**
	*	@brief  Takes in a bool vector and returns a short string containing the serialized vector
	*
	*	@param  t_vec specifies the value to be converted
	*	@return string containing the serialized vector
	*/
	static std::string serializeBoolVector(std::vector<bool>& t_vec)
	{
		std::string serializedVector = "";
		serializedVector.reserve(t_vec.size() / 8 + 2);
		
		size_t byteId{ 0 };
		size_t bitId{ 0 };
		char curChar{ 0x00 };
		for (size_t i{ 0 }; i < t_vec.size(); ++i)
		{
			// Byte defaults to 00000000
			// Set bit if boolean == True
			if (t_vec.at(i)) {
				curChar |= (1 << bitId);
			}
			
			// Increment
			++bitId;
			if (bitId >= 8) {
				bitId = 0;
				serializedVector += curChar;
				++byteId;
			}
		}
		// If we were in the middle of a byte, write it to the string
		if (bitId > 0) {
			serializedVector += curChar;
		}

		return serializedVector;
	}

	/**
	*	@brief  Modifies a bool vector to pull values from a serialized string
	*
	*	@param  t_vec specifies the vector to be modified
	*   @param  t_serialized is the serialized value
	*   @param  t_strFirstIndex is the location where the vector data begins
	*	@return size_t indicating where the next data in the serialized string is location
	*/
	static size_t deserializeBoolVector(std::vector<bool>& t_vec, std::string& t_serialized, const size_t t_strFirstIndex)
	{
		size_t bytesUsed;
		if (t_vec.size() % 8 == 0) {
			bytesUsed = t_vec.size() / 8;
		}
		else {
			bytesUsed = t_vec.size() / 8 + 1;
		}

		size_t byteId { t_strFirstIndex };
		size_t bitId { 0 };
		size_t vectorSize{ t_vec.size() };
		char buffer { t_serialized.begin()[byteId] };
		for (size_t i{ 0 }; i < vectorSize; ++i)
		{
			// Read bit as boolean
			t_vec.at(i) = (buffer >> bitId) & 0x1;
			
			// Iterate
			if (++bitId >= 8 && i < vectorSize - 1) {
				bitId = 0;
				buffer = t_serialized.begin()[++byteId];
			}
		}

		return (t_strFirstIndex + bytesUsed);
	}

	/**
	*	@brief  Returns a string containing shuffled bytes from two source strings
	*	This is used during the crossover phase of the GA
	*
	*	@param  t_source1 specifies the 1st source string
	*	@param  t_source2 specifies the 2nd source string
	*   @param  t_randomGenerator is the random number generator to use
	*	@return a shuffled string
	*/
	static std::string shuffleSerialized(std::string const& t_source1, std::string const& t_source2, int(*t_randomGenerator)(void))
	{
		if (t_source1.length() != t_source2.length())
		{
			// Error: One of parents was not serialized correctly
			std::cout << "\nSERIAL ERROR!\n";
			return "";
		}
		std::string shuffledString = "";
		shuffledString.reserve(t_source1.length());

		for (size_t i{ 0 }; i < t_source1.length(); ++i)
		{
			if (t_randomGenerator() % 2) {
				shuffledString += t_source1.at(i);
			}
			else {
				shuffledString += t_source2.at(i);
			}

		}

		return shuffledString;
	}

	/**
	*	@brief  Returns a string containing bytes from two source strings, split into n-partitions
	*	This is used during the crossover phase of the GA
	*
	*	@param  t_source1 specifies the 1st source string
	*	@param  t_source2 specifies the 2nd source string
	*	@param  t_splits specifies the number of times to alternate between source strings
	*   @param  t_randomGenerator is the random number generator to use
	*	@return a shuffled string
	*/
	static std::string nSplitSerialized(std::string const& t_source1, std::string const& t_source2, size_t t_splits, int(*t_randomGenerator)(void))
	{
		if (t_source1.length() != t_source2.length())
		{
			// Error: One of parents was not serialized correctly
			std::cout << "\nSERIAL ERROR!\n";
			return "";
		}
		std::string result = "";
		result.reserve(t_source1.length());

		// Split locations refer to the beginning of each partition
		// The first location is always set to be 0
		std::vector<size_t> splitLocations{0};
		size_t size1{ static_cast<size_t>(1) };
		getUniqueRandomNumbers(splitLocations, t_splits - size1, size1, t_source1.length() - size1, t_randomGenerator);

		short int useStringNow{ t_randomGenerator() % 2 };
		size_t locationNow{ 0 };
		for (size_t i{ 0 }; i < splitLocations.size(); ++i)
		{
			size_t partitionSize { (i != splitLocations.size() - size1) ? splitLocations.at(i + size1) - i : t_source1.length() - i };
			result += useStringNow ? t_source1.substr(locationNow, partitionSize) : t_source2.substr(locationNow, partitionSize);

			useStringNow = (useStringNow + 1) % 2;
			locationNow += partitionSize;
		}

		return result;
	}

	static void mutateRandomBits(std::string& t_serial, std::vector<SerialPartition>& t_serialIndices, const MutationLimits t_mutationLimits,
		const MutationSelection t_mutationSelection, const size_t t_mutationCount, const size_t t_mutationBitWidth,
		const short int t_mutationChanceIn100, int(*t_randomGenerator)(void))
	{
		for (size_t nMutate{ 0 }; nMutate < t_mutationCount; ++nMutate)
		{
			// Select start location and range for mutation
			size_t byteId{ 0 };
			short int bitId{ 0 };
			size_t bitsLeft{ t_mutationBitWidth };
			SerialPartitionType partitionType{ SerialPartitionType::normal };
			
			// Apply MutationSelection settings
			if (t_mutationSelection == MutationSelection::entirePartition) {
				size_t partitionId{ t_randomGenerator() % t_mutationLimits.partitions };
				
				byteId = t_serialIndices.at(partitionId).location;
				bitsLeft = t_serialIndices.at(partitionId).bytes * 8;
				partitionType = t_serialIndices.at(partitionId).type;
			}
			else if (t_mutationSelection == MutationSelection::pureRandom) {
				byteId = t_randomGenerator() % t_mutationLimits.bytes;
				bitId = t_randomGenerator() % 8;
			}
			else if (t_mutationSelection == MutationSelection::randomByte) {
				byteId = t_randomGenerator() % t_mutationLimits.bytes;
			}

			if (partitionType == SerialPartitionType::normal)
			{
				// Modify sequence of bits, up to t_mutationWidth
				char byte{ t_serial.begin()[byteId] };
				while (bitsLeft > 0)
				{
					// Random selection for bits to toggle
					if (t_randomGenerator() % 100 < t_mutationChanceIn100) {
						// Toggle bit
						byte ^= (1 << bitId);
					}

					// Advance to next bit
					++bitId;
					// Advance to next byte
					if (bitId >= 8) {
						// Return mutated byte to t_serial
						t_serial.begin()[byteId] = byte;

						// Move to next byte, if possible
						if (byteId + 1 < t_mutationLimits.bytes) {
							bitId -= 8;
							++byteId;

							byte = t_serial.begin()[byteId];
						}
						else {
							// Exit loop because there are no more possible bits to modify
							break;
						}
					}

					--bitsLeft;
				}
				// Return mutated byte to t_serial
				t_serial.begin()[byteId] = byte;
			}
			else if (partitionType == SerialPartitionType::eachBitUnique)
			{
				// Special handling for boolean partitions
				// As the intent of EntirePartition mode is to
				// modify only specific values

				// Boolean partitions contain many booleans,
				// so we select only one random bit and modify it
				size_t selectByte{ byteId + t_randomGenerator() % (bitsLeft / 8) };
				size_t selectBit{ t_randomGenerator() % static_cast<size_t>(8) };

				// Toggle random bit
				selectByte = byteId;
				char byte{ t_serial.begin()[selectByte] };
				byte ^= (1 << selectBit);

				// Return mutated byte to t_serial
				t_serial.begin()[selectByte] = byte;
			}
		}
	}

}

#endif	// CHROMO_SERIALIZATION_H_