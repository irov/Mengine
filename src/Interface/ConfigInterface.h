#pragma once

#include "Interface/Interface.h"

#include "Kernel/ConstString.h"
#include "Kernel/Tags.h"
#include "Kernel/VectorConstString.h"
#include "Kernel/FilePath.h"
#include "Kernel/VectorFilePath.h"
#include "Kernel/Resolution.h"
#include "Kernel/AspectRatioViewport.h"
#include "Kernel/VectorAspectRatioViewports.h"
#include "Kernel/Color.h"
#include "Kernel/Params.h"

#include "Config/Typedef.h"
#include "Config/VectorString.h"

namespace Mengine
{
    class ConfigInterface
        : public Interface
    {
    public:
        virtual void setPlatformTags( const Tags & _name ) = 0;
        virtual const Tags & getPlatformTags() const = 0;

    public:
        virtual bool hasValue( const Char * _section, const Char * _key, bool * _value ) const = 0;
        virtual bool hasValue( const Char * _section, const Char * _key, int32_t * _value ) const = 0;
        virtual bool hasValue( const Char * _section, const Char * _key, uint32_t * _value ) const = 0;
        virtual bool hasValue( const Char * _section, const Char * _key, uint64_t * _value ) const = 0;
        virtual bool hasValue( const Char * _section, const Char * _key, float * _value ) const = 0;
        virtual bool hasValue( const Char * _section, const Char * _key, double * _value ) const = 0;
        virtual bool hasValue( const Char * _section, const Char * _key, const Char ** _value ) const = 0;
        virtual bool hasValue( const Char * _section, const Char * _key, ConstString * _value ) const = 0;
        virtual bool hasValue( const Char * _section, const Char * _key, FilePath * _value ) const = 0;
        virtual bool hasValue( const Char * _section, const Char * _key, Tags * _value ) const = 0;
        virtual bool hasValue( const Char * _section, const Char * _key, Resolution * _value ) const = 0;
        virtual bool hasValue( const Char * _section, const Char * _key, Color * _value ) const = 0;

    public:
        virtual bool hasSection( const Char * _section ) const = 0;

    public:
        virtual bool getValue( const Char * _section, const Char * _key, bool _default ) const = 0;
        virtual int32_t getValue( const Char * _section, const Char * _key, int32_t _default ) const = 0;
        virtual uint32_t getValue( const Char * _section, const Char * _key, uint32_t _default ) const = 0;
        virtual uint64_t getValue( const Char * _section, const Char * _key, uint64_t _default ) const = 0;
        virtual float getValue( const Char * _section, const Char * _key, float _default ) const = 0;
        virtual double getValue( const Char * _section, const Char * _key, double _default ) const = 0;
        virtual const Char * getValue( const Char * _section, const Char * _key, const Char * _default ) const = 0;
        virtual ConstString getValue( const Char * _section, const Char * _key, const ConstString & _default ) const = 0;
        virtual FilePath getValue( const Char * _section, const Char * _key, const FilePath & _default ) const = 0;
        virtual Tags getValue( const Char * _section, const Char * _key, const Tags & _default ) const = 0;
        virtual Resolution getValue( const Char * _section, const Char * _key, const Resolution & _default ) const = 0;
        virtual Color getValue( const Char * _section, const Char * _key, const Color & _default ) const = 0;

    public:
        virtual void getValues( const Char * _section, const Char * _key, VectorAspectRatioViewports & _values ) const = 0;
        virtual void getValues( const Char * _section, const Char * _key, VectorFilePath & _values ) const = 0;
        virtual void getValues( const Char * _section, const Char * _key, VectorConstString & _values ) const = 0;
        virtual void getValues( const Char * _section, const Char * _key, VectorString & _values ) const = 0;

    public:
        virtual void setValue( const Char * _section, const Char * _key, const Char * _value ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ConfigInterface> ConfigInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}

