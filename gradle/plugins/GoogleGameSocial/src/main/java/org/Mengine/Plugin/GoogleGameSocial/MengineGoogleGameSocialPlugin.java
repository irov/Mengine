package org.Mengine.Plugin.GoogleGameSocial;

import android.app.Activity;
import android.app.PendingIntent;
import android.content.Intent;
import android.content.IntentSender;
import android.os.Bundle;

import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.IntentSenderRequest;
import androidx.activity.result.contract.ActivityResultContracts;
import androidx.annotation.NonNull;

import com.google.android.gms.common.api.ResolvableApiException;
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

import java.util.Map;

public class MengineGoogleGameSocialPlugin extends MengineService implements MengineListenerApplication, MengineListenerActivity {
    public static final String SERVICE_NAME = "GGameSocial";
    public static final boolean SERVICE_EMBEDDING = true;

    private ActivityResultLauncher<Intent> m_achievementLauncher;
    private ActivityResultLauncher<Intent> m_leaderboardLauncher;
    private ActivityResultLauncher<IntentSenderRequest> m_gamesResolutionLauncher;

    private GamesSignInClient m_gamesSignInClient;
    private AchievementsClient m_achievementsClient;
    private LeaderboardsClient m_leaderboardsClient;
    private EventsClient m_eventsClient;

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
        ActivityResultLauncher<Intent> achievementLauncher = activity.registerForActivityResultIntent(result -> {
            if (result.getResultCode() == Activity.RESULT_OK) {
                Intent data = result.getData();

                this.logInfo("achievementLauncher onActivityResult intent: %s"
                    , data
                );

                this.nativeCall("onGoogleGameSocialShowAchievementSuccess");
            }
        });

        m_achievementLauncher = achievementLauncher;

        ActivityResultLauncher<Intent> leaderboardLauncher = activity.registerForActivityResultIntent(result -> {
            if (result.getResultCode() == Activity.RESULT_OK) {
                Intent data = result.getData();

                this.logInfo("leaderboardLauncher onActivityResult intent: %s"
                    , data
                );

                this.nativeCall("onGoogleGameSocialShowLeaderboardSuccess");
            } else {
                this.logInfo("leaderboardLauncher onActivityResult resultCode: %d"
                    , result.getResultCode()
                );
            }
        });

        m_leaderboardLauncher = leaderboardLauncher;



        ActivityResultLauncher<IntentSenderRequest> gamesResolutionLauncher = activity.registerForActivityResultIntentSender(result -> {
            if (result.getResultCode() == Activity.RESULT_OK) {
                Intent data = result.getData();

                this.logInfo("gamesResolutionLauncher onActivityResult intent: %s",
                    data
                );

                this.signInIntent();
            } else {
                this.logInfo("gamesResolutionLauncher onActivityResult resultCode: %d",
                    result.getResultCode()
                );
            }
        });

        m_gamesResolutionLauncher = gamesResolutionLauncher;

        m_gamesSignInClient = PlayGames.getGamesSignInClient(activity);
        m_achievementsClient = PlayGames.getAchievementsClient(activity);
        m_leaderboardsClient = PlayGames.getLeaderboardsClient(activity);
        m_eventsClient = PlayGames.getEventsClient(activity);
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

        if (m_gamesResolutionLauncher != null) {
            m_gamesResolutionLauncher.unregister();

            m_gamesResolutionLauncher = null;
        }

        m_gamesSignInClient = null;
        m_achievementsClient = null;
        m_leaderboardsClient = null;
        m_eventsClient = null;
    }

    @Override
    public void onResume(@NonNull MengineActivity activity) {
        this.signInSilently();
    }

    public boolean isAuthenticated() {
        return m_isAuthenticated;
    }

    private void signInSilently() {
        if (m_isAuthenticated == true) {
            this.logInfo("signInSilently already authenticated");

            return;
        }

        if (MengineNetwork.isNetworkAvailable() == false) {
            this.logInfo("signInSilently network not available");

            return;
        }

        m_gamesSignInClient.isAuthenticated().addOnCompleteListener(isAuthenticatedTask -> {
            boolean isSuccessful = isAuthenticatedTask.isSuccessful();

            if (isSuccessful == false) {
                Exception e = isAuthenticatedTask.getException();

                if (e != null) {
                    this.logException(e, Map.of());
                } else {
                    this.logError("[ERROR] google game social isAuthenticated failed: null exception");
                }

                m_isAuthenticated = false;

                return;
            }

            AuthenticationResult result = isAuthenticatedTask.getResult();

            if (result.isAuthenticated() == false) {
                this.logInfo("google game social isAuthenticated failed");

                m_isAuthenticated = false;

                return;
            }

            this.logInfo("google game social isAuthenticated success");

            m_isAuthenticated = true;
        });
    }

    public void signInIntent() {
        if (MengineNetwork.isNetworkAvailable() == false) {
            this.logInfo("signInIntent network not available");

            this.nativeCall("onGoogleGameSocialSignInIntentError", new RuntimeException("network not available"));

            return;
        }

        this.logInfo("signInIntent");

        m_gamesSignInClient.signIn().addOnCompleteListener(task -> {
            if (task.isSuccessful() == false) {
                Exception e = task.getException();

                if (e instanceof ResolvableApiException) {
                    ResolvableApiException resolvable = (ResolvableApiException)e;
                    PendingIntent pendingIntent = resolvable.getResolution();

                    if (pendingIntent == null) {
                        this.logInfo("signInIntent ResolvableApiException resolution null");

                        this.nativeCall("onGoogleGameSocialSignInIntentError", e);

                        return;
                    }

                    try {
                        IntentSender sender = pendingIntent.getIntentSender();

                        IntentSenderRequest request = new IntentSenderRequest.Builder(sender)
                            .build();

                        if (m_gamesResolutionLauncher != null) {
                            m_gamesResolutionLauncher.launch(request);
                        } else {
                            this.logError("[ERROR] signInIntent gamesResolutionLauncher not available");

                            this.nativeCall("onGoogleGameSocialSignInIntentError", e);
                        }
                    } catch (Exception ex) {
                        this.logException(ex, Map.of());

                        this.nativeCall("onGoogleGameSocialSignInIntentError", ex);
                    }

                    return;
                }

                if (e != null) {
                    this.logException(e, Map.of());
                } else {
                    this.logError("[ERROR] signInIntent failed: null exception");
                }

                this.nativeCall("onGoogleGameSocialSignInIntentError", e);

                return;
            }

            AuthenticationResult result = task.getResult();

            if (result.isAuthenticated() == false) {
                this.logInfo("signInIntent failed");

                m_isAuthenticated = false;

                this.nativeCall("onGoogleGameSocialSignInIntentFailed");

                return;
            }

            this.logInfo("signInIntent success");

            m_isAuthenticated = true;

            this.nativeCall("onGoogleGameSocialSignInIntentSuccess");
        });
    }

    public void showAchievements() {
        if (MengineNetwork.isNetworkAvailable() == false) {
            this.logInfo("showAchievements network not available");

            this.nativeCall("onGoogleGameSocialShowAchievementError", new RuntimeException("network not available"));

            return;
        }

        this.logInfo("showAchievements");

        m_achievementsClient.getAchievementsIntent()
            .addOnSuccessListener(intent -> {
                this.logInfo("get achievements success");

                m_achievementLauncher.launch(intent);
            }).addOnFailureListener(e -> {
                this.logException(e, Map.of());

                this.nativeCall("onGoogleGameSocialShowAchievementError", e);
            }).addOnCanceledListener(() -> {
                this.logInfo("get achievements canceled");

                this.nativeCall("onGoogleGameSocialShowAchievementCanceled");
            });
    }

    public void unlockAchievement(String achievementId) {
        if (MengineNetwork.isNetworkAvailable() == false) {
            this.logInfo("unlockAchievement achievementId: %s network not available"
                , achievementId
            );

            this.nativeCall("onGoogleGameSocialUnlockAchievementError", achievementId, new RuntimeException("network not available"));

            return;
        }

        if (m_isAuthenticated == false) {
            this.logInfo("unlockAchievement achievementId: %s not authenticated"
                , achievementId
            );

            this.nativeCall("onGoogleGameSocialUnlockAchievementError", achievementId, new RuntimeException("not authenticated"));

            return;
        }

        this.logInfo("unlockAchievement achievementId: %s"
            , achievementId
        );

        m_achievementsClient.unlockImmediate(achievementId)
            .addOnSuccessListener(unused -> {
                this.logInfo("unlockAchievement complete achievementId: %s"
                    , achievementId
                );

                MengineFragmentGame.INSTANCE.unlockAchievement(achievementId);

                this.nativeCall("onGoogleGameSocialUnlockAchievementSuccess", achievementId);
            }).addOnFailureListener(e -> {
                this.logException(e, Map.of(
                    "achievementId", achievementId
                ));

                this.nativeCall("onGoogleGameSocialUnlockAchievementError", achievementId, e);
            }).addOnCanceledListener(() -> {
                this.logInfo("unlockAchievement canceled achievementId: %s"
                    , achievementId
                );

                this.nativeCall("onGoogleGameSocialUnlockAchievementCanceled", achievementId);
            });
    }

    public void incrementAchievement(String achievementId, int numSteps) {
        if (MengineNetwork.isNetworkAvailable() == false) {
            this.logInfo("incrementAchievement achievementId: %s numSteps: %d network not available"
                , achievementId
                , numSteps
            );

            this.nativeCall("onGoogleGameSocialIncrementAchievementError", achievementId, numSteps, new RuntimeException("network not available"));

            return;
        }

        if (m_isAuthenticated == false) {
            this.logInfo("incrementAchievement achievementId: %s numSteps: %d not authenticated"
                , achievementId
                , numSteps
            );

            this.nativeCall("onGoogleGameSocialIncrementAchievementError", achievementId, numSteps, new RuntimeException("not authenticated"));

            return;
        }

        this.logInfo("incrementAchievement achievementId: %s numSteps: %d"
            , achievementId
            , numSteps
        );

        m_achievementsClient.incrementImmediate(achievementId, numSteps)
            .addOnSuccessListener(aBoolean -> {
                this.logInfo("incrementAchievement complete achievementId: %s numSteps: %d"
                    , achievementId
                    , numSteps
                );

                MengineFragmentGame.INSTANCE.incrementAchievement(achievementId, numSteps);

                this.nativeCall("onGoogleGameSocialIncrementAchievementSuccess", achievementId, numSteps);
            }).addOnFailureListener(e -> {
                this.logException(e, Map.of(
                    "achievementId", achievementId,
                    "numSteps", numSteps
                ));

                this.nativeCall("onGoogleGameSocialIncrementAchievementError", achievementId, numSteps, e);
            }).addOnCanceledListener(() -> {
                this.logInfo("incrementAchievement canceled achievementId: %s numSteps: %d"
                    , achievementId
                    , numSteps
                );

                this.nativeCall("onGoogleGameSocialIncrementAchievementCanceled", achievementId, numSteps);
            });
    }

    public void revealAchievement(String achievementId) {
        if (MengineNetwork.isNetworkAvailable() == false) {
            this.logInfo("revealAchievement achievementId: %s network not available"
                , achievementId
            );

            this.nativeCall("onGoogleGameSocialRevealAchievementError", achievementId, new RuntimeException("network not available"));

            return;
        }

        if (m_isAuthenticated == false) {
            this.logInfo("revealAchievement achievementId: %s not authenticated"
                , achievementId
            );

            this.nativeCall("onGoogleGameSocialRevealAchievementError", achievementId, new RuntimeException("not authenticated"));

            return;
        }

        this.logInfo("revealAchievement achievementId: %s "
            , achievementId
        );

        m_achievementsClient.revealImmediate(achievementId)
            .addOnSuccessListener(unused -> {
                this.logInfo("revealAchievement complete achievementId: %s"
                    , achievementId
                );

                MengineFragmentGame.INSTANCE.revealAchievement(achievementId);

                this.nativeCall("onGoogleGameSocialRevealAchievementSuccess", achievementId);
            }).addOnFailureListener(e -> {
                this.logException(e, Map.of(
                    "achievementId", achievementId
                ));

                this.nativeCall("onGoogleGameSocialRevealAchievementError", achievementId, e);
            }).addOnCanceledListener(() -> {
                this.logInfo("revealAchievement canceled achievementId: %s"
                    , achievementId
                );

                this.nativeCall("onGoogleGameSocialRevealAchievementCanceled", achievementId);
            });
    }

    public void submitLeaderboardScore(String leaderboardId, long score) {
        if (MengineNetwork.isNetworkAvailable() == false) {
            this.logInfo("submitLeaderboardScore leaderboardId: %s score: %d network not available"
                , leaderboardId
                , score
            );

            this.nativeCall("onGoogleGameSocialLeaderboardScoreError", leaderboardId, score, new RuntimeException("network not available"));

            return;
        }

        if (m_isAuthenticated == false) {
            this.logInfo("submitLeaderboardScore leaderboardId: %s score: %d not authenticated"
                , leaderboardId
                , score
            );

            this.nativeCall("onGoogleGameSocialLeaderboardScoreError", leaderboardId, score, new RuntimeException("not authenticated"));

            return;
        }

        this.logInfo("submitLeaderboardScore leaderboardId: %s score: %d"
            , leaderboardId
            , score
        );

        m_leaderboardsClient.submitScoreImmediate(leaderboardId, score)
            .addOnSuccessListener(result -> {
                this.logInfo("submitLeaderboardScore complete leaderboardId: %s score: %d"
                    , leaderboardId
                    , score
                );

                MengineFragmentGame.INSTANCE.submitLeaderboardScore(leaderboardId, score);

                this.nativeCall("onGoogleGameSocialLeaderboardScoreSuccess", leaderboardId, score);
            }).addOnFailureListener(e -> {
                this.logException(e, Map.of(
                    "leaderboardId", leaderboardId,
                    "score", score
                ));

                this.nativeCall("onGoogleGameSocialLeaderboardScoreError", leaderboardId, score, e);
            }).addOnCanceledListener(() -> {
                this.logInfo("submitLeaderboardScore canceled leaderboardId: %s score: %d"
                    , leaderboardId
                    , score
                );

                this.nativeCall("onGoogleGameSocialLeaderboardScoreCanceled", leaderboardId, score);
            });
    }

    public void showLeaderboard(String leaderboardId) {
        if (MengineNetwork.isNetworkAvailable() == false) {
            this.logInfo("showLeaderboard leaderboardId: %s network not available"
                , leaderboardId
            );

            this.nativeCall("onGoogleGameSocialShowLeaderboardError", leaderboardId, new RuntimeException("network not available"));

            return;
        }

        if (m_isAuthenticated == false) {
            this.logInfo("showLeaderboard leaderboardId: %s not authenticated"
                , leaderboardId
            );

            this.nativeCall("onGoogleGameSocialShowLeaderboardError", leaderboardId, new RuntimeException("not authenticated"));

            return;
        }

        this.logInfo("showLeaderboard: %s"
            , leaderboardId
        );

        m_leaderboardsClient.getLeaderboardIntent(leaderboardId)
            .addOnSuccessListener(intent -> {
                this.logInfo("showLeaderboard complete leaderboardId: %s"
                    , leaderboardId
                );

                m_leaderboardLauncher.launch(intent);
            }).addOnFailureListener(e -> {
                this.logException(e, Map.of(
                    "leaderboardId", leaderboardId
                ));

                this.nativeCall("onGoogleGameSocialShowLeaderboardError", leaderboardId, e);
            }).addOnCanceledListener(() -> {
                this.logInfo("showLeaderboard canceled leaderboardId: %s"
                    , leaderboardId
                );

                this.nativeCall("onGoogleGameSocialShowLeaderboardCanceled", leaderboardId);
            });
    }

    public void incrementEvent(String eventId, int value) {
        if (MengineNetwork.isNetworkAvailable() == false) {
            this.logInfo("incrementEvent eventId: %s value: %d network not available"
                , eventId
                , value
            );

            this.nativeCall("onGoogleGameSocialIncrementEventError", eventId, value, new RuntimeException("network not available"));

            return;
        }

        if (m_isAuthenticated == false) {
            this.logInfo("incrementEvent eventId: %s value: %d not authenticated"
                , eventId
                , value
            );

            this.nativeCall("onGoogleGameSocialIncrementEventError", eventId, value, new RuntimeException("not authenticated"));

            return;
        }

        this.logInfo("incrementEvent: %s value: %d"
            , eventId
            , value
        );

        m_eventsClient.increment(eventId, value);

        this.nativeCall("onGoogleGameSocialIncrementEventSuccess", eventId, value);
    }
}
