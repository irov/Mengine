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
        
        m_productIdentifier = [AppleString NSStringToConstString:m_skProduct.productIdentifier];
        m_productTitle = [AppleString NSStringToString:m_skProduct.localizedTitle];
        m_productDescription = [AppleString NSStringToString:m_skProduct.localizedDescription];
        
        NSLocale * priceLocale = m_skProduct.priceLocale;
        
        NSString * currencyCode = [priceLocale currencyCode];
        
        m_productCurrencyCode = [AppleString NSStringToString:currencyCode];
        
        NSNumberFormatter * formatter = [[NSNumberFormatter alloc] init];
        [formatter setFormatterBehavior:NSNumberFormatterBehavior10_4];
        [formatter setNumberStyle:NSNumberFormatterCurrencyStyle];
        [formatter setLocale:priceLocale];
        
        NSDecimalNumber * price = m_skProduct.price;
        
        NSString * priceFormatted = [formatter stringFromNumber:price];
        
        m_productPriceFormatted = [AppleString NSStringToString:priceFormatted];
        
        m_productPrice = [price doubleValue];
    }
    /////////////////////////////////////////////////////////////////////////////
    SKProduct * AppleStoreInAppPurchaseProduct::getSKProduct() const
    {
        return m_skProduct;
    }
    /////////////////////////////////////////////////////////////////////////////
    const ConstString & AppleStoreInAppPurchaseProduct::getProductIdentifier() const
    {
        return m_productIdentifier;
    }
    /////////////////////////////////////////////////////////////////////////////
    const String & AppleStoreInAppPurchaseProduct::getProductTitle() const
    {
        return m_productTitle;
    }
    /////////////////////////////////////////////////////////////////////////////
    const String & AppleStoreInAppPurchaseProduct::getProductDescription() const
    {
        return m_productDescription;
    }
    /////////////////////////////////////////////////////////////////////////////
    const String & AppleStoreInAppPurchaseProduct::getProductCurrencyCode() const
    {
        return m_productCurrencyCode;
    }
    /////////////////////////////////////////////////////////////////////////////
    const String & AppleStoreInAppPurchaseProduct::getProductPriceFormatted() const
    {
        return m_productPriceFormatted;
    }
    /////////////////////////////////////////////////////////////////////////////
    double AppleStoreInAppPurchaseProduct::getProductPrice() const
    {
        return m_productPrice;
    }
    /////////////////////////////////////////////////////////////////////////////
}
