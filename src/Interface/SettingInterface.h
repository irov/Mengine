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
    enum ESettingType
    {
        EST_NONE,
        EST_BOOL,
        EST_INTEGER,
        EST_REAL,
        EST_STRING,
        EST_VEC2F,
        EST_VEC3F,
        EST_COLOR
    };
    //////////////////////////////////////////////////////////////////////////
    class SettingInterface
        : public ServantInterface
    {
    public:
        virtual bool initialize( const ConstString & _name, const ContentInterfacePtr & _content, const DocumentInterfacePtr & _doc ) = 0;
        virtual void finalize() = 0;

    public:
        virtual bool save( const DocumentInterfacePtr & _doc ) const = 0;

    public:
        virtual const ConstString & getName() const = 0;
        virtual const ContentInterfacePtr & getContent() const = 0;

    public:
        typedef Lambda<void( const Char * )> LambdaKeys;
        virtual void foreachKeys( const LambdaKeys & _lambda ) const = 0;

    public:
        virtual ESettingType getValueType( const Char * _key ) const = 0;

    public:
        virtual bool getValueBoolean( const Char * _key, bool _default ) const = 0;
        virtual int64_t getValueInteger( const Char * _key, int64_t _default ) const = 0;
        virtual double getValueFloat( const Char * _key, double _default ) const = 0;
        virtual const Char * getValueString( const Char * _key, const Char * _default ) const = 0;
        virtual mt::vec2f getValueVec2f( const Char * _key, const mt::vec2f & _default ) const = 0;
        virtual mt::vec3f getValueVec3f( const Char * _key, const mt::vec3f & _default ) const = 0;
        virtual Color getValueColor( const Char * _key, const Color & _default ) const = 0;

    public:
        virtual bool setValueBoolean( const Char * _key, bool _value ) = 0;
        virtual bool setValueInteger( const Char * _key, int64_t _value ) = 0;
        virtual bool setValueFloat( const Char * _key, double _value ) = 0;
        virtual bool setValueString( const Char * _key, const Char * _value ) = 0;
        virtual bool setValueVec2f( const Char * _key, const mt::vec2f & _value ) = 0;
        virtual bool setValueVec3f( const Char * _key, const mt::vec3f & _value ) = 0;
        virtual bool setValueColor( const Char * _key, const Color & _value ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SettingInterface> SettingInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}