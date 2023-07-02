#include "Interface/AnalyticsServiceInterface.h"

#include "AnalyticsFactoryInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AnalyticsEventGetterParameterDouble
        : public AnalyticsEventParameterDoubleInterface
    {
        DECLARE_FACTORABLE( AnalyticsEventGetterParameterDouble );

    public:
        AnalyticsEventGetterParameterDouble();
        ~AnalyticsEventGetterParameterDouble() override;

    public:
        void setAnalyticsFactory( const AnalyticsFactoryInterfacePtr & _analyticsFactory );
        const AnalyticsFactoryInterfacePtr & getAnalyticsFactory() const;

    public:
        void setGetter( const LambdaAnalyticsParameterGetterDouble & _lambda );

    public:
        EAnalyticsEventParameterType getType() const override;

    public:
        AnalyticsEventParameterInterfacePtr resolveParameter( const DocumentInterfacePtr & _doc ) const override;

    protected:
        double resolveValue() const override;

    protected:
        AnalyticsFactoryInterfacePtr m_analyticsFactory;

        LambdaAnalyticsParameterGetterDouble m_lambda;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AnalyticsEventGetterParameterDouble, AnalyticsEventParameterInterface> AnalyticsEventGetterParameterDoublePtr;
    //////////////////////////////////////////////////////////////////////////
}