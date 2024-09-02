#pragma once

#include "Interface/GraveyardServiceInterface.h"
#include "Interface/TimepipeInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Vector.h"

#include "Config/UniqueId.h"

namespace Mengine
{
    typedef IntrusivePtr<class RenderImageInterface> RenderImageInterfacePtr;

    class GraveyardService
        : public ServiceBase<GraveyardServiceInterface>
        , public TimepipeInterface
    {
    public:
        GraveyardService();
        ~GraveyardService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    protected:
        void onTimepipe( const UpdateContext * _context ) override;

    public:
        void clearTextures() override;

    public:
        bool buryTexture( RenderTextureInterface * _texture ) override;
        RenderTextureInterfacePtr resurrectTexture( const ContentInterfacePtr & _content, const DocumentInterfacePtr & _doc ) override;

    protected:
        void onEngineTextureDestroy( RenderTextureInterface * _texture );

    protected:
        uint32_t m_count;
        float m_graveyardTime;

        struct RenderTextureGraveEntry
        {
            ContentInterfacePtr content;

            RenderImageInterfacePtr image;
            uint32_t width;
            uint32_t height;

            float time;
        };

        typedef Vector<RenderTextureGraveEntry> VectorTextureGrave;
        VectorTextureGrave m_textures;
    };
}