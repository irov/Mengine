#include "Interface/AnalyticsServiceInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AnalyticsEventConstParameterString
        : public AnalyticsEventParameterStringInterface
    {
        DECLARE_FACTORABLE( AnalyticsEventConstParameterString );

    public:
        AnalyticsEventConstParameterString();
        ~AnalyticsEventConstParameterString() override;

    public:
        void setValue( const String & _value );

    public:
        EAnalyticsEventParameterType getType() const override;

    public:
        AnalyticsEventParameterInterfacePtr resolveParameter( const DocumentInterfacePtr & _doc ) const override;

    protected:
        const String & resolveValue() const override;

    protected:
        String m_value;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AnalyticsEventConstParameterString, AnalyticsEventParameterInterface> AnalyticsEventConstParameterStringPtr;
    //////////////////////////////////////////////////////////////////////////
}