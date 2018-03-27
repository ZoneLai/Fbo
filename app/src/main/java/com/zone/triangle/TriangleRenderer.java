package com.zone.triangle;

import android.content.Context;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.opengl.GLUtils;

import java.io.IOException;
import java.io.InputStream;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * @authon 赖忠安
 * Create on 2018/3/14.
 */

public class TriangleRenderer implements GLSurfaceView.Renderer {
    private static final int MAX_WIDGET_WIDTH = 1080;
    private static final int MAX_WIDGET_HEIGHT = 1920;
    private Context mContext;
    private Bitmap mBitmap;

    static {
        System.loadLibrary("Triangle");
    }

    public TriangleRenderer(Context context) {
        mContext = context;
        nativeOnCreate();
    }

    public void setBitmap(Bitmap bitmap) {
        mBitmap = bitmap;
    }

    @Override
    public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {
//        if (mBitmap != null && !mBitmap.isRecycled()) {
//            nativeInitGL(MAX_WIDGET_WIDTH, MAX_WIDGET_HEIGHT, mBitmap.getWidth(), mBitmap.getHeight());
//            int i = nativeGetTextureId();
//            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, nativeGetTextureId());
//            GLUtils.texImage2D(GLES20.GL_TEXTURE_2D, 0, mBitmap, 0);
//        }
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        if (mBitmap != null && !mBitmap.isRecycled()) {
            nativeInitGL(MAX_WIDGET_WIDTH, MAX_WIDGET_HEIGHT, mBitmap.getWidth(), mBitmap.getHeight());
            int i = nativeGetTextureId();
            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, nativeGetTextureId());
            GLUtils.texImage2D(GLES20.GL_TEXTURE_2D, 0, mBitmap, 0);

            nativeOnSurfaceChanged(width, height);
        }
    }

    @Override
    public void onDrawFrame(GL10 gl10) {
        if (mBitmap != null && !mBitmap.isRecycled()) {
            nativeDrawFrame();
            mBitmap.recycle();
        }
    }

    public void onStop() {
        nativeOnStop();
    }

    public void onDestroy() {
        nativeOnDestroy();
    }


    private static native void nativeOnCreate();

    private static native void nativeOnResume();

    private static native void nativeOnPause();

    private static native void nativeOnStop();

    private static native void nativeOnDestroy();

    private static native void nativeInitGL(int widgetWidth, int widgetHeight, int photoWidth, int photoHeigth);

    private static native void nativeDrawFrame();

    private static native void nativeOnSurfaceChanged(int widgetWidth, int widgetHeight);

    private static native int nativeGetTextureId();

    private static native void nativeSetAssetBmp(AssetManager assetManager, String fileName);
}
