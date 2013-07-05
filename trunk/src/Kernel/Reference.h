#	pragma once

namespace Menge
{
	class Reference
	{
	public:
		Reference();

	public:
		unsigned int incrementReference();
		unsigned int decrementReference();

		unsigned int countReference();

	protected:
		virtual bool _incrementZero();
		virtual void _decrementZero();

	protected:
		unsigned int m_refcounter;
	};
    //////////////////////////////////////////////////////////////////////////
    inline unsigned int Reference::countReference()
    {
        return m_refcounter;
    }
}
