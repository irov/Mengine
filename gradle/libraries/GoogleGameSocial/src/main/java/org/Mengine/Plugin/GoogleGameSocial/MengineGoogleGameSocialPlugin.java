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
import com.google.android.gms.games.AchievementsClient;
import com.google.android.gms.games.Games;
import com.google.android.gms.games.achievement.Achievements;
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
     *  - onMengineGoogleGameSocialAchievementIncrementError(String achievementId)
     *  - onMengineGoogleGameSocialAchievementIncrementSuccess(String achievementId)
     *
     * Ачивка событияная - открываем сразу
     * boolean unlockAchievement(String achievementId)
     *  - onMengineGoogleGameSocialAchievementSuccess(String achievementId)
     *  - onMengineGoogleGameSocialAchievementError(String achievementId)
     *
     * Показать окно ачивок
     * boolean showAchievements()
     *  - onMengineGoogleGameSocialShowAchievementSuccess()
     *  - onMengineGoogleGameSocialShowAchievementError()
     */

    private int RC_SIGN_IN;
    private int RC_UNUSED;

    private @NonNull
    GoogleSignInClient m_signInClient;

    private final GoogleSignInOptions m_signInOptions = GoogleSignInOptions.DEFAULT_GAMES_SIGN_IN;

    @Override
    public void onPythonEmbedding(MengineActivity activity) {
        this.addPythonPlugin("GoogleGameSocial");
    }

    private AchievementsClient mAchievementsClient;

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) {
        super.onCreate(activity, savedInstanceState);

        RC_SIGN_IN = this.getActivity().genRequestCode("RC_SIGN_IN");
        RC_UNUSED = this.getActivity().genRequestCode("RC_UNUSED");

        m_signInClient = GoogleSignIn.getClient(
                activity,
                new GoogleSignInOptions.Builder(m_signInOptions)
//                .requestEmail()
                        .requestProfile()
                        .build()
        );
    }

    void startSignInIntent() {
        getActivity().startActivityForResult(m_signInClient.getSignInIntent(), RC_SIGN_IN);
    }

    void signOut() {
        m_signInClient.signOut().addOnCompleteListener(getActivity(), new OnCompleteListener<Void>() {
            @Override
            public void onComplete(@NonNull Task<Void> task) {
                MengineGoogleGameSocialPlugin.this.log("google game social log OUT");
                MengineGoogleGameSocialPlugin.this.pythonCall("onGoogleGameSocialSignOut");
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
                MengineGoogleGameSocialPlugin.this.log("Google game social error %s status %s"
                        , result.getStatus().getStatusMessage()
                        , result.getStatus()
                );
                MengineGoogleGameSocialPlugin.this.pythonCall("onGoogleGameSocialOnSignError");
            }
        }
    }

    private void signInCallback(@Nullable GoogleSignInAccount account) {
        if (account == null) {
            MengineGoogleGameSocialPlugin.this.log("GoogleSignInAccount == null");

            return;
        }

        mAchievementsClient = Games.getAchievementsClient(this.getActivity(), account);

        //аккаунт от гугла - профиль от гугла
        MengineGoogleGameSocialPlugin.this.log("player include '%s' ->id = '%s'"
                , account.getDisplayName()
                , account.getId()
        );

        this.pythonCall("onGoogleGameSocialOnSign", account.getId());
    }

    public void signInSilently() {
        GoogleSignInAccount account = GoogleSignIn.getLastSignedInAccount(getActivity());
        if (GoogleSignIn.hasPermissions(account, m_signInOptions.getScopeArray())) {
            // Already signed in.
            // The signed in account is stored in the 'account' variable.
            signInCallback(account);
        } else {
            // Haven't been signed-in before. Try the silent sign-in first.
            GoogleSignInClient signInClient = GoogleSignIn.getClient(getActivity(), m_signInOptions);
            signInClient.silentSignIn().addOnCompleteListener(getActivity(), new OnCompleteListener<GoogleSignInAccount>() {
                @Override
                public void onComplete(@NonNull Task<GoogleSignInAccount> googleSignInAccountTask) {
                    if (googleSignInAccountTask.isSuccessful()) {

                        signInCallback(googleSignInAccountTask.getResult());
                    } else {
                        Exception ex = googleSignInAccountTask.getException();

                        if (ex == null) {
                            ex = googleSignInAccountTask.getException();
                        }

                        if (ex != null) {
                            MengineGoogleGameSocialPlugin.this.log("not success login: %s"
                                    , ex.getLocalizedMessage()
                            );
                        } else {
                            MengineGoogleGameSocialPlugin.this.log("not success login");
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
        if (mAchievementsClient == null) {
            return false;
        }

        mAchievementsClient.getAchievementsIntent()
                .addOnSuccessListener(new OnSuccessListener<Intent>() {
                    @Override
                    public void onSuccess(Intent intent) {
                        MengineGoogleGameSocialPlugin.this.pythonCall("onMengineGoogleGameSocialShowAchievementSuccess");
                        MengineGoogleGameSocialPlugin.this.getActivity().startActivityForResult(intent, RC_UNUSED);
                    }
                })
                .addOnFailureListener(new OnFailureListener() {
                    @Override
                    public void onFailure(@NonNull Exception e) {
                        MengineGoogleGameSocialPlugin.this.pythonCall("onMengineGoogleGameSocialShowAchievementError");
                        MengineGoogleGameSocialPlugin.this.log("achievements error '%s'"
                                , e.getLocalizedMessage());
                    }
                });

        return true;
    }

    public boolean unlockAchievement(String achievementId) {
        if (mAchievementsClient == null) {
            return false;
        }

        mAchievementsClient.unlockImmediate(achievementId).addOnSuccessListener(new OnSuccessListener<Void>() {
            @Override
            public void onSuccess(Void unused) {
                MengineGoogleGameSocialPlugin.this.pythonCall("onMengineGoogleGameSocialAchievementSuccess", achievementId);
                MengineGoogleGameSocialPlugin.this.log("unlockAchievement '%s' complete", achievementId);
            }
        }).addOnFailureListener(new OnFailureListener() {
            @Override
            public void onFailure(@NonNull Exception e) {
                MengineGoogleGameSocialPlugin.this.pythonCall("onMengineGoogleGameSocialAchievementError", achievementId);
                MengineGoogleGameSocialPlugin.this.log("unlockAchievement '%s' error '%s'"
                        , achievementId, e.getLocalizedMessage());
            }
        });

        return true;
    }

    public boolean incrementAchievement(String achievementId, int numSteps) {
        if (mAchievementsClient == null) {
            return false;
        }

        mAchievementsClient.incrementImmediate(achievementId, numSteps).addOnSuccessListener(new OnSuccessListener<Boolean>() {
            @Override
            public void onSuccess(Boolean aBoolean) {
                MengineGoogleGameSocialPlugin.this.log("incrementImmediate '%s' complete", achievementId);
                MengineGoogleGameSocialPlugin.this.pythonCall("onMengineGoogleGameSocialAchievementIncrementSuccess", achievementId);
            }
        }).addOnFailureListener(new OnFailureListener() {
            @Override
            public void onFailure(@NonNull Exception e) {
                MengineGoogleGameSocialPlugin.this.pythonCall("onMengineGoogleGameSocialAchievementIncrementError", achievementId);
                MengineGoogleGameSocialPlugin.this.log("incrementImmediate '%s' error '%s'"
                        , achievementId, e.getLocalizedMessage());
            }
        });
        return true;
    }
}
