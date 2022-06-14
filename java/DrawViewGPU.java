package view;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.PixelFormat;
import android.graphics.PorterDuff;
import android.view.SurfaceHolder;
import android.view.SurfaceView;



@SuppressWarnings("all")
public class DrawViewGPU extends SurfaceView implements DrawViewInter {

    SurfaceHolder holder;


    public DrawViewGPU(Context context) {
        super(context);
        holder = this.getHolder();
        setZOrderOnTop(true);
        holder.setFormat(PixelFormat.TRANSLUCENT);
    }

    @Override
    public Canvas lockCanvas() {
        return holder.lockHardwareCanvas();
    }

    @Override
    public void clear() {
        Canvas canvas = lockCanvas();
        if (canvas != null) {
            canvas.drawColor(Color.TRANSPARENT, PorterDuff.Mode.CLEAR);
            holder.unlockCanvasAndPost(canvas);
        }
    }

    @Override
    public void unlockCanvasAndPost(Canvas canvas) {
        if (canvas != null) {
            holder.unlockCanvasAndPost(canvas);
            canvas = null;
        }
    }
}
