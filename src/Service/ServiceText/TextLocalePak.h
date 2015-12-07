# pragma once

#	include "Interface/MemoryInterface.h"

#	include "Core/ConstString.h"
#	include "Core/String.h"
#	include "Core/FilePath.h"

#	include "Factory/FactorablePtr.h"

namespace Menge
{
	class TextLocalePack
		: public FactorablePtr
	{
	public:
		TextLocalePack();
		~TextLocalePack();

	public:
		bool initialize( ServiceProviderInterface * _serviceProvider, const ConstString & _locale, const ConstString & _pakName, const FilePath & _path );
		
	public:
		const ConstString & getLocale() const;
		const ConstString & getPackName() const;
		const FilePath & getPath() const;

	public:
		MemoryInterfacePtr getXmlBuffer() const;

	protected:
		ServiceProviderInterface * m_serviceProvider;

		ConstString m_locale;
		ConstString m_pakName;
		FilePath m_path;

		MemoryInterfacePtr m_memory;		
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<TextLocalePack> TextLocalePackPtr;
}
