#pragma once

#include "Interface/AnalyticsServiceInterface.h"

#include "AnalyticsFactoryInterface.h"

#include "Kernel/Vector.h"

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

    public:
        uint32_t getCountParameters() const override;

        void foreachParameters( const LambdaForeachParameters & _lambda ) const override;

    public:
        void addParameterBoolean( const ConstString & _name, bool _value, const DocumentInterfacePtr & _doc ) override;
        void addParameterInteger( const ConstString & _name, int64_t _value, const DocumentInterfacePtr & _doc ) override;
        void addParameterDouble( const ConstString & _name, double _value, const DocumentInterfacePtr & _doc ) override;
        void addParameterString( const ConstString & _name, const String & _value, const DocumentInterfacePtr & _doc ) override;
        void addParameterConstString( const ConstString & _name, const ConstString & _value, const DocumentInterfacePtr & _doc ) override;

    public:
        void addParameterGetterBoolean( const ConstString & _name, const LambdaAnalyticsParameterGetterBoolean & _lambda, const DocumentInterfacePtr & _doc ) override;
        void addParameterGetterInteger( const ConstString & _name, const LambdaAnalyticsParameterGetterInteger & _lambda, const DocumentInterfacePtr & _doc ) override;
        void addParameterGetterDouble( const ConstString & _name, const LambdaAnalyticsParameterGetterDouble & _lambda, const DocumentInterfacePtr & _doc ) override;
        void addParameterGetterString( const ConstString & _name, const LambdaAnalyticsParameterGetterString & _lambda, const DocumentInterfacePtr & _doc ) override;
        void addParameterGetterConstString( const ConstString & _name, const LambdaAnalyticsParameterGetterConstString & _lambda, const DocumentInterfacePtr & _doc ) override;

    public:
        AnalyticsContextInterfacePtr resolveContext( const DocumentInterfacePtr & _doc ) const override;

    protected:
        AnalyticsFactoryInterfacePtr m_analyticsFactory;

        struct AnalyticsEventParameterDesc
        {
            ConstString name;
            AnalyticsEventParameterInterfacePtr parameter;
        };

        typedef Vector<AnalyticsEventParameterDesc> VectorAnalyticsEventParameter;
        VectorAnalyticsEventParameter m_parameters;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AnalyticsContext, AnalyticsContextInterface> AnalyticsContextPtr;
    //////////////////////////////////////////////////////////////////////////
}