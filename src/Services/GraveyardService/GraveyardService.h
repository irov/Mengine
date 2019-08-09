#pragma once

#include "Interface/GraveyardServiceInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Observable.h"

#include "Config/Vector.h"

namespace Mengine
{
    typedef IntrusivePtr<class RenderImageInterface> RenderImageInterfacePtr;

    class GraveyardService
        : public ServiceBase<GraveyardServiceInterface>
        , public Observable
    {
    public:
        GraveyardService();
        ~GraveyardService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        void tick( const UpdateContext * _context ) override;

    public:
        void clearTextures() override;

    public:
        bool buryTexture( RenderTextureInterface * _texture ) override;
        RenderTextureInterfacePtr resurrectTexture( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const Char * _doc ) override;

    protected:
        void onEngineTextureDestroy( RenderTextureInterface * _texture );

    protected:
        uint32_t m_count;
        float m_graveyardTime;

        struct RenderTextureGraveEntry
        {
            FileGroupInterfacePtr category;
            FilePath filePath;

            RenderImageInterfacePtr image;
            uint32_t width;
            uint32_t height;

            float time;
        };

        typedef Vector<RenderTextureGraveEntry> VectorTextureGrave;
        VectorTextureGrave m_textures;

    protected:
        void updateTexture_( RenderTextureGraveEntry * _entry, float _timing );
    };
}