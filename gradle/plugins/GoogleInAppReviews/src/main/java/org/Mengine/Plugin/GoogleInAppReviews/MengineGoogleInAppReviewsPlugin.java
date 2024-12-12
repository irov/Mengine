package org.Mengine.Plugin.GoogleInAppReviews;

import android.os.Bundle;

import com.google.android.play.core.review.ReviewInfo;
import com.google.android.play.core.review.ReviewManager;
import com.google.android.play.core.review.ReviewManagerFactory;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineService;
import org.Mengine.Base.MengineListenerActivity;
import org.Mengine.Base.MengineServiceInvalidInitializeException;

public class MengineGoogleInAppReviewsPlugin extends MengineService implements MengineListenerActivity {
    public static final String SERVICE_NAME = "MengineGInAppReviews";
    public static final boolean SERVICE_EMBEDDING = true;

    private ReviewManager m_manager;
    private ReviewInfo m_reviewInfo;

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) throws MengineServiceInvalidInitializeException {
        m_manager = ReviewManagerFactory.create(activity);

        m_manager.requestReviewFlow().addOnCompleteListener(task -> {
            if (task.isSuccessful() == true) {
                MengineGoogleInAppReviewsPlugin.this.logMessage("requestReviewFlow successful");

                m_reviewInfo = task.getResult();

                MengineGoogleInAppReviewsPlugin.this.activateSemaphore("onGoogleInAppReviewsGettingReviewObject");
            } else {
                Exception exception = task.getException();

                if (exception != null) {
                    MengineGoogleInAppReviewsPlugin.this.logWarning("requestReviewFlow error message: %s trace: %s"
                        , exception.getMessage()
                        , exception.fillInStackTrace()
                    );
                } else {
                    MengineGoogleInAppReviewsPlugin.this.logWarning("requestReviewFlow unknown error");
                }
            }
        });
    }

    @Override
    public void onDestroy(MengineActivity activity) {
        m_manager = null;
        m_reviewInfo = null;
    }

    public void launchTheInAppReview() {
        if (m_reviewInfo == null) {
            this.logError("[ERROR] reviewInfo == null");

            return;
        }

        this.logMessage("launchTheInAppReview");

        this.buildEvent("mng_inapp_review")
            .log();

        MengineActivity activity = this.getMengineActivity();

        m_manager.launchReviewFlow(activity, m_reviewInfo).addOnCompleteListener(task -> {
            MengineGoogleInAppReviewsPlugin.this.logMessage("Launching the review completed");

            MengineGoogleInAppReviewsPlugin.this.buildEvent("mng_inapp_review_completed")
                .log();

            MengineGoogleInAppReviewsPlugin.this.pythonCall("onGoogleInAppReviewsLaunchingTheReviewCompleted");
        });
    }
}
