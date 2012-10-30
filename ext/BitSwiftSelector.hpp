#include <GeoIP.h>
#include <GeoIPCity.h>
#include <sstream>
#include <curl/curl.h>
#include <stdlib.h>
#include <iostream>
using namespace std;
#include <fstream>
#include <iomanip>
#define _USE_MATH_DEFINES
#include <cmath>
#include <string.h>
#include <vector>
#include <mysql++.h>
#define DB "peerselector" //database name
#define HOST "localhost" // so, where's your mysql server?
#define USERNAME "root" // a user granted access to the above database?
#define PASSWORD "123456" // enter the password for the above user. If there's no password, leave it as it is...

struct ipPort_s
{
    string ipAddress;
    int port;
};

struct ipAddress_s
{ 
    char* ipAddress;
    int port;
    double score;
};

struct rtt_s
{ 
    char* ipAddress;
    int port;
    double rtt;
};

struct asHop_s
{ 
    char* ipAddress;
    int port;
    int hopcount;
};

struct temp_data{
    string *urgentlist;
    int length;
};

    
//std::vector<ipPort_s>ipPortList;

class BitSwiftSelector{
public:
    BitSwiftSelector();
    ~BitSwiftSelector();
	void addpeer(string ip, int port, string hash);
	void deletepeer(string ip, int port, string hash, int type = 100);
	void deletepeers(string hash, int type = 100);
	void getpeer(string hash, int type, std::vector<ipPort_s> &ip_port, int count = 0);
    void getPeerImmediately(string hash, int type, std::vector<ipPort_s> &ip_port, int count = 0);
protected:
    double calculateDistance(double peerLatitude, double peerLongitude, double myLatitude, double myLongitude);
    void getRecords(char* ipAddress, string &continent, string & country, string &city);
    bool isInSameCity(char* peerIp);
    bool isInSameContinent(char* peerIp);
    bool isInSameCountry(char* peerIp);
    char * getProvider(char* ipAddress);
    bool isInSameProvider(char* peerIp);
    int getASN(const char* ipAddress);
    bool isInSameASN(char* peerIp);
    int calculateScore(char* peerIp);
    void initialize();
    void getCoordinates(char *ip, double &latitude, double & longitude);
    void sortIP(ipAddress_s ipAddressList[], int length);
    void sortRTT(rtt_s rttList[], int length);
	void sortAsHop(asHop_s ashopList[], int length);
    double calculateRtt(char* ipAddress);
	int calculateHopCount(char* ipAddress1, char* ipAddress2);
    void getIpAddress(std::vector<ipPort_s> &list, string hash);
    void storePeerInScoreTable(ipAddress_s ipAddressList[], int length, string hash);
    void storePeerInRttTable(rtt_s ipAddressList[], int length, string hash);
    void storePeerInAsHopTable(asHop_s asHopList[], int length, string hash);
    static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream);

private:
    static char *m_myIp;
    GeoIP *m_gi;
    GeoIP *m_ASgi;
};

