package org.Mengine.Build.Facebook;

import com.facebook.FacebookException;

/**
 * Created by sweatcoin7 on 6/26/18.
 */

public interface FacebookShareCallback {
    void onShareSuccess(String postId);
    void onShareCancel();
    void onShareError(FacebookException exception);
}
