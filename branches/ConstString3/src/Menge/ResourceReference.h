#	pragma once

#	include "Loadable.h"

#	include "Resource.h"
#	include "Reference.h"

#	include "ResourceDeclare.h"

#	include "Factorable.h"
#	include "Factory.h"

#	include "Core/ConstString.h"

namespace Menge
{
	class ResourceVisitor;
	
	struct ResourceFactoryParam
	{
		ConstString name;
		ConstString category;
		ConstString group;
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

	protected:
		void loader( BinParser * _parser ) override;
		
	protected:
		void _loaded() override;

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
}
