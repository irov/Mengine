package org.Mengine.Plugin.OneSignal;

import androidx.annotation.NonNull;

import com.onesignal.Continue;
import com.onesignal.OneSignal;
import com.onesignal.debug.LogLevel;

import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineListenerApplication;
import org.Mengine.Base.MengineListenerTransparencyConsent;
import org.Mengine.Base.MengineService;
import org.Mengine.Base.MengineServiceInvalidInitializeException;
import org.Mengine.Base.MengineTransparencyConsentParam;
import org.Mengine.Base.MengineUtils;

import java.util.List;

public class MengineOneSignalPlugin extends MengineService implements MengineListenerApplication, MengineListenerTransparencyConsent {
    public static final String SERVICE_NAME = "OneSignal";

    public static final String METADATA_APP_ID = "mengine.onesignal.app_id";

    @Override
    public void onAppCreate(@NonNull MengineApplication application) throws MengineServiceInvalidInitializeException {
        String MengineOneSignalPlugin_AppId = this.getMetaDataString(METADATA_APP_ID);

        this.logInfo("%s: %s"
            , METADATA_APP_ID
            , MengineUtils.getRedactedValue(MengineOneSignalPlugin_AppId)
        );

        if (BuildConfig.DEBUG == true) {
            OneSignal.getDebug().setLogLevel(LogLevel.WARN);
        } else {
            OneSignal.getDebug().setLogLevel(LogLevel.ERROR);
        }

        MengineTransparencyConsentParam tcParam = application.makeTransparencyConsentParam();

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
    public void onMengineTransparencyConsent(@NonNull MengineApplication application, MengineTransparencyConsentParam tcParam) {
        if (tcParam.getPurposeConsentArguments(List.of(0, 3, 4)) == true) {
            OneSignal.setConsentGiven(true);

            OneSignal.getNotifications().requestPermission(false, Continue.none());
        }
    }
}
