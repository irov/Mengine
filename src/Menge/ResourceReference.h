#	pragma once

#	include "Resource.h"
#	include "Reference.h"

#	include "ResourceDeclare.h"

class XmlElement;

namespace Menge
{
	class ResourceReference
		: public Resource
		, public Reference
	{
	public:
		ResourceReference( const ResourceFactoryParam & _params );

	public:
		void setName( const std::string & _name );
		const std::string & getName();

	public:
		virtual void loader( XmlElement * _xml );

	protected:
		void _incrementZero() override;
		void _decrementZero() override;

	protected:
		ResourceFactoryParam m_params;
	};
}