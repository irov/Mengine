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

public class MengineGoogleInAppReviewsPlugin extends MengineService implements MengineListenerActivity {
    public static final String SERVICE_NAME = "GInAppReviews";
    public static final boolean SERVICE_EMBEDDING = true;

    private ReviewManager m_manager;
    private ReviewInfo m_reviewInfo;

    @Override
    public void onCreate(@NonNull MengineActivity activity, Bundle savedInstanceState) throws MengineServiceInvalidInitializeException {
        m_manager = ReviewManagerFactory.create(activity);

        m_manager.requestReviewFlow().addOnCompleteListener(task -> {
            if (task.isSuccessful() == false) {
                Exception exception = task.getException();

                if (exception != null) {
                    this.logWarning("requestReviewFlow error message: %s trace: %s"
                        , exception.getMessage()
                        , exception.fillInStackTrace()
                    );
                } else {
                    this.logWarning("requestReviewFlow unknown error");
                }

                this.nativeCall("onGoogleInAppReviewsRequestReviewError", exception);

                return;
            }

            this.logInfo("requestReviewFlow success");

            m_reviewInfo = task.getResult();

            this.activateSemaphore("onGoogleInAppReviewsGettingReviewObject");
        });
    }

    @Override
    public void onDestroy(@NonNull MengineActivity activity) {
        m_manager = null;
        m_reviewInfo = null;
    }

    public void launchTheInAppReview() {
        if (m_reviewInfo == null) {
            this.logError("[ERROR] reviewInfo == null");

            this.nativeCall("onGoogleInAppReviewsLaunchingTheReviewError", new RuntimeException("reviewInfo == null"));

            return;
        }

        if (MengineNetwork.isNetworkAvailable() == false) {
            this.logError("[ERROR] network is not available");

            this.nativeCall("onGoogleInAppReviewsLaunchingTheReviewError", new RuntimeException("network is not available"));

            return;
        }

        MengineActivity activity = this.getMengineActivity();

        if (activity == null) {
            this.logError("[ERROR] invalid activity");

            this.nativeCall("onGoogleInAppReviewsLaunchingTheReviewError", new RuntimeException("invalid activity"));

            return;
        }

        this.logInfo("launchTheInAppReview");

        this.buildEvent("mng_inapp_review")
            .log();

        m_manager.launchReviewFlow(activity, m_reviewInfo).addOnCompleteListener(task -> {
            if (task.isSuccessful() == false) {
                Exception exception = task.getException();

                if (exception != null) {
                    this.logWarning("launchReviewFlow error message: %s trace: %s"
                        , exception.getMessage()
                        , exception.fillInStackTrace()
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
