/*
 * main.cxx
 *
 * T38Modem simulator - main program
 *
 * Open H323 Project
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.0 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
 * the License for the specific language governing rights and limitations
 * under the License.
 *
 * The Original Code is Open H323 Library.
 *
 * The Initial Developer of the Original Code is Equivalence Pty Ltd
 *
 * Contributor(s): Vyacheslav Frolov
 *
 * $Log: h323ep.cxx,v $
 * Revision 1.12  2002-02-11 08:48:43  vfrolov
 * Changed some trace and cout messages
 *
 * Revision 1.12  2002/02/11 08:48:43  vfrolov
 * Changed some trace and cout messages
 *
 * Revision 1.11  2002/01/10 06:16:00  craigs
 * Added muLaw codec as well as ALaw
 *
 * Revision 1.10  2002/01/10 06:09:47  craigs
 * Changed to use RequestModeChangeT38
 * Added MPL header
 *
 * 
 */

// mostly "stolen" from OpenAM

#include <ptlib.h>
#include <ptlib/pipechan.h>

#include "version.h"
#include "h323t38.h"
#include "t38engine.h"
#include "pmodem.h"
#include "main.h"

PCREATE_PROCESS(T38Modem);

#define new PNEW

///////////////////////////////////////////////////////////////

T38Modem::T38Modem()
  : PProcess("OpenH323 Project", "T38Modem",
             MAJOR_VERSION, MINOR_VERSION, BUILD_TYPE, BUILD_NUMBER)
{
}


T38Modem::~T38Modem()
{
}

void T38Modem::Main()
{
  cout << GetName()
       << " Version " << GetVersion(TRUE)
       << " by " << GetManufacturer()
       << " on " << GetOSClass() << ' ' << GetOSName()
       << " (" << GetOSVersion() << '-' << GetOSHardware() << ")\n\n";

  PConfigArgs args(GetArguments());

  args.Parse(
             "f-fax."
	     "p-ptty:"
	     "-route:"

             "F-fastenable."
             "T-h245tunneldisable."

             "g-gatekeeper:"         "n-no-gatekeeper."
             "-require-gatekeeper."  "-no-require-gatekeeper."
             "h-help."
             "i-interface:"          "-no-interface."
             "-listenport:"          "-no-listenport."
             "-connectport:"         "-no-connectport."
#if PMEMORY_CHECK
             "-setallocationbreakpoint:"
#endif
#if PTRACING
             "t-trace."
             "o-output:"
#endif
             "r-run:"                "-no-run."
             "-save."
#if PTRACING
             "t-trace."
#endif
	     "u-username:"           "-no-username."
          , FALSE);

#if PMEMORY_CHECK
  if (args.HasOption("setallocationbreakpoint"))
    PMemoryHeap::SetAllocationBreakpoint(args.GetOptionString("setallocationbreakpoint").AsInteger());
#endif

#if PTRACING
  PTrace::Initialise(args.GetOptionCount('t'),
                     args.HasOption('o') ? (const char *)args.GetOptionString('o') : NULL);
#endif

  if (args.HasOption('h')) {
    cout << "Usage : " << GetName() << " [options]\n"
            "Options:\n"
            "  -p --ptty tty[,tty...]  : Pseudo ttys (mandatory). Can be used multiple times\n"
            "                            tty ~= |" << PseudoModem::ttyPattern() << "|\n"
            "     --route prefix@host  : route number with prefix to host. Can be used multiple times\n"
            "                            Discards prefix from number. Prefix 'all' is all numbers\n"
	    "                            Mandatory if not using GK\n"
            "  -i --interface ip       : Bind to a specific interface\n"
            "  --listenport port       : Listen on a specific port\n"
            "  --connectport port      : Connect to a specific port\n"
            "  -g --gatekeeper host    : Specify gatekeeper host.\n"
            "  -n --no-gatekeeper      : Disable gatekeeper discovery.\n"
            "  --require-gatekeeper    : Exit if gatekeeper discovery fails.\n"
            "  -F --fastenable         : Enable fast start\n"
            "  -T --h245tunneldisable  : Disable H245 tunnelling.\n"
#if PTRACING
            "  -t --trace              : Enable trace, use multiple times for more detail\n"
            "  -o --output             : File for trace output, default is stderr\n"
#endif
            "     --save               : Save arguments in configuration file\n"
            "  -h --help               : Display this help message\n";
    return;
  }

  args.Save("save");

  MyH323EndPoint endpoint;

  PString userName = "OpenH323 Answering Machine v" + GetVersion();
  if (args.HasOption('u'))
    userName = args.GetOptionString('u');
  endpoint.SetLocalUserName(userName);

  if (!endpoint.Initialise(args))
    return;


  // start the H.323 listener
  H323ListenerTCP * listener;
  PIPSocket::Address interfaceAddress(INADDR_ANY);
  WORD listenPort = H323ListenerTCP::DefaultSignalPort;

  if (args.HasOption("listenport"))
    listenPort = (WORD)args.GetOptionString("listenport").AsInteger();

  if (args.HasOption('i'))
    interfaceAddress = PIPSocket::Address(args.GetOptionString('i'));

  listener  = new H323ListenerTCP(endpoint, interfaceAddress, listenPort);

  if (!endpoint.StartListener(listener)) {
    cout <<  "Could not open H.323 listener port on "
         << listener->GetListenerPort() << endl;
    delete listener;
    return;
  }

  if (args.HasOption('g')) {
    PString gkName = args.GetOptionString('g');
    if (endpoint.SetGatekeeper(gkName, new H323TransportUDP(endpoint)))
      cout << "Gatekeeper set: " << *endpoint.GetGatekeeper() << endl;
    else {
      cout << "Error registering with gatekeeper at \"" << gkName << '"' << endl;
      return;
    }
  }
  else if (!args.HasOption('n')) {
    cout << "Searching for gatekeeper..." << flush;
    if (endpoint.DiscoverGatekeeper(new H323TransportUDP(endpoint)))
      cout << "\nGatekeeper found: " << *endpoint.GetGatekeeper() << endl;
    else {
      cout << "\nNo gatekeeper found." << endl;
      if (args.HasOption("require-gatekeeper"))
        return;
    }
  }

  cout << "Waiting for incoming calls for \"" << endpoint.GetLocalUserName() << '"' << endl;

  for (;;) 
    PThread::Sleep(5000);
}

///////////////////////////////////////////////////////////////

MyH323EndPoint::MyH323EndPoint()
{
  forceT38Mode = FALSE;
  pmodemQ = new PseudoModemQ();
  //autoStartTransmitFax = TRUE;
}

void MyH323EndPoint::OnMyCallback(PObject &from, INT extra)
{
  myPTRACE(1, "MyH323EndPoint::OnMyCallback " << from.GetClass() << " " << extra);
  if (from.IsDescendant(PStringToString::Class()) ) {
    PStringToString &request = (PStringToString &)from;
    PString command = request("command");
    PString modemToken = request("modemtoken");
    PString response = "reject";
  
    if (command == "dial" ) {
      PseudoModem *modem = pmodemQ->Dequeue(modemToken);
      if (modem != NULL ) {

        PString callToken;
        PString num = request("number");
        PString remote;

	if (GetGatekeeper() != NULL) {
          remote = num;
	} else {
          for( PINDEX i = 0 ; i < routes.GetSize() ; i++ ) {
            PString r = routes[i];
            PStringArray rs = r.Tokenise("@", FALSE);
            if( rs.GetSize() == 2 ) {
              if( rs[0] == "all" ) {
                remote = rs[1];
                break;
              } else if (num.Find(rs[0]) == 0) {
                remote = rs[1];
                num.Delete(0, rs[0].GetLength());
                break;
              }
            }
          }
        
          // add in the route and port if required
          if (!remote.IsEmpty()) {
            num += "@" + remote;
            if ((num.Find(':') == P_MAX_INDEX) && (connectPort != H323ListenerTCP::DefaultSignalPort))
	      num += psprintf(":%i", connectPort);
	  }
        }

        if (remote.IsEmpty()) 
          request.SetAt("diag", "noroute");
        else {
          PTRACE(1, "MyH323EndPoint::OnMyCallback MakeCall(" << num << ")");

	  // make the call
          MakeCall(num, callToken);

          request.SetAt("calltoken", callToken);
          H323Connection * _conn = FindConnectionWithLock(callToken);

          if (_conn == NULL ) 
            pmodemQ->Enqueue(modem);
          else {
            cout << "O/G connection to " << num << "\n";
            PAssert(_conn->IsDescendant(MyH323Connection::Class()), PInvalidCast);
            MyH323Connection *conn = (MyH323Connection *)_conn;
            if( conn->Attach(modem) )
              response = "confirm";
            else
              pmodemQ->Enqueue(modem);
            _conn->Unlock();
          }
        }
      }
    } else if( command == "answer" ) {
      PString callToken = request("calltoken");
      H323Connection * _conn = FindConnectionWithLock(callToken);
      if( _conn != NULL ) {
        PAssert(_conn->IsDescendant(MyH323Connection::Class()), PInvalidCast);
        MyH323Connection *conn = (MyH323Connection *)_conn;
        conn->AnsweringCall(H323Connection::AnswerCallNow);
        _conn->Unlock();
        response = "confirm";
      }
    } else if( command == "clearcall" ) {
      PString callToken = request("calltoken");
      if( ClearCall(callToken) ) {
        response = "confirm";
      }
    }
    request.SetAt("response", response);
    
    myPTRACE(1, "MyH323EndPoint::OnMyCallback request={\n" << request << "}");
  }
}

BOOL MyH323EndPoint::OnIncomingCall(H323Connection & /*_conn*/,
                                    const H323SignalPDU & setupPDU,
                                    H323SignalPDU &)
{
  PTRACE(1, "MyH323EndPoint::OnIncomingCall");
  /*{
    PStringStream s;
    s << "DumpStatistics {\n";
    PMemoryHeap::DumpStatistics(s);
    s << " }\n";
    PTRACE(1, "MyH323EndPoint::OnIncomingCall " << s);
  } // */

  return TRUE;
}

H323Connection * MyH323EndPoint::CreateConnection(unsigned callReference)
{
  return new MyH323Connection(*this, callReference);
}

PseudoModem * MyH323EndPoint::PMAlloc() const
{
  return pmodemQ->Dequeue();
}

void MyH323EndPoint::PMFree(PseudoModem *pmodem) const
{
  if( pmodem != NULL )
    pmodemQ->Enqueue(pmodem);
}

BOOL MyH323EndPoint::Initialise(PConfigArgs & args)
{
  DisableFastStart(!args.HasOption('F'));
  DisableH245Tunneling(args.HasOption('T'));

  if (args.HasOption("connectport"))
    connectPort = (WORD)args.GetOptionString("connectport").AsInteger();
  else
    connectPort = H323ListenerTCP::DefaultSignalPort;

  if (args.HasOption('f')) {
    forceT38Mode = TRUE;
    myPTRACE(1, "Force T38 mode !!!");
  }

  if (args.HasOption("route")) {
    PString r = args.GetOptionString("route");
    routes = r.Tokenise("\r\n", FALSE);

    cout << "Route O/G calls:\n";

    for( PINDEX i = 0 ; i < routes.GetSize() ; i++ ) {
      r = routes[i];
      PStringArray rs = r.Tokenise("@", FALSE);
      if( rs.GetSize() == 2 ) {
        cout << "  " << rs[0] << " --> " << rs[1] << "\n";
        PTRACE(1, "Route " << rs[0] << " --> " << rs[1]);
        if( rs[0] == "all" )
          break;
      }
    }
  }
  
  if (args.HasOption('p')) {
    PString tty = args.GetOptionString('p');
    PStringArray ttys = tty.Tokenise(",\r\n ", FALSE);
    
    for( PINDEX i = 0 ; i < ttys.GetSize() ; i++ ) {
      PseudoModem *modem = new PseudoModem(ttys[i], PCREATE_NOTIFIER(OnMyCallback));
      
      if( modem->IsValid() ) {
        if( pmodemQ->Find(modem->modemToken()) == NULL ) {
          pmodemQ->Enqueue(modem);
        } else {
          cout << "Can't add " << ttys[i] << " to queue, delete" << endl;
          delete modem;
        }
      } else {
        cout << ttys[i] << " in not valid" << endl;
        delete modem;
      }
    }
  }

  SetCapability(0, 0, new H323_G711Capability(H323_G711Capability::muLaw, H323_G711Capability::At64k));
  SetCapability(0, 0, new H323_G711Capability(H323_G711Capability::ALaw,  H323_G711Capability::At64k));

  SetCapability(0, 0, new H323_T38Capability(H323_T38Capability::e_UDP));
    
  capabilities.Remove(args.GetOptionString('D').Lines());
  capabilities.Reorder(args.GetOptionString('P').Lines());

  cout << "Codecs (in preference order):\n" << setprecision(2) << capabilities << endl;

  return TRUE;
}

void MyH323EndPoint::OnConnectionEstablished(H323Connection & /*connection*/,
                                                const PString & /*token*/)
{
  PTRACE(2, "MyH323EndPoint::OnConnectionEstablished");

  /* it's does not work here :(
  
  if( ForceT38Mode() && connection.HadAnsweredCall() ) {
    connection.RequestModeChangeT38();
  }
  */
}

///////////////////////////////////////////////////////////////

MyH323Connection::MyH323Connection(MyH323EndPoint & _ep, unsigned callReference)
  : H323Connection(_ep, callReference), ep(_ep),
    t38handler(NULL), T38TransportUDP(NULL), pmodem(NULL),
    audioWrite(NULL), audioRead(NULL)
{
}

MyH323Connection::~MyH323Connection()
{
  cout << "Closing connection" << endl;

  if (t38handler != NULL) {
    if (pmodem != NULL) {
      PAssert(t38handler->IsDescendant(T38Engine::Class()), PInvalidCast);
      pmodem->Detach((T38Engine *)t38handler);
    }
    delete t38handler;
  }

  if (T38TransportUDP != NULL)
    delete T38TransportUDP;

  if (pmodem != NULL) {
      PStringToString request;
      request.SetAt("command", "clearcall");
      request.SetAt("calltoken", GetCallToken());
      if( !pmodem->Request(request) ) {
        myPTRACE(1, "MyH323Connection::~MyH323Connection error request={\n" << request << "}");
      }
    
      ep.PMFree(pmodem);
  }

  if (audioWrite != NULL)
    delete audioWrite;

  if (audioRead != NULL)
    delete audioRead;
}

BOOL MyH323Connection::Attach(PseudoModem *_pmodem)
{
  if( pmodem != NULL )
    return FALSE;
  pmodem = _pmodem;
  return TRUE;
}

OpalT38Protocol * MyH323Connection::CreateT38ProtocolHandler() const
{
  PTRACE(2, "MyH323Connection::CreateT38ProtocolHandler");

  PAssert(pmodem != NULL, "pmodem is NULL");

  /*
  OpalT38Protocol * t38 = new T38Engine();
  pmodem->Attach((T38Engine *)t38);

  return t38;
  */

  PWaitAndSignal mutexWait(T38Mutex);
  /*
   * we can't have more then one t38handler per connection
   * at the same time and we should delete it on connection clean
   */
  if( t38handler == NULL ) {
    PTRACE(2, "MyH323Connection::CreateT38ProtocolHandler create new one");
    ((MyH323Connection *)this)->	// workaround for const
      t38handler = new T38Engine(pmodem->ptyName());
    pmodem->Attach((T38Engine *)t38handler);
  }
  return t38handler;
}
/*
H323TransportUDP * MyH323Connection::GetT38TransportUDP()
{
  PWaitAndSignal mutexWait(T38Mutex);
  if( T38TransportUDP == NULL ) {
      PIPSocket::Address ip;
      WORD port;
      if (!GetControlChannel().GetLocalAddress().GetIpAndPort(ip, port)) {
        PTRACE(2, "H323T38\tTrying to use UDP when base transport is not TCP/IP");
        PIPSocket::GetHostAddress(ip);
      }
    
      for( WORD localDataPort = 5000 ; localDataPort < 6000 ; localDataPort += 2 ) {
        T38TransportUDP = new H323TransportUDP(GetEndPoint(), ip, localDataPort);
        if( T38TransportUDP->IsOpen() ) {
          break;
        }
        myPTRACE(2, "myH323_T38Channel::CreateTransport transport=" << T38TransportUDP->GetLocalAddress() <<
            " Error: " << T38TransportUDP->GetErrorText());
            delete T38TransportUDP;
        T38TransportUDP = NULL;
      }
    myPTRACE(2, "myH323_T38Channel::CreateTransport transport=" << T38TransportUDP->GetLocalAddress());
    //  transportControl = new H323TransportUDP(connection.GetEndPoint(), ip, 0);  // ?????
    //  myPTRACE(2, "myH323_T38Channel::CreateTransport transportControl=" << transportControl->GetLocalAddress());
  }
  return T38TransportUDP;
}
*/
H323Connection::AnswerCallResponse
     MyH323Connection::OnAnswerCall(const PString & caller,
                                    const H323SignalPDU & setupPDU,
                                    H323SignalPDU & /*connectPDU*/)
{
  PString number;
  cout << "I/C connection\n";
  PTRACE(1, "I/C connection");
  
  if (setupPDU.GetSourceE164(number)) {
    cout << "From: " << number << "\n";
    PTRACE(1, "From: " << number);
  }

  if (setupPDU.GetDestinationE164(number)) {
    cout << "To:   " << number << "\n";
    PTRACE(1, "To: " << number);
}

  pmodem = ep.PMAlloc();

  if(pmodem == NULL) {
    myPTRACE(1, "... denied (all modems busy)");
    return AnswerCallDenied;
  }
  
  PString old = PThread::Current()->GetThreadName();
  PThread::Current()->SetThreadName(pmodem->ptyName() + "(c):%0x");
  PTRACE(2, "MyH323Connection::AnswerCallResponse old ThreadName=" << old);
    
  PStringToString request;
  request.SetAt("command", "call");
  request.SetAt("calltoken", GetCallToken());
    
  PString srcNum;
  if( setupPDU.GetSourceE164(srcNum) )
    request.SetAt("srcnum", srcNum);
      
  PString dstNum;
  if( setupPDU.GetDestinationE164(dstNum) )
    request.SetAt("dstnum", dstNum);
      
  unsigned distinctiveRing = setupPDU.GetDistinctiveRing();
  if( distinctiveRing )
    request.SetAt("distinctivering", psprintf("%u", distinctiveRing));
    
  if( !pmodem->Request(request) ) {
    myPTRACE(1, "... denied (modem is not ready)");	// or we can try other modem
    return AnswerCallDenied;
  }

  PString response = request("response");

  if( response == "confirm" ) {
    PString answer = request("answer");
    if( answer == "pending" ) {
      myPTRACE(1, "... Ok (AnswerCallPending)");
      return AnswerCallPending;
    }
    myPTRACE(1, "... Ok (AnswerCallNow)");
    return AnswerCallNow;
  }

  myPTRACE(1, "... denied (no confirm)");
  return AnswerCallDenied;
}

BOOL MyH323Connection::OnStartLogicalChannel(H323Channel & channel)
{
  myPTRACE(1, "MyH323Connection::OnStartLogicalChannel ch=" << channel << " cp=" << channel.GetCapability() << " sid=" << channel.GetSessionID() << " d=" << (int)channel.GetDirection());

  if (!H323Connection::OnStartLogicalChannel(channel))
    return FALSE;

  cout << "Started logical channel: ";

  switch (channel.GetDirection()) {
    case H323Channel::IsTransmitter :
      cout << "sending ";
      break;

    case H323Channel::IsReceiver :
      cout << "receiving ";
      break;

    default :
      break;
  }
  
  cout << channel << endl;
  
  return TRUE;
}

void MyH323Connection::OnClosedLogicalChannel(const H323Channel & channel)
{
  PTRACE(2, "MyH323Connection::OnClosedLogicalChannel beg");
  
  H323Connection::OnClosedLogicalChannel(channel);

  myPTRACE(1, "MyH323Connection::OnClosedLogicalChannel ch=" << channel << " cp=" << channel.GetCapability() << " sid=" << channel.GetSessionID() << " d=" << (int)channel.GetDirection());
}

BOOL MyH323Connection::OpenAudioChannel(BOOL isEncoding, unsigned /* bufferSize */, H323AudioCodec & codec)
{
  //codec.SetSilenceDetectionMode(H323AudioCodec::NoSilenceDetection);
  
  PStringStream codecName;
  codecName << codec;
  
  PTRACE(2, "MyH323Connection::OpenAudioChannel " << codec);

  PWaitAndSignal mutex(connMutex);

  if (audioWrite == NULL) {
    audioWrite = new AudioWrite(*this);
  }

  if (audioRead == NULL) {
    audioRead = new AudioRead(*this);
  }

  if (isEncoding) {
    codec.AttachChannel(audioRead, FALSE);
  } else {
    codec.AttachChannel(audioWrite, FALSE);
  }

  return TRUE;
}

///////////////////////////////////////////////////////////////

AudioRead::AudioRead(MyH323Connection & _conn)
  : conn(_conn), closed(FALSE)
{
  triggered = FALSE;
}

BOOL AudioRead::Read(void * buffer, PINDEX amount)
{
  PWaitAndSignal mutex(Mutex);

  // if the channel is closed, then return error
  if (closed)
    return FALSE;

  if (!triggered && conn.ForceT38Mode() && conn.HadAnsweredCall()) {
    if( !conn.RequestModeChangeT38() ) {
      PTRACE(2, "AudioRead::Read RequestMode T38 - fail");
    } else {
      PTRACE(2, "AudioRead::Read RequestMode T38 - OK");
    }
    triggered = TRUE;
  }

  memset(buffer, 0, amount);

  delay.Delay(amount/16);

  return TRUE;
}

BOOL AudioRead::Close()
{
  PWaitAndSignal mutex(Mutex);
  closed = TRUE;
  return TRUE;
}

///////////////////////////////////////////////////////////////

AudioWrite::AudioWrite(MyH323Connection & _conn)
  : conn(_conn), closed(FALSE)
{
}

BOOL AudioWrite::Close()
{
  PWaitAndSignal mutex(Mutex);

  closed = TRUE;
  return TRUE;
}

BOOL AudioWrite::Write(const void * buf, PINDEX len)
{
  // Wait for the mutex, and Signal it at the end of this function
  PWaitAndSignal mutex(Mutex);

  if (closed)
    return FALSE;
    
  delay.Delay(len/16);
  return TRUE;
}

///////////////////////////////////////////////////////////////

AudioDelay::AudioDelay()
{
  firstTime = TRUE;
  error = 0;
}

void AudioDelay::Restart()
{
  firstTime = TRUE;
}

BOOL AudioDelay::Delay(int frameTime)
{
  if (firstTime) {
    firstTime = FALSE;
    previousTime = PTime();
    return TRUE;
  }

  error += frameTime;

  PTime now;
  PTimeInterval delay = now - previousTime;
  error -= (int)delay.GetMilliSeconds();
  previousTime = now;

  if (error > 0)
#ifdef P_LINUX
    usleep(error * 1000);
#else
    PThread::Sleep(error);
#endif

  return error <= -frameTime;
}

// End of File ///////////////////////////////////////////////////////////////

