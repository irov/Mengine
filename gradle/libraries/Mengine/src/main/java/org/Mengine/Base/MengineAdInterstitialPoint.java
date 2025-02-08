package org.Mengine.Base;

import androidx.annotation.NonNull;

import org.json.JSONObject;

public class MengineAdInterstitialPoint extends MengineAdBasePoint {
    public static final String TAG = "MengineAdInterstitialPoint";

    public int m_id;
    public boolean m_enabled;
    public int m_actionOffset;
    public int m_actionCooldown;
    public int m_showLimit;
    public long m_timeOffset;
    public long m_timeCooldown;
    public long m_installTimeOffset;
    public String m_cooldownGroupName;

    public int m_actionAttempts = 0;
    public int m_showCount = 0;

    public MengineAdCooldown m_cooldown;

    MengineAdInterstitialPoint(@NonNull String name, @NonNull JSONObject values) {
        super(name);

        m_id = this.parseAdPointInteger(values, "id", false, 1);
        m_enabled = this.parseAdPointBoolean(values, "enable", true, false);
        m_actionOffset = this.parseAdPointInteger(values, "action_offset", false, -1);
        m_actionCooldown = this.parseAdPointInteger(values, "action_cooldown", false, -1);
        m_showLimit = this.parseAdPointInteger(values, "show_limit", false, -1);
        m_timeOffset = this.parseAdPointLong(values, "time_offset", false, -1);
        m_timeCooldown = this.parseAdPointLong(values, "time_cooldown", false, -1);
        m_installTimeOffset = this.parseAdPointLong(values, "install_time_offset", false, 600);
        m_cooldownGroupName = this.parseAdPointString(values, "cooldown_group", false, null);
    }

    public String getCooldownGroupName() {
        return m_cooldownGroupName;
    }

    public void setCooldown(MengineAdCooldown cooldown) {
        m_cooldown = cooldown;
    }

    public MengineAdCooldown getCooldown() {
        return m_cooldown;
    }

    public boolean canYouShowAd(@NonNull MengineApplication application) {
        if (m_enabled == false) {
            return false;
        }

        m_actionAttempts++;

        if (m_actionOffset != -1 && m_actionAttempts < m_actionOffset) {
            return false;
        }

        if (m_actionCooldown != -1 && m_actionCooldown != 0) {
            if (m_actionAttempts % m_actionCooldown != 0) {
                return false;
            }
        }

        if (m_showLimit != -1 && m_showCount >= m_showLimit) {
            return false;
        }

        long currentTimestamp = MengineUtils.getTimestamp();

        if (m_installTimeOffset != -1) {
            long installTimestamp = application.getInstallTimestamp();

            if (currentTimestamp - installTimestamp < m_installTimeOffset) {
                return false;
            }
        }

        long sessionTimestamp = application.getSessionTimestamp();

        if (m_timeOffset != -1) {
            if (currentTimestamp - sessionTimestamp < m_timeOffset) {
                return false;
            }
        }

        long cooldownTimestamp = m_cooldown.getLastShownTimestamp();

        if (m_timeCooldown != -1) {
            if (currentTimestamp - cooldownTimestamp < m_timeCooldown) {
                return false;
            }
        }

        return true;
    }

    public void showAd() {
        m_showCount++;
    }

    public void completeAd() {
        m_cooldown.resetShownTimestamp();
    }
}