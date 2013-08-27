#   pragma once

#   include "Interface/ServiceInterface.h"

#   include "Config/String.h"

#   include "Core/ConstString.h"
#   include "Core/FilePath.h"

namespace Menge
{
    struct TextEntry
    {
        ConstString text;
        ConstString font;
        float charOffset;
        float lineOffset;
    };

    class TextServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE("TextService")

    public:
		virtual bool initialize( size_t _size ) = 0;

	public:
		virtual bool loadTextEntry( const ConstString & _locale, const ConstString & _pakName, const FilePath & _path ) = 0;

	public:        
        virtual const TextEntry & getTextEntry( const ConstString& _key ) const = 0;
        virtual bool existText( const ConstString& _key, const TextEntry ** _entry ) const = 0;

    public:
        virtual void setDefaultResourceFontName( const ConstString & _fontName ) = 0;
        virtual const ConstString & getDefaultResourceFontName() const = 0;
    };

#   define TEXT_SERVICE( serviceProvider )\
    (Menge::Helper::getService<Menge::TextServiceInterface>(serviceProvider))
}