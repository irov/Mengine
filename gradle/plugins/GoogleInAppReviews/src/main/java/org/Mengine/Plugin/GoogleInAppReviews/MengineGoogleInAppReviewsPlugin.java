package org.Mengine.Plugin.GoogleInAppReviews;

import android.os.Bundle;

import androidx.annotation.NonNull;

import com.google.android.play.core.review.ReviewInfo;
import com.google.android.play.core.review.ReviewManager;
import com.google.android.play.core.review.ReviewManagerFactory;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineNetwork;
import org.Mengine.Base.MengineService;
import org.Mengine.Base.MengineListenerActivity;
import org.Mengine.Base.MengineServiceInvalidInitializeException;
import org.Mengine.Base.MengineUtils;

public class MengineGoogleInAppReviewsPlugin extends MengineService implements MengineListenerActivity {
    public static final String SERVICE_NAME = "GInAppReviews";
    public static final boolean SERVICE_EMBEDDING = true;

    private ReviewManager m_manager;
    private boolean m_reviewInProgress;

    @Override
    public void onCreate(@NonNull MengineActivity activity, Bundle savedInstanceState) throws MengineServiceInvalidInitializeException {
        m_manager = ReviewManagerFactory.create(activity);
        this.activateSemaphore("GoogleInAppReviewsReady");
    }

    @Override
    public void onDestroy(@NonNull MengineActivity activity) {
        m_manager = null;
        m_reviewInProgress = false;
    }

    public void launchTheInAppReview() {
        MengineUtils.performOnMainThread(this::requestReviewFlow);
    }

    private void requestReviewFlow() {
        if (m_reviewInProgress == true) {
            return;
        }

        if (MengineNetwork.isNetworkAvailable() == false) {
            this.logInfo("in-app review unavailable while offline");

            this.nativeCall("onGoogleInAppReviewsRequestReviewError", new IllegalStateException("network is not available"));

            return;
        }

        MengineActivity activity = this.getMengineActivity();

        if (MengineUtils.isActivityAvailable(activity) == false) {
            this.nativeCall("onGoogleInAppReviewsRequestReviewError", new IllegalStateException("review activity is not available"));

            return;
        }

        ReviewManager manager = m_manager;

        if (manager == null) {
            this.nativeCall("onGoogleInAppReviewsRequestReviewError", new IllegalStateException("review manager is not available"));

            return;
        }

        m_reviewInProgress = true;

        manager.requestReviewFlow().addOnCompleteListener(task -> {
            if (m_manager != manager) {
                return;
            }

            if (task.isSuccessful() == false) {
                m_reviewInProgress = false;
                Exception exception = task.getException();

                this.logWarning("requestReviewFlow failed: %s"
                    , exception
                );

                this.nativeCall("onGoogleInAppReviewsRequestReviewError", exception);

                return;
            }

            if (MengineUtils.isCurrentActivity(activity, this.getMengineActivity()) == false) {
                m_reviewInProgress = false;

                return;
            }

            ReviewInfo reviewInfo = task.getResult();
            this.launchReviewFlow(activity, manager, reviewInfo);
        });
    }

    private void launchReviewFlow(@NonNull MengineActivity activity, @NonNull ReviewManager manager, @NonNull ReviewInfo reviewInfo) {
        this.logInfo("launchTheInAppReview");

        this.buildEvent("mng_inapp_review")
            .log();

        manager.launchReviewFlow(activity, reviewInfo).addOnCompleteListener(task -> {
            if (m_manager != manager) {
                return;
            }

            m_reviewInProgress = false;

            if (task.isSuccessful() == false) {
                Exception exception = task.getException();

                if (exception != null) {
                    this.logWarning("launchReviewFlow error message: %s trace: %s"
                        , exception.getMessage()
                        , exception
                    );
                } else {
                    this.logWarning("launchReviewFlow unknown error");
                }

                this.buildEvent("mng_inapp_review_error")
                    .addParameterException("exception", exception)
                    .log();

                this.nativeCall("onGoogleInAppReviewsLaunchingTheReviewError", exception);

                return;
            }

            this.logInfo("launching the review success");

            this.buildEvent("mng_inapp_review_success")
                .log();

            this.nativeCall("onGoogleInAppReviewsLaunchingTheReviewSuccess");
        });
    }
}
