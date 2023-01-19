#include "AppleStoreInAppPurchasePaymentTransaction.h"

#include "Environment/Apple/AppleString.h"

#include "Kernel/Logger.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleStoreInAppPurchasePaymentTransaction::AppleStoreInAppPurchasePaymentTransaction()
        : m_skPaymentTransaction( nil )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleStoreInAppPurchasePaymentTransaction::~AppleStoreInAppPurchasePaymentTransaction()
    {
        [m_skPaymentTransaction release];
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleStoreInAppPurchasePaymentTransaction::setSKPaymentTransaction( SKPaymentTransaction * _skPaymentTransaction )
    {
        m_skPaymentTransaction = [_skPaymentTransaction retain];
        
        SKPayment * payment = m_skPaymentTransaction.payment;
        
        m_productIdentifier = Helper::NSStringToConstString( payment.productIdentifier );
    }
    //////////////////////////////////////////////////////////////////////////
    SKPaymentTransaction * AppleStoreInAppPurchasePaymentTransaction::getSKPaymentTransaction() const
    {
        return m_skPaymentTransaction;
    }
    /////////////////////////////////////////////////////////////////////////////
    const ConstString & AppleStoreInAppPurchasePaymentTransaction::getProductIdentifier() const
    {
        return m_productIdentifier;
    }
    /////////////////////////////////////////////////////////////////////////////
    void AppleStoreInAppPurchasePaymentTransaction::finish()
    {
        LOGGER_INFO( "inapppurchase", "payment transaction '%s' finish"
            , m_productIdentifier.c_str()
        );
        
        [[SKPaymentQueue defaultQueue] finishTransaction:m_skPaymentTransaction];
    }
    /////////////////////////////////////////////////////////////////////////////
}
