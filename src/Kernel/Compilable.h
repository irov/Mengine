#pragma once

#include "Config/Typedef.h"
#include "Config/Lambda.h"

#include "Kernel/Mixin.h"
#include "Kernel/IntrusivePtr.h"

namespace Mengine
{
	class Compilable
        : public Mixin
	{
	public:
		Compilable();
        ~Compilable();

	public:
		inline bool isCompile() const;

	public:
		virtual bool compile();
		virtual void release();

	public:
		bool recompile( const Lambda<> & _lambda );
		
	protected:
		virtual bool _compile();
		virtual void _release();

		virtual void _recompile();
        virtual void _uncompile();

	protected:
		bool m_compile;
	};
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Compilable> CompilablePtr;
	//////////////////////////////////////////////////////////////////////////
	inline bool Compilable::isCompile() const
	{
		return m_compile;
	}
}
