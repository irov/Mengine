#include "AppleStoreInAppPurchasePaymentTransaction.h"

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
    void AppleStoreInAppPurchasePaymentTransaction::finish()
    {
        [[SKPaymentQueue defaultQueue] finishTransaction:m_skPaymentTransaction];
    }
    /////////////////////////////////////////////////////////////////////////////
}
