/**
* @class ChromoSerialization.h
* @author Bryan Franz
* @date December 26, 2017
* @brief Contains static functions for working with encoded data
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
	*	@brief  Takes in a value and returns a short string containing the encoded value
	*	The length of the string is the number of bytes in the primitive
	*
	*	@param  t_data specifies the value to be converted
	*	@return string containing the encoded data
	*/
	template <typename T>
	static std::string encodePrimitive(T t_data)
	{
		std::string encoded = "";
		encoded.reserve(sizeof(T) + 1);

		T* pointerToData = &t_data;
		char *charPointer = reinterpret_cast<char*>(pointerToData);
		for (std::size_t i{ 0 }; i < sizeof(T); ++i)
		{
			char ch = *charPointer;
			encoded += ch;
			charPointer++;
		}
		return encoded;
	}

	/**
	*	@brief  Modifies a primitive to be the value of an encoded string
	*
	*	@param  t_data specifies the value to be modified
	*   @param  t_encoded is the encoded value
	*	@return void
	*/
	template <typename T>
	static std::size_t decodePrimitive(T& t_data, std::string& t_encoded)
	{
		// First, convert to a character array
		char* encodedChars = new char[sizeof(T)];
		for (std::size_t i{ 0 }; i < sizeof(T); ++i) {
			encodedChars[i] = t_encoded.at(i);
		}

		T number = *((T*)encodedChars);
		t_data = number;
		free(encodedChars);
		return sizeof(T);
	}

	/**
	*	@brief  Takes in a vector and returns a short string containing the encoded vector
	*	The length of the string is the number of bytes in the primitive multiplied by the size of the vector
	*
	*	@param  t_vec specifies the value to be converted
	*	@return string containing the encoded vector
	*/
	template <typename T>
	static std::string encodeVector(std::vector<T>& t_vec)
	{
		std::string encodedVector = "";
		const std::size_t n{ t_vec.size() };
		encodedVector.reserve(sizeof(T) * n + 1);

		for (std::size_t i{ 0 }; i < n; ++i)
		{
			T* pointerToData = &t_vec.at(i);
			char *charPointer = reinterpret_cast<char*>(pointerToData);
			for (std::size_t ii = 0; ii < sizeof(T); ++ii)
			{
				char ch = *charPointer;
				encodedVector += ch;
				charPointer++;
			}
		}

		return encodedVector;
	}

	/**
	*	@brief  Modifies a vector to pull values from a encoded string
	*
	*	@param  t_vec specifies the vector to be modified
	*   @param  t_encoded is the encoded value
	*   @param  t_strFirstIndex is the location where the vector data begins
	*	@return std::size_t indicating where the next data in the encoded string is location
	*/
	template <typename T>
	static std::size_t decodeVector(std::vector<T>& t_vec, const std::string& t_encoded, const std::size_t t_strFirstIndex)
	{
		// Character buffer for each value
		char* buffer = new char[sizeof(T)];

		// Iterate through each item
		const std::size_t n { t_vec.size() };
		for (std::size_t i{ 0 }; i < n; ++i)
		{
			// Items can be contained within multiple characters
			// So we iterate through each character
			for (std::size_t ii{ 0 }; ii < sizeof(T); ++ii) {
				buffer[ii] = t_encoded.at(t_strFirstIndex + i * sizeof(T) + ii);
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
	*	@brief  Takes in a bool vector and returns a short string containing the encoded vector
	*
	*	@param  t_vec specifies the value to be converted
	*	@return string containing the encoded vector
	*/
	static std::string encodeBoolVector(std::vector<bool>& t_vec)
	{
		const std::size_t n{ t_vec.size() };
		std::string encodedVector = "";
		encodedVector.reserve(n / 8 + 2);
		
		std::size_t byteId{ 0 };
		std::size_t bitId{ 0 };
		char curChar{ 0x00 };
		for (std::size_t i{ 0 }; i < n; ++i)
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
				encodedVector += curChar;
				++byteId;
			}
		}
		// If we were in the middle of a byte, write it to the string
		if (bitId > 0) {
			encodedVector += curChar;
		}

		return encodedVector;
	}

	/**
	*	@brief  Modifies a bool vector to pull values from an encoded string
	*
	*	@param  t_vec specifies the vector to be modified
	*   @param  t_encoded is the encoded value
	*   @param  t_strFirstIndex is the location where the vector data begins
	*	@return std::size_t indicating where the next data in the encoded string is location
	*/
	static std::size_t decodeBoolVector(std::vector<bool>& t_vec, std::string& t_encoded, const std::size_t t_strFirstIndex)
	{
		std::size_t bytesUsed;
		if (t_vec.size() % 8 == 0) {
			bytesUsed = t_vec.size() / 8;
		}
		else {
			bytesUsed = t_vec.size() / 8 + 1;
		}

		std::size_t byteId { t_strFirstIndex };
		std::size_t bitId { 0 };
		std::size_t vectorSize{ t_vec.size() };
		char buffer { t_encoded.begin()[byteId] };
		for (std::size_t i{ 0 }; i < vectorSize; ++i)
		{
			// Read bit as boolean
			t_vec.at(i) = (buffer >> bitId) & 0x1;
			
			// Iterate
			if (++bitId >= 8 && i < vectorSize - 1) {
				bitId = 0;
				buffer = t_encoded.begin()[++byteId];
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
	static std::string shuffleEncodedData(std::string const& t_source1, std::string const& t_source2, int(*t_randomGenerator)(void))
	{
		if (t_source1.length() != t_source2.length())
		{
			// Error: One of parents was not encoded correctly
			std::cout << "\nENCODING ERROR!\n";
			return "";
		}

		const std::size_t n{ t_source1.length() };
		std::string shuffledString = "";
		shuffledString.reserve(n);

		for (std::size_t i{ 0 }; i < n; ++i)
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
	static std::string nSplitEncodedData(std::string const& t_source1, std::string const& t_source2, std::size_t t_splits, int(*t_randomGenerator)(void))
	{
		if (t_source1.length() != t_source2.length())
		{
			// Error: One of parents was not encoded correctly
			std::cout << "\nENCODING ERROR!\n";
			return "";
		}
		std::string result = "";
		result.reserve(t_source1.length());

		// Split locations refer to the beginning of each partition
		// The first location is always set to be 0
		std::vector<std::size_t> splitLocations{0};
		std::size_t size1{ 1 };
		getUniqueRandomNumbers(splitLocations, t_splits - size1, size1, t_source1.length() - size1, t_randomGenerator);

		short int useStringNow{ t_randomGenerator() % 2 };
		std::size_t locationNow{ 0 };
		const std::size_t n{ splitLocations.size() };
		for (std::size_t i{ 0 }; i < n; ++i)
		{
			std::size_t partitionSize { (i != n - size1) ? splitLocations.at(i + size1) - i : t_source1.length() - i };
			result += useStringNow ? t_source1.substr(locationNow, partitionSize) : t_source2.substr(locationNow, partitionSize);

			useStringNow = (useStringNow + 1) % 2;
			locationNow += partitionSize;
		}

		return result;
	}

	static void mutateRandomBits(std::string& t_encoded, std::vector<EncodedPartition>& t_encodedPartitions, const MutationLimits t_mutationLimits,
		const MutationSelection t_mutationSelection, const std::size_t t_mutationCount, const std::size_t t_mutationBitWidth,
		const short int t_mutationChanceIn100, int(*t_randomGenerator)(void))
	{
		for (std::size_t nMutate{ 0 }; nMutate < t_mutationCount; ++nMutate)
		{
			// Select start location and range for mutation
			std::size_t byteId{ 0 };
			short int bitId{ 0 };
			std::size_t bitsLeft{ t_mutationBitWidth };
			EncodedPartitionType partitionType{ EncodedPartitionType::normal };
			
			// Apply MutationSelection settings
			if (t_mutationSelection == MutationSelection::entirePartition) {
				std::size_t partitionId{ t_randomGenerator() % t_mutationLimits.partitions };
				
				byteId = t_encodedPartitions.at(partitionId).location;
				bitsLeft = t_encodedPartitions.at(partitionId).bytes * 8;
				partitionType = t_encodedPartitions.at(partitionId).type;
			}
			else if (t_mutationSelection == MutationSelection::pureRandom) {
				byteId = t_randomGenerator() % t_mutationLimits.bytes;
				bitId = t_randomGenerator() % 8;
			}
			else if (t_mutationSelection == MutationSelection::randomByte) {
				byteId = t_randomGenerator() % t_mutationLimits.bytes;
			}

			if (partitionType == EncodedPartitionType::normal)
			{
				// Modify sequence of bits, up to t_mutationWidth
				char byte{ t_encoded.begin()[byteId] };
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
						// Return mutated byte
						t_encoded.begin()[byteId] = byte;

						// Move to next byte, if possible
						if (byteId + 1 < t_mutationLimits.bytes) {
							bitId -= 8;
							++byteId;

							byte = t_encoded.begin()[byteId];
						}
						else {
							// Exit loop because there are no more possible bits to modify
							break;
						}
					}

					--bitsLeft;
				}
				// Return mutated byte
				t_encoded.begin()[byteId] = byte;
			}
			else if (partitionType == EncodedPartitionType::eachBitUnique)
			{
				// Special handling for boolean partitions
				// As the intent of EntirePartition mode is to
				// modify only specific values

				// Boolean partitions contain many booleans,
				// so we select only one random bit and modify it
				std::size_t selectByte{ byteId + t_randomGenerator() % (bitsLeft / 8) };
				std::size_t selectBit{ t_randomGenerator() % static_cast<std::size_t>(8) };

				// Toggle random bit
				selectByte = byteId;
				char byte{ t_encoded.begin()[selectByte] };
				byte ^= (1 << selectBit);

				// Return mutated byte
				t_encoded.begin()[selectByte] = byte;
			}
		}
	}

}

#endif	// CHROMO_SERIALIZATION_H_