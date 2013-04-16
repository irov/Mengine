#	pragma once

#   include "Interface/TextInterface.h"

#	include "Core/ConstString.h"

#	include "Config/Typedef.h"

#	include <map>

namespace Menge
{
    class UnicodeServiceInterface;
    class LogServiceInterface;
    class FileServiceInterface;

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
		const TextEntry & getTextEntry( const ConstString& _key ) const override;

		bool existText( const ConstString& _key, const TextEntry ** _entry ) const override;
		void addTextEntry( const ConstString& _key, const TextEntry& _entry ) override;

		void setDefaultResourceFontName( const ConstString & _fontName ) override;
		const ConstString & getDefaultResourceFontName() const override;

    protected:
        ServiceProviderInterface * m_serviceProvider;

		typedef std::map<ConstString, TextEntry> TMapTextEntry;
		TMapTextEntry m_textMap;

		ConstString m_defaultResourceFontName;
	};
}
