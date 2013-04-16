#include "dbghelp.h"

namespace Menge
{
    class MiniDump
    {
    public:
        MiniDump();
        ~MiniDump();

    public:
        void initialize( const WString & _filePath );

    protected:
        void CreateUniqueDumpFile();        

    protected:
        WString m_filePath;

        static wyWChar m_appname[32];
        static MINIDUMPWRITEDUMP m_dump;
        static HANDLE m_hfile;
        static HMODULE m_hdll;
    };
}