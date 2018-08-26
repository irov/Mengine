#pragma once

#include "Interface/RenderImageInterface.h"
#include "Interface/FileGroupInterface.h"

#include "Kernel/ServantBase.h"

#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"

namespace Mengine
{
    class DecoderRenderImageProvider
        : public ServantBase<RenderImageProviderInterface>
    {
    public:
        DecoderRenderImageProvider();
        ~DecoderRenderImageProvider() override;

    public:
        void initialize( const FileGroupInterfacePtr& _fileGroup, const FilePath & _fileName, const ConstString & _codecName );

    public:
        RenderImageLoaderInterfacePtr getLoader() const override;

    protected:
        FileGroupInterfacePtr m_fileGroup;
        FilePath m_fileName;
        ConstString m_codecName;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DecoderRenderImageProvider> DecoderRenderImageProviderPtr;
    //////////////////////////////////////////////////////////////////////////
}
