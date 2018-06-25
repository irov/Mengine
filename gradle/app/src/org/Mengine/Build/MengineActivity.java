package org.Mengine.Build;

import android.content.*;
import android.os.Bundle;

import com.facebook.CallbackManager;
import com.facebook.appevents.AppEventsLogger;

import org.Mengine.Build.Facebook.FacebookInteractionLayer;
import org.Mengine.Build.Facebook.FacebookLoginCallback;
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
        super.onCreate(savedInstanceState);

        _instance = this;

        //Facebook initialization
        AppEventsLogger.activateApp(getApplication());
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
    public static void setupLogin(final FacebookLoginCallback facebookLoginCallback) {
        if(_instance == null) {
            return;
        }
        _instance.facebookInteractionLayer.setupLogin(facebookLoginCallback);
    }

    public static void performLogin() {
        if(_instance == null) {
            return;
        }
        _instance.facebookInteractionLayer.performLogin(_instance);
    }
}
