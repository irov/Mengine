#pragma once

#include "Kernel/FactoryWithMutex.h"
#include "Kernel/FilePath.h"

namespace Mengine
{
    class SDLFileGroupDirectoryFactory
        : public FactoryWithMutex
    {
        DECLARE_FACTORABLE( SDLFileGroupDirectoryFactory );

    public:
        SDLFileGroupDirectoryFactory( const FilePath & _relationPath );
        ~SDLFileGroupDirectoryFactory() override;

    protected:
        Factorable * _createObject() override;
        void _destroyObject( Factorable * _obj ) override;

    protected:
        FilePath m_relationPath;
    };
}