#	pragma once

namespace Menge
{
	//! Reference - ��������� ��� �������.

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

	protected:
		bool m_compile;
	};
	//////////////////////////////////////////////////////////////////////////
	inline bool Resource::isCompile() const
	{
		return m_compile;
	}
}
