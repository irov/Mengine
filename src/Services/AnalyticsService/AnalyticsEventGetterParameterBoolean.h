#include "Interface/AnalyticsServiceInterface.h"

#include "AnalyticsFactoryInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AnalyticsEventGetterParameterBoolean
        : public AnalyticsEventParameterBooleanInterface
    {
        DECLARE_FACTORABLE( AnalyticsEventGetterParameterBoolean );

    public:
        AnalyticsEventGetterParameterBoolean();
        ~AnalyticsEventGetterParameterBoolean() override;

    public:
        void setAnalyticsFactory( const AnalyticsFactoryInterfacePtr & _analyticsFactory );
        const AnalyticsFactoryInterfacePtr & getAnalyticsFactory() const;

    public:
        void setGetter( const LambdaAnalyticsParameterGetterBoolean & _lambda );

    public:
        EAnalyticsEventParameterType getType() const override;

    public:
        AnalyticsEventParameterInterfacePtr resolveParameter( const DocumentPtr & _doc ) const override;

    protected:
        bool resolveValue() const override;

    protected:
        AnalyticsFactoryInterfacePtr m_analyticsFactory;

        LambdaAnalyticsParameterGetterBoolean m_lambda;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AnalyticsEventGetterParameterBoolean, AnalyticsEventParameterInterface> AnalyticsEventGetterParameterBooleanPtr;
    //////////////////////////////////////////////////////////////////////////
}