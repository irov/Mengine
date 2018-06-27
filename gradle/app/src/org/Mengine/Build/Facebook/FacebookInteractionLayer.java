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

    native void onLoginSuccess(String loginResult);
    native void onLoginCancel();
    native void onLoginError(String exception);

    native void onUserFetchSuccess(String object, String response);

    native void onShareSuccess(String postId);
    native void onShareCancel();
    native void onShareError(String exception);

    public FacebookInteractionLayer(CallbackManager callbackManager) {
        _callbackManager = callbackManager;
    }

    public void performLogin(Activity activity, String[] readPermissions) {
        LoginManager.getInstance().registerCallback(_callbackManager, new FacebookCallback<LoginResult>() {
            @Override
            public void onSuccess(LoginResult loginResult) {
                _accessToken = loginResult.getAccessToken();
                onLoginSuccess(_accessToken.getToken());
            }

            @Override
            public void onCancel() {
                onLoginCancel();
            }

            @Override
            public void onError(FacebookException exception) {
                onLoginError(exception.getMessage());
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

    public void getUser() {
        GraphRequest request = GraphRequest.newMeRequest
                (_accessToken, new GraphRequest.GraphJSONObjectCallback() {
                    @Override
                    public void onCompleted(JSONObject object, GraphResponse response) {
                        onUserFetchSuccess(object.toString(), response.getRawResponse());
                    }
                });
        Bundle parameters = new Bundle();
        parameters.putString("fields", "id, name, email, gender");
        request.setParameters(parameters);
        request.executeAsync();
    }

    public void shareLink(Activity activity, String link) {
        ShareDialog shareDialog = new ShareDialog(activity);
        shareDialog.registerCallback(_callbackManager, new FacebookCallback<Sharer.Result>() {
            @Override
            public void onSuccess(Sharer.Result result) {
                onShareSuccess(result.getPostId());
            }

            @Override
            public void onCancel() {
                onShareCancel();
            }

            @Override
            public void onError(FacebookException exception) {
                onShareError(exception.getMessage());
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