#pragma once

#include "Kernel/FactoryWithMutex.h"
#include "Kernel/FilePath.h"

namespace Mengine
{
    class POSIXFileGroupDirectoryFactory
        : public FactoryWithMutex
    {
        DECLARE_FACTORABLE( POSIXFileGroupDirectoryFactory );

    public:
        explicit POSIXFileGroupDirectoryFactory( const FilePath & _relationPath );
        ~POSIXFileGroupDirectoryFactory() override;

    protected:
        Factorable * _createObject() override;
        void _destroyObject( Factorable * _object ) override;

    protected:
        FilePath m_relationPath;
    };
}
