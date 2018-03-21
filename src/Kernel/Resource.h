#pragma once

#include "Config/Typedef.h"

namespace Mengine
{
	class Resource
	{
	public:
		Resource();
        ~Resource();

	public:
		inline bool isCompile() const;

	public:
		virtual bool compile();
		virtual void release();

	public:
		bool recompile();
		
	protected:
		virtual bool _compile();
		virtual void _release();

		virtual void _recompile();
        virtual void _uncompile();

	protected:
		bool m_compile;
	};
	//////////////////////////////////////////////////////////////////////////
	inline bool Resource::isCompile() const
	{
		return m_compile;
	}
}
