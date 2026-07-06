#include "iOSStoreInAppPurchaseProduct.h"

#import "Environment/Apple/AppleString.h"

#include "Config/StdString.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    iOSStoreInAppPurchaseProduct::iOSStoreInAppPurchaseProduct()
        : m_skProduct( nil )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    iOSStoreInAppPurchaseProduct::~iOSStoreInAppPurchaseProduct()
    {
        m_skProduct = nil;
    }
    /////////////////////////////////////////////////////////////////////////////
    void iOSStoreInAppPurchaseProduct::setSKProduct( SKProduct * _skProduct )
    {
        m_skProduct = _skProduct;
    }
    /////////////////////////////////////////////////////////////////////////////
    SKProduct * iOSStoreInAppPurchaseProduct::getSKProduct() const
    {
        return m_skProduct;
    }
    /////////////////////////////////////////////////////////////////////////////
    NSString * iOSStoreInAppPurchaseProduct::getProductIdentifier() const
    {
        return m_skProduct.productIdentifier;
    }
    /////////////////////////////////////////////////////////////////////////////
    NSString * iOSStoreInAppPurchaseProduct::getProductTitle() const
    {
        return m_skProduct.localizedTitle;
    }
    /////////////////////////////////////////////////////////////////////////////
    NSString * iOSStoreInAppPurchaseProduct::getProductDescription() const
    {
        return m_skProduct.localizedDescription;
    }
    /////////////////////////////////////////////////////////////////////////////
    NSString * iOSStoreInAppPurchaseProduct::getProductCurrencyCode() const
    {
        NSLocale * priceLocale = m_skProduct.priceLocale;

        NSString * currencyCode = [priceLocale currencyCode];

        return currencyCode;
    }
    /////////////////////////////////////////////////////////////////////////////
    NSString * iOSStoreInAppPurchaseProduct::getProductPriceFormatted() const
    {
        NSLocale * priceLocale = m_skProduct.priceLocale;

        NSNumberFormatter * formatter = [[NSNumberFormatter alloc] init];
        [formatter setFormatterBehavior:NSNumberFormatterBehavior10_4];
        [formatter setNumberStyle:NSNumberFormatterCurrencyStyle];
        [formatter setLocale:priceLocale];

        NSDecimalNumber * price = m_skProduct.price;

        NSString * priceFormatted = [formatter stringFromNumber:price];

        return priceFormatted;
    }
    /////////////////////////////////////////////////////////////////////////////
    double iOSStoreInAppPurchaseProduct::getProductPrice() const
    {
        NSDecimalNumber * price = m_skProduct.price;

        return [price doubleValue];
    }
    /////////////////////////////////////////////////////////////////////////////
}
