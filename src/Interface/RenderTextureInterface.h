#pragma once

#include "Interface/ServantInterface.h"
#include "Interface/FileGroupInterface.h"
#include "Interface/RenderImageInterface.h"

#include "Kernel/Mixin.h"
#include "Kernel/Rect.h"

#include "math/uv4.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderTextureInterface
        : public ServantInterface
    {
    public:
        virtual void release() = 0;

    public:
        virtual const RenderImageInterfacePtr & getImage() const = 0;

    public:
        virtual uint32_t getId() const = 0;

    public:
        virtual const Rect & getRect() const = 0;

        virtual const mt::uv4f & getUV() const = 0;

        virtual void setFileGroup( const FileGroupInterfacePtr & _fileGroup ) = 0;
        virtual const FileGroupInterfacePtr & getFileGroup() const = 0;

        virtual void setFilePath( const FilePath & _filePath ) = 0;
        virtual const FilePath & getFilePath() const = 0;

    public:
        virtual uint32_t getWidth() const = 0;
        virtual uint32_t getHeight() const = 0;

    public:
        virtual float getWidthInv() const = 0;
        virtual float getHeightInv() const = 0;

    public:
        virtual bool isPow2() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderTextureInterface> RenderTextureInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
