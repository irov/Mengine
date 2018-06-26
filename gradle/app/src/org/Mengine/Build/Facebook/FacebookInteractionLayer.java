package org.Mengine.Build.Facebook;

import android.app.Activity;
import android.net.Uri;
import android.os.Bundle;

import com.facebook.AccessToken;
import com.facebook.CallbackManager;
import com.facebook.FacebookCallback;
import com.facebook.FacebookException;
import com.facebook.GraphRequest;
import com.facebook.GraphResponse;
import com.facebook.login.LoginManager;
import com.facebook.login.LoginResult;
import com.facebook.share.Sharer;
import com.facebook.share.model.ShareLinkContent;
import com.facebook.share.widget.ShareDialog;

import org.json.JSONObject;

import java.util.Arrays;
import java.util.HashSet;
import java.util.List;

/**
 * Created by sweatcoin7 on 6/25/18.
 */

public class FacebookInteractionLayer {

    private static final String TAG = "Facebook";
    private static final String EMAIL = "email";
    private static final String PROFILE = "public_profile";

    private CallbackManager _callbackManager;
    private AccessToken _accessToken;

    public FacebookInteractionLayer(CallbackManager callbackManager) {
        _callbackManager = callbackManager;
    }

    public void performLogin(Activity activity, final FacebookLoginCallback facebookLoginCallback, String[] readPermissions) {
        LoginManager.getInstance().registerCallback(_callbackManager, new FacebookCallback<LoginResult>() {
            @Override
            public void onSuccess(LoginResult loginResult) {
                FacebookLoginResultWrapper wrappedResult = new FacebookLoginResultWrapper();
                wrappedResult.recentlyGrantedPermissions =
                        loginResult.getRecentlyGrantedPermissions()
                                .toArray(new String[loginResult.getRecentlyGrantedPermissions().size()]);
                wrappedResult.recentlyDeniedPermissions =
                        loginResult.getRecentlyDeniedPermissions()
                                .toArray(new String[loginResult.getRecentlyDeniedPermissions().size()]);
                wrappedResult.accessToken = new FacebookAccessTokenWrapper(loginResult.getAccessToken());

                facebookLoginCallback.onLoginSuccess(wrappedResult);
                _accessToken = loginResult.getAccessToken();
            }

            @Override
            public void onCancel() {
                facebookLoginCallback.onLoginCancel();
            }

            @Override
            public void onError(FacebookException exception) {
                facebookLoginCallback.onLoginError(exception);
            }
        });
        List<String> permissions;
        if(readPermissions == null) {
            permissions = Arrays.asList(PROFILE, EMAIL);
        } else {
            permissions = Arrays.asList(readPermissions);
        }
        LoginManager.getInstance().logInWithReadPermissions(activity, permissions);
    }

    public void getUser(final FacebookUserCallback facebookUserCallback) {
        GraphRequest request = GraphRequest.newMeRequest
                (_accessToken, new GraphRequest.GraphJSONObjectCallback() {
                    @Override
                    public void onCompleted(JSONObject object, GraphResponse response) {
                        facebookUserCallback.onUserFetchSuccess(object.toString(), response.getRawResponse());
                    }
                });
        Bundle parameters = new Bundle();
        parameters.putString("fields", "id, name, email, gender");
        request.setParameters(parameters);
        request.executeAsync();
    }

    public void getUser(FacebookAccessTokenWrapper accessTokenWrapper, final FacebookUserCallback facebookUserCallback) {
        AccessToken accessToken =
                new AccessToken(accessTokenWrapper.token,
                        accessTokenWrapper.applicationId,
                        accessTokenWrapper.userId,
                        new HashSet<>(Arrays.asList(accessTokenWrapper.permissions)),
                        new HashSet<>(Arrays.asList(accessTokenWrapper.declinedPermissions)),
                        null,
                        null,
                        null);

        GraphRequest request = GraphRequest.newMeRequest
                (accessToken, new GraphRequest.GraphJSONObjectCallback() {
                    @Override
                    public void onCompleted(JSONObject object, GraphResponse response) {
                        facebookUserCallback.onUserFetchSuccess(object.toString(), response.getRawResponse());
                    }
                });
        Bundle parameters = new Bundle();
        parameters.putString("fields", "id, name, email, gender");
        request.setParameters(parameters);
        request.executeAsync();
    }

    public void shareLink(Activity activity, String link, final FacebookShareCallback facebookShareCallback) {
        ShareDialog shareDialog = new ShareDialog(activity);
        shareDialog.registerCallback(_callbackManager, new FacebookCallback<Sharer.Result>() {
            @Override
            public void onSuccess(Sharer.Result result) {
                facebookShareCallback.onShareSuccess(result.getPostId());
            }

            @Override
            public void onCancel() {
                facebookShareCallback.onShareCancel();
            }

            @Override
            public void onError(FacebookException exception) {
                facebookShareCallback.onShareError(exception);
            }
        });
        if (ShareDialog.canShow(ShareLinkContent.class)) {
            ShareLinkContent linkContent = new ShareLinkContent.Builder()
                    .setContentUrl(Uri.parse(link))
                    .build();
            shareDialog.show(linkContent);
        }
    }
}