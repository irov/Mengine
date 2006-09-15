#	pragma once

namespace Utility
{
	template<int N>
	struct Enumerator
	{
		static size_t NextValue()
		{
			++GetValue();
			return GetValue();
		}

		static size_t CurrentValue()
		{
			return GetValue();
		}

		static void Reset()
		{
			GetValue() = 0;
		}

		static size_t & GetValue()
		{
			static size_t s_Value;
			return s_Value;
		}
	};
}