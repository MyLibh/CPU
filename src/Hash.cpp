// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <cassert>

#include "Hash.hpp"

namespace NHash
{
	Hash::Hash(CRSTRING str) :
		hash_(str)
	{ }

	Hash::~Hash()
	{ }

	WORD Hash::getExistCode(WORD x) const _NOEXCEPT
	{
		x += 256;
		while (!(((x < 57) && (x >= 48)) || ((x <= 90) && (x >= 65)) || ((x <= 122) && (x >= 97))))
			if (x < 48) x += 24;
			else        x -= 47;

		return x;
	}

	WORD Hash::getControlSum(CRSTRING str) const _NOEXCEPT
	{
		WORD   sault  = 0;
		SIZE_T length = str.length();
		auto   mod    = 1 << length;
		for (SIZE_T i = 0; i < length; ++i) sault += static_cast<WORD>((str[i] * (1 << (i + 1))) % mod);
		
		return sault % mod;
	}

	std::string Hash::getHash(WORD hashLength /* = MD5_HASH_LENGTH */)
	{
		assert(hashLength >= MIN_HASH_LENGTH);

		DWORD minLength     = 2,
			  realMinLength = 0,
			  strSault      = getControlSum(hash_),
			  strLength     = static_cast<DWORD>(hash_.length());

		while (minLength <= hashLength) realMinLength = (minLength <<= 1); // Get the length of a string that is a multiple of the power of two and closest to the length of the hash

		while (minLength < strLength) minLength <<= 1; // Get the number of the form 2 ^ n that is closest to the original length of the string
		
		if(minLength < (strLength << 1)) minLength <<= 1;

		std::string tmp(hash_);
		for (SIZE_T i = 0, symbolsToAdd = minLength - strLength; i < symbolsToAdd; ++i) // Add missing items
		{
			if (i + 1 == strLength)
			{
				i -= strLength;

				symbolsToAdd -= strLength;
			}

			assert(((i + 1) % strLength < strLength) && ((i % strLength) < strLength));
			tmp += static_cast<CHAR>(getExistCode(hash_[i % strLength] + hash_[(i + 1) % strLength]));
		}

		DWORD maxSault  = getControlSum(tmp),
			  maxLength = static_cast<DWORD>(tmp.length());
		
		hash_.clear();
		while (tmp.length() != realMinLength)
		{
			for (SIZE_T i = 0, mid = (tmp.length() >> 1); i < mid; ++i)
			{
				assert(mid - i > 0 && mid + i < tmp.length());

				hash_ += static_cast<CHAR>(getExistCode(tmp[mid - 1] + tmp[mid + 1]));
			}

			tmp = hash_;
			hash_.clear();
		}

		WORD symbolsToSub = realMinLength - hashLength;
		for (SIZE_T i = 0, compressCount = realMinLength / symbolsToSub; hash_.length() < static_cast<WORD>(hashLength - 4); ++i) // Removing redundant items
		{
			if (i % compressCount == 0) hash_ += static_cast<CHAR>(getExistCode(tmp[i] + tmp[++i]));
			else                        hash_ += static_cast<CHAR>(getExistCode(tmp[i]));
		}

		hash_ += static_cast<CHAR>(getExistCode(strSault));
		hash_ += static_cast<CHAR>(getExistCode(strLength));

		hash_ += static_cast<CHAR>(getExistCode(maxSault));
		hash_ += static_cast<CHAR>(getExistCode(maxLength));

		return hash_;
	}
} // namespace NHash