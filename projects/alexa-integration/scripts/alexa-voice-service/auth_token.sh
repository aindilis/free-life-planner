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

# AUTH_URL="https://www.amazon.com/ap/oa?client_id=${CLIENT_ID}&scope=$(echo $SCOPE | urlencode)&scope_data=$(echo $SCOPE_DATA | urlencode)&response_type=${RESPONSE_TYPE}&redirect_uri=$(echo $REDIRECT_URI | urlencode)"
# echo ${AUTH_URL}
# firefox -new-tab -url ${AUTH_URL}

CLIENT_ID="amzn1.application-oa2-client.<CLIENTID>"
CLIENT_SECRET="<CLIENTSECRET>"
CODE="<CODE>"
# GRANT_TYPE="authorization_code"
  
# curl -X POST --data "grant_type=${GRANT_TYPE}&code=${CODE}&client_id=${CLIENT_ID}&client_secret=${CLIENT_SECRET}&redirect_uri=${REDIRECT_URI}" https://api.amazon.com/auth/o2/token

# AUTH_URL="https://api.amazon.com/auth/o2/token?grant_type=${GRANT_TYPE}&code=${CODE}&client_id=${CLIENT_ID}&client_secret=${CLIENT_SECRET}&redirect_uri=${REDIRECT_URI}"
# echo ${AUTH_URL}
# firefox -new-tab -url ${AUTH_URL}

REFRESH="<REFRESH>"
# CLIENT_ID="amzn1.application-oa2-client.<APPID>..."
# CLIENT_SECRET="<CLIENTSECRET>"
GRANT_TYPE="refresh_token"
# REDIRECT_URI="https://localhost:9745/authresponse"

curl -X POST --data "grant_type=${GRANT_TYPE}&refresh_token=${REFRESH}&client_id=${CLIENT_ID}&client_secret=${CLIENT_SECRET}&redirect_uri=${REDIRECT_URI}" https://api.amazon.com/auth/o2/token

# {"access_token":"<ACCESSTOKEN>","refresh_token":"<REFRESHTOKEN>":3600}she
