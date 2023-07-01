#include "Interface/AnalyticsServiceInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AnalyticsEventConstParameterDouble
        : public AnalyticsEventParameterDoubleInterface
    {
        DECLARE_FACTORABLE( AnalyticsEventConstParameterDouble );

    public:
        AnalyticsEventConstParameterDouble();
        ~AnalyticsEventConstParameterDouble() override;

    public:
        void setValue( double _value );

    public:
        EAnalyticsEventParameterType getType() const override;

    public:
        AnalyticsEventParameterInterfacePtr resolveParameter( const DocumentPtr & _doc ) const override;

    protected:
        double resolveValue() const override;

    protected:
        double m_value;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AnalyticsEventConstParameterDouble, AnalyticsEventParameterInterface> AnalyticsEventConstParameterDoublePtr;
    //////////////////////////////////////////////////////////////////////////
}