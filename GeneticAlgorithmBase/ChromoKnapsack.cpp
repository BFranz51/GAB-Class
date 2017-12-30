#include "ChromoKnapsack.h"

namespace ga
{
	/**
	*	@brief  Sets up any initial values for this chromosome.
	*	Fill your vectors with values here.
	*
	*	@param  t_initialStateId is a value sent to the GeneticAlgorithm class and routed here. It is an easy way to start the chromosome from different states.
	*	@return void
	*/
	void ChromoKnapsack::initializeValues(const int t_initialStateId)
	{
		for (size_t i{ 0 }; i < 16; ++i) {
			hasItem.push_back(false);
		}
	}

	/**
	*	@brief  This is an operator overload that allows the chromosome to be printed.
	*
	*
	*	@param  t_output is the output stream
	*   @param  self is the reference to this Chromo
	*	@return Output stream
	*/
	std::ostream& operator<<(std::ostream& t_output, const ChromoKnapsack& self) {
		t_output << self.hasItem;
		t_output << "\t Score = " << self.getScore();
		//t_output << ", \tserial = [[" << self.m_serialized << "]]";
		return t_output;
	}

	/**
	*	@brief  Writes Chromo data to a CSV file stream.
	*
	*	@param  t_oStream specifies the file stream to write to
	*	@return void
	*/
	void ChromoKnapsack::writeDataToCSV(std::ostream& oStream)
	{
		bif::Import::writeBoolVectorToCSVStream(oStream, hasItem);
	}

	/**
	*	@brief  Reads Chromo data from a CSV line
	*
	*	@param  t_chromoValues is a vector of strings containing the CSV row values
	*	@return void
	*/
	void ChromoKnapsack::readDataFromCSV(std::vector<std::string>& chromoValues)
	{
		hasItem = bif::Import::vectorStringToBoolVector(chromoValues, 16);
	}

	/**
	*	@brief  This is a static function used by GeneticAlgorithm.h to obtain the data partitions within the serialized chromosome.
	*
	*	@param  t_indices specifies the vector of partitions to be modified
	*   @param  t_mutateBytes specifies the number of bytes allowed for the crossover and mutation phases
	*	@return void
	*/
	void ChromoKnapsack::getSerialItemIndices(std::vector<SerialPartition>& t_indices, MutationLimits& t_mutationLimits)
	{
		t_indices.clear();
		size_t location{ 0 };
		t_mutationLimits.bytes = 0;
		t_mutationLimits.partitions = 0;
		Chromo::addItemIndicesOfBoolVector(t_indices, t_mutationLimits, location, 16, "Bools", true);
	}

	/**
	*	@brief  Runs the fitness function for this chromosome. This operates locally and does not affect other chromosomes that are being tested.
	*	This fitness function sets the score equal to the sum of the
	*	item values, if the total weight is under 100.
	*
	*	@return void
	*/
	void ChromoKnapsack::runFitnessFunctionLocal()
	{
		short sumValue{ 0 };
		short sumWeight{ 0 };

		// Each item has a certain value and weight
		std::vector<short> weights{ 30, 40, 23, 46, 11, 6, 87, 5, 64, 97, 23, 45, 21, 21, 64, 3 };
		std::vector<short> values{ 28, 41, 25, 45, 8, 4, 100, 1, 60, 110, 24, 46, 20, 21, 63, 2 };
		for (size_t i{ 0 }; i < hasItem.size(); ++i) {
			if (hasItem.at(i)) {
				sumValue += weights.at(i);
				sumWeight += values.at(i);
			}
		}

		// Must be under weight limit
		if (sumWeight < 100) {
			Chromo::setScore(sumValue + 1);
		}
		else {
			Chromo::setScore(0);
		}
	}

	/**
	*	@brief  Converts chromosome data values into a string meant to be crossed over and/or mutated.
	*
	*	@return void
	*/
	void ChromoKnapsack::serialize()
	{
		m_serialized = "";

		// Calculate string size needed
		size_t serialSize{ 0 };
		serialSize += sizeof(bool) * hasItem.size();

		// Reserve string memory
		m_serialized.reserve(serialSize + static_cast<size_t>(1));

		m_serialized += serializeBoolVector(hasItem);
	}

	/**
	*	@brief  Converts the serialized string into data values used in fitness function.
	*
	*	@return void
	*/
	void ChromoKnapsack::deserialize()
	{
		if (m_serialized.length() > 0) {
			size_t curStrIndex{ 0 };
			curStrIndex = deserializeBoolVector(hasItem, m_serialized, curStrIndex);
		}
		else {
			std::cout << "ERROR: Serialized string not found!";
		}
	}

	/**
	*	@brief  Performs custom mutations on data values
	*	These data values should be part of the serialized string,
	*	yet not used in the crossover and mutation phases.
	*
	*	@return void
	*/
	void ChromoKnapsack::mutateCustom()
	{
		// No custom mutations required!
	}

	/**
	*	@brief  Applies limits to data values.
	*
	*	@return void
	*/
	void ChromoKnapsack::applyLimits()
	{
		// No limits required!
	}

} // namespace ga