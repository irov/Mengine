package org.Mengine.Plugin.GoogleGameSocial;

import androidx.annotation.NonNull;

import com.google.android.gms.games.achievement.Achievement;

import org.json.JSONException;
import org.json.JSONObject;

public final class MengineGoogleGameSocialAchievement {
    private final String m_id;
    private String m_name;
    private String m_description;
    private int m_state;
    private int m_type;
    private long m_xpValue;
    private long m_lastUpdatedTimestamp;
    private int m_currentSteps;
    private int m_totalSteps;

    public MengineGoogleGameSocialAchievement(@NonNull Achievement achievement) {
        m_id = achievement.getAchievementId();

        this.updateFromAchievement(achievement);
    }

    private MengineGoogleGameSocialAchievement(@NonNull String achievementId) {
        m_id = achievementId;

        m_name = "";
        m_description = "";
        m_state = Achievement.STATE_HIDDEN;
        m_type = Achievement.TYPE_STANDARD;
        m_xpValue = 0L;
        m_lastUpdatedTimestamp = 0L;
        m_currentSteps = 0;
        m_totalSteps = 0;
    }

    public static MengineGoogleGameSocialAchievement placeholder(@NonNull String achievementId) {
        return new MengineGoogleGameSocialAchievement(achievementId);
    }

    public void updateFromAchievement(@NonNull Achievement achievement) {
        m_name = achievement.getName();
        m_description = achievement.getDescription();
        m_state = achievement.getState();
        m_type = achievement.getType();
        m_xpValue = achievement.getXpValue();
        m_lastUpdatedTimestamp = achievement.getLastUpdatedTimestamp();

        if (achievement.getType() == Achievement.TYPE_INCREMENTAL) {
            m_currentSteps = achievement.getCurrentSteps();
            m_totalSteps = achievement.getTotalSteps();
        } else {
            m_currentSteps = 0;
            m_totalSteps = 0;
        }
    }

    public boolean isUnlocked() {
        return m_state == Achievement.STATE_UNLOCKED;
    }

    public boolean isIncremental() {
        return m_type == Achievement.TYPE_INCREMENTAL;
    }

    public String getId() {
        return m_id;
    }

    public void markUnlocked(long timestamp) {
        m_state = Achievement.STATE_UNLOCKED;
        m_lastUpdatedTimestamp = timestamp;

        if (this.isIncremental()) {
            m_currentSteps = m_totalSteps;
        }
    }

    public void markRevealed(long timestamp) {
        if (m_state == Achievement.STATE_HIDDEN) {
            m_state = Achievement.STATE_REVEALED;
        }

        m_lastUpdatedTimestamp = timestamp;
    }

    public void incrementSteps(int numSteps, long timestamp) {
        if (this.isIncremental() == false) {
            return;
        }

        if (numSteps <= 0) {
            return;
        }

        m_currentSteps = Math.min(m_totalSteps, m_currentSteps + numSteps);
        m_lastUpdatedTimestamp = timestamp;

        if (m_currentSteps >= m_totalSteps && m_totalSteps > 0) {
            m_state = Achievement.STATE_UNLOCKED;
        }
    }

    public JSONObject toJSONObject() throws JSONException {
        JSONObject achievementJSON = new JSONObject();

        achievementJSON.put("achievementId", m_id);
        achievementJSON.put("name", m_name);
        achievementJSON.put("description", m_description);
        achievementJSON.put("state", m_state);
        achievementJSON.put("type", m_type);
        achievementJSON.put("xpValue", m_xpValue);
        achievementJSON.put("lastUpdatedTimestamp", m_lastUpdatedTimestamp);
        achievementJSON.put("currentSteps", m_currentSteps);
        achievementJSON.put("totalSteps", m_totalSteps);
        achievementJSON.put("isUnlocked", this.isUnlocked());
        achievementJSON.put("isIncremental", this.isIncremental());

        return achievementJSON;
    }
}
