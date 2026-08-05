#pragma once

#include "Kernel/DevelopmentConverter.h"
#include "Kernel/Magic.h"

#include "Interface/ArchivatorInterface.h"

namespace Mengine
{
    class ArchiveDevelopmentConverter
        : public DevelopmentConverter
    {
        DECLARE_FACTORABLE( ArchiveDevelopmentConverter );

    public:
        ArchiveDevelopmentConverter( const ConstString & _convertExt, const ArchivatorInterfacePtr & _archivator, magic_number_type _magicNumber, magic_version_type _magicVersion, bool _crc32, EArchivatorCompress _compress );
        ~ArchiveDevelopmentConverter() override;

    protected:
        bool _initialize() override;
        void _finalize() override;

    public:
        bool convert() override;

    protected:
        bool validateVersion( const InputStreamInterfacePtr & _stream ) const override;

    protected:
        magic_number_type m_magicNumber;
        magic_version_type m_magicVersion;
        bool m_crc32;
        EArchivatorCompress m_compress;
        ArchivatorInterfacePtr m_archivator;
    };
}
