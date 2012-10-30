/**
 * swift selector
 */
#include<cstdio>
#include<cstdlib>
#include<iostream>
#include<fstream>
#include<queue>
extern "C" {
#include <getopt.h>
#include"GeoIP.h"
//#include <netinet/in.h>
}

#include"selector.h"
#include "../swift.h"

using namespace swift;
IPProfile::IPProfile(string ip) :
	ip(ip) {

}
IPProfile::IPProfile(string ip, string country) :
	ip(ip), country(country) {
}
string IPProfile::getCountry() {
	return this->country;
}
string IPProfile::getCity() {
	return this->city;
}
string IPProfile::getAsn() {
	return this->asn;
}
string IPProfile::getIP() {
	return this->ip;
}
int IPProfile::getScore() {
	return this->score;
}
int IPProfile::getStatus() {
	return this->status;
}
Sha1Hash IPProfile::getHash() {
	return this->root;
}
void IPProfile::setScore(int score) {
	this->score = score;
}
void IPProfile::setStatus(int status) {
	this->status = status;

}
void IPProfile::setHash(const Sha1Hash& root) {
	this->root = root;
}

Selector::Selector() {
#ifdef	BITSWIFT

	p_Btselector = new BitSwiftSelector();
	NeedPeer(false);
#else
	string ipAddress = "130.229.184.23";
	gicountry = GeoIP_open("ext/db/GeoIP.dat", GEOIP_STANDARD
			| GEOIP_CHECK_CACHE | GEOIP_MEMORY_CACHE);
	const char * returnedCountry = GeoIP_country_code_by_addr(gicountry,
			ipAddress.c_str());
	string country(returnedCountry);
	localprofile = new IPProfile(ipAddress, country);

#endif
}
void Selector::AddPeer(const Address& addr, const Sha1Hash& root) {
#ifdef	BITSWIFT
	if (p_Btselector == NULL)
		inpeers.push_back(addr);
	uint32_t ipv4 = ntohl(addr.addr.sin_addr.s_addr);
	char rs[20] = { 0 };
	sprintf(rs, "%i.%i.%i.%i", ipv4 >> 24, (ipv4 >> 16) & 0xff, (ipv4 >> 8)
			& 0xff, ipv4 & 0xff);
	string ipString(rs);
	p_Btselector->addpeer(ipString, addr.port(), root.hex());

#else
	candidates.push_front(addr);
	uint32_t ipv4 = ntohl(addr.addr.sin_addr.s_addr);
	char rs[20] = {0};
	sprintf(rs, "%i.%i.%i.%i", ipv4 >> 24, (ipv4 >> 16) & 0xff, (ipv4 >> 8)
			& 0xff, ipv4 & 0xff);
	string ipString(rs);
	const char * returnedCountry = GeoIP_country_code_by_addr(gicountry, rs);
	string country;
	if (returnedCountry == NULL) {
		country = "";
	} else {
		country = string(returnedCountry);
	}
	IPProfile peer(ipString, country);
	ScorePeer(peer);
	outpeer.push_back(peer);
	UpdatePeerList();
	int size = outpeer.size();
	for (int i = 0; i < size; i++) {
		int s = outpeer[i].getScore();
		fprintf(stdout, "%d\n", s);
	}
#endif
}

void Selector::DelPeer(const Address& addr, const Sha1Hash& root) {
	if (p_Btselector == NULL) {
		for (int i = 1; i < inpeers.size(); i++) {
			if (inpeers[i] == addr)
				inpeers.erase(inpeers.begin() + i);
		}
	}
#ifdef	BITSWIFT
	uint32_t ipv4 = ntohl(addr.addr.sin_addr.s_addr);
	char rs[20] = { 0 };
	sprintf(rs, "%i.%i.%i.%i", ipv4 >> 24, (ipv4 >> 16) & 0xff, (ipv4 >> 8)
			& 0xff, ipv4 & 0xff);
	string ipString(rs);
	p_Btselector->deletepeer(ipString, addr.port(), root.hex());
#else
	unsigned int counter = candidates.size();
	for (unsigned int i = 0; i < counter; i++) {
		if (addr == candidates[i])
		candidates.erase(candidates.begin() + i);
	}
#endif
}
Address Selector::GetPeer(const Sha1Hash& for_root) {
	if (p_Btselector == NULL)
		return inpeers[0];
#ifdef	BITSWIFT
	std::vector<ipPort_s> ip_port_list(100);
	p_Btselector->getpeer(for_root.hex(), 0, ip_port_list);
	ipPort_s ipport = ip_port_list.front();
	const char* ip = ipport.ipAddress.c_str();
	uint16_t port = 8000;
	Address addr("83.179.40.16", port);
	return addr;
#else
	int size = outpeer.size();
	for (int i = 0; i < size; i++) {
		if (outpeer[i].getHash() == for_root) {

		}
	}
#endif
}
std::vector<Address> Selector::GetPeers(int type, const Sha1Hash& for_root) {
	std::vector<Address> peers;
	if (p_Btselector == NULL) {
		std::copy(inpeers.begin(), inpeers.end(), std::back_inserter(peers));
		return peers;
	}

#ifdef	BITSWIFT
	std::vector<ipPort_s> ip_port_list;
	//p_Btselector->getpeer(for_root.hex(), type, ip_port_list, 20);
	  p_Btselector->getPeerImmediately(for_root.hex(), type, ip_port_list,20);
	for (std::vector<ipPort_s>::const_iterator j = ip_port_list.begin(); j
			!= ip_port_list.end(); ++j) {
		if(j->port!=8000)continue;
		Address addr(j->ipAddress.c_str(), j->port);
		peers.push_back(addr);
	}
	return peers;
#else
	int size = outpeer.size();
	for (int i = 0; i < size; i++) {
		if (outpeer[i].getHash() == for_root) {

		}
	}
#endif
}
void Selector::SuspendPeer(const Address& addr, const Sha1Hash& root) {

}
void Selector::NeedPeer(bool need) {
	need_peer = need;
}
Address Selector::GetBestPeer() {
	IPProfile peer = outpeer.front();
	outpeer.pop_front();
	peer.setStatus(1);
	outpeer.push_front(peer);
	Address addr(peer.getIP().c_str(), 8000);
	return addr;
}
void Selector::UpdatePeerList() {
	int size = outpeer.size();
	if (size == 1)
		return;
	IPProfile peer = outpeer.back();
	outpeer.pop_back();
	--size;
	int score = peer.getScore();
	deque<IPProfile>::iterator it = outpeer.begin();
	for (int i = 0; i <= size; i++) {
		if (i == size) {
			outpeer.push_back(peer);
			return;
		}
		int scoreTemp = outpeer[i].getScore();
		if (score >= scoreTemp) {
			outpeer.insert(it, peer);
			break;
		}
	}
}
void Selector::ScorePeer(IPProfile &peer) {
	bool sameCountry = false;
	bool sameCity = false;
	bool sameAsn = false;
	int score = 0;

	if (peer.getCountry() == this->localprofile->getCountry())
		sameCountry = true;
	score += (sameCountry ? 10 : 0);
	peer.setScore(score);
}
void Selector::ScorePeers() {
	int size = outpeer.size();
	for (int i = 0; i < size; i++) {
		ScorePeer(outpeer[i]);
	}

}
