package org.Mengine.Plugin.GoogleInAppReviews;

import android.os.Bundle;

import com.google.android.gms.tasks.Task;
import com.google.android.play.core.review.ReviewInfo;
import com.google.android.play.core.review.ReviewManager;
import com.google.android.play.core.review.ReviewManagerFactory;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginActivityListener;
import org.Mengine.Base.MenginePluginInvalidInitializeException;

public class MengineGoogleInAppReviewsPlugin extends MenginePlugin implements MenginePluginActivityListener {
    public static final String PLUGIN_NAME = "GoogleInAppReviews";
    public static final boolean PLUGIN_EMBEDDING = true;

    /**
     * <p>
     * событие об успешном получении объекта - после которого можно вызвать Review
     * - onGoogleInAppReviewsGettingReviewObject
     * <p>
     * Вызов окна Review
     * void launchTheInAppReview()
     * - onGoogleInAppReviewsLaunchingTheReviewCompleted
     */

    private ReviewManager m_manager;
    private ReviewInfo m_reviewInfo;

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) throws MenginePluginInvalidInitializeException {
        m_manager = ReviewManagerFactory.create(activity);

        Task<ReviewInfo> request = m_manager.requestReviewFlow();
        request.addOnCompleteListener(task -> {
            if (task.isSuccessful() == true) {
                MengineGoogleInAppReviewsPlugin.this.logMessage("requestReviewFlow successful");

                m_reviewInfo = task.getResult();

                MengineGoogleInAppReviewsPlugin.this.activateSemaphore("onGoogleInAppReviewsGettingReviewObject");
            } else {
                Exception exception = task.getException();

                if (exception != null) {
                    MengineGoogleInAppReviewsPlugin.this.logWarning("requestReviewFlow error message: %s trace: %s"
                        , task.getException().getMessage()
                        , task.getException().fillInStackTrace()
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

        Task<Void> flow = m_manager.launchReviewFlow(activity, m_reviewInfo);

        flow.addOnCompleteListener(task -> {
            MengineGoogleInAppReviewsPlugin.this.logMessage("Launching the review completed");

            this.buildEvent("mng_inapp_review_completed")
                .log();

            MengineGoogleInAppReviewsPlugin.this.pythonCall("onGoogleInAppReviewsLaunchingTheReviewCompleted");
        });
    }
}
