#	pragma once

#	include "Loadable.h"

#	include "Resource.h"
#	include "Reference.h"

#	include "ResourceDeclare.h"

#	include "Factory/Factorable.h"
#	include "Factory/Factory.h"

namespace Menge
{
	class ResourceVisitor;
	
	struct ResourceFactoryParam
	{
		Menge::String name;
		Menge::String category;
		Menge::String group;
		Menge::String file;
	};

	class ResourceReference
		: public Factorable
		, public Resource
		, public Reference
		, public Loadable
	{
	public:
		virtual void accept(ResourceVisitor * _visitor) = 0;

	public:
		ResourceReference();
		~ResourceReference();

	public:
		void initialize( const ResourceFactoryParam & _params );

	public:
		void setName( const String& _name );
		const String& getName() const;

		const ResourceFactoryParam& getFactoryParams() const;

	public:
		void loader( XmlElement * _xml ) override;
		void parser( BinParser * _parser ) override;

	protected:
		bool _incrementZero() override;
		void _decrementZero() override;

	protected:
		ResourceFactoryParam m_params;
	};
}
