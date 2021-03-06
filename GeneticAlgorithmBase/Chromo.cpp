#include "Chromo.h"

namespace ga
{
	/**
	*	@brief  Writes the encoded data to a file stream.
	*	Outputs the character count, followed by the encoded string.
	*
	*	@param  t_oStream specifies the file stream to write to
	*	@return void
	*/
	void Chromo::writeToFileAsBinary(std::ofstream& t_oStream)
	{
		std::size_t characters = m_encoded.size();
		t_oStream.write((char*) &characters, sizeof(std::size_t));
		t_oStream.write(&m_encoded[0], characters);
	}

	/**
	*	@brief  Reads the encoded data from a a file stream.
	*	Inputs the character count, followed by the encoded string.
	*
	*	@param  t_iStream specifies the file stream to read from
	*	@return void
	*/
	void Chromo::readFromFileAsBinary(std::ifstream& t_iStream)
	{
		// Read encoded size first
		char* readMemSizeT;
		readMemSizeT = new char[sizeof(std::size_t)];
		t_iStream.read(readMemSizeT, sizeof(std::size_t));
		std::size_t characters = *((std::size_t*)readMemSizeT);
		
		// Now read string
		m_encoded.resize(characters);
		t_iStream.read(&m_encoded[0], characters);
		
		delete[] readMemSizeT;
	}

	/**
	*	@brief  Writes Chromo data to a CSV file stream.
	*	Outputs the ID and score, but calls an inherited function to
	*	output the actual data values. This is because the Chromo
	*	base class does not actually know the variables used.
	*
	*	@param  t_id specifies the Chromo ID
	*	@param  t_oStream specifies the file stream to write to
	*	@return void
	*/
	void Chromo::writeToFileAsCSV(const std::size_t t_id, std::ofstream& t_oStream)
	{
		// Write CSV values that base class knows
		t_oStream << std::to_string(t_id) << "," << getScore();

		// Call virtual function to write data in inherited class
		writeDataToCSV(t_oStream);
		t_oStream << "\n";
	}

	/**
	*	@brief  Reads Chromo data from a CSV file stream.
	*	Inputs the CSV row corresponding to this Chromo's data.
	*	Retrieves the ID and score, but calls an inherited function
	*	to retrieve the rest of the values. This is because the Chromo
	*	base class does not actually know the variables used.
	*
	*	@param  t_iStream specifies the file stream to read from
	*	@return void
	*/
	void Chromo::readFromFileAsCSV(std::ifstream& t_iStream)
	{
		// Read line as a vector of strings
		std::string line;
		std::getline(t_iStream, line);
		std::vector<std::string> chromoValues = bif::Import::parseString(line, ",", bif::Import::CleanTokenBy::none);

		// Get ID (not used) and score
		std::size_t id;
		double score;
		stringToNumber(chromoValues.at(0), id);
		stringToNumber(chromoValues.at(1), score);
		setScore(score);

		// Erase first 2 entries
		chromoValues.erase(chromoValues.begin());
		chromoValues.erase(chromoValues.begin());

		// Call virtual function to read data in inherited class
		readDataFromCSV(chromoValues);
	}

	/**
	*	@brief  Returns the Chromo's score, according to the fitness function
	*
	*	@return m_score
	*/
	double Chromo::getScore() const
	{
		return m_score;
	}

	/**
	*	@brief  Returns the Chromo's encoded string
	*
	*	@return m_score
	*/
	std::string Chromo::getEncoding() const
	{
		return m_encoded;
	}

	/**
	*	@brief  Sets the fitness score.
	*
	*	@param  t_score is the new score
	*	@return void
	*/
	void Chromo::setScore(double t_score)
	{
		m_score = t_score;
	}

	/**
	*	@brief  Mutates the encoded data.
	*	The number of mutations is randomly decided between 1 and the mutation count max.
	*
	*	@param  t_encodedPartitions is a vector of indices that identifies where values are in the encoded string
	*   @param  t_mutationMode is a MutationMode enum specifying how mutations are handled
	*	@return void
	*/
	void Chromo::mutate(std::vector<EncodedPartition>& t_encodedPartitions, const MutationLimits t_mutationLimits, const MutationSelection t_mutationSelection,
		const std::size_t t_mutationCountMax, const std::size_t t_mutationBitWidth, const short int t_mutationChanceIn100)
	{
		const std::size_t mutationCount{ m_randomGenerator() % t_mutationCountMax + 1 };
		//std::cout << "\n[" << m_encoded << "] to";
		mutateRandomBits(m_encoded, t_encodedPartitions, t_mutationLimits, t_mutationSelection, mutationCount, t_mutationBitWidth, t_mutationChanceIn100, m_randomGenerator);
		//std::cout << "\n[" << m_encoded << "]";
	}

	/**
	*	@brief  Modifies a EncodedPartition vector, outlining where values will be within the encoded string according to a data vector.
	*	For each vector item, a new partition is pushed to t_encodedPartitions
	*	with information on that data value.
	*
	*	@param  t_encodedPartitions is a vector of indices that identifies where values are in the encoded string. The function adds entries to this vector.
	*	@param  t_mutationLimits is a struct containing byte and partition limits to modify
	*   @param  t_nextLocation is an index pointing to the next location in the encoded string. It is incremented as partitions are found.
	*   @param  t_itemSize is the number of bytes in the vector type
	*   @param  t_vectorSize is the number of items in the vector
	*   @param  t_vectorName is the name of the vector
	*   @param  t_mutatable is a bool representing whether to allow this partition to be mutated
	*	@return void
	*/
	void Chromo::addItemIndicesOfVector(std::vector<EncodedPartition>& t_encodedPartitions, MutationLimits& t_mutationLimits, std::size_t& t_nextLocation, const std::size_t t_itemSize, const std::size_t t_vectorSize, const std::string t_vectorName, const bool t_mutatable)
	{
		std::string partitionName;
		partitionName.reserve(t_vectorName.length() + 5);
		for (std::size_t i{ 0 }; i < t_vectorSize; ++i) {
			partitionName = t_vectorName + "_" + std::to_string(i);
			t_encodedPartitions.push_back(EncodedPartition(partitionName, t_nextLocation, t_itemSize, EncodedPartitionType::normal));
			t_nextLocation += t_itemSize;
		}
		
		// Add to limits
		if (t_mutatable) {
			t_mutationLimits.bytes += t_vectorSize * t_itemSize;;
			t_mutationLimits.partitions += t_vectorSize;
		}
	}

	/**
	*	@brief  Modifies an EncodedPartition vector, outlining where values will be within the encoded string according to a boolean vector.
	*	For each vector item, a new partition is pushed to t_encodedPartitions
	*	with information on that data value.
	*	
	*	Since in the encoded string, booleans are packed into bytes
	*	containing 8 boolean values each, the partition size is the
	*	vector size divided by 8 and rounded up.
	*
	*	@param  t_encodedPartitions is a vector of indices that identifies where values are in the encoded string. The function adds entries to this vector.
	*	@param  t_mutationLimits is a struct containing byte and partition limits to modify
	*   @param  t_nextLocation is an index pointing to the next location in the encoded string. It is incremented as partitions are found.
	*   @param  t_vectorSize is the number of items in the vector
	*   @param  t_vectorName is the name of the vector
	*   @param  t_mutatable is a bool representing whether to allow this partition to be mutated
	*	@return void
	*/
	void Chromo::addItemIndicesOfBoolVector(std::vector<EncodedPartition>& t_encodedPartitions, MutationLimits& t_mutationLimits, std::size_t& t_nextLocation, const std::size_t t_vectorSize, const std::string t_vectorName, const bool t_mutatable)
	{
		// Bools are treated as bits
		// So this is stored as a single partition, marked as BoolPartition
		
		// Round up for bytes used
		std::size_t bytesUsed{ 0 };
		if (t_vectorSize % 8 == 0) {
			bytesUsed = t_vectorSize / 8;
		}
		else {
			bytesUsed = t_vectorSize / 8 + 1;
		}

		t_encodedPartitions.push_back(EncodedPartition(t_vectorName, t_nextLocation, bytesUsed, EncodedPartitionType::eachBitUnique, t_vectorSize));
		t_nextLocation += bytesUsed;
		
		// Add to limits
		if (t_mutatable) {
			t_mutationLimits.bytes += bytesUsed;
			t_mutationLimits.partitions++;
		}
	}

} // namespace ga