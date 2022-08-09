#pragma once

#include "Interface/MemoryInterface.h"
#include "Interface/FactoryInterface.h"

#include "AstralaxInterface.h"
#include "AstralaxIncluder.h"
#include "AstralaxEmitter2.h"

#include "Kernel/Factorable.h"
#include "Kernel/VectorResourceImages.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AstralaxService;
    //////////////////////////////////////////////////////////////////////////
    class AstralaxEmitterContainer
        : public AstralaxEmitterContainerInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( AstralaxEmitterContainer );

    public:
        AstralaxEmitterContainer();
        ~AstralaxEmitterContainer() override;

    public:
        bool initialize( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const ArchivatorInterfacePtr & _archivator );
        void finalize();

    public:
        bool isValid() const override;

    public:
        uint32_t getPtcId() const;

    public:
        bool setAtlasResourceImage( uint32_t _index, const ResourceImagePtr & _resourceImage ) override;
        const ResourceImagePtr & getAtlasResourceImage( uint32_t _index ) const override;
        uint32_t getAtlasResourceImageCount() const override;

    public:
        const ResourceImagePtr & findAtlasResourceImage( const Char * _file ) const;

    public:
        HM_EMITTER initialEmitterId() const;
        HM_EMITTER createEmitterId();
        void destroyEmitterId( HM_EMITTER _id );

    protected:
        bool loadContainer_( const void * _buffer, size_t _size, HM_FILE * const _mf ) const;

    protected:
        HM_FILE m_mf;

        uint32_t m_dublicateCount;

        uint32_t m_ptcId;

        MemoryInterfacePtr m_memory;

        VectorResourceImages m_resourceImages;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AstralaxEmitterContainer, AstralaxEmitterContainerInterface> AstralaxEmitterContainerPtr;
    //////////////////////////////////////////////////////////////////////////
}