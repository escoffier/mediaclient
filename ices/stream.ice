
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
	string ip;
	int port;
	string userName;
	string password;
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
	["amd"] bool openRealStream(RealStreamReqParam ctg, out RealStreamRespParam stm);

	["amd"] void closeStream(string id, string callid);

	void getStreamStatic( string id , out StreamStatic static);

		//��ȡ¼���ļ��б�
	void getRecordFiles(string startTime, string endtime);

	//����¼���ļ�
	void openVodStream(RealStreamReqParam req, out RealStreamRespParam resp);

	//�رղ��ŵ�¼��
	void closeVodStream(string callid, string id);

	//¼����ƣ����ٲ��ţ�������˵ȣ�
	void controlVodStream(string cmd, string callid, string id);
};

};
