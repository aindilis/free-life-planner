package org.frdcsa.unilang.agent;

        import java.io.BufferedReader;
        import java.io.IOException;
        import java.io.InputStream;
        import java.io.InputStreamReader;
        import java.io.OutputStream;
        import java.net.Socket;
        import java.net.SocketException;
        import java.util.ArrayList;
        import java.util.Iterator;
        import java.util.List;
        import java.util.concurrent.atomic.AtomicBoolean;

        import com.thoughtworks.xstream.XStream;

        import org.frdcsa.unilang.util.Message;

/**
 * UniLang agent interface. This class allows an agent to communicate
 * asynchronously with a UniLang server.
 */
public class Agent implements Runnable {
    private String name;
    private Socket sock;
    private InputStream input;
    private BufferedReader reader;
    private OutputStream output;
    private List listeners = new ArrayList();
    private Thread thread;
    private AtomicBoolean shutdownFlag = new AtomicBoolean(false);
    private static XStream xstream = new XStream();

    static {
        xstream.alias("message", Message.class);
    }

    /**
     * Create a new agent. The agent should first be
     * {@link #connect(String, int)}ed
     * to the UniLang server and then {@link #start()}ed to begin
     * dispatching messages.
     *
     * @param name The name of this agent.
     */
    public Agent(String name) {
        this.name = name;
        if(name == null || name.equals(""))
            throw new RuntimeException("Agent name must be specified");
    }

    /**
     * Start the message loop of this agent. A new thread is created and
     * message dispatching begins.
     */
    public void start() {
        if(thread != null)
            throw new IllegalThreadStateException("Agent already running");
        thread = new Thread(this, "UniLang-Agent-" + name);
        thread.setDaemon(true);
        thread.start();
    }

    /**
     * Stop message dispatching of the agent. This does not interfere with
     * the UniLang connection so it's possible to {@link #start()} and stop
     * the agent while it remains connected.
     *
     * @throws InterruptedException If interrupted waiting for thread to stop.
     */
    public void stop() throws InterruptedException {
        if(thread == null)
            throw new IllegalThreadStateException("Agent not running");
        shutdownFlag.set(true);
        synchronized(this) {
            thread.interrupt();
            wait();
            thread = null;
        }
    }

    /**
     * Implementation of {@link Runnable} interface.
     */
    @Override
    public void run() {
        String line;
        String message = "";
        String end = "UNKNOWN";
        try {
            while(true) {
                // spin slowly until ready to read
                try {
                    if(!reader.ready())
                        Thread.sleep(1000);
                } catch(InterruptedException ex) {
                    if(shutdownFlag.get())
                        break;
                    // TODO else log interruption
                }

                if(!reader.ready())
                    continue;

                // read a line
                line = reader.readLine();
                // socket closed under us
                if(line == null)
                    break;

                // current protocol is a hack to know how long a message is
                message += line;
                if(message.equals(line)) {
                    end = line.replace("<", "</");
                } else if(line.startsWith(end)) {
                    dispatchMessage((Message)xstream.fromXML(message));
                    message = "";
                }
            }
        } catch(IOException ex) {
            // TODO
            ex.printStackTrace();
        }
        synchronized(this) {
            notify();
        }
    }

    /**
     * Connect to the UniLang server. This should be called before
     * {@link #start()}ing the agent.
     *
     * @param hostname Hostname of the UniLang server to connect to.
     * @param port Port to connect to.
     * @throws IOException If connection fails.
     */
    public void connect(String hostname, int port) throws IOException {
        sock = new Socket(hostname, port);
        input = sock.getInputStream();
        reader = new BufferedReader(new InputStreamReader(input));
        output = sock.getOutputStream();

        Message register = new Message();
        register.contents = "Register";
        sendMessage(register);
    }

    /**
     * Disconnect the agent from the UniLang server. This should not
     * be called until the agent is stopped.
     *
     * @see #stop()
     * @throws IOException If closing the socket fails.
     */
    public void disconnect() throws IOException {
        if(sock == null)
            return;
        Message deregister = new Message();
        deregister.contents = "Deregister";
        sendMessage(deregister);
        sock.close();
        sock = null;
    }

    /**
     * Dispatch a message from UniLang to registered listeners.
     *
     * @param msg The message to dispatch to the listeners.
     */
    protected void dispatchMessage(Message msg) {
        synchronized(listeners) {
            for(Iterator i = listeners.iterator(); i.hasNext(); ) {
                AgentListener al = (AgentListener)i.next();
                al.onMessage(msg);
            }
        }
    }

    /**
     * Send a message to the UniLang server. This method is thread-safe
     * and synchronized on the socket to prevent interleaved messages
     * being sent to the server.
     *
     * @param msg The message to send.
     * @throws IOException If there's a problem sending the message.
     */
    public void sendMessage(Message msg) throws IOException {
        if(sock == null)
            throw new SocketException("Not connected");
        if(msg == null)
            throw new IllegalArgumentException("Invalid message");
        msg.sender = name;
        String msgXml = xstream.toXML(msg) + "\n";
        // hack way to not use XML entities, is it ok?
        msgXml = msgXml.replaceAll("&apos;", "'");
        msgXml = msgXml.replaceAll("&quot;", "\"");
        synchronized(sock) {
            output.write(msgXml.getBytes());
            output.flush();
        }
        System.out.print("Wrote message: \n'" + msgXml + "'\n");
    }

    public void addListener(AgentListener l) {
        synchronized(listeners) {
            listeners.add(l);
        }
    }

    public void removeListener(AgentListener l) {
        synchronized(listeners) {
            listeners.remove(l);
        }
    }
}