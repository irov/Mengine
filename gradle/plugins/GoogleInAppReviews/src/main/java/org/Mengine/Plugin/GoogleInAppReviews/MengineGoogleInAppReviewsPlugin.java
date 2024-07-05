package org.Mengine.Plugin.GoogleInAppReviews;

import android.os.Bundle;

import com.google.android.play.core.review.ReviewInfo;
import com.google.android.play.core.review.ReviewManager;
import com.google.android.play.core.review.ReviewManagerFactory;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginActivityListener;
import org.Mengine.Base.MenginePluginInvalidInitializeException;

public class MengineGoogleInAppReviewsPlugin extends MenginePlugin implements MenginePluginActivityListener {
    public static final String PLUGIN_NAME = "MengineGInAppReviews";
    public static final boolean PLUGIN_EMBEDDING = true;

    private ReviewManager m_manager;
    private ReviewInfo m_reviewInfo;

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) throws MenginePluginInvalidInitializeException {
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
