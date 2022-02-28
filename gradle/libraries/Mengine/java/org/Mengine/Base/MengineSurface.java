package org.Mengine.Build;

import android.content.*;
import android.hardware.*;

import org.libsdl.app.SDLSurface;

class MengineSurface extends SDLSurface {
    //public static native void onNativeAccel(float x, float y, float z);
	//public static native void onNativeLinearAccel(float x, float y, float z);


    public MengineSurface(Context context) {
        super(context);
    }
	
	@Override
    public void handlePause() {
        super.handlePause();
        	
        //enableSensor(Sensor.TYPE_LINEAR_ACCELERATION, false);
    }

    @Override
    public void handleResume() {
        super.handleResume();
			
        //enableSensor(Sensor.TYPE_LINEAR_ACCELERATION, true);
    }

    @Override
    public void onSensorChanged(SensorEvent event) {
        super.onSensorChanged(event);

		/*
        if (event.sensor.getType() == Sensor.TYPE_ACCELEROMETER) {
            MengineSurface.onNativeAccel(
                    event.values[0] / SensorManager.GRAVITY_EARTH,
                    event.values[1] / SensorManager.GRAVITY_EARTH,
                    event.values[2] / SensorManager.GRAVITY_EARTH);
        }

        if (event.sensor.getType() == Sensor.TYPE_LINEAR_ACCELERATION) {
            MengineSurface.onNativeLinearAccel(
                    event.values[0] / SensorManager.GRAVITY_EARTH,
                    event.values[1] / SensorManager.GRAVITY_EARTH,
                    event.values[2] / SensorManager.GRAVITY_EARTH);
        }
		*/
    }
}