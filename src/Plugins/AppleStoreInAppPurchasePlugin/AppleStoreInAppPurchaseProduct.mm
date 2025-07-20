#include "AppleStoreInAppPurchaseProduct.h"

#import "Environment/Apple/AppleString.h"

#include "Config/StdString.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleStoreInAppPurchaseProduct::AppleStoreInAppPurchaseProduct()
        : m_skProduct( nil )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleStoreInAppPurchaseProduct::~AppleStoreInAppPurchaseProduct()
    {
        m_skProduct = nil;
    }
    /////////////////////////////////////////////////////////////////////////////
    void AppleStoreInAppPurchaseProduct::setSKProduct( SKProduct * _skProduct )
    {
        m_skProduct = _skProduct;
    }
    /////////////////////////////////////////////////////////////////////////////
    SKProduct * AppleStoreInAppPurchaseProduct::getSKProduct() const
    {
        return m_skProduct;
    }
    /////////////////////////////////////////////////////////////////////////////
    NSString * AppleStoreInAppPurchaseProduct::getProductIdentifier() const
    {
        return m_skProduct.productIdentifier;
    }
    /////////////////////////////////////////////////////////////////////////////
    NSString * AppleStoreInAppPurchaseProduct::getProductTitle() const
    {
        return m_skProduct.localizedTitle;
    }
    /////////////////////////////////////////////////////////////////////////////
    NSString * AppleStoreInAppPurchaseProduct::getProductDescription() const
    {
        return m_skProduct.localizedDescription;
    }
    /////////////////////////////////////////////////////////////////////////////
    NSString * AppleStoreInAppPurchaseProduct::getProductCurrencyCode() const
    {
        NSLocale * priceLocale = m_skProduct.priceLocale;
        
        NSString * currencyCode = [priceLocale currencyCode];
        
        return currencyCode;
    }
    /////////////////////////////////////////////////////////////////////////////
    NSString * AppleStoreInAppPurchaseProduct::getProductPriceFormatted() const
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
    double AppleStoreInAppPurchaseProduct::getProductPrice() const
    {
        NSDecimalNumber * price = m_skProduct.price;
        
        return [price doubleValue];
    }
    /////////////////////////////////////////////////////////////////////////////
}
