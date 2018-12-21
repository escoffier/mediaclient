#!/usr/bin/env python
# **********************************************************************
#
# Copyright (c) 2003-2017 ZeroC, Inc. All rights reserved.
#
# **********************************************************************

import sys, traceback, Ice

Ice.loadSlice('./stream.ice')
import Media

def run(communicator):
    twoway = Media.StreamPrx.checkedCast(\
        communicator.propertyToProxy('Stream.Proxy').ice_twoway().ice_secure(False))
		#communicator.stringToProxy("stream"))
    if not twoway:
        print("invalid proxy")
        return 1

    oneway = Media.StreamPrx.uncheckedCast(twoway.ice_oneway())
    batchOneway = Media.StreamPrx.uncheckedCast(twoway.ice_batchOneway())
    datagram = Media.StreamPrx.uncheckedCast(twoway.ice_datagram())
    batchDatagram = Media.StreamPrx.uncheckedCast(twoway.ice_batchDatagram())

    secure = False
    timeout = -1
    delay = 0
    reqparam = Media.RealStreamReqParam()
    reqparam.ip = "192.168.21.236"
    reqparam.port = 8000
    reqparam.name = "admin"
    reqparam.pwd = "dtnvs3000"
    reqparam.destip= "192.168.21.121"
    reqparam.destport = 10889;
    reqparam.id = "60000000001310001430";
    reqparam.callid = "xdededdde122"
    reqparam.ssrc = 1024;
    reqparam.pt = 96
    
    #stat = Media.StreamStatic()
    resp = Media.RealStreamRespParam()
    menu()

    c = None
    while c != 'x':
        try:
            sys.stdout.write("==> ")
            sys.stdout.flush()
            c = sys.stdin.readline().strip()
            if c == 't':
                resp = twoway.openRealStream(reqparam)
                print("callid: " + resp.callid + "  sourceip: " + resp.sourceip + "  sourceport" + resp.sourceport)
            elif c == 'c':
                twoway.closeStream(resp.callid, resp.id)
            elif c == 'o':
                oneway.openRealStream(reqparam)
            elif c == 'g':
                stat = twoway.getStreamStatic(reqparam.id)
                print("busy number: " + str(stat.busynode) + "  freenode: " + str(stat.freenode))
            elif c == 'O':
                batchOneway.openRealStream(reqparam)
            elif c == 'd':
                if secure:
                    print("secure datagrams are not supported")
                else:
                    datagram.openRealStream(reqparam)
            elif c == 'D':
                if secure:
                    print("secure datagrams are not supported")
                else:
                    batchDatagram.openRealStream(reqparam)
            elif c == 'f':
                batchOneway.ice_flushBatchRequests()
                batchDatagram.ice_flushBatchRequests()
            elif c == 'T':
                if timeout == -1:
                    timeout = 2000
                else:
                    timeout = -1

                twoway = Media.StreamPrx.uncheckedCast(twoway.ice_invocationTimeout(timeout))
                oneway = Media.StreamPrx.uncheckedCast(oneway.ice_invocationTimeout(timeout))
                batchOneway = Media.StreamPrx.uncheckedCast(batchOneway.ice_invocationTimeout(timeout))

                if timeout == -1:
                    print("timeout is now switched off")
                else:
                    print("timeout is now set to 2000ms")
            elif c == 'P':
                if delay == 0:
                    delay = 2500
                else:
                    delay = 0

                if delay == 0:
                    print("server delay is now deactivated")
                else:
                    print("server delay is now set to 2500ms")
            elif c == 'S':
                secure = not secure

                twoway = Media.StreamPrx.uncheckedCast(twoway.ice_secure(secure))
                oneway = Media.StreamPrx.uncheckedCast(oneway.ice_secure(secure))
                batchOneway = Media.StreamPrx.uncheckedCast(batchOneway.ice_secure(secure))
                datagram = Media.StreamPrx.uncheckedCast(datagram.ice_secure(secure))
                batchDatagram = Media.StreamPrx.uncheckedCast(batchDatagram.ice_secure(secure))

                if secure:
                    print("secure mode is now on")
                else:
                    print("secure mode is now off")
            elif c == 's':
                twoway.shutdown()
            elif c == 'x':
                pass # Nothing to do
            elif c == '?':
                menu()
            else:
                print("unknown command `" + c + "'")
                menu()
        except KeyboardInterrupt:
            return 1
        except EOFError:
            return 1
        except Ice.Exception as ex:
            print(ex)

    return 0

def menu():
    print("""
usage:
t: send greeting as twoway
o: send greeting as oneway
O: send greeting as batch oneway
d: send greeting as datagram
D: send greeting as batch datagram
f: flush all batch requests
T: set a timeout
P: set a server delay
S: switch secure mode on/off
s: shutdown server
x: exit
?: help
""")

status = 0
with Ice.initialize(sys.argv, "config.client") as communicator:
    if len(sys.argv) > 1:
        print(sys.argv[0] + ": too many arguments")
        status = 1
    else:
        status = run(communicator)
sys.exit(status)
