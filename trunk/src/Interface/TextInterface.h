#   pragma once

#   include "Interface/ServiceInterface.h"

#   include "Core/ConstString.h"

namespace Menge
{
    struct TextEntry
    {
        WString text;
        ConstString font;
        float charOffset;
        float lineOffset;
    };

    class TextServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE("TextService")

    public:
        virtual void addTextEntry( const ConstString& _key, const TextEntry& _entry ) = 0;
        virtual const TextEntry & getTextEntry( const ConstString& _key ) const = 0;
        virtual bool existText( const ConstString& _key, const TextEntry ** _entry ) const = 0;

    public:
        virtual void setDefaultResourceFontName( const ConstString & _fontName ) = 0;
        virtual const ConstString & getDefaultResourceFontName() const = 0;
    };

#   define TEXT_SERVICE( serviceProvider )\
    (Menge::getService<Menge::TextServiceInterface>(serviceProvider))
}