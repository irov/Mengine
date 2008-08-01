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
		virtual ~ResourceReference();

	public:
		void setName( const std::string & _name );
		const std::string & getName();

		const ResourceFactoryParam& getFactoryParams() const;

	public:
		virtual void loader( XmlElement * _xml );

	protected:
		bool _incrementZero() override;
		void _decrementZero() override;

	protected:
		ResourceFactoryParam m_params;
	};
}