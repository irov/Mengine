#	pragma once

namespace Menge
{
	class Reference
	{
	public:
		Reference();

	public:
		size_t incrementReference();
		size_t decrementReference();

		size_t countReference() const;

	protected:
		virtual bool _incrementZero();
		virtual void _decrementZero();

	protected:
		size_t m_refcounter;
	};
    //////////////////////////////////////////////////////////////////////////
    inline size_t Reference::countReference() const
    {
        return m_refcounter;
    }
}
