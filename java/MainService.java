

public class MainService  {
    private MServiceCallback callback;
    public mConnect connect;    //Socket工具

   
    public void initSocket() {
        callback = new MServiceCallback(this);
        connect = new mConnect(1001, 1002, Config.isTest, callback);
        connect.initTCP();
    }
    
    public void close(){
        if (connect != null) connect.closeTCP();
    }
}

