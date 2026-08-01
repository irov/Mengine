#pragma once

#include "Interface/OutputStreamInterface.h"

#include "Kernel/Data.h"
#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MCPMemoryOutputStream
        : public OutputStreamInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( MCPMemoryOutputStream );

    public:
        MCPMemoryOutputStream();
        ~MCPMemoryOutputStream() override;

    public:
        size_t write( const void * _data, size_t _size ) override;
        size_t size() const override;
        bool flush() override;

    public:
        const Data & getData() const;

    protected:
        Data m_data;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MCPMemoryOutputStream, OutputStreamInterface> MCPMemoryOutputStreamPtr;
    //////////////////////////////////////////////////////////////////////////
}
