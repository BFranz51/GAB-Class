#include "ChromoTestFeatures.h"

namespace ga
{
	/**
	*	@brief  Sets up any initial values for this chromosome.
	*	Fill your vectors with values here.
	*
	*	@param  t_initialStateId is a value sent to the GeneticAlgorithm class and routed here. It is an easy way to start the chromosome from different states.
	*	@return void
	*/
	void ChromoTestFeatures::initializeValues(const int t_initialStateId)
	{
		for (std::size_t i{ 0 }; i < 16; ++i)
			sudoku.push_back(1);
		return;
		if (t_initialStateId == 0)
		{
			num.push_back(1);
			num.push_back(0);

			num2.push_back(0);
			num2.push_back(0);

			bools.push_back(true);
			bools.push_back(true);
			bools.push_back(false);
			bools.push_back(false);
			bools.push_back(true);
			bools.push_back(true);

			floats.push_back(0.0f);
			floats.push_back(0.0f);

			betterFloats.push_back(0.0f);
			betterFloats.push_back(0.0f);
		}
		else if (t_initialStateId == 1)
		{
			num.push_back(m_randomGenerator() % 100);
			num.push_back(m_randomGenerator() % 100);

			num2.push_back(m_randomGenerator() % 100);
			num2.push_back(m_randomGenerator() % 100);

			bools.push_back(true);
			bools.push_back(true);
			bools.push_back(false);
			bools.push_back(false);
			bools.push_back(true);
			bools.push_back(true);

			floats.push_back(m_randomGenerator() % 100);
			floats.push_back(m_randomGenerator() % 100);

			betterFloats.push_back(0);
			betterFloats.push_back(0);
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
	std::ostream& operator<<(std::ostream& t_output, const ChromoTestFeatures& self) {
		t_output << self.sudoku;
		//t_output << self.num;
		//t_output << "\t" << self.num2;
		//t_output << "\t" << self.bools;
		//t_output << "\t" << self.floats;
		//t_output << "\t" << self.betterFloats;
		t_output << "\t Score = " << self.getScore();
		//t_output << ", \tencoded = [[" << self.m_encoded << "]]";
		return t_output;
	}

	/**
	*	@brief  Writes Chromo data to a CSV file stream.
	*
	*	@param  t_oStream specifies the file stream to write to
	*	@return void
	*/
	void ChromoTestFeatures::writeDataToCSV(std::ostream& oStream)
	{
		bif::Import::writeVectorToCSVStream(oStream, num);
		bif::Import::writeVectorToCSVStream(oStream, num2);
		bif::Import::writeBoolVectorToCSVStream(oStream, bools);
		bif::Import::writeVectorToCSVStream(oStream, floats);
		bif::Import::writeVectorToCSVStream(oStream, betterFloats);
	}

	/**
	*	@brief  Reads Chromo data from a CSV line
	*
	*	@param  t_chromoValues is a vector of strings containing the CSV row values
	*	@return void
	*/
	void ChromoTestFeatures::readDataFromCSV(std::vector<std::string>& chromoValues)
	{
		num = bif::Import::vectorStringToVector<short>(chromoValues, 2);
		num2 = bif::Import::vectorStringToVector<int>(chromoValues, 2);
		bools = bif::Import::vectorStringToBoolVector(chromoValues, 6);
		floats = bif::Import::vectorStringToVector<float>(chromoValues, 2);
		betterFloats = bif::Import::vectorStringToVector<float>(chromoValues, 2);
	}

	/**
	*	@brief  This is a static function used by GeneticAlgorithm.h to obtain the data partitions within the encoded chromosome.
	*
	*	@param  t_indices specifies the vector of partitions to be modified
	*   @param  t_mutateBytes specifies the number of bytes allowed for the crossover and mutation phases
	*	@return void
	*/
	void ChromoTestFeatures::getEncodedPartitions(std::vector<EncodedPartition>& t_indices, MutationLimits& t_mutationLimits)
	{
		t_indices.clear();
		std::size_t location{ 0 };
		t_mutationLimits.bytes = 0;
		t_mutationLimits.partitions = 0;
		Chromo::addItemIndicesOfVector(t_indices, t_mutationLimits, location, sizeof(short int), 16, "Sudoku", true);
		/*Chromo::addItemIndicesOfVector(t_indices, t_mutationLimits, location, sizeof(short int), 2, "RandomShort", true);
		Chromo::addItemIndicesOfVector(t_indices, t_mutationLimits, location, sizeof(int), 2, "RandomInt", true);
		Chromo::addItemIndicesOfBoolVector(t_indices, t_mutationLimits, location, 6, "Bools", true);
		Chromo::addItemIndicesOfVector(t_indices, t_mutationLimits, location, sizeof(float), 2, "RandomFloat", true);
		Chromo::addItemIndicesOfVector(t_indices, t_mutationLimits, location, sizeof(float), 2, "BetterFloat", false);*/
	}

	/**
	*	@brief  Runs the fitness function for this chromosome. This operates locally and does not affect other chromosomes that are being tested.
	*
	*	@return void
	*/
	void ChromoTestFeatures::runFitnessFunctionLocal()
	{
		double total{ 0 };
		int penalty{ 0 };

		// Test lines
		for (std::size_t i{ 0 }; i < 4; ++i)
		{
			std::vector<bool> hasNumH = { false, false, false, false };
			std::vector<bool> hasNumV = { false, false, false, false };
			for (std::size_t j{ 0 }; j < 4; ++j)
			{
				short int val;

				// Test each horizontal line
				val = sudoku.at(i * 4 + j);
				if (hasNumH.at(val))
					penalty += 1;
				else hasNumH.at(val) = true;

				// Test each vertical line
				val = sudoku.at(j * 4 + i);
				if (hasNumV.at(val))
					penalty += 1;
				else hasNumV.at(val) = true;
			}
		}

		// Test clusters
		for (std::size_t i{ 0 }; i < 2; ++i)
		{
			for (std::size_t j{ 0 }; j < 2; ++j)
			{
				// Test inside cluster
				std::vector<bool> hasNum = { false, false, false, false };
				for (std::size_t x{ 0 }; x < 2; ++x)
				{
					for (std::size_t y{ 0 }; y < 2; ++y)
					{
						short int val = sudoku.at(y * 4 + x + i * 2 + j * 8);
						if (hasNum.at(val))
							penalty += 1;
						else hasNum.at(val) = true;
					}
				}
			}
		}

		setScore(10000 - penalty * penalty);
		return;

		// Only count unique numbers
		std::unordered_map<std::size_t, int> usedNumbers;
		for (std::size_t i = 0; i < num.size(); ++i) {
			if (usedNumbers.find(num.at(i)) == usedNumbers.end()) {
				total += num.at(i);
				usedNumbers[num.at(i)] = 1;
			}
		}

		// Count all integers from num2
		for (std::size_t i = 0; i < num2.size(); ++i) {
			total += num2.at(i);
		}

		// Booleans should be in pattern True-False-True-False... etc
		for (std::size_t i = 0; i < bools.size(); ++i) {
			if (i % 2 == 0) {
				total += (bools.at(i) ? 5 : 0);
			}
			else {
				total += (bools.at(i) ? 0 : 5);
			}
		}

		Chromo::setScore(total);
	}

	/**
	*	@brief  Converts chromosome data values into a string meant to be crossed over and/or mutated.
	*
	*	@return void
	*/
	void ChromoTestFeatures::encode()
	{
		m_encoded = "";

		// Calculate string size needed
		std::size_t encodedSize{ 0 };
		encodedSize += sizeof(sudoku.at(0)) * sudoku.size();
		/*encodedSize += sizeof(num.at(0)) * num.size();
		encodedSize += sizeof(num2.at(0)) * num2.size();
		encodedSize += (bools.size() / 8 + 1);
		encodedSize += sizeof(floats.at(0)) * floats.size();
		encodedSize += sizeof(betterFloats.at(0)) * betterFloats.size();*/
		
		// Reserve string memory
		m_encoded.reserve(encodedSize + static_cast<std::size_t>(1));

		m_encoded += encodeVector(sudoku);
		/*m_encoded += encodeVector(num);
		m_encoded += encodeVector(num2);
		m_encoded += encodeBoolVector(bools);
		m_encoded += encodeVector(floats);
		m_encoded += encodeVector(betterFloats);*/
	}

	/**
	*	@brief  Converts the encoded string into data values used in fitness function.
	*
	*	@return void
	*/
	void ChromoTestFeatures::decode()
	{
		if (m_encoded.length() > 0) {
			std::size_t curStrIndex{ 0 };
			curStrIndex = decodeVector(sudoku, m_encoded, curStrIndex);
			/*curStrIndex = decodeVector(num, m_encoded, curStrIndex);
			curStrIndex = decodeVector(num2, m_encoded, curStrIndex);
			curStrIndex = decodeBoolVector(bools, m_encoded, curStrIndex);
			curStrIndex = decodeVector(floats, m_encoded, curStrIndex);
			curStrIndex = decodeVector(betterFloats, m_encoded, curStrIndex);*/
		} else {
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
	void ChromoTestFeatures::mutateCustom()
	{
		// 100% chance
		return;
		if (m_randomGenerator() % 100 < 100) {
			// Mutate every entry in betterFloats
			for (std::size_t i{ 0 }; i < betterFloats.size(); ++i) {
				betterFloats.at(i) += randomRangeDouble(-5.0, 5.0, m_randomGenerator);
			}
		}
	}

	/**
	*	@brief  Applies limits to data values.
	*
	*	@return void
	*/
	void ChromoTestFeatures::applyLimits()
	{
		limitVectorModulo(sudoku, static_cast<short int>(0), static_cast<short int>(3));
		/*limitVectorModulo(num, static_cast<short int>(-100), static_cast<short int>(100));
		limitVectorModulo(num2, static_cast<int>(-100), static_cast<int>(100));
		limitVectorModuloDouble(floats, static_cast<float>(-100), static_cast<float>(100));
		limitVector(betterFloats, static_cast<float>(-100), static_cast<float>(100));*/
	}

} // namespace ga