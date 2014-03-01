#	pragma once

#	include <Config/Typedef.h>

namespace Menge
{
	class Resource
	{
	public:
		Resource();

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

	public:
		virtual size_t memoryUse() const;

	protected:
		bool m_compile;
	};
	//////////////////////////////////////////////////////////////////////////
	inline bool Resource::isCompile() const
	{
		return m_compile;
	}
}
