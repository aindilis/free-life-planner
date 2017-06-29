(plan
 (completed
  (get test skill working))
 (concurrently
  (develop test2 skill, in which we learn more about the internals
   of the application)
  (develop testA skill which uses http client, in order to
   communicate with our web server))

 (buy amazon echo dot)
 (get OAuth 2.0 set up)
 (publish skill)
 )

(tutorial
 (https://github.com/pluralsight/guides/blob/master/published/node-js/amazon-alexa-skill-tutorial/article.md))

(solution
 (error message
  (Error: Please make sure that "Alexa Skills Kit" is selected for the event source type of arn))
 (progn
  (urlFn "https://acloud.guru/forums/intro-alexa-free/discussion/<SOMECODE>/i-am-getting-error-configuring-the-skill-endpoint-please-make-sure-that-alexa-sk")
  (I was able to fix this issue by going to my skill and add trigger by selecting the "Alexa Skill...")))

(solution
 (solution
  (my alexa skill works with the echosim but not with my alexa device)
  (If your custom skill not available in English(US), you will not be able to use it on a device yet.
   You will only be able to use it it on the service simulator in the developer portal at this time.))
 (During development, there are multiple options for testing a new skill:
  Do simple testing using the Service Simulator displayed on the Test page in the developer portal.
  Test by voice using an Alexa-enabled device.))


(https://forums.aws.amazon.com/thread.jspa?threadID=170919)
(https://github.com/amzn/alexa-skills-kit-js/blob/master/samples/tidePooler/src/index.js)

(https://developer.amazon.com/public/solutions/alexa/alexa-skills-kit/docs/deploying-a-sample-skill-as-a-web-service)


(    http.get(endpoint + queryString, function (res) {
	      var noaaResponseString = '';
	      console.log('Status Code: ' + res.statusCode);

	      if (res.statusCode != 200) {
	      tideResponseCallback(new Error("Non 200 Response"));
	      }

	      res.on('data', function (data) {
		     noaaResponseString += data;
		     });

	      res.on('end', function () {
		     var noaaResponseObject = JSON.parse(noaaResponseString);

		     if (noaaResponseObject.error) {
		     console.log("NOAA error: " + noaaResponseObj.error.message);
		     tideResponseCallback(new Error(noaaResponseObj.error.message));
		     } else {
		     var highTide = findHighTide(noaaResponseObject);
		     tideResponseCallback(null, highTide);
		     }
		     });
	      }).on('error', function (e) {
		    console.log("Communications error: " + e.message);
		    tideResponseCallback(new Error(e.message));
		        });)

(wishlist
 (possibly integrate FLP with lists via: https://developer.amazon.com/announcements/lists-beta.html)
 (buy Amazon Echo Dot
  (we need to buy a new Amazon Echo Dot and link it with our
  aws.amazon.com <> account.  then we will be
  able to test the FLP on a real device.))
 (get OAuth2.0 set up
  (we need to set up OAuth 2.0 on FLP / FCMS in order to make the
   system available to us for use on the FLP.  However we don't
   want anyone else signing up.  We have to ask them about this,
   whether we can block sigh ins.  Actually, now that we know how
   to use an actual Echo Dot with my developer account, I won't
   have to get the OAuth set up right away))
 )

(todo
 (we need to determine whether if we get OAuth working we will
  still need to do a non-aws http thing.  Probably, in which case it
  will be wise to work on the http interface before OAuth, because
  I will be able to use it with echosim in the meantime then.)
 

 (completed
  (I could ask willthechill if he could try to get
   OAuth-2.0 working with shinycms.))
 
 (we probably need to enable HTTP instead of AWS in order to use
  the FLP with the Alexa device)
 )

(links
 (http://wiki.catalystframework.org/wiki/resources/httpstricks.view)
 (https://metacpan.org/pod/Plack::Middleware::DoormanAuth0)
 )



(in order to get account linking going I need to start serving
 the life planner using https)

(could possibly use this to get things bootstrapped
 (Note that account linking is needed when the skill needs to
  connect with a system that requires authentication. If your
  custom skill just needs to keep track of a user to save
  attributes between sessions, you do not need to use account
  linking. Instead, you can use the userId provided in each
  request to identify the user. The userId for a given user is
  generated when the user enables your skill in the Alexa
  app. Every subsequent request from that user to your skill
  contains the same userId unless the user disables the
  skill. See the Score Keeper sample skill for an example of
  saving userId values in a database to preserve attributes
  between sessions))

(In order to force catalyst to use https: If you're using
 Catalyst 5.80008 or later, set
 MyApp->config(using_frontend_proxy => 1) and simply have your
 proxy set the X-Forwarded-Port header. For Catalyst versions
 prior to 5.80008, still set using_frontend_proxy so you get the
 actual client_ip, but to generate the correct URIs have your web
 server set the environment variable HTTPS to ON)
