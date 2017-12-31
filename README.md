# Genetic Algorithm Base Class (GAB-Class)
This project provides a starting point for quickly implementing and testing new genetic algorithms (GAs) in C++. It minimizes the time spent working on crossovers and mutations so that GA programmers can spend most of their time on the fitness function.

**_Note:_** This is a hobby project that began 12/3/17 (right before my finals!) and is still a work in progress.

# Table of Contents
* [What is a Genetic Algorithm?](#whatIsGA)
* [GAB-Class Implementation](#gabClass)
* [Quick Start!](#quickStart)
* [Crossover Notes](#crossovers)
* [Mutation Notes](#mutations)
* [Planned Features](#plannedFeatures)
* [References](#references)

<a name = "whatIsGA"/>
# What is a Genetic Algorithm?
Genetic algorithms are optimization algorithms that use the principles of evolution and natural selection to solve problems. GAs are useful for problems with the following properties:
* Large or infinite number of possible solutions
* Possible to mathematically evaluate potential solutions
* No need to prove that solutions are optimal (at least, not as part of the GA)

GAs have the following phases:
1. Evaluation Phase - each solution is scored according to a fitness function
2. Crossover Phase – lower scored solutions are replaced with combinations of higher scored solutions
3. Mutation Phase – solutions are randomly changed

<a name = "gabClass"/>
# GAB-Class Implementation

## Workflow:

GAB-Class is the combination of several classes:

* GeneticAlgorithm class:
  * Manages vector of chromosomes
  * Runs all GA phases, calling appropriate Chromo methods
* Chromo class:
  * This is a base class that provides common functionality for chromosomes
  * Contains crossover and mutation functions
  * Contains file I/O functions
* A class that inherits from Chromo:
  * This is created by users and contains any data required for the problem being solved
  * Contains fitness function
  * Contains custom mutations
  * Overrides functions to handle serialization
		* GAB-Class includes a ChromoDefault class which can be used as a template for this
* ChromoSerialization.h and GeneticAlgorithmTechniques.h:
  * Static helper functions and enums

## Serialization:
The biggest problem I encountered in this project was that the bulk of the work was still being done in the derived classes. Ideally, users should spend their time on the fitness function and let GAB-Class handle the crossover and mutation phases.

GAB-Class needed some way to modify arbitrary data contained in the derived classes. I decided to “serialize” the data into long string variables.

The crossover and mutation phases are handled by GAB-Class and performed on the serialized strings. Afterwards, they are deserialized back into data that the fitness function can work with.

## "Safe" vs "Unsafe":
In the code and this documentation, I use the terms "safe" and "unsafe."

Each generation, GAB-Class counts up the number of Chromos which are not intended to be crossed over (based on user settings). The highest-scoring Chromos should not be replaced. They are referred to as "safe." The rest of the Chromos are called "unsafe", and will be overwritten during the crossover phase.

However, sometimes there are many Chromos with invalid scores, including "safe" Chromos. To fix this, GAB-Class iterates through "safe" Chromos in reverse, marking invalid ones as "unsafe". Those Chromos will be copied from "safe" Chromos.

<a name = "quickStart"/>
# Quick Start
## Step 1: Set up project
Come up with an amazing name for your Chromo class. This is where you will implement your fitness function.

Download all the headers and include them:

```
#include "GeneticAlgorithm.h"
#include "Chromo.h"
#include "AmazingChromoName.h" // <-- Change this to whichever amazing name you chose 
```

## Step 2: Basic variable setup
Copy-paste the code from ChromoDefault.cpp and ChromoDefault.h into AmazingChromoName.cpp and AmazingChromoName.h.

Create variables to hold your solution data:

```
// Example
std::vector<int> num;
std::vector<bool> bools;
std::vector<float> floats;
```

This the data you will use for your fitness function. In order to allow GAB-Class to use it for crossovers, mutations, and file I/O, you need to add it to the following encoding methods.

First up is the serialize() method, where you convert all the data to a single string named m_serialized. This string is stored in the Chromo base class.

```
// Example
void ChromoTestFeatures::serialize()
{
  m_serialized = "";

  // Optional: Calculate string size needed and reserve that space
  size_t serialSize{ 0 };
  serialSize += sizeof(num.at(0)) * num.size();
  serialSize += (bools.size() / 8 + 1);
  serialSize += sizeof(floats.at(0)) * floats.size();
  m_serialized.reserve(serialSize + static_cast<size_t>(1));

  // Add data to string
  m_serialized += serializeVector(num);
  m_serialized += serializeBoolVector(bools);
  m_serialized += serializeVector(floats);
}
```

As the old saying goes, "what is serialized must be deserialized."

After our serialized string is crossed over and mutated, we need a way to translate it back to the more friendly variables used in the fitness function.

```
// Example
void ChromoTestFeatures::deserialize()
{
  if (m_serialized.length() > 0) {
    // Variable to keep track of where we are at in the serialized string
    size_t curStrIndex{ 0 };
    curStrIndex = deserializeVector(num, m_serialized, curStrIndex);
    curStrIndex = deserializeBoolVector(bools, m_serialized, curStrIndex);
    curStrIndex = deserializeVector(floats, m_serialized, curStrIndex);
  } else {
    std::cout << "ERROR: Serialized string not found!";
  }
}
```

Lastly, we need to give the GA some basic info about the serialized string. This info will be used for file I/O and making crossovers and mutations more useful. More on that in later sections...

This process is handled by the static method getSerialItemIndices().

```
// Example
void ChromoTestFeatures::getSerialItemIndices(std::vector<SerialPartition>& t_indices, MutationLimits& t_mutationLimits)
{
  t_indices.clear();
  size_t location{ 0 };
  t_mutationLimits.bytes = 0;
  t_mutationLimits.partitions = 0;
  Chromo::addItemIndicesOfVector(t_indices, t_mutationLimits, location, sizeof(int), 2, "RandomInt", true);
  Chromo::addItemIndicesOfBoolVector(t_indices, t_mutationLimits, location, 6, "Bools", true);
  Chromo::addItemIndicesOfVector(t_indices, t_mutationLimits, location, sizeof(float), 2, "RandomFloat", false);
}
```

Let's break this down. Consider the following line:

Chromo::addItemIndicesOfVector(~~t_indices, t_mutationLimits, location~~, sizeof(int), 2, "RandomInt", true);

*sizeof(int)* sends the size of each vector item.

*2* indicates that the vector is of size 2

*"RandomInt"* is the name, which will be used when writing to CSV

*true/false* tells the GA whether to include the vector in the mutation phase

**_NOTE:_** If you use variables that are meant to be excluded from mutations, serialize them LAST!



## Step 3: Fill in optional methods

You may have noticed that in getSerialItemIndices(), the floats were not set to be included in the crossover and mutation phases. This is because while floats and doubles can be mutated, it's easier to get useful mutations by altering them manually.

The following is an easy way to mutate floats/doubles. mutateCustom is called every generation.

```
// Example
void ChromoTestFeatures::mutateCustom()
{
  // 30% chance of mutation
  if (m_randomGenerator() % 100 < 30) {
    // Mutate a random float
    const size_t i { m_randomGenerator() % floats.size() };
    floats.at(i) += randomRangeDouble(-5.0, 5.0, m_randomGenerator);
  }
}
```

In order for mutateCustom() to be called, set the following method in the header file:
```
const static bool hasCustomMutations() { return true; };
```

Sometimes, we want to limit data to a certain range. GAB-Class includes some static methods to help out with this. Notice how floats/doubles are treated differently.

```
void ChromoTestFeatures::applyLimits()
{
  // Wrap around, within range
  limitVectorModulo(num, static_cast<int>(-100), static_cast<int>(100));
  // Hard limit to range
  limitVector(num, static_cast<int>(-100), static_cast<int>(100));
  // Another wrap-around limit
  limitVectorModuloDouble(floats, static_cast<float>(-100), static_cast<float>(100));
}
```

Lastly, if you want to write to CSV rather than binary, you need to implement the following functions. I recommend saving this until later, when you have good results and want to store them in a readable fashion.

```
void ChromoTestFeatures::writeDataToCSV(std::ostream& oStream)
{
  bif::Import::writeVectorToCSVStream(oStream, num);
  bif::Import::writeBoolVectorToCSVStream(oStream, bools);
  bif::Import::writeVectorToCSVStream(oStream, floats);
}
```

```
void ChromoTestFeatures::readDataFromCSV(std::vector<std::string>& chromoValues)
{
  num = bif::Import::vectorStringToVector<short>(chromoValues, 2);
  bools = bif::Import::vectorStringToBoolVector(chromoValues, 6);
  floats = bif::Import::vectorStringToVector<float>(chromoValues, 2);
}
```



## Step 4: Add your fitness function and start testing!!!



<a name = "crossovers"/>
# Crossover Notes
There are 3 types of crossovers included. Users can set the number of times each is used per generation.

The basic idea is that each "unsafe" Chromo is replaced, using 1-2 "safe" Chromos as sources. "Unsafe" Chromos use the following crossover procedures:

*	Copy
  *	The GA selects a random safe Chromo and copies its values
		
*	Shuffle
  *	The GA selects 2 safe Chromos to use as sources
  *	Each byte is randomly chosen from one of the sources
  *	The bytes are kept in the same order

*	n-Split Crossover
  *	The GA selects 2 safe Chromos to use as sources
  *	The serialized strings are split into n partitions
  *	Each partition is filled with data from a source string, in alternating fashion



<a name = "mutations"/>
# Mutation Notes
The way mutations occur has a huge impact on the number of generations you need to run in order to get results. There are several settings to help with this.

## MutationCountMax:
Each time a Chromo is selected for mutation, the number of mutations is randomly set in the range [1, MutationCountMax]. Use this setting to increase/decrease the variability of data.

## MutationBitWidth:
Every time a mutation occurs, it is done over a range of bits. If you want to mutate an entire byte, set this to 8.

## MutationChanceIn100:
For each bit in a mutation range, this is the chance that the bit is toggled.

## MutationSelection:
This is an enum setting that gives you control over how mutation ranges are selected.
*	pureRandom:
  *	A random bit from a random byte will be chosen as the starting point for the mutation block 
*	randomByte:
  *	Same as above, but the mutation block will always start on a new byte
*	entirePartition:
  *	Each partition within the serialized data will be mutated on its own
  *	This is useful for when you want one number to mutate at a time

My personal suggestion is to use **entirePartition**. If you want multiple values to be mutated at once, increase the **MutationCountMax** setting. This lets mutations be more randomly distributed, rather than certain values changing at the same time just because they were closer together in the serialized string.

## Custom Mutations:
This feature is intended for data that you want more control over, as well as data that is hard to mutate randomly (floats and doubles).

The most common use is altering variables by a random range. GAB-Class includes this data in the crossover phase and file I/O, but does not perform mutations on it.



<a name = "plannedFeatures"/>
# Planned Features

*	Allow vectors of different sizes
*	Utility functions for 2D and 3D arrays
* Variability for partitions so users can make certain data more/less likely to mutate
* Currently, scores are invalid if they are 0.0 or below. Scores should be changed to a struct containing an *isInvalid* bool



<a name = "references"/>
## References
* Knuth Algorithm – used to generate random unique numbers
