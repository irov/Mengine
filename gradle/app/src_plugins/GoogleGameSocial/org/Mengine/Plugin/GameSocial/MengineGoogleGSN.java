package org.Mengine.Plugin.GameSocial;

import org.Mengine.Build.MengineApplication;
import org.Mengine.Build.MenginePlugin;
import org.Mengine.Build.MengineActivity;

import com.facebook.AccessToken;
import com.facebook.CallbackManager;
import com.facebook.FacebookCallback;
import com.facebook.FacebookException;
import com.facebook.GraphRequest;
import com.facebook.GraphResponse;
import com.facebook.HttpMethod;
import com.facebook.login.LoginManager;
import com.facebook.login.LoginResult;
import com.facebook.share.Sharer;
import com.facebook.share.model.ShareLinkContent;
import com.facebook.share.widget.ShareDialog;
import com.facebook.appevents.AppEventsLogger;
import com.google.android.datatransport.runtime.logging.Logging;
import com.google.android.gms.auth.api.Auth;
import com.google.android.gms.auth.api.signin.GoogleSignIn;
import com.google.android.gms.auth.api.signin.GoogleSignInAccount;
import com.google.android.gms.auth.api.signin.GoogleSignInClient;
import com.google.android.gms.auth.api.signin.GoogleSignInOptions;
import com.google.android.gms.auth.api.signin.GoogleSignInResult;
import com.google.android.gms.tasks.OnCompleteListener;
import com.google.android.gms.tasks.Task;

import android.app.Activity;
import android.app.Application;
import android.net.Uri;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import org.json.JSONException;
import org.json.JSONObject;

import java.util.Arrays;
import java.util.List;

public class MengineGoogleGSN extends MenginePlugin {

    private void Log(String msg) {
        Logging.i("GoogleGSN", msg);
    }

    private final int RC_SIGN_IN = 9000;
    private final int RC_LEADERBOARD_UI = 9004;


    private @NonNull
    GoogleSignInClient _signInClient;

    private final GoogleSignInOptions _signInOptions = GoogleSignInOptions.DEFAULT_GAMES_SIGN_IN;


    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) {
        super.onCreate(activity, savedInstanceState);
        _signInClient = GoogleSignIn.getClient(
                activity,
                new GoogleSignInOptions.Builder(_signInOptions)
//                .requestEmail()
//                .requestProfile()
                        .build()
        );
    }

    void startSignInIntent(Activity activity) {
        activity.startActivityForResult(_signInClient.getSignInIntent(), RC_SIGN_IN);
    }

    void signOut(Activity activity) {
        _signInClient.signOut().addOnCompleteListener(activity, new OnCompleteListener<Void>() {
            @Override
            public void onComplete(@NonNull Task<Void> task) {
                Log("google game social log OUT");
            }
        });
    }

    @Override
    public void onActivityResult(MengineActivity activity, int requestCode, int resultCode, Intent data) {
        super.onActivityResult(activity, requestCode, resultCode, data);
        if (requestCode == RC_SIGN_IN) {
            GoogleSignInResult result = Auth.GoogleSignInApi.getSignInResultFromIntent(data);
            if (result != null && result.isSuccess()) {
                signInCallback(result.getSignInAccount());
            } else {
                Log("Google game social error " + result.getStatus().getStatusMessage() + " " + result.getStatus());
            }
        } else if (requestCode == RC_LEADERBOARD_UI) {
            Log("RC_LEADERBOARD_UI");
        }
    }

    private void signInCallback(@Nullable GoogleSignInAccount account) {
        if (account == null) {
            Log("GoogleSignInAccount == null");
            return;
        }
        //аккаунт от гугла - профиль от гугла
        Log("player include '" + account.getDisplayName() + "' ->id= " + account.getId());
    }

    public void signInSilently(Activity activity) {
        GoogleSignInAccount account = GoogleSignIn.getLastSignedInAccount(activity);
        if (GoogleSignIn.hasPermissions(account, _signInOptions.getScopeArray())) {
            // Already signed in.
            // The signed in account is stored in the 'account' variable.
            signInCallback(account);
        } else {
            // Haven't been signed-in before. Try the silent sign-in first.
            GoogleSignInClient signInClient = GoogleSignIn.getClient(activity, _signInOptions);
            signInClient.silentSignIn().addOnCompleteListener(activity, new OnCompleteListener<GoogleSignInAccount>() {
                @Override
                public void onComplete(@NonNull Task<GoogleSignInAccount> googleSignInAccountTask) {
                    if (googleSignInAccountTask.isSuccessful()) {

                        signInCallback(googleSignInAccountTask.getResult());
                    } else {
                        Exception ex = googleSignInAccountTask.getException();
                        if (ex == null) ex = googleSignInAccountTask.getException();
                        if (ex != null) {
                            Log(ex.getLocalizedMessage());
                        } else {
                            Log("not success login");
                        }
                        // Player will need to sign-in explicitly using via UI.
                        // See [sign-in best practices](http://developers.google.com/games/services/checklist) for guidance on how and when to implement Interactive Sign-in,
                        // and [Performing Interactive Sign-in](http://developers.google.com/games/services/android/signin#performing_interactive_sign-in) for details on how to implement
                        // Interactive Sign-in.

                        /*App.scopeUI.launch {
                            startSignInIntent()
                        }*/

                    }
                }

            });
        }
    }
}
