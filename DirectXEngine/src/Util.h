#pragma once

#include <string>

// Store global utility functions here

// Function taken from here: http://rextester.com/YJB48513
static constexpr unsigned int fnv1aBasis = 0x811C9DC5;
static constexpr unsigned int fnv1aPrime = 0x01000193;

inline constexpr unsigned int stringHash(const char* string, unsigned int h = fnv1aBasis)
{
	return !*string ? h : stringHash(string + 1, static_cast<unsigned int>((h ^ *string) * static_cast<unsigned long long>(fnv1aPrime)));
}

inline std::string removeSpacesFromString(std::string string)
{
	for (size_t i = 0; (i = string.find(' ', i)) != -1;)
	{
		string.erase(i, 1);
	}

	return string;
}
