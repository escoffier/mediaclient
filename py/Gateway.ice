// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#pragma once

module Gateway
{

exception RequestCanceledException
{
};

exception OpenStreamException
{
    string reason;
};

exception DeviceControlException
{
    string reason;
};

struct RealStreamReqParam
{
    string id;
	string callid;
    string destip;
	int destport;
	int pt;
	int ssrc;
};

struct RealStreamRespParam
{
    string id;
	string callid;
	string sourceip;
	string sourceport;
	
};

struct DeviceInfo
{
    string id;
};

struct DeviceStatus
{
    string id;
	int status;
};

interface DeviceControl
{
	["amd"] void openRealStream(RealStreamReqParam req, out RealStreamRespParam resp)
	throws OpenStreamException;

	["amd"] void closeStream(string callid, string id);

    ["amd"] void ptzControl(string id, string cmd)
	throws DeviceControlException;

	["amd"] void getDeviceInfo(string id, out DeviceInfo info)
	throws DeviceControlException;

	["amd"] void getDeviceStatus(string id, out DeviceStatus status)
	throws DeviceControlException;

	bool login(string user, string password);

    void shutdown();
	
	  //登录
    //["amd","ami"] void login(
	//string userName
	//,string userPass
	//,out bool bSuccess);
    
	["amd","ami"] void beatHeart(
	string info
	,out string rinfo);

	//呼叫请求 SIPID 和 呼叫类型
    ["amd","ami"] void CallSipUserReq(
	string pSzSipData
	,int iType
	,string pSzIp
	,int iport
	,out int iResMsglen
	,out string sResMsgbuf);

    //校时 SIPid 和 校对时间
    ["amd","ami"] void CalibrationTimeReq(
	string pSzSipData
	,out int iResMsglen
	,out string sResMsgbuf);

    //云台控制 SIPID 和 操作类型
    ["amd","ami"] void HolderOperReq(
	string pSzSipData
	,string pSzTypeOper
	,out int iResMsglen
	,out string sResMsgbuf);

    //录像请求 SIPID
    ["amd","ami"] void RecordingOperReq(
	string pSzSipData
	,out int iResMsglen
	,out string sResMsgbuf);

    //停止录像请求 SIPID
    ["amd","ami"] void StopRecordingOperReq(
	string pSzSipData
	,out int iResMsglen
	,out string sResMsgbuf);

    //播放历史文件链接播放的服务端
    ["amd","ami"] void StartVoideRecoingFile(
	string pSzIp
	, int iport
	,out int iResMsglen
	,out string sResMsgbuf);

    //录像文件回放 SIPID 开始时间 结束时间 类型
    ["amd","ami"] void RecordingFileReq(
	string pSzSipData
	,string pSzStartTime
	,string pSzEndTime
	,int iType
	,out int iResMsglen
	,out string sResMsgbuf);

    //下载文件
    ["amd","ami"] void LoadRecordingFile(
	 string pSzSipIdVal
	,string pSzStarTime
	,string pSzEndTime
	,int sbType
	,string pSzFilePath
	,out int iResMsglen
	,out string sResMsgbuf);

    //录像文件查找 SIPID 开始时间 结束时间
    ["amd","ami"] void FindRecordingFileReq(
	string pSzSipData
	,string pSzStartTime
	,string pSzEndTime
	,out int iResMsglen
	,out string sResMsgbuf);

    //停止开始SIPID 
    ["amd","ami"] void DataPlayStopOrStartReq(
	string pSzSipData
	,out int iResMsglen
	,out string sResMsgbuf);

    //布防请求
    ["amd","ami"] void SetupAlarmReq(
	string pSzSipData
	,out int iResMsglen
	,out string sResMsgbuf);

    //撤防请求
    ["amd","ami"] void CloseAlarmReq(
	string pSzSipData
	,out int iResMsglen
	,out string sResMsgbuf);

    //恢复设备默认参数请求
    ["amd","ami"] void RestorConfigReq(
	string pSzSipData
	,out int iResMsglen
	,out string sResMsgbuf);
    
    //设备重起请求
    ["amd","ami"] void RebootReq(
	string pSzSipData
	,out int iResMsglen
	,out string sResMsgbuf);

    //远程控制请求
    ["amd","ami"] void RemoteControlReq(
	string pSzSipData
	,out int iResMsglen
	,out string sResMsgbuf);

    //开启语音请求信息
    ["amd","ami"] void MsgStartVioceReq(string pSzSipData
	,out int iResMsglen
	,out string sResMsgbuf);

    //停止语音请求信息
    ["amd","ami"] void MsgStopVioceReq(
	string pSzSipData
	,out int iResMsglen
	,out string sResMsgbuf);

    //查询设备ipc状态请求
    ["amd","ami"] void MsgQueryDeviceIpcStatusReq(
	string pSzSipData
	,out int iResMsglen
	,out string sResMsgbuf);

    //查询设备ip信息请求
    ["amd","ami"] void MsgQueryDeviceIpcInfoReq(
	string pSzSipData
	,out int iResMsglen
	,out string sResMsgbuf);

    //开始收取服务器性能信息请求
    ["amd","ami"] void MsgStartPropertyServerReq(
	string pSzSipData
	,out int iResMsglen
	,out string sResMsgbuf);

    //停止收取服务器性能信息请求
    ["amd","ami"] void MsgStopPropertyServerReq(
	string pSzSipData
	,out int iResMsglen
	,out string sResMsgbuf);

    //设备刷新请求
    ["amd","ami"] void DeviceRefreshReq(
	string pSzSipData
	,out int iResMsglen
	,out string sResMsgbuf);

    //设备共享通知
    ["amd","ami"] void DeviceShareNotify(
	string pSzSipData
	,out int iResMsglen
	,out string sResMsgbuf);

    //网关重启请求
   ["amd","ami"]  void GatewayRerootReq(
   string pSzSipData
   ,out int iResMsglen
   ,out string sResMsgbuf);
};


};
