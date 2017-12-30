#pragma once

#include <string>

#include "MyTypedefs.hpp"

namespace NHash
{
	CONST WORD MIN_HASH_LENGTH = 4;
	CONST WORD MD5_HASH_LENGTH = 1 << 5;

		
	class Hash
	{
		Hash(CONST Hash&);
		Hash(Hash&&);

		Hash &operator=(CONST Hash&);
		Hash &operator=(Hash&&);

		WORD getExistCode(WORD)      const _NOEXCEPT;
		WORD getControlSum(CRSTRING) const _NOEXCEPT;

	public:
		explicit Hash(CRSTRING);
		~Hash();

		
		std::string getHash(WORD = MD5_HASH_LENGTH);

	private:
		std::string hash_;
	};
} // namespace NHash
