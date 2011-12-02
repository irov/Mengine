#	pragma once

#	include "Loadable.h"

#	include "Resource.h"
#	include "Identity.h"
#	include "Reference.h"

#	include "ResourceDeclare.h"

#	include "Factorable.h"
#	include "Factory.h"

#	include "Core/ConstString.h"

namespace Menge
{
	class ResourceVisitor;
	
	class ResourceReference
		: public Factorable
		, public Resource
		, public Identity
		, public Reference
		, public Loadable
	{
	public:
		ResourceReference();
		~ResourceReference();

	public:
		void setCategory( const ConstString & _category );
		inline const ConstString & getCategory() const;

		void setGroup( const ConstString & _group );
		inline const ConstString & getGroup() const;

	public:
		virtual bool isValid() const;

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
		ConstString m_category;
		ConstString m_group;
	};
	//////////////////////////////////////////////////////////////////////////
	inline const ConstString & ResourceReference::getCategory() const
	{
		return m_category;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const ConstString & ResourceReference::getGroup() const
	{
		return m_group;
	}
}
