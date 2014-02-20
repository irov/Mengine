#	pragma once

namespace Menge
{
	template<class T>
	class FactorableStore
		: public T
	{
	public:
		FactorableUnique()
		{
		}

    public:
        void destroy() override
        {
			//Empty
        }
	};
}
