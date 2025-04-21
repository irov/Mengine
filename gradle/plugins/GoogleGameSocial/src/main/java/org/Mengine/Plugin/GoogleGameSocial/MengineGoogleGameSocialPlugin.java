package org.Mengine.Plugin.GoogleGameSocial;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;

import androidx.activity.result.ActivityResultLauncher;
import androidx.annotation.NonNull;

import com.google.android.gms.games.AchievementsClient;
import com.google.android.gms.games.AuthenticationResult;
import com.google.android.gms.games.EventsClient;
import com.google.android.gms.games.GamesSignInClient;
import com.google.android.gms.games.LeaderboardsClient;
import com.google.android.gms.games.PlayGames;
import com.google.android.gms.games.PlayGamesSdk;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineFragmentGame;
import org.Mengine.Base.MengineNetwork;
import org.Mengine.Base.MengineService;
import org.Mengine.Base.MengineListenerActivity;
import org.Mengine.Base.MengineListenerApplication;
import org.Mengine.Base.MengineServiceInvalidInitializeException;

import org.Mengine.Plugin.GoogleService.MengineGoogleServicePlugin;

public class MengineGoogleGameSocialPlugin extends MengineService implements MengineListenerApplication, MengineListenerActivity {
    public static final String SERVICE_NAME = "GGameSocial";
    public static final boolean SERVICE_EMBEDDING = true;

    private ActivityResultLauncher<Intent> m_achievementLauncher;
    private ActivityResultLauncher<Intent> m_leaderboardLauncher;

    private boolean m_isAuthenticated = false;

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
        ActivityResultLauncher<Intent> achievementLauncher = activity.registerForActivityResult(result -> {
            if (result.getResultCode() == Activity.RESULT_OK) {
                Intent data = result.getData();

                this.logInfo("achievementLauncher onActivityResult intent: %s"
                    , data
                );

                //ToDo
            }
        });

        m_achievementLauncher = achievementLauncher;

        ActivityResultLauncher<Intent> leaderboardLauncher = activity.registerForActivityResult(result -> {
            if (result.getResultCode() == Activity.RESULT_OK) {
                Intent data = result.getData();

                this.logInfo("leaderboardLauncher onActivityResult intent: %s"
                    , data
                );

                //ToDo
            }
        });

        m_leaderboardLauncher = leaderboardLauncher;
    }

    @Override
    public void onDestroy(@NonNull MengineActivity activity) {
        if (m_achievementLauncher != null) {
            m_achievementLauncher.unregister();

            m_achievementLauncher = null;
        }

        if (m_leaderboardLauncher != null) {
            m_leaderboardLauncher.unregister();

            m_leaderboardLauncher = null;
        }
    }

    private void signInSilently(@NonNull MengineActivity activity) {
        GamesSignInClient gamesSignInClient = PlayGames.getGamesSignInClient(activity);

        gamesSignInClient.isAuthenticated().addOnCompleteListener(isAuthenticatedTask -> {
            boolean isAuthenticated = isAuthenticatedTask.isSuccessful();

            if (isAuthenticated == false) {
                Exception e = isAuthenticatedTask.getException();

                this.logError("[ERROR] google game social isAuthenticated failed: %s"
                    , e.getMessage()
                );

                m_isAuthenticated = false;

                this.pythonCall("onGoogleGameSocialOnAuthenticatedError", e);

                return;
            }

            AuthenticationResult result = isAuthenticatedTask.getResult();

            if (result.isAuthenticated() == false) {
                this.logInfo("google game social isAuthenticated failed");

                m_isAuthenticated = false;

                this.pythonCall("onGoogleGameSocialOnAuthenticatedFailed");

                return;
            }

            this.logInfo("google game social isAuthenticated success");

            m_isAuthenticated = true;

            this.pythonCall("onGoogleGameSocialOnAuthenticatedSuccess");
        });
    }

    @Override
    public void onResume(@NonNull MengineActivity activity) {
        this.signInSilently(activity);
    }

    public boolean showAchievements() {
        this.logInfo("showAchievements");

        MengineActivity activity = this.getMengineActivity();

        AchievementsClient achievementsClient = PlayGames.getAchievementsClient(activity);

        achievementsClient.getAchievementsIntent()
            .addOnSuccessListener(intent -> {
                this.logInfo("get achievements success");

                this.pythonCall("onGoogleGameSocialShowAchievementSuccess");

                m_achievementLauncher.launch(intent);
            })
            .addOnFailureListener(e -> {
                this.logError("[ERROR] get achievements error: %s"
                    , e.getMessage()
                );

                this.pythonCall("onGoogleGameSocialShowAchievementError", e);
            });

        return true;
    }

    public boolean unlockAchievement(String achievementId) {
        if (MengineNetwork.isNetworkAvailable() == false) {
            this.logInfo("unlockAchievement achievementId: %s network not available"
                , achievementId
            );

            return false;
        }

        if (m_isAuthenticated == false) {
            this.logInfo("unlockAchievement achievementId: %s not authenticated"
                , achievementId
            );

            return false;
        }

        this.logInfo("unlockAchievement achievementId: %s"
            , achievementId
        );

        MengineActivity activity = this.getMengineActivity();

        AchievementsClient achievementsClient = PlayGames.getAchievementsClient(activity);

        achievementsClient.unlockImmediate(achievementId)
            .addOnSuccessListener(unused -> {
                this.logInfo("unlockAchievement complete achievementId: %s"
                    , achievementId
                );

                MengineFragmentGame.INSTANCE.unlockAchievement(achievementId);

                this.pythonCall("onGoogleGameSocialUnlockAchievementSuccess", achievementId);
            }).addOnFailureListener(e -> {
                this.logError("[ERROR] unlockAchievement achievementId: %s error: %s"
                    , achievementId
                    , e.getMessage()
                );

                this.pythonCall("onGoogleGameSocialUnlockAchievementError", achievementId, e);
            });

        return true;
    }

    public boolean incrementAchievement(String achievementId, int numSteps) {
        if (MengineNetwork.isNetworkAvailable() == false) {
            this.logInfo("incrementAchievement achievementId: %s numSteps: %d network not available"
                , achievementId
                , numSteps
            );

            return false;
        }

        if (m_isAuthenticated == false) {
            this.logInfo("incrementAchievement achievementId: %s numSteps: %d not authenticated"
                , achievementId
                , numSteps
            );

            return false;
        }

        this.logInfo("incrementAchievement achievementId: %s numSteps: %d"
            , achievementId
            , numSteps
        );

        MengineActivity activity = this.getMengineActivity();

        AchievementsClient achievementsClient = PlayGames.getAchievementsClient(activity);

        achievementsClient.incrementImmediate(achievementId, numSteps)
            .addOnSuccessListener(aBoolean -> {
                this.logInfo("incrementAchievement complete achievementId: %s numSteps: %d"
                    , achievementId
                    , numSteps
                );

                MengineFragmentGame.INSTANCE.incrementAchievement(achievementId, numSteps);

                this.pythonCall("onGoogleGameSocialIncrementAchievementSuccess", achievementId, numSteps);
            }).addOnFailureListener(e -> {
                this.logError("[ERROR] incrementAchievement achievementId: %s numSteps: %d error: %s"
                    , achievementId
                    , numSteps
                    , e.getMessage()
                );

                this.pythonCall("onGoogleGameSocialIncrementAchievementError", achievementId, numSteps, e);
            });

        return true;
    }

    public boolean revealAchievement(String achievementId) {
        if (MengineNetwork.isNetworkAvailable() == false) {
            this.logInfo("revealAchievement achievementId: %s network not available"
                , achievementId
            );

            return false;
        }

        if (m_isAuthenticated == false) {
            this.logInfo("revealAchievement achievementId: %s not authenticated"
                , achievementId
            );

            return false;
        }

        this.logInfo("revealAchievement achievementId: %s "
            , achievementId
        );

        MengineActivity activity = this.getMengineActivity();

        AchievementsClient achievementsClient = PlayGames.getAchievementsClient(activity);

        achievementsClient.revealImmediate(achievementId)
            .addOnSuccessListener(unused -> {
                this.logInfo("revealAchievement complete achievementId: %s"
                    , achievementId
                );

                MengineFragmentGame.INSTANCE.revealAchievement(achievementId);

                this.pythonCall("onGoogleGameSocialRevealAchievementSuccess", achievementId);
            }).addOnFailureListener(e -> {
                this.logError("[ERROR] revealAchievement achievementId: %s error: %s"
                    , achievementId
                    , e.getMessage()
                );

                this.pythonCall("onGoogleGameSocialRevealAchievementError", achievementId, e);
            });

        return true;
    }

    public boolean submitLeaderboardScore(String leaderboardId, long score) {
        if (MengineNetwork.isNetworkAvailable() == false) {
            this.logInfo("submitLeaderboardScore leaderboardId: %s score: %d network not available"
                , leaderboardId
                , score
            );

            return false;
        }

        if (m_isAuthenticated == false) {
            this.logInfo("submitLeaderboardScore leaderboardId: %s score: %d not authenticated"
                , leaderboardId
                , score
            );

            return false;
        }

        this.logInfo("submitLeaderboardScore leaderboardId: %s score: %d"
            , leaderboardId
            , score
        );

        MengineActivity activity = this.getMengineActivity();

        LeaderboardsClient leaderboardsClient = PlayGames.getLeaderboardsClient(activity);

        leaderboardsClient.submitScoreImmediate(leaderboardId, score)
            .addOnSuccessListener(result -> {
                this.logInfo("submitLeaderboardScore complete leaderboardId: %s score: %d"
                    , leaderboardId
                    , score
                );

                MengineFragmentGame.INSTANCE.submitLeaderboardScore(leaderboardId, score);

                this.pythonCall("onGoogleGameSocialLeaderboardScoreSuccess", leaderboardId, score);
            })
            .addOnFailureListener(e -> {
                this.logError("[ERROR] submitLeaderboardScore leaderboardId: %s score: %d error: %s"
                    , leaderboardId
                    , score
                    , e.getMessage()
                );

                this.pythonCall("onGoogleGameSocialLeaderboardScoreError", leaderboardId, score, e);
            });

        return true;
    }

    public void showLeaderboard(String leaderboardId) {
        this.logInfo("showLeaderboard: %s"
            , leaderboardId
        );

        MengineActivity activity = this.getMengineActivity();

        LeaderboardsClient leaderboardsClient = PlayGames.getLeaderboardsClient(activity);

        leaderboardsClient.getLeaderboardIntent(leaderboardId)
            .addOnSuccessListener(intent -> {
                this.logInfo("showLeaderboard complete leaderboardId: %s"
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
    }

    public boolean incrementEvent(String eventId, int value) {
        if (MengineNetwork.isNetworkAvailable() == false) {
            this.logInfo("incrementEvent eventId: %s value: %d network not available"
                , eventId
                , value
            );

            return false;
        }

        if (m_isAuthenticated == false) {
            this.logInfo("incrementEvent eventId: %s value: %d not authenticated"
                , eventId
                , value
            );

            return false;
        }

        this.logInfo("incrementEvent: %s value: %d"
            , eventId
            , value
        );

        MengineActivity activity = this.getMengineActivity();

        EventsClient eventsClient = PlayGames.getEventsClient(activity);

        eventsClient.increment(eventId, value);

        return true;
    }
}
