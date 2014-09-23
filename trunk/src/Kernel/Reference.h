#	pragma once

#	include <Config/Typedef.h>

namespace Menge
{
	class Reference
	{
	public:
		Reference();

	public:
		uint32_t incrementReference();
		uint32_t decrementReference();

		uint32_t countReference() const;

	protected:
		virtual bool _incrementZero();
		virtual void _decrementZero();

	protected:
		uint32_t m_refcounter;
	};
    //////////////////////////////////////////////////////////////////////////
    inline uint32_t Reference::countReference() const
    {
        return m_refcounter;
    }
}
