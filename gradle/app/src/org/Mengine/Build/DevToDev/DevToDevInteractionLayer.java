package org.Mengine.Build.DevToDev;

import android.app.Activity;

import com.devtodev.core.DevToDev;
import com.devtodev.core.data.consts.AccrualType;

/**
 * Created by sweatcoin7 on 7/13/18.
 */

public class DevToDevInteractionLayer {
    private static final String APPID = "eb9820ae-f1aa-09f2-9295-5b1d931f7d64";
    private static final String SECRET = "YeSsjbXTg6GAM5lPBEOI3Jx2ZNh7qcDR";
    private static final String APIKEY = "ak-BVbZMln2aXWOLEuSD1tipYsr8kqN5dIA";

    public DevToDevInteractionLayer(Activity activity) {
        DevToDev.init(activity, APPID, SECRET);
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

    public void onSimpleCustomEvent(String eventName) {
        DevToDev.customEvent(eventName);
    }
}
