package org.Mengine.Plugin.GoogleConsent;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginActivityListener;
import org.Mengine.Base.MenginePluginInvalidInitializeException;
import org.Mengine.Base.MengineTransparencyConsentParam;

import android.content.Context;
import android.os.Bundle;

import com.google.android.ump.ConsentDebugSettings;
import com.google.android.ump.ConsentInformation;
import com.google.android.ump.ConsentRequestParameters;
import com.google.android.ump.UserMessagingPlatform;

public class MengineGoogleConsentPlugin extends MenginePlugin implements MenginePluginActivityListener {
    public static final String PLUGIN_NAME = "MengineGoogleConsent";

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) throws MenginePluginInvalidInitializeException {
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

                MengineGoogleConsentPlugin.this.logMessage("updated consent info update successful status: %d formAvailable: %b privacyOptionsRequirementStatus: %s"
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

                MengineGoogleConsentPlugin.this.loadForm(activity);
            }
            , (formError) -> {
                MengineGoogleConsentPlugin.this.logError("consent info update failure: %s [%d]"
                    , formError.getMessage()
                    , formError.getErrorCode()
                );
            });
    }

    public void loadForm(MengineActivity activity) {
        MengineTransparencyConsentParam beforeTcParam = activity.makeTransparencyConsentParam();

        UserMessagingPlatform.loadAndShowConsentFormIfRequired(activity
            , (loadAndShowError) -> {
                if (loadAndShowError != null) {
                    MengineGoogleConsentPlugin.this.logError("consent form load and show failure: %s [%d]"
                        , loadAndShowError.getMessage()
                        , loadAndShowError.getErrorCode()
                    );

                    return;
                }

                MengineGoogleConsentPlugin.this.logMessage("consent form load and show successful");

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