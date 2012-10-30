/*
 * selector.h
 *
 *  Created on: Nov 21, 2011
 *      Author: fu tang
 */

#ifndef SELECTOR_H_
#define SELECTOR_H_
#define BITSWIFT
#include<string>
#ifdef BITSWIFT
#include "BitSwiftSelector.hpp"
#endif
using namespace std;
//#include "../swift.h"
extern "C" {
#include <getopt.h>
#include"GeoIP.h"
#include "../datagram.h"
}

using namespace swift;

class IPProfile {
public:
	IPProfile(string ip);
	IPProfile(string ip, string country);
	string getCountry();
	string getCity();
	string getAsn();
	string getIP();
	int getScore();
	int getStatus();
	Sha1Hash getHash();
	void setScore(int score);
	void setStatus(int status);
	void setHash(const Sha1Hash& root);
private:
	string ip;
	string country;
	string city;
	string asn;
	int score;
	int status;
	Sha1Hash root;
};

class Selector {
public:
	deque<Address> candidates;
	deque<IPProfile> outpeer;
	//deque<Address> inpeers;

public:
	Selector();
	void AddPeer(const Address& addr, const Sha1Hash& root);
	void DelPeer(const Address& addr, const Sha1Hash& root);
	void SuspendPeer(const Address& addr, const Sha1Hash& root);
	Address GetPeer(const Sha1Hash& for_root);
	void NeedPeer(bool need);
	bool IsNeedPeer(){return need_peer;}
	std::vector<Address> GetPeers(int type,const Sha1Hash& for_root);
	Address GetBestPeer();
private:
	IPProfile *localprofile;
	GeoIP * gicountry;
	GeoIP * gicity;
	GeoIP * giasn;
	bool need_peer;
	deque<Address> inpeers;
#ifdef BITSWIFT
	BitSwiftSelector *p_Btselector;
#endif
private:
	void UpdatePeerList();
	void ScorePeer(IPProfile &peer);
	void ScorePeers();
};

#endif /* SELECTOR_H_ */
