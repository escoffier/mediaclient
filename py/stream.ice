
#pragma once

module Media
{

exception RequestCanceledException
{
};
exception OpenStreamException
{
    string callid;
    string reason;
};
struct RealStreamRespParam
{
    string id;
	string callid;
	string sourceip;
	string sourceport;
};
struct RealStreamReqParam
{
    string id;
	string callid;
	string ip;  //camara ip
	int port;    //camara control port number
	string name;  //camara login username
	string pwd;
	string destip;  // ip
	int destport;    //port number
	int ssrc;
	int pt;
	string sdk;
	//DEVICETYPE type = HAIKANG;
};

struct StreamStatic
{
    int freenode;
	int busynode;
};

interface Stream
{
	["amd"] void openRealStream(RealStreamReqParam ctg, out RealStreamRespParam stm)
	throws OpenStreamException;

	["amd"] void closeStream(string callid, string id);

	void getStreamStatic( string id , out StreamStatic static);
};

};
