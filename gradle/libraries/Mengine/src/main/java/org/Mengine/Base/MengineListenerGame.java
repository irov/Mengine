package org.Mengine.Base;

import androidx.annotation.NonNull;

public interface MengineListenerGame extends MengineServiceInterface {
    void onMengineUnlockAchievement(@NonNull MengineApplication application, String achievementId);
    void onMengineIncrementAchievement(@NonNull MengineApplication application, String achievementId, int numSteps);
    void onMengineRevealAchievement(@NonNull MengineApplication application, String achievementId);
    void onMengineSubmitLeaderboardScore(@NonNull MengineApplication application, String leaderboardId, long score);
}