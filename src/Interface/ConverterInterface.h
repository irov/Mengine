#pragma once

#include "Config/Typedef.h"

#include "Interface/Interface.h"
#include "Interface/InputStreamInterface.h"
#include "Interface/FileGroupInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"

#include "Config/String.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct ConverterOptions
    {
        FileGroupInterfacePtr fileGroup;

        FilePath inputFilePath;
        FilePath outputFilePath;
    };
    //////////////////////////////////////////////////////////////////////////
    class ConverterInterface
        : public Interface
    {
    public:
        virtual bool initialize() = 0;

    public:
        virtual void setOptions( ConverterOptions * _options ) = 0;

    public:
        virtual const ConstString & getConvertExt() const = 0;

    public:
        virtual bool validateVersion( const InputStreamInterfacePtr & _stream ) const = 0;

    public:
        virtual bool convert() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ConverterInterface> ConverterInterfacePtr;
    //////////////////////////////////////////////////////////////////////////  
}
