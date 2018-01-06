#include "ChromoDefault.h"

namespace ga
{
	/**
	*	@brief  Sets up any initial values for this chromosome.
	*	Fill your vectors with values here.
	*
	*	@param  t_initialStateId is a value sent to the GeneticAlgorithm class and routed here. It is an easy way to start the chromosome from different states.
	*	@return void
	*/
	void ChromoDefault::initializeValues(const int t_initialStateId)
	{
		// TODO: Set variables...
		// TODO: Fill any vectors with initial values...
	}

	/**
	*	@brief  This is an operator overload that allows the chromosome to be printed.
	*	
	*
	*	@param  t_output is the output stream
	*   @param  self is the reference to this Chromo
	*	@return Output stream
	*/
	std::ostream& operator<<(std::ostream& t_output, const ChromoDefault& self) {
		t_output << "\t Score = " << self.getScore();
		
		// TODO: Output any variables and vectors that would be helpful

		t_output << ", \tencoded = [[" << self.m_encoded << "]]";

		return t_output;
	}

	/**
	*	@brief  Writes Chromo data to a CSV file stream.
	*
	*	@param  t_oStream specifies the file stream to write to
	*	@return void
	*/
	void ChromoDefault::writeDataToCSV(std::ostream& t_oStream)
	{
		// TODO: Write data to t_oStream
		// EXAMPLE:
		//	bif::Import::writeVectorToCSVStream(t_oStream, ints);
		//	bif::Import::writeBoolVectorToCSVStream(t_oStream, bools);
	}

	/**
	*	@brief  Reads Chromo data from a CSV line
	*
	*	@param  t_chromoValues is a vector of strings containing the CSV row values
	*	@return void
	*/
	void ChromoDefault::readDataFromCSV(std::vector<std::string>& t_chromoValues)
	{
		// TODO: Read data from t_chromoValues
		// EXAMPLE:
		//	ints = bif::Import::vectorStringToVector(chromoValues, 5);
		//	bools = bif::Import::vectorStringToBoolVector(chromoValues, 10);
	}

	/**
	*	@brief  This is a static function used by GeneticAlgorithm.h to obtain the data partitions within the encoded chromosome.
	*
	*	@param  t_indices specifies the vector of partitions to be modified
	*   @param  t_mutateBytes specifies the number of bytes allowed for the crossover and mutation phases
	*	@return void
	*/
	void ChromoDefault::getEncodedPartitions(std::vector<EncodedPartition>& t_indices, MutationLimits& t_mutationLimits)
	{
		t_indices.clear();
		size_t location{ 0 };
		t_mutationLimits.bytes = 0;
		t_mutationLimits.partitions = 0;
		// TODO: Fill in t_indices with all the encoded values which
		// are meant to be modified during the crossover and mutation phases
		
		// EXAMPLE:
		//Chromo::addItemIndicesOfVector(t_indices, t_mutationLimits, location, sizeof(int), 5, "Ints", true);
		//Chromo::addItemIndicesOfBoolVector(t_indices, t_mutationLimits, location, 10, "Bools", true);
	}

	/**
	*	@brief  Runs the fitness function for this chromosome. This operates locally and does not affect other chromosomes that are being tested.
	*
	*	@return void
	*/
	void ChromoDefault::runFitnessFunctionLocal()
	{
		Chromo::setScore(0);
	}

	/**
	*	@brief  Converts chromosome data values into a string meant to be crossed over and/or mutated.
	*
	*	@return void
	*/
	void ChromoDefault::encode()
	{
		m_encoded = "";

		// Calculate string size needed
		size_t encodedSize{ 0 };
		// TODO: Calculate total string size needed
		// EXAMPLE:
		//	encodedSize += sizeof(ints.at(0)) * ints.size();
		//	encodedSize += sizeof(bool) * bools.size();

		// Reserve string memory
		m_encoded.reserve(encodedSize + static_cast<size_t>(1));

		// TODO: Add data values to encoded string, using methods from static libraries
		// EXAMPLE:
		//	m_encoded += encodeVector(ints);
		//	m_encoded += encodeBoolVector(bools);
	}

	/**
	*	@brief  Converts the encoded string into data values used in fitness function.
	*
	*	@return void
	*/
	void ChromoDefault::decode()
	{
		if (m_encoded.length() > 0) {
			size_t curStrIndex{ 0 };
			// TODO: Use static methods to pull data values from the encoded string
			// EXAMPLE:
			//	curStrIndex = decodeVector(ints, m_encoded, curStrIndex);
			//	curStrIndex = decodeBoolVector(bools, m_encoded, curStrIndex);
		}
		else {
			std::cout << "ERROR: Encoded string not found!";
		}
	}

	/**
	*	@brief  Performs custom mutations on data values
	*	These data values should be part of the encoded string,
	*	yet not used in the crossover and mutation phases.
	*
	*	@return void
	*/
	void ChromoDefault::mutateCustom()
	{
		// TODO: Add any custom mutations here
	}

	/**
	*	@brief  Applies limits to data values.
	*
	*	@return void
	*/
	void ChromoDefault::applyLimits()
	{
		// TODO: Any limit functions
		// EXAMPLE:
		//	limitVectorModulo(num, static_cast<short int>(-100), static_cast<short int>(10));
		//	limitVectorModuloDouble(floats, static_cast<float>(-10), static_cast<float>(10));
	}

} // namespace ga