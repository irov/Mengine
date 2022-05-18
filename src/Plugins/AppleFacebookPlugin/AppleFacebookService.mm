#include "AppleFacebookService.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/Logger.h"

////////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AppleFacebookService, Mengine::AppleFacebookService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleFacebookService::AppleFacebookService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleFacebookService::~AppleFacebookService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleFacebookService::login(){
        FBSDKLoginManager *loginManager = [FBSDKLoginManager new];
        FBSDKLoginConfiguration *configuration =
          [[FBSDKLoginConfiguration alloc] initWithPermissions:@[@"email", @"user_friends", @"user_birthday", @"user_age_range", @"user_hometown", @"user_location", @"user_gender", @"user_link"]
                                                      tracking:FBSDKLoginTrackingLimited
                                                         nonce:@"123"];
        [loginManager logInFromViewController:self
                                configuration:configuration
                                   completion:^(FBSDKLoginManagerLoginResult * result, NSError *error) {
          if (!error && !result.isCancelled) {
            // Login successful

            // getting user ID
            NSString *userID =
              FBSDKProfile.currentProfile.userID;

            // getting id token string
            NSString *idTokenString =
              FBSDKAuthenticationToken.currentAuthenticationToken.tokenString;

            // fetching pre-populated email
            NSString *email = FBSDKProfile.currentProfile.email;
          
            // getting pre-populated friends list
            NSArray<FBSDKUserIdentifier *> *friendIDs = FBSDKProfile.currentProfile.friendIDs;

            // getting pre-populated user birthday
            NSDate *birthday = FBSDKProfile.currentProfile.birthday;

            // getting pre-populated age range
            FBSDKUserAgeRange *ageRange = FBSDKProfile.currentProfile.ageRange;
          
            // getting pre-populated age range
            FBSDKLocation *hometown = FBSDKProfile.currentProfile.hometown;
          
            // getting pre-populated age range
            FBSDKLocation *location = FBSDKProfile.currentProfile.location;
          
            // getting pre-populated age range
            NSString *gender = FBSDKProfile.currentProfile.gender;
          
            // getting pre-populated age range
            NSURL *userLink = FBSDKProfile.currentProfile.linkURL;
          }
        }];
        
    }
    /////////////////////////////////////////////////////////////////////////////
    bool AppleFacebookService::_initializeService()
    {
        return true;
    }
    ////////////////////////////////////////////////////////////////////////
    void AppleFacebookService::_finalizeService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
}
