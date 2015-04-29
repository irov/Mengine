#	pragma once

#	include "Kernel/Loadable.h"

#	include "Kernel/Resource.h"
#	include "Kernel/Identity.h"
#	include "Kernel/Reference.h"

#	include "Kernel/Scriptable.h"

#	include "Kernel/ResourceDeclare.h"
#	include "Kernel/ResourceHolder.h"

#	include "Factory/Factorable.h"
#	include "Factory/Factory.h"

#	include "Core/ConstString.h"
#   include "Core/FilePath.h"

namespace Menge
{
    class ServiceProviderInterface;

	class ResourceVisitor;
	
	class ResourceReference
		: public Factorable
		, public Resource
		, public Identity
		, public Reference
		, public Loadable
        , public Scriptable
	{
	public:
		ResourceReference();
		~ResourceReference();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider );
		ServiceProviderInterface * getServiceProvider() const;

	public:
		void setCategory( const ConstString & _category );
		inline const ConstString & getCategory() const;

		void setGroup( const ConstString & _group );
		inline const ConstString & getGroup() const;

	public:
		bool isValid() const;

    protected:
        virtual bool _isValid() const;

	public:
		bool _loader( const Metabuf::Metadata * _parser ) override;
		
    public:
        bool convert();

    protected:
        virtual bool _convert();

	protected:
		bool convertDefault2_( const ConstString & _converter, const ConstString & _path, ConstString & _out );
		bool convertDefault_( const ConstString & _converter, const ConstString & _path, ConstString & _out, ConstString & _codecType );

	public:
		bool cache();
		void uncache();

	public:
		inline bool isCache() const;

	protected:
		virtual void _cache();
		virtual void _uncache();

	public:
		virtual void accept( ResourceVisitor * _visitor ) = 0;

	protected:
		bool _incrementZero() override;
		void _decrementZero() override;

    protected:
		pybind::object _embedded() override;

	protected:
        ServiceProviderInterface * m_serviceProvider;
        
		ConstString m_category;
		ConstString m_group;

		bool m_cache;
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
	//////////////////////////////////////////////////////////////////////////
	inline bool ResourceReference::isCache() const
	{
		return m_cache;
	}
}
