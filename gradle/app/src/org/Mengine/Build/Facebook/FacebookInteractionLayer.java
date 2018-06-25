package org.Mengine.Build.Facebook;

import android.app.Activity;
import android.support.annotation.NonNull;

import com.facebook.CallbackManager;
import com.facebook.FacebookCallback;
import com.facebook.FacebookException;
import com.facebook.login.LoginManager;
import com.facebook.login.LoginResult;

import java.util.Arrays;

/**
 * Created by sweatcoin7 on 6/25/18.
 */

public class FacebookInteractionLayer {

    private static final String TAG = "Facebook";
    private static final String EMAIL = "email";
    private static final String PROFILE = "public_profile";

    private CallbackManager _callbackManager;

    public FacebookInteractionLayer(CallbackManager callbackManager) {
        _callbackManager = callbackManager;
    }

    public void setupLogin(final FacebookLoginCallback facebookLoginCallback) {
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

                facebookLoginCallback.onLoginSuccess(wrappedResult);
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
    }

    public void performLogin(Activity activity) {
        LoginManager.getInstance().logInWithReadPermissions(activity, Arrays.asList(PROFILE, EMAIL));
    }
}