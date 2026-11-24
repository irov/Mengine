package org.Mengine.Plugin.GoogleConsent;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineOperation;
import org.Mengine.Base.MenginePlatformEventQueue;
import org.Mengine.Base.MengineService;
import org.Mengine.Base.MengineListenerActivity;
import org.Mengine.Base.MengineServiceInvalidInitializeException;
import org.Mengine.Base.MengineParamTransparencyConsent;
import org.Mengine.Base.MengineConsentFlowUserGeography;
import org.Mengine.Base.MengineTransparencyConsentProviderInterface;
import org.Mengine.Base.MengineUtils;

import android.os.Bundle;

import androidx.annotation.MainThread;
import androidx.annotation.NonNull;

import com.google.android.ump.ConsentDebugSettings;
import com.google.android.ump.ConsentForm;
import com.google.android.ump.ConsentInformation;
import com.google.android.ump.ConsentRequestParameters;
import com.google.android.ump.FormError;
import com.google.android.ump.UserMessagingPlatform;

import java.util.function.Consumer;

public class MengineGoogleConsentPlugin extends MengineService implements MengineListenerActivity, MengineTransparencyConsentProviderInterface {
    public static final String SERVICE_NAME = "GConsent";
    public static final boolean SERVICE_EMBEDDING = true;

    private boolean m_consentRequestInProgress;
    private boolean m_consentResolved;
    private MengineOperation<FormError> m_consentForm;

    @Override
    public boolean isTransparencyConsentProvider() {
        return true;
    }

    @Override
    public void onCreate(@NonNull MengineActivity activity, Bundle savedInstanceState) throws MengineServiceInvalidInitializeException {
        this.cancelConsentForm();

        this.m_consentRequestInProgress = false;
        this.m_consentResolved = false;

        this.requestConsentInfoUpdate(activity);
    }

    @Override
    public void onResume(@NonNull MengineActivity activity) {
        if (this.m_consentResolved == true) {
            return;
        }

        if (this.m_consentRequestInProgress == true) {
            return;
        }

        if (m_consentForm != null) {
            return;
        }

        this.requestConsentInfoUpdate(activity);
    }

    @Override
    public void onDestroy(@NonNull MengineActivity activity) {
        if (activity != this.getMengineActivity()) {
            return;
        }

        this.cancelConsentForm();
        this.m_consentRequestInProgress = false;
    }

    @Override
    public void onAppFinalize(@NonNull MengineApplication application) {
        super.onAppFinalize(application);

        this.cancelConsentForm();
    }

    private void requestConsentInfoUpdate(@NonNull MengineActivity activity) {
        if (MengineUtils.isCurrentActivity(activity, this.getMengineActivity()) == false) {
            return;
        }

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

        this.m_consentRequestInProgress = true;

        this.logInfo("Google Consent requestConsentInfoUpdate started");

        consentInformation.requestConsentInfoUpdate(activity, params
            , () -> {
                if (MengineUtils.isCurrentActivity(activity, this.getMengineActivity()) == false) {
                    return;
                }

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
                if (MengineUtils.isCurrentActivity(activity, this.getMengineActivity()) == false) {
                    return;
                }

                this.logError("consent info update failure: %s [%d]"
                    , formError.getMessage()
                    , formError.getErrorCode()
                );

                this.m_consentRequestInProgress = false;
                this.notifyTransparencyConsent(application, consentInformation);
            });
    }

    private boolean notifyTransparencyConsent(@NonNull MengineApplication application, @NonNull ConsentInformation consentInformation) {
        boolean canRequestAds = consentInformation.canRequestAds();

        this.logInfo("Google Consent canRequestAds: %b", canRequestAds);

        if (canRequestAds == false) {
            return false;
        }

        MengineParamTransparencyConsent tcParam = application.makeTransparencyConsentParam();

        if (tcParam.TRANSPARENCYCONSENT_GDPRAPPLIES == 1) {
            MengineParamTransparencyConsent.setConsentFlowUserGeography(application, MengineConsentFlowUserGeography.MengineConsentFlowUserGeography_EEA);
        } else {
            MengineParamTransparencyConsent.setConsentFlowUserGeography(application, MengineConsentFlowUserGeography.MengineConsentFlowUserGeography_NonEEA);
        }

        application.checkTransparencyConsentServices();

        return true;
    }

    private boolean isCurrentConsentActivity(@NonNull MengineApplication application, @NonNull MengineActivity activity) {
        if (application != this.getMengineApplication()) {
            return false;
        }

        if (application.isQuitRequested() == true) {
            return false;
        }

        if (activity.getLifecycleState() == MengineActivity.ELifecycleState.ACTIVITY_DESTROY) {
            return false;
        }

        MengineActivity currentActivity = this.getMengineActivity();

        if (MengineUtils.isCurrentActivity(activity, currentActivity) == false) {
            return false;
        }

        return true;
    }

    public void loadForm(@NonNull MengineApplication application, @NonNull MengineActivity activity, @NonNull ConsentInformation consentInformation) {
        ConsentForm.OnConsentFormDismissedListener onDismissed = formError -> {
            if (this.isCurrentConsentActivity(application, activity) == false) {
                return;
            }

            if (formError != null) {
                this.logError("consent form load and show failure: %s [%d]"
                    , formError.getMessage()
                    , formError.getErrorCode()
                );
            } else {
                this.logInfo("consent form load and show success");
            }

            this.m_consentRequestInProgress = false;
            boolean canRequestAds = this.notifyTransparencyConsent(application, consentInformation);

            if (formError != null) {
                return;
            }

            this.m_consentResolved = canRequestAds;
        };

        if (consentInformation.canRequestAds() == true) {
            onDismissed.onConsentFormDismissed(null);

            return;
        }

        UserMessagingPlatform.loadConsentForm(activity
            , consentForm -> {
                this.showForm(activity, listener -> {
                    consentForm.show(activity, listener);
                }, onDismissed);
            }
            , formError -> {
                onDismissed.onConsentFormDismissed(formError);
            });
    }

    @MainThread
    private void cancelConsentForm() {
        if (m_consentForm == null) {
            return;
        }

        m_consentForm.complete(new FormError(FormError.ErrorCode.INVALID_OPERATION, "consent form canceled"));
    }

    @MainThread
    private void showForm(@NonNull MengineActivity activity, @NonNull Consumer<ConsentForm.OnConsentFormDismissedListener> show, @NonNull ConsentForm.OnConsentFormDismissedListener onDismissed) {
        if (MengineUtils.isCurrentActivity(activity, this.getMengineActivity()) == false) {
            onDismissed.onConsentFormDismissed(new FormError(FormError.ErrorCode.INVALID_OPERATION, "invalid activity"));

            return;
        }

        if (m_consentForm != null) {
            onDismissed.onConsentFormDismissed(new FormError(FormError.ErrorCode.INVALID_OPERATION, "consent form already in progress"));

            return;
        }

        MengineApplication application = this.getMengineApplication();

        if (application == null) {
            onDismissed.onConsentFormDismissed(new FormError(FormError.ErrorCode.INVALID_OPERATION, "consent application finalized"));

            return;
        }

        MengineOperation<FormError> operation = new MengineOperation<>("show consent form") {
            @Override
            protected void onComplete(FormError formError) {
                if (m_consentForm == this) {
                    m_consentForm = null;

                    // The queue applies unfreeze after freeze even if the acknowledgement is pending.
                    MenginePlatformEventQueue.pushUnfreezeEvent(MenginePlatformEventQueue.FREEZE_RENDER);
                }

                if (MengineGoogleConsentPlugin.this.isCurrentConsentActivity(application, activity) == false) {
                    return;
                }

                onDismissed.onConsentFormDismissed(formError);
            }
        };

        Consumer<Boolean> onRenderFreeze = applied -> {
            if (operation.isCompleted() == true) {
                return;
            }

            if (applied == false) {
                operation.complete(new FormError(FormError.ErrorCode.INTERNAL_ERROR, "failed to pause rendering for consent form"));

                return;
            }

            if (this.isCurrentConsentActivity(application, activity) == false) {
                operation.complete(new FormError(FormError.ErrorCode.INVALID_OPERATION, "invalid activity"));

                return;
            }

            try {
                show.accept(formError -> {
                    if (operation.isCompleted() == true) {
                        return;
                    }

                    operation.complete(formError);
                });
            } catch (final RuntimeException e) {
                if (operation.isCompleted() == true) {
                    throw e;
                }

                operation.complete(new FormError(FormError.ErrorCode.INTERNAL_ERROR, e.toString()));
            }
        };

        if (MenginePlatformEventQueue.pushFreezeEvent(MenginePlatformEventQueue.FREEZE_RENDER, onRenderFreeze) == false) {
            onRenderFreeze.accept(false);

            return;
        }

        m_consentForm = operation;
    }

    public void resetConsentInformation() {
        MengineActivity activity = this.getMengineActivity();

        if (activity == null) {
            this.logWarning("reset consent information invalid activity");

            return;
        }

        ConsentInformation consentInformation = UserMessagingPlatform.getConsentInformation(activity);
        consentInformation.reset();
        this.m_consentResolved = false;
    }

    public void showConsentFlow() {
        MengineUtils.performOnMainThread(() -> {
            MengineActivity activity = this.getMengineActivity();

            if (MengineUtils.isActivityAvailable(activity) == false) {
                this.logError("[ERROR] showConsentFlow invalid activity");

                RuntimeException exception = new RuntimeException("invalid activity");
                this.nativeCall("onAndroidGoogleConsentFlowError", exception);

                return;
            }

            this.showForm(activity, listener -> {
                UserMessagingPlatform.showPrivacyOptionsForm(activity, listener);
            }, formError -> {
                if (formError != null) {
                    String message = formError.getMessage();

                    this.logError("Failed to show consent dialog error: %s [%d]"
                        , message
                        , formError.getErrorCode()
                    );

                    RuntimeException exception = new RuntimeException(message);
                    this.nativeCall("onAndroidGoogleConsentFlowError", exception);

                    return;
                }

                this.logInfo("Consent dialog was shown");

                MengineApplication application = this.getMengineApplication();
                ConsentInformation consentInformation = UserMessagingPlatform.getConsentInformation(activity);

                this.m_consentResolved = this.notifyTransparencyConsent(application, consentInformation);

                this.nativeCall("onAndroidGoogleConsentFlowCompleted");
            });
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
