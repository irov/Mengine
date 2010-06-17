#	pragma once

#	include "Loadable.h"

#	include "Resource.h"
#	include "Reference.h"

#	include "ResourceDeclare.h"

#	include "Core/ConstString.h"

#	include "Factory/Factorable.h"
#	include "Factory/Factory.h"

namespace Menge
{
	class ResourceVisitor;
	
	struct ResourceFactoryParam
	{
		ConstString name;
		ConstString category;
		ConstString group;
		ConstString file;
	};

	class ResourceReference
		: public Factorable
		, public Resource
		, public Reference
		, public Loadable
	{
	public:
		ResourceReference();
		~ResourceReference();

	public:
		void initialize( const ResourceFactoryParam & _params );

	public:
		inline const ConstString & getName() const;
		inline const ConstString & getCategory() const;
		inline const ConstString & getGroup() const;
		inline const ConstString & getFile() const;

	public:
		void loader( XmlElement * _xml ) override;
		void parser( BinParser * _parser ) override;

	public:
		virtual void accept( ResourceVisitor * _visitor ) = 0;

	protected:
		bool _incrementZero() override;
		void _decrementZero() override;

	protected:
		ResourceFactoryParam m_param;
	};
	//////////////////////////////////////////////////////////////////////////
	inline const ConstString & ResourceReference::getName() const
	{
		return m_param.name;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const ConstString & ResourceReference::getCategory() const
	{
		return m_param.category;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const ConstString & ResourceReference::getGroup() const
	{
		return m_param.group;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const ConstString & ResourceReference::getFile() const
	{
		return m_param.file;
	}
}
