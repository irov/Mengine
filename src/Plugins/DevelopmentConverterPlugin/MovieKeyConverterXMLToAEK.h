#pragma once

#include "DevelopmentConverter.h"

#include "Interface/ArchivatorInterface.h"
#include "Interface/DataflowInterface.h"

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
        bool loadFramePackage_( Blobject & _buffer );
        bool writeFramePackage_( const Blobject & _buffer );

    protected:
        ArchivatorInterfacePtr m_archivator;

        DataflowInterfacePtr m_dataflow;
    };
}