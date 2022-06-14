package view;


import android.annotation.SuppressLint;
import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.PixelFormat;
import android.graphics.PorterDuff;
import android.graphics.Rect;
import android.os.Build;



import android.view.Gravity;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.WindowManager;


import java.io.BufferedReader;
import java.util.HashMap;
import java.util.Locale;
import java.util.Map;


public class DrawManage {
    private static final String TAG = "MoveView_g";
    private WindowManager windowManager = null;
    private WindowManager.LayoutParams viewParam = null;
    Context context;

    Paint namePaint, backPaint, textPaint, testPain, hppaint, catPaintRed;
    Paint linePaint, boxPaint, hpBoxPaint, headPaint, fpsPaint;
    Paint tempPaint, botCountPaint, heroCountPaint;

    private BufferedReader br = null;

    private boolean isZh = true; //系统语言是否为中文

    DrawViewInter drawView;

    @SuppressLint("WrongConstant")
    public DrawManage(Context context) {
        this.context = context;
        initItem();

        isZh = isZh();

        windowManager = (WindowManager) context.getSystemService(Context.WINDOW_SERVICE);
        viewParam = new WindowManager.LayoutParams();
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O_MR1) {
            viewParam.type = WindowManager.LayoutParams.TYPE_APPLICATION_OVERLAY;
        } else {
            viewParam.type = WindowManager.LayoutParams.TYPE_SYSTEM_ERROR;
        }

        viewParam.format = PixelFormat.RGBA_8888;
        viewParam.flags = WindowManager.LayoutParams.FLAG_NOT_TOUCHABLE | WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE;

        viewParam.gravity = Gravity.LEFT | Gravity.TOP;
        viewParam.x = 0;
        viewParam.y = 0;
        viewParam.width = (int) AppData.width;
        viewParam.height = (int) AppData.height;

        // 设置屏幕填充
        ScreenUtil.setScreenFull(viewParam);
        // 设置禁止录屏
        ScreenUtil.setNoScreenRecoding(viewParam);

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            drawView = new DrawViewGPU(context);
        }else {
            drawView = new DrawViewCPU(context);
        }

        windowManager.addView((View) drawView, viewParam);

        linePaint = new Paint();
        linePaint.setStyle(Paint.Style.STROKE);
        linePaint.setStrokeWidth(AppData.lineStroke);
        linePaint.setAntiAlias(true);
        linePaint.setColor(Color.WHITE);

        //boxColor
        boxPaint = new Paint();
        boxPaint.setStyle(Paint.Style.STROKE);
        boxPaint.setStrokeWidth(AppData.boxStroke);
        boxPaint.setAntiAlias(true);
        boxPaint.setColor(Color.WHITE);


        hpBoxPaint = new Paint();
        hpBoxPaint.setStyle(Paint.Style.STROKE);
        hpBoxPaint.setStrokeWidth(AppData.hpBoxStroke);
        hpBoxPaint.setAntiAlias(true);
        hpBoxPaint.setColor(Color.BLACK);

        headPaint = new Paint();
        headPaint.setColor(Color.GREEN);
        headPaint.setStyle(Paint.Style.FILL);
        headPaint.setAntiAlias(true);

        catPaintRed = new Paint();
        catPaintRed.setTextSize(AppData.resourcesTextSize);
        catPaintRed.setAntiAlias(true);
        catPaintRed.setColor(AppData.resourcesTextColor);

        tempPaint = new Paint();
        tempPaint.setTextSize(25);
        tempPaint.setAntiAlias(true);
        tempPaint.setAlpha(80);
        tempPaint.setColor(Color.WHITE);

        //distanceTextSize
        textPaint = new Paint();
        textPaint.setTextSize(AppData.distanceTextSize);
        textPaint.setAntiAlias(true);
        textPaint.setAlpha(80);
        textPaint.setColor(Color.WHITE);

        //nameTextSzie
        //nameTextColor

        namePaint = new Paint();
        namePaint.setTextSize(AppData.nameTextSzie);
        namePaint.setAntiAlias(true);
        namePaint.setAlpha(80);
        namePaint.setColor(AppData.nameTextColor);

        hppaint = new Paint();
        hppaint.setStyle(Paint.Style.FILL);
        hppaint.setAntiAlias(true);
        hppaint.setStrokeWidth(0.9F);
        //hppaint.setARGB(170, 02, 255, 50);
        hppaint.setColor(AppData.hpColor);

        //playerCountTextColor playerCountTextSize
        testPain = new Paint();
        testPain.setTextSize(AppData.playerCountTextSize);
        testPain.setAntiAlias(true);
        //testPain.setColor(Color.RED);
        testPain.setColor(AppData.playerCountTextColor);

        fpsPaint = new Paint();
        fpsPaint.setTextSize(25);
        fpsPaint.setAntiAlias(true);
        fpsPaint.setColor(Color.RED);

        botCountPaint = new Paint();
        botCountPaint.setStyle(Paint.Style.FILL);
        botCountPaint.setAntiAlias(true);
        botCountPaint.setColor(AppData.botBoxColor);

        heroCountPaint = new Paint();
        heroCountPaint.setStyle(Paint.Style.FILL);
        heroCountPaint.setAntiAlias(true);
        heroCountPaint.setColor(AppData.boxColor);

        backPaint = new Paint();
        backPaint.setStyle(Paint.Style.FILL);
        backPaint.setAntiAlias(true);
        backPaint.setColor(AppData.boxColor);

//        holder = this.getHolder();
        // holder.addCallback(this);

        /*画布背景透明*/
//        setZOrderOnTop(true);
//        holder.setFormat(PixelFormat.TRANSLUCENT);

    }

    public void updateSetting() {
        linePaint.setStrokeWidth(AppData.lineStroke);
        boxPaint.setStrokeWidth(AppData.boxStroke);
        catPaintRed.setTextSize(AppData.resourcesTextSize);
        hpBoxPaint.setStrokeWidth(AppData.hpBoxStroke);
        catPaintRed.setTextSize(AppData.resourcesTextSize);
        catPaintRed.setColor(AppData.resourcesTextColor);
        textPaint.setTextSize(AppData.distanceTextSize);
        namePaint.setTextSize(AppData.nameTextSzie);
        namePaint.setColor(AppData.nameTextColor);
        hppaint.setColor(AppData.hpColor);
        testPain.setTextSize(AppData.playerCountTextSize);
        testPain.setColor(AppData.playerCountTextColor);
        heroCountPaint.setColor(AppData.boxColor);
        botCountPaint.setColor(AppData.botBoxColor);
    }

    public void updateView() {
        viewParam.width = (int) AppData.width;
        viewParam.height = (int) AppData.height;
        windowManager.updateViewLayout((View) drawView, viewParam);
    }

    public void removeView() {
        windowManager.removeView((View) drawView);
    }

    int t_fps = 0;
    int f_fps = 0;
    long time = System.currentTimeMillis();

    public void DrawCat(Canvas canvas, String name, float fx, float fy, float fw, float fh, int d, boolean isD, Paint paint) {
        if (isD) {
            canvas.drawText(String.format("%s(%d)", name, d), fx + AppData.box_x, fy, paint);
        } else {
            canvas.drawText(name, fx + AppData.box_x, fy, paint);
        }
    }

    public void DrawText(Canvas canvas, String str, float fx, float fy, Paint paint) {
        canvas.drawText(str, fx + AppData.box_x, fy, paint);
    }

    public void DrawLine(Canvas canvas, float startx, float starty, float endx, float endy) {
        canvas.drawLine(startx + AppData.box_x, starty + AppData.box_y, endx + AppData.box_x, endy + AppData.box_y, linePaint);
    }

    float left, right, top, bottom;
    float m_left, m_right, m_top, m_bottom;
    float b_top;
    float maxHp = 100;

    public void DrawHero(Canvas canvas, float fx, float fy, float bx, float by, float d3x, float d3y, float fw, float fh, int d, int isRob, int hp, int isxz, int teamId) {

        if (isRob == 0) {
            if (isxz == 0) {
                this.boxPaint.setColor(AppData.botBoxColor);
                this.linePaint.setColor(AppData.botlineColor);
            } else if (isxz == 1) {
                this.linePaint.setColor(AppData.boxLineSelectColor);
                this.boxPaint.setColor(AppData.boxLineSelectColor);
            } else if (isxz == 2) {
                this.linePaint.setColor(AppData.lockSelectColor);
                this.boxPaint.setColor(AppData.lockSelectColor);
            }

        } else {
            if (isxz == 0) {
                this.linePaint.setColor(AppData.lineColor);
                this.boxPaint.setColor(AppData.boxColor);
            } else if (isxz == 1) {
                this.linePaint.setColor(AppData.boxLineSelectColor);
                this.boxPaint.setColor(AppData.boxLineSelectColor);
            } else if (isxz == 2) {
                this.linePaint.setColor(AppData.lockSelectColor);
                this.boxPaint.setColor(AppData.lockSelectColor);
            }

        }

       /* if (d3x != 0 && d3x != 0) {
            canvas.drawCircle(d3x + mapX, d3y + mapY, palearRadius, isRob != 0 ? heroCountPaint : botCountPaint);
        }*/

        if (AppData.Back && (fx <= 0 || fx >= AppData.width || fy <= 0 || fy >= AppData.height)) {
            float hx = fx;
            float hy = fy;

            if (hx <= 100) {
                hx = 100;
            } else if (hx > (AppData.width - 100)) {
                hx = AppData.width - 200;
            }

            if (hy <= 100) {
                hy = 100;
            } else if (hy > (AppData.height - 100)) {
                hy = AppData.height - 100;
            }
            canvas.drawRect(hx, hy - 30, hx + 100, hy + 30, isRob != 0 ? heroCountPaint : botCountPaint);
            canvas.drawRect(hx, hy - 30, hx + 100, hy + 30, hpBoxPaint);
            // canvas.drawLine(hx + 50, hy - 30, hx + 50, hy + 30, hpBoxPaint);
            String format = String.format("%dM", d);
            float v = namePaint.measureText(format);
            canvas.drawText(format, (hx + 50) - (v / 2) - 10, hy + 10, namePaint);
            return;
        }

        left = fx - fw + AppData.box_x;
        right = fx + fw + AppData.box_x;
        top = fy - fh + AppData.box_y - 20;
        bottom = fy + fh + AppData.box_y;

        if (AppData.isK) {
            //canvas.drawRect(left, top, right, bottom, boxPaint);
            float fbx = fx + AppData.box_x;
            float fby = fy + AppData.box_y - 10;

            canvas.drawLine(fbx - (fw / 3) / 2, fby - fh, fbx - fw, fby - fh, boxPaint);
            canvas.drawLine(fbx - fw, fby - fh, fbx - fw, fby - fh / 2, boxPaint);

            canvas.drawLine(fbx + (fw / 3) / 2, fby - fh, fbx + fw, fby - fh, boxPaint);
            canvas.drawLine(fbx + fw, fby - fh, fbx + fw, fby - fh / 2, boxPaint);

            canvas.drawLine(fbx - (fw / 3) / 2, fby + fh, fbx - fw, fby + fh, boxPaint);
            canvas.drawLine(fbx - fw, fby + fh, fbx - fw, fby + fh / 2, boxPaint);

            canvas.drawLine(fbx + (fw / 3) / 2, fby + fh, fbx + fw, fby + fh, boxPaint);
            canvas.drawLine(fbx + fw, fby + fh, fbx + fw, fby + fh / 2, boxPaint);
        }

        if (AppData.Health) {
            if (right - left > 100) {
                m_left = this.left;
                m_top = this.top - 10 - AppData.boxStroke;/*this.top - (this.bottom - this.top) / 15;*/
                m_right = this.right;
                m_bottom = this.top - AppData.boxStroke;
            } else {
                m_left = fx + AppData.box_x - 50;
                m_top = this.top - 10 - AppData.boxStroke;
                m_right = fx + AppData.box_x + 50;
                m_bottom = this.top - AppData.boxStroke;
            }

            float hpf = (((m_right - 2.0f) - (m_left + 2.0f)) / maxHp) * hp;
            canvas.drawRect(m_left, m_top, m_right, m_bottom, hpBoxPaint);
            canvas.drawRect(m_left, m_top, m_left + hpf, m_bottom, hppaint);
            //canvas.drawText("" + teamId, m_left, m_top,textPaint);
        }

        if (AppData.Head && (fx < AppData.getWidth() && fx > 0) && (fy < AppData.getHeight() && fy > 0)) {
            int redius = (int) (fw / 4);
            canvas.drawCircle(bx + AppData.box_x, by + AppData.box_y, redius, headPaint);
        }
        if (AppData.Name) {
            String format = String.format("%dM", d);
            float v = textPaint.measureText(format);
            // canvas.drawText(format, fx - v / 2 , bottom + 30 + AppData.boxStroke, textPaint);
            canvas.drawText(format, fx - v / 2 + AppData.box_x, bottom + 30 + AppData.boxStroke, textPaint);
        }

        if (AppData.isLine) {
            canvas.drawLine((int) (float) AppData.floor - 80, AppData.playerCountHeight + 15, fx + AppData.box_x, this.top - (AppData.Health ? 10 : 3) - AppData.boxStroke, linePaint);
        }
    }


    /**
     * 清理视图
     */
    public void clear() {
        synchronized (drawView) {
            try {
                drawView.clear();
            } catch (Exception ignored) {
            }
        }
    }


    float mapX = 600F;
    float mapY = 200F;
    float radius = 200F;
    float palearRadius = 10F;

    public void draw(DataDec dataRec) {
        Canvas canvas = null;
        synchronized (drawView) {
            try {
                canvas = drawView.lockCanvas();
                canvas.drawColor(Color.TRANSPARENT, PorterDuff.Mode.CLEAR);
                int strType = dataRec.getInt();

              /*  canvas.drawCircle(mapX, mapY, radius, hpBoxPaint);
                canvas.drawLine(mapX, mapY, mapX, mapY + radius, hpBoxPaint);
                canvas.drawLine(mapX - radius, mapY, mapX + radius, mapY, hpBoxPaint);*/

                for (int i = 0; i < dataRec.getCount(); i++) {
                    try {
                        int type = dataRec.getInt();
                        int scid = dataRec.getInt();
                        float fx = dataRec.getFloat();
                        float fy = dataRec.getFloat();
                        int bx = dataRec.getInt();
                        int by = dataRec.getInt();
                        float d3x = dataRec.getFloat();
                        float d3y = dataRec.getFloat();

                        float fw = dataRec.getFloat();
                        float fh = dataRec.getFloat();
                        int d = dataRec.getInt();
                        int hp = dataRec.getInt();
                        int isRob = dataRec.getInt();
                        int isxz = dataRec.getInt();
                        int teamId = dataRec.getInt(); // 团队id

                        int Head_X = dataRec.getInt();
                        int Head_Y = dataRec.getInt();

                        int Pit_X = dataRec.getInt();
                        int Pit_Y = dataRec.getInt();

                        int Pelvis_X = dataRec.getInt();
                        int Pelvis_Y = dataRec.getInt();

                        int Lcollar_X = dataRec.getInt();
                        int Lcollar_Y = dataRec.getInt();

                        int Rcollar_X = dataRec.getInt();
                        int Rcollar_Y = dataRec.getInt();

                        int Lelbow_X = dataRec.getInt();
                        int Lelbow_Y = dataRec.getInt();

                        int Relbow_X = dataRec.getInt();
                        int Relbow_Y = dataRec.getInt();

                        int Lwrist_X = dataRec.getInt();
                        int Lwrist_Y = dataRec.getInt();

                        int Rwrist_X = dataRec.getInt();
                        int Rwrist_Y = dataRec.getInt();

                        int Lthigh_X = dataRec.getInt();
                        int Lthigh_Y = dataRec.getInt();

                        int Rthigh_X = dataRec.getInt();
                        int Rthigh_Y = dataRec.getInt();

                        int Lknee_X = dataRec.getInt();
                        int Lknee_Y = dataRec.getInt();

                        int Rknee_X = dataRec.getInt();
                        int Rknee_Y = dataRec.getInt();

                        int Lankle_X = dataRec.getInt();
                        int Lankle_Y = dataRec.getInt();

                        int Rankle_X = dataRec.getInt();
                        int Rankle_Y = dataRec.getInt();

                        String na;
                        if (strType == 0) {
                            na = dataRec.getString("UTF-8");
                        } else {
                            na = dataRec.getString("UTF-16LE");
                        }
                        // na = String.format("%s(%d)", na, teamId);
                        if (type == 1) {
                            DrawHero(canvas, fx, fy, bx, by, d3x, d3y, fw, fh, d, isRob, hp, isxz, teamId);
                            if (AppData.Name) {
                                String name = "";
                                if (scid == 0) {
                                    name = "空手";
                                } else if (scid == -1) {
                                    name = "持枪";
                                } else if (scid > 0) {
                                    name = getname(scid);
                                }
                                String format = String.format("%s(%s,%d)", na, name, teamId);
                                //计算文字像素长度
                                float v = namePaint.measureText(format);
                               /* String teamIdStr = String.format("%d", teamId);
                                float teamIdStrLen = namePaint.measureText(teamIdStr);*/
                                DrawText(canvas, format, fx - v / 2, fy - fh - 40, namePaint);
                                // DrawText(canvas, teamIdStr, fx - teamIdStrLen / 2, fy - fh - 40, namePaint);
                            }

                            if (AppData.Bone && !(fx <= 0 || fx >= AppData.width || fy <= 0 || fy >= AppData.height)) {
                                DrawLine(canvas, Head_X, Head_Y, Pit_X, Pit_Y);
                                DrawLine(canvas, Pit_X, Pit_Y, Pelvis_X, Pelvis_Y);

                                DrawLine(canvas, Pit_X, Pit_Y, Lcollar_X, Lcollar_Y);
                                DrawLine(canvas, Pit_X, Pit_Y, Rcollar_X, Rcollar_Y);

                                DrawLine(canvas, Lcollar_X, Lcollar_Y, Lelbow_X, Lelbow_Y);
                                DrawLine(canvas, Rcollar_X, Rcollar_Y, Relbow_X, Relbow_Y);

                                DrawLine(canvas, Lelbow_X, Lelbow_Y, Lwrist_X, Lwrist_Y);
                                DrawLine(canvas, Relbow_X, Relbow_Y, Rwrist_X, Rwrist_Y);

                                DrawLine(canvas, Pelvis_X, Pelvis_Y, Lthigh_X, Lthigh_Y);
                                DrawLine(canvas, Pelvis_X, Pelvis_Y, Rthigh_X, Rthigh_Y);

                                DrawLine(canvas, Lthigh_X, Lthigh_Y, Lknee_X, Lknee_Y);
                                DrawLine(canvas, Rthigh_X, Rthigh_Y, Rknee_X, Rknee_Y);

                                DrawLine(canvas, Lknee_X, Lknee_Y, Lankle_X, Lankle_Y);
                                DrawLine(canvas, Rknee_X, Rknee_Y, Rankle_X, Rankle_Y);
                            }


                        } else {
                            if (type == 10610 || type == 10613) {//如果是投掷武器
                                DrawCat(canvas, isMt((int) type), fx - 70f, fy - 40, fw, fh, d, true, catPaintRed);
                                DrawText(canvas, "↓↓", fx - 20f, fy - 10, catPaintRed);
                            } /*else if (type == 12345) {
                                this.tempPaint.setARGB(255, 0, 0, 0);
                                DrawCat(canvas, String.format("%s(%d)", ".", d), fx, fy, fw, fh, d, true, tempPaint);
                            } */ else {
//                                String getname = na;
                                String getname = getname(type);
                                if (getname != null) {
                                    DrawCat(canvas, getname, fx, fy, fw, fh, d, true, tempPaint);
                                }
                            }

                        }

                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }

                t_fps++;
                if (System.currentTimeMillis() - time >= 1000) {
                    f_fps = t_fps;
                    t_fps = 0;
                    time = System.currentTimeMillis();
                }
                //fps
                canvas.drawText(String.format("FPS:%d", f_fps), 520, 40, fpsPaint);

                int heroCount = dataRec.getInt();
                int botCount = dataRec.getInt();

                String heroCountStr = String.format("玩家:%d", heroCount);
                String botCountStr = String.format("人机:%d", botCount);

                float max = Math.max(testPain.measureText(heroCountStr), testPain.measureText(botCountStr));

                float left = (AppData.floor - 80);
                float right = (AppData.floor - 80);
                float top = AppData.playerCountHeight - (max / 3) - 5;
                float bottom = AppData.playerCountHeight + 5;


                canvas.drawRect(left, top, right + max + 20, bottom, botCountPaint);
                canvas.drawRect(left - max - 20, top, right, bottom, heroCountPaint);

                canvas.drawText(heroCountStr, left - max - 10, bottom - 10, testPain);
                canvas.drawText(botCountStr, left + 10, bottom - 10, testPain);

                canvas.drawRect(left, top, right + max + 20, bottom, hpBoxPaint);
                canvas.drawRect(left - max - 20, top, right, bottom, hpBoxPaint);

            } catch (Exception e) {
                e.printStackTrace();
            } finally {
                drawView.unlockCanvasAndPost(canvas);
            }
        }
    }


    //开始
    public String getname(int id) {
        Goods goods = idmap.get(id);
        if (goods != null) {
            this.tempPaint.setColor(goods.getColor());
            return goods.getName();
        }
        return null;

    }

    Map<Integer, Goods> idmap = new HashMap<>();

    public void initItem() {
        //载具
        idmap.put(10901, new Goods("飞机", Color.WHITE));
    }


    /**
     * 判断当前系统语言是否为中文
     *
     * @return
     */
    private boolean isZh() {
        Locale locale = context.getResources().getConfiguration().locale;
        String language = locale.getLanguage();
        if (locale.getLanguage().equals(new Locale("zh", "", "").getLanguage()))
            return true;
        else
            return false;
    }

}
