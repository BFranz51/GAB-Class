/**
* @class ImportData.h
* @author Bryan Franz
* @date December 26, 2017
* @brief Contains static functions to aid in import/export of files
*
* @see (link to GitHub)
*/

#ifndef IMPORT_DATA_H_
#define IMPORT_DATA_H_

#ifdef _MSC_VER
#pragma once
#endif	// _MSC_VER

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>

// Amazingly clever namespace name
namespace bif
{
	class Import
	{
	public:
		// Collection of enums
		enum class CleanTokenBy { none, trimSpaces };

		/**
		*	@brief  Prints the C++ version
		*
		*	@return void
		*/
		static void printCppVersion()
		{
			// Print nice * line
			std::cout << std::string(25, '*');
			std::cout << "\n  Running ";
			if (__cplusplus == 201402L) {
				std::cout << "C++14";
			}
			else if (__cplusplus == 201103L) {
				std::cout << "C++11";
			}
			else if (__cplusplus == 199711L) {
				std::cout << "C++98";
			}
			else {
				std::cout << "C++ version unknown";
			}
			std::cout << " (" << __cplusplus << ")\n";
			std::cout << std::string(25, '*');
			std::cout << "\n\n";
		}

		/**
		*	@brief  Reads a file and stores lines into a vector
		*
		*	@param  t_filename specifies the path and filename of the input file
		*	@return Vector containing each file line as a string element
		*/
		static std::vector<std::string> fileToLines(std::string const t_filename)
		{
			// Open the file
			std::ifstream file(t_filename);
			std::vector<std::string> ret;

			if (file.is_open())
			{
				// Fill output vector
				std::string line;
				while (std::getline(file, line))
					ret.push_back(line);
				file.close();
			}
			else std::cout << "\nERROR: File " << t_filename << " not found!\n";

			// Output the vector
			return ret;
		}

		/**
		*	@brief  Reads a CSV file and stores lines into a "2D vector" (vector of vectors)
		*
		*	@param  t_filename specifies the path and filename of the input file
		*	@param  t_cleanTokenBy specifies the function(s) to apply to each element
		*	@return Vector of vectors containing each token from the CSV file
		*/
		static std::vector<std::vector<std::string>> csvToVector(std::string const t_filename, CleanTokenBy const t_cleanTokenBy = CleanTokenBy::trimSpaces)
		{
			// Open the file
			std::ifstream file(t_filename);
			std::vector<std::vector<std::string>> ret;
			if (file.is_open())
			{
				// Fill return vector
				std::string line;
				while (std::getline(file, line))
				{
					// Split line
					std::vector<std::string> newRow = parseString(line, ",", t_cleanTokenBy);
					ret.push_back(newRow);
				}
				file.close();
			}
			else std::cout << "\nERROR: File " << t_filename << " not found!\n";
			return ret;
		}

		/**
		*	@brief  Converts a vector of strings to a vector of another type, erasing those string items
		*	The user must specify the number of items to convert. This
		*	allows for converting a large vector of strings to several
		*	vectors of other types.
		*
		*	@param  t_stringVec specifies the vector of strings to convert
		*	@param  t_count specifies the number of items to convert
		*	@return Vector containing the converted items
		*/
		template <typename T>
		static std::vector<T> vectorStringToVector(std::vector<std::string>& t_stringVec, const std::size_t t_count)
		{
			std::vector<T> result;
			for (std::size_t iter{ 0 }; iter < t_count; ++iter)
			{
				T number;
				std::stringstream ss(t_stringVec.at(0));
				ss >> number;
				result.push_back(number);
				t_stringVec.erase(t_stringVec.begin());
			}
			return result;
		}

		/**
		*	@brief  Converts a vector of strings to a bool vector, erasing those string items
		*	The user must specify the number of items to convert. This
		*	allows for converting a large vector of strings to several
		*	vectors of other types.
		*
		*	@param  t_stringVec specifies the vector of strings to convert
		*	@param  t_count specifies the number of items to convert
		*	@return Vector of bools containing the converted items
		*/
		static std::vector<bool> vectorStringToBoolVector(std::vector<std::string>& t_stringVec, const std::size_t t_count)
		{
			std::vector<bool> result;
			for (std::size_t iter{ 0 }; iter < t_count; ++iter)
			{
				if (t_stringVec.at(0).compare("T")) {
					result.push_back(true);
				}
				else {
					result.push_back(false);
				}
				t_stringVec.erase(t_stringVec.begin());
			}
			return result;
		}

		/**
		*	@brief  Parses an input string by a delimiter, applying optional functions to each token
		*	The user must specify the number of items to convert. This
		*	allows for converting a large vector of strings to several
		*	vectors of other types.
		*
		*	@param  t_input specifies the string to be parsed
		*	@param  t_delimiter specifies the string that separates each token
		*	@param  t_apply specifies the function(s) to apply to each element
		*	@return Vector of strings containing each token as a element
		*/
		static std::vector<std::string> parseString(std::string const t_input, std::string const t_delimiter, CleanTokenBy const t_cleanTokenBy = CleanTokenBy::trimSpaces)
		{
			std::vector<std::string> parsedItems;

			// Prepare positions, ensuring we use correct std::size_t
			auto nextToken = decltype(t_input.size()) {0};
			auto lastToken = decltype(nextToken) {0};
			const auto num0 = decltype(nextToken) {0};
			const auto num1 = decltype(nextToken) {1};

			// Token must be outsize loop scope in order to handle last token
			std::string token;

			// Iterate through string looking for tokens
			while ((nextToken = t_input.find(t_delimiter, lastToken + num1)) != std::string::npos)
			{
				const auto tokenStartPos = decltype(nextToken) { lastToken == 0 ? num0 : lastToken + num1 };
				token = t_input.substr(tokenStartPos, nextToken - tokenStartPos);

				// Apply string functions
				if (t_cleanTokenBy == CleanTokenBy::trimSpaces)
					token = trim(token);
				parsedItems.push_back(token);
				lastToken = nextToken;
			}

			// Add last token, if it exists
			token = t_input.substr(lastToken + num1, nextToken - lastToken - num1);
			// Apply string functions
			if (t_cleanTokenBy == CleanTokenBy::trimSpaces)
				token = trim(token);
			parsedItems.push_back(token);

			return parsedItems;
		}

		/**
		*	@brief  Removes certain characters from the start and end of a string
		*	The user must specify the number of items to convert. This
		*	allows for converting a large vector of strings to several
		*	vectors of other types.
		*
		*	@param  t_input specifies the string to be formatted
		*	@param  t_delimiter specifies the character to remove (defaults to ' ')
		*	@return String without leading and trailing characters
		*/
		static std::string trim(const std::string& t_input, const char t_removeChar = ' ')
		{
			const auto firstValidPos{ t_input.find_first_not_of(t_removeChar) };

			// Handle strings that have no characters which are not t_removeChar
			if (firstValidPos == std::string::npos) {
				return "";
			}

			const auto lastValidPos{ t_input.find_last_not_of(t_removeChar) };
			const auto add1 = decltype(firstValidPos) {1};
			const auto validStringLength{ lastValidPos - firstValidPos + add1 };
			return t_input.substr(firstValidPos, validStringLength);
		}

		/**
		*	@brief  Outputs a vector to a stream in CSV format
		*
		*	@param  t_oStream specifies the file stream to write to
		*	@param  t_vec specifies the vector
		*	@return void
		*/
		template <typename T>
		static void writeVectorToCSVStream(std::ostream& t_oStream, const std::vector<T>& t_vec)
		{
			const auto itemCount{ t_vec.size() };
			for (auto i = decltype(itemCount) {0}; i < itemCount; ++i) {
				t_oStream << "," << t_vec.at(i);
			}
		}

		/**
		*	@brief  Outputs a bool vector to a stream in CSV format
		*
		*	@param  t_oStream specifies the file stream to write to
		*	@param  t_vec specifies the bool vector
		*	@return void
		*/
		static void writeBoolVectorToCSVStream(std::ostream& t_oStream, const std::vector<bool>& t_vec)
		{
			const auto itemCount{ t_vec.size() };
			for (auto i = decltype(itemCount) {0}; i < itemCount; ++i) {
				t_oStream << "," << (t_vec.at(i) ? "T" : "F");
			}
		}

		/**
		*	@brief  Prints a vector of strings
		*
		*	@param  t_vec specifies the vector of strings
		*	@param  t_tokenSeparator specifies the separator to add between each token (defaults to ", ")
		*	@return void
		*/
		static void printVector(const std::vector<std::string> &t_vec, const std::string t_tokenSeparator = ", ")
		{
			const auto rowCount{ t_vec.size() };
			for (auto row = decltype(rowCount) {0}; row < rowCount; ++row)
			{
				if (row > 0)
					std::cout << t_tokenSeparator;
				std::cout << '[' << row << "] = '" << t_vec.at(row) << "'";
			}
			std::cout << "\n\n";
		}

		/**
		*	@brief  Print a vector of vector of strings (a 2D array)
		*
		*	@param  t_vec specifies the vector of vector of strings
		*	@param  t_tokenSeparator specifies the separator to add between each token (defaults to ", ")
		*	@param  t_rowSeparator specifies the separator to add between each row (defaults to "\n")
		*	@return void
		*/
		static void printVector(const std::vector<std::vector<std::string>> &t_vec, const std::string t_tokenSeparator = ", ", const std::string t_rowSeparator = "\n")
		{
			const auto rowCount{ t_vec.size() };
			for (auto row = decltype(rowCount) {0}; row < rowCount; ++row)
			{
				const auto columnCount{ t_vec.at(row).size() };
				for (auto col = decltype(columnCount) {0}; col < columnCount; ++col)
				{
					if (col > 0)
						std::cout << t_tokenSeparator;
					std::cout << '[' << t_vec.at(row).at(col) << "] ";
				}
				std::cout << t_rowSeparator;
			}
			std::cout << '\n';
		}

	};
}

#endif // IMPORT_DATA_H_