package connect;



import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketException;

@SuppressWarnings("all")
public class mConnect {
    private int tcp_port;
    private int udp_port;
    private boolean isrun;
    private DataDec dataRec;
    private DataEnc dataEnc;
    private boolean isTest = false;
    private mCallback callBack = null;


    Socket socket = null;    //二进制文件连接对象
    InputStream is = null;
    ServerSocket server = null;//服务器对象
    DatagramSocket ds = null;
    byte sendbuffer[] = new byte[1024 * 1024];//1M发送数据缓冲区
    byte recBuffer[] = new byte[1024 * 1024];//1M接收数据缓冲区

    public mConnect(int tcp_port, int udp_port, boolean isTest, mCallback callBack) {
        this.callBack = callBack;
        this.tcp_port = tcp_port;
        this.udp_port = udp_port;
        this.isTest = isTest;
        isrun = true;
        dataRec = new DataDec();
        dataEnc = new DataEnc(sendbuffer);
        initUDP();
        //  initTCP();
    }

    public DataEnc getDataEnc() {
        return dataEnc;
    }

    //boolean isAccept = false;

    /**
     * 初始化TCP
     */
    public void initTCP() {
        //isAccept = false;
        ViewUpdate.runThread(() -> {
            try {
                server = new ServerSocket(tcp_port);
                if (!isTest) {
                    socket = server.accept();
                    if (callBack != null) {
                        callBack.connectSuccess();
                    }
                    is = socket.getInputStream();
                    while (is.read() != -1) ;
                    if (callBack != null) {
                        callBack.disConnect();
                    }
                } else {
                    while (isTest) {
                        socket = server.accept();
                        if (callBack != null) {
                            callBack.connectSuccess();
                        }
                        is = socket.getInputStream();
                        while (is.read() != -1) ;
                        if (callBack != null) {
                            callBack.disConnect();
                        }
                    }
                }
                // isAccept = true;

            } catch (IOException e) {
                e.printStackTrace();
            } finally {
                closeTCP();
                if (callBack != null) {
                    callBack.disConnect();
                }
            }
        });
        //while (!isAccept) ;
    }

    /**
     * 初始化udp
     */
    public void initUDP() {
        ViewUpdate.runThread(() -> {
            try {
                ds = new DatagramSocket(udp_port);
            } catch (SocketException e) {
                e.printStackTrace();
            }
            DatagramPacket dp = new DatagramPacket(recBuffer, recBuffer.length);
            while (isrun) {
                try {
                    ds.receive(dp);
                    dataRec.setData(recBuffer);
                    if (callBack != null) {
                        callBack.CallBackData(dataRec);
                    }

                } catch (Exception e) {
                }
            }
        });
    }

    /**
     * 设置运行
     *
     * @param isrun
     */
    public void setRun(boolean isrun) {
        this.isrun = isrun;
    }

    /**
     * 发送byte数组
     *
     * @param bytes
     * @param offset
     * @param ten
     */
    public void sendBytes(byte[] bytes, int offset, int ten) {
        if (socket != null && bytes != null) {
            ViewUpdate.runThread(new Runnable() {
                @Override
                public void run() {
                    try {
                        OutputStream ot = socket.getOutputStream();
                        ot.write(bytes, offset, ten);
                        ot.flush();
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }
            });
        } else {
        }

    }



    /**
     * 发送数据
     */
    public void send_Data() {
        sendBytes(dataEnc.getData(), 0, dataEnc.getDataLen());
        dataEnc.reset();
    }

    /**
     * 关闭所有流
     */
    public void closeAll() {
        isrun = false;
        CloseUtils.closeIO(is, socket, server, ds);
    }

    /**
     * 关闭TCP
     */
    public void closeTCP() {
        CloseUtils.closeIO(is, socket, server);
    }

    /**
     * 关闭UDP
     */
    public void closeUDP() {
        isrun = false;
        CloseUtils.closeIO(ds);
    }



}
