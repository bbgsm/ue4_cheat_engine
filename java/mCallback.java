package connect;

public interface mCallback {

    void CallBackData(DataDec dataRec);//返回数据

    void error(String messge);//错误调用

    void connectSuccess();   //已经连接成功

    void disConnect();    //连接断开
}

