#import "AppleStoreInAppPurchaseEntitlements.h"

#import "Environment/iOS/iOSLog.h"
#import "Environment/Apple/AppleKeyChain.h"

#define APPLE_STOREINAPPPURCHASE_ENTITLEMENTS_KEY @"mengine.billing.purchase.owned.products"

@interface AppleStoreInAppPurchaseEntitlements ()
@property (nonatomic, strong) NSMutableSet<NSString *> * m_entitlements;
@end

@implementation AppleStoreInAppPurchaseEntitlements

#pragma mark -

+ (instancetype _Nonnull)sharedInstance {
    static AppleStoreInAppPurchaseEntitlements * sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [[AppleStoreInAppPurchaseEntitlements alloc] init];
    });
    return sharedInstance;
}

- (instancetype _Nonnull)init {
    self = [super init];
    
    if (self) {
        NSSet * entitlements = [AppleKeyChain getSetForKey:APPLE_STOREINAPPPURCHASE_ENTITLEMENTS_KEY];
        
        self.m_entitlements = entitlements != nil ? [entitlements mutableCopy] : [NSMutableSet set];
    }
    
    return self;
}

- (void)add:(NSString * _Nonnull)productIdentifier {
    @synchronized (self) {
        [self.m_entitlements addObject:productIdentifier];
    }
    
    [AppleKeyChain setSetForKey:APPLE_STOREINAPPPURCHASE_ENTITLEMENTS_KEY value:self.m_entitlements];
}

- (void)remove:(NSString * _Nonnull)productIdentifier {
    @synchronized (self) {
        [self.m_entitlements removeObject:productIdentifier];
    }
    
    [AppleKeyChain setSetForKey:APPLE_STOREINAPPPURCHASE_ENTITLEMENTS_KEY value:self.m_entitlements];
}

- (BOOL)isPurchased:(NSString * _Nonnull)productIdentifier {
    @synchronized (self) {
        return [self.m_entitlements containsObject:productIdentifier];
    }
}

@end
