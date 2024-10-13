package org.Mengine.Plugin.DevToDev;

import android.content.Context;
import android.os.Bundle;
import androidx.annotation.NonNull;

import com.devtodev.analytics.external.DTDLogLevel;
import com.devtodev.analytics.external.analytics.DTDAnalytics;
import com.devtodev.analytics.external.analytics.DTDAnalyticsConfiguration;
import com.devtodev.analytics.external.analytics.DTDCustomEventParameters;

import org.Mengine.Base.MengineAdRevenueParam;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineEvent;
import org.Mengine.Base.MenginePluginAdRevenueListener;
import org.Mengine.Base.MenginePluginAnalyticsListener;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginApplicationListener;
import org.Mengine.Base.MenginePluginInvalidInitializeException;

import java.util.Map;

public class MengineDevToDevPlugin extends MenginePlugin implements MenginePluginAnalyticsListener, MenginePluginAdRevenueListener, MenginePluginApplicationListener, MenginePluginSessionIdListener {
    public static final String PLUGIN_NAME = "MengineDevToDev";
    public static final boolean PLUGIN_EMBEDDING = true;

    public static final String PLUGIN_METADATA_APP_ID = "mengine.devtodev.app_id";

    private boolean m_initializeSuccessful;

    @Override
    public void onAppPrepare(MengineApplication application) throws MenginePluginInvalidInitializeException {
        m_initializeSuccessful = false;

        String MengineDevToDevPlugin_AppId = this.getMetaDataString(PLUGIN_METADATA_APP_ID);

        this.logInfo("%s: %s"
            , PLUGIN_METADATA_APP_ID
            , MengineDevToDevPlugin_AppId
        );

        String sessionId = application.getSessionId();
        DTDAnalytics.INSTANCE.setUserId(sessionId);

        DTDAnalytics.INSTANCE.setInitializationCompleteCallback(() -> {
            MengineDevToDevPlugin.this.logMessage("Initialized DevToDev has been finished");

            m_initializeSuccessful = true;

            return null;
        });

        DTDAnalytics.INSTANCE.coppaControlEnable();

        DTDAnalyticsConfiguration configuration = new DTDAnalyticsConfiguration();

        if (BuildConfig.DEBUG == true) {
            configuration.setLogLevel(DTDLogLevel.Debug);
        } else {
            configuration.setLogLevel(DTDLogLevel.Error);
        }

        Context context = application.getApplicationContext();

        DTDAnalytics.INSTANCE.initialize(MengineDevToDevPlugin_AppId, configuration, context);
    }

    @Override
    void onMengineSessionId(MengineApplication application, String sessionId) {
        if (m_initializeSuccessful == false) {
            return;
        }

        DTDAnalytics.INSTANCE.setUserId(sessionId);
    }

    @Override
    public void onMengineAnalyticsEvent(MengineApplication application, String eventName, long timestamp, Map<String, Object> bases, Map<String, Object> parameters) {
        if (m_initializeSuccessful == false) {
            return;
        }

        DTDCustomEventParameters params = new DTDCustomEventParameters();

        for (Map.Entry<String, Object> entry : bases.entrySet()) {
            String name = entry.getKey();
            Object parameter = entry.getValue();

            if (parameter instanceof Boolean) {
                params.add(name, (Boolean)parameter);
            } else if (parameter instanceof Long) {
                params.add(name, (Long)parameter);
            } else if (parameter instanceof Double) {
                params.add(name, (Double)parameter);
            } else if (parameter instanceof String) {
                params.add(name, (String)parameter);
            } else {
                this.logError("[ERROR] customEvent unsupported parameter: %s class: %s"
                    , parameter
                    , parameter.getClass()
                );

                return;
            }
        }

        for (Map.Entry<String, Object> entry : parameters.entrySet()) {
            String name = entry.getKey();
            Object parameter = entry.getValue();

            if (parameter instanceof Boolean) {
                params.add(name, (Boolean)parameter);
            } else if (parameter instanceof Long) {
                params.add(name, (Long)parameter);
            } else if (parameter instanceof Double) {
                params.add(name, (Double)parameter);
            } else if (parameter instanceof String) {
                params.add(name, (String)parameter);
            } else {
                this.logError("[ERROR] customEvent unsupported parameter: %s class: %s"
                    , parameter
                    , parameter.getClass()
                );

                return;
            }
        }

        DTDAnalytics.INSTANCE.customEvent(eventName, params);
    }

    @Override
    public void onMengineAnalyticsFlush(MengineApplication application) {
        if (m_initializeSuccessful == false) {
            return;
        }

        DTDAnalytics.INSTANCE.sendBufferedEvents();
    }

    @Override
    public void onMengineAdRevenue(MengineApplication application, MengineAdRevenueParam revenue) {
        if (m_initializeSuccessful == false) {
            return;
        }

        String network = revenue.ADREVENUE_NETWORK;
        String adUnitId = revenue.ADREVENUE_ADUNITID;
        String placement = revenue.ADREVENUE_PLACEMENT;
        double revenueValue = revenue.ADREVENUE_REVENUE_VALUE;

        DTDAnalytics.INSTANCE.adImpression(network, revenueValue, placement, adUnitId);
    }

    public void logEvent(@NonNull String eventName, @NonNull DTDCustomEventParameters params) {
        if (m_initializeSuccessful == false) {
            return;
        }

        DTDAnalytics.INSTANCE.customEvent(eventName, params);
    }
}
