package org.Mengine.Plugin.DevToDev;

import android.os.Bundle;
import androidx.annotation.NonNull;

import com.devtodev.analytics.external.DTDLogLevel;
import com.devtodev.analytics.external.analytics.DTDAccrualType;
import com.devtodev.analytics.external.analytics.DTDAnalytics;
import com.devtodev.analytics.external.analytics.DTDAnalyticsConfiguration;
import com.devtodev.analytics.external.analytics.DTDCustomEventParameters;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MenginePluginAnalyticsListener;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginInvalidInitializeException;

import java.util.Map;

public class MengineDevToDevPlugin extends MenginePlugin implements MenginePluginAnalyticsListener {
    public static final String PLUGIN_NAME = "DevToDev";
    public static final boolean PLUGIN_EMBEDDING = true;

    public static final String PLUGIN_METADATA_APP_ID = "mengine.devtodev.app_id";

    private boolean m_initializeSuccessful;

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) throws MenginePluginInvalidInitializeException {
        m_initializeSuccessful = false;

        String MengineDevToDevPlugin_AppId = activity.getMetaDataString(PLUGIN_METADATA_APP_ID);

        if (MengineDevToDevPlugin_AppId == null) {
            this.invalidInitialize("initialize unsetup [%s]", PLUGIN_METADATA_APP_ID);

            return;
        }

        this.logInfo("%s: %s"
            , PLUGIN_METADATA_APP_ID
            , MengineDevToDevPlugin_AppId
        );

        DTDAnalytics.INSTANCE.setInitializationCompleteCallback(() -> {
            MengineDevToDevPlugin.this.logMessage("Initialized DevToDev has been finished");

            m_initializeSuccessful = true;

            return null;
        });

        DTDAnalytics.INSTANCE.coppaControlEnable();

        DTDAnalyticsConfiguration configuration = new DTDAnalyticsConfiguration();

        if (BuildConfig.DEBUG) {
            configuration.setLogLevel(DTDLogLevel.Debug);
        } else {
            configuration.setLogLevel(DTDLogLevel.Error);
        }

        DTDAnalytics.INSTANCE.initialize(MengineDevToDevPlugin_AppId, configuration, activity);
    }

    @Override
    public void onDestroy(MengineActivity activity) {
        //Empty
    }

    public void tutorialEvent(int stateOrStep) {
        if (m_initializeSuccessful == false) {
            return;
        }

        this.logMessage("tutorialEvent stateOrStep: %d"
            , stateOrStep
        );

        DTDAnalytics.INSTANCE.tutorial(stateOrStep);
    }

    public void setCurrentLevelEvent(int level) {
        if (m_initializeSuccessful == false) {
            return;
        }

        this.logMessage("setCurrentLevelEvent level: %d"
            , level
        );

        DTDAnalytics.INSTANCE.setCurrentLevel(level);
    }

    public void levelUpEvent(int level) {
        if (m_initializeSuccessful == false) {
            return;
        }

        this.logMessage("levelUpEvent level: %d"
            , level
        );

        DTDAnalytics.INSTANCE.levelUp(level);
    }

    public void currencyAccrualEvent(String currencyName, int currencyAmount, String sourceName, int accrualType) {
        if (m_initializeSuccessful == false) {
            return;
        }

        this.logMessage("levelUpEvent currencyName: %s currencyAmount: %d sourceName: %s accrualType: %d"
            , currencyName
            , currencyAmount
            , sourceName
            , accrualType
        );

        DTDAnalytics.INSTANCE.currencyAccrual(currencyName, currencyAmount, sourceName, DTDAccrualType.values()[accrualType]);
    }

    public void realCurrencyPaymentEvent(String paymentId, float inAppPrice, String inAppName, String inAppCurrencyISOCode) {
        if (m_initializeSuccessful == false) {
            return;
        }

        this.logMessage("realCurrencyPaymentEvent paymentId: %s inAppPrice: %f inAppName: %s inAppCurrencyISOCode: %s"
            , paymentId
            , inAppPrice
            , inAppName
            , inAppCurrencyISOCode
        );

        DTDAnalytics.INSTANCE.realCurrencyPayment(paymentId, inAppPrice, inAppName, inAppCurrencyISOCode);
    }

    public void virtualCurrencyPaymentEvent(String purchaseId, String purchaseType, int purchaseAmount, int purchasePrice, String purchaseCurrency) {
        if (m_initializeSuccessful == false) {
            return;
        }

        this.logMessage("virtualCurrencyPaymentEvent purchaseId: %s purchaseType: %s purchaseAmount: %d purchasePrice: %d purchaseCurrency: %s"
            , purchaseId
            , purchaseType
            , purchaseAmount
            , purchasePrice
            , purchaseCurrency
        );

        DTDAnalytics.INSTANCE.virtualCurrencyPayment(purchaseId, purchaseType, purchaseAmount, purchasePrice, purchaseCurrency);
    }

    @Override
    public void onMengineAnalyticsEvent(MengineApplication application, int eventType, String eventName, long timestamp, Map<String, Object> context, Map<String, Object> parameters) {
        if (m_initializeSuccessful == false) {
            return;
        }

        switch (eventType) {
            case  EAET_CUSTOM: {
                DTDCustomEventParameters params = new DTDCustomEventParameters();

                for (Map.Entry<String, Object> entry : context.entrySet()) {
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
                        this.logError("customEvent unsupported parameter: %s class: %s"
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
                        this.logError("customEvent unsupported parameter: %s class: %s"
                            , parameter
                            , parameter.getClass()
                        );

                        return;
                    }
                }

                this.logInfo("logEvent [CUSTOM] eventName: %s params: %s"
                    , eventName
                    , params
                );

                DTDAnalytics.INSTANCE.customEvent(eventName, params);
            }break;
        }
    }

    public void logEvent(@NonNull String eventName, @NonNull DTDCustomEventParameters params) {
        if (m_initializeSuccessful == false) {
            return;
        }

        this.logInfo("logEvent [CUSTOM] eventName: %s params: %s"
            , eventName
            , params
        );

        DTDAnalytics.INSTANCE.customEvent(eventName, params);
    }
}
