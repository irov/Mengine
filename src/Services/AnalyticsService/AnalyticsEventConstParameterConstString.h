#include "Interface/AnalyticsServiceInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AnalyticsEventConstParameterConstString
        : public AnalyticsEventParameterConstStringInterface
    {
        DECLARE_FACTORABLE( AnalyticsEventConstParameterString );

    public:
        AnalyticsEventConstParameterConstString();
        ~AnalyticsEventConstParameterConstString() override;

    public:
        void setValue( const ConstString & _value );

    public:
        EAnalyticsEventParameterType getType() const override;

    public:
        AnalyticsEventParameterInterfacePtr resolveParameter() const override;

    protected:
        const ConstString & resolveValue() const override;

    protected:
        ConstString m_value;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AnalyticsEventConstParameterConstString, AnalyticsEventParameterInterface> AnalyticsEventConstParameterConstStringPtr;
    //////////////////////////////////////////////////////////////////////////
}