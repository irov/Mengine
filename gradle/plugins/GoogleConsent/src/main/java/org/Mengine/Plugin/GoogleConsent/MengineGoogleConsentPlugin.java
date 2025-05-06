package org.Mengine.Plugin.GoogleConsent;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineService;
import org.Mengine.Base.MengineListenerActivity;
import org.Mengine.Base.MengineServiceInvalidInitializeException;
import org.Mengine.Base.MengineParamTransparencyConsent;

import android.os.Bundle;

import androidx.annotation.NonNull;

import com.google.android.ump.ConsentDebugSettings;
import com.google.android.ump.ConsentInformation;
import com.google.android.ump.ConsentRequestParameters;
import com.google.android.ump.UserMessagingPlatform;

public class MengineGoogleConsentPlugin extends MengineService implements MengineListenerActivity {
    public static final String SERVICE_NAME = "GConsent";

    @Override
    public void onCreate(@NonNull MengineActivity activity, Bundle savedInstanceState) throws MengineServiceInvalidInitializeException {
        ConsentRequestParameters.Builder builder = new ConsentRequestParameters.Builder();

        if (BuildConfig.DEBUG == true) {
            if (BuildConfig.MENGINE_APP_PLUGIN_GOOGLE_CONSENT_TEST_DEVICE_HASHED_ID != null) {
                ConsentDebugSettings debugSettings = new ConsentDebugSettings.Builder(activity)
                    .setDebugGeography(ConsentDebugSettings
                        .DebugGeography
                        .DEBUG_GEOGRAPHY_EEA)
                    .addTestDeviceHashedId(BuildConfig.MENGINE_APP_PLUGIN_GOOGLE_CONSENT_TEST_DEVICE_HASHED_ID)
                    .build();

                builder.setConsentDebugSettings(debugSettings);
            }
        }

        builder.setTagForUnderAgeOfConsent(false);

        ConsentRequestParameters params = builder.build();

        ConsentInformation consentInformation = UserMessagingPlatform.getConsentInformation(activity);

        consentInformation.requestConsentInfoUpdate(activity, params
            , () -> {
                boolean formAvailable = consentInformation.isConsentFormAvailable();
                int consentStatus = consentInformation.getConsentStatus();
                ConsentInformation.PrivacyOptionsRequirementStatus privacyOptionsRequirementStatus = consentInformation.getPrivacyOptionsRequirementStatus();

                this.logInfo("updated consent info update success status: %d formAvailable: %b privacyOptionsRequirementStatus: %s"
                    , consentStatus
                    , formAvailable
                    , privacyOptionsRequirementStatus
                );

                if (formAvailable == false) {
                    return;
                }

                if (privacyOptionsRequirementStatus == ConsentInformation.PrivacyOptionsRequirementStatus.NOT_REQUIRED) {
                    MengineApplication application = this.getMengineApplication();

                    application.checkTransparencyConsentServices();

                    return;
                }

                if (consentStatus != ConsentInformation.ConsentStatus.REQUIRED) {
                    return;
                }

                this.loadForm(activity);
            }
            , (formError) -> {
                this.logError("consent info update failure: %s [%d]"
                    , formError.getMessage()
                    , formError.getErrorCode()
                );
            });
    }

    public void loadForm(@NonNull MengineActivity activity) {
        UserMessagingPlatform.loadAndShowConsentFormIfRequired(activity
            , (loadAndShowError) -> {
                if (loadAndShowError != null) {
                    this.logError("consent form load and show failure: %s [%d]"
                        , loadAndShowError.getMessage()
                        , loadAndShowError.getErrorCode()
                    );

                    return;
                }

                this.logInfo("consent form load and show success");

                MengineApplication application = this.getMengineApplication();

                application.checkTransparencyConsentServices();
            });
    }

    public void resetConsentInformation() {
        if (MengineActivity.INSTANCE == null) {
            this.logWarning("reset consent information invalid activity");

            return;
        }

        ConsentInformation consentInformation = UserMessagingPlatform.getConsentInformation(MengineActivity.INSTANCE);
        consentInformation.reset();
    }
}