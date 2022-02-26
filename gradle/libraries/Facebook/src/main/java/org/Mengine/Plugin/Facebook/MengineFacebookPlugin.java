package org.Mengine.Plugin.Facebook;

import org.Mengine.Build.MenginePlugin;
import org.Mengine.Build.MengineActivity;

import com.facebook.AccessToken;
import com.facebook.CallbackManager;
import com.facebook.FacebookCallback;
import com.facebook.FacebookException;
import com.facebook.GraphRequest;
import com.facebook.GraphResponse;
import com.facebook.HttpMethod;
import com.facebook.login.LoginManager;
import com.facebook.login.LoginResult;
import com.facebook.share.Sharer;
import com.facebook.share.model.ShareLinkContent;
import com.facebook.share.widget.ShareDialog;
import com.facebook.appevents.AppEventsLogger;

import android.app.Application;
import android.net.Uri;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;

import org.json.JSONException;
import org.json.JSONObject;

import java.util.Arrays;
import java.util.List;

public class MengineFacebookPlugin extends MenginePlugin {
    private static final String EMAIL = "email";
    private static final String PROFILE = "public_profile";
    
    private CallbackManager m_facebookCallbackManager;
    private AccessToken m_facebookAccessToken;
    private String m_facebookUserId;

    @Override
    public void onPythonEmbedding(MengineActivity activity) {
        this.addPythonPlugin("Facebook");
    }
    
    public boolean isLoggedIn() {
        return m_facebookAccessToken != null && !m_facebookAccessToken.isExpired();
    }
    
    public String getAccessToken() {
        if (!this.isLoggedIn()){
            return "";
        }
        
        String token = m_facebookAccessToken.getToken();
        
        return token;
    }
    
    public void performLogin(String[] readPermissions) {
        MengineActivity activity = this.getActivity();

        List<String> permissions;
        if (readPermissions == null) {
            permissions = Arrays.asList(PROFILE, EMAIL);
        } else {
            permissions = Arrays.asList(readPermissions);
        }
        
        LoginManager.getInstance().logInWithReadPermissions(activity, permissions);
    }
    
    public void logout() {
        AccessToken accessToken = AccessToken.getCurrentAccessToken();
        
        // user already logged out
        if (accessToken == null) {
            this.pythonCall("onFacebookLogoutCancel");
            
            return;
        }

        MengineFacebookPlugin self = this;

        new GraphRequest(accessToken, "/me/permissions/", null, HttpMethod.DELETE, new GraphRequest.Callback() {
            @Override
            public void onCompleted(GraphResponse graphResponse) {
                String token = accessToken.getToken();

                self.pythonCall("onFacebookLogoutSuccess", token);

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

        MengineFacebookPlugin self = this;
        
        GraphRequest request = GraphRequest.newMeRequest(m_facebookAccessToken
            , new GraphRequest.GraphJSONObjectCallback() {
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
                
                        self.pythonCall("onFacebookUserFetchSuccess", objectString, responseString);
                    }
                }
            );
                
        Bundle parameters = new Bundle();
        parameters.putString("fields", "id, name, picture");
        request.setParameters(parameters);
        request.executeAsync();
    }
    
    public void shareLink(String link, String picture, String message) {
        MengineActivity activity = this.getActivity();

        MengineFacebookPlugin self = this;

        ShareDialog shareDialog = new ShareDialog(activity);
        shareDialog.registerCallback(m_facebookCallbackManager, new FacebookCallback<Sharer.Result>() {
            @Override
            public void onSuccess(Sharer.Result result) {
                String postId = result.getPostId() != null ? result.getPostId() : "";
                
                self.pythonCall("onFacebookShareSuccess", postId);
            }

            @Override
            public void onCancel() {
                self.pythonCall("onFacebookShareCancel");
            }

            @Override
            public void onError(FacebookException exception) {
                String message = exception.getMessage();
                self.pythonCall("onFacebookShareError", message);
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
        if (!this.isLoggedIn()) {
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

        MengineFacebookPlugin self = this;
        
        String graphPath = "/" + user_id + "/picture" + typeParameter;
        
        GraphRequest request = GraphRequest.newGraphPathRequest(
            m_facebookAccessToken, graphPath, new GraphRequest.Callback() {
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
                        
                        self.pythonCall("onFacebookProfilePictureLinkGet", user_id, true, pictureURL);
                    }
                });

        Bundle parameters = new Bundle();
        parameters.putBoolean("redirect", false);
        request.setParameters(parameters);
        request.executeAsync();
    }
    
    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) {
        m_facebookCallbackManager = CallbackManager.Factory.create();

        MengineFacebookPlugin self = this;
        
        LoginManager.getInstance().registerCallback(m_facebookCallbackManager, new FacebookCallback<LoginResult>() {
            @Override
            public void onSuccess(LoginResult loginResult) {
                m_facebookAccessToken = loginResult.getAccessToken();
                
                String token = m_facebookAccessToken.getToken();
                
                self.pythonCall("onFacebookLoginSuccess", token);
            }

            @Override
            public void onCancel() {
                self.pythonCall("onFacebookLoginCancel");
            }

            @Override
            public void onError(FacebookException exception) {
                String message = exception.getMessage();

                self.pythonCall("onFacebookLoginError", message);
            }
        });
        
        Application application = activity.getApplication();
        
        AppEventsLogger.activateApp(application);
    }
    
    @Override
    public void onActivityResult(MengineActivity activity, int requestCode, int resultCode, Intent data) {
        m_facebookCallbackManager.onActivityResult(requestCode, resultCode, data);
    }

    @Override
    public void onMengineInitializeBaseServices(MengineActivity activity)
    {
    }

    @Override
    public void onMengineCreateApplication(MengineActivity activity)
    {
    }
}
