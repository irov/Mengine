package org.Mengine.Plugin.Facebook;

import android.net.Uri;
import android.os.Bundle;

import androidx.annotation.NonNull;

import com.facebook.AccessToken;
import com.facebook.AccessTokenTracker;
import com.facebook.CallbackManager;
import com.facebook.FacebookCallback;
import com.facebook.FacebookException;
import com.facebook.FacebookRequestError;
import com.facebook.FacebookSdk;
import com.facebook.GraphRequest;
import com.facebook.HttpMethod;
import com.facebook.LoggingBehavior;
import com.facebook.LoginStatusCallback;
import com.facebook.Profile;
import com.facebook.ProfileTracker;
import com.facebook.appevents.AppEventsConstants;
import com.facebook.appevents.AppEventsLogger;
import com.facebook.login.LoginManager;
import com.facebook.login.LoginResult;
import com.facebook.share.Sharer;
import com.facebook.share.model.ShareHashtag;
import com.facebook.share.model.ShareLinkContent;
import com.facebook.share.widget.ShareDialog;

import org.Mengine.Base.BuildConfig;
import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineAdRevenueParam;
import org.Mengine.Base.MengineAnalyticsEventCategory;
import org.Mengine.Base.MengineAnalyticsEventParam;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineListenerAdRevenue;
import org.Mengine.Base.MengineListenerTransparencyConsent;
import org.Mengine.Base.MengineService;
import org.Mengine.Base.MengineListenerAnalytics;
import org.Mengine.Base.MengineListenerApplication;
import org.Mengine.Base.MengineServiceInvalidInitializeException;
import org.Mengine.Base.MengineListenerPushToken;
import org.Mengine.Base.MengineListenerRemoteMessage;
import org.Mengine.Base.MengineListenerUser;
import org.Mengine.Base.MengineTransparencyConsentParam;
import org.Mengine.Base.MengineUtils;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.List;
import java.util.Map;

public class MengineFacebookPlugin extends MengineService implements MengineListenerAnalytics, MengineListenerApplication, MengineListenerRemoteMessage, MengineListenerPushToken, MengineListenerUser, MengineListenerAdRevenue, MengineListenerTransparencyConsent {
    public static final String SERVICE_NAME = "Facebook";
    public static final boolean SERVICE_EMBEDDING = true;

    private static final int ERROR_CODE_UNKNOWN = 0;
    private static final int ERROR_CODE_LOGGED_OUT = 1;
    private static final int ERROR_CODE_EXPIRED = 2;
    private static final int ERROR_CODE_INVALID_PARAMETER = 3;


    private CallbackManager m_facebookCallbackManager;
    private AccessTokenTracker m_accessTokenTracker;
    private ProfileTracker m_profileTracker;
    private AppEventsLogger m_logger;

    @Override
    public String onAppVersion(@NonNull MengineApplication application) {
        String fbSdkVersion = FacebookSdk.getSdkVersion();

        return fbSdkVersion;
    }

    @Override
    public void onAppCreate(@NonNull MengineApplication application) throws MengineServiceInvalidInitializeException {
        MengineTransparencyConsentParam tcParam = application.makeTransparencyConsentParam();
        boolean AD_STORAGE = tcParam.getConsentAdStorage();

        FacebookSdk.setAdvertiserIDCollectionEnabled(AD_STORAGE);
        FacebookSdk.setAutoLogAppEventsEnabled(AD_STORAGE);

        try {
            AppEventsLogger.activateApp(application);
        } catch (final Exception e) {
            this.logError("[ERROR] AppEventsLogger activateApp caught exception: %s"
                , e.getMessage()
            );
        }

        if (BuildConfig.DEBUG == true) {
            FacebookSdk.setIsDebugEnabled(true);

            //FacebookSdk.addLoggingBehavior(LoggingBehavior.APP_EVENTS);
            FacebookSdk.addLoggingBehavior(LoggingBehavior.DEVELOPER_ERRORS);
        }

        LoginManager.getInstance().retrieveLoginStatus(application, new LoginStatusCallback() {
            @Override
            public void onCompleted(@NonNull AccessToken accessToken) {
                MengineFacebookPlugin.this.logInfo("retrieve login [onCompleted] application: %s user: %s token: %s"
                    , accessToken.getApplicationId()
                    , accessToken.getUserId()
                    , MengineUtils.getRedactedValue(accessToken.getToken())
                );

                MengineFacebookPlugin.this.activateSemaphore("FacebookRetrieveLoginCompleted");
            }

            @Override
            public void onFailure() {
                MengineFacebookPlugin.this.logInfo("retrieve login [onFailure]");
            }

            @Override
            public void onError(@NonNull Exception e) {
                MengineFacebookPlugin.this.logError("[ERROR] retrieve login [onError] exception: %s"
                    , e.getMessage()
                );
            }
        });

        m_accessTokenTracker = new AccessTokenTracker() {
            @Override
            protected void onCurrentAccessTokenChanged(AccessToken oldToken, AccessToken newToken) {
                MengineFacebookPlugin.this.logInfo("onCurrentAccessTokenChanged old token: %s new token: %s"
                    , MengineUtils.getRedactedValue(oldToken)
                    , MengineUtils.getRedactedValue(newToken)
                );

                String oldTokenString = oldToken != null ? oldToken.getToken() : "";
                String newTokenString = newToken != null ? newToken.getToken() : "";

                MengineFacebookPlugin.this.pythonCall("onFacebookCurrentAccessTokenChanged", oldTokenString, newTokenString);
            }
        };

        m_accessTokenTracker.startTracking();

        m_profileTracker = new ProfileTracker() {
            @Override
            protected void onCurrentProfileChanged(Profile oldProfile, Profile newProfile) {
                MengineFacebookPlugin.this.logInfo("onCurrentProfileChanged old profile: %s new profile: %s"
                    , oldProfile.getId()
                    , newProfile.getId()
                );

                MengineFacebookPlugin.this.pythonCall("onFacebookCurrentProfileChanged");
            }
        };

        m_profileTracker.startTracking();

        m_facebookCallbackManager = CallbackManager.Factory.create();

        LoginManager.getInstance().registerCallback(m_facebookCallbackManager, new FacebookCallback<>() {
            @Override
            public void onSuccess(LoginResult loginResult) {
                Profile.fetchProfileForCurrentAccessToken();

                AccessToken accessToken = loginResult.getAccessToken();

                String applicationId = accessToken.getApplicationId();
                String userId = accessToken.getUserId();
                String token = accessToken.getToken();

                MengineFacebookPlugin.this.logInfo("login [onSuccess] application: %s user: %s token: %s"
                    , applicationId
                    , userId
                    , MengineUtils.getRedactedValue(token)
                );

                MengineFacebookPlugin.this.pythonCall("onFacebookLoginSuccess", token);
            }

            @Override
            public void onCancel() {
                MengineFacebookPlugin.this.logInfo("login [onCancel]");

                AccessToken.setCurrentAccessToken(null);

                MengineFacebookPlugin.this.pythonCall("onFacebookLoginCancel");
            }

            @Override
            public void onError(@NonNull FacebookException e) {
                String message = e.getMessage();

                MengineFacebookPlugin.this.logError("[ERROR] login [onError] exception: %s"
                    , message
                );

                AccessToken.setCurrentAccessToken(null);

                MengineFacebookPlugin.this.pythonCall("onFacebookLoginError", ERROR_CODE_UNKNOWN, e);
            }
        });

        String userId = application.getUserId();
        AppEventsLogger.setUserID(userId);

        AppEventsLogger logger = AppEventsLogger.newLogger(application);
        m_logger = logger;
    }

    @Override
    public void onAppTerminate(@NonNull MengineApplication application) {
        if (m_logger != null) {
            m_logger.flush();
            m_logger = null;
        }

        if (m_accessTokenTracker != null) {
            m_accessTokenTracker.stopTracking();
            m_accessTokenTracker = null;
        }

        if (m_profileTracker != null) {
            m_profileTracker.stopTracking();
            m_profileTracker = null;
        }

        if (m_facebookCallbackManager != null) {
            LoginManager.getInstance().unregisterCallback(m_facebookCallbackManager);
            m_facebookCallbackManager = null;
        }
    }

    @Override
    public void onMengineChangeUserId(@NonNull MengineApplication application, String userId) {
        AppEventsLogger.setUserID(userId);
    }

    @Override
    public void onMengineRemoveUserData(@NonNull MengineApplication application) {
        LoginManager.getInstance().logOut();

        AccessToken.setCurrentAccessToken(null);
        Profile.setCurrentProfile(null);

        AppEventsLogger.setUserID(null);
        AppEventsLogger.clearUserData();
        AppEventsLogger.clearUserID();
    }

    @Override
    public void onMengineChangePushToken(@NonNull MengineApplication application, String token) {
        AppEventsLogger.setPushNotificationsRegistrationId(token);
    }

    private void updateBundle(@NonNull Bundle bundle, @NonNull Map<String, Object> parameters) {
        for (Map.Entry<String, Object> entry : parameters.entrySet()) {
            String name = entry.getKey();
            Object value = entry.getValue();

            if (value == null) {
                bundle.putString(name, null);
            } else if (value instanceof Boolean) {
                if ((Boolean)value == true) {
                    bundle.putLong(name, 1);
                } else {
                    bundle.putLong(name, 0);
                }
            } else if (value instanceof Integer) {
                bundle.putInt(name, (Integer)value);
            } else if (value instanceof Long) {
                bundle.putLong(name, (Long)value);
            } else if (value instanceof Double) {
                bundle.putDouble(name, (Double)value);
            } else if (value instanceof String) {
                bundle.putString(name, (String)value);
            } else {
                this.logError("[ERROR] unsupported parameter: %s class: %s"
                    , value
                    , value == null ? "null" : value.getClass()
                );
            }
        }
    }

    @Override
    public void onMengineAnalyticsEvent(@NonNull MengineApplication application, @NonNull MengineAnalyticsEventParam param) {
        if (m_logger == null) {
            return;
        }

        if (param.ANALYTICS_CATEGORY == MengineAnalyticsEventCategory.MengineAnalyticsEventCategory_System) {
            return;
        }

        Bundle bundle = new Bundle();

        this.updateBundle(bundle, param.ANALYTICS_BASES);
        this.updateBundle(bundle, param.ANALYTICS_PARAMETERS);

        m_logger.logEvent(param.ANALYTICS_NAME, bundle);
    }

    @Override
    public void onMengineAnalyticsFlush(@NonNull MengineApplication application) {
        if (m_logger == null) {
            return;
        }

        m_logger.flush();
    }

    @Override
    public void onMengineRemoteMessageNewToken(@NonNull MengineApplication application, String token) {
        AppEventsLogger.setPushNotificationsRegistrationId(token);
    }

    public boolean isLoggedIn() {
        AccessToken accessToken = AccessToken.getCurrentAccessToken();

        if (accessToken == null) {
            return false;
        }

        if (accessToken.isExpired() == true) {
            return false;
        }

        return true;
    }
    
    public String getAccessToken() {
        AccessToken accessToken = AccessToken.getCurrentAccessToken();

        if (accessToken == null) {
            return "";
        }

        if (accessToken.isExpired() == true) {
            return "";
        }
        
        String token = accessToken.getToken();
        
        return token;
    }
    
    public void performLogin(List<String> permissions) {
        this.logInfo("performLogin permissions: %s"
            , permissions
        );

        MengineActivity activity = this.getMengineActivity();

        LoginManager.getInstance().logInWithReadPermissions(activity, m_facebookCallbackManager, permissions);
    }

    private boolean checkValidAccessToken(AccessToken accessToken, String errorMethod) {
        if (accessToken == null) {
            this.logInfo("is not logged in");

            this.pythonCall(errorMethod, ERROR_CODE_LOGGED_OUT, new RuntimeException("is not logged in"));

            return false;
        }

        if (accessToken.isExpired() == true) {
            this.logInfo("is expired");

            this.pythonCall(errorMethod, ERROR_CODE_EXPIRED, new RuntimeException("is expired"));

            return false;
        }

        return true;
    }
    
    public void logout() {
        this.logInfo("logout");

        AccessToken accessToken = AccessToken.getCurrentAccessToken();

        if (this.checkValidAccessToken(accessToken, "onFacebookLogoutError") == false) {
            return;
        }

        this.buildEvent("mng_fb_logout")
            .log();

        GraphRequest request = new GraphRequest(accessToken, "/me/permissions/", null, HttpMethod.DELETE, graphResponse -> {
            this.logInfo("GraphRequest DELETE onCompleted");

            LoginManager.getInstance().logOut();

            this.pythonCall("onFacebookLogoutSuccess");
        });

        request.executeAsync();
    }
    
    public void getUser() {
        this.logInfo("getUser");

        AccessToken accessToken = AccessToken.getCurrentAccessToken();

        if (this.checkValidAccessToken(accessToken, "onFacebookUserFetchError") == false) {
            return;
        }

        GraphRequest request = GraphRequest.newMeRequest(accessToken, (object, response) -> {
            if (response == null) {
                this.logError("[ERROR] GraphRequest newMeRequest returned null response");

                this.pythonCall("onFacebookUserFetchError", ERROR_CODE_UNKNOWN, new RuntimeException("null response"));

                return;
            }

            this.logInfo("GraphRequest new Me request completed object: %s response: %s"
                , object
                , response.getRawResponse()
            );

            FacebookRequestError responseError = response.getError();

            if (responseError != null) {
                int errorCode = responseError.getErrorCode();
                int subErrorCode = responseError.getSubErrorCode();
                String errorMessage = responseError.getErrorMessage();

                this.logError("[ERROR] facebook error code: %d subCode: %d message: %s"
                    , errorCode
                    , subErrorCode
                    , errorMessage
                );

                this.pythonCall("onFacebookUserFetchError", ERROR_CODE_UNKNOWN, new RuntimeException(errorMessage));

                return;
            }

            String objectString = object != null ? object.toString() : "";
            String responseString = response.toString();

            this.pythonCall("onFacebookUserFetchSuccess", objectString, responseString);
        });

        Bundle parameters = new Bundle();
        parameters.putString("fields", "id, name, picture");
        request.setParameters(parameters);
        request.executeAsync();
    }
    
    public void shareLink(String link, String hashtag, String quote) {
        this.logInfo("shareLink link: %s hashtag: %s quote: %s"
            , link
            , hashtag
            , quote
        );

        if (ShareDialog.canShow(ShareLinkContent.class) == false) {
            this.logWarning("shareLink can't show");

            MengineFacebookPlugin.this.pythonCall("onFacebookShareError", ERROR_CODE_UNKNOWN, new RuntimeException("can't show"));

            return;
        }

        this.buildEvent("mng_fb_share_link")
            .addParameterString("url", link)
            .addParameterString("hashtag", hashtag)
            .addParameterString("quote", quote)
            .log();

        MengineActivity activity = this.getMengineActivity();

        ShareDialog shareDialog = new ShareDialog(activity);
        shareDialog.registerCallback(m_facebookCallbackManager, new FacebookCallback<>() {
            @Override
            public void onSuccess(Sharer.Result result) {
                String postId = result.getPostId() != null ? result.getPostId() : "";

                MengineFacebookPlugin.this.logInfo("shareLink success postId: %s"
                    , postId
                );

                MengineFacebookPlugin.this.buildEvent("mng_fb_share_link_success")
                    .addParameterString("url", link)
                    .addParameterString("hashtag", hashtag)
                    .addParameterString("quote", quote)
                    .addParameterString("postId", postId)
                    .log();

                MengineFacebookPlugin.this.pythonCall("onFacebookShareSuccess", postId);
            }

            @Override
            public void onCancel() {
                MengineFacebookPlugin.this.logInfo("shareLink cancel");

                MengineFacebookPlugin.this.buildEvent("mng_fb_share_link_cancel")
                    .addParameterString("url", link)
                    .addParameterString("hashtag", hashtag)
                    .addParameterString("quote", quote)
                    .log();

                MengineFacebookPlugin.this.pythonCall("onFacebookShareCancel");
            }

            @Override
            public void onError(@NonNull FacebookException exception) {
                String error_message = exception.getMessage();

                MengineFacebookPlugin.this.logError("[ERROR] shareLink error: %s"
                    , error_message
                );

                MengineFacebookPlugin.this.buildEvent("mng_fb_share_link_error")
                    .addParameterString("url", link)
                    .addParameterString("hashtag", hashtag)
                    .addParameterString("quote", quote)
                    .addParameterException("exception", exception)
                    .log();

                MengineFacebookPlugin.this.pythonCall("onFacebookShareError", ERROR_CODE_UNKNOWN, exception);
            }
        });

        Uri contentUrl = Uri.parse(link);

        ShareHashtag shareHashtag = new ShareHashtag.Builder()
            .setHashtag(hashtag)
            .build();

        ShareLinkContent linkContent = new ShareLinkContent.Builder()
            .setContentUrl(contentUrl)
            .setShareHashtag(shareHashtag)
            .setQuote(quote)
            .build();

        shareDialog.show(linkContent);
    }

    protected void getProfileUserPictureLinkWithAccessToken(AccessToken accessToken, final String user_id, final String typeParameter) {
        String graphPath = "/" + user_id + "/picture";

        GraphRequest request = GraphRequest.newGraphPathRequest(accessToken, graphPath, response -> {
            FacebookRequestError error = response.getError();

            if (error != null) {
                String errorMessage = error.getErrorMessage();

                this.logError("[ERROR] profile user picture link: %s get error: %s"
                    , graphPath
                    , errorMessage
                );

                this.pythonCall("onFacebookProfilePictureLinkGetError", ERROR_CODE_UNKNOWN, new RuntimeException(errorMessage));

                return;
            }

            JSONObject responseObject = response.getJSONObject();

            if (responseObject == null) {
                this.logError("[ERROR] profile user picture link: %s invalid response: %s"
                    , graphPath
                    , response
                );

                this.pythonCall("onFacebookProfilePictureLinkGetError", ERROR_CODE_UNKNOWN, new RuntimeException("invalid response"));

                return;
            }

            String pictureURL = "";
            try {
                pictureURL = responseObject.getJSONObject("data").getString("url");
            } catch (final JSONException e) {
                this.logError("[ERROR] profile user picture link: %s catch JSONException: %s"
                    , graphPath
                    , e.getMessage()
                );

                this.pythonCall("onFacebookProfilePictureLinkGetError", ERROR_CODE_UNKNOWN, e);

                return;
            }

            this.logInfo("request profile user: %s picture link completed: %s"
                , user_id
                , pictureURL
            );

            this.pythonCall("onFacebookProfilePictureLinkGetSuccess", user_id, pictureURL);
        });

        Bundle parameters = new Bundle();
        parameters.putBoolean("redirect", false);
        parameters.putString("type", typeParameter);
        request.setParameters(parameters);
        request.executeAsync();
    }

    public void getProfilePictureLink(final String typeParameter) {
        this.logInfo("getProfilePictureLink typeParameter: %s"
            , typeParameter
        );

        AccessToken accessToken = AccessToken.getCurrentAccessToken();

        if (this.checkValidAccessToken(accessToken, "onFacebookProfilePictureLinkGetError") == false) {
            return;
        }

        String userId = accessToken.getUserId();

        this.getProfileUserPictureLinkWithAccessToken(accessToken, userId, typeParameter);
    }
    
    public void getProfileUserPictureLink(final String user_id, final String typeParameter) {
        this.logInfo("getProfileUserPictureLink user_id: %s typeParameter: %s"
            , user_id
            , typeParameter
        );

        if (user_id.isEmpty() == true) {
            this.logWarning("user_id is empty");

            this.pythonCall("onFacebookProfilePictureLinkGetError", ERROR_CODE_INVALID_PARAMETER, new RuntimeException("user_id is empty"));

            return;
        }

        AccessToken accessToken = AccessToken.getCurrentAccessToken();

        if (this.checkValidAccessToken(accessToken, "onFacebookProfilePictureLinkGetError") == false) {
            return;
        }

        this.getProfileUserPictureLinkWithAccessToken(accessToken, user_id, typeParameter);
    }

    @Override
    public void onMengineTransparencyConsent(@NonNull MengineApplication application, @NonNull MengineTransparencyConsentParam tcParam) {
        boolean AD_STORAGE = tcParam.getConsentAdStorage();

        FacebookSdk.setAdvertiserIDCollectionEnabled(AD_STORAGE);
        FacebookSdk.setAutoLogAppEventsEnabled(AD_STORAGE);
    }

    @Override
    public void onMengineAdRevenue(@NonNull MengineApplication application, @NonNull MengineAdRevenueParam revenue) {
        if (m_logger == null) {
            return;
        }

        Bundle params = new Bundle();
        params.putString(AppEventsConstants.EVENT_PARAM_CURRENCY, revenue.ADREVENUE_REVENUE_CURRENCY);
        params.putString(AppEventsConstants.EVENT_PARAM_AD_TYPE, revenue.ADREVENUE_FORMAT.getName());

        m_logger.logEvent(AppEventsConstants.EVENT_NAME_AD_IMPRESSION, revenue.ADREVENUE_REVENUE_VALUE, params);
    }
}
