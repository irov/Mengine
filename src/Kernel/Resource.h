#	pragma once

namespace Menge
{
	class Resource
	{
	public:
		Resource();

	public:
		virtual bool compile();
		virtual void release();

		inline bool isCompile() const;

		virtual unsigned int memoryUse() const;

	public:
		bool recompile();

	protected:
		void setCompile( bool _value );

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
