package org.Mengine.Plugin.Facebook;

import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;

import androidx.annotation.NonNull;

import com.facebook.AccessToken;
import com.facebook.AccessTokenTracker;
import com.facebook.CallbackManager;
import com.facebook.FacebookCallback;
import com.facebook.FacebookException;
import com.facebook.FacebookSdk;
import com.facebook.GraphRequest;
import com.facebook.GraphResponse;
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

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineEvent;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginActivityListener;
import org.Mengine.Base.MenginePluginAnalyticsListener;
import org.Mengine.Base.MenginePluginApplicationListener;
import org.Mengine.Base.MenginePluginInvalidInitializeException;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.List;
import java.util.Map;

public class MengineFacebookPlugin extends MenginePlugin implements MenginePluginAnalyticsListener, MenginePluginApplicationListener, MenginePluginActivityListener {
    public static final String PLUGIN_NAME = "Facebook";
    public static final boolean PLUGIN_EMBEDDING = true;

    private CallbackManager m_facebookCallbackManager;
    private AccessTokenTracker m_accessTokenTracker;
    private ProfileTracker m_profileTracker;
    private AppEventsLogger m_logger;

    private String m_facebookUserId;

    @Override
    public void onEvent(MengineApplication application, MengineEvent event, Object ... args) {
        if (event == MengineEvent.EVENT_PUSH_TOKEN) {
            final String token = (String)args[0];

            AppEventsLogger.setPushNotificationsRegistrationId(token);
        } else if (event == MengineEvent.EVENT_SESSION_ID) {
            String sessionId = (String)args[0];

            AppEventsLogger.setUserID(sessionId);
        }
    }

    @Override
    public void onAppPrepare(MengineApplication application) throws MenginePluginInvalidInitializeException {
        Context context = application.getApplicationContext();

        m_logger = AppEventsLogger.newLogger(context);

        try {
            AppEventsLogger.activateApp(application);
        } catch (Exception e) {
            this.logError("AppEventsLogger activateApp caught exception: %s"
                , e.getLocalizedMessage()
            );
        }
    }

    @Override
    public void onAppCreate(MengineApplication application) throws MenginePluginInvalidInitializeException {
        //Empty
    }

    @Override
    public void onAppTerminate(MengineApplication application) {
        m_logger = null;
    }

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) throws MenginePluginInvalidInitializeException {
        if (BuildConfig.DEBUG == true) {
            FacebookSdk.setIsDebugEnabled(true);
            FacebookSdk.addLoggingBehavior(LoggingBehavior.APP_EVENTS);
        }

        LoginManager.getInstance().retrieveLoginStatus(activity, new LoginStatusCallback() {
            @Override
            public void onCompleted(@NonNull AccessToken accessToken) {
                // User was previously logged in, can log them in directly here.
                // If this callback is called, a popup notification appears that says
                // "Logged in as <User Name>"

                MengineFacebookPlugin.this.logMessage("retrieve login [onCompleted] application: %s user: %s token: %s"
                    , accessToken.getApplicationId()
                    , accessToken.getUserId()
                    , accessToken.getToken()
                );

                MengineFacebookPlugin.this.activateSemaphore("FacebookRetrieveLoginCompleted");
            }

            @Override
            public void onFailure() {
                MengineFacebookPlugin.this.logWarning("retrieve login [onFailure]");
            }

            @Override
            public void onError(@NonNull Exception e) {
                MengineFacebookPlugin.this.logError("retrieve login [onError] exception: %s"
                    , e.getLocalizedMessage()
                );
            }
        });

        m_facebookCallbackManager = CallbackManager.Factory.create();

        m_accessTokenTracker = new AccessTokenTracker() {
            @Override
            protected void onCurrentAccessTokenChanged(AccessToken oldToken, AccessToken newToken) {
                MengineFacebookPlugin.this.logMessage("onCurrentAccessTokenChanged old token: %s new token: %s"
                    , oldToken
                    , newToken
                );

                if (newToken != null) {
                    m_facebookUserId = newToken.getUserId();
                }

                String oldTokenString = oldToken != null ? oldToken.getToken() : "";
                String newTokenString = newToken != null ? newToken.getToken() : "";

                MengineFacebookPlugin.this.pythonCall("onFacebookCurrentAccessTokenChanged", oldTokenString, newTokenString);
            }
        };

        m_accessTokenTracker.startTracking();

        m_profileTracker = new ProfileTracker() {
            @Override
            protected void onCurrentProfileChanged(Profile oldProfile, Profile newProfile) {
                MengineFacebookPlugin.this.logMessage("onCurrentProfileChanged old profile: %s new profile: %s"
                    , oldProfile
                    , newProfile
                );

                AccessToken accessToken = AccessToken.getCurrentAccessToken();

                if (accessToken != null) {
                    m_facebookUserId = accessToken.getUserId();
                }

                MengineFacebookPlugin.this.pythonCall("onFacebookCurrentProfileChanged");
            }
        };

        m_profileTracker.startTracking();

        LoginManager.getInstance().registerCallback(m_facebookCallbackManager, new FacebookCallback<>() {
            @Override
            public void onSuccess(LoginResult loginResult) {
                Profile.fetchProfileForCurrentAccessToken();

                AccessToken accessToken = loginResult.getAccessToken();

                m_facebookUserId = accessToken.getUserId();

                String applicationId = accessToken.getApplicationId();
                String userId = accessToken.getUserId();
                String token = accessToken.getToken();

                MengineFacebookPlugin.this.logMessage("login [onSuccess] application: %s user: %s token: %s"
                    , applicationId
                    , userId
                    , token
                );

                MengineFacebookPlugin.this.pythonCall("onFacebookLoginSuccess", token);
            }

            @Override
            public void onCancel() {
                MengineFacebookPlugin.this.logMessage("login [onCancel]");

                AccessToken.setCurrentAccessToken(null);

                MengineFacebookPlugin.this.pythonCall("onFacebookLoginCancel");
            }

            @Override
            public void onError(@NonNull FacebookException e) {
                String message = e.getLocalizedMessage();

                MengineFacebookPlugin.this.logError("login [onError] exception: %s"
                    , message
                );

                AccessToken.setCurrentAccessToken(null);

                MengineFacebookPlugin.this.pythonCall("onFacebookLoginError", message);
            }
        });

        AccessToken accessToken = AccessToken.getCurrentAccessToken();
        if (accessToken != null) {
            m_facebookUserId = accessToken.getUserId();
        }
    }

    @Override
    public void onDestroy(MengineActivity activity) {
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
    public void onActivityResult(MengineActivity activity, int requestCode, int resultCode, Intent data) {
        if (m_facebookCallbackManager != null) {
            m_facebookCallbackManager.onActivityResult(requestCode, resultCode, data);
        }
    }

    @Override
    public void onMengineAnalyticsEvent(MengineApplication application, int eventType, String eventName, long timestamp, Map<String, Object> bases, Map<String, Object> parameters) {
        if (m_logger == null) {
            return;
        }

        switch (eventType) {
            case EAET_CUSTOM: {
                Bundle params = new Bundle();

                for (Map.Entry<String, Object> entry : bases.entrySet()) {
                    String name = entry.getKey();
                    Object value = entry.getValue();

                    if (value instanceof Boolean) {
                        params.putBoolean(name, (Boolean)value);
                    } else if (value instanceof Long) {
                        params.putLong(name, (Long)value);
                    } else if (value instanceof Double) {
                        params.putDouble(name, (Double)value);
                    } else if (value instanceof String) {
                        params.putString(name, (String)value);
                    } else {
                        this.logError("unsupported parameter: %s class: %s"
                            , value
                            , value.getClass()
                        );

                        return;
                    }
                }

                for (Map.Entry<String, Object> entry : parameters.entrySet()) {
                    String name = entry.getKey();
                    Object value = entry.getValue();

                    if (value instanceof Boolean) {
                        params.putBoolean(name, (Boolean)value);
                    } else if (value instanceof Long) {
                        params.putLong(name, (Long)value);
                    } else if (value instanceof Double) {
                        params.putDouble(name, (Double)value);
                    } else if (value instanceof String) {
                        params.putString(name, (String)value);
                    } else {
                        this.logError("unsupported parameter: %s class: %s"
                            , value
                            , value.getClass()
                        );

                        return;
                    }
                }

                this.logInfo("logEvent [CUSTOM] eventName: %s params: %s"
                    , eventName
                    , params
                );

                m_logger.logEvent(eventName, params);
            } break;
            case EAET_EARN_VIRTUAL_CURRENCY: {
                //ToDo EAET_EARN_VIRTUAL_CURRENCY
            } break;
            case EAET_SPEND_VIRTUAL_CURRENCY: {
                //ToDo EAET_SPEND_VIRTUAL_CURRENCY
            } break;
            case EAET_UNLOCK_ACHIEVEMENT: {
                Bundle params = new Bundle();

                String achievementId = (String)parameters.get("@ACHIEVEMENT_ID");

                params.putString(AppEventsConstants.EVENT_PARAM_DESCRIPTION, achievementId);

                this.logInfo("logEvent [UNLOCK_ACHIEVEMENT] eventName: %s params: %s"
                    , AppEventsConstants.EVENT_NAME_UNLOCKED_ACHIEVEMENT
                    , params
                );

                m_logger.logEvent(AppEventsConstants.EVENT_NAME_UNLOCKED_ACHIEVEMENT, params);
            } break;
            case EAET_LEVEL_UP: {
                //ToDo EAET_LEVEL_UP
            }break;
            case EAET_LEVEL_START: {
                //ToDo EAET_LEVEL_START
            }break;
            case EAET_LEVEL_END: {
                //ToDo EAET_LEVEL_END
            }break;
            case EAET_SELECT_ITEM: {
                //ToDo EAET_SELECT_ITEM
            }break;
            case EAET_TUTORIAL_BEGIN: {
                //ToDo EAET_TUTORIAL_BEGIN
            }break;
            case EAET_TUTORIAL_COMPLETE: {
                //ToDo EAET_TUTORIAL_COMPLETE
            }break;
            case EAET_SCREEN_VIEW: {
                //ToDo EAET_SCREEN_VIEW
            }break;
            default: {
                this.logError("unknown event type: %d name: %s"
                	, eventType
                    , eventName
                );
            } break;
        }
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
        if (this.isLoggedIn() == false) {
            return "";
        }

        AccessToken accessToken = AccessToken.getCurrentAccessToken();

        if (accessToken == null) {
            return "";
        }
        
        String token = accessToken.getToken();
        
        return token;
    }
    
    public void performLogin(List<String> permissions) {
        this.logMessage("performLogin permissions: %s"
            , permissions
        );

        MengineActivity activity = this.getMengineActivity();

        LoginManager.getInstance().logInWithReadPermissions(activity, permissions);
    }
    
    public void logout() {
        this.logMessage("logout");

        AccessToken accessToken = AccessToken.getCurrentAccessToken();
        
        // user already logged out
        if (accessToken == null) {
            this.logWarning("user already logged out");

            this.pythonCall("onFacebookLogoutCancel");
            
            return;
        }

        String token = accessToken.getToken();

        new GraphRequest(accessToken, "/me/permissions/", null, HttpMethod.DELETE, new GraphRequest.Callback() {
            @Override
            public void onCompleted(GraphResponse graphResponse) {
                MengineFacebookPlugin.this.logMessage("GraphRequest DELETE onCompleted");

                LoginManager.getInstance().logOut();

                MengineFacebookPlugin.this.pythonCall("onFacebookLogoutSuccess", token);
            }
        }).executeAsync();
    }
    
    public void getUser() {
        this.logMessage("getUser");

        if (this.isLoggedIn() == false) {
            this.logWarning("is not logged in");

            this.pythonCall("onFacebookUserFetchSuccess", "", "");
            
            return;
        }

        AccessToken accessToken = AccessToken.getCurrentAccessToken();

        GraphRequest request = GraphRequest.newMeRequest(accessToken, new GraphRequest.GraphJSONObjectCallback() {
                @Override
                public void onCompleted(JSONObject object, GraphResponse response) {
                    MengineFacebookPlugin.this.logMessage("GraphRequest new Me request completed object: %s response: %s"
                        , object
                        , response
                    );

                    String objectString = object != null ? object.toString() : "";
                    String responseString = response != null ? response.toString() : "";

                    MengineFacebookPlugin.this.pythonCall("onFacebookUserFetchSuccess", objectString, responseString);
                }
            });
                
        Bundle parameters = new Bundle();
        parameters.putString("fields", "id, name, picture");
        request.setParameters(parameters);
        request.executeAsync();
    }
    
    public void shareLink(String link, String hashtag, String quote) {
        this.logMessage("shareLink link: %s hashtag: %s quote: %s"
            , link
            , hashtag
            , quote
        );

        if (ShareDialog.canShow(ShareLinkContent.class) == false) {
            this.logWarning("shareLink can't show");

            return;
        }

        this.buildEvent("mengine_fb_share_link")
            .addParameterString("url", link)
            .addParameterString("hashtag", hashtag)
            .addParameterString("quote", quote)
            .log();

        MengineActivity activity = this.getMengineActivity();

        ShareDialog shareDialog = new ShareDialog(activity);
        shareDialog.registerCallback(m_facebookCallbackManager, new FacebookCallback<Sharer.Result>() {
            @Override
            public void onSuccess(Sharer.Result result) {
                String postId = result.getPostId() != null ? result.getPostId() : "";

                MengineFacebookPlugin.this.logMessage("shareLink success [%s]"
                    , postId
                );

                MengineFacebookPlugin.this.buildEvent("mengine_fb_share_link_success")
                    .addParameterString("url", link)
                    .addParameterString("hashtag", hashtag)
                    .addParameterString("quote", quote)
                    .addParameterString("postId", postId)
                    .log();

                MengineFacebookPlugin.this.pythonCall("onFacebookShareSuccess", postId);
            }

            @Override
            public void onCancel() {
                MengineFacebookPlugin.this.logMessage("shareLink cancel");

                MengineFacebookPlugin.this.buildEvent("mengine_fb_share_link_cancel")
                    .addParameterString("url", link)
                    .addParameterString("hashtag", hashtag)
                    .addParameterString("quote", quote)
                    .log();

                MengineFacebookPlugin.this.pythonCall("onFacebookShareCancel");
            }

            @Override
            public void onError(@NonNull FacebookException exception) {
                String error_message = exception.getMessage();

                MengineFacebookPlugin.this.logError("shareLink error: %s"
                    , error_message
                );

                MengineFacebookPlugin.this.buildEvent("mengine_fb_share_link_error")
                    .addParameterString("url", link)
                    .addParameterString("hashtag", hashtag)
                    .addParameterString("quote", quote)
                    .addParameterString("error", error_message)
                    .log();

                MengineFacebookPlugin.this.pythonCall("onFacebookShareError", error_message);
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
    
    public void getProfilePictureLink(final String typeParameter) {
        this.logMessage("getProfilePictureLink typeParameter: %s"
            , typeParameter
        );

        if (this.isLoggedIn() == false) {
            this.logWarning("is not logged in");

            this.pythonCall("onFacebookProfilePictureLinkGet", m_facebookUserId, false, "");
            
            return;
        }
        
        this.getProfileUserPictureLink(m_facebookUserId, typeParameter);
    }
    
    public void getProfileUserPictureLink(final String user_id, final String typeParameter) {
        this.logMessage("getProfileUserPictureLink user_id: %s typeParameter: %s"
            , user_id
            , typeParameter
        );

        if (this.isLoggedIn() == false || user_id.isEmpty() == true) {
            this.logWarning("is not logged in");

            this.pythonCall("onFacebookProfilePictureLinkGet", user_id, false, "");

            return;
        }

        AccessToken accessToken = AccessToken.getCurrentAccessToken();
        
        String graphPath = "/" + user_id + "/picture" + typeParameter;
        
        GraphRequest request = GraphRequest.newGraphPathRequest(accessToken, graphPath, new GraphRequest.Callback() {
            @Override
            public void onCompleted(@NonNull GraphResponse response) {
                if (response.getError() != null) {
                    MengineFacebookPlugin.this.logError("profile user picture link [%s] get error: %s"
                        , graphPath
                        , response.getError().getErrorMessage()
                    );

                    MengineFacebookPlugin.this.pythonCall("onFacebookProfilePictureLinkGet", user_id, false, "");

                    return;
                }

                JSONObject responseObject = response.getJSONObject();

                if (responseObject == null) {
                    MengineFacebookPlugin.this.logError("profile user picture link [%s] invalid response: %s"
                        , graphPath
                        , response
                    );

                    MengineFacebookPlugin.this.pythonCall("onFacebookProfilePictureLinkGet", user_id, false, "");

                    return;
                }

                String pictureURL = "";
                try {
                    pictureURL = responseObject.getJSONObject("data").getString("url");
                } catch (JSONException e) {
                    e.printStackTrace();

                    MengineFacebookPlugin.this.logError("profile user picture link [%s] catch JSONException: %s"
                        , graphPath
                        , e.getLocalizedMessage()
                    );

                    MengineFacebookPlugin.this.pythonCall("onFacebookProfilePictureLinkGet", user_id, false, "");

                    return;
                }

                MengineFacebookPlugin.this.logMessage("request profile user [%s] picture link completed: %s"
                    , user_id
                    , pictureURL
                );

                MengineFacebookPlugin.this.pythonCall("onFacebookProfilePictureLinkGet", user_id, true, pictureURL);
            }
        });

        Bundle parameters = new Bundle();
        parameters.putBoolean("redirect", false);
        request.setParameters(parameters);
        request.executeAsync();
    }
}
