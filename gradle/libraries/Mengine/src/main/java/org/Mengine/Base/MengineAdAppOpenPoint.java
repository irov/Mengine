package org.Mengine.Base;

import androidx.annotation.NonNull;

import org.json.JSONObject;

public class MengineAdAppOpenPoint extends MengineAdBasePoint {
    public static final String TAG = "MengineAdAppOpenPoint";

    protected int m_id;
    protected boolean m_enabled;

    protected long m_timeSleep;
    protected long m_installTimeOffset;

    MengineAdAppOpenPoint(@NonNull String name, @NonNull JSONObject values) {
        super(name);

        m_id = this.parseAdPointInteger(values, "id", false, 1);
        m_enabled = this.parseAdPointBoolean(values, "enable", true, false);
        m_timeSleep = this.parseAdPointTime(values, "time_sleep", false, -1);
        m_installTimeOffset = this.parseAdPointTime(values, "trigger_install_time_offset", false, 600);
    }

    public boolean canYouShowAd(@NonNull MengineApplication application, long timeStop) {
        if (m_enabled == false) {
            return false;
        }

        if (m_installTimeOffset > 0) {
            long currentTimestamp = MengineUtils.getTimestamp();
            long installTimestamp = application.getInstallTimestamp();

            if (currentTimestamp - installTimestamp < m_installTimeOffset) {
                return false;
            }
        }

        if (m_timeSleep >= 0) {
            if (m_timeSleep > timeStop) {
                return false;
            }
        }

        m_attempts.attempt();

        return true;
    }

    public void showAd() {
        //ToDo: Implement this method
    }
}