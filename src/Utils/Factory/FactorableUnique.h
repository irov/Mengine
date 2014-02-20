#	pragma once

namespace Menge
{
	template<class T>
	class FactorableUnique
		: public T
	{
	public:
		FactorableUnique()
		{
		}

    public:
        void destroy() override
        {
			delete this;
        }
	};
}
