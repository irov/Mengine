#	pragma once

namespace Menge
{
	//! Reference - интерфейс для ресурса.

	class Resource
	{
	public:
		Resource();

	public:
		virtual bool compile();
		virtual void release();

		inline bool isCompile() const;

	public:
		bool recompile();

	protected:
		virtual bool _compile();
		virtual void _release();
		virtual void _recompile();

	protected:
		bool m_compile;
	};
	//////////////////////////////////////////////////////////////////////////
	inline bool Resource::isCompile() const
	{
		return m_compile;
	}
}
