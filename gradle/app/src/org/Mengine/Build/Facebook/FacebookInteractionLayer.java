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
import java.util.List;

/**
 * Created by Alex Korzh on 6/25/18.
 */

public class FacebookInteractionLayer {

    private static final String TAG = "Facebook";

    private static final String EMAIL = "email";
    private static final String PROFILE = "public_profile";

    private CallbackManager _callbackManager;
    private AccessToken _accessToken;

    static native void AndroidNativeFacebook_onLoginSuccess(String loginResult);

    static native void AndroidNativeFacebook_onLoginCancel();

    static native void AndroidNativeFacebook_onLoginError(String exception);

    static native void AndroidNativeFacebook_onUserFetchSuccess(String object, String response);

    static native void AndroidNativeFacebook_onShareSuccess(String postId);

    static native void AndroidNativeFacebook_onShareCancel();

    static native void AndroidNativeFacebook_onShareError(String exception);

    public FacebookInteractionLayer(CallbackManager callbackManager) {
        _callbackManager = callbackManager;
        _accessToken = AccessToken.getCurrentAccessToken();
    }

    public boolean isLoggedIn() {
        return _accessToken != null && !_accessToken.isExpired();
    }

    public void performLogin(Activity activity, String[] readPermissions) {
        LoginManager.getInstance().registerCallback(_callbackManager, new FacebookCallback<LoginResult>() {
            @Override
            public void onSuccess(LoginResult loginResult) {
                _accessToken = loginResult.getAccessToken();
                AndroidNativeFacebook_onLoginSuccess(_accessToken.getToken());
            }

            @Override
            public void onCancel() {
                AndroidNativeFacebook_onLoginCancel();
            }

            @Override
            public void onError(FacebookException exception) {
                AndroidNativeFacebook_onLoginError(exception.getMessage());
            }
        });
        List<String> permissions;
        if (readPermissions == null) {
            permissions = Arrays.asList(PROFILE, EMAIL);
        } else {
            permissions = Arrays.asList(readPermissions);
        }
        LoginManager.getInstance().logInWithReadPermissions(activity, permissions);
    }

    public void getUser() {
        GraphRequest request = GraphRequest.newMeRequest
                (_accessToken, new GraphRequest.GraphJSONObjectCallback() {
                    @Override
                    public void onCompleted(JSONObject object, GraphResponse response) {
                        String objectString = object != null ? object.toString() : "";
                        String responseString = response != null ? response.toString() : "";
                        AndroidNativeFacebook_onUserFetchSuccess(objectString, responseString);
                    }
                });
        Bundle parameters = new Bundle();
        parameters.putString("fields", "id, name, email, gender");
        request.setParameters(parameters);
        request.executeAsync();
    }

    public void shareLink(final Activity activity, String link) {
        ShareDialog shareDialog = new ShareDialog(activity);
        shareDialog.registerCallback(_callbackManager, new FacebookCallback<Sharer.Result>() {
            @Override
            public void onSuccess(Sharer.Result result) {
                String post = result.getPostId() != null ? result.getPostId() : "";
                AndroidNativeFacebook_onShareSuccess(result.getPostId());
            }

            @Override
            public void onCancel() {
                AndroidNativeFacebook_onShareCancel();
            }

            @Override
            public void onError(FacebookException exception) {
                AndroidNativeFacebook_onShareError(exception.getMessage());
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