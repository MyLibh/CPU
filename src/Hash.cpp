// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <cassert> // assert

#include "Hash.hpp"

namespace NHash
{
#pragma region Hash

	Hash::Hash(std::string_view str) :
		hash_(str)
	{ }

#pragma endregion

	unsigned int Hash::getExistCode(unsigned int x) const noexcept
	{
		x += 256;
		while (!(((x < 57) && (x >= 48)) || ((x <= 90) && (x >= 65)) || ((x <= 122) && (x >= 97))))
			if (x < 48) x += 24;
			else        x -= 47;

		return x;
	}

	unsigned int Hash::getControlSum(std::string_view str) const noexcept
	{
		size_t length = str.length();

		unsigned int sault = 0,
		             mod   = 1 << length;
		for (size_t i = 0; i < length; ++i) sault += (str[i] * (1 << (i + 1))) % mod;
		
		return sault % mod;
	}

	std::string_view Hash::getHash(unsigned short hashLength /* = MD5_HASH_LENGTH */) const
	{
		assert(hashLength >= MIN_HASH_LENGTH);

		unsigned int minLength     = 2,
					 realMinLength = 0,
					 strSault      = getControlSum(hash_),
					 strLength     = static_cast<unsigned int>(hash_.length());

		while (minLength <= hashLength) realMinLength = (minLength <<= 1); // Get the length of a string that is a multiple of the power of two and closest to the length of the hash

		while (minLength < strLength) minLength <<= 1; // Get the number of the form 2 ^ n that is closest to the original length of the string
		
		if(minLength < (strLength << 1)) minLength <<= 1;

		std::string tmp(hash_);
		for (size_t i = 0, symbolsToAdd = minLength - strLength; i < symbolsToAdd; ++i) // Add missing items
		{
			if (i + 1 == strLength)
			{
				i -= strLength;

				symbolsToAdd -= strLength;
			}

			assert(((i + 1) % strLength < strLength) && ((i % strLength) < strLength));
			tmp += static_cast<char>(getExistCode(hash_[i % strLength] + hash_[(i + 1) % strLength]));
		}

		unsigned int maxSault  = getControlSum(tmp),
			         maxLength = tmp.length();
		
		hash_.clear();
		while (tmp.length() != realMinLength)
		{
			for (size_t i = 0, mid = (tmp.length() >> 1); i < mid; ++i)
			{
				assert(mid != i && mid + i < tmp.length());

				hash_ += static_cast<char>(getExistCode(tmp[mid - 1] + tmp[mid + 1]));
			}

			tmp = hash_;
			hash_.clear();
		}

		unsigned int symbolsToSub = realMinLength - hashLength;
		for (size_t i = 0, compressCount = realMinLength / symbolsToSub; hash_.length() < static_cast<unsigned int>(hashLength - 4); ++i) // Removing redundant items
		{
			if (i % compressCount == 0) hash_ += static_cast<char>(getExistCode(tmp[i] + tmp[++i]));
			else                        hash_ += static_cast<char>(getExistCode(tmp[i]));
		}

		hash_ += static_cast<char>(getExistCode(strSault));
		hash_ += static_cast<char>(getExistCode(strLength));

		hash_ += static_cast<char>(getExistCode(maxSault));
		hash_ += static_cast<char>(getExistCode(maxLength));

		return hash_;
	}
} // namespace NHash