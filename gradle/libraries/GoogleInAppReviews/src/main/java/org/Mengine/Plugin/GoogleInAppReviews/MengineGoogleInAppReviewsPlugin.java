package org.Mengine.Plugin.GoogleInAppReviews;

import android.os.Bundle;

import com.google.android.gms.tasks.Task;
import com.google.android.play.core.review.ReviewInfo;
import com.google.android.play.core.review.ReviewManager;
import com.google.android.play.core.review.ReviewManagerFactory;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MenginePlugin;

public class MengineGoogleInAppReviewsPlugin extends MenginePlugin {

    /**
     * <p>
     * событие об успешном получении объекта - после которого можно вызвать Review
     * - onGoogleInAppReviewsGettingReviewObject
     * <p>
     * Вызов окна Review
     * void launchTheInAppReview()
     * - onGoogleInAppReviewsLaunchingTheReviewCompleted
     */

    @Override
    public void onPythonEmbedding(MengineActivity activity) {
        this.addPythonPlugin("GoogleInAppReviews");
    }

    ReviewManager m_manager;
    ReviewInfo m_reviewInfo = null;

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) {
        m_manager = ReviewManagerFactory.create(activity);
    }

    public void initialize() {
        Task<ReviewInfo> request = m_manager.requestReviewFlow();
        request.addOnCompleteListener(task -> {
            if (task.isSuccessful()) {
                m_reviewInfo = task.getResult();
                this.pythonCall("onGoogleInAppReviewsGettingReviewObject");
            } else {
                this.logInfo("requestReviewFlow error %s -> %s", task.getException().getMessage(), task.getException().fillInStackTrace().toString());
            }
        });
    }

    public void launchTheInAppReview() {
        if (m_reviewInfo == null) {
            this.logInfo("reviewInfo == null -> laynchTheInAppReview disable");
            return;
        }

        Task<Void> flow = m_manager.launchReviewFlow(this.getActivity(), m_reviewInfo);
        flow.addOnCompleteListener(task -> {
            this.pythonCall("onGoogleInAppReviewsLaunchingTheReviewCompleted");
        });
    }
}
