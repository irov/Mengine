#pragma once

#include "Interface/AnalyticsServiceInterface.h"

#include "AnalyticsFactoryInterface.h"

#include "Kernel/Hashtable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AnalyticsContext
        : public AnalyticsContextInterface
    {
        DECLARE_FACTORABLE( AnalyticsContext );

    public:
        AnalyticsContext();
        ~AnalyticsContext() override;

    public:
        void setAnalyticsFactory( const AnalyticsFactoryInterfacePtr & _analyticsFactory );
        const AnalyticsFactoryInterfacePtr & getAnalyticsFactory() const;

    public:
        void addParameter( const ConstString & _name, const AnalyticsEventParameterInterfacePtr & _parameter ) override;
        const AnalyticsEventParameterInterfacePtr & getParameter( const ConstString & _name ) const override;

    public:
        void setParameterBoolean( const ConstString & _name, bool _value ) override;
        void setParameterString( const ConstString & _name, const String & _value ) override;
        void setParameterInteger( const ConstString & _name, int64_t _value ) override;
        void setParameterDouble( const ConstString & _name, double _value ) override;

    public:
        void setParameterGetterBoolean( const ConstString & _name, const LambdaAnalyticsParameterGetterBoolean & _lambda ) override;
        void setParameterGetterString( const ConstString & _name, const LambdaAnalyticsParameterGetterString & _lambda ) override;
        void setParameterGetterInteger( const ConstString & _name, const LambdaAnalyticsParameterGetterInteger & _lambda ) override;
        void setParameterGetterDouble( const ConstString & _name, const LambdaAnalyticsParameterGetterDouble & _lambda ) override;

    public:
        AnalyticsContextInterfacePtr resolveContext() const override;

    protected:
        AnalyticsFactoryInterfacePtr m_analyticsFactory;

        typedef Hashtable<ConstString, AnalyticsEventParameterInterfacePtr> HashtableAnalyticsEventParameter;
        HashtableAnalyticsEventParameter m_parameters;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AnalyticsContext, AnalyticsContextInterface> AnalyticsContextPtr;
    //////////////////////////////////////////////////////////////////////////
}