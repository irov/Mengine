#pragma once

#include "Interface/ServantInterface.h"
#include "Interface/ContentInterface.h"

#include "Kernel/Color.h"
#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"

#include "math/vec2.h"
#include "math/vec3.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SettingInterface
        : public ServantInterface
    {
    public:
        virtual bool initialize( const ContentInterfacePtr & _content, const DocumentInterfacePtr & _doc ) = 0;
        virtual void finalize() = 0;

    public:
        virtual const ContentInterfacePtr & getContent() const = 0;

    public:
        virtual bool getValue( const Char * _key, bool _default ) const = 0;
        virtual int32_t getValue( const Char * _key, int32_t _default ) const = 0;
        virtual uint32_t getValue( const Char * _key, uint32_t _default ) const = 0;
        virtual float getValue( const Char * _key, float _default ) const = 0;
        virtual const Char * getValue( const Char * _key, const Char * _default ) const = 0;
        virtual mt::vec2f getValue( const Char * _key, const mt::vec2f & _default ) const = 0;
        virtual mt::vec3f getValue( const Char * _key, const mt::vec3f & _default ) const = 0;
        virtual Color getValue( const Char * _key, const Color & _default ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SettingInterface> SettingInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}