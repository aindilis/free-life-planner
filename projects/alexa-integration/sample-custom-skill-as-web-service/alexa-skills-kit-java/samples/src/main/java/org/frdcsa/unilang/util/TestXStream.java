package org.frdcsa.unilang.util;

import org.frdcsa.unilang.util.Message;

import com.thoughtworks.xstream.XStream;
// import org.xmlpull.v1.XmlPullParser;

public class TestXStream {
    private static XStream xstream = new XStream();
    static {
        xstream.alias("message", Message.class);
    }

    public static void main(String[] args) {
        String origmessage = "<message>\n	<id>1</id>\n	<sender>Android-FRDCSA-Server</sender>\n	<receiver>Android-FRDCSA-Client</receiver>\n	<date>Wed May 12 22:41::52 CDT 2010</date>\n	<contents>Command not yet implemented: Incorrect</contents>\n	<data>$VAR1 = {};\n	</data>\n</message>";
        Message msg2 = (Message) xstream.fromXML(origmessage);
        String response = msg2.contents;
        System.out.println(response + "\n");
    }
}