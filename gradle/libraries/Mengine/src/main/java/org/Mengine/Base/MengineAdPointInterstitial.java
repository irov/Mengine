package org.Mengine.Base;

import androidx.annotation.NonNull;

import org.json.JSONObject;

public class MengineAdPointInterstitial extends MengineAdPointBase {
    public static final MengineTag TAG = MengineTag.of("MengineAdPointInterstitial");

    protected int m_actionOffset;
    protected int m_actionCooldown;
    protected long m_timeOffset;
    protected long m_timeCooldown;
    protected long m_installTimeOffset;
    protected long m_sessionOffset;

    MengineAdPointInterstitial(@NonNull String name, @NonNull JSONObject values) {
        super(name, values);

        m_actionOffset = this.parseAdPointInteger(values, "trigger_action_offset", false, -1);
        m_actionCooldown = this.parseAdPointInteger(values, "trigger_action_cooldown", false, -1);
        m_timeOffset = this.parseAdPointTime(values, "trigger_time_offset", false, -1);
        m_timeCooldown = this.parseAdPointTime(values, "trigger_time_cooldown", false, -1);
        m_installTimeOffset = this.parseAdPointTime(values, "trigger_install_time_offset", false, 600);
        m_sessionOffset = this.parseAdPointLong(values, "trigger_session_offset", false, -1);
    }

    public boolean canYouShowAd(@NonNull MengineApplication application) {
        if (BuildConfig.DEBUG == true) {
            if (MengineUtils.hasOption(application, "adservice.always_interstitial_point." + m_name) == true) {
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

        int attempt = m_attempts.incrementAttempts();

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

        if (m_timeCooldown > 0) {
            long cooldownTimestamp = m_cooldown.getLastShownTimestamp();

            if (currentTimestamp - cooldownTimestamp < m_timeCooldown) {
                return false;
            }
        }

        return true;
    }
}