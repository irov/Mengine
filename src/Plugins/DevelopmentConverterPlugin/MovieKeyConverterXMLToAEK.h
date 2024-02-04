#pragma once

#include "Kernel/DevelopmentConverter.h"

#include "Interface/ArchivatorInterface.h"
#include "Interface/DataflowInterface.h"

#include "Kernel/Data.h"

namespace Mengine
{
    class MovieKeyConverterXMLToAEK
        : public DevelopmentConverter
    {
        DECLARE_FACTORABLE( MovieKeyConverterXMLToAEK );

    public:
        MovieKeyConverterXMLToAEK();
        ~MovieKeyConverterXMLToAEK() override;

    public:
        bool _initialize() override;
        void _finalize() override;

    public:
        bool convert() override;

    public:
        bool validateVersion( const InputStreamInterfacePtr & _stream ) const override;

    protected:
        bool loadFramePackage_( Data & _buffer );
        bool writeFramePackage_( const Data & _buffer );

    protected:
        ArchivatorInterfacePtr m_archivator;

        DataflowInterfacePtr m_dataflow;
    };
}