#include "gfmul.h"
#include "general.h"
#include "Client.h"
#include "Server.h"



int main() {
	Client client("user");
	//client.KeyGen();
	client.test_Decode_and_Encode(s_a,key_A);
	chall chall_a;
	chall_a = client.challenge();
	Server serverA("serverA");
	alignas(16) uint8_t proofA[16];
	serverA.response(chall_a,file_a,proofA);
	return 0;
	
}