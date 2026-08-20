package org.Mengine.Plugin.GoogleConsent;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineService;
import org.Mengine.Base.MengineListenerActivity;
import org.Mengine.Base.MengineServiceInvalidInitializeException;
import org.Mengine.Base.MengineParamTransparencyConsent;
import org.Mengine.Base.MengineConsentFlowUserGeography;
import org.Mengine.Base.MengineTransparencyConsentProviderInterface;

import android.os.Bundle;

import androidx.annotation.NonNull;

import com.google.android.ump.ConsentDebugSettings;
import com.google.android.ump.ConsentInformation;
import com.google.android.ump.ConsentRequestParameters;
import com.google.android.ump.UserMessagingPlatform;

public class MengineGoogleConsentPlugin extends MengineService implements MengineListenerActivity, MengineTransparencyConsentProviderInterface {
    public static final String SERVICE_NAME = "GConsent";
    public static final boolean SERVICE_EMBEDDING = true;

    @Override
    public boolean isTransparencyConsentProvider() {
        return true;
    }

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

                this.loadForm(application, activity, consentInformation);
            }
            , (formError) -> {
                this.logError("consent info update failure: %s [%d]"
                    , formError.getMessage()
                    , formError.getErrorCode()
                );

                this.notifyTransparencyConsent(application, consentInformation);
            });
    }

    private void notifyTransparencyConsent(@NonNull MengineApplication application, @NonNull ConsentInformation consentInformation) {
        boolean canRequestAds = consentInformation.canRequestAds();

        this.logInfo("Google Consent canRequestAds: %b", canRequestAds);

        if (canRequestAds == false) {
            return;
        }

        MengineParamTransparencyConsent tcParam = application.makeTransparencyConsentParam();

        if (tcParam.TRANSPARENCYCONSENT_GDPRAPPLIES == 1) {
            MengineParamTransparencyConsent.setConsentFlowUserGeography(application, MengineConsentFlowUserGeography.MengineConsentFlowUserGeography_EEA);
        } else {
            MengineParamTransparencyConsent.setConsentFlowUserGeography(application, MengineConsentFlowUserGeography.MengineConsentFlowUserGeography_NonEEA);
        }

        application.checkTransparencyConsentServices();
    }

    public void loadForm(@NonNull MengineApplication application, @NonNull MengineActivity activity, @NonNull ConsentInformation consentInformation) {
        UserMessagingPlatform.loadAndShowConsentFormIfRequired(activity
            , (loadAndShowError) -> {
                if (loadAndShowError != null) {
                    this.logError("consent form load and show failure: %s [%d]"
                        , loadAndShowError.getMessage()
                        , loadAndShowError.getErrorCode()
                    );
                } else {
                    this.logInfo("consent form load and show success");
                }

                this.notifyTransparencyConsent(application, consentInformation);
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

            MengineApplication application = this.getMengineApplication();
            ConsentInformation consentInformation = UserMessagingPlatform.getConsentInformation(activity);

            this.notifyTransparencyConsent(application, consentInformation);

            this.nativeCall("onAndroidGoogleConsentFlowCompleted");
        });
    }

    public boolean isConsentFlowUserGeographyGDPR() {
        MengineApplication application = this.getMengineApplication();
        MengineParamTransparencyConsent tcParam = application.makeTransparencyConsentParam();

        return tcParam.isEEA();
    }

    public boolean isPrivacyOptionsRequired() {
        MengineActivity activity = this.getMengineActivity();

        if (activity == null) {
            return false;
        }

        ConsentInformation consentInformation = UserMessagingPlatform.getConsentInformation(activity);
        ConsentInformation.PrivacyOptionsRequirementStatus privacyOptionsRequirementStatus = consentInformation.getPrivacyOptionsRequirementStatus();

        return privacyOptionsRequirementStatus == ConsentInformation.PrivacyOptionsRequirementStatus.REQUIRED;
    }
}
