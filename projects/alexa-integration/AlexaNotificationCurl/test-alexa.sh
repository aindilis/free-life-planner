#!/bin/sh

curl -s -X POST -H "Authorization: Bearer <AUTHTOKEN>" --data-binary @multipart_body.txt https://access-alexa-na.amazon.com/v1/avs/speechrecognizer/recognize > /tmp/tmp.mp3
