#include "AppleStoreInAppPurchasePaymentTransaction.h"

#import "Environment/Apple/AppleString.h"
#import "Environment/iOS/iOSLog.h"

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
    NSString * AppleStoreInAppPurchasePaymentTransaction::getProductIdentifier() const
    {
        SKPayment * payment = m_skPaymentTransaction.payment;
        
        return payment.productIdentifier;
    }
    /////////////////////////////////////////////////////////////////////////////
    void AppleStoreInAppPurchasePaymentTransaction::finish()
    {
        IOS_LOGGER_MESSAGE( @"payment transaction finish product: %@"
            , this->getProductIdentifier()
        );
        
        [m_skPaymentQueue finishTransaction:m_skPaymentTransaction];
    }
    /////////////////////////////////////////////////////////////////////////////
}
