package connect.utils;

import java.io.UnsupportedEncodingException;

//数据包装类
public class DataEnc {
    public static final int HEADER_LEN = 16;//头长度

    private byte bytes[] = null;
    private int index = HEADER_LEN;
    private int byteLen = 0;

    public DataEnc() {
    }

    public DataEnc(byte bs[]) {
        setData(bs);
    }

    public void setData(byte bs[]) {
        reset(); // 重置读取下标
        this.bytes = bs;
        if (bs != null) {
            byteLen = bs.length;
        }

    }

    public void packData(int cmd) {
        index = HEADER_LEN;
        setCmd(cmd);
        setCount(0);
        setLength(0);
    }

    public DataEnc setCmd(int cmd) {
        if (byteLen <= 0) {
            return null;
        }
        ByteUtil.intToBytes(cmd, bytes, 0);
        return this;
    }

    public DataEnc setCount(int val) {
        if (byteLen <= 0) {
            return null;
        }
        ByteUtil.intToBytes(val, bytes, 4);
        return this;
    }

    public DataEnc setLength(int val) {
        if (byteLen <= 0) {
            return null;
        }
        ByteUtil.longToBytes(val, bytes, 8);
        return this;
    }


    public DataEnc putInt(int val) {
        if (index > byteLen) {
            return null;
        }
        ByteUtil.intToBytes(val, bytes, index);
        index += 4;
        return this;
    }

    public DataEnc putBool(boolean val) {
        bytes[index++] = (byte) (val ? 1 : 0);
        return this;
    }

    public DataEnc putFloat(float val) {
        return putInt((int) (val * 1000));
    }

    public DataEnc putDouble(double val) {
        return putLong((long) (val * 1000000));
    }

    public DataEnc putLong(long val) {
        if (index > byteLen) {
            return null;
        }
        ByteUtil.longToBytes(val, bytes, index);
        index += 8;
        return this;
    }


    public DataEnc putString(String val) {
        if (index > byteLen) {
            return null;
        }
        try {
            byte[] bs = val.getBytes("UTF-8");
            putInt(bs.length);
            System.arraycopy(bs, 0, bytes, index, bs.length);
            index += bs.length;
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
        }
        return this;
    }

    /**
     * 设置偏移
     *
     * @param skip
     */
    public void skip(int skip) {
        index = HEADER_LEN + skip;
    }

    /**
     * 重置读取下标
     */
    public void reset() {
        index = HEADER_LEN;
    }

    public int getDataLen() {
        return index;
    }

    public int getHeaderSize() {
        return HEADER_LEN;
    }

    public byte[] getData() {
        setLength(index - HEADER_LEN);
        return bytes;
    }
}
