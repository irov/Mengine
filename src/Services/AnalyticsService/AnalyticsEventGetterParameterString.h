#include "Interface/AnalyticsServiceInterface.h"

#include "AnalyticsFactoryInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AnalyticsEventGetterParameterString
        : public AnalyticsEventParameterStringInterface
    {
        DECLARE_FACTORABLE( AnalyticsEventGetterParameterString );

    public:
        AnalyticsEventGetterParameterString();
        ~AnalyticsEventGetterParameterString() override;

    public:
        void setAnalyticsFactory( const AnalyticsFactoryInterfacePtr & _analyticsFactory );
        const AnalyticsFactoryInterfacePtr & getAnalyticsFactory() const;

    public:
        void setGetter( const LambdaAnalyticsParameterGetterString & _lambda );

    public:
        EAnalyticsEventParameterType getType() const override;

    public:
        AnalyticsEventParameterInterfacePtr resolveParameter() const override;

    protected:
        const String & resolveValue() const override;

    protected:
        AnalyticsFactoryInterfacePtr m_analyticsFactory;

        LambdaAnalyticsParameterGetterString m_lambda;

        mutable String m_cache;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AnalyticsEventGetterParameterString, AnalyticsEventParameterInterface> AnalyticsEventGetterParameterStringPtr;
    //////////////////////////////////////////////////////////////////////////
}