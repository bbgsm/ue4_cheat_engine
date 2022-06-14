package connect.utils;

import java.io.UnsupportedEncodingException;


//数据解析类
public class DataDec {
    public static final int HEADER_LEN = 16;
    private byte bytes[] = null;
    private int index = HEADER_LEN;


    public DataDec() {

    }

    public DataDec(byte bs[]) {
        setData(bs);
    }

    public void setData(byte bs[]) {
        reset(); // 重置读取下标
        this.bytes = bs;
    }


    public int getInt() {
        if (bytes != null && bytes.length >= index + 4) {
            int data = ByteUtil.bytesToInt(bytes, (int) index);
            index += 4;
            return data;
        }
        return 0;
    }

    public boolean getBool() {
        if (bytes != null && bytes.length >= index + 1) {
            return bytes[index++] == 1;
        }
        return false;
    }

    public long getLong() {
        if (bytes != null && bytes.length >= index + 8) {
            long data = ByteUtil.bytesToLong(bytes, (int) index);
            index += 8;
            return data;
        }
        return 0;
    }


    public float getFloat() {
        return getInt() / 1000.0f;
    }

    public double getDouble() {
        if (bytes != null && bytes.length >= index + 8) {
            long data = ByteUtil.bytesToLong(bytes, (int) index);
            index += 8;
            return data / 1000000.0d;
        }
        return 0;
    }

    public long getLastLong() {
        long length = getLength();
        if (length > 0) {
            return ByteUtil.bytesToLong(bytes, (int) +HEADER_LEN - 8);
        }
        return 0;
    }


    /**
     * 设置读取偏移
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


    /**
     * 获取String
     *
     * @param en
     * @return
     */
    public String getString(String en) {
        int strLen = ByteUtil.bytesToInt(bytes, (int) index);
        index += 4;
        if (bytes != null && bytes.length >= index + strLen) {
            String str = null;
            try {
                str = new String(bytes, (int) index, strLen, en);
            } catch (UnsupportedEncodingException e) {
                e.printStackTrace();
            }
            index += strLen;
            return str;
        }
        return null;
    }

    public String getString() {
        return getString("UTF-8");
    }

    public int getCmd() {
        if (bytes != null && bytes.length >= HEADER_LEN) {
            return ByteUtil.bytesToInt(bytes, 0);
        }
        return -1;
    }

    public int getCount() {
        if (bytes != null && bytes.length >= HEADER_LEN) {
            return ByteUtil.bytesToInt(bytes, 4);
        }
        return -1;
    }

    public long getLength() {
        if (bytes != null && bytes.length >= HEADER_LEN) {
            return ByteUtil.bytesToLong(bytes, 8);
        }
        return -1;
    }

    public int getHeaderSize() {
        return HEADER_LEN;
    }


}
