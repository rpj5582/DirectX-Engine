#pragma once

#include <Windows.h>
#include <DirectXMath.h>
#include <string>
#include <tuple>

namespace Util
{
	// Store global utility functions here

	// Function taken from here: http://rextester.com/YJB48513
	static constexpr unsigned int fnv1aBasis = 0x811C9DC5;
	static constexpr unsigned int fnv1aPrime = 0x01000193;

	inline constexpr unsigned int stringHash(const char* string, unsigned int h = fnv1aBasis)
	{
		return !*string ? h : stringHash(string + 1, static_cast<unsigned int>((h ^ *string) * static_cast<unsigned long long>(fnv1aPrime)));
	}

	inline void hash_combine(size_t& seed, const size_t& hash) {
		seed ^= hash + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}

	inline std::string removeSpacesFromString(std::string string)
	{
		for (size_t i = 0; (i = string.find(' ', i)) != -1;)
		{
			string.erase(i, 1);
		}

		return string;
	}

	std::string saveFileDialog(HWND hWnd, const char* fileTypeFilter);
	std::string loadFileDialog(HWND hWnd, const char* fileTypeFilter);
}

namespace std
{
	// Float3 hash adapted from this post: https://stackoverflow.com/questions/40256769/vertices-duplication-elimination-in-models
	// Used for hashing XMFLOAT3's when searching through vertices

	inline bool operator==(const DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs)
	{
		if (lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z)
			return true;

		return false;
	}

	template<>
	struct hash<DirectX::XMFLOAT3>
	{
		size_t operator() (const DirectX::XMFLOAT3& float3) const
		{
			auto hasher = hash<float>{};
			size_t hashed_x = hasher(float3.x);
			size_t hashed_y = hasher(float3.y);
			size_t hashed_z = hasher(float3.z);

			size_t seed = 0;
			Util::hash_combine(seed, hashed_x);
			Util::hash_combine(seed, hashed_y);
			Util::hash_combine(seed, hashed_z);
			return seed;
		}
	};

	template<>
	struct hash<tuple<unsigned int, unsigned int, unsigned int>>
	{
		size_t operator() (const tuple<unsigned int, unsigned int, unsigned int>& tuple) const
		{
			auto hasher = hash<unsigned int>{};
			
			size_t num1;
			size_t num2;
			size_t num3;

			tie(num1, num2, num3) = tuple;

			size_t hashed_1 = hasher(num1);
			size_t hashed_2 = hasher(num2);
			size_t hashed_3 = hasher(num3);

			size_t seed = 0;
			Util::hash_combine(seed, hashed_1);
			Util::hash_combine(seed, hashed_2);
			Util::hash_combine(seed, hashed_3);
			return seed;
		}
	};
}
