#	pragma once

#	include <string>

class TiXmlElement;

namespace Menge
{
	class Resource
	{
	public:
		Resource();

	public:
		virtual bool compile();
		virtual void release();

		virtual bool isCompile();

	protected:
		virtual bool _compile();
		virtual void _release();

	protected:
		bool m_compile;
	};
}