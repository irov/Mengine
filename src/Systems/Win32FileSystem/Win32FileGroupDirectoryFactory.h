#include "Kernel/FactoryWithMutex.h"
#include "Kernel/FilePath.h"

namespace Mengine
{
    class Win32FileGroupDirectoryFactory
        : public FactoryWithMutex
    {
        DECLARE_FACTORABLE( Win32FileGroupDirectoryFactory );

    public:
        Win32FileGroupDirectoryFactory( const FilePath & _relationPath );
        ~Win32FileGroupDirectoryFactory() override;

    protected:
        Factorable * _createObject() override;
        void _destroyObject( Factorable * _obj ) override;

    protected:
        FilePath m_relationPath;
    };
}