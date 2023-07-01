#include "Interface/AnalyticsServiceInterface.h"

#include "AnalyticsFactoryInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AnalyticsEventGetterParameterConstString
        : public AnalyticsEventParameterConstStringInterface
    {
        DECLARE_FACTORABLE( AnalyticsEventGetterParameterConstString );

    public:
        AnalyticsEventGetterParameterConstString();
        ~AnalyticsEventGetterParameterConstString() override;

    public:
        void setAnalyticsFactory( const AnalyticsFactoryInterfacePtr & _analyticsFactory );
        const AnalyticsFactoryInterfacePtr & getAnalyticsFactory() const;

    public:
        void setGetter( const LambdaAnalyticsParameterGetterConstString & _lambda );

    public:
        EAnalyticsEventParameterType getType() const override;

    public:
        AnalyticsEventParameterInterfacePtr resolveParameter( const DocumentPtr & _doc ) const override;

    protected:
        const ConstString & resolveValue() const override;

    protected:
        AnalyticsFactoryInterfacePtr m_analyticsFactory;

        LambdaAnalyticsParameterGetterConstString m_lambda;

        mutable ConstString m_cache;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AnalyticsEventGetterParameterConstString, AnalyticsEventParameterInterface> AnalyticsEventGetterParameterConstStringPtr;
    //////////////////////////////////////////////////////////////////////////
}