package org.Mengine.Build.DevToDev;

import android.app.Activity;

import com.devtodev.core.DevToDev;
import com.devtodev.core.data.consts.AccrualType;
import com.devtodev.core.data.metrics.aggregated.events.CustomEventParams;

import org.json.JSONException;
import org.json.JSONObject;

import java.util.Iterator;

/**
 * Created by sweatcoin7 on 7/13/18.
 */

public class DevToDevInteractionLayer {
    private final String _appId;
    private final String _secret;
    private final String _apiKey;

    public DevToDevInteractionLayer(Activity activity, String appId, String secret, String apiKey) {
        _appId = appId;
        _secret = secret;
        _apiKey = apiKey;

        DevToDev.init(activity, _appId, _secret);
    }

    public void onTutorialEvent(int stateOrStep) {
        DevToDev.tutorialCompleted(stateOrStep);
    }

    public void setCurrentLevel(int level) {
        DevToDev.setCurrentLevel(level);
    }

    public void onLevelUp(int level) {
        DevToDev.levelUp(level);
    }

    public void onCurrencyAccrual(String currencyName, int currencyAmount, int accrualType) {
        DevToDev.currencyAccrual(currencyName, currencyAmount, AccrualType.values()[accrualType]);
    }

    public void onRealPayment(String paymentId, float inAppPrice, String inAppName, String inAppCurrencyISOCode) {
        DevToDev.realPayment(paymentId, inAppPrice, inAppName, inAppCurrencyISOCode);
    }

    public void onInAppPurchase(String purchaseId, String purchaseType, int purchaseAmount, int purchasePrice, String purchaseCurrency) {
        DevToDev.inAppPurchase(purchaseId, purchaseType, purchaseAmount, purchasePrice, purchaseCurrency);
    }

    public void onSimpleCustomEvent(String eventName, String intJSON, String floatJSON, String stringJSON) {
        CustomEventParams customEventParams = new CustomEventParams();
        if (!intJSON.isEmpty()) {
            try {
                final JSONObject obj = new JSONObject(intJSON);
                final Iterator<String> iterator =  obj.keys();
                while(iterator.hasNext()){
                    final String key = iterator.next();
                    customEventParams.putInteger(key, obj.getInt(key));
                }
            } catch (JSONException e) {
                e.printStackTrace();
            }
        }
        if (!floatJSON.isEmpty()) {
            try {
                final JSONObject obj = new JSONObject(floatJSON);
                final Iterator<String> iterator =  obj.keys();
                while(iterator.hasNext()){
                    final String key = iterator.next();
                    customEventParams.putDouble(key, obj.getDouble(key));
                }
            } catch (JSONException e) {
                e.printStackTrace();
            }
        }
        if (!stringJSON.isEmpty()) {
            try {
                final JSONObject obj = new JSONObject(stringJSON);
                final Iterator<String> iterator =  obj.keys();
                while(iterator.hasNext()){
                    final String key = iterator.next();
                    customEventParams.putString(key, obj.getString(key));
                }
            } catch (JSONException e) {
                e.printStackTrace();
            }
        }
        DevToDev.customEvent(eventName, customEventParams);
    }
}
