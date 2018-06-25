package org.Mengine.Build;

import android.content.*;
import android.os.Bundle;

import com.facebook.CallbackManager;
import com.facebook.appevents.AppEventsLogger;

import org.Mengine.Build.Facebook.FacebookAccessTokenWrapper;
import org.Mengine.Build.Facebook.FacebookInteractionLayer;
import org.Mengine.Build.Facebook.FacebookLoginCallback;
import org.Mengine.Build.Facebook.FacebookShareCallback;
import org.Mengine.Build.Facebook.FacebookUserCallback;
import org.libsdl.app.SDLActivity;
import org.libsdl.app.SDLSurface;

public class MengineActivity extends SDLActivity {

    public FacebookInteractionLayer facebookInteractionLayer;

    private static MengineActivity _instance;
    private CallbackManager _callbackManager;

    @Override
    protected String[] getLibraries() {
        return new String[]{
                "SDL2",
                "SDLApplication"
        };
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        //Facebook initialization should be done before setContentView
        AppEventsLogger.activateApp(getApplication());

        super.onCreate(savedInstanceState);

        _instance = this;

        _callbackManager = CallbackManager.Factory.create();
        facebookInteractionLayer = new FacebookInteractionLayer(_callbackManager);
    }

    @Override
    protected void onStop() {
        super.onStop();

        _instance = null;
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        _callbackManager.onActivityResult(requestCode, resultCode, data);
    }

    @Override
    protected SDLSurface createSDLSurface(Context context) {
        return new MengineSurface(context);
    }

    //FacebookStaticMethods
    public static void performLogin(final FacebookLoginCallback facebookLoginCallback) {
        if(_instance == null) {
            return;
        }
        _instance.facebookInteractionLayer.performLogin(_instance, facebookLoginCallback);
    }

    public static void getUser(FacebookAccessTokenWrapper accessTokenWrapper, FacebookUserCallback facebookUserCallback) {
        if(_instance == null) {
            return;
        }
        if(accessTokenWrapper == null) {
            _instance.facebookInteractionLayer.getUser(facebookUserCallback);
        } else {
            _instance.facebookInteractionLayer.getUser(accessTokenWrapper, facebookUserCallback);
        }
    }

    public static void shareLink(String link, FacebookShareCallback facebookShareCallback) {
        if(_instance == null) {
            return;
        }
        _instance.facebookInteractionLayer.shareLink(_instance, link, facebookShareCallback);
    }
}
