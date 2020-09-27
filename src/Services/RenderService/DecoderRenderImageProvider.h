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
        void initialize( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const ConstString & _codecType, uint32_t _codecFlags );

    public:
        RenderImageLoaderInterfacePtr getLoader( const DocumentPtr & _doc ) const override;

    protected:
        FileGroupInterfacePtr m_fileGroup;
        FilePath m_filePath;
        ConstString m_codecType;
        uint32_t m_codecFlags;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DecoderRenderImageProvider> DecoderRenderImageProviderPtr;
    //////////////////////////////////////////////////////////////////////////
}
