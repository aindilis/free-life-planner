#!/bin/bash

cd alexa-skills-kit-java/samples/ && mvn assembly:assembly -DdescriptorId=jar-with-dependencies package; mvn exec:java -Dexec.executable=”java” -DdisableRequestSignatureCheck=true
