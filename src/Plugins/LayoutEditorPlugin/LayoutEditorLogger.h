#pragma once

#include "Interface/LoggerRecordInterface.h"

#include "Kernel/LoggerBase.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    class LayoutEditorLogger
        : public LoggerBase
    {
        DECLARE_FACTORABLE( LayoutEditorLogger );

    public:
        LayoutEditorLogger();
        ~LayoutEditorLogger() override;

    public:
        const Vector<LoggerRecordInterfacePtr> & getRecords() const;
        void clearRecords();

    protected:
        bool _initializeLogger() override;
        void _finalizeLogger() override;

    protected:
        void _log( const LoggerRecordInterfacePtr & _record ) override;
        void _flush() override;

    protected:
        Vector<LoggerRecordInterfacePtr> m_records;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<LayoutEditorLogger> LayoutEditorLoggerPtr;
    //////////////////////////////////////////////////////////////////////////
}

