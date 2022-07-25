package org.Mengine.Plugin.DevToDev;

import android.os.Bundle;

import com.devtodev.analytics.external.DTDLogLevel;
import com.devtodev.analytics.external.analytics.DTDAccrualType;
import com.devtodev.analytics.external.analytics.DTDAnalytics;
import com.devtodev.analytics.external.analytics.DTDAnalyticsConfiguration;
import com.devtodev.analytics.external.analytics.DTDCustomEventParameters;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MenginePlugin;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.Iterator;

public class MengineDevToDevPlugin extends MenginePlugin {
    @Override
    public void onPythonEmbedding(MengineActivity activity) {
        this.addPythonPlugin("DevToDev");
    }

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) {
        DTDAnalyticsConfiguration configuration = new DTDAnalyticsConfiguration();

        if (BuildConfig.DEBUG) {
            configuration.setLogLevel(DTDLogLevel.Debug);
        } else {
            configuration.setLogLevel(DTDLogLevel.Error);
        }

        String appKey = activity.getString(R.string.dev_to_dev_app_id);

        DTDAnalytics.INSTANCE.initialize(appKey, configuration, activity);
    }

    public void onTutorialEvent(int stateOrStep) {
        DTDAnalytics.INSTANCE.tutorial(stateOrStep);
    }

    public void setCurrentLevel(int level) {
        DTDAnalytics.INSTANCE.setCurrentLevel(level);
    }

    public void onLevelUp(int level) {
        DTDAnalytics.INSTANCE.levelUp(level);
    }

    public void onCurrencyAccrual(String currencyName, int currencyAmount, String sourceName, int accrualType) {
        DTDAnalytics.INSTANCE.currencyAccrual(currencyName, currencyAmount, sourceName, DTDAccrualType.values()[accrualType]);
    }

    public void onRealPayment(String paymentId, float inAppPrice, String inAppName, String inAppCurrencyISOCode) {
        DTDAnalytics.INSTANCE.realCurrencyPayment(paymentId, inAppPrice, inAppName, inAppCurrencyISOCode);
    }

    public void onInAppPurchase(String purchaseId, String purchaseType, int purchaseAmount, int purchasePrice, String purchaseCurrency) {
        DTDAnalytics.INSTANCE.virtualCurrencyPayment(purchaseId, purchaseType, purchaseAmount, purchasePrice, purchaseCurrency);
    }

    public void onSimpleCustomEvent(String eventName, String intJSON, String floatJSON, String stringJSON) {
        DTDCustomEventParameters customEventParams = new DTDCustomEventParameters();

        if (!intJSON.isEmpty()) {
            try {
                final JSONObject obj = new JSONObject(intJSON);
                final Iterator<String> iterator = obj.keys();
                while (iterator.hasNext()) {
                    final String key = iterator.next();
                    customEventParams.add(key, obj.getLong(key));
                }
            } catch (JSONException e) {
                e.printStackTrace();
            }
        }

        if (!floatJSON.isEmpty()) {
            try {
                final JSONObject obj = new JSONObject(floatJSON);
                final Iterator<String> iterator = obj.keys();
                while (iterator.hasNext()) {
                    final String key = iterator.next();
                    customEventParams.add(key, obj.getDouble(key));
                }
            } catch (JSONException e) {
                e.printStackTrace();
            }
        }

        if (!stringJSON.isEmpty()) {
            try {
                final JSONObject obj = new JSONObject(stringJSON);
                final Iterator<String> iterator = obj.keys();
                while (iterator.hasNext()) {
                    final String key = iterator.next();
                    customEventParams.add(key, obj.getString(key));
                }
            } catch (JSONException e) {
                e.printStackTrace();
            }
        }

        DTDAnalytics.INSTANCE.customEvent(eventName, customEventParams);
    }
}
