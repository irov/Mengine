package org.Mengine.Build.Facebook;

import com.facebook.AccessToken;

/**
 * Created by sweatcoin7 on 6/25/18.
 */

public class FacebookAccessTokenWrapper {
    public String expires;
    public String[] permissions;
    public String[] declinedPermissions;
    public String token;
    public String lastRefresh;
    public String applicationId;
    public String userId;

    public FacebookAccessTokenWrapper(AccessToken accessToken) {
        expires = accessToken.getExpires().toString();
        permissions = accessToken.getPermissions()
                .toArray(new String[accessToken.getPermissions().size()]);
        declinedPermissions = accessToken.getDeclinedPermissions()
                .toArray(new String[accessToken.getDeclinedPermissions().size()]);
        token = accessToken.getToken();
        lastRefresh = accessToken.getLastRefresh().toString();
        applicationId = accessToken.getApplicationId();
        userId = accessToken.getUserId();
    }
}
