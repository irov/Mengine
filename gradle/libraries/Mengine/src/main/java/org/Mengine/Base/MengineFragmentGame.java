package org.Mengine.Base;

public class MengineFragmentGame extends MengineFragment<MengineListenerGame> {
    public static MengineFragmentGame INSTANCE = null;

    MengineFragmentGame() {
        super(MengineListenerGame.class);

        INSTANCE = this;
    }

    public void unlockAchievement(String achievementId) {
        this.propagate(MengineListenerGame::onMengineUnlockAchievement, achievementId);
    }

    public void incrementAchievement(String achievementId, int numSteps) {
        this.propagate(MengineListenerGame::onMengineIncrementAchievement, achievementId, numSteps);
    }

    public void revealAchievement(String achievementId) {
        this.propagate(MengineListenerGame::onMengineRevealAchievement, achievementId);
    }

    public void submitLeaderboardScore(String leaderboardId, long score) {
        this.propagate(MengineListenerGame::onMengineSubmitLeaderboardScore, leaderboardId, score);
    }
}
