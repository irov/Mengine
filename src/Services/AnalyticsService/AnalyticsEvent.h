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
        void setContext( const AnalyticsContextInterfacePtr & _context ) override;
        const AnalyticsContextInterfacePtr & getContext() const override;

    public:
        void setTimestamp( TimeMilliseconds _timestamp ) override;
        TimeMilliseconds getTimestamp() const override;

    public:
        void addParameter( const ConstString & _name, const AnalyticsEventParameterInterfacePtr & _parameter ) override;

    public:
        void foreachParameters( const LambdaEventParameter & _lambda ) const override;

    protected:
        ConstString m_name;

        AnalyticsContextInterfacePtr m_context;

        TimeMilliseconds m_timestamp;

        struct AnalyticsEventParameterDesc
        {
            ConstString name;
            AnalyticsEventParameterInterfacePtr parameter;
        };

        typedef Vector<AnalyticsEventParameterDesc> VectorAnalyticsEventParameter;
        VectorAnalyticsEventParameter m_parameters;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AnalyticsEvent, AnalyticsEventInterface> AnalyticsEventPtr;
    //////////////////////////////////////////////////////////////////////////
}