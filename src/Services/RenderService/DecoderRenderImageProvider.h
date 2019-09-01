#pragma once

#include "Interface/RenderImageInterface.h"
#include "Interface/FileGroupInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"

namespace Mengine
{
    class DecoderRenderImageProvider
        : public RenderImageProviderInterface
        , public Factorable
    {
    public:
        DecoderRenderImageProvider();
        ~DecoderRenderImageProvider() override;

    public:
        void initialize( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const ConstString & _codecName );

    public:
        RenderImageLoaderInterfacePtr getLoader() const override;

    protected:
        FileGroupInterfacePtr m_fileGroup;
        FilePath m_filePath;
        ConstString m_codecName;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DecoderRenderImageProvider> DecoderRenderImageProviderPtr;
    //////////////////////////////////////////////////////////////////////////
}
