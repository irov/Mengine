package org.Mengine.Plugin.Facebook;

import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MengineActivity;

import com.facebook.AccessToken;
import com.facebook.CallbackManager;
import com.facebook.FacebookCallback;
import com.facebook.FacebookException;
import com.facebook.FacebookSdk;
import com.facebook.GraphRequest;
import com.facebook.GraphResponse;
import com.facebook.HttpMethod;
import com.facebook.LoggingBehavior;
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

import org.json.JSONException;
import org.json.JSONObject;

import java.util.Arrays;
import java.util.List;

public class MengineFacebookPlugin extends MenginePlugin {
    private CallbackManager m_facebookCallbackManager;
    private AccessToken m_facebookAccessToken;
    private String m_facebookUserId;

    @Override
    public void onPythonEmbedding(MengineActivity activity) {
        this.addPythonPlugin("Facebook");
    }

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) {
    }

    @Override
    public void onDestroy(MengineActivity activity) {
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

    public void initialize() {
        if (BuildConfig.DEBUG) {
            FacebookSdk.setIsDebugEnabled(true);
            FacebookSdk.addLoggingBehavior(LoggingBehavior.APP_EVENTS);
        }

        m_facebookCallbackManager = CallbackManager.Factory.create();

        LoginManager.getInstance().registerCallback(m_facebookCallbackManager, new FacebookCallback<LoginResult>() {
            @Override
            public void onSuccess(LoginResult loginResult) {
                m_facebookAccessToken = loginResult.getAccessToken();

                String token = m_facebookAccessToken.getToken();

                MengineFacebookPlugin.this.pythonCall("onFacebookLoginSuccess", token);
            }

            @Override
            public void onCancel() {
                MengineFacebookPlugin.this.pythonCall("onFacebookLoginCancel");
            }

            @Override
            public void onError(FacebookException exception) {
                String message = exception.getMessage();

                MengineFacebookPlugin.this.pythonCall("onFacebookLoginError", message);
            }
        });

        Application application = this.getApplication();
        AppEventsLogger.activateApp(application);
    }

    public boolean isLoggedIn() {
        return m_facebookAccessToken != null && !m_facebookAccessToken.isExpired();
    }
    
    public String getAccessToken() {
        if (this.isLoggedIn() == false) {
            return "";
        }
        
        String token = m_facebookAccessToken.getToken();
        
        return token;
    }
    
    public void performLogin(List<String> permissions) {
        MengineActivity activity = this.getActivity();

        LoginManager.getInstance().logInWithReadPermissions(activity, permissions);
    }
    
    public void logout() {
        AccessToken accessToken = AccessToken.getCurrentAccessToken();
        
        // user already logged out
        if (accessToken == null) {
            this.pythonCall("onFacebookLogoutCancel");
            
            return;
        }

        new GraphRequest(accessToken, "/me/permissions/", null, HttpMethod.DELETE, new GraphRequest.Callback() {
            @Override
            public void onCompleted(GraphResponse graphResponse) {
                String token = accessToken.getToken();

                MengineFacebookPlugin.this.pythonCall("onFacebookLogoutSuccess", token);

                LoginManager.getInstance().logOut();
                
                m_facebookAccessToken = AccessToken.getCurrentAccessToken();
            }
        }).executeAsync();
    }
    
    public void getUser() {
        if (this.isLoggedIn() == false) {
            this.pythonCall("onFacebookUserFetchSuccess", "", "");
            
            return;
        }

        GraphRequest request = GraphRequest.newMeRequest(m_facebookAccessToken, new GraphRequest.GraphJSONObjectCallback() {
                @Override
                public void onCompleted(JSONObject object, GraphResponse response) {
                    String objectString = object != null ? object.toString() : "";
                    String responseString = response != null ? response.toString() : "";

                    try {
                        if (object != null) {
                            m_facebookUserId = object.getString("id");
                        }
                    } catch (JSONException e) {
                        e.printStackTrace();
                    }

                    MengineFacebookPlugin.this.pythonCall("onFacebookUserFetchSuccess", objectString, responseString);
                }
            });
                
        Bundle parameters = new Bundle();
        parameters.putString("fields", "id, name, picture");
        request.setParameters(parameters);
        request.executeAsync();
    }
    
    public void shareLink(String link, String picture, String message) {
        MengineActivity activity = this.getActivity();

        ShareDialog shareDialog = new ShareDialog(activity);
        shareDialog.registerCallback(m_facebookCallbackManager, new FacebookCallback<Sharer.Result>() {
            @Override
            public void onSuccess(Sharer.Result result) {
                String postId = result.getPostId() != null ? result.getPostId() : "";

                MengineFacebookPlugin.this.logInfo("facebook success [%s]"
                    , postId
                );

                MengineFacebookPlugin.this.pythonCall("onFacebookShareSuccess", postId);
            }

            @Override
            public void onCancel() {
                MengineFacebookPlugin.this.logInfo("facebook cancel");

                MengineFacebookPlugin.this.pythonCall("onFacebookShareCancel");
            }

            @Override
            public void onError(FacebookException exception) {
                String message = exception.getMessage();

                MengineFacebookPlugin.this.logInfo("facebook error: %s"
                    , message
                );

                MengineFacebookPlugin.this.pythonCall("onFacebookShareError", message);
            }
        });
        
        if (ShareDialog.canShow(ShareLinkContent.class)) {
            ShareLinkContent linkContent = new ShareLinkContent.Builder()
                    .setContentUrl(Uri.parse(link))
                    .setQuote(message)
                    .build();

            shareDialog.show(linkContent);
        }
    }
    
    public void getProfilePictureLink(final String typeParameter) {
        if (this.isLoggedIn() == false) {
            this.pythonCall("onFacebookProfilePictureLinkGet", m_facebookUserId, false, "");
            
            return;
        }
        
        this.getProfileUserPictureLink(m_facebookUserId, typeParameter);
    }
    
    public void getProfileUserPictureLink(final String user_id, final String typeParameter) {
        if (user_id.isEmpty()) {
            this.pythonCall("onFacebookProfilePictureLinkGet", user_id, false, "");

            return;
        }
        
        String graphPath = "/" + user_id + "/picture" + typeParameter;
        
        GraphRequest request = GraphRequest.newGraphPathRequest(m_facebookAccessToken, graphPath, new GraphRequest.Callback() {
                @Override
                public void onCompleted(GraphResponse response) {
                    String pictureURL = "";
                    if (response != null) {
                        JSONObject responseObject = response.getJSONObject();
                        if (responseObject != null) {
                            try {
                                pictureURL = responseObject.getJSONObject("data").getString("url");
                            } catch (JSONException e) {
                                e.printStackTrace();
                            }
                        }
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
