package org.Mengine.Base;

import android.content.SharedPreferences;

import androidx.annotation.Size;

import java.util.Locale;
import java.util.Timer;
import java.util.TimerTask;

public class MenginePlugin implements MenginePluginInterface {
    private MengineApplication m_application;
    private MengineActivity m_activity;
    private String m_pluginName;

    public MengineApplication getMengineApplication() {
        return m_application;
    }

    public MengineActivity getMengineActivity() {
        return m_activity;
    }

    public boolean onInitialize(MengineApplication application, String pluginName) {
        m_application = application;
        m_pluginName = pluginName;

        return true;
    }

    public void onFinalize(MengineApplication application) {
        m_application = null;
    }

    public void setActivity(MengineActivity activity) {
        m_activity = activity;
    }

    public String getPluginName() {
        return m_pluginName;
    }

    public <T> T newInstance(String name, boolean exist) {
        T instance = MengineUtils.newInstance(m_pluginName, name, exist);

        return instance;
    }

    public boolean hasOption(String option) {
        boolean value = m_application.hasOption(option);

        return value;
    }

    public SharedPreferences getPrivateSharedPreferences() {
        SharedPreferences settings = m_application.getPrivateSharedPreferences(m_pluginName);

        return settings;
    }

    public void setPrivateSettingString(String name, String value) {
        SharedPreferences settings = this.getPrivateSharedPreferences();

        SharedPreferences.Editor editor = settings.edit();
        editor.putString(name, value);
        editor.apply();
    }

    public String getPrivateSettingString(String name, String defaultValue) {
        SharedPreferences settings = this.getPrivateSharedPreferences();

        String value = settings.getString(name, defaultValue);

        return value;
    }

    protected void invalidInitialize(String format, Object ... args) throws MenginePluginInvalidInitializeException {
        String message = MengineLog.buildTotalMsg(format, args);

        throw new MenginePluginInvalidInitializeException(message);
    }

    public void makeToastLong(String text, long delayed) {
        if (m_activity == null) {
            return;
        }

        MengineUtils.makeToastDelayed(m_activity, text, delayed);
    }

    public void sendEvent(MengineEvent event, Object ... args) {
        MengineApplication application = this.getMengineApplication();

        application.sendEvent(event, args);
    }

    public void runOnUiThread(Runnable action) {
        if (m_activity == null) {
            return;
        }

        m_activity.runOnUiThread(action);
    }

    public Timer scheduleAtFixedRate(long period, Runnable runnable) {
        Timer timer = new Timer();

        TimerTask task = new TimerTask() {
            @Override
            public void run() {
                MenginePlugin.this.runOnUiThread(runnable);
            }
        };

        timer.scheduleAtFixedRate(task, 0, period);

        return timer;
    }

    public void setState(String name, Object value) {
        m_application.setState(name, value);
    }

    public String logInfo(String format, Object ... args) {
        String m = MengineLog.logInfo(m_pluginName, format, args);

        return m;
    }

    public String logMessage(String format, Object ... args) {
        String m = MengineLog.logMessage(m_pluginName, format, args);

        return m;
    }

    public String logWarning(String format, Object ... args) {
        String m = MengineLog.logWarning(m_pluginName, format, args);

        return m;
    }

    public String logError(String format, Object ... args) {
        String m = MengineLog.logError(m_pluginName, format, args);

        return m;
    }

    public void assertionError(String format, Object ... args) {
        String msg = String.format(Locale.US, format, args);

        throw new AssertionError("[" + m_pluginName + "] " + msg);
    }

    public MengineAnalyticsEventBuilder buildEvent(@Size(min = 1L,max = 40L) String name) {
        MengineAnalyticsEventBuilder eventBuilder = MengineAnalytics.buildEvent(name);

        return eventBuilder;
    }

    public void pythonCall(String method, Object ... args) {
        if (m_activity == null) {
            return;
        }

        m_activity.pythonCall(m_pluginName, method, args);
    }

    public void activateSemaphore(String name) {
        if (m_activity == null) {
            return;
        }

        m_activity.activateSemaphore(name);
    }

    public void deactivateSemaphore(String name) {
        if (m_activity == null) {
            return;
        }

        m_activity.deactivateSemaphore(name);
    }

    public void waitSemaphore(String name, MengineFunctorVoid cb) {
        if (m_activity == null) {
            return;
        }

        m_activity.waitSemaphore(name, cb);
    }

    public boolean getMetaDataBoolean(String name) throws MenginePluginInvalidInitializeException {
        try {
            boolean value = m_application.getMetaDataBoolean(name);

            return value;
        } catch (RuntimeException e) {
            this.invalidInitialize("invalid get meta data [%s]", name);
        }

        return false;
    }

    public int getMetaDataInteger(String name) throws MenginePluginInvalidInitializeException {
        try {
            int value = m_application.getMetaDataInteger(name);

            return value;
        } catch (RuntimeException e) {
            this.invalidInitialize("invalid get meta data [%s]", name);
        }

        return 0;
    }

    public String getMetaDataString(String name) throws MenginePluginInvalidInitializeException {
        try {
            String value = m_application.getMetaDataString(name);

            return value;
        } catch (RuntimeException e) {
            this.invalidInitialize("invalid get meta data [%s]", name);
        }

        return null;
    }

    public void onEvent(MengineApplication application, MengineEvent event, Object ... args) {
        //Empty
    }

    public void onState(MengineApplication application, String name, Object value) {
        //Empty
    }
}