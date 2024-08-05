package org.Mengine.Plugin.GoogleGameSocial;

import android.content.ActivityNotFoundException;
import android.content.Intent;
import android.os.Bundle;


import com.google.android.gms.games.GamesSignInClient;
import com.google.android.gms.games.PlayGames;
import com.google.android.gms.games.PlayGamesSdk;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginActivityListener;
import org.Mengine.Base.MenginePluginInvalidInitializeException;

import org.Mengine.Plugin.GoogleService.MengineGoogleServicePlugin;

public class MengineGoogleGameSocialPlugin extends MenginePlugin implements MenginePluginActivityListener {
    public static final String PLUGIN_NAME = "MengineGGameSocial";
    public static final boolean PLUGIN_EMBEDDING = true;

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
        PlayGamesSdk.initialize(activity);

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
    }

    @Override
    public void onActivityResult(MengineActivity activity, int requestCode, int resultCode, Intent data) {
        if (requestCode == MengineGoogleGameSocialPlugin.RC_ACHIEVEMENT_UI) {
            //ToDo
        }
    }

    public boolean showAchievements() {
        this.logMessage("showAchievements");

        this.runOnUiThread(() -> {
            MengineActivity activity = this.getMengineActivity();

            PlayGames.getAchievementsClient(activity)
                .getAchievementsIntent()
                .addOnSuccessListener(intent -> {
                    MengineGoogleGameSocialPlugin.this.logMessageRelease("get achievements successful");

                    MengineGoogleGameSocialPlugin.this.pythonCall("onGoogleGameSocialShowAchievementSuccess");

                    MengineActivity activity1 = MengineGoogleGameSocialPlugin.this.getMengineActivity();

                    try {
                        activity1.startActivityForResult(intent, MengineGoogleGameSocialPlugin.RC_ACHIEVEMENT_UI);
                    } catch (ActivityNotFoundException e) {
                        MengineGoogleGameSocialPlugin.this.logError("[ERROR] showAchievements catch ActivityNotFoundException: %s"
                            , e.getMessage()
                        );
                    }
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

            PlayGames.getAchievementsClient(activity).unlockImmediate(achievementId)
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

            PlayGames.getAchievementsClient(activity).incrementImmediate(achievementId, numSteps)
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

            PlayGames.getAchievementsClient(activity).revealImmediate(achievementId)
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
