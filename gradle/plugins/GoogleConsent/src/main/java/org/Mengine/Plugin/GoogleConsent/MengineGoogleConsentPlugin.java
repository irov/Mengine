package org.Mengine.Plugin.GoogleConsent;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineService;
import org.Mengine.Base.MengineListenerActivity;
import org.Mengine.Base.MengineServiceInvalidInitializeException;
import org.Mengine.Base.MengineParamTransparencyConsent;
import org.Mengine.Base.MengineConsentFlowUserGeography;

import android.os.Bundle;

import androidx.annotation.NonNull;

import com.google.android.ump.ConsentDebugSettings;
import com.google.android.ump.ConsentInformation;
import com.google.android.ump.ConsentRequestParameters;
import com.google.android.ump.UserMessagingPlatform;

public class MengineGoogleConsentPlugin extends MengineService implements MengineListenerActivity {
    public static final String SERVICE_NAME = "GConsent";
    public static final boolean SERVICE_EMBEDDING = true;

    @Override
    public void onCreate(@NonNull MengineActivity activity, Bundle savedInstanceState) throws MengineServiceInvalidInitializeException {
        ConsentRequestParameters.Builder builder = new ConsentRequestParameters.Builder();

        if (BuildConfig.DEBUG == true) {
            if (BuildConfig.MENGINE_APP_PLUGIN_GOOGLE_CONSENT_TEST_DEVICE_HASHED_ID.isEmpty() == false) {
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

        MengineApplication application = this.getMengineApplication();

        this.logInfo("Google Consent requestConsentInfoUpdate started");

        consentInformation.requestConsentInfoUpdate(activity, params
            , () -> {
                boolean formAvailable = consentInformation.isConsentFormAvailable();
                int consentStatus = consentInformation.getConsentStatus();
                ConsentInformation.PrivacyOptionsRequirementStatus privacyOptionsRequirementStatus = consentInformation.getPrivacyOptionsRequirementStatus();

                this.logInfo("Google Consent requestConsentInfoUpdate success consentStatus: %d formAvailable: %b privacyOptionsRequirementStatus: %s"
                    , consentStatus
                    , formAvailable
                    , privacyOptionsRequirementStatus.toString()
                );

                if (formAvailable == false) {
                    application.checkTransparencyConsentServices();

                    return;
                }

                if (privacyOptionsRequirementStatus == ConsentInformation.PrivacyOptionsRequirementStatus.NOT_REQUIRED) {
                    application.checkTransparencyConsentServices();

                    return;
                }

                if (consentStatus != ConsentInformation.ConsentStatus.REQUIRED) {
                    application.checkTransparencyConsentServices();

                    return;
                }

                this.loadForm(application, activity);
            }
            , (formError) -> {
                this.logError("consent info update failure: %s [%d]"
                    , formError.getMessage()
                    , formError.getErrorCode()
                );

                application.checkTransparencyConsentServices();
            });
    }

    public void loadForm(@NonNull MengineApplication application, @NonNull MengineActivity activity) {
        UserMessagingPlatform.loadAndShowConsentFormIfRequired(activity
            , (loadAndShowError) -> {
                if (loadAndShowError != null) {
                    this.logError("consent form load and show failure: %s [%d]"
                        , loadAndShowError.getMessage()
                        , loadAndShowError.getErrorCode()
                    );

                    application.checkTransparencyConsentServices();

                    return;
                }

                this.logInfo("consent form load and show success");

                application.checkTransparencyConsentServices();
            });
    }

    public void resetConsentInformation() {
        MengineActivity activity = this.getMengineActivity();

        if (activity == null) {
            this.logWarning("reset consent information invalid activity");

            return;
        }

        ConsentInformation consentInformation = UserMessagingPlatform.getConsentInformation(activity);
        consentInformation.reset();
    }

    public void showConsentFlow() {
        MengineActivity activity = this.getMengineActivity();

        if (activity == null) {
            this.logError("[ERROR] showConsentFlow invalid activity");

            this.nativeCall("onAndroidGoogleConsentFlowError", new RuntimeException("invalid activity"));

            return;
        }

        UserMessagingPlatform.showPrivacyOptionsForm(activity, formError -> {
            if (formError != null) {
                this.logError("Failed to show consent dialog error: %s [%d]"
                    , formError.getMessage()
                    , formError.getErrorCode()
                );

                this.nativeCall("onAndroidGoogleConsentFlowError", new RuntimeException(formError.getMessage()));

                return;
            }

            this.logInfo("Consent dialog was shown");

            this.nativeCall("onAndroidGoogleConsentFlowCompleted");
        });
    }

    public boolean isConsentFlowUserGeographyGDPR() {
        MengineConsentFlowUserGeography geography = MengineParamTransparencyConsent.getTransparencyconsentUsergeography();

        if (geography != MengineConsentFlowUserGeography.MengineConsentFlowUserGeography_EEA) {
            return false;
        }

        return true;
    }
}