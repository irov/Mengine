#include "iOSStoreInAppPurchasePaymentTransaction.h"

#import "Environment/Apple/AppleString.h"
#import "Environment/iOS/iOSLog.h"

#include "Kernel/Logger.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    iOSStoreInAppPurchasePaymentTransaction::iOSStoreInAppPurchasePaymentTransaction()
        : m_skPaymentTransaction( nil )
        , m_skPaymentQueue( nil )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    iOSStoreInAppPurchasePaymentTransaction::~iOSStoreInAppPurchasePaymentTransaction()
    {
        m_skPaymentTransaction = nil;
        m_skPaymentQueue = nil;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSStoreInAppPurchasePaymentTransaction::setSKPaymentTransaction( SKPaymentTransaction * _skPaymentTransaction )
    {
        m_skPaymentTransaction = _skPaymentTransaction;
    }
    //////////////////////////////////////////////////////////////////////////
    SKPaymentTransaction * iOSStoreInAppPurchasePaymentTransaction::getSKPaymentTransaction() const
    {
        return m_skPaymentTransaction;
    }
    /////////////////////////////////////////////////////////////////////////////
    void iOSStoreInAppPurchasePaymentTransaction::setSKPaymentQueue( SKPaymentQueue * _skPaymentQueue )
    {
        m_skPaymentQueue = _skPaymentQueue;
    }
    /////////////////////////////////////////////////////////////////////////////
    SKPaymentQueue * iOSStoreInAppPurchasePaymentTransaction::getSKPaymentQueue() const
    {
        return m_skPaymentQueue;
    }
    /////////////////////////////////////////////////////////////////////////////
    NSString * iOSStoreInAppPurchasePaymentTransaction::getProductIdentifier() const
    {
        SKPayment * payment = m_skPaymentTransaction.payment;

        return payment.productIdentifier;
    }
    /////////////////////////////////////////////////////////////////////////////
    void iOSStoreInAppPurchasePaymentTransaction::finish()
    {
        IOS_LOGGER_MESSAGE( @"payment transaction finish product: %@"
            , this->getProductIdentifier()
        );

        [m_skPaymentQueue finishTransaction:m_skPaymentTransaction];
    }
    /////////////////////////////////////////////////////////////////////////////
}
