#include "Interface/AnalyticsServiceInterface.h"

#include "AnalyticsFactory.h"

#include "Kernel/Vector.h"
#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AnalyticsEvent
        : public AnalyticsEventInterface
    {
        DECLARE_FACTORABLE( AnalyticsEvent );

    public:
        AnalyticsEvent();
        ~AnalyticsEvent() override;

    public:
        void setName( const ConstString & _name );
        const ConstString & getName() const override;

    public:
        void setTimestamp( Timestamp _timestamp ) override;
        Timestamp getTimestamp() const override;

    public:
        void setContext( const AnalyticsContextInterfacePtr & _context ) override;
        const AnalyticsContextInterfacePtr & getContext() const override;

    public:
        void setLocalContext( const AnalyticsContextInterfacePtr & _localContext ) override;
        const AnalyticsContextInterfacePtr & getLocalContext() const override;

    public:
        void setGlobalContext( const AnalyticsContextInterfacePtr & _globalContext ) override;
        const AnalyticsContextInterfacePtr & getGlobalContext() const override;

    public:
        uint32_t getCountParameters() const override;
        void foreachParameters( const LambdaEventParameter & _lambda ) const override;

    protected:
        ConstString m_name;

        Timestamp m_timestamp;

        AnalyticsContextInterfacePtr m_context;
        AnalyticsContextInterfacePtr m_localContext;
        AnalyticsContextInterfacePtr m_globalContext;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AnalyticsEvent, AnalyticsEventInterface> AnalyticsEventPtr;
    //////////////////////////////////////////////////////////////////////////
}