#include "Interface/AnalyticsServiceInterface.h"

#include "AnalyticsFactoryInterface.h"

#include "Kernel/Vector.h"
#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AnalyticsEventBuilder
        : public AnalyticsEventBuilderInterface
    {
        DECLARE_FACTORABLE( AnalyticsEventBuilder );

    public:
        AnalyticsEventBuilder();
        ~AnalyticsEventBuilder() override;

    public:
        void setAnalyticsFactory( const AnalyticsFactoryInterfacePtr & _analyticsFactory );
        const AnalyticsFactoryInterfacePtr & getAnalyticsFactory() const;

    public:
        void setContext( const AnalyticsContextInterfacePtr & _context );
        const AnalyticsContextInterfacePtr & getContext() const;

    public:
        void setGlobalContext( const AnalyticsContextInterfacePtr & _globalContext );
        const AnalyticsContextInterfacePtr & getGlobalContext() const;

    public:
        void setEventCategory( EAnalyticsEventCategory _category );
        EAnalyticsEventCategory getEventCategory() const;

    public:
        void setEventName( const ConstString & _eventName );
        const ConstString & getEventName() const;

    public:
        void addParameter( const ConstString & _name, const AnalyticsEventParameterInterfacePtr & _parameter );

    public:
        void setLocalContext( const AnalyticsContextInterfacePtr & _context ) override;
        const AnalyticsContextInterfacePtr & getLocalContext() const override;

    public:
        AnalyticsEventBuilderInterface * addParameterBoolean( const ConstString & _name, bool _value ) override;
        AnalyticsEventBuilderInterface * addParameterInteger( const ConstString & _name, int64_t _value ) override;
        AnalyticsEventBuilderInterface * addParameterDouble( const ConstString & _name, double _value ) override;
        AnalyticsEventBuilderInterface * addParameterString( const ConstString & _name, const String & _value ) override;
        AnalyticsEventBuilderInterface * addParameterConstString( const ConstString & _name, const ConstString & _value ) override;

    public:
        Timestamp log() const override;

    protected:
        AnalyticsFactoryInterfacePtr m_analyticsFactory;
        AnalyticsContextInterfacePtr m_context;
        AnalyticsContextInterfacePtr m_globalContext;

        ConstString m_eventName;

        EAnalyticsEventCategory m_category;

        AnalyticsContextInterfacePtr m_localContext;

        struct AnalyticsEventParameterDesc
        {
            ConstString name;
            AnalyticsEventParameterInterfacePtr parameter;
        };

        typedef Vector<AnalyticsEventParameterDesc> VectorAnalyticsEventParameter;
        VectorAnalyticsEventParameter m_parameters;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AnalyticsEventBuilder, AnalyticsEventBuilderInterface> AnalyticsEventBuilderPtr;
    //////////////////////////////////////////////////////////////////////////
}