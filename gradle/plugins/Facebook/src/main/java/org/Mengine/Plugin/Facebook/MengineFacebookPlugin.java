package org.Mengine.Plugin.Facebook;

import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MengineActivity;

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
import com.facebook.login.LoginManager;
import com.facebook.login.LoginResult;
import com.facebook.share.Sharer;
import com.facebook.share.model.ShareLinkContent;
import com.facebook.share.widget.ShareDialog;
import com.facebook.appevents.AppEventsLogger;

import android.app.Application;
import android.content.Context;
import android.net.Uri;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;

import androidx.annotation.NonNull;

import org.Mengine.Base.MenginePluginAnalyticsListener;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.Arrays;
import java.util.List;
import java.util.Map;

public class MengineFacebookPlugin extends MenginePlugin implements MenginePluginAnalyticsListener {
    public static String PLUGIN_NAME = "Facebook";
    public static boolean PLUGIN_EMBEDDING = true;

    private CallbackManager m_facebookCallbackManager;
    private AccessTokenTracker m_accessTokenTracker;
    private ProfileTracker m_profileTracker;
    private AppEventsLogger m_logger;

    private String m_facebookUserId;

    @Override
    public void onEvent(MengineActivity activity, String id, Object ... args) {
        if (id.equals("PushToken") == true) {
            String token = (String)args[0];

            AppEventsLogger.setPushNotificationsRegistrationId(token);
        }
    }

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) {
        if (BuildConfig.DEBUG) {
            FacebookSdk.setIsDebugEnabled(true);
            FacebookSdk.addLoggingBehavior(LoggingBehavior.APP_EVENTS);
        }

        LoginManager.getInstance().retrieveLoginStatus(activity, new LoginStatusCallback() {
            @Override
            public void onCompleted(@NonNull AccessToken accessToken) {
                // User was previously logged in, can log them in directly here.
                // If this callback is called, a popup notification appears that says
                // "Logged in as <User Name>"

                MengineFacebookPlugin.this.logInfo("retrieve login [onCompleted] application: %s user: %s token: %s"
                    , accessToken.getApplicationId()
                    , accessToken.getUserId()
                    , accessToken.getToken()
                );

                MengineFacebookPlugin.this.activateSemaphore("FacebookRetrieveLoginCompleted");
            }

            @Override
            public void onFailure() {
                MengineFacebookPlugin.this.logInfo("retrieve login [onFailure]");
            }

            @Override
            public void onError(@NonNull Exception e) {
                MengineFacebookPlugin.this.logInfo("retrieve login [onError] exception: %s"
                    , e.getLocalizedMessage()
                );
            }
        });

        m_facebookCallbackManager = CallbackManager.Factory.create();

        m_accessTokenTracker = new AccessTokenTracker() {
            @Override
            protected void onCurrentAccessTokenChanged(AccessToken oldToken, AccessToken newToken) {
                String oldTokenString = oldToken != null ? oldToken.getToken() : "";
                String newTokenString = newToken != null ? newToken.getToken() : "";

                MengineFacebookPlugin.this.pythonCall("onFacebookCurrentAccessTokenChanged", oldTokenString, newTokenString);
            }
        };

        m_accessTokenTracker.startTracking();

        m_profileTracker = new ProfileTracker() {
            @Override
            protected void onCurrentProfileChanged(Profile oldProfile, Profile newProfile) {
                MengineFacebookPlugin.this.pythonCall("onFacebookCurrentProfileChanged");
            }
        };

        m_profileTracker.startTracking();

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
                    , token
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
                String message = e.getLocalizedMessage();

                MengineFacebookPlugin.this.logError("login [onError] exception: %s"
                    , message
                );

                AccessToken.setCurrentAccessToken(null);

                MengineFacebookPlugin.this.pythonCall("onFacebookLoginError", message);
            }
        });

        try {
            MengineApplication application = this.getApplication();
            AppEventsLogger.activateApp(application);
        } catch (Exception ex) {
            MengineFacebookPlugin.this.logError("activateApp catch exception: %s"
                , ex.getLocalizedMessage()
            );
        }

        m_logger = AppEventsLogger.newLogger(activity);
    }

    @Override
    public void onDestroy(MengineActivity activity) {
        m_accessTokenTracker.stopTracking();
        m_accessTokenTracker = null;

        m_profileTracker.stopTracking();
        m_profileTracker = null;

        LoginManager.getInstance().unregisterCallback(m_facebookCallbackManager);
        m_facebookCallbackManager = null;
    }

    @Override
    public void onActivityResult(MengineActivity activity, int requestCode, int resultCode, Intent data) {
        m_facebookCallbackManager.onActivityResult(requestCode, resultCode, data);
    }

    @Override
    public void onMengineAnalyticsEvent(MengineActivity activity, int eventType, String eventName, long timestamp, Map<String, Object> parameters) {
        switch (eventType) {
            case EAET_CUSTOM: {
                Bundle params = new Bundle();

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
                        this.logError("unsupported parameter [%s] %s"
                        	, value.getClass()
                            , value
                        );

                        return;
                    }
                }

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

                String achievementId = (String)parameters.get("@INTERNAL_ACHIEVEMENT_ID");

                params.putString(AppEventsConstants.EVENT_PARAM_DESCRIPTION, achievementId);

                m_logger.logEvent(AppEventsConstants.EVENT_NAME_UNLOCKED_ACHIEVEMENT, params);
            } break;
            default: {
                this.logError("Unknown event type: %d name: %s"
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
        this.logInfo("performLogin permissions: %s"
            , permissions
        );

        MengineActivity activity = this.getActivity();

        LoginManager.getInstance().logInWithReadPermissions(activity, permissions);
    }
    
    public void logout() {
        this.logInfo("logout");

        AccessToken accessToken = AccessToken.getCurrentAccessToken();
        
        // user already logged out
        if (accessToken == null) {
            this.logInfo("user already logged out");

            this.pythonCall("onFacebookLogoutCancel");
            
            return;
        }

        String token = accessToken.getToken();

        new GraphRequest(accessToken, "/me/permissions/", null, HttpMethod.DELETE, new GraphRequest.Callback() {
            @Override
            public void onCompleted(GraphResponse graphResponse) {
                MengineFacebookPlugin.this.logInfo("GraphRequest DELETE onCompleted");

                LoginManager.getInstance().logOut();

                MengineFacebookPlugin.this.pythonCall("onFacebookLogoutSuccess", token);
            }
        }).executeAsync();
    }
    
    public void getUser() {
        this.logInfo("getUser");

        if (this.isLoggedIn() == false) {
            this.logError("is not logged in");

            this.pythonCall("onFacebookUserFetchSuccess", "", "");
            
            return;
        }

        AccessToken accessToken = AccessToken.getCurrentAccessToken();

        GraphRequest request = GraphRequest.newMeRequest(accessToken, new GraphRequest.GraphJSONObjectCallback() {
                @Override
                public void onCompleted(JSONObject object, GraphResponse response) {
                    MengineFacebookPlugin.this.logInfo("GraphRequest new Me request completed");

                    String objectString = object != null ? object.toString() : "";
                    String responseString = response != null ? response.toString() : "";

                    try {
                        if (object != null) {
                            m_facebookUserId = object.getString("id");
                        }
                    } catch (JSONException e) {
                        e.printStackTrace();

                        MengineFacebookPlugin.this.logError("GraphRequest new Me request exception: %s"
                            , e.getLocalizedMessage()
                        );
                    }

                    MengineFacebookPlugin.this.pythonCall("onFacebookUserFetchSuccess", objectString, responseString);
                }
            });
                
        Bundle parameters = new Bundle();
        parameters.putString("fields", "id, name, picture");
        request.setParameters(parameters);
        request.executeAsync();
    }
    
    public void shareLink(String link, String picture, String quote) {
        this.logInfo("shareLink link: %s picture: %s quote: %s"
            , link
            , picture
            , quote
        );

        if (ShareDialog.canShow(ShareLinkContent.class) == false) {
            this.logWarning("shareLink can't show");

            return;
        }

        this.buildEvent("fb_share_link")
            .addParameterString("url", link)
            .addParameterString("picture", picture)
            .addParameterString("quote", quote)
            .log();

        MengineActivity activity = this.getActivity();

        ShareDialog shareDialog = new ShareDialog(activity);
        shareDialog.registerCallback(m_facebookCallbackManager, new FacebookCallback<Sharer.Result>() {
            @Override
            public void onSuccess(Sharer.Result result) {
                String postId = result.getPostId() != null ? result.getPostId() : "";

                MengineFacebookPlugin.this.logInfo("shareLink success [%s]"
                    , postId
                );

                MengineFacebookPlugin.this.buildEvent("fb_share_link_success")
                    .addParameterString("url", link)
                    .addParameterString("picture", picture)
                    .addParameterString("quote", quote)
                    .addParameterString( "postId", postId)
                    .log();

                MengineFacebookPlugin.this.pythonCall("onFacebookShareSuccess", postId);
            }

            @Override
            public void onCancel() {
                MengineFacebookPlugin.this.logInfo("shareLink cancel");

                MengineFacebookPlugin.this.buildEvent("fb_share_link_cancel")
                    .addParameterString("url", link)
                    .addParameterString("picture", picture)
                    .addParameterString("quote", quote)
                    .log();

                MengineFacebookPlugin.this.pythonCall("onFacebookShareCancel");
            }

            @Override
            public void onError(@NonNull FacebookException exception) {
                String error_message = exception.getMessage();

                MengineFacebookPlugin.this.logInfo("shareLink error: %s"
                    , error_message
                );

                MengineFacebookPlugin.this.buildEvent("fb_share_link_error")
                    .addParameterString("url", link)
                    .addParameterString("picture", picture)
                    .addParameterString("quote", quote)
                    .addParameterString("error", error_message)
                    .log();

                MengineFacebookPlugin.this.pythonCall("onFacebookShareError", error_message);
            }
        });

        Uri url = Uri.parse(link);

        ShareLinkContent linkContent = new ShareLinkContent.Builder()
            .setContentUrl(url)
            .setQuote(quote)
            .build();

        shareDialog.show(linkContent);
    }
    
    public void getProfilePictureLink(final String typeParameter) {
        this.logInfo("getProfilePictureLink typeParameter: %s"
            , typeParameter
        );

        if (this.isLoggedIn() == false) {
            this.logError("is not logged in");

            this.pythonCall("onFacebookProfilePictureLinkGet", m_facebookUserId, false, "");
            
            return;
        }
        
        this.getProfileUserPictureLink(m_facebookUserId, typeParameter);
    }
    
    public void getProfileUserPictureLink(final String user_id, final String typeParameter) {
        this.logInfo("getProfileUserPictureLink user_id: %s typeParameter: %s"
            , user_id
            , typeParameter
        );

        if (this.isLoggedIn() == false || user_id.isEmpty() == true) {
            this.logError("is not logged in");

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
                    MengineFacebookPlugin.this.logError("profile user picture link [%s] invalid response [%s]"
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

                    MengineFacebookPlugin.this.logError("profile user picture link [%s] catch JSONException [%s]"
                        , graphPath
                        , e.getLocalizedMessage()
                    );

                    MengineFacebookPlugin.this.pythonCall("onFacebookProfilePictureLinkGet", user_id, false, "");

                    return;
                }

                MengineFacebookPlugin.this.logInfo("request profile user [%s] picture link completed: %s"
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
