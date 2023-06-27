#include "general.h"
#include "Client.h"
#include "Server.h"


int main() {
	Client client("user");
	client.keyGen();
	client.test_Decode_and_Encode();
	chall new_chall;
	new_chall = client.challenge();
	Server serverA("serverA");
	Server serverB("serverB");
	alignas(16) uint8_t proofA[16];
	alignas(16) uint8_t proofB[16];
	serverA.response(new_chall, file_a, proofA);
	serverB.response(new_chall, file_b, proofB);
	client.Verify(new_chall,proofA,proofB);
	return 0;
	
}