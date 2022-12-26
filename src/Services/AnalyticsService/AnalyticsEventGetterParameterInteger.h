#include "Interface/AnalyticsServiceInterface.h"

#include "AnalyticsFactoryInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AnalyticsEventGetterParameterInteger
        : public AnalyticsEventParameterIntegerInterface
    {
        DECLARE_FACTORABLE( AnalyticsEventGetterParameterInteger );

    public:
        AnalyticsEventGetterParameterInteger();
        ~AnalyticsEventGetterParameterInteger() override;

    public:
        void setAnalyticsFactory( const AnalyticsFactoryInterfacePtr & _analyticsFactory );
        const AnalyticsFactoryInterfacePtr & getAnalyticsFactory() const;

    public:
        void setGetter( const LambdaAnalyticsParameterGetterInteger & _lambda );

    public:
        EAnalyticsEventParameterType getType() const override;

    public:
        AnalyticsEventParameterInterfacePtr resolveParameter() const override;

    protected:
        int64_t resolveValue() const override;

    protected:
        AnalyticsFactoryInterfacePtr m_analyticsFactory;

        LambdaAnalyticsParameterGetterInteger m_lambda;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AnalyticsEventGetterParameterInteger, AnalyticsEventParameterInterface> AnalyticsEventGetterParameterIntegerPtr;
    //////////////////////////////////////////////////////////////////////////
}