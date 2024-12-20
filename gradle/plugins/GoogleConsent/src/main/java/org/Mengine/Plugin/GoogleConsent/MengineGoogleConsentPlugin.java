package org.Mengine.Plugin.GoogleConsent;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineService;
import org.Mengine.Base.MengineListenerActivity;
import org.Mengine.Base.MengineServiceInvalidInitializeException;
import org.Mengine.Base.MengineTransparencyConsentParam;

import android.content.Context;
import android.os.Bundle;

import com.google.android.ump.ConsentDebugSettings;
import com.google.android.ump.ConsentInformation;
import com.google.android.ump.ConsentRequestParameters;
import com.google.android.ump.UserMessagingPlatform;

public class MengineGoogleConsentPlugin extends MengineService implements MengineListenerActivity {
    public static final String SERVICE_NAME = "MengineGConsent";

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) throws MengineServiceInvalidInitializeException {
        ConsentRequestParameters.Builder builder = new ConsentRequestParameters.Builder();

        if (BuildConfig.DEBUG == true) {
            ConsentDebugSettings debugSettings = new ConsentDebugSettings.Builder(activity)
                .setDebugGeography(ConsentDebugSettings
                    .DebugGeography
                    .DEBUG_GEOGRAPHY_EEA)
                .addTestDeviceHashedId("9203703DF2B7213315680FD6966D3CDB")
                .build();

            builder.setConsentDebugSettings(debugSettings);
        }

        builder.setTagForUnderAgeOfConsent(false);

        ConsentRequestParameters params = builder.build();

        ConsentInformation consentInformation = UserMessagingPlatform.getConsentInformation(activity);

        consentInformation.requestConsentInfoUpdate(activity, params
            , () -> {
                boolean formAvailable = consentInformation.isConsentFormAvailable();
                int consentStatus = consentInformation.getConsentStatus();
                ConsentInformation.PrivacyOptionsRequirementStatus privacyOptionsRequirementStatus = consentInformation.getPrivacyOptionsRequirementStatus();

                this.logMessage("updated consent info update successful status: %d formAvailable: %b privacyOptionsRequirementStatus: %s"
                    , consentStatus
                    , formAvailable
                    , privacyOptionsRequirementStatus
                );

                if (formAvailable == false) {
                    return;
                }

                if (privacyOptionsRequirementStatus == ConsentInformation.PrivacyOptionsRequirementStatus.NOT_REQUIRED) {
                    MengineTransparencyConsentParam tcParam = activity.makeTransparencyConsentParam();

                    activity.onMengineTransparencyConsent(tcParam);

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

    public void loadForm(MengineActivity activity) {
        UserMessagingPlatform.loadAndShowConsentFormIfRequired(activity
            , (loadAndShowError) -> {
                if (loadAndShowError != null) {
                    this.logError("consent form load and show failure: %s [%d]"
                        , loadAndShowError.getMessage()
                        , loadAndShowError.getErrorCode()
                    );

                    return;
                }

                this.logMessage("consent form load and show successful");

                MengineTransparencyConsentParam consent = activity.makeTransparencyConsentParam();

                activity.onMengineTransparencyConsent(consent);
            });
    }

    public void resetConsentInformation() {
        MengineActivity activity = this.getMengineActivity();

        ConsentInformation consentInformation = UserMessagingPlatform.getConsentInformation(activity);
        consentInformation.reset();
    }
}