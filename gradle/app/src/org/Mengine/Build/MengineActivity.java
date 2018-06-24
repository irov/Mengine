package org.Mengine.Build;

import android.content.*;

import org.libsdl.app.SDLActivity;
import org.libsdl.app.SDLSurface;

public class MengineActivity extends SDLActivity {
    @Override
    protected String[] getLibraries() {
        return new String[] {
            "SDL2",
            "SDLApplication"
        };
    }
    
    @Override
    protected SDLSurface createSDLSurface(Context context) {
        return new MengineSurface(context);
    }
}
