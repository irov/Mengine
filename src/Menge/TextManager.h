#	pragma once

#   include "Interface/TextInterface.h"

#	include "TextLocalePak.h"

#	include "Core/ConstString.h"

#   include "stdex/binary_vector.h"

#	include "Config/Typedef.h"

namespace Menge
{
	class TextManager
        : public TextServiceInterface
	{
	public:
		TextManager();
		~TextManager();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

    public:
		bool initialize( size_t _size ) override;
		
	public:
		bool loadTextEntry( const ConstString & _locale, const ConstString & _pakName, const FilePath & _path ) override;

	public:
		const TextEntry & getTextEntry( const ConstString& _key ) const override;

		bool existText( const ConstString& _key, const TextEntry ** _entry ) const override;
		
		void setDefaultResourceFontName( const ConstString & _fontName ) override;
		const ConstString & getDefaultResourceFontName() const override;

	public:
		void addTextEntry( const ConstString& _key, const TextEntry& _entry );

    protected:
        ServiceProviderInterface * m_serviceProvider;
		
		typedef stdex::binary_vector<ConstString, TextEntry> TMapTextEntry;
		TMapTextEntry m_texts;

		typedef std::vector<TextLocalePak *> TVectorPaks;
		TVectorPaks m_paks;

		ConstString m_defaultResourceFontName;
	};
}
