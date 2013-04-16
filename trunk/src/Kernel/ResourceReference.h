#	pragma once

#	include "Loadable.h"

#	include "Resource.h"
#	include "Identity.h"
#	include "Reference.h"

#	include "ResourceDeclare.h"

#	include "Factory/Factorable.h"
#	include "Factory/Factory.h"

#	include "Core/ConstString.h"
#   include "Core/FilePath.h"

namespace Menge
{
    class ServiceProviderInterface;

    class LogServiceInterface;
    class CodecServiceInterface;

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
        void setServiceProvider( ServiceProviderInterface * _serviceProvider );

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

	public:
		virtual void accept( ResourceVisitor * _visitor ) = 0;

	protected:
		bool _incrementZero() override;
		void _decrementZero() override;

    protected:
        const ConstString & getCodec_( const FilePath & _filename ) const;

	protected:
        ServiceProviderInterface * m_serviceProvider;
        
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
