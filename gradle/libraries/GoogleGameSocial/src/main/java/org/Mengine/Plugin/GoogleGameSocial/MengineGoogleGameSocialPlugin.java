package org.Mengine.Plugin.GoogleGameSocial;

import android.content.Intent;
import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.google.android.gms.auth.api.Auth;
import com.google.android.gms.auth.api.signin.GoogleSignIn;
import com.google.android.gms.auth.api.signin.GoogleSignInAccount;
import com.google.android.gms.auth.api.signin.GoogleSignInClient;
import com.google.android.gms.auth.api.signin.GoogleSignInOptions;
import com.google.android.gms.auth.api.signin.GoogleSignInResult;
import com.google.android.gms.common.api.Scope;
import com.google.android.gms.games.AchievementsClient;
import com.google.android.gms.games.Games;
import com.google.android.gms.tasks.OnCompleteListener;
import com.google.android.gms.tasks.OnFailureListener;
import com.google.android.gms.tasks.OnSuccessListener;
import com.google.android.gms.tasks.Task;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MenginePlugin;

public class MengineGoogleGameSocialPlugin extends MenginePlugin {

    //добавить каллбеки на ошибку авторизации
    //глянуть id по возврату в Activity

    /**
     * Метода для  библиотеки
     * <p>
     * Авторизация
     * void startSignInIntent()
     * - onGoogleGameSocialOnSign
     * - onGoogleGameSocialOnSignError
     * <p>
     * Тихая авторизация - если возможно то без вопроса у пользователя произвести авторизацию
     * void signInSilently()
     * - onGoogleGameSocialOnSign
     * - onGoogleGameSocialOnSignError
     * <p>
     * Выход
     * void signOut()
     * - onGoogleGameSocialOnSignOut
     *
     * Ачивка этапная - увеличиваем этап и по достижении последнего даётся ачивка
     * boolean incrementAchievement(String achievementId, int numSteps)
     *  - onGoogleGameSocialAchievementIncrementError(String achievementId)
     *  - onGoogleGameSocialAchievementIncrementSuccess(String achievementId)
     *
     * Ачивка событияная - открываем сразу
     * boolean unlockAchievement(String achievementId)
     *  - onGoogleGameSocialAchievementSuccess(String achievementId)
     *  - onGoogleGameSocialAchievementError(String achievementId)
     *
     * Показать окно ачивок
     * boolean showAchievements()
     *  - onGoogleGameSocialShowAchievementSuccess()
     *  - onGoogleGameSocialShowAchievementError()
     */

    private int RC_SIGN_IN;
    private int RC_UNUSED;

    private @NonNull GoogleSignInClient m_signInClient;
    private final GoogleSignInOptions m_signInOptions = GoogleSignInOptions.DEFAULT_GAMES_SIGN_IN;
    private AchievementsClient m_achievementsClient;

    @Override
    public void onPythonEmbedding(MengineActivity activity) {
        this.addPythonPlugin("GoogleGameSocial");
    }

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) {
        RC_SIGN_IN = activity.genRequestCode("RC_SIGN_IN");
        RC_UNUSED = activity.genRequestCode("RC_UNUSED");

        m_signInClient = GoogleSignIn.getClient(
                activity,
                new GoogleSignInOptions.Builder(m_signInOptions)
                        .requestProfile()
                        .build()
        );
    }

    void startSignInIntent() {
        MengineActivity activity = this.getActivity();
        Intent intent = m_signInClient.getSignInIntent();

        activity.startActivityForResult(intent, RC_SIGN_IN);
    }

    void signOut() {
        m_signInClient.signOut().addOnCompleteListener(getActivity(), new OnCompleteListener<Void>() {
            @Override
            public void onComplete(@NonNull Task<Void> task) {
                MengineGoogleGameSocialPlugin.this.logInfo("google game social log OUT");

                MengineGoogleGameSocialPlugin.this.pythonCall("onGoogleGameSocialSignOut");
            }
        });
    }

    @Override
    public void onActivityResult(MengineActivity activity, int requestCode, int resultCode, Intent data) {
        if (requestCode == RC_SIGN_IN) {
            GoogleSignInResult result = Auth.GoogleSignInApi.getSignInResultFromIntent(data);

            if (result != null && result.isSuccess() == true) {
                GoogleSignInAccount account = result.getSignInAccount();

                this.signInCallback(account);
            } else {
                MengineGoogleGameSocialPlugin.this.logError("Google game social error %s status %s"
                        , result.getStatus().getStatusMessage()
                        , result.getStatus()
                );

                MengineGoogleGameSocialPlugin.this.pythonCall("onGoogleGameSocialOnSignError");
            }
        }
    }

    private void signInCallback(@Nullable GoogleSignInAccount account) {
        if (account == null) {
            MengineGoogleGameSocialPlugin.this.logError("GoogleSignInAccount == null");

            return;
        }

        MengineActivity activity = this.getActivity();

        m_achievementsClient = Games.getAchievementsClient(activity, account);

        //аккаунт от гугла - профиль от гугла
        MengineGoogleGameSocialPlugin.this.logInfo("player include '%s' ->id = '%s'"
                , account.getDisplayName()
                , account.getId()
        );

        this.pythonCall("onGoogleGameSocialOnSign", account.getId());
    }

    public void signInSilently() {
        MengineActivity activity = this.getActivity();

        GoogleSignInAccount account = GoogleSignIn.getLastSignedInAccount(getActivity());

        Scope[] scopes = m_signInOptions.getScopeArray();

        if (GoogleSignIn.hasPermissions(account, scopes)) {
            // Already signed in.
            // The signed in account is stored in the 'account' variable.
            this.signInCallback(account);
        } else {
            // Haven't been signed-in before. Try the silent sign-in first.
            GoogleSignInClient signInClient = GoogleSignIn.getClient(activity, m_signInOptions);

            signInClient.silentSignIn().addOnCompleteListener(activity, new OnCompleteListener<GoogleSignInAccount>() {
                @Override
                public void onComplete(@NonNull Task<GoogleSignInAccount> googleSignInAccountTask) {
                    if (googleSignInAccountTask.isSuccessful() == true) {
                        GoogleSignInAccount account = googleSignInAccountTask.getResult();

                        MengineGoogleGameSocialPlugin.this.signInCallback(account);
                    } else {
                        Exception ex = googleSignInAccountTask.getException();

                        if (ex == null) {
                            ex = googleSignInAccountTask.getException();
                        }

                        if (ex != null) {
                            MengineGoogleGameSocialPlugin.this.logError("not success login: %s"
                                , ex.getLocalizedMessage()
                            );
                        } else {
                            MengineGoogleGameSocialPlugin.this.logInfo("not success login");
                        }

                        MengineGoogleGameSocialPlugin.this.pythonCall("onGoogleGameSocialOnSignError");
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

    public boolean showAchievements() {
        if (m_achievementsClient == null) {
            return false;
        }

        m_achievementsClient.getAchievementsIntent()
            .addOnSuccessListener(new OnSuccessListener<Intent>() {
                @Override
                public void onSuccess(Intent intent) {
                    MengineGoogleGameSocialPlugin.this.pythonCall("onGoogleGameSocialShowAchievementSuccess");

                    MengineGoogleGameSocialPlugin.this.getActivity().startActivityForResult(intent, RC_UNUSED);
                }
            })
            .addOnFailureListener(new OnFailureListener() {
                @Override
                public void onFailure(@NonNull Exception e) {
                    MengineGoogleGameSocialPlugin.this.logError("achievements error '%s'"
                            , e.getLocalizedMessage()
                    );

                    MengineGoogleGameSocialPlugin.this.pythonCall("onGoogleGameSocialShowAchievementError");
                }
            });

        return true;
    }

    public boolean unlockAchievement(String achievementId) {
        if (m_achievementsClient == null) {
            return false;
        }

        m_achievementsClient.unlockImmediate(achievementId).addOnSuccessListener(new OnSuccessListener<Void>() {
            @Override
            public void onSuccess(Void unused) {
                MengineGoogleGameSocialPlugin.this.logInfo("unlockAchievement '%s' complete"
                    , achievementId
                );

                MengineGoogleGameSocialPlugin.this.pythonCall("onGoogleGameSocialAchievementSuccess", achievementId);

            }
        }).addOnFailureListener(new OnFailureListener() {
            @Override
            public void onFailure(@NonNull Exception e) {
                MengineGoogleGameSocialPlugin.this.logError("unlockAchievement '%s' error '%s'"
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
            return false;
        }

        m_achievementsClient.incrementImmediate(achievementId, numSteps).addOnSuccessListener(new OnSuccessListener<Boolean>() {
            @Override
            public void onSuccess(Boolean aBoolean) {
                MengineGoogleGameSocialPlugin.this.logInfo("incrementImmediate '%s' complete"
                    , achievementId
                );

                MengineGoogleGameSocialPlugin.this.pythonCall("onGoogleGameSocialAchievementIncrementSuccess", achievementId);
            }
        }).addOnFailureListener(new OnFailureListener() {
            @Override
            public void onFailure(@NonNull Exception e) {
                MengineGoogleGameSocialPlugin.this.logError("incrementImmediate '%s' error '%s'"
                    , achievementId
                    , e.getLocalizedMessage()
                );

                MengineGoogleGameSocialPlugin.this.pythonCall("onGoogleGameSocialAchievementIncrementError", achievementId);
            }
        });

        return true;
    }
}
