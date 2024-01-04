#pragma once

#include "Interface/ContentInterface.h"
#include "Interface/OutputStreamInterface.h"

#include "Kernel/FilePath.h"
#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PythonFileLogger
        : public Factorable
    {
        DECLARE_FACTORABLE( PythonFileLogger );

    public:
        PythonFileLogger();
        ~PythonFileLogger() override;

    public:
        void setTimestamp( bool _timestamp );
        bool getTimestamp() const;

    public:
        bool initialize( const ContentInterfacePtr & _content );
        void finalize();

    public:
        void write( const Char * _msg );

    protected:
        bool m_timestamp;

        OutputStreamInterfacePtr m_stream;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PythonFileLogger> PythonFileLoggerPtr;
    //////////////////////////////////////////////////////////////////////////
}