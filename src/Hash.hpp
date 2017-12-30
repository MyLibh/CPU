#pragma once

#include <string>

#include "MyTypedefs.hpp"

namespace NHash
{
	CONST WORD MIN_HASH_LENGTH = 4;
	CONST WORD MD5_HASH_LENGTH = 1 << 5;

	class Hash
	{
		WORD getExistCode(WORD)      const _NOEXCEPT;
		WORD getControlSum(CRSTRING) const _NOEXCEPT;

	public:
		explicit Hash(CRSTRING);
		Hash(CONST Hash&) = delete;
		Hash(Hash&&)      = delete;
		~Hash();

		Hash &operator=(CONST Hash&) = delete;
		Hash &operator=(Hash&&)      = delete;

		std::string getHash(WORD = MD5_HASH_LENGTH);

	private:
		std::string hash_;
	};
} // namespace NHash
