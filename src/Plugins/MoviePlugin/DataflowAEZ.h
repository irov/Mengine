#pragma once

#include "Interface/DataflowInterface.h"
#include "Interface/ArchivatorInterface.h"
#include "Interface/FactoryInterface.h"

#include "Kernel/Factorable.h"

#include "movie/movie.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DataflowAEZ
        : public DataflowInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( DataflowAEZ );

    public:
        DataflowAEZ();
        ~DataflowAEZ() override;

    public:
        void setMovieInstance( const aeMovieInstance * _movieInstance );
        const aeMovieInstance * getMovieInstance() const;

    public:
        void setArchivator( const ArchivatorInterfacePtr & _archivator );
        const ArchivatorInterfacePtr & getArchivator() const;

    public:
        bool initialize() override;
        void finalize() override;

    public:
        bool isThreadFlow() const override;

    public:
        DataInterfacePtr create( const DocumentPtr & _doc ) override;

    public:
        MemoryInterfacePtr load( const InputStreamInterfacePtr & _stream, const DocumentPtr & _doc ) override;
        bool flow( const DataInterfacePtr & _data, const MemoryInterfacePtr & _memory, const DataflowContext * _context, const DocumentPtr & _doc ) override;

    protected:
        const aeMovieInstance * m_movieInstance;

        ArchivatorInterfacePtr m_archivator;

        FactoryInterfacePtr m_factoryMovieData;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DataflowAEZ, DataflowInterface> DataflowAEZPtr;
    //////////////////////////////////////////////////////////////////////////
}