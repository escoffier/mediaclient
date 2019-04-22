// mediaclinet.cpp : �������̨Ӧ�ó������ڵ㡣
//
#define WIN32_LEAN_AND_MEAN
//#include "stdafx.h"
//#include <winsock2.h>
//#include<windows.h>
#include <Ice/Ice.h>
#include <IceGrid/IceGrid.h>
#include <stream.h>
#include <memory>
#include "jrtplib3/rtpsession.h"
#include "jrtplib3/rtpudpv4transmitter.h"
#include "jrtplib3/rtpipv4address.h"
#include "jrtplib3/rtpsessionparams.h"
#include "jrtplib3/rtppacket.h"
#include "jrtplib3/rtpsourcedata.h"
#define RTP_MAX_PACKET_LEN 1450

using namespace jrtplib;
using namespace std;

FILE *streamfile = fopen("stream106client", "wb");

void checkerror(int rtperr)
{
	if (rtperr < 0)
	{
		std::cout << "ERROR: " << RTPGetErrorString(rtperr) << std::endl;
		exit(-1);
	}
}

class MyRTPSession : public RTPSession
{
  public:
	void createSession(int port);

  protected:
	void OnPollThreadStep();
	void ProcessRTPPacket(const RTPSourceData &srcdat, const RTPPacket &rtppack);

	RTPUDPv4TransmissionParams transparams;
};

void MyRTPSession::OnPollThreadStep()
{
	BeginDataAccess();

	// check incoming packets
	if (GotoFirstSourceWithData())
	{
		do
		{
			RTPPacket *pack;
			RTPSourceData *srcdat;

			srcdat = GetCurrentSourceInfo();

			while ((pack = GetNextPacket()) != NULL)
			{
				ProcessRTPPacket(*srcdat, *pack);
				DeletePacket(pack);
			}
			//printf("Got packet !\n");
		} while (GotoNextSourceWithData());
	}
	//printf("OnPollThreadStep !\n");
	EndDataAccess();
}

void MyRTPSession::ProcessRTPPacket(const RTPSourceData &srcdat, const RTPPacket &rtppack)
{
	static int count = 0;
	++count;
	uint32_t dataLength = (uint32_t)rtppack.GetPayloadLength();
	uint8_t *rawdata = rtppack.GetPayloadData();
	fwrite(rawdata, 1, dataLength, streamfile);
	// You can inspect the packet and the source's info here
	if(count%200 == 0){
		std::cout << "Got packet " << rtppack.GetExtendedSequenceNumber() << " from SSRC " << srcdat.GetSSRC() << std::endl;
	}
}

void MyRTPSession::createSession(int portbase)
{
	RTPSessionParams sessparams;
	sessparams.SetOwnTimestampUnit(1.0 / 90000.0);

	transparams.SetPortbase(portbase);

	int status = Create(sessparams,&transparams);	
	checkerror(status);
}

class MeidaClient : public Ice::Application
{
  public:
	virtual int run(int, char *[]);
};

void menu()
{
	cout << "usage:\n"
			"t: send greeting\n"
			"s: shutdown server\n"
			"x: exit\n"
			"?: help\n";
}

void openexception(::std::exception_ptr ex)
{
	try
	{
		std::rethrow_exception(ex);
	}
	catch (const ::Media::OpenStreamException &e)
	{
		std::cout << "OpenStreamException: " << e.reason << endl;
	}
	catch (const Ice::Exception &e)
	{
		std::cout << e.what() << endl;
	}
}

enum PROXT_TYPE {
    GRID,
    DISCOVERY,
    DIRECT
};

int run(const shared_ptr<Ice::Communicator> &communicator, PROXT_TYPE proxyType)
{
	
	std::shared_ptr<Media::StreamPrx> stream;
    switch (proxyType) {
    case GRID:
        try
        {
            stream = Ice::checkedCast<Media::StreamPrx>(communicator->stringToProxy("stream"));
        }
        catch (const Ice::NotRegisteredException &)
        {
            auto query = Ice::checkedCast<IceGrid::QueryPrx>(communicator->stringToProxy("MediaGrid/Query"));
            stream = Ice::checkedCast<Media::StreamPrx>(query->findObjectByType("::Media::Stream"));
        }
        catch (const Ice::NoEndpointException &e)
        {
            std::cerr << "-----------------" << std::endl;
            std::cerr << e.what() << std::endl;
        }
        break;
    case DISCOVERY:
        try {
        stream = Ice::checkedCast<Media::StreamPrx>(communicator->stringToProxy("Stream"));
    } catch(const Ice::NoEndpointException &e) {
           std::cerr << e.what() << std::endl;
           return EXIT_FAILURE;
        }

        break;

    case DIRECT:
        stream = Ice::uncheckedCast<Media::StreamPrx>(communicator->propertyToProxy("Stream.Proxy"));
        break;
    default:
        break;
    }

	if (!stream)
	{
		std::cerr << "couldn't find a `::Media::Stream' object." << std::endl;
		return EXIT_FAILURE;
	}

	stream->ice_connectionCached(false);
	stream->ice_getConnection()->setACM(30, Ice::nullopt, Ice::ACMHeartbeat::HeartbeatOnIdle);
	menu();

	MyRTPSession sess;

	std::string callid;
	std::string id;

	char c = 'x';
	do
	{
		try
		{
			cout << "---==> ";
			cin >> c;
			if (c == 't')
			{
				int portbase;
				cin >> portbase;
				std::cout << std::endl;
				sess.createSession(portbase);
				//hello->sayHello();
				Media::RealStreamReqParam req;
				req.id = "60000000001310001430";
				req.ip = "192.168.21.236";
				req.port = 8000;
				req.userName = "admin";
				req.password = "dtnvs3000";
				req.destip = "192.168.21.197";
				req.destport = portbase;
				req.ssrc = 100001;
				req.pt = 96;

				id = req.id;
				std::cout << "---openRealStream--- port: " <<portbase<< std::endl;
				try
				{
					stream->openRealStreamAsync(req, [&callid](bool success, Media::RealStreamRespParam resp) {
						if (!success)
						{
							std::cout << "openRealStreamAsync failed" << std::endl;
							return;
						}
						callid = resp.callid;

						std::cout << "*****StreamInfo of the server: " << resp.id << "--" << resp.callid << std::endl;
					},
												openexception);
				}
				catch (const Ice::Exception &e)
				{
					std::cout << "Exception: " << e.what() << endl;
				}

				std::cout << "test \n";
				// try{
				// 	auto fut = stream->openRealStreamAsync(req);
				// 	std::cout << "StreamInfo of the server(future): " << fut.get().id << "--" << fut.get().callid << std::endl;
				// }
				// catch (const Media::OpenStreamException &e)
				// {
				// 	std::cout << "OpenStreamException: " << e.reason << endl;
				// }
				// catch (const Ice::Exception &e)
				// {
				// 	std::cout << "Exception: " << e.what() << endl;
				// }
				// catch (const std::exception &e)
				// {
				// 	std::cout << "exception: " << e.what() << endl;
				// }
			}
			else if (c == 'd')
			{
				stream->closeStream(id, callid);
				sess.BYEDestroy(RTPTime(3,0),0,0);
				std::cout << "close stream " << std::endl;
			}
			else if (c == 'c')
			{
				cout << "conncetion: " << stream->ice_getConnection()->toString();
				//stream->close("close connection");
				stream->ice_getConnection()->close(Ice::ConnectionClose::GracefullyWithWait);
			}
			else if (c == 's')
			{
				//stream->shutdown();
			}
			else if (c == 'x')
			{
				// Nothing to do
			}
			else if (c == '?')
			{
				menu();
			}
			else
			{
				cout << "unknown command `" << c << "'" << endl;
				menu();
			}
		}
		catch (const Media::OpenStreamException &e)
		{
			cout << e.reason << endl;
		}
		catch (const Ice::Exception &ex)
		{
			cerr << ex << endl;
		}

	} while (cin.good() && c != 'x');

	return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
	//MeidaClient app;
	//return app.main(argc, argv, "config.client");
	int status = EXIT_SUCCESS;
	try
	{
		//
		// CommunicatorHolder's ctor initializes an Ice communicator,
		// and its dtor destroys this communicator.
		//
		//Ice::CommunicatorHolder ich(argc, argv, "config.client");
        PROXT_TYPE proxyType;
        std::string configfile;
        if( strcmp(argv[1], "discovery") == 0)
        {
            proxyType = DISCOVERY;
            configfile = "config-discovery.client";
        }
        else if(strcmp(argv[1], "grid") == 0)
        {
            proxyType = GRID;
            configfile = "gridconfig.client";
        }
        else {
            proxyType = DIRECT;
            configfile = "config.client";
        }
        std::shared_ptr<Ice::Communicator> communicator = Ice::initialize(configfile);
		//
		// The communicator initialization removes all Ice-related arguments from argc/argv
		//
        if (argc > 2)
		{
			cerr << argv[0] << ": too many arguments" << endl;
			status = EXIT_FAILURE;
		}
		else
		{
            status = run(communicator, proxyType);
		}
	}
	catch (const std::exception &ex)
	{
		cerr << argv[0] << ": " << ex.what() << endl;
		status = EXIT_FAILURE;
	}

#if 0
	int status = 0;
	try
	{

		Ice::CommunicatorPtr communicator = Ice::initialize(argc, argv);
		Ice::PropertiesPtr props = communicator->getProperties();
		props->load("config.client");
		//
		// Create a proxy to the well-known object with the `pricing'
		// identity.
		//
		//std::shared_ptr<Media::StreamPrx> stream = Ice::uncheckedCast<Media::StreamPrx>(communicator->propertyToProxy("Stream.Proxy"));
		std::shared_ptr<Media::StreamPrx> stream;
		try
		{
			stream = Ice::checkedCast<Media::StreamPrx>(communicator->stringToProxy("stream"));
		}
		catch (const Ice::NotRegisteredException&)
		{
			auto query = Ice::checkedCast<IceGrid::QueryPrx>(communicator->stringToProxy("MediaGrid/Query"));
			stream = Ice::checkedCast<Media::StreamPrx>(query->findObjectByType("::Media::Stream"));
		}
		catch (const Ice::NoEndpointException &e)
		{
			std::cerr << e.what() << std::endl;
		}

		if (!stream)
		{
			std::cerr << "couldn't find a `::Demo::Hello' object." << std::endl;
			return EXIT_FAILURE;
		}

		//
		// If no context is set on the default locator (with the
		// Ice.Default.Locator.Context property, see the comments from the
		// `config.client' file in this directory), ask for the preferred
		// currency.
		//
		//Ice::Context ctx = communicator()->getDefaultLocator()->ice_getContext();
		//if (ctx["currency"].empty())
		//{
		//	cout << "enter your preferred currency (USD, EUR, GBP, INR, AUD, JPY): ";
		//	string currency;
		//	cin >> currency;

		//	//
		//	// Setup a locator proxy with a currency context.
		//	//
		//	Ice::LocatorPrx locator = communicator()->getDefaultLocator();
		//	ctx["currency"] = currency;
		//	pricing = pricing->ice_locator(locator->ice_context(ctx));
		//}
		//else
		//{
		//	cout << "Preferred currency configured for the client: " << ctx["currency"] << endl;
		//}

		//
		// Get the preferred currencies of the server
		//
		Media::RealStreamReqParam req;
		req.id = "60000000001310001430";
		req.ip = "192.168.254.106";
		req.port = 8000;
		req.name = "admin";
		req.pwd = "dtnvs3000";
		req.destip = "192.168.254.233";
		req.destport = 10098;
		req.ssrc = 100001;
		req.pt = 96;
		
		Media::RealStreamRespParam resp;
		stream->openRealStream(req, resp);
		//Ice::StringSeq currencies = pricing->getPreferredCurrencies();
		std::cout << "StreamInfo of the server: " << resp.id << "--" << resp.callid << std::endl;
		//for (Ice::StringSeq::const_iterator p = currencies.begin(); p != currencies.end(); ++p)
		//{
		//	cout << *p;
		//	if (p + 1 != currencies.end())
		//	{
		//		cout << ", ";
		//	}
		//}
		//cout << endl;
	}
	catch (const Ice::Exception& e) {
		std::cerr << e << std::endl;
		status = 1;
	}
	catch (const std::string& msg) {
		std::cerr << msg << std::endl;
		status = 1;
	}
	catch (const char* msg) {
		std::cerr << msg << std::endl;
		status = 1;
	}
#endif
	// RTPUDPv4TransmissionParams transParams;
	// RTPSessionParams sessionParams;

	// transParams.SetRTPSendBuffer(65535); // default: 32768
	// transParams.SetPortbase(10098);

	// sessionParams.SetOwnTimestampUnit(1.0 / 90000.0); //RTP_TIMESTAMP_UNIT);
	// sessionParams.SetAcceptOwnPackets(true);
	// sessionParams.SetMaximumPacketSize(RTP_MAX_PACKET_LEN);

	// MyRTPSession rtpss_;
	// int ret = rtpss_.Create(sessionParams, &transParams);
	// checkerror(ret);
	// //rtpss_.SetDefaultPayloadType(96);
	// //rtpss_.SetDefaultMark(true);
	// //rtpss_.SetDefaultTimestampIncrement(3600);

	// RTPTime::Wait(RTPTime(20, 0));

	// rtpss_.BYEDestroy(RTPTime(10, 0), 0, 0);
#if 0
	rtpss_.BeginDataAccess();

	// check incoming packets
	if (rtpss_.GotoFirstSourceWithData())
	{
		do
		{
			RTPPacket *pack;

			while ((pack = rtpss_.GetNextPacket()) != NULL)
			{
				// You can examine the data here
				printf("Got packet !\n");

				// we don't longer need the packet, so
				// we'll delete it
				rtpss_.DeletePacket(pack);
			}
		} while (rtpss_.GotoNextSourceWithData());
	}

	rtpss_.EndDataAccess();
#endif

	char c = 'x';
	do
	{
		std::cin >> c;
	} while (std::cin.good() && c != 'x');
	//system("pause");
	return status;
}

int MeidaClient::run(int argc, char *argv[])
{
	if (argc > 1)
	{
		std::cerr << appName() << ": too many arguments" << std::endl;
		return EXIT_FAILURE;
	}
	int status = 0;
	try
	{

		Ice::CommunicatorPtr communicator = Ice::initialize(argc, argv);
		Ice::PropertiesPtr props = communicator->getProperties();
		props->load("config.server");

		//
		// Create a proxy to the well-known object with the `pricing'
		// identity.
		//
		std::shared_ptr<Media::StreamPrx> stream = Ice::uncheckedCast<Media::StreamPrx>(communicator->propertyToProxy("Stream.Proxy"));
		if (!stream)
		{
			std::cerr << argv[0] << ": couldn't find a `::Demo::PricingEngine' object." << std::endl;
			return EXIT_FAILURE;
		}

		//
		// If no context is set on the default locator (with the
		// Ice.Default.Locator.Context property, see the comments from the
		// `config.client' file in this directory), ask for the preferred
		// currency.
		//
		//Ice::Context ctx = communicator()->getDefaultLocator()->ice_getContext();
		//if (ctx["currency"].empty())
		//{
		//	cout << "enter your preferred currency (USD, EUR, GBP, INR, AUD, JPY): ";
		//	string currency;
		//	cin >> currency;

		//	//
		//	// Setup a locator proxy with a currency context.
		//	//
		//	Ice::LocatorPrx locator = communicator()->getDefaultLocator();
		//	ctx["currency"] = currency;
		//	pricing = pricing->ice_locator(locator->ice_context(ctx));
		//}
		//else
		//{
		//	cout << "Preferred currency configured for the client: " << ctx["currency"] << endl;
		//}

		//
		// Get the preferred currencies of the server
		//
		Media::RealStreamReqParam req;
		;

		req.id = "60000000001310001430";
		// req.ip = "192.168.254.106";
		// req.port = 8000;
		// req.name = "admin";
		// req.pwd = "dtnvs3000";
		req.destip = "192.168.21.249";
		req.destport = 10098;
		req.ssrc = 100001;

		Media::RealStreamRespParam resp;
		stream->openRealStream(req, resp);
		//Ice::StringSeq currencies = pricing->getPreferredCurrencies();
		std::cout << "---StreamInfo of the server: " << resp.id << "--" << resp.callid << std::endl;
		//for (Ice::StringSeq::const_iterator p = currencies.begin(); p != currencies.end(); ++p)
		//{
		//	cout << *p;
		//	if (p + 1 != currencies.end())
		//	{
		//		cout << ", ";
		//	}
		//}
		//cout << endl;
	}
	catch (const Ice::Exception &e)
	{
		std::cerr << e << std::endl;
		status = 1;
	}
	catch (const std::string &msg)
	{
		std::cerr << msg << std::endl;
		status = 1;
	}
	catch (const char *msg)
	{
		std::cerr << msg << std::endl;
		status = 1;
	}
	return status;
}
