#include "Interface/AnalyticsServiceInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AnalyticsEventConstParameterBoolean
        : public AnalyticsEventParameterBooleanInterface
    {
        DECLARE_FACTORABLE( AnalyticsEventConstParameterBoolean );

    public:
        AnalyticsEventConstParameterBoolean();
        ~AnalyticsEventConstParameterBoolean() override;

    public:
        void setValue( bool _value );

    public:
        EAnalyticsEventParameterType getType() const override;

    public:
        AnalyticsEventParameterInterfacePtr resolveParameter( const DocumentPtr & _doc ) const override;

    protected:
        bool resolveValue() const override;

    protected:
        bool m_value;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AnalyticsEventConstParameterBoolean, AnalyticsEventParameterInterface> AnalyticsEventConstParameterBooleanPtr;
    //////////////////////////////////////////////////////////////////////////
}