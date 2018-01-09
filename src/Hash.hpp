#pragma once

#include <string> // std::string, std::string_view

#include "MyDefines.hpp"

namespace NHash
{
	constexpr unsigned short MIN_HASH_LENGTH = 4;
	constexpr unsigned short MD5_HASH_LENGTH = 1 << 5;

	class Hash final
	{
		Hash(const Hash&) = default;
		Hash(Hash&&)      = default;

		Hash &operator=(const Hash&) = default;
		Hash &operator=(Hash&&)      = default;

		unsigned int getExistCode(unsigned int)      const noexcept;
		unsigned int getControlSum(std::string_view) const noexcept;

	public:
		explicit Hash(std::string_view);
		~Hash() = default;

		std::string_view getHash(unsigned short = MD5_HASH_LENGTH) const;

	private:
		mutable std::string hash_;
	};
} // namespace NHash
