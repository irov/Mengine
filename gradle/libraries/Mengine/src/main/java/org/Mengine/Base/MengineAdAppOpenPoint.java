package org.Mengine.Base;

import androidx.annotation.NonNull;

import org.json.JSONObject;

public class MengineAdAppOpenPoint extends MengineAdBasePoint {
    public static final String TAG = "MengineAdAppOpenPoint";

    protected long m_timeSleep;
    protected int m_actionOffset;
    protected int m_actionCooldown;
    protected long m_timeOffset;
    protected long m_timeCooldown;
    protected long m_installTimeOffset;
    protected long m_sessionOffset;

    MengineAdAppOpenPoint(@NonNull String name, @NonNull JSONObject values) {
        super(name, values);

        m_timeSleep = this.parseAdPointTime(values, "time_sleep", false, -1);
        m_actionOffset = this.parseAdPointInteger(values, "trigger_action_offset", false, -1);
        m_actionCooldown = this.parseAdPointInteger(values, "trigger_action_cooldown", false, -1);
        m_timeOffset = this.parseAdPointTime(values, "trigger_time_offset", false, -1);
        m_timeCooldown = this.parseAdPointTime(values, "trigger_time_cooldown", false, -1);
        m_installTimeOffset = this.parseAdPointTime(values, "trigger_install_time_offset", false, 600);
        m_sessionOffset = this.parseAdPointLong(values, "trigger_session_offset", false, -1);
    }

    public boolean canYouShowAd(@NonNull MengineApplication application, long timeStop) {
        if (BuildConfig.DEBUG == true) {
            if (application.hasOption("adservice.always_appopen_point." + m_name) == true) {
                return true;
            }
        }

        if (m_enabled == false) {
            return false;
        }

        if (m_sessionOffset > 0) {
            long sessionIndex = application.getSessionIndex();

            if (sessionIndex < m_sessionOffset) {
                return false;
            }
        }

        int attempt = m_attempts.attempt();

        if (m_actionOffset >= 0 && attempt < m_actionOffset) {
            return false;
        }

        if (m_actionCooldown > 0) {
            if (m_actionOffset >= 0) {
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

        if (m_installTimeOffset > 0) {
            long installTimestamp = application.getInstallTimestamp();

            if (currentTimestamp - installTimestamp < m_installTimeOffset) {
                return false;
            }
        }

        if (m_timeOffset > 0) {
            long sessionTimestamp = application.getSessionTimestamp();

            if (currentTimestamp - sessionTimestamp < m_timeOffset) {
                return false;
            }
        }

        long cooldownTimestamp = m_cooldown.getLastShownTimestamp();

        if (m_timeCooldown > 0) {
            if (currentTimestamp - cooldownTimestamp < m_timeCooldown) {
                return false;
            }
        }

        if (m_timeSleep >= 0) {
            if (m_timeSleep > timeStop) {
                return false;
            }
        }

        return true;
    }
}