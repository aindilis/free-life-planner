package org.frdcsa.unilang.util;

/**
 * UniLang message.
 */

public class Message {
    public String contents = "";
    public Integer id = new Integer(1);
    public String receiver = "UniLang";
    public String data = "";
    //    public Date date = new Date();
    // Needs this exact format
    public String date = "Sat Aug 23 03:06:36 CDT 2008";
    public String sender = "";

    //@Override
    public String toString() {
// 	return String.format("UniLang message: {Contents => '%s', ID => %d, " +
// 			     "Receiver => '%s', Data => '%s', Date => %s, " +
// 			     "Sender => '%s'}", contents, id, receiver, data,
// 			     date.toString(), sender);
        return "";
    }
}