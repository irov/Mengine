package org.Mengine.Plugin.GoogleGameSocial;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;


import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts;

import com.google.android.gms.games.AchievementsClient;
import com.google.android.gms.games.GamesSignInClient;
import com.google.android.gms.games.PlayGames;
import com.google.android.gms.games.PlayGamesSdk;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginActivityListener;
import org.Mengine.Base.MenginePluginApplicationListener;
import org.Mengine.Base.MenginePluginInvalidInitializeException;

import org.Mengine.Plugin.GoogleService.MengineGoogleServicePlugin;

public class MengineGoogleGameSocialPlugin extends MenginePlugin implements MenginePluginApplicationListener, MenginePluginActivityListener {
    public static final String PLUGIN_NAME = "MengineGGameSocial";
    public static final boolean PLUGIN_EMBEDDING = true;

    private ActivityResultLauncher<Intent> m_achievementLauncher;

    @Override
    public boolean onAvailable(MengineApplication application) {
        if (MengineGoogleServicePlugin.isGooglePlayServicesAvailable(application) == false) {
            return false;
        }

        return true;
    }

    @Override
    public void onAppCreate(MengineApplication application) throws MenginePluginInvalidInitializeException {
        PlayGamesSdk.initialize(application);
    }

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) throws MenginePluginInvalidInitializeException {
        GamesSignInClient gamesSignInClient = PlayGames.getGamesSignInClient(activity);

        gamesSignInClient.isAuthenticated().addOnCompleteListener(isAuthenticatedTask -> {
            if (isAuthenticatedTask.isSuccessful() && isAuthenticatedTask.getResult().isAuthenticated()) {
                this.logMessageRelease("google game social isAuthenticated successful");

                this.pythonCall("onGoogleGameSocialOnAuthenticatedSuccess", "authenticated");
            } else {
                this.logMessageRelease("google game social isAuthenticated failed");

                this.pythonCall("onGoogleGameSocialOnAuthenticatedError");
            }
        });

        ActivityResultLauncher<Intent> achievementLauncher = activity.registerForActivityResult(new ActivityResultContracts.StartActivityForResult()
            ,   result -> {
                if (result.getResultCode() == Activity.RESULT_OK) {
                    Intent data = result.getData();

                    MengineGoogleGameSocialPlugin.this.logMessage("achievementLauncher onActivityResult intent: %s"
                        , data
                    );

                    //ToDo
                }
            }
        );

        m_achievementLauncher = achievementLauncher;
    }

    @Override
    public void onDestroy(MengineActivity activity) {
        if (m_achievementLauncher != null) {
            m_achievementLauncher.unregister();

            m_achievementLauncher = null;
        }
    }

    public boolean showAchievements() {
        this.logMessage("showAchievements");

        this.runOnUiThread(() -> {
            MengineActivity activity = this.getMengineActivity();

            AchievementsClient achievementsClient = PlayGames.getAchievementsClient(activity);

            achievementsClient.getAchievementsIntent()
                .addOnSuccessListener(intent -> {
                    MengineGoogleGameSocialPlugin.this.logMessageRelease("get achievements successful");

                    MengineGoogleGameSocialPlugin.this.pythonCall("onGoogleGameSocialShowAchievementSuccess");

                    m_achievementLauncher.launch(intent);
                })
                .addOnFailureListener(e -> {
                    MengineGoogleGameSocialPlugin.this.logError("[ERROR] get achievements error: %s"
                        , e.getMessage()
                    );

                    MengineGoogleGameSocialPlugin.this.pythonCall("onGoogleGameSocialShowAchievementError");
                });
        });

        return true;
    }

    public boolean unlockAchievement(String achievementId) {
        this.logMessage("unlockAchievement: %s"
            , achievementId
        );

        this.runOnUiThread(() -> {
            MengineActivity activity = this.getMengineActivity();

            AchievementsClient achievementsClient = PlayGames.getAchievementsClient(activity);

            achievementsClient.unlockImmediate(achievementId)
                .addOnSuccessListener(unused -> {
                    MengineGoogleGameSocialPlugin.this.logMessage("unlockAchievement complete achievementId: %s"
                        , achievementId
                    );

                    MengineGoogleGameSocialPlugin.this.pythonCall("onGoogleGameSocialUnlockAchievementSuccess", achievementId);

                }).addOnFailureListener(e -> {
                    MengineGoogleGameSocialPlugin.this.logError("[ERROR] unlockAchievement achievementId: %s error: %s"
                        , achievementId
                        , e.getMessage()
                    );

                    MengineGoogleGameSocialPlugin.this.pythonCall("onGoogleGameSocialUnlockAchievementError", achievementId);
                });
        });

        return true;
    }

    public boolean incrementAchievement(String achievementId, int numSteps) {
        this.logMessage("incrementAchievement achievementId: %s numSteps: %d"
            , achievementId
            , numSteps
        );

        this.runOnUiThread(() -> {
            MengineActivity activity = this.getMengineActivity();

            AchievementsClient achievementsClient = PlayGames.getAchievementsClient(activity);

            achievementsClient.incrementImmediate(achievementId, numSteps)
                .addOnSuccessListener(aBoolean -> {
                    MengineGoogleGameSocialPlugin.this.logMessage("incrementAchievement complete achievementId: %s numSteps: %d"
                        , achievementId
                        , numSteps
                    );

                    MengineGoogleGameSocialPlugin.this.pythonCall("onGoogleGameSocialIncrementAchievementSuccess", achievementId);
                }).addOnFailureListener(e -> {
                    MengineGoogleGameSocialPlugin.this.logError("[ERROR] incrementAchievement achievementId: %s numSteps: %d error: %s"
                        , achievementId
                        , numSteps
                        , e.getMessage()
                    );

                    MengineGoogleGameSocialPlugin.this.pythonCall("onGoogleGameSocialIncrementAchievementError", achievementId);
                });
        });

        return true;
    }

    public boolean revealAchievement(String achievementId) {
        this.logMessage("revealAchievement achievementId: %s "
            , achievementId
        );

        this.runOnUiThread(() -> {
            MengineActivity activity = this.getMengineActivity();

            AchievementsClient achievementsClient = PlayGames.getAchievementsClient(activity);

            achievementsClient.revealImmediate(achievementId)
                .addOnSuccessListener(unused -> {
                    MengineGoogleGameSocialPlugin.this.logMessage("revealAchievement complete achievementId: %s"
                        , achievementId
                    );

                    MengineGoogleGameSocialPlugin.this.pythonCall("onGoogleGameSocialAchievementRevealSuccess", achievementId);
                }).addOnFailureListener(e -> {
                    MengineGoogleGameSocialPlugin.this.logError("[ERROR] revealAchievement achievementId: %s error: %s"
                        , achievementId
                        , e.getMessage()
                    );

                    MengineGoogleGameSocialPlugin.this.pythonCall("onGoogleGameSocialAchievementRevealError", achievementId);
                });
        });

        return true;
    }
}
