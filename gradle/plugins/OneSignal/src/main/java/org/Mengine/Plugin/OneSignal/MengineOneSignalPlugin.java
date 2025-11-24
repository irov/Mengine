package org.Mengine.Plugin.OneSignal;

import androidx.annotation.NonNull;
import androidx.annotation.StringRes;

import com.onesignal.Continue;
import com.onesignal.OneSignal;
import com.onesignal.debug.LogLevel;

import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineListenerApplication;
import org.Mengine.Base.MengineListenerTransparencyConsent;
import org.Mengine.Base.MengineService;
import org.Mengine.Base.MengineServiceInvalidInitializeException;
import org.Mengine.Base.MengineParamTransparencyConsent;
import org.Mengine.Base.MengineUtils;

import java.util.List;

public class MengineOneSignalPlugin extends MengineService implements MengineListenerApplication, MengineListenerTransparencyConsent {
    public static final String SERVICE_NAME = "OneSignal";

    public static final @StringRes int METADATA_APP_ID = R.string.mengine_onesignal_app_id;

    @Override
    public void onAppCreate(@NonNull MengineApplication application) throws MengineServiceInvalidInitializeException {
        String MengineOneSignalPlugin_AppId = this.getResourceString(METADATA_APP_ID);

        this.logInfo("%s: %s"
            , this.getResourceName(METADATA_APP_ID)
            , MengineUtils.getRedactedValue(MengineOneSignalPlugin_AppId)
        );

        if (BuildConfig.DEBUG == true) {
            OneSignal.getDebug().setLogLevel(LogLevel.WARN);
        } else {
            OneSignal.getDebug().setLogLevel(LogLevel.ERROR);
        }

        MengineParamTransparencyConsent tcParam = application.makeTransparencyConsentParam();

        if (tcParam.isEEA() == true) {
            OneSignal.setConsentRequired(true);

            if (tcParam.getPurposeConsentArguments(List.of(0, 3, 4)) == true) {
                OneSignal.setConsentGiven(true);
            }
        }

        OneSignal.initWithContext(application, MengineOneSignalPlugin_AppId);

        String userId = application.getUserId();
        OneSignal.login(userId);
    }

    @Override
    public void onMengineTransparencyConsent(@NonNull MengineApplication application, @NonNull MengineParamTransparencyConsent consent) {
        if (consent.isEEA() == true) {
            if (consent.getPurposeConsentArguments(List.of(0, 3, 4)) == true) {
                OneSignal.setConsentGiven(true);
            }
        }

        OneSignal.getNotifications().requestPermission(false, Continue.none());
    }
}
