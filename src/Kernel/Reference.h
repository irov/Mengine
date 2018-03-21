#pragma once

#include "Config/Typedef.h"

namespace Mengine
{
	class Reference
	{
	public:
		Reference();

	public:
		bool incrementReference();
		bool decrementReference();

		uint32_t countReference() const;

	protected:
		virtual bool _incrementZero();
		virtual void _decrementZero();

    protected:
        virtual void _debugIncrementReference();
        virtual void _debugDecrementReference();

	protected:
		uint32_t m_refcounter;
	};
    //////////////////////////////////////////////////////////////////////////
    inline uint32_t Reference::countReference() const
    {
        return m_refcounter;
    }
}
