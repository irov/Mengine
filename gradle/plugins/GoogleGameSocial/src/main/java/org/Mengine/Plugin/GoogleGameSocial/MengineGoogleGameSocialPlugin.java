package org.Mengine.Plugin.GoogleGameSocial;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;

import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts;
import androidx.annotation.NonNull;

import com.google.android.gms.games.AchievementsClient;
import com.google.android.gms.games.AuthenticationResult;
import com.google.android.gms.games.GamesSignInClient;
import com.google.android.gms.games.PlayGames;
import com.google.android.gms.games.PlayGamesSdk;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineService;
import org.Mengine.Base.MengineListenerActivity;
import org.Mengine.Base.MengineListenerApplication;
import org.Mengine.Base.MengineServiceInvalidInitializeException;

import org.Mengine.Plugin.GoogleService.MengineGoogleServicePlugin;

public class MengineGoogleGameSocialPlugin extends MengineService implements MengineListenerApplication, MengineListenerActivity {
    public static final String SERVICE_NAME = "MengineGGameSocial";
    public static final boolean SERVICE_EMBEDDING = true;

    private ActivityResultLauncher<Intent> m_achievementLauncher;
    private ActivityResultLauncher<Intent> m_leaderboardLauncher;

    @Override
    public boolean onAvailable(@NonNull MengineApplication application) {
        if (MengineGoogleServicePlugin.isGooglePlayServicesAvailable(application) == false) {
            return false;
        }

        return true;
    }

    @Override
    public void onAppCreate(@NonNull MengineApplication application) throws MengineServiceInvalidInitializeException {
        PlayGamesSdk.initialize(application);
    }

    @Override
    public void onCreate(@NonNull MengineActivity activity, Bundle savedInstanceState) throws MengineServiceInvalidInitializeException {
        GamesSignInClient gamesSignInClient = PlayGames.getGamesSignInClient(activity);

        gamesSignInClient.isAuthenticated().addOnCompleteListener(isAuthenticatedTask -> {
            if (isAuthenticatedTask.isSuccessful() == false) {
                this.logError("[ERROR] google game social isAuthenticated failed: %s"
                    , isAuthenticatedTask.getException().getMessage()
                );

                this.pythonCall("onGoogleGameSocialOnAuthenticatedError");

                return;
            }

            AuthenticationResult result = isAuthenticatedTask.getResult();

            if (result.isAuthenticated() == false) {
                this.logMessageRelease("google game social isAuthenticated failed");

                this.pythonCall("onGoogleGameSocialOnAuthenticatedError");

                return;
            }

            this.logMessageRelease("google game social isAuthenticated successful");

            this.pythonCall("onGoogleGameSocialOnAuthenticatedSuccess", "authenticated");
        });

        ActivityResultLauncher<Intent> achievementLauncher = activity.registerForActivityResult(new ActivityResultContracts.StartActivityForResult()
            , result -> {
                if (result.getResultCode() == Activity.RESULT_OK) {
                    Intent data = result.getData();

                    this.logMessage("achievementLauncher onActivityResult intent: %s"
                        , data
                    );

                    //ToDo
                }
            }
        );

        m_achievementLauncher = achievementLauncher;

        ActivityResultLauncher<Intent> leaderboardLauncher = activity.registerForActivityResult(new ActivityResultContracts.StartActivityForResult()
            , result -> {
                if (result.getResultCode() == Activity.RESULT_OK) {
                    Intent data = result.getData();

                    this.logMessage("leaderboardLauncher onActivityResult intent: %s"
                        , data
                    );

                    //ToDo
                }
            }
        );

        m_leaderboardLauncher = leaderboardLauncher;
    }

    @Override
    public void onDestroy(@NonNull MengineActivity activity) {
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
                    this.logMessageRelease("get achievements successful");

                    this.pythonCall("onGoogleGameSocialShowAchievementSuccess");

                    m_achievementLauncher.launch(intent);
                })
                .addOnFailureListener(e -> {
                    this.logError("[ERROR] get achievements error: %s"
                        , e.getMessage()
                    );

                    this.pythonCall("onGoogleGameSocialShowAchievementError");
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
                    this.logMessage("unlockAchievement complete achievementId: %s"
                        , achievementId
                    );

                    this.pythonCall("onGoogleGameSocialUnlockAchievementSuccess", achievementId);

                }).addOnFailureListener(e -> {
                    this.logError("[ERROR] unlockAchievement achievementId: %s error: %s"
                        , achievementId
                        , e.getMessage()
                    );

                    this.pythonCall("onGoogleGameSocialUnlockAchievementError", achievementId);
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
                    this.logMessage("incrementAchievement complete achievementId: %s numSteps: %d"
                        , achievementId
                        , numSteps
                    );

                    this.pythonCall("onGoogleGameSocialIncrementAchievementSuccess", achievementId);
                }).addOnFailureListener(e -> {
                    this.logError("[ERROR] incrementAchievement achievementId: %s numSteps: %d error: %s"
                        , achievementId
                        , numSteps
                        , e.getMessage()
                    );

                    this.pythonCall("onGoogleGameSocialIncrementAchievementError", achievementId);
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
                    this.logMessage("revealAchievement complete achievementId: %s"
                        , achievementId
                    );

                    this.pythonCall("onGoogleGameSocialAchievementRevealSuccess", achievementId);
                }).addOnFailureListener(e -> {
                    this.logError("[ERROR] revealAchievement achievementId: %s error: %s"
                        , achievementId
                        , e.getMessage()
                    );

                    this.pythonCall("onGoogleGameSocialAchievementRevealError", achievementId);
                });
        });

        return true;
    }

    public void submitLeaderboardScore(String leaderboardId, long value) {
        this.logMessage("updateLeaderboards: %d"
            , value
        );

        this.runOnUiThread(() -> {
            MengineActivity activity = this.getMengineActivity();

            PlayGames.getLeaderboardsClient(activity).submitScoreImmediate(leaderboardId, value)
                .addOnSuccessListener(result -> {
                    this.logMessage("submitLeaderboardScore complete leaderboardId: %s value: %d"
                        , leaderboardId
                        , value
                    );
                })
                .addOnFailureListener(e -> {
                    this.logError("[ERROR] submitLeaderboardScore leaderboardId: %s value: %d error: %s"
                        , leaderboardId
                        , value
                        , e.getMessage()
                    );
                });
        });
    }

    public void showLeaderboard(String leaderboardId) {
        this.logMessage("showLeaderboard: %s"
            , leaderboardId
        );

        this.runOnUiThread(() -> {
            MengineActivity activity = this.getMengineActivity();

            PlayGames.getLeaderboardsClient(activity).getLeaderboardIntent(leaderboardId)
                .addOnSuccessListener(intent -> {
                    this.logMessage("showLeaderboard complete leaderboardId: %s"
                        , leaderboardId
                    );

                    m_leaderboardLauncher.launch(intent);
                })
                .addOnFailureListener(e -> {
                    this.logError("[ERROR] showLeaderboard leaderboardId: %s error: %s"
                        , leaderboardId
                        , e.getMessage()
                    );
                });
        });
    }
}
