// Twitter client sketch for ENC28J60 based Ethernet Shield. Uses 
// arduino-tweet.appspot.com as a OAuth gateway.
// Step by step instructions:
// 
//  1. Get a oauth token:
//     http://arduino-tweet.appspot.com/oauth/twitter/login
//  2. Put the token value in the TOKEN define below
//  3. Run the sketch!
//
//  WARNING: Don't send more than 1 tweet per minute! (1분에 1개 이상의 트위터를 보내지 마시오)
//  NOTE: Twitter rejects tweets with identical content as dupes (returns 403)
 
#include <EtherCard.h>
 
// 트위터 API 이용을 위해 OAUTH 토큰값을 넣는다. (트위터 ID/패스워드 필요)
// http://arduino-tweet.appspot.com/oauth/twitter/login 여기에서 트위터 로그인하고 토큰발급 받는다.
#define TOKEN   "853890302473453568-4jK5BilQKcP0zsYbf795HDwxpXEzRTb"
 
// ethernet interface mac address, must be unique on the LAN
byte mymac[] = { 0x00,0x00,0x00,0x00,0x00,0x00 };
 
const char website[] PROGMEM = "arduino-tweet.appspot.com";
 
static byte session;
 
byte Ethernet::buffer[700];
Stash stash;
 
static void sendToTwitter () {
  Serial.println("Sending tweet...");
  byte sd = stash.create();
 
  const char tweet[] = "hello world!!!!!!"; //트위터에 올릴 글 내용
  stash.print("token=");
  stash.print(TOKEN);
  stash.print("&status=");
  stash.println(tweet);
  stash.save();
  int stash_size = stash.size();
 
  // Compose the http POST request, taking the headers below and appending
  // previously created stash in the sd holder.
  Stash::prepare(PSTR("POST http://$F/update HTTP/1.0" "\r\n"
    "Host: $F" "\r\n"
    "Content-Length: $D" "\r\n"
    "\r\n"
    "$H"),
  website, website, stash_size, sd);
 
  // send the packet - this also releases all stash buffers once done
  // Save the session ID so we can watch for it in the main loop.
  session = ether.tcpSend();
}
 
void setup () {
  Serial.begin(57600);
  Serial.println("\n[Twitter Client]");
 
  if (ether.begin(sizeof Ethernet::buffer, mymac) == 0) 
    Serial.println(F("Failed to access Ethernet controller"));
  if (!ether.dhcpSetup())
    Serial.println(F("DHCP failed"));
 
  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);  
  ether.printIp("DNS: ", ether.dnsip);  
 
  if (!ether.dnsLookup(website))
    Serial.println(F("DNS failed"));
 
  ether.printIp("SRV: ", ether.hisip);
 
  sendToTwitter();
}
 
void loop () {
 
  //트위터 서버로 부터 받은 결과를 출력
  ether.packetLoop(ether.packetReceive());
  
  const char* reply = ether.tcpReply(session);
  if (reply != 0) {
    Serial.println("Got a response!");
    Serial.println(reply);
  }
 
}
