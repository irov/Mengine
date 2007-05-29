#	pragma once

#	include "Resource.h"

namespace Menge
{
	class ResourceImpl
		: public Resource
	{
	public:
		ResourceImpl( const std::string & _name );

	public:
		bool compile();
		void release();

		bool isCompile();

		void incRef();
		size_t decRef();
		size_t refCount();

		void setName( const std::string & _name );
		const std::string & getName();

	public:
		void loader(TiXmlElement *xml);

	protected:
		bool _compile();
		void _release();

	protected:
		bool m_compile;
		size_t m_refcounter;

		std::string m_name;
	};
}