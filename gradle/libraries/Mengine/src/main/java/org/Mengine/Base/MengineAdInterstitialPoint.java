package org.Mengine.Base;

import androidx.annotation.NonNull;

import org.json.JSONObject;

public class MengineAdInterstitialPoint extends MengineAdBasePoint {
    public static final String TAG = "MengineAdInterstitialPoint";

    protected int m_id;
    protected boolean m_enabled;
    protected int m_actionOffset;
    protected int m_actionCooldown;
    protected long m_timeOffset;
    protected long m_timeCooldown;
    protected long m_installTimeOffset;
    protected String m_cooldownGroupName;

    protected MengineAdCooldown m_cooldown;
    protected MengineAdAttempts m_attempts;

    MengineAdInterstitialPoint(@NonNull String name, @NonNull JSONObject values) {
        super(name);

        m_id = this.parseAdPointInteger(values, "id", false, 1);
        m_enabled = this.parseAdPointBoolean(values, "enable", true, false);
        m_actionOffset = this.parseAdPointInteger(values, "trigger_action_offset", false, -1);
        m_actionCooldown = this.parseAdPointInteger(values, "trigger_action_cooldown", false, -1);
        m_timeOffset = this.parseAdPointLong(values, "trigger_time_offset", false, -1);
        m_timeCooldown = this.parseAdPointLong(values, "trigger_time_cooldown", false, -1);
        m_installTimeOffset = this.parseAdPointLong(values, "trigger_install_time_offset", false, 600);
        m_cooldownGroupName = this.parseAdPointString(values, "trigger_cooldown_group", false, null);
    }

    public String getCooldownGroupName() {
        return m_cooldownGroupName;
    }

    public void setCooldown(MengineAdCooldown cooldown) {
        m_cooldown = cooldown;
    }

    public void setAttempts(MengineAdAttempts attempts) {
        m_attempts = attempts;
    }

    public MengineAdCooldown getCooldown() {
        return m_cooldown;
    }

    public boolean canYouShowAd(@NonNull MengineApplication application) {
        if (m_enabled == false) {
            return false;
        }

        int attempt = m_attempts.attempt();

        if (m_actionOffset != -1 && attempt < m_actionOffset) {
            return false;
        }

        if (m_actionCooldown != -1 && m_actionCooldown != 0) {
            if (m_actionOffset != -1) {
                if ((attempt - m_actionOffset) % m_actionCooldown != 0) {
                    return false;
                }
            } else {
                if (attempt % m_actionCooldown != 0) {
                    return false;
                }
            }
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
        //ToDo: Implement this method
    }

    public void completeAd() {
        m_cooldown.resetShownTimestamp();
    }
}