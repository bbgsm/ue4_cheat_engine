package connect;

import java.io.UnsupportedEncodingException;

/**
 * 数据解析
 */

public class DataRecevice {
    public static final int HEADER_LEN = 16;
    private byte bytes[] = null;
    private long index = HEADER_LEN;
    private long length = 0;
    private int cmd = -1;
    private int count = 0;

    public DataRecevice() {
    }


    public void setData(byte bs[]) {
        reset(); // 重置读取下标
        if (bs != null && bs.length > 0) {
            this.bytes = bs;
            cmd = ByteUtil.bytesToInt(bs, 0);
            count = ByteUtil.bytesToInt(bs, 4);
            length = ByteUtil.bytesToLong(bs, 8);
        }
        for (int i = 16; i < length; i++) {
            bytes[i] = (byte) (bytes[i] ^ 1);
        }

    }

    public int getInt() {
        if (bytes != null && bytes.length >= index + 4) {
            int data = ByteUtil.bytesToInt(bytes, (int) index);
            index += 4;
            return data;
        }
        return 0;
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
        if (bytes != null && bytes.length >= index + 8) {
            long data = ByteUtil.bytesToLong(bytes, (int) index);
            index += 8;
            return data / 1000.0f;
        }
        return 0;
    }

    public double getDouble() {
        if (bytes != null && bytes.length >= index + 8) {
            long data = ByteUtil.bytesToLong(bytes, (int) index);
            index += 8;
            return data / 1000000.0d;
        }
        return 0;
    }


    /**
     * 设置读取偏移
     *
     * @param skip
     */
    public void skip(int skip) {
        index = 16 + skip;
    }

    /**
     * 重置读取下标
     */
    public void reset() {
        index = 16;
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


    public int getCmd() {
        return cmd;
    }

    public long getLength() {
        return length;
    }

    public int getCount() {
        return count;
    }
}
