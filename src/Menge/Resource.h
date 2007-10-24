#	pragma once

#	include <string>

class TiXmlElement;

namespace Menge
{
	class Resource
	{
	public:
		virtual ~Resource(){}

	public:
		virtual bool compile() = 0;
		virtual void release() = 0;

		virtual bool isCompile() = 0;

		virtual void incRef() = 0;
		virtual size_t decRef() = 0;
		virtual size_t refCount() = 0;

		virtual void setName( const std::string & _name ) = 0;
		virtual const std::string & getName() = 0;

	protected:
		virtual bool _compile() = 0;
		virtual void _release() = 0;

	public:
		virtual void loader( TiXmlElement * _xml ) = 0;
	};
}