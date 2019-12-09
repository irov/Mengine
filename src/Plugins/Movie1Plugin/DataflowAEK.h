#pragma once

#include "Interface/DataflowInterface.h"
#include "Interface/MemoryInterface.h"
#include "Interface/ArchivatorInterface.h"

#include "MovieKeyFrameInterface.h"
#include "MovieFramePack.h"

#include "Kernel/Factorable.h"
#include "Kernel/Factory.h"

namespace Mengine
{
    class DataflowAEK
        : public DataflowInterface
        , public Factorable
    {
    public:
        DataflowAEK();
        ~DataflowAEK() override;

    public:
        bool initialize() override;
        void finalize() override;

    public:
        bool isThreadFlow() const override;

    public:
        DataInterfacePtr create( const DocumentPtr & _doc ) override;
        MemoryInterfacePtr load( const InputStreamInterfacePtr & _stream, const DocumentPtr & _doc ) override;
        bool flow( const DataInterfacePtr & _data, const MemoryInterfacePtr & _memory, const DocumentPtr & _doc ) override;

    protected:
        bool loadBuffer_( MovieFramePack * _pack, const void * _buffer, size_t _size );

    protected:
        ArchivatorInterfacePtr m_archivator;

        FactoryPtr m_factoryMovieFramePack;
    };
}