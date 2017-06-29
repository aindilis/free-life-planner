#!/bin/bash

# note you have to configure it to use port 443 in order for it to work

openssl genrsa -out private-key.pem 2048

penssl req -new -x509 -days 365 \
       -key private-key.pem \
       -config configuration.cnf \
       -out certificate.pem

openssl pkcs12 \
	-keypbe PBE-SHA1-3DES \
	-certpbe PBE-SHA1-3DES \
	-inkey private-key.pem \
	-in certificate.pem -export \
	-out keystore.pkcs12 \

$JAVA_HOME/bin/keytool \
	-importkeystore \
	-destkeystore java-keystore.jks \
	-srckeystore keystore.pkcs12 \
	-srcstoretype PKCS12 \

# <java classname="Launcher"
# classpathref="java.sdk.classpath" fork="true">
#   <sysproperty key="javax.net.ssl.keyStore"
# 	       value="/insert/your/path/java-keystore.jks" />
#   <sysproperty key="javax.net.ssl.keyStorePassword"
# 	       value="insert_your_password" />
# </java>

cd samples/

mvn assembly:assembly -DdescriptorId=jar-with-dependencies package

mvn exec:java -Dexec.executable=”java” -DdisableRequestSignatureCheck=true



# curl -v -k https://localhost:8888/hello --data-binary  '{
#   "version": "1.0",
#   "session": {
#     "new": true,
#     "sessionId": "session1234",
#     "application": {
#       "applicationId": "amzn1.echo-sdk-ams.app.1234"
#     },
#     "attributes": {},
#     "user": {
#       "userId": null
#     }
#   },
#   "request": {
#     "type": "LaunchRequest",
#     "requestId": "request5678",
#     "timestamp": "2015-05-13T12:34:56Z"
#   }
# }'

curl -v -k https://central.frdcsa.org:8888/hello --data-binary  '{
  "version": "1.0",
  "session": {
    "new": true,
    "sessionId": "session1234",
    "application": {
      "applicationId": "amzn1.echo-sdk-ams.app.1234"
    },
    "attributes": {},
    "user": {
      "userId": null
    }
  },
  "request": {
    "type": "LaunchRequest",
    "requestId": "request5678",
    "timestamp": "2015-05-13T12:34:56Z"
  }
}'
