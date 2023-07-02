#pragma once

#include "Config/Typedef.h"

#include "Interface/ServiceInterface.h"
#include "Interface/DecoderFactoryInterface.h"
#include "Interface/EncoderFactoryInterface.h"

#include "Kernel/DocumentHelper.h"
#include "Kernel/Factorable.h"
#include "Kernel/FilePath.h"
#include "Kernel/DecoderInterfacePointer.h"
#include "Kernel/EncoderInterfacePointer.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class CodecServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "CodecService" )

    public:
        virtual DecoderInterfacePointer createDecoder( const ConstString & _type, const DocumentInterfacePtr & _doc ) = 0;

    public:
        virtual EncoderInterfacePointer createEncoder( const ConstString & _type, const DocumentInterfacePtr & _doc ) = 0;

    public:
        virtual bool registerCodecExt( const ConstString & _ext, const ConstString & _codecType ) = 0;
        virtual void removeCodecExt( const ConstString & _ext ) = 0;
        virtual const ConstString & findCodecType( const FilePath & _filePath ) const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define CODEC_SERVICE()\
    ((Mengine::CodecServiceInterface *)SERVICE_GET(Mengine::CodecServiceInterface))
//////////////////////////////////////////////////////////////////////////