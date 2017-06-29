/**
    Copyright 2014-2015 Amazon.com, Inc. or its affiliates. All Rights Reserved.

    Licensed under the Apache License, Version 2.0 (the "License"). You may not use this file except in compliance with the License. A copy of the License is located at

        http://aws.amazon.com/apache2.0/

    or in the "license" file accompanying this file. This file is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
 */
package helloworld;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.amazon.speech.slu.Intent;
import com.amazon.speech.speechlet.IntentRequest;
import com.amazon.speech.speechlet.LaunchRequest;
import com.amazon.speech.speechlet.Session;
import com.amazon.speech.speechlet.SessionEndedRequest;
import com.amazon.speech.speechlet.SessionStartedRequest;
import com.amazon.speech.speechlet.Speechlet;
import com.amazon.speech.speechlet.SpeechletException;
import com.amazon.speech.speechlet.SpeechletResponse;
import com.amazon.speech.ui.PlainTextOutputSpeech;
import com.amazon.speech.ui.Reprompt;
import com.amazon.speech.ui.SimpleCard;

import org.frdcsa.unilang.util.Message;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import java.util.List;
import java.util.Arrays;
import java.net.URL;
import java.net.MalformedURLException;
import org.apache.xmlrpc.client.XmlRpcClient;
import org.apache.xmlrpc.client.XmlRpcClientConfigImpl;
import org.apache.xmlrpc.XmlRpcException;
import com.thoughtworks.xstream.XStream;

/**
 * This sample shows how to create a simple speechlet for handling speechlet requests.
 */
public class HelloWorldSpeechlet implements Speechlet {
    private static final Logger log = LoggerFactory.getLogger(HelloWorldSpeechlet.class);
    private static XStream xstream = new XStream();

    private static final String SLOT_TEXT = "Text";

    @Override
    public void onSessionStarted(final SessionStartedRequest request, final Session session)
            throws SpeechletException {
        log.info("onSessionStarted requestId={}, sessionId={}", request.getRequestId(),
                session.getSessionId());
        // any initialization logic goes here
    }

    @Override
    public SpeechletResponse onLaunch(final LaunchRequest request, final Session session)
            throws SpeechletException {
        log.info("onLaunch requestId={}, sessionId={}", request.getRequestId(),
                session.getSessionId());
        return getWelcomeResponse();
    }

    @Override
    public SpeechletResponse onIntent(IntentRequest request, Session session)
            throws SpeechletException {
        log.info("onIntent requestId={}, sessionId={}", request.getRequestId(),
                session.getSessionId());

        Intent intent = request.getIntent();
        String intentName = (intent != null) ? intent.getName() : null;

        if ("HelloWorldIntent".equals(intentName)) {
            return getHelloResponse();
        } else if ("RawText".equals(intentName)) {
            return queryAgent(intent,session);
        } else if ("AMAZON.HelpIntent".equals(intentName)) {
            return getHelpResponse();
        } else {
            throw new SpeechletException("Invalid Intent");
        }
    }

    @Override
    public void onSessionEnded(final SessionEndedRequest request, final Session session)
            throws SpeechletException {
        log.info("onSessionEnded requestId={}, sessionId={}", request.getRequestId(),
                session.getSessionId());
        // any cleanup logic goes here
    }

    /**
     * Creates and returns a {@code SpeechletResponse} with a welcome message.
     *
     * @return SpeechletResponse spoken and visual response for the given intent
     */
    private SpeechletResponse getWelcomeResponse() {
        String speechText = "Welcome to the Alexa Skills Kit, you can say hello";

        // Create the Simple card content.
        SimpleCard card = new SimpleCard();
        card.setTitle("HelloWorld");
        card.setContent(speechText);

        // Create the plain text output.
        PlainTextOutputSpeech speech = new PlainTextOutputSpeech();
        speech.setText(speechText);

        // Create reprompt
        Reprompt reprompt = new Reprompt();
        reprompt.setOutputSpeech(speech);

        return SpeechletResponse.newAskResponse(speech, reprompt, card);
    }

    /**
     * Creates a {@code SpeechletResponse} for the hello intent.
     *
     * @return SpeechletResponse spoken and visual response for the given intent
     */
    private SpeechletResponse getHelloResponse() {
        String speechText = "Hello Andy";

        // Create the Simple card content.
        SimpleCard card = new SimpleCard();
        card.setTitle("HelloWorld");
        card.setContent(speechText);

        // Create the plain text output.
        PlainTextOutputSpeech speech = new PlainTextOutputSpeech();
        speech.setText(speechText);

        return SpeechletResponse.newTellResponse(speech, card);
    }

    /**
     * Creates a {@code SpeechletResponse} for the help intent.
     *
     * @return SpeechletResponse spoken and visual response for the given intent
     */
    private SpeechletResponse getHelpResponse() {
        String speechText = "You can say hello to me!";

        // Create the Simple card content.
        SimpleCard card = new SimpleCard();
        card.setTitle("HelloWorld");
        card.setContent(speechText);

        // Create the plain text output.
        PlainTextOutputSpeech speech = new PlainTextOutputSpeech();
        speech.setText(speechText);

        // Create reprompt
        Reprompt reprompt = new Reprompt();
        reprompt.setOutputSpeech(speech);

        return SpeechletResponse.newAskResponse(speech, reprompt, card);
    }

    private SpeechletResponse queryAgent(Intent intent, Session session) {
        String speechText = "Error connecting to FLP";

	// see /Doug/data/posi.frdcsa.org/sda1/gitroot/minor-1.1/android/systems/workspace-justin-2/AFCService/src/org/frdcsa/android/AFCActivity.java

	String text = intent.getSlot(SLOT_TEXT).getValue();
	text = text.replaceAll("'", "\\'");
	text = text.replaceAll("\"", "\\\"");
	// go ahead and generate the message
	Message msg = new Message();
	msg.sender = "Alexa-Skill-FLP";
	msg.receiver = "Agent1";
	msg.contents = "";
	msg.data = "$VAR1 = {_DoNotLog => 1, Eval => [['_prolog_list',['_prolog_list',\\*{'::?Response'}],['alexaSkillFLPQuery','" + text + "',\\*{'::?Response'}]]]};\n	";
	String msgXml = xstream.toXML(msg) + "\n";
	// hack way to not use XML entities, is it ok?
	msgXml = msgXml.replaceAll("&apos;", "'");
	msgXml = msgXml.replaceAll("&quot;", "\"");
	// Object[] params = new Object[] { msgXml };
	List<List<String>> params = Arrays.asList( Arrays.asList( msgXml ) );

	try {
	    XmlRpcClientConfigImpl config = new XmlRpcClientConfigImpl();
	    config.setServerURL(new URL("http://ai.frdcsa.org:10000"));
	    XmlRpcClient client = new XmlRpcClient();
	    client.setConfig(config);
	    Object[] result = (Object[]) client.execute("Org.FRDCSA.UniLang.Plugins.WebService.QueryAgent", params);
	    Pattern p = Pattern.compile("alexa-skill-flp-result>(.+).....alexa-skill-flp-result>",Pattern.DOTALL);
	    Matcher m = p.matcher(result[0].toString());
	    System.out.println(result[0].toString());
	    if (m.find()) {
		speechText = m.group(1);
	    } else {
		speechText = "I don't know.";
	    }
	} catch (XmlRpcException e) {
	    System.out.println(e.toString());
	    speechText = "I'm sorry, the life planner is currently unavailable.";
	} catch (MalformedURLException e) {
	    System.out.println(e.toString());
	}
	
        // Create the Simple card content.
        SimpleCard card = new SimpleCard();
        card.setTitle("HelloWorld");
        card.setContent(speechText);

        // Create the plain text output.
        PlainTextOutputSpeech speech = new PlainTextOutputSpeech();
        speech.setText(speechText);

        return SpeechletResponse.newTellResponse(speech, card);
    }

    // void ProcessMessage(Message message) {
    // 	// probably want to add some kind of response handler here
    // 	// what we can try to do is extract data from the data portion,
    // 	// perhaps, we need to allow for an XML serialized format there
    // 	history.add(0,"Response: " + message.contents);
    // 	say(message.contents);
    // 	// awesome, now can control applications on the phone this way...
    // 	Pattern p = Pattern.compile(".*Start Flashlight.*",Pattern.DOTALL);
    // 	Matcher m = p.matcher(message.data);
    // 	if (m.matches()) {
    // 	    Intent i = new Intent(); 
    // 	    i.setClassName("org.dyndns.devesh.flashlight","org.dyndns.devesh.flashlight.Flashlight"); 
    // 	    startActivity(i);
    // 	}
    // }
}
