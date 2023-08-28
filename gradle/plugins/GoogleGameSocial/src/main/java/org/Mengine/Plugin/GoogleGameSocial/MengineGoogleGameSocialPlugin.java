package org.Mengine.Plugin.GoogleGameSocial;

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
import com.google.android.gms.games.AchievementsClient;
import com.google.android.gms.games.Games;
import com.google.android.gms.tasks.OnCanceledListener;
import com.google.android.gms.tasks.OnCompleteListener;
import com.google.android.gms.tasks.OnFailureListener;
import com.google.android.gms.tasks.OnSuccessListener;
import com.google.android.gms.tasks.Task;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginActivityListener;
import org.Mengine.Base.MenginePluginInvalidInitializeException;

public class MengineGoogleGameSocialPlugin extends MenginePlugin implements MenginePluginActivityListener {
    public static final String PLUGIN_NAME = "GoogleGameSocial";
    public static final boolean PLUGIN_EMBEDDING = true;

    //добавить каллбеки на ошибку авторизации
    //глянуть id по возврату в Activity

    /**
    * Метода для  библиотеки
    * <p>
    * Авторизация
    * void startSignInIntent()
    * - onGoogleGameSocialOnSign
    * - onGoogleGameSocialOnSignError
    * - onGoogleGameSocialOnSignFailed
    * <p>
    * Тихая авторизация - если возможно то без вопроса у пользователя произвести авторизацию
    * void signInSilently()
    * - onGoogleGameSocialOnSign
    * - onGoogleGameSocialOnSignError
    * <p>
    * Выход
    * void signOut()
    * - onGoogleGameSocialSignOutComplete
    * - onGoogleGameSocialSignOutFailure
    * - onGoogleGameSocialSignOutCanceled
    * - onGoogleGameSocialSignOutSuccess
    * <p>
    * Ачивка этапная - увеличиваем этап и по достижении последнего даётся ачивка
    * boolean incrementAchievement(String achievementId, int numSteps)
    * - onGoogleGameSocialAchievementIncrementSuccess(String achievementId)
    * - onGoogleGameSocialAchievementIncrementError(String achievementId)
    * <p>
    * Ачивка событияная - открываем сразу
    * boolean unlockAchievement(String achievementId)
    *  - onGoogleGameSocialAchievementSuccess(String achievementId)
    *  - onGoogleGameSocialAchievementError(String achievementId)
    * <p>
    * Показать окно ачивок
    * boolean showAchievements()
    *  - onGoogleGameSocialShowAchievementSuccess()
    *  - onGoogleGameSocialShowAchievementError()
    **/

    private int RC_SIGN_IN;
    private int RC_UNUSED;

    private GoogleSignInClient m_signInClient;
    private AchievementsClient m_achievementsClient;

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) throws MenginePluginInvalidInitializeException {
        RC_SIGN_IN = activity.genRequestCode("RC_SIGN_IN");
        RC_UNUSED = activity.genRequestCode("RC_UNUSED");

        GoogleSignInOptions signInOptions = new GoogleSignInOptions.Builder(GoogleSignInOptions.DEFAULT_GAMES_SIGN_IN)
            .build();

        m_signInClient = GoogleSignIn.getClient(activity, signInOptions);
    }

    @Override
    public void onResume(MengineActivity activity) {
        //this.signInSilently();
    }

    @Override
    public void onDestroy(MengineActivity activity) {
        if (m_signInClient == null) {
            m_signInClient.signOut();
            m_signInClient = null;
        }

        m_achievementsClient = null;
    }

    public void startSignInIntent() {
        if (m_signInClient == null) {
            this.logWarning("startSignInIntent google client not created");

            return;
        }

        this.logMessage("startSignInIntent");

        MengineActivity activity = this.getMengineActivity();

        Intent intent = m_signInClient.getSignInIntent();

        activity.startActivityForResult(intent, RC_SIGN_IN);
    }

    public void signOut() {
        if (m_signInClient == null) {
            this.logWarning("signOut google client not created");

            return;
        }

        this.logMessage("signOut");

        MengineActivity activity = this.getMengineActivity();

        m_signInClient.signOut()
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
                        , ex.getLocalizedMessage()
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
        if (m_signInClient == null) {
            this.logWarning("revokeAccess google client not created");

            return;
        }

        this.logMessage("revokeAccess");

        MengineActivity activity = this.getMengineActivity();

        m_signInClient.revokeAccess()
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
        if (requestCode != RC_SIGN_IN) {
            return;
        }

        GoogleSignInResult result = Auth.GoogleSignInApi.getSignInResultFromIntent(data);

        if (result == null) {
            return;
        }

        if (result.isSuccess() == true) {
            this.logMessage("onActivityResult resign");

            GoogleSignInAccount account = result.getSignInAccount();

            this.signInCallback(account);
        } else {
            this.logMessage("onActivityResult login");

            Status status = result.getStatus();

            int statusCode = status.getStatusCode();

            switch (statusCode) {
                case CommonStatusCodes.SIGN_IN_REQUIRED: {
                    this.logError("google game social signIn required" );

                    if (status.hasResolution() == true) {
                        try {
                            status.startResolutionForResult(activity, RC_SIGN_IN);
                        } catch (IntentSender.SendIntentException ex) {
                            this.logError("start resolution for result RC_SIGN_IN catch SendIntentException: %s"
                                , ex.getLocalizedMessage()
                            );
                        }
                    } else {
                        this.logError("google game social signIn required failed has resolution" );

                        this.pythonCall("onGoogleGameSocialOnSignError");
                    }
                }break;
                default: {
                    this.logWarning("google game social signIn failed status error message: %s code: %d"
                        , status.getStatusMessage()
                        , status.getStatusCode()
                    );

                    this.pythonCall("onGoogleGameSocialOnSignError");
                }break;
            }
        }
    }

    private void signInCallback(GoogleSignInAccount account) {
        if (account == null) {
            this.logWarning("signInCallback account == null");

            return;
        }

        this.logMessage("signInCallback");

        MengineActivity activity = this.getMengineActivity();

        m_achievementsClient = Games.getAchievementsClient(activity, account);

        Uri accountPhotoUrl = account.getPhotoUrl();
        String accountDisplayName = account.getDisplayName();
        String accountEmail = account.getEmail();
        String accountFamilyName = account.getFamilyName();
        String accountGivenName = account.getGivenName();
        String accountId = account.getId();
        String accountIdToken = account.getIdToken();
        String accountServerAuthCode = account.getServerAuthCode();

        this.logInfo("PhotoUrl: %s", accountPhotoUrl);
        this.logInfo("DisplayName: %s", accountDisplayName);
        this.logInfo("Email: %s", accountEmail);
        this.logInfo("FamilyName: %s", accountFamilyName);
        this.logInfo("GivenName: %s", accountGivenName);
        this.logInfo("Id: %s", accountId);
        this.logInfo("IdToken: %s", accountIdToken);
        this.logInfo("ServerAuthCode: %s", accountServerAuthCode);

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

                        //MengineGoogleGameSocialPlugin.this.startSignInIntent();

                        MengineGoogleGameSocialPlugin.this.pythonCall("onGoogleGameSocialOnSignError");

                        //[ToDo]
                        //MengineGoogleGameSocialPlugin.this.pythonCall("onGoogleGameSocialNeedIntentSign");
                    }
                }
            });
    }

    public boolean showAchievements() {
        if (m_achievementsClient == null) {
            this.logWarning("showAchievements achievements client not created");

            return false;
        }

        this.logMessage("showAchievements");

        m_achievementsClient.getAchievementsIntent()
            .addOnSuccessListener(new OnSuccessListener<Intent>() {
                @Override
                public void onSuccess(Intent intent) {
                    MengineGoogleGameSocialPlugin.this.logMessage("get achievements successful");

                    MengineGoogleGameSocialPlugin.this.pythonCall("onGoogleGameSocialShowAchievementSuccess");

                    MengineActivity activity = MengineGoogleGameSocialPlugin.this.getMengineActivity();

                    activity.startActivityForResult(intent, RC_UNUSED);
                }
            })
            .addOnFailureListener(new OnFailureListener() {
                @Override
                public void onFailure(@NonNull Exception e) {
                    MengineGoogleGameSocialPlugin.this.logError("get achievements error: %s"
                        , e.getLocalizedMessage()
                    );

                    MengineGoogleGameSocialPlugin.this.pythonCall("onGoogleGameSocialShowAchievementError");
                }
            });

        return true;
    }

    public boolean unlockAchievement(String achievementId) {
        if (m_achievementsClient == null) {
            this.logWarning("unlockAchievement achievements client not created");

            return false;
        }

        this.logMessage("unlockAchievement: %s"
            , achievementId
        );

        m_achievementsClient.unlockImmediate(achievementId)
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
                    MengineGoogleGameSocialPlugin.this.logError("unlockAchievement achievementId: %s error: %s"
                        , achievementId
                        , e.getLocalizedMessage()
                    );

                    MengineGoogleGameSocialPlugin.this.pythonCall("onGoogleGameSocialAchievementError", achievementId);
                }
            });

        return true;
    }

    public boolean incrementAchievement(String achievementId, int numSteps) {
        if (m_achievementsClient == null) {
            this.logWarning("incrementAchievement achievements client not created");

            return false;
        }

        this.logMessage("incrementAchievement achievementId: %s numSteps: %d"
            , achievementId
            , numSteps
        );

        m_achievementsClient.incrementImmediate(achievementId, numSteps)
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
                    MengineGoogleGameSocialPlugin.this.logError("incrementAchievement achievementId: %s numSteps: %d error: %s"
                        , achievementId
                        , numSteps
                        , e.getLocalizedMessage()
                    );

                    MengineGoogleGameSocialPlugin.this.pythonCall("onGoogleGameSocialAchievementIncrementError", achievementId);
                }
            });

        return true;
    }
}
