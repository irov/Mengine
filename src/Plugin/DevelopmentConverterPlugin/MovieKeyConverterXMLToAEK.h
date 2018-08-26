#pragma once

#include "DevelopmentConverter.h"

#include "Interface/ArchiveInterface.h"
#include "Interface/DataInterface.h"

#include "Config/Blobject.h"

namespace Mengine
{
    class MovieKeyConverterXMLToAEK
        : public DevelopmentConverter
    {
    public:
        MovieKeyConverterXMLToAEK();
        ~MovieKeyConverterXMLToAEK() override;

    public:
        bool _initialize() override;

    public:
        bool convert() override;

    public:
        bool validateVersion( const InputStreamInterfacePtr & _stream ) const override;

    protected:
        bool loadFramePak_( Blobject & _buffer );
        bool writeFramePak_( const Blobject & _buffer );

    protected:
        ArchivatorInterfacePtr m_archivator;

        DataflowInterfacePtr m_dataflow;
    };
}