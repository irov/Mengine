package org.Mengine.Plugin.GoogleGameSocial;

import android.content.ActivityNotFoundException;
import android.content.Context;
import android.content.Intent;
import android.content.IntentSender;
import android.net.Uri;
import android.os.Bundle;

import androidx.annotation.NonNull;

import com.google.android.gms.auth.api.Auth;
import com.google.android.gms.auth.api.signin.GoogleSignIn;
import com.google.android.gms.auth.api.signin.GoogleSignInAccount;
import com.google.android.gms.auth.api.signin.GoogleSignInClient;
import com.google.android.gms.auth.api.signin.GoogleSignInOptions;
import com.google.android.gms.auth.api.signin.GoogleSignInResult;
import com.google.android.gms.common.api.CommonStatusCodes;
import com.google.android.gms.common.api.Scope;
import com.google.android.gms.common.api.Status;
import com.google.android.gms.games.PlayGames;
import com.google.android.gms.tasks.OnCanceledListener;
import com.google.android.gms.tasks.OnCompleteListener;
import com.google.android.gms.tasks.OnFailureListener;
import com.google.android.gms.tasks.OnSuccessListener;
import com.google.android.gms.tasks.Task;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginActivityListener;
import org.Mengine.Base.MenginePluginInvalidInitializeException;

import org.Mengine.Plugin.GoogleService.MengineGoogleServicePlugin;

public class MengineGoogleGameSocialPlugin extends MenginePlugin implements MenginePluginActivityListener {
    public static final String PLUGIN_NAME = "MengineGGameSocial";
    public static final boolean PLUGIN_EMBEDDING = true;

    private static final int RC_SIGN_IN = MengineActivity.genRequestCode("RC_SIGN_IN");
    private static final int RC_ACHIEVEMENT_UI = MengineActivity.genRequestCode("RC_ACHIEVEMENT_UI");

    @Override
    public boolean onAvailable(MengineApplication application) {
        if (MengineGoogleServicePlugin.isGooglePlayServicesAvailable(application) == false) {
            return false;
        }

        return true;
    }

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) throws MenginePluginInvalidInitializeException {
        this.signInSilently();
    }

    @Override
    public void onResume(MengineActivity activity) {
        // FIXME sign-in loop:
        //  resume - silence sign in - fail - intent sign in - resume - silence sign in ...
        //this.signInSilently();
    }

    @Override
    public void onDestroy(MengineActivity activity) {
    }

    public void startSignInIntent() {
        this.logMessage("startSignInIntent");

        MengineActivity activity = this.getMengineActivity();

        GoogleSignInClient signInClient = GoogleSignIn.getClient(activity, GoogleSignInOptions.DEFAULT_GAMES_SIGN_IN);

        Intent intent = signInClient.getSignInIntent();

        try {
            activity.startActivityForResult(intent, MengineGoogleGameSocialPlugin.RC_SIGN_IN);
        } catch (ActivityNotFoundException e) {
            this.logError("[ERROR] startSignInIntent catch ActivityNotFoundException: %s"
                , e.getMessage()
            );
        }
    }

    public void signOut() {
        this.logMessage("signOut");

        MengineActivity activity = this.getMengineActivity();

        GoogleSignInClient signInClient = GoogleSignIn.getClient(activity, GoogleSignInOptions.DEFAULT_GAMES_SIGN_IN);

        signInClient.signOut()
            .addOnCompleteListener(activity, new OnCompleteListener<Void>() {
                @Override
                public void onComplete(@NonNull Task<Void> task) {
                    MengineGoogleGameSocialPlugin.this.logMessage("google game social log OUT complete");

                    MengineGoogleGameSocialPlugin.this.pythonCall("onGoogleGameSocialSignOutComplete");
                }
            }).addOnFailureListener(activity, new OnFailureListener() {
                @Override
                public void onFailure(@NonNull Exception ex) {
                    MengineGoogleGameSocialPlugin.this.logMessage("google game social log OUT failure: %s"
                        , ex.getMessage()
                    );

                    MengineGoogleGameSocialPlugin.this.pythonCall("onGoogleGameSocialSignOutFailure");
                }
            }).addOnCanceledListener(activity, new OnCanceledListener() {
                @Override
                public void onCanceled() {
                    MengineGoogleGameSocialPlugin.this.logMessage("google game social log OUT canceled");

                    MengineGoogleGameSocialPlugin.this.pythonCall("onGoogleGameSocialSignOutCanceled");
                }
            }).addOnSuccessListener(activity, new OnSuccessListener<Void>() {
                @Override
                public void onSuccess(Void unused) {
                    MengineGoogleGameSocialPlugin.this.logMessage("google game social log OUT success");

                    MengineGoogleGameSocialPlugin.this.pythonCall("onGoogleGameSocialSignOutSuccess");
                }
            });
    }

    public void revokeAccess() {
        this.logMessage("revokeAccess");

        MengineActivity activity = this.getMengineActivity();

        GoogleSignInClient signInClient = GoogleSignIn.getClient(activity, GoogleSignInOptions.DEFAULT_GAMES_SIGN_IN);

        signInClient.revokeAccess()
            .addOnCompleteListener(activity, new OnCompleteListener<Void>() {
                @Override
                public void onComplete(@NonNull Task<Void> task) {
                    MengineGoogleGameSocialPlugin.this.logMessage("google game social revoke access success");

                    MengineGoogleGameSocialPlugin.this.pythonCall("onGoogleGameSocialRevokeAccessSuccess");
                }
            });
    }

    @Override
    public void onActivityResult(MengineActivity activity, int requestCode, int resultCode, Intent data) {
        if (requestCode == MengineGoogleGameSocialPlugin.RC_SIGN_IN) {
            switch (resultCode) {
                case MengineActivity.RESULT_CANCELED: {
                    this.logMessage("onActivityResult sign in canceled");

                    this.pythonCall("onGoogleGameSocialOnSignCanceled");

                    return;
                }
                case MengineActivity.RESULT_OK: {
                    this.logMessage("onActivityResult sign in ok");

                    GoogleSignInResult result = Auth.GoogleSignInApi.getSignInResultFromIntent(data);

                    if (result == null) {
                        this.logError("[ERROR] getSignInResultFromIntent result == null");

                        this.pythonCall("onGoogleGameSocialOnSignError");

                        return;
                    }

                    if (result.isSuccess() == false) {
                        Status status = result.getStatus();

                        this.logMessage("onActivityResult sign in failed code: %d message: %s"
                                , status.getStatusCode()
                                , status.getStatusMessage()
                        );

                        int statusCode = status.getStatusCode();

                        switch (statusCode) {
                            case CommonStatusCodes.SIGN_IN_REQUIRED: {
                                this.logError("[ERROR] google game social signIn SIGN_IN_REQUIRED [hasResolution %s]"
                                        , status.hasResolution()
                                );

                                if (status.hasResolution() == true) {
                                    try {
                                        status.startResolutionForResult(activity, MengineGoogleGameSocialPlugin.RC_SIGN_IN);
                                    } catch (IntentSender.SendIntentException ex) {
                                        this.logError("[ERROR] start resolution for result RC_SIGN_IN catch SendIntentException: %s"
                                                , ex.getMessage()
                                        );
                                    }
                                }
                            }
                            break;
                            default: {
                                this.logWarning("google game social signIn failed status error message: %s code: %d"
                                        , status.getStatusMessage()
                                        , status.getStatusCode()
                                );
                            }
                            break;
                        }

                        this.pythonCall("onGoogleGameSocialOnSignError");

                        return;
                    }

                    this.logMessage("onActivityResult resign");

                    GoogleSignInAccount account = result.getSignInAccount();

                    this.signInCallback(account);

                    return;
                }
                default: {
                    this.logError("[ERROR] onActivityResult sign in unknown resultCode: %d"
                        , resultCode
                    );

                    this.pythonCall("onGoogleGameSocialOnSignError");

                    return;
                }
            }
        } else if (requestCode == MengineGoogleGameSocialPlugin.RC_ACHIEVEMENT_UI) {
            //ToDo
        }
    }

    private void signInCallback(GoogleSignInAccount account) {
        if (account == null) {
            this.logWarning("signInCallback account == null");

            return;
        }

        this.logMessage("signInCallback");

        Uri accountPhotoUrl = account.getPhotoUrl();
        String accountDisplayName = account.getDisplayName();
        String accountEmail = account.getEmail();
        String accountFamilyName = account.getFamilyName();
        String accountGivenName = account.getGivenName();
        String accountId = account.getId();
        String accountIdToken = account.getIdToken();
        String accountServerAuthCode = account.getServerAuthCode();

        this.logInfo("get achievement account PhotoUrl: %s DisplayName: %s Email: %s FamilyName: %s GivenName: %s Id: %s IdToken: %s ServerAuthCode: %s"
            , accountPhotoUrl
            , accountDisplayName
            , accountEmail
            , accountFamilyName
            , accountGivenName
            , accountId
            , accountIdToken
            , accountServerAuthCode
        );

        this.pythonCall("onGoogleGameSocialOnSign", accountId);
    }

    public void signInSilently() {
        this.logMessage("signInSilently");

        MengineActivity activity = this.getMengineActivity();

        GoogleSignInOptions signInOptions = GoogleSignInOptions.DEFAULT_GAMES_SIGN_IN;
        GoogleSignInAccount account = GoogleSignIn.getLastSignedInAccount(activity);

        Scope[] scopes = signInOptions.getScopeArray();

        if (GoogleSignIn.hasPermissions(account, scopes) == true) {
            // Already signed in.
            // The signed in account is stored in the 'account' variable.
            this.signInCallback(account);

            return;
        }

        // Haven't been signed-in before. Try the silent sign-in first.
        GoogleSignInClient signInClient = GoogleSignIn.getClient(activity, signInOptions);

        signInClient.silentSignIn()
            .addOnCompleteListener(activity, new OnCompleteListener<GoogleSignInAccount>() {
                @Override
                public void onComplete(@NonNull Task<GoogleSignInAccount> task) {
                    if (task.isSuccessful() == true) {
                        MengineGoogleGameSocialPlugin.this.logMessage("successful silently login");

                        GoogleSignInAccount account = task.getResult();

                        MengineGoogleGameSocialPlugin.this.signInCallback(account);
                    } else {
                        MengineGoogleGameSocialPlugin.this.logWarning("failed silently login try intent");

                        // Player will need to sign-in explicitly using via UI.
                        // See [sign-in best practices](http://developers.google.com/games/services/checklist) for guidance on how and when to implement Interactive Sign-in,
                        // and [Performing Interactive Sign-in](http://developers.google.com/games/services/android/signin#performing_interactive_sign-in) for details on how to implement
                        // Interactive Sign-in.

                        MengineGoogleGameSocialPlugin.this.pythonCall("onGoogleGameSocialNeedIntentSign");
                    }
                }
            });
    }

    public boolean showAchievements() {
        this.logMessage("showAchievements");

        MengineActivity activity = this.getMengineActivity();

        PlayGames.getAchievementsClient(activity)
            .getAchievementsIntent()
            .addOnSuccessListener(new OnSuccessListener<Intent>() {
                @Override
                public void onSuccess(Intent intent) {
                    MengineGoogleGameSocialPlugin.this.logMessage("get achievements successful");

                    MengineGoogleGameSocialPlugin.this.pythonCall("onGoogleGameSocialShowAchievementSuccess");

                    MengineActivity activity = MengineGoogleGameSocialPlugin.this.getMengineActivity();

                    try {
                        activity.startActivityForResult(intent, MengineGoogleGameSocialPlugin.RC_ACHIEVEMENT_UI);
                    } catch (ActivityNotFoundException e) {
                        MengineGoogleGameSocialPlugin.this.logError("[ERROR] showAchievements catch ActivityNotFoundException: %s"
                            , e.getMessage()
                        );
                    }
                }
            })
            .addOnFailureListener(new OnFailureListener() {
                @Override
                public void onFailure(@NonNull Exception e) {
                    MengineGoogleGameSocialPlugin.this.logError("[ERROR] get achievements error: %s"
                        , e.getMessage()
                    );

                    MengineGoogleGameSocialPlugin.this.pythonCall("onGoogleGameSocialShowAchievementError");
                }
            });

        return true;
    }

    public boolean unlockAchievement(String achievementId) {
        MengineActivity activity = this.getMengineActivity();

        this.logMessage("unlockAchievement: %s"
            , achievementId
        );

        PlayGames.getAchievementsClient(activity).unlockImmediate(achievementId)
            .addOnSuccessListener(new OnSuccessListener<Void>() {
                @Override
                public void onSuccess(Void unused) {
                    MengineGoogleGameSocialPlugin.this.logMessage("unlockAchievement complete achievementId: %s"
                        , achievementId
                    );

                    MengineGoogleGameSocialPlugin.this.pythonCall("onGoogleGameSocialAchievementSuccess", achievementId);

                }
            }).addOnFailureListener(new OnFailureListener() {
                @Override
                public void onFailure(@NonNull Exception e) {
                    MengineGoogleGameSocialPlugin.this.logError("[ERROR] unlockAchievement achievementId: %s error: %s"
                        , achievementId
                        , e.getMessage()
                    );

                    MengineGoogleGameSocialPlugin.this.pythonCall("onGoogleGameSocialAchievementError", achievementId);
                }
            });

        return true;
    }

    public boolean incrementAchievement(String achievementId, int numSteps) {
        MengineActivity activity = this.getMengineActivity();

        this.logMessage("incrementAchievement achievementId: %s numSteps: %d"
            , achievementId
            , numSteps
        );

        PlayGames.getAchievementsClient(activity).incrementImmediate(achievementId, numSteps)
            .addOnSuccessListener(new OnSuccessListener<Boolean>() {
                @Override
                public void onSuccess(Boolean aBoolean) {
                    MengineGoogleGameSocialPlugin.this.logMessage("incrementAchievement complete achievementId: %s numSteps: %d"
                        , achievementId
                        , numSteps
                    );

                    MengineGoogleGameSocialPlugin.this.pythonCall("onGoogleGameSocialAchievementIncrementSuccess", achievementId);
                }
            }).addOnFailureListener(new OnFailureListener() {
                @Override
                public void onFailure(@NonNull Exception e) {
                    MengineGoogleGameSocialPlugin.this.logError("[ERROR] incrementAchievement achievementId: %s numSteps: %d error: %s"
                        , achievementId
                        , numSteps
                        , e.getMessage()
                    );

                    MengineGoogleGameSocialPlugin.this.pythonCall("onGoogleGameSocialAchievementIncrementError", achievementId);
                }
            });

        return true;
    }
}
