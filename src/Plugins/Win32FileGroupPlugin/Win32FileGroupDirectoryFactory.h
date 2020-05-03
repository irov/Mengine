#include "Kernel/Factory.h"
#include "Kernel/FilePath.h"

namespace Mengine
{
    class Win32FileGroupDirectoryFactory
        : public Factory
    {
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