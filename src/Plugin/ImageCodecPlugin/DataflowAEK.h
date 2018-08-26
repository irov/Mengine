#pragma once

#include "Interface/MovieKeyFrameInterface.h"
#include "Interface/DataInterface.h"
#include "Interface/ArchiveInterface.h"

#include "MovieFramePack.h"

#include "Kernel/ServantBase.h"

#include "Kernel/Factory.h"

namespace Mengine
{
    class DataflowAEK
        : public ServantBase<DataflowInterface>
    {
    public:
        DataflowAEK();
        ~DataflowAEK() override;

    public:
        bool initialize() override;
        void finalize() override;

    public:
        DataInterfacePtr create() override;
        bool load( const DataInterfacePtr & _data, const InputStreamInterfacePtr & _stream ) override;

    protected:
        bool loadBuffer_( MovieFramePack * _pack, const void * _buffer, size_t _size );

    protected:
        ArchivatorInterfacePtr m_archivator;

        FactoryPtr m_poolMovieFramePack;
    };
}