/*
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.quake.llvm;
/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


import android.content.Context;
import android.opengl.GLSurfaceView;
import android.opengl.GLUtils;
import android.util.AttributeSet;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.graphics.Paint;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import java.io.InputStream;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.BufferedOutputStream;
import java.io.IOException;

import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.drawable.Drawable;

/**
 * An implementation of SurfaceView that uses the dedicated surface for
 * displaying an OpenGL animation.  This allows the animation to run in a
 * separate thread, without requiring that it be driven by the update mechanism
 * of the view hierarchy.
 *
 * The application-specific rendering code is delegated to a GLView.Renderer
 * instance.
 */
class QuakeView extends GLSurfaceView {
    QuakeView(Context context) {
        super(context);
        init();
    }

    public QuakeView(Context context, AttributeSet attrs) {
        super(context, attrs);
        init();
    }

    private void init() {
        // We want events.
        setFocusable(true);
        setFocusableInTouchMode(true);
        requestFocus();
       // bitmap, canvas, and paint for fps
        mPaint = new Paint();
        mPaint.setTextSize(40);
        mBitmap = Bitmap.createBitmap(200, 200, Bitmap.Config.RGB_565);
        mBitmap.eraseColor(Color.BLACK);
        mCanvas = new Canvas(mBitmap);
        mTextures = new int[1];
    }

    public void setQuakeLib(QuakeLib quakeLib) {
        mQuakeLib = quakeLib;
        setRenderer(new QuakeRenderer());
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        if (!weWantThisKeyCode(keyCode)) {
            return super.onKeyDown(keyCode, event);
        }
        switch (keyCode) {
        case KeyEvent.KEYCODE_ALT_RIGHT:
        case KeyEvent.KEYCODE_ALT_LEFT:
            mAltKeyPressed = true;
            break;
        case KeyEvent.KEYCODE_SHIFT_RIGHT:
        case KeyEvent.KEYCODE_SHIFT_LEFT:
            mShiftKeyPressed = true;
            break;
        }
        queueKeyEvent(QuakeLib.KEY_PRESS,
                keyCodeToQuakeCode(keyCode));
        return true;
    }

    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
        if (!weWantThisKeyCode(keyCode)) {
            return super.onKeyUp(keyCode, event);
        }
        switch (keyCode) {
        case KeyEvent.KEYCODE_ALT_RIGHT:
        case KeyEvent.KEYCODE_ALT_LEFT:
            mAltKeyPressed = false;
            break;
        case KeyEvent.KEYCODE_SHIFT_RIGHT:
        case KeyEvent.KEYCODE_SHIFT_LEFT:
            mShiftKeyPressed = false;
            break;        }
        queueKeyEvent(QuakeLib.KEY_RELEASE,
                keyCodeToQuakeCode(keyCode));
        return true;
    }

    @Override
    public boolean onTrackballEvent(MotionEvent event) {
        if (!mGameMode) {
            return super.onTrackballEvent(event);
        }
        queueTrackballEvent(event);
        return true;
    }

    private boolean weWantThisKeyCode(int keyCode) {
        return (keyCode != KeyEvent.KEYCODE_VOLUME_UP) &&
            (keyCode != KeyEvent.KEYCODE_VOLUME_DOWN) &&
            (keyCode != KeyEvent.KEYCODE_SEARCH) &&
            (keyCode != KeyEvent.KEYCODE_MENU);
    }

    @Override
    public boolean dispatchTouchEvent(MotionEvent ev) {
        queueMotionEvent(ev);
        return true;
    }

    private int keyCodeToQuakeCode(int keyCode) {
        int key = 0;
        if (keyCode >= sKeyCodeToQuakeCode.length) {
            return 0;
        }
        if (mAltKeyPressed) {
            key = sKeyCodeToQuakeCodeAlt[keyCode];
            if (key == 0) {
                key = sKeyCodeToQuakeCodeShift[keyCode];
                if (key == 0) {
                    key = sKeyCodeToQuakeCode[keyCode];
                }
            }
        } else if (mShiftKeyPressed) {
            key = sKeyCodeToQuakeCodeShift[keyCode];
            if (key == 0) {
                key = sKeyCodeToQuakeCode[keyCode];
            }
        } else {
            key = sKeyCodeToQuakeCode[keyCode];
        }
        if (key == 0) {
            key = '$';
        }
        return key;
    }

    public void queueKeyEvent(final int type, final int keyCode) {
        queueEvent(
            new Runnable() {
                public void run() {
                    mQuakeLib.event(type, keyCode);
                }
            });
    }

    public void queueMotionEvent(final MotionEvent ev) {
        queueEvent(
            new Runnable() {
                public void run() {
                    mQuakeLib.motionEvent(ev.getEventTime(),
                            ev.getAction(),
                            ev.getX(), ev.getY(),
                            ev.getPressure(), ev.getSize(),
                            ev.getDeviceId());
                }
            });
    }

    public void queueTrackballEvent(final MotionEvent ev) {
        queueEvent(
            new Runnable() {
                public void run() {
                    mQuakeLib.trackballEvent(ev.getEventTime(),
                            ev.getAction(),
                            ev.getX(), ev.getY());
                }
            });
    }

    byte[] pgm;
    int pgmLength;

    private boolean mode = false;
    private Paint mPaint = null;
    private Bitmap mBitmap = null;
    private Canvas mCanvas = null;
    private int[] mTextures = null;

    public void switchMode()
    {
       mode = !mode;
    }

    private boolean mShiftKeyPressed;
    private boolean mAltKeyPressed;

    private static final int[] sKeyCodeToQuakeCode = {
        '$', QuakeLib.K_ESCAPE, '$', '$',  QuakeLib.K_ESCAPE, '$', '$', '0', //  0.. 7
        '1', '2', '3', '4',  '5', '6', '7', '8', //  8..15
        '9', '$', '$', QuakeLib.K_UPARROW,  QuakeLib.K_DOWNARROW, QuakeLib.K_LEFTARROW, QuakeLib.K_RIGHTARROW, QuakeLib.K_ENTER, // 16..23
        '$', '$', '$', QuakeLib.K_HOME,  '$', 'a', 'b', 'c', // 24..31

        'd', 'e', 'f', 'g',  'h', 'i', 'j', 'k', // 32..39
        'l', 'm', 'n', 'o',  'p', 'q', 'r', 's', // 40..47
        't', 'u', 'v', 'w',  'x', 'y', 'z', ',', // 48..55
        '.', QuakeLib.K_ALT, QuakeLib.K_ALT, QuakeLib.K_SHIFT,  QuakeLib.K_SHIFT, QuakeLib.K_TAB, ' ', '$', // 56..63
        '$', '$', QuakeLib.K_ENTER, QuakeLib.K_BACKSPACE, '`', '-',  '=', '[', // 64..71
        ']', '\\', ';', '\'', '/', QuakeLib.K_CTRL,  '#', '$', // 72..79
        QuakeLib.K_HOME, '$', QuakeLib.K_ESCAPE, '$',  '$', 0, 0, 0, 0,             // 80..
        0, 0, QuakeLib.K_PGUP, QuakeLib.K_PGDN, 0,  0, 0, 0, 0, 0,             // 90..
        0, 0, 0, 0, 0,  0, 0, 0, 0, 0,             // 100..
        0, QuakeLib.K_ESCAPE, QuakeLib.K_DEL, QuakeLib.K_CTRL, QuakeLib.K_CTRL,  0, 0, 0, 0, 0,             // 110..
        0, 0, QuakeLib.K_HOME, QuakeLib.K_END, QuakeLib.K_INS,  0, 0, 0, 0, 0,             // 120..
        0, QuakeLib.K_F1, QuakeLib.K_F2, QuakeLib.K_F3, QuakeLib.K_F4,                    // 130..134
        QuakeLib.K_F5, QuakeLib.K_F6, QuakeLib.K_F7, QuakeLib.K_F8, QuakeLib.K_F9,        // 135..139
        QuakeLib.K_F10, QuakeLib.K_F11, QuakeLib.K_F12, 0, 0,  0, 0, 0, 0, 0,             // 140..
        0, 0, 0, 0, 0,  0, 0, 0, 0, 0,             // 150..
        0, 0, 0, 0, 0,  0, 0, 0, 0, 0,             // 160
        0, 0, 0, 0, 0,  0, 0, 0, 0, 0,             // 170..
        0, 0, 0, 0, 0,  0, 0, 0, 0, 0              // 180..
    };

    private static final int sKeyCodeToQuakeCodeShift[] =
    {
        0, 0, 0, 0,  0, 0, 0, ')', //  0.. 7
        '!', '@', '#', '$',  '%', '^', '&', '*', //  8..15
        '(', 0, 0, 0,  0, 0, 0, 0, // 16..23
        0, 0, 0, 0,  0, 0, ']', 0, // 24..31

        '\\', '_', '{', '}',  ':', '-', ';', '"', // 32..39
        '\'', '>', '<', '+',  '=', 0, 0, '|', // 40..47
        0, 0, '[', '`',  0, 0, QuakeLib.K_PAUSE, ';', // 48..55
        0, 0, 0, 0,  0, 0, 0, 0, // 56..63
        0, 0, 0, 0,  0, 0, 0, 0, // 64..71
        0, 0, '?', '0',  0, QuakeLib.K_CTRL, 0, 0, // 72..79
        0, 0, 0, 0, 0,  0, 0, 0, 0, 0,             // 80..
        0, 0, QuakeLib.K_PGUP, QuakeLib.K_PGDN, 0,  0, 0, 0, 0, 0,             // 90..
        0, 0, 0, 0, 0,  0, 0, 0, 0, 0,             // 100..
        0, QuakeLib.K_ESCAPE, QuakeLib.K_DEL, QuakeLib.K_CTRL, QuakeLib.K_CTRL,  0, 0, 0, 0, 0,             // 110..
        0, 0, QuakeLib.K_HOME, QuakeLib.K_END, QuakeLib.K_INS,  0, 0, 0, 0, 0,             // 120..
        0, QuakeLib.K_F1, QuakeLib.K_F2, QuakeLib.K_F3, QuakeLib.K_F4,                    // 130..134
        QuakeLib.K_F5, QuakeLib.K_F6, QuakeLib.K_F7, QuakeLib.K_F8, QuakeLib.K_F9,        // 135..139
        QuakeLib.K_F10, QuakeLib.K_F11, QuakeLib.K_F12, 0, 0,  0, 0, 0, 0, 0,             // 140..
        0, 0, 0, 0, 0,  0, 0, 0, 0, 0,             // 150..
        0, 0, 0, 0, 0,  0, 0, 0, 0, 0,             // 160
        0, 0, 0, 0, 0,  0, 0, 0, 0, 0,             // 170..
        0, 0, 0, 0, 0,  0, 0, 0, 0, 0              // 180..
    };

    private static final int sKeyCodeToQuakeCodeAlt[] =
    {
        0, 0, 0, 0,  0, 0, 0, QuakeLib.K_F10, //  0.. 7
        QuakeLib.K_F1, QuakeLib.K_F2, QuakeLib.K_F3, QuakeLib.K_F4,  QuakeLib.K_F5, QuakeLib.K_F6, QuakeLib.K_F7, QuakeLib.K_F8, //  8..15
        QuakeLib.K_F9, 0, 0, 0,  0, 0, 0, 0, // 16..23
        0, 0, 0, 0,  0, 0, 0, 0, // 24..31

        0, 0, 0, 0,  0, 0, 0, 0, // 32..39
        0, 0, 0, 0,  0, 0, 0, 0, // 40..47
        QuakeLib.K_F11, 0, 0, 0,  0, QuakeLib.K_F12, 0, 0, // 48..55
        0, 0, 0, 0,  0, 0, 0, 0, // 56..63
        0, 0, 0, 0,  0, 0, 0, 0, // 64..71
        0, 0, 0, 0,  0, 0, 0, 0, // 72..79
        0, 0, 0, 0,  0, 0, 0, 0, 0,             // 80..
        0, 0, QuakeLib.K_PGUP, QuakeLib.K_PGDN, 0,  0, 0, 0, 0, 0,             // 90..
        0, 0, 0, 0, 0,  0, 0, 0, 0, 0,             // 100..
        0, QuakeLib.K_ESCAPE, QuakeLib.K_DEL, QuakeLib.K_CTRL, QuakeLib.K_CTRL,  0, 0, 0, 0, 0,             // 110..
        0, 0, QuakeLib.K_HOME, QuakeLib.K_END, QuakeLib.K_INS,  0, 0, 0, 0, 0,             // 120..
        0, QuakeLib.K_F1, QuakeLib.K_F2, QuakeLib.K_F3, QuakeLib.K_F4,                    // 130..134
        QuakeLib.K_F5, QuakeLib.K_F6, QuakeLib.K_F7, QuakeLib.K_F8, QuakeLib.K_F9,        // 135..139
        QuakeLib.K_F10, QuakeLib.K_F11, QuakeLib.K_F12, 0, 0,  0, 0, 0, 0, 0,             // 140..
        0, 0, 0, 0, 0,  0, 0, 0, 0, 0,             // 150..
        0, 0, 0, 0, 0,  0, 0, 0, 0, 0,             // 160
        0, 0, 0, 0, 0,  0, 0, 0, 0, 0,             // 170..
        0, 0, 0, 0, 0,  0, 0, 0, 0, 0             // 180..
    };

    private class QuakeRenderer implements GLSurfaceView.Renderer {
        private static final String TAG = "QuakeRenderer";
        public void onDrawFrame(GL10 gl) {
            if (mWidth != 0 &&  mHeight != 0) {
                mGameMode = mQuakeLib.step(mWidth, mHeight);
	       // ToDo: draw fps
            if (false) {
	      ////ToDo: the following doesn't work
                mCanvas.drawText((mode ? "LLVM" : "GCC") + " Frame: " + Integer.toString(20), 100, 100, mPaint);
              //Generate one texture pointer...
              //gl.glDeleteTextures(1, mTextures, 1);
                gl.glGenTextures(1, mTextures, 0);
              //...and bind it to our array
                gl.glBindTexture(GL10.GL_TEXTURE_2D, mTextures[0]);
              //Create Nearest Filtered Texture
                gl.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_MIN_FILTER, GL10.GL_NEAREST);
                gl.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_MAG_FILTER, GL10.GL_LINEAR);
              //Different possible texture parameters, e.g. GL10.GL_CLAMP_TO_EDGE
                gl.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_WRAP_S, GL10.GL_REPEAT);
                gl.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_WRAP_T, GL10.GL_REPEAT);
              //Use the Android GLUtils to specify a two-dimensional texture image from our bitmap
                GLUtils.texImage2D(GL10.GL_TEXTURE_2D, 0, mBitmap, 0);
              //Clean up
                //mBitmap.recycle();
            } else if (false) {
	      ////ToDo: the following doesn't work either
              //Create an empty, mutable bitmap
                Bitmap bitmap = Bitmap.createBitmap(256, 256, Bitmap.Config.ARGB_4444);
              //get a canvas to paint over the bitmap
                Canvas canvas = new Canvas(bitmap);
                bitmap.eraseColor(0);
              //get a background image from resources
              //note the image format must match the bitmap format
              //Drawable background = context.getResources().getDrawable(R.drawable.background);
              //background.setBounds(0, 0, 256, 256);
              //background.draw(canvas); // draw the background to our bitmap
              //Draw the text
                Paint textPaint = new Paint();
                textPaint.setTextSize(32);
                textPaint.setAntiAlias(true);
                textPaint.setARGB(0xff, 0x00, 0x00, 0x00);
              //draw the text centered
                canvas.drawText("Hello World", 16,112, textPaint);
              //Generate one texture pointer...
                gl.glGenTextures(1, mTextures, 0);
              //...and bind it to our array
                gl.glBindTexture(GL10.GL_TEXTURE_2D, mTextures[0]);
              //Create Nearest Filtered Texture
                gl.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_MIN_FILTER, GL10.GL_NEAREST);
                gl.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_MAG_FILTER, GL10.GL_LINEAR);
              //Different possible texture parameters, e.g. GL10.GL_CLAMP_TO_EDGE
                gl.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_WRAP_S, GL10.GL_REPEAT);
                gl.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_WRAP_T, GL10.GL_REPEAT);
              //Use the Android GLUtils to specify a two-dimensional texture image from our bitmap
                GLUtils.texImage2D(GL10.GL_TEXTURE_2D, 0, bitmap, 0);
              //Clean up
                bitmap.recycle();
		}
            }
        }

        public void onSurfaceChanged(GL10 gl, int width, int height) {
            mWidth = width;
            mHeight = height;
            mQuakeLib.init();
        }

        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
            // Do nothing.
        }
        private int mWidth;
        private int mHeight;
    }

    private QuakeLib mQuakeLib;
    private boolean mGameMode;
}

