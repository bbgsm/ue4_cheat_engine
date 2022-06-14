package main;


public class MServiceCallback implements mCallback {
    MainService service;
    DrawManage drawManage;


    public MServiceCallback(MainService service) {
        this.service = service;
    }


    @Override
    public void CallBackData(DataDec dataRec) {
        int cmd = dataRec.getCmd();
        if (cmd == SendData.D_DATA) { //接收数据
            if (service.isrun) {
                drawManage.draw(dataRec);
            }
        } else if (cmd == SendData.EXIT) {//游戏结束
            T.ToastSuccess("游戏结束");
        } else if (cmd == SendData.D_CLEAR) {

        } else if (cmd == SendData.D_PROISNOT) {
            ViewUpdate.threadUi(() -> {
                service.so.setRun(false);
                T.ToastError("进程打开失败");
            });
        } else if (cmd == SendData.D_MESSGE) {
            String string = dataRec.getString("UTF-8");
            int anInt = dataRec.getInt();
            if (anInt == 0) {
                T.ToastSuccess(string);
            } else {
                T.ToastError(string);
            }
        }
    }

    @Override
    public void error(String messge) {

    }

    @Override
    public void connectSuccess() {
          T.ToastSuccess("连接成功");
        try {
            Thread.sleep(100);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        service.connect.getDataEnc().setCmd(SendData.START)
                .putInt((int) AppData.getWidth())
                .putInt((int) AppData.getHeight())
                .putInt((int) AppData.fps)
                .putInt(AppData.Bone ? 1 : 0)
                .putInt(AppData.Vehicle ? 1 : 0)
                .putInt(AppData.Material ? 1 : 0)
                .putInt(AppData.zmRange)
                .putInt(AppData.zmMode)
                .putInt(AppData.zmspeed)
                .putInt(AppData.zmTouchX)
                .putInt(AppData.zmTouchY)
                .putInt(250 - AppData.zmx)
                .putInt(250 - AppData.zmy)
                .putInt(AppData.zmSelectType)
                .putInt(AppData.isZmDowm)
                .putInt(AppData.isZmCoherent)

        service.connect.send_Data();
    }



    @Override
    public void disConnect() {
       // 断开连接
    }
}
