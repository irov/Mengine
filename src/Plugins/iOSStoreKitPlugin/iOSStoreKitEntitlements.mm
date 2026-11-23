#import "iOSStoreKitEntitlements.h"

#import "Environment/Apple/AppleKeyChain.h"

#define IOS_STOREKIT_ENTITLEMENTS_KEY @"mengine.billing.purchase.owned.products"

@interface iOSStoreKitEntitlements ()
@property (nonatomic, strong) NSMutableSet<NSString *> * m_entitlements;
@end

@implementation iOSStoreKitEntitlements

+ (instancetype _Nonnull)sharedInstance {
    static iOSStoreKitEntitlements * sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once( &onceToken, ^{
        sharedInstance = [[iOSStoreKitEntitlements alloc] init];
    } );

    return sharedInstance;
}

- (instancetype _Nonnull)init {
    self = [super init];

    if (self == nil) {
        return nil;
    }

    NSSet * entitlements = [AppleKeyChain getSetForKey:IOS_STOREKIT_ENTITLEMENTS_KEY];

    self.m_entitlements = entitlements != nil ? [entitlements mutableCopy] : [NSMutableSet set];

    return self;
}

- (void)add:(NSString * _Nonnull)productIdentifier {
    @synchronized (self) {
        if ([self.m_entitlements containsObject:productIdentifier] == YES) {
            return;
        }

        [self.m_entitlements addObject:productIdentifier];

        [AppleKeyChain setSetForKey:IOS_STOREKIT_ENTITLEMENTS_KEY value:self.m_entitlements];
    }
}

- (void)remove:(NSString * _Nonnull)productIdentifier {
    @synchronized (self) {
        if ([self.m_entitlements containsObject:productIdentifier] == NO) {
            return;
        }

        [self.m_entitlements removeObject:productIdentifier];

        [AppleKeyChain setSetForKey:IOS_STOREKIT_ENTITLEMENTS_KEY value:self.m_entitlements];
    }
}

- (BOOL)isPurchased:(NSString * _Nonnull)productIdentifier {
    @synchronized (self) {
        if ([self.m_entitlements containsObject:productIdentifier] == NO) {
            return NO;
        }

        return YES;
    }
}

@end
