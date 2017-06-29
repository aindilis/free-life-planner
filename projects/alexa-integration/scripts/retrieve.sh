CLIENT_ID="amzn1.application-oa2-client.<APPID>"
DEVICE_TYPE_ID="Test"
DEVICE_SERIAL_NUMBER=123
REDIRECT_URI="https://localhost:9745/authresponse"
RESPONSE_TYPE="code"
SCOPE="alexa:all"
SCOPE_DATA="{\"alexa:all\": {\"productID\": \"$DEVICE_TYPE_ID\", \"productInstanceAttributes\": {\"deviceSerialNumber\": \"${DEVICE_SERIAL_NUMBER}\"}}}"

function urlencode() {
  perl -MURI::Escape -ne 'chomp;print uri_escape($_),"\n"'
}

open ${AUTH_URL}
  
