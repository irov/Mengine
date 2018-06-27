#pragma once

#include "Kernel/Loadable.h"

#include "Kernel/Servant.h"
#include "Kernel/Compilable.h"
#include "Kernel/Identity.h"
#include "Kernel/Reference.h"

#include "Kernel/Scriptable.h"
#include "Kernel/Visitable.h"
#include "Kernel/Comable.h"

#include "Kernel/ResourceHolder.h"

#include "Factory/Factorable.h"
#include "Factory/Factory.h"

#include "Core/ConstString.h"
#include "Core/FilePath.h"

namespace Mengine
{
    class ResourceVisitor;

    typedef IntrusivePtr<class FileGroupInterface> FileGroupInterfacePtr;
    	
	class Resource
		: public Servant
		, public Compilable
		, public Identity
		, public Reference
		, public Loadable
        , public Scriptable
		, public Visitable
	{
		DECLARE_VISITABLE_BASE();

	public:
		Resource();
		~Resource() override;

	public:
		void setLocale( const ConstString & _locale );
		inline const ConstString & getLocale() const;

		void setCategory( const FileGroupInterfacePtr & _category );
		inline const FileGroupInterfacePtr & getCategory() const;

		void setGroupName( const ConstString & _groupName );
		inline const ConstString & getGroupName() const;

	public:
		bool initialize();

	protected:
		virtual bool _initialize();

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
		bool convertDefault2_( const ConstString & _converter, const FilePath & _path, FilePath & _out );
		bool convertDefault_( const ConstString & _converter, const FilePath & _path, FilePath & _out, ConstString & _codecType );

	public:
		bool cache();
		void uncache();

	public:
		inline bool isCache() const;

	protected:
		virtual void _cache();
		virtual void _uncache();

	protected:
		bool _incrementZero() override;
		void _decrementZero() override;

	protected:       
		ConstString m_locale;
        FileGroupInterfacePtr m_category;
		ConstString m_groupName;

		bool m_cache;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef IntrusivePtr<Resource> ResourcePtr;
	//////////////////////////////////////////////////////////////////////////
	inline const ConstString & Resource::getLocale() const
	{
		return m_locale;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const FileGroupInterfacePtr & Resource::getCategory() const
	{
		return m_category;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const ConstString & Resource::getGroupName() const
	{
		return m_groupName;
	}
	//////////////////////////////////////////////////////////////////////////
	inline bool Resource::isCache() const
	{
		return m_cache;
	}
}
