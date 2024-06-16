#pragma once

#include "Interface/ServantInterface.h"
#include "Interface/InputStreamInterface.h"
#include "Interface/DocumentInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/Tags.h"
#include "Kernel/VectorConstString.h"
#include "Kernel/FilePath.h"
#include "Kernel/VectorFilePath.h"
#include "Kernel/Resolution.h"
#include "Kernel/AspectRatioViewport.h"
#include "Kernel/VectorAspectRatioViewports.h"
#include "Kernel/Color.h"
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
        virtual bool load( const InputStreamInterfacePtr & _stream, const DocumentInterfacePtr & _doc ) = 0;
        virtual void unload() = 0;

    public:
        virtual bool existValue( const Char * _section, const Char * _key ) const = 0;

    public:
        virtual bool hasValue( const Char * _section, const Char * _key, bool _default, bool * const _value ) const = 0;
        virtual bool hasValue( const Char * _section, const Char * _key, int8_t _default, int8_t * const _value ) const = 0;
        virtual bool hasValue( const Char * _section, const Char * _key, uint8_t _default, uint8_t * const _value ) const = 0;
        virtual bool hasValue( const Char * _section, const Char * _key, int32_t _default, int32_t * const _value ) const = 0;
        virtual bool hasValue( const Char * _section, const Char * _key, uint32_t _default, uint32_t * const _value ) const = 0;
        virtual bool hasValue( const Char * _section, const Char * _key, int64_t _default, int64_t * const _value ) const = 0;
        virtual bool hasValue( const Char * _section, const Char * _key, uint64_t _default, uint64_t * const _value ) const = 0;
        virtual bool hasValue( const Char * _section, const Char * _key, float _default, float * const _value ) const = 0;
        virtual bool hasValue( const Char * _section, const Char * _key, double _default, double * const _value ) const = 0;
        virtual bool hasValue( const Char * _section, const Char * _key, const Char * _default, const Char ** const _value ) const = 0;
        virtual bool hasValue( const Char * _section, const Char * _key, const ConstString & _default, ConstString * const _value ) const = 0;
        virtual bool hasValue( const Char * _section, const Char * _key, const FilePath & _default, FilePath * const _value ) const = 0;
        virtual bool hasValue( const Char * _section, const Char * _key, const Tags & _default, Tags * const _value ) const = 0;
        virtual bool hasValue( const Char * _section, const Char * _key, const Resolution & _default, Resolution * const _value ) const = 0;
        virtual bool hasValue( const Char * _section, const Char * _key, const Color & _default, Color * const _value ) const = 0;

    public:
        virtual bool setValue( const Char * _section, const Char * _key, bool _value ) = 0;
        virtual bool setValue( const Char * _section, const Char * _key, int8_t _value ) = 0;
        virtual bool setValue( const Char * _section, const Char * _key, uint8_t _value ) = 0;
        virtual bool setValue( const Char * _section, const Char * _key, int32_t _value ) = 0;
        virtual bool setValue( const Char * _section, const Char * _key, uint32_t _value ) = 0;
        virtual bool setValue( const Char * _section, const Char * _key, int64_t _value ) = 0;
        virtual bool setValue( const Char * _section, const Char * _key, uint64_t _value ) = 0;
        virtual bool setValue( const Char * _section, const Char * _key, float _value ) = 0;
        virtual bool setValue( const Char * _section, const Char * _key, double _value ) = 0;
        virtual bool setValue( const Char * _section, const Char * _key, const Char * _value ) = 0;
        virtual bool setValue( const Char * _section, const Char * _key, const ConstString & _value ) = 0;
        virtual bool setValue( const Char * _section, const Char * _key, const FilePath & _value ) = 0;
        virtual bool setValue( const Char * _section, const Char * _key, const Tags & _value ) = 0;
        virtual bool setValue( const Char * _section, const Char * _key, const Resolution & _value ) = 0;
        virtual bool setValue( const Char * _section, const Char * _key, const Color & _value ) = 0;

    public:
        virtual bool hasSection( const Char * _section ) const = 0;

    public:
        virtual void getValues( const Char * _section, const Char * _key, VectorAspectRatioViewports * const _values ) const = 0;
        virtual void getValues( const Char * _section, const Char * _key, VectorFilePath * const _values ) const = 0;
        virtual void getValues( const Char * _section, const Char * _key, VectorConstString * const _values ) const = 0;
        virtual void getValues( const Char * _section, const Char * _key, VectorString * const _values ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ConfigInterface> ConfigInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}

