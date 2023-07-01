#include "Interface/AnalyticsServiceInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AnalyticsEventConstParameterInteger
        : public AnalyticsEventParameterIntegerInterface
    {
        DECLARE_FACTORABLE( AnalyticsEventConstParameterInteger );

    public:
        AnalyticsEventConstParameterInteger();
        ~AnalyticsEventConstParameterInteger() override;

    public:
        void setValue( int64_t _value );

    public:
        EAnalyticsEventParameterType getType() const override;

    public:
        AnalyticsEventParameterInterfacePtr resolveParameter( const DocumentPtr & _doc ) const override;

    protected:
        int64_t resolveValue() const override;

    protected:
        int64_t m_value;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AnalyticsEventConstParameterInteger, AnalyticsEventParameterInterface> AnalyticsEventConstParameterIntegerPtr;
    //////////////////////////////////////////////////////////////////////////
}