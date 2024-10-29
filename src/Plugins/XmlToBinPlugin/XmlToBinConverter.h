#pragma once

#include "Interface/ArchivatorInterface.h"
#include "Interface/FileGroupInterface.h"

#include "Kernel/DevelopmentConverter.h"

namespace Mengine
{
    class XmlToBinConverter
        : public DevelopmentConverter
    {
        DECLARE_FACTORABLE( XmlToBinConverter );

    public:
        XmlToBinConverter();
        ~XmlToBinConverter() override;

    public:
        bool _initialize() override;
        void _finalize() override;

    public:
        bool convert() override;

    protected:
        ArchivatorInterfacePtr m_archivator;

        InputStreamInterfacePtr m_stream;
    };
}