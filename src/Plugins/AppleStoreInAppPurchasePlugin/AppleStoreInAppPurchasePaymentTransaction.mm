#include "AppleStoreInAppPurchasePaymentTransaction.h"

#import "Environment/Apple/AppleString.h"

#include "Kernel/Logger.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleStoreInAppPurchasePaymentTransaction::AppleStoreInAppPurchasePaymentTransaction()
        : m_skPaymentTransaction( nil )
        , m_skPaymentQueue( nil )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleStoreInAppPurchasePaymentTransaction::~AppleStoreInAppPurchasePaymentTransaction()
    {
        m_skPaymentTransaction = nil;
        m_skPaymentQueue = nil;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleStoreInAppPurchasePaymentTransaction::setSKPaymentTransaction( SKPaymentTransaction * _skPaymentTransaction )
    {
        m_skPaymentTransaction = _skPaymentTransaction;
        
        SKPayment * payment = m_skPaymentTransaction.payment;
        
        m_productIdentifier = [AppleString NSStringToConstString:payment.productIdentifier];
    }
    //////////////////////////////////////////////////////////////////////////
    SKPaymentTransaction * AppleStoreInAppPurchasePaymentTransaction::getSKPaymentTransaction() const
    {
        return m_skPaymentTransaction;
    }
    /////////////////////////////////////////////////////////////////////////////
    void AppleStoreInAppPurchasePaymentTransaction::setSKPaymentQueue( SKPaymentQueue * _skPaymentQueue )
    {
        m_skPaymentQueue = _skPaymentQueue;
    }
    /////////////////////////////////////////////////////////////////////////////
    SKPaymentQueue * AppleStoreInAppPurchasePaymentTransaction::getSKPaymentQueue() const
    {
        return m_skPaymentQueue;
    }
    /////////////////////////////////////////////////////////////////////////////
    const ConstString & AppleStoreInAppPurchasePaymentTransaction::getProductIdentifier() const
    {
        return m_productIdentifier;
    }
    /////////////////////////////////////////////////////////////////////////////
    void AppleStoreInAppPurchasePaymentTransaction::finish()
    {
        LOGGER_MESSAGE( "payment transaction finish product: %s"
            , m_productIdentifier.c_str()
        );
        
        [m_skPaymentQueue finishTransaction:m_skPaymentTransaction];
    }
    /////////////////////////////////////////////////////////////////////////////
}
