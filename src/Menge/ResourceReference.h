#	pragma once

#	include "Resource.h"
#	include "Reference.h"

#	include "ResourceDeclare.h"

class XmlElement;

namespace Menge
{
	class ResourceVisitor;

	class ResourceReference
		: public Resource
		, public Reference
	{
	public:
		virtual void accept(ResourceVisitor * _visitor) = 0;

	public:
		ResourceReference( const ResourceFactoryParam & _params );
		virtual ~ResourceReference();

	public:
		void setName( const String& _name );
		const String& getName() const;

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