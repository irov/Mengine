package org.Mengine.Plugin.GoogleConsent;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MenginePlugin;

import android.content.Context;
import android.os.Bundle;

import com.google.android.ump.ConsentDebugSettings;
import com.google.android.ump.ConsentForm;
import com.google.android.ump.ConsentInformation;
import com.google.android.ump.ConsentRequestParameters;
import com.google.android.ump.FormError;
import com.google.android.ump.UserMessagingPlatform;

public class MengineGoogleConsentPlugin extends MenginePlugin {
    public static String PLUGIN_NAME = "GoogleConsent";

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) {
        final Context context = activity.getBaseContext();

        ConsentRequestParameters.Builder builder = new ConsentRequestParameters.Builder();

        if (BuildConfig.DEBUG == true) {
            ConsentDebugSettings debugSettings = new ConsentDebugSettings.Builder(context)
                .setDebugGeography(ConsentDebugSettings
                    .DebugGeography
                    .DEBUG_GEOGRAPHY_EEA)
                .addTestDeviceHashedId("5F4058CA8BD59AB743821B316CB2F63C")
                .build();

            builder.setConsentDebugSettings(debugSettings);
        }

        builder.setTagForUnderAgeOfConsent(false);

        ConsentRequestParameters params = builder.build();

        ConsentInformation consentInformation = UserMessagingPlatform.getConsentInformation(context);
        consentInformation.requestConsentInfoUpdate(activity, params
            , new ConsentInformation.OnConsentInfoUpdateSuccessListener() {
                @Override
                public void onConsentInfoUpdateSuccess() {
                    int consentStatus = consentInformation.getConsentStatus();
                    boolean formAvailable = consentInformation.isConsentFormAvailable();

                    MengineGoogleConsentPlugin.this.logInfo("updated consent info update successful status: %d formAvailable: %b"
                        , consentStatus
                        , formAvailable
                    );

                    if (formAvailable == true && consentStatus == ConsentInformation.ConsentStatus.REQUIRED) {
                        MengineGoogleConsentPlugin.this.loadForm(activity, context);
                    }
                }
            }
            , new ConsentInformation.OnConsentInfoUpdateFailureListener() {
                @Override
                public void onConsentInfoUpdateFailure (FormError formError) {
                    MengineGoogleConsentPlugin.this.logError("consent info update failure: %s [%d]"
                        , formError.getMessage()
                        , formError.getErrorCode()
                    );
                }
            });
    }

    void loadForm(MengineActivity activity, Context context) {
        UserMessagingPlatform.loadConsentForm(context
                , new UserMessagingPlatform.OnConsentFormLoadSuccessListener() {
                    @Override
                    public void onConsentFormLoadSuccess(ConsentForm consentForm) {
                        consentForm.show(activity
                            , new ConsentForm.OnConsentFormDismissedListener() {
                                @Override
                                public void onConsentFormDismissed(FormError formError) {
                                    MengineGoogleConsentPlugin.this.logError("consent form dismissed: %s [%d]"
                                            , formError.getMessage()
                                            , formError.getErrorCode()
                                    );

                                    MengineGoogleConsentPlugin.this.loadForm(activity, context);
                                }
                            });
                    }
                }
                , new UserMessagingPlatform.OnConsentFormLoadFailureListener() {
                    @Override
                    public void onConsentFormLoadFailure(FormError formError) {
                        MengineGoogleConsentPlugin.this.logError("consent form load failure: %s [%d]"
                                , formError.getMessage()
                                , formError.getErrorCode()
                        );
                    }
                });
    }

    public void resetConsentInformation() {
        MengineActivity activity = this.getActivity();

        ConsentInformation consentInformation = UserMessagingPlatform.getConsentInformation(activity);
        consentInformation.reset();
    }
}
