#pragma once

#include "Interface/ServantInterface.h"
#include "Interface/InputStreamInterface.h"

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
#include "Kernel/VectorString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ConfigInterface
        : public ServantInterface
    {
    public:
        virtual void setPlatformTags( const Tags & _name ) = 0;
        virtual const Tags & getPlatformTags() const = 0;

    public:
        virtual bool load( const InputStreamInterfacePtr & _stream, const DocumentPtr & _doc ) = 0;

    public:
        virtual bool existValue( const Char * _section, const Char * _key ) const = 0;

    public:
        virtual bool hasValue( const Char * _section, const Char * _key, bool * const _value ) const = 0;
        virtual bool hasValue( const Char * _section, const Char * _key, int8_t * const _value ) const = 0;
        virtual bool hasValue( const Char * _section, const Char * _key, uint8_t * const _value ) const = 0;
        virtual bool hasValue( const Char * _section, const Char * _key, int32_t * const _value ) const = 0;
        virtual bool hasValue( const Char * _section, const Char * _key, uint32_t * const _value ) const = 0;
        virtual bool hasValue( const Char * _section, const Char * _key, int64_t * const _value ) const = 0;
        virtual bool hasValue( const Char * _section, const Char * _key, uint64_t * const _value ) const = 0;
        virtual bool hasValue( const Char * _section, const Char * _key, float * const _value ) const = 0;
        virtual bool hasValue( const Char * _section, const Char * _key, double * const _value ) const = 0;
        virtual bool hasValue( const Char * _section, const Char * _key, const Char ** const _value ) const = 0;
        virtual bool hasValue( const Char * _section, const Char * _key, ConstString * const _value ) const = 0;
        virtual bool hasValue( const Char * _section, const Char * _key, FilePath * const _value ) const = 0;
        virtual bool hasValue( const Char * _section, const Char * _key, Tags * const _value ) const = 0;
        virtual bool hasValue( const Char * _section, const Char * _key, Resolution * const _value ) const = 0;
        virtual bool hasValue( const Char * _section, const Char * _key, Color * const _value ) const = 0;

    public:
        virtual bool hasSection( const Char * _section ) const = 0;

    public:
        virtual bool getValue( const Char * _section, const Char * _key, bool _default ) const = 0;
        virtual int8_t getValue( const Char * _section, const Char * _key, int8_t _default ) const = 0;
        virtual uint8_t getValue( const Char * _section, const Char * _key, uint8_t _default ) const = 0;
        virtual int32_t getValue( const Char * _section, const Char * _key, int32_t _default ) const = 0;
        virtual uint32_t getValue( const Char * _section, const Char * _key, uint32_t _default ) const = 0;
        virtual int64_t getValue( const Char * _section, const Char * _key, int64_t _default ) const = 0;
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
        virtual void getValues( const Char * _section, const Char * _key, VectorAspectRatioViewports * const _values ) const = 0;
        virtual void getValues( const Char * _section, const Char * _key, VectorFilePath * const _values ) const = 0;
        virtual void getValues( const Char * _section, const Char * _key, VectorConstString * const _values ) const = 0;
        virtual void getValues( const Char * _section, const Char * _key, VectorString * const _values ) const = 0;

    public:
        virtual void setValue( const Char * _section, const Char * _key, const Char * _value ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ConfigInterface> ConfigInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}

