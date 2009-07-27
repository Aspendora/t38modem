/*
 * t38engine.cxx
 *
 * T38FAX Pseudo Modem
 *
 * Copyright (c) 2001-2009 Vyacheslav Frolov
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
 * The Initial Developer of the Original Code is Vyacheslav Frolov
 *
 * Contributor(s): Equivalence Pty ltd
 *
 * $Log: t38engine.cxx,v $
 * Revision 1.54  2009-07-27 16:21:24  vfrolov
 * Moved h323lib specific code to h323lib directory
 *
 * Revision 1.54  2009/07/27 16:21:24  vfrolov
 * Moved h323lib specific code to h323lib directory
 *
 * Revision 1.53  2009/07/03 09:12:04  vfrolov
 * Included opal/buildopts.h
 *
 * Revision 1.52  2009/07/02 15:15:43  vfrolov
 * Fixed handling 5 sec buffer empty event for +FTM command
 *
 * Revision 1.51  2009/07/02 13:03:22  vfrolov
 * Fixed aborting +FTM immediately after CONNECT if no data in the buffer
 *
 * Revision 1.50  2009/02/05 14:15:18  vfrolov
 * Added missing cbpOutBufNoFull notification (for ECM)
 *
 * Revision 1.49  2009/01/27 14:00:50  vfrolov
 * Added missing startedTimeOutBufEmpty initialization
 *
 * Revision 1.48  2008/09/24 14:51:45  frolov
 * Added 5 sec. timeout  for DCE's transmit buffer empty
 *
 * Revision 1.47  2008/09/10 11:15:00  frolov
 * Ported to OPAL SVN trunk
 *
 * Revision 1.46  2007/05/24 17:03:54  vfrolov
 * Added more PTRACING checks
 *
 * Revision 1.45  2007/05/10 10:40:33  vfrolov
 * Added ability to continuously resend last UDPTL packet
 *
 * Revision 1.44  2007/05/03 09:21:47  vfrolov
 * Added compile time optimization for original ASN.1 sequence
 * in T.38 (06/98) Annex A or for CORRIGENDUM No. 1 fix
 *
 * Revision 1.43  2007/04/26 13:57:17  vfrolov
 * Changed logging for OPAL
 *
 * Revision 1.42  2007/04/11 14:19:15  vfrolov
 * Changed for OPAL
 *
 * Revision 1.41  2007/03/23 10:14:36  vfrolov
 * Implemented voice mode functionality
 *
 * Revision 1.40  2006/12/11 11:19:48  vfrolov
 * Fixed race condition with modem Callback
 *
 * Revision 1.39  2006/12/07 10:50:39  vfrolov
 * Fixed possible dead lock
 *
 * Revision 1.38  2006/10/20 10:04:00  vfrolov
 * Added code for ignoring repeated indicators
 * Added code for sending repeated indicators (disabled by default)
 *
 * Revision 1.37  2006/07/05 04:37:17  csoutheren
 * Applied 1488904 - SetPromiscuous(AcceptFromLastReceivedOnly) for T.38
 * Thanks to Vyacheslav Frolov
 *
 * Revision 1.36  2005/07/21 06:49:02  vfrolov
 * Added missing CompleteEncoding()
 *
 * Revision 1.35  2005/07/18 11:39:47  vfrolov
 * Changed for OPAL
 *
 * Revision 1.34  2005/03/03 16:09:06  vfrolov
 * Fixed memory leak
 *
 * Revision 1.33  2005/02/04 10:18:49  vfrolov
 * Fixed warnings for No Trace build
 *
 * Revision 1.32  2005/02/03 11:32:12  vfrolov
 * Fixed MSVC compile warnings
 *
 * Revision 1.31  2004/08/24 16:12:10  vfrolov
 * Fixed bit counter overflow
 *
 * Revision 1.30  2004/07/06 16:07:24  vfrolov
 * Included ptlib.h for precompiling
 *
 * Revision 1.29  2004/06/18 15:06:29  vfrolov
 * Fixed race condition by adding mutex for modemCallback
 *
 * Revision 1.28  2004/03/01 17:10:32  vfrolov
 * Fixed duplicated mutexes
 * Added volatile to T38Mode
 *
 * Revision 1.27  2003/12/04 16:09:58  vfrolov
 * Implemented FCS generation
 * Implemented ECM support
 *
 * Revision 1.26  2003/01/08 16:55:52  vfrolov
 * Added cbpOutBufNoFull and isOutBufFull()
 * Added data speed tracing
 * Fixed "thread did not terminate" (added msMaxOutDelay)
 * Discarded useless name tracing
 *
 * Revision 1.25  2002/12/30 12:49:46  vfrolov
 * Added tracing thread's CPU usage (Linux only)
 *
 * Revision 1.24  2002/12/20 10:13:08  vfrolov
 * Implemented tracing with PID of thread (for LinuxThreads)
 *   or ID of thread (for other POSIX Threads)
 *
 * Revision 1.23  2002/12/19 14:19:20  vfrolov
 * Added missing brackets (fixed all CPU usage reported by Markus Storm)
 *
 * Revision 1.22  2002/12/19 11:54:43  vfrolov
 * Removed DecodeIFPPacket() and utilized HandleRawIFP()
 *
 * Revision 1.21  2002/11/28 09:17:31  vfrolov
 * Added missing const
 *
 * Revision 1.20  2002/11/21 07:16:46  robertj
 * Changed promiscuous mode to be three way. Fixes race condition in gkserver
 *   which can cause crashes or more PDUs to be sent to the wrong place.
 *
 * Revision 1.19  2002/11/18 23:12:17  craigs
 * Removed reference to t38old.h
 *
 * Revision 1.18  2002/11/18 23:01:58  craigs
 * Changed name of pre CORRIGENDUM ASN
 *
 * Revision 1.17  2002/11/18 22:57:53  craigs
 * Added patches from Vyacheslav Frolov for CORRIGENDUM
 *
 * Revision 1.16  2002/11/15 07:43:52  vfrolov
 * Do not wait no-signal if received *-sig-end
 * Fixed compiler warnings
 *
 * Revision 1.15  2002/05/22 15:21:26  vfrolov
 * Added missed enableTimeout check
 * Fixed bad ifp tracing bug
 *
 * Revision 1.14  2002/05/22 12:01:45  vfrolov
 * Implemented redundancy error protection scheme
 *
 * Revision 1.13  2002/05/15 16:05:17  vfrolov
 * Changed algorithm of handling isCarrierIn
 * Removed delay after sending dtSilence
 *
 * Revision 1.12  2002/05/08 16:33:16  vfrolov
 * Adjusted post training delays
 *
 * Revision 1.11  2002/05/07 11:06:12  vfrolov
 * Discarded const from ModemCallbackWithUnlock()
 *
 * Revision 1.10  2002/05/07 10:15:38  vfrolov
 * Fixed dead lock on modemCallback
 *
 * Revision 1.9  2002/04/19 13:58:59  vfrolov
 * Added SendOnIdle()
 *
 * Revision 1.8  2002/03/01 09:02:04  vfrolov
 * Added Copyright header
 * Added name name to trace messages
 *
 * Revision 1.7  2002/02/11 16:46:18  vfrolov
 * Discarded transport arg from Originate() and Answer()
 * Thanks to Christopher Curtis
 *
 * Revision 1.6  2002/01/10 06:10:03  craigs
 * Added MPL header
 *
 * Revision 1.5  2002/01/06 03:48:45  craigs
 * Added changes to support efax 0.9
 * Thanks to Vyacheslav Frolov
 *
 * Revision 1.4  2002/01/03 21:36:42  craigs
 * Added additional logic to work with efax
 * Thanks to Vyacheslav Frolov
 *
 * Revision 1.3  2002/01/02 04:50:34  craigs
 * General formatting cleanups whilst looking for efax problem
 *
 * Revision 1.2  2002/01/01 23:59:52  craigs
 * Lots of additional implementation thanks to Vyacheslav Frolov
 *
 */

#include <ptlib.h>

#ifdef USE_OPAL
  #include <opal/buildopts.h>
  #include <asn/t38.h>
#else
  #include <t38.h>
#endif

#include "t38engine.h"

#define new PNEW

#define T38I(t30_indicator) T38_Type_of_msg_t30_indicator::t30_indicator
#define T38D(msg_data) T38_Type_of_msg_data::msg_data
#define T38F(field_type) T38_Data_Field_subtype_field_type::field_type
#define msMaxOutDelay 1000

#ifdef P_LINUX
  #define mySleep(ms) usleep((ms) * 1000L)
#else
  #define mySleep(ms) PThread::Sleep(ms)
#endif
///////////////////////////////////////////////////////////////
enum StateOut {
  stOutIdle,
  stOutCedWait,
  stOutSilenceWait,
  stOutIndWait,
  stOutData,
  stOutHdlcFcs,
  stOutDataNoSig,
  stOutNoSig,
};
///////////////////////////////////////////////////////////////
enum StateModem {
  stmIdle,
  stmError,

  stmOutMoreData,
  stmOutNoMoreData,

  stmInWaitData,
  stmInReadyData,
  stmInRecvData,
};

#define isStateModemOut() (stateModem >= stmOutMoreData && stateModem <= stmOutNoMoreData)
#define isStateModemIn() (stateModem >= stmInWaitData && stateModem <= stmInRecvData)
///////////////////////////////////////////////////////////////
class ModStream
{
  public:
    ModStream(const MODPARS &_ModPars);
    ~ModStream();

    void PushBuf();
    PBoolean DeleteFirstBuf();
    PBoolean PopBuf();
    int GetData(void *pBuf, PINDEX count);
    int PutData(const void *pBuf, PINDEX count);
    PBoolean SetDiag(int diag);
    PBoolean PutEof(int diag = 0);
    void Move(ModStream &from);

    DataStream *firstBuf;
    DataStreamQ bufQ;
    DataStream *lastBuf;	// if not NULL then shold be in bufQ or firstBuf
    MODPARS ModPars;

    HDLC hdlc;
};

ModStream::ModStream(const MODPARS &_ModPars) : firstBuf(NULL), lastBuf(NULL), ModPars(_ModPars)
{
}

ModStream::~ModStream()
{
  if (firstBuf != NULL) {
    PTRACE(1, "ModStream::~ModStream firstBuf != NULL, clean");
    delete firstBuf;
  }
  PTRACE_IF(1, bufQ.GetSize() > 0,
    "ModStream::~ModStream bufQ.GetSize()=" << bufQ.GetSize() << ", clean");
}

void ModStream::PushBuf()
{
  lastBuf = new DataStream();
  bufQ.Enqueue(lastBuf);
}

PBoolean ModStream::DeleteFirstBuf()
{
  if (firstBuf != NULL) {
    if (lastBuf == firstBuf)
      lastBuf = NULL;
    delete firstBuf;
    firstBuf = NULL;
    return TRUE;
  }
  return FALSE;
}

PBoolean ModStream::PopBuf()
{
  if (DeleteFirstBuf()) {
    PTRACE(1, "ModStream::PopBuf DeleteFirstBuf(), clean");
  }
  firstBuf = bufQ.Dequeue();
  if (!firstBuf)
    return FALSE;

  if (ModPars.dataType == T38Engine::dtRaw && ModPars.dataTypeT38 == T38Engine::dtHdlc) {
    hdlc = HDLC();
    hdlc.PutHdlcData(firstBuf);
    hdlc.GetRawStart(10);
  }
  return TRUE;
}

int ModStream::GetData(void *pBuf, PINDEX count)
{
  if (firstBuf == NULL) {
    myPTRACE(1, "ModStream::GetData firstBuf == NULL");
    return -1;
  }

  if (ModPars.dataType == T38Engine::dtRaw && ModPars.dataTypeT38 == T38Engine::dtHdlc) {
    int len;

    while ((len = hdlc.GetData(pBuf, count)) < 0) {
      DataStream *_firstBuf;
      if ((_firstBuf = bufQ.Dequeue()) != NULL) {
        DeleteFirstBuf();
        firstBuf = _firstBuf;
        hdlc.PutHdlcData(firstBuf);
        hdlc.GetRawStart();
      } else {
        if ((firstBuf->GetDiag() & T38Engine::diagNoCarrier) == 0) {
          DeleteFirstBuf();
          return 0;
        }
        return -1;
      }
    }
    return len;
  }

  if (ModPars.dataTypeT38 != ModPars.dataType) {
    myPTRACE(1, "ModStream::GetData ModPars.dataType("
      << ModPars.dataType
      << ") != ModPars.dataTypeT38("
      << ModPars.dataTypeT38
      << ")");
    return -1;
  }

  return firstBuf->GetData(pBuf, count);
}

int ModStream::PutData(const void *pBuf, PINDEX count)
{
  if( lastBuf == NULL ) {
    myPTRACE(1, "ModStream::PutData lastBuf == NULL");
    return -1;
  }
  return lastBuf->PutData(pBuf, count);
}

PBoolean ModStream::SetDiag(int diag)
{
  if( lastBuf == NULL )
    return FALSE;
  lastBuf->SetDiag(lastBuf->GetDiag() | diag);
  return TRUE;
}

PBoolean ModStream::PutEof(int diag)
{
  if( lastBuf == NULL )
    return FALSE;
  lastBuf->SetDiag(lastBuf->GetDiag() | diag).PutEof();
  lastBuf = NULL;
  return TRUE;
}

void ModStream::Move(ModStream &from)
{
  if( from.firstBuf != NULL ) {
    bufQ.Enqueue(from.firstBuf);
    from.firstBuf = NULL;
  }

  DataStream *buf;
  while( (buf = from.bufQ.Dequeue()) != NULL ) {
    bufQ.Enqueue(buf);
  }
  lastBuf = from.lastBuf;
  from.lastBuf = NULL;
}
///////////////////////////////////////////////////////////////
MODPARS::MODPARS(int _val, unsigned _ind, int _lenInd, unsigned _msgType, int _br)
      : dataType(T38Engine::dtNone), dataTypeT38(T38Engine::dtNone),
        val(_val), ind(_ind), lenInd(_lenInd),
        msgType(_msgType), br(_br)
{
}

static const MODPARS mods[] = {
MODPARS(   3, T38I(e_v21_preamble),              900, T38D(e_v21),         300 ),
MODPARS(  24, T38I(e_v27_2400_training),        1100, T38D(e_v27_2400),   2400 ),
MODPARS(  48, T38I(e_v27_4800_training),         900, T38D(e_v27_4800),   4800 ),
MODPARS(  72, T38I(e_v29_7200_training),         300, T38D(e_v29_7200),   7200 ),
MODPARS(  73, T38I(e_v17_7200_long_training),   1500, T38D(e_v17_7200),   7200 ),
MODPARS(  74, T38I(e_v17_7200_short_training),   300, T38D(e_v17_7200),   7200 ),
MODPARS(  96, T38I(e_v29_9600_training),         300, T38D(e_v29_9600),   9600 ),
MODPARS(  97, T38I(e_v17_9600_long_training),   1500, T38D(e_v17_9600),   9600 ),
MODPARS(  98, T38I(e_v17_9600_short_training),   300, T38D(e_v17_9600),   9600 ),
MODPARS( 121, T38I(e_v17_12000_long_training),  1500, T38D(e_v17_12000), 12000 ),
MODPARS( 122, T38I(e_v17_12000_short_training),  300, T38D(e_v17_12000), 12000 ),
MODPARS( 145, T38I(e_v17_14400_long_training),  1500, T38D(e_v17_14400), 14400 ),
MODPARS( 146, T38I(e_v17_14400_short_training),  300, T38D(e_v17_14400), 14400 ),
};

static const MODPARS invalidMods;

enum GetModParsBy {
  by_val,
  by_ind,
};

static const MODPARS &GetModPars(int key, enum GetModParsBy by = by_val) {
  for( PINDEX i = 0 ; i < PINDEX(sizeof(mods)/sizeof(mods[0])) ; i++ ) {
    switch( by ) {
      case by_val:
        if( mods[i].val == key )
          return mods[i];
        break;
      case by_ind:
        if( mods[i].ind == (unsigned)key )
          return mods[i];
        break;
      default:
        return invalidMods;
    }
  }
  return invalidMods;
}
///////////////////////////////////////////////////////////////
static void t38indicator(T38_IFP &ifp, unsigned type)
{
    ifp.m_type_of_msg.SetTag(T38_Type_of_msg::e_t30_indicator);
    (T38_Type_of_msg_t30_indicator &)ifp.m_type_of_msg = type;
}

#ifdef OPTIMIZE_CORRIGENDUM_IFP
  #define T38_DATA_FIELD T38_Data_Field_subtype
#else
  #define T38_DATA_FIELD T38_PreCorrigendum_Data_Field_subtype
#endif

static T38_DATA_FIELD &t38data(T38_IFP &ifp, unsigned type, unsigned field_type)
{
    ifp.m_type_of_msg.SetTag(T38_Type_of_msg::e_data);
    (T38_Type_of_msg_data &)ifp.m_type_of_msg = type;

    ifp.IncludeOptionalField(T38_IFPPacket::e_data_field);
    ifp.m_data_field.SetSize(ifp.m_data_field.GetSize()+1);
    T38_DATA_FIELD &Data_Field = ifp.m_data_field[0];
    Data_Field.m_field_type = field_type;
    return Data_Field;
}

static void t38data(T38_IFP &ifp, unsigned type, unsigned field_type, const PBYTEArray &data)
{
    T38_DATA_FIELD &Data_Field = t38data(ifp, type, field_type);

    if( data.GetSize() > 0 ) {
        Data_Field.IncludeOptionalField(T38_Data_Field_subtype::e_field_data);
        Data_Field.m_field_data = data;
    }
}
///////////////////////////////////////////////////////////////
T38Engine::T38Engine(const PString &_name)
  : EngineBase(_name),
    bufOut(2048),
    startedTimeOutBufEmpty(FALSE)
{
  PTRACE(2, name << " T38Engine::T38Engine");
  stateModem = stmIdle;
  stateOut = stOutNoSig;
  onIdleOut = dtNone;
  delayRestOut = 0;

  modStreamIn = NULL;
  modStreamInSaved = NULL;

  preparePacketTimeout = -1;

  T38Mode = TRUE;
  isCarrierIn = 0;
}

T38Engine::~T38Engine()
{
  PTRACE(1, name << " T38Engine::~T38Engine ");

  SignalOutDataReady();
  PWaitAndSignal mutexWaitIn(MutexIn);
  PWaitAndSignal mutexWaitOut(MutexOut);
  PWaitAndSignal mutexWaitModem(MutexModem);

  if( modStreamIn != NULL )
    delete modStreamIn;
  if( modStreamInSaved != NULL )
    delete modStreamInSaved;
  if( !modemCallback.IsNULL() )
    myPTRACE(1, name << " T38Engine::~T38Engine !modemCallback.IsNULL()");
}

PBoolean T38Engine::Attach(const PNotifier &callback)
{
  PTRACE(1, name << " T38Engine::Attach");
  PWaitAndSignal mutexWait(Mutex);

  if (!modemCallback.IsNULL()) {
    myPTRACE(1, name << " T38Engine::Attach !modemCallback.IsNULL()");
    return FALSE;
  }
  modemCallback = callback;
  _ResetModemState();
  return TRUE;
}

void T38Engine::Detach(const PNotifier &callback)
{
  PTRACE(1, name << " T38Engine::Detach");
  PWaitAndSignal mutexWait(Mutex);

  if (modemCallback == callback) {
    modemCallback = NULL;
    _ResetModemState();
    T38Mode = FALSE;
    myPTRACE(1, name << " T38Engine::Detach Detached");
    SignalOutDataReady();
  } else {
    myPTRACE(1, name << " T38Engine::Detach "
      << (modemCallback.IsNULL() ? "Already Detached" : "modemCallback != callback"));
  }
}
///////////////////////////////////////////////////////////////
//
void T38Engine::ModemCallbackWithUnlock(INT extra)
{
  Mutex.Signal();
  ModemCallback(extra);
  Mutex.Wait();
}

void T38Engine::Close()
{
  myPTRACE(1, name << " T38Engine::Close");

  PWaitAndSignal mutexWait(Mutex);
  T38Mode = FALSE;
  SignalOutDataReady();
  ModemCallbackWithUnlock(cbpReset);
}

void T38Engine::ResetModemState() {
  PWaitAndSignal mutexWaitModem(MutexModem);
  PWaitAndSignal mutexWait(Mutex);

  _ResetModemState();
}

void T38Engine::_ResetModemState() {
  if (modStreamIn && modStreamIn->DeleteFirstBuf()) {
    PTRACE(1, name << " T38Engine::ResetModemState modStreamIn->DeleteFirstBuf(), clean");
  }

  bufOut.PutEof();
  if (stateModem != stmIdle) {
    if (!isStateModemOut()) {
      myPTRACE(1, name << " T38Engine::ResetModemState stateModem(" << stateModem << ") != stmIdle, reset");
      stateModem = stmIdle;
    } else
      myPTRACE(1, name << " T38Engine::ResetModemState stateModem(" << stateModem << ") != stmIdle");
  }
  callbackParamIn = cbpReset;
  callbackParamOut = cbpReset;
}

PBoolean T38Engine::isOutBufFull() const
{
  PWaitAndSignal mutexWait(Mutex);
  return bufOut.isFull();
}
///////////////////////////////////////////////////////////////
void T38Engine::SendOnIdle(int _dataType)
{
  PTRACE(2, name << " T38Engine::SendOnIdle " << _dataType);

  PWaitAndSignal mutexWaitModem(MutexModem);
  PWaitAndSignal mutexWait(Mutex);

  onIdleOut = _dataType;
  SignalOutDataReady();
}

PBoolean T38Engine::SendStart(int _dataType, int param)
{
  PWaitAndSignal mutexWaitModem(MutexModem);
  if (!IsT38Mode())
    return FALSE;

  if (stateModem != stmIdle)  {
    myPTRACE(1, name << " T38Engine::SendStart stateModem(" << stateModem << ") != stmIdle");
    return FALSE;
  }

  PWaitAndSignal mutexWait(Mutex);

  if (modStreamIn != NULL) {
    delete modStreamIn;
    modStreamIn = NULL;
  }

  if (modStreamInSaved != NULL)  {
    delete modStreamInSaved;
    modStreamInSaved = NULL;
  }

  ModParsOut = invalidMods;

  switch( _dataType ) {
    case dtCed:
      ModParsOut.dataTypeT38 = ModParsOut.dataType = _dataType;
      ModParsOut.ind = T38I(e_ced);
      ModParsOut.lenInd = param;
      break;
    case dtSilence:
      ModParsOut.dataTypeT38 = ModParsOut.dataType = _dataType;
      ModParsOut.ind = T38I(e_no_signal);
      ModParsOut.lenInd = param;
      break;
    case dtHdlc:
    case dtRaw:
      ModParsOut = GetModPars(param);
      ModParsOut.dataType = _dataType;
      ModParsOut.dataTypeT38 =
          (ModParsOut.msgType == T38D(e_v21) || t30.hdlcOnly()) ? dtHdlc : dtRaw;
      if (!ModParsOut.IsModValid())
        return FALSE;
      break;
    default:
      return FALSE;
  }

  bufOut.Clean();		// reset eof
  stateModem = stmOutMoreData;
  SignalOutDataReady();
  return TRUE;
}

int T38Engine::Send(const void *pBuf, PINDEX count)
{
  PWaitAndSignal mutexWaitModem(MutexModem);
  if (!IsT38Mode())
    return -1;

  if (stateModem != stmOutMoreData) {
    myPTRACE(1, name << " T38Engine::Send stateModem(" << stateModem << ") != stmOutMoreData");
    return -1;
  }

  PWaitAndSignal mutexWait(Mutex);
  int res = bufOut.PutData(pBuf, count);
  if (res < 0) {
    myPTRACE(1, name << " T38Engine::Send res(" << res << ") < 0");
  }

  SignalOutDataReady();
  return res;
}

PBoolean T38Engine::SendStop(PBoolean moreFrames, int _callbackParam)
{
  PWaitAndSignal mutexWaitModem(MutexModem);
  if(!IsT38Mode())
    return FALSE;

  if (stateModem != stmOutMoreData ) {
    myPTRACE(1, name << " T38Engine::SendStop stateModem(" << stateModem << ") != stmOutMoreData");
    return FALSE;
  }

  PWaitAndSignal mutexWait(Mutex);
  bufOut.PutEof();
  stateModem = stmOutNoMoreData;
  moreFramesOut = moreFrames;
  callbackParamOut = _callbackParam;

  PTRACE(3, name << " T38Engine::SendStop moreFramesOut=" << moreFramesOut
                 << " callbackParamOut=" << callbackParamOut);

  SignalOutDataReady();
  return TRUE;
}
///////////////////////////////////////////////////////////////
PBoolean T38Engine::RecvWait(int _dataType, int param, int _callbackParam, PBoolean &done)
{
  PWaitAndSignal mutexWaitModem(MutexModem);
  if (!IsT38Mode())
    return FALSE;

  if( stateModem != stmIdle ) {
    myPTRACE(1, name << " T38Engine::RecvWait stateModem(" << stateModem << ") != stmIdle");
    return FALSE;
  }

  PWaitAndSignal mutexWait(Mutex);
  switch( _dataType ) {
    case dtHdlc:
    case dtRaw:
      break;
    default:
      return FALSE;
  }

  const MODPARS &ModParsIn = GetModPars(param);

  if (!ModParsIn.IsModValid())
    return FALSE;

  callbackParamIn = _callbackParam;

  if (modStreamIn != NULL) {
    if (modStreamIn->DeleteFirstBuf()) {
      PTRACE(1, name << " T38Engine::RecvWait modStreamIn->DeleteFirstBuf(), clean");
    }

    if (modStreamIn->bufQ.GetSize() > 0) {
      PTRACE(1, name << " T38Engine::RecvWait modStreamIn->bufQ.GetSize()=" << modStreamIn->bufQ.GetSize());
      if (ModParsIn.IsEqual(modStreamIn->ModPars)) {
        PTRACE(1, name << " T38Engine::RecvWait ModParsIn == modStreamIn->ModPars("
          << modStreamIn->ModPars.val
          << ")");
        modStreamIn->ModPars.dataType = _dataType;
        modStreamIn->ModPars.dataTypeT38 =
            (modStreamIn->ModPars.msgType == T38D(e_v21) || t30.hdlcOnly()) ? dtHdlc : dtRaw;
        stateModem = stmInReadyData;
        done = TRUE;
        return TRUE;
      }
    }
    delete modStreamIn;
  }

  modStreamIn = new ModStream(ModParsIn);
  modStreamIn->ModPars.dataType = _dataType;
  modStreamIn->ModPars.dataTypeT38 =
      (modStreamIn->ModPars.msgType == T38D(e_v21) || t30.hdlcOnly()) ? dtHdlc : dtRaw;

  if (modStreamInSaved != NULL) {
    if (modStreamIn->ModPars.IsEqual(modStreamInSaved->ModPars)) {
      myPTRACE(2, name << " T38Engine::RecvWait modStreamIn->ModPars == modStreamInSaved->ModPars("
        << modStreamInSaved->ModPars.val
        << ")");
      modStreamIn->Move(*modStreamInSaved);
      delete modStreamInSaved;
      modStreamInSaved = NULL;
    } else {
      myPTRACE(2, name << " T38Engine::RecvWait modStreamIn->ModPars("
        << modStreamIn->ModPars.val
        << ") != modStreamInSaved->ModPars("
        << modStreamInSaved->ModPars.val
        << ")");
      modStreamIn->PushBuf();
      modStreamIn->PutEof(diagDiffSig);
    }
    stateModem = stmInReadyData;
    done = TRUE;
    return TRUE;
  }

  stateModem = stmInWaitData;
  return TRUE;
}

PBoolean T38Engine::RecvStart(int _callbackParam)
{
  PWaitAndSignal mutexWaitModem(MutexModem);
  if (!IsT38Mode())
    return FALSE;

  if (stateModem != stmInReadyData ) {
    myPTRACE(1, name << " T38Engine::RecvStart stateModem(" << stateModem << ") != stmInReadyData");
    return FALSE;
  }
  PWaitAndSignal mutexWait(Mutex);
  callbackParamIn = _callbackParam;

  if (modStreamIn != NULL) {
    if (modStreamIn->PopBuf()) {
      if (modStreamIn->ModPars.msgType == T38D(e_v21))
        t30.v21Begin();
      stateModem = stmInRecvData;
      return TRUE;
    }
    myPTRACE(1, name << " T38Engine::RecvStart can't receive firstBuf");
    delete modStreamIn;
    modStreamIn = NULL;
  } else {
    myPTRACE(1, name << " T38Engine::RecvStart modStreamIn == NULL");
  }

  stateModem = stmIdle;
  return FALSE;
}

int T38Engine::Recv(void *pBuf, PINDEX count)
{
  PWaitAndSignal mutexWaitModem(MutexModem);
  if (!IsT38Mode())
    return -1;

  if( stateModem != stmInRecvData ) {
    myPTRACE(1, name << " T38Engine::Recv stateModem(" << stateModem << ") != stmInRecvData");
    return -1;
  }
  PWaitAndSignal mutexWait(Mutex);
  if( modStreamIn == NULL ) {
    myPTRACE(1, name << " T38Engine::Recv modStreamIn == NULL");
    return -1;
  }

  int len = modStreamIn->GetData(pBuf, count);

  if (modStreamIn->ModPars.msgType == T38D(e_v21)) {
    if (len > 0)
      t30.v21Data(pBuf, len);
    else
    if (len < 0)
      t30.v21End(FALSE);
  }

  return len;
}

int T38Engine::RecvDiag()
{
  PWaitAndSignal mutexWaitModem(MutexModem);
  PWaitAndSignal mutexWait(Mutex);
  if( modStreamIn == NULL ) {
    myPTRACE(1, name << " T38Engine::RecvDiag modStreamIn == NULL");
    return diagError;
  }
  if( modStreamIn->firstBuf == NULL ) {
    myPTRACE(1, name << " T38Engine::RecvDiag modStreamIn->firstBuf == NULL");
    return diagError;
  }
  return modStreamIn->firstBuf->GetDiag();
}

void T38Engine::RecvStop()
{
  PWaitAndSignal mutexWaitModem(MutexModem);
  if(!IsT38Mode())
    return;

  if(!isStateModemIn()) {
    myPTRACE(1, name << " T38Engine::RecvStop stateModem(" << stateModem << ") != stmIn");
    return;
  }
  PWaitAndSignal mutexWait(Mutex);

  if( modStreamIn )
    modStreamIn->DeleteFirstBuf();
  if( isStateModemIn() )
    stateModem = stmIdle;
}
///////////////////////////////////////////////////////////////
#ifdef USE_OPAL
  #define PTNAME name << " "
#else
  #define PTNAME
#endif
///////////////////////////////////////////////////////////////
int T38Engine::PreparePacket(T38_IFP & ifp)
{
  PWaitAndSignal mutexWaitOut(MutexOut);

  if(!IsT38Mode())
    return 0;

  //myPTRACE(1, PTNAME "T38Engine::PreparePacket begin stM=" << stateModem << " stO=" << stateOut);

  ifp = T38_IFP();
  PBoolean doDalay = TRUE;
  PBoolean wasCarrierIn = FALSE;

  for(;;) {
    PBoolean redo = FALSE;

    if (doDalay) {
      int outDelay;

      if (delayRestOut > 0)
        outDelay = delayRestOut;
      else switch( stateOut ) {
        case stOutIdle:			outDelay = msPerOut; break;

        case stOutCedWait:		outDelay = ModParsOut.lenInd; break;
        case stOutSilenceWait:		outDelay = ModParsOut.lenInd; break;
        case stOutIndWait:		outDelay = ModParsOut.lenInd; break;

        case stOutData:
        case stOutHdlcFcs:
          outDelay = int((PInt64(hdlcOut.getRawCount()) * 8 * 1000)/ModParsOut.br +
                     msPerOut - (PTime() - timeBeginOut).GetMilliSeconds());
          break;

        case stOutDataNoSig:		outDelay = msPerOut; break;

        case stOutNoSig:		outDelay = msPerOut; break;
        default:			outDelay = 0;
      }

      //myPTRACE(1, PTNAME "T38Engine::PreparePacket outDelay=" << outDelay);

      if (outDelay > 0) {
        if (preparePacketTimeout > 0 && outDelay > preparePacketTimeout) {
          delayRestOut = outDelay - preparePacketTimeout;
          mySleep(preparePacketTimeout);
          return -1;
        } else {
          while(outDelay > msMaxOutDelay) {
            mySleep(msMaxOutDelay);
            if (!IsT38Mode())
              return 0;
            outDelay -= msMaxOutDelay;
          }
          mySleep(outDelay);
        }
      }
    } else
      doDalay = TRUE;

    delayRestOut = 0;

    if (!IsT38Mode())
      return 0;

    for(;;) {
      PBoolean waitData = FALSE;
      {
        PWaitAndSignal mutexWait(Mutex);
        if( isStateModemOut() || stateOut != stOutIdle ) {
          switch( stateOut ) {
            case stOutIdle:
              if (isCarrierIn) {
                myPTRACE(1, PTNAME "T38Engine::PreparePacket isCarrierIn for dataType=" << ModParsOut.dataType);
                int waitms = 0;
                /*
                 * We can't to begin sending data while the carrier is detected because
                 * it's possible that all data (including indication) will be losted.
                 * It's too critical for image data because it's possible to receive
                 * MCF generated for previous page after sending small page that was
                 * not delivered.
                 */
                switch( ModParsOut.dataType ) {
                  case dtHdlc:		waitms = 500; break;	// it's can't be too long
                  case dtRaw:		waitms = 2000; break;	// it's can't be too short
                }

                if (waitms) {
                  if (!wasCarrierIn) {
                    wasCarrierIn = TRUE;
                    timeBeginOut = PTime() + PTimeInterval(1000);
                    redo = TRUE;
                    break;
                  } else if (timeBeginOut > PTime()) {
                    redo = TRUE;
                    break;
                  } else {
                    myPTRACE(1, PTNAME "T38Engine::PreparePacket isCarrierIn expired");
                  }
                }
              }

              wasCarrierIn = FALSE;

              switch (ModParsOut.dataTypeT38) {
                case dtHdlc:
                case dtRaw:
                  t38indicator(ifp, ModParsOut.ind);
                  stateOut = stOutIndWait;
                  break;
                case dtCed:
                  t38indicator(ifp, ModParsOut.ind);
                  stateOut = stOutCedWait;
                  break;
                case dtSilence:
                  stateOut = stOutSilenceWait;
                  redo = TRUE;
                  break;
                default:
                  myPTRACE(1, PTNAME "T38Engine::PreparePacket bad dataTypeT38=" << ModParsOut.dataTypeT38);
                  return 0;
              }
              break;
            ////////////////////////////////////////////////////
            case stOutCedWait:
              stateOut = stOutNoSig;
              stateModem = stmIdle;
              ModemCallbackWithUnlock(callbackParamOut);
              redo = TRUE;
              break;
            ////////////////////////////////////////////////////
            case stOutSilenceWait:
              stateOut = stOutIdle;
              stateModem = stmIdle;
              ModemCallbackWithUnlock(callbackParamOut);
              doDalay = FALSE;
              redo = TRUE;
              break;
            ////////////////////////////////////////////////////
            case stOutIndWait:
              stateOut = stOutData;
              countOut = 0;
              startedTimeOutBufEmpty = FALSE;
              timeBeginOut = PTime();
              hdlcOut = HDLC();
              if (ModParsOut.msgType == T38D(e_v21))
                t30.v21Begin();

              switch (ModParsOut.dataType) {
                case dtHdlc:
                  hdlcOut.PutHdlcData(&bufOut);
                  break;
                case dtRaw:
                  hdlcOut.PutRawData(&bufOut);
                  break;
                default:
                  myPTRACE(1, PTNAME "T38Engine::PreparePacket bad dataType=" << ModParsOut.dataType);
                  return 0;
              }

              switch (ModParsOut.dataTypeT38) {
                case dtHdlc:
                  hdlcOut.GetHdlcStart(TRUE);
                  break;
                case dtRaw:
                  if (ModParsOut.dataType == dtHdlc) {
                    myPTRACE(1, PTNAME "T38Engine::PreparePacket sending dtHdlc like dtRaw not implemented");
                    return 0;
                  }
                  hdlcOut.GetRawStart();
                  break;
                default:
                  myPTRACE(1, PTNAME "T38Engine::PreparePacket bad dataTypeT38=" << ModParsOut.dataTypeT38);
                  return 0;
              }

              redo = TRUE;
              break;
            ////////////////////////////////////////////////////
            case stOutData:
              {
                BYTE b[(msPerOut * 14400)/(8*1000)];
                PINDEX len = (msPerOut * ModParsOut.br)/(8*1000);
                if (len > PINDEX(sizeof(b)))
                  len = sizeof(b);
                PBoolean wasFull = bufOut.isFull();
                int count = hdlcOut.GetData(b, len);
                if (wasFull && !bufOut.isFull())
                  ModemCallbackWithUnlock(cbpOutBufNoFull);

                switch( count ) {
                  case -1:
                    startedTimeOutBufEmpty = FALSE;

                    switch (ModParsOut.dataTypeT38) {
                      case dtHdlc:
                        stateOut = stOutHdlcFcs;
                        break;
                      case dtRaw:
                        stateOut = stOutDataNoSig;
                        break;
                      default:
                        myPTRACE(1, PTNAME "T38Engine::PreparePacket stOutData bad dataTypeT38="
                            << ModParsOut.dataTypeT38);
                        return 0;
                    }
                    redo = TRUE;
                    break;
                  case 0:
                    if (hdlcOut.getLastChar() != -1 &&
                        (ModParsOut.dataType == dtHdlc || hdlcOut.getLastChar() != 0))
                    {
                      ModemCallbackWithUnlock(cbpOutBufEmpty);
                    }
                    else
                    if (!startedTimeOutBufEmpty) {
                      timeOutBufEmpty = PTime() + PTimeInterval(5000);
                      startedTimeOutBufEmpty = TRUE;
                    }
                    else
                    if (timeOutBufEmpty <= PTime()) {
                      ModemCallbackWithUnlock(cbpOutBufEmpty);
                    }
                    waitData = TRUE;
                    break;
                  default:
                    startedTimeOutBufEmpty = FALSE;

                    switch (ModParsOut.dataTypeT38) {
                      case dtHdlc:
                        if (ModParsOut.msgType == T38D(e_v21)) {
                          t30.v21Data(b, count);
                        }
                        t38data(ifp, ModParsOut.msgType, T38F(e_hdlc_data), PBYTEArray(b, count));
                        break;
                      case dtRaw:
                        t38data(ifp, ModParsOut.msgType, T38F(e_t4_non_ecm_data), PBYTEArray(b, count));
                        break;
                      default:
                        myPTRACE(1, PTNAME "T38Engine::PreparePacket stOutData bad dataTypeT38="
                            << ModParsOut.dataTypeT38);
                        return 0;
                    }
                    countOut += count;
                }
              }
              break;
            ////////////////////////////////////////////////////
            case stOutHdlcFcs:
              if (ModParsOut.msgType == T38D(e_v21)) {
                t30.v21End(TRUE);
                t30.v21Begin();
              }
              if (ModParsOut.dataType == dtRaw) {
                PBoolean wasFull = bufOut.isFull();

                if (countOut)
                  t38data(ifp, ModParsOut.msgType, hdlcOut.isFcsOK() ? T38F(e_hdlc_fcs_OK) : T38F(e_hdlc_fcs_BAD));

                hdlcOut.GetHdlcStart(FALSE);
                countOut = 0;

                if (hdlcOut.GetData(NULL, 0) != -1)
                  stateOut = stOutData;
                else
                  stateOut = stOutDataNoSig;

                if (wasFull && !bufOut.isFull())
                  ModemCallbackWithUnlock(cbpOutBufNoFull);
              } else {
                if( stateModem != stmOutNoMoreData ) {
                  myPTRACE(1, PTNAME "T38Engine::PreparePacket stOutHdlcFcs stateModem("
                      << stateModem << ") != stmOutNoMoreData");
                  return 0;
                }
                if (countOut)
                  t38data(ifp, ModParsOut.msgType, T38F(e_hdlc_fcs_OK));
                countOut = 0;
                bufOut.Clean();		// reset eof
                hdlcOut.PutHdlcData(&bufOut);
                hdlcOut.GetHdlcStart(FALSE);
                if (moreFramesOut) {
                  stateOut = stOutData;
                  stateModem = stmOutMoreData;
                  ModemCallbackWithUnlock(callbackParamOut);
                } else {
                  stateOut = stOutDataNoSig;
                }
              }
              break;
            ////////////////////////////////////////////////////
            case stOutDataNoSig:
#if PTRACING
              if (myCanTrace(3) || (myCanTrace(2) && ModParsOut.dataType == dtRaw)) {
                PInt64 msTime = (PTime() - timeBeginOut).GetMilliSeconds();
                myPTRACE(2, PTNAME "Sent " << hdlcOut.getRawCount() << " bytes in " << msTime << " ms ("
                  << (PInt64(hdlcOut.getRawCount()) * 8 * 1000)/(msTime ? msTime : 1) << " bits/s)");
              }
#endif
              if( stateModem != stmOutNoMoreData ) {
                myPTRACE(1, PTNAME "T38Engine::PreparePacket stOutDataNoSig stateModem("
                     << stateModem << ") != stmOutNoMoreData");
                return 0;
              }
              switch (ModParsOut.dataTypeT38) {
                case dtHdlc:
                  t38data(ifp, ModParsOut.msgType, T38F(e_hdlc_sig_end));
                  break;
                case dtRaw:
                  t38data(ifp, ModParsOut.msgType, T38F(e_t4_non_ecm_sig_end));
                  break;
                default:
                  myPTRACE(1, PTNAME "T38Engine::PreparePacket stOutDataNoSig bad dataTypeT38="
                      << ModParsOut.dataTypeT38);
                  return 0;
              }
              stateOut = stOutNoSig;
              stateModem = stmIdle;
              ModemCallbackWithUnlock(callbackParamOut);
              break;
            ////////////////////////////////////////////////////
            case stOutNoSig:
              t38indicator(ifp, T38I(e_no_signal));
              stateOut = stOutIdle;
              break;
            default:
              myPTRACE(1, PTNAME "T38Engine::PreparePacket bad stateOut=" << stateOut);
              return 0;
          }
        } else {
          switch( onIdleOut ) {
            case dtCng:
              t38indicator(ifp, T38I(e_cng));
              break;
            case dtSilence:
              t38indicator(ifp, T38I(e_no_signal));
              break;
            default:
              waitData = TRUE;
          }
          onIdleOut = dtNone;
        }
      }
      if (!waitData)
        break;
      if (preparePacketTimeout > 0) {
        if (!WaitOutDataReady(preparePacketTimeout))
          return -1;
      } else {
        if (startedTimeOutBufEmpty) {
          PInt64 timeout = (timeOutBufEmpty - PTime()).GetMilliSeconds() + 1;

          if (timeout > 0)
            WaitOutDataReady(timeout);
        } else {
          WaitOutDataReady();
        }
      }

      if (!IsT38Mode())
        return 0;

      {
        PWaitAndSignal mutexWait(Mutex);
        if (stateOut == stOutData) {
#if PTRACING
          if (myCanTrace(3) || (myCanTrace(2) && ModParsOut.dataType == dtRaw)) {
            PInt64 msTime = (PTime() - timeBeginOut).GetMilliSeconds();
            myPTRACE(2, PTNAME "Sent " << hdlcOut.getRawCount() << " bytes in " << msTime << " ms ("
              << (PInt64(hdlcOut.getRawCount()) * 8 * 1000)/(msTime ? msTime : 1) << " bits/s)");
          }
#endif
          myPTRACE(1, PTNAME "T38Engine::PreparePacket DTE's data delay, reset " << hdlcOut.getRawCount());
          hdlcOut.resetRawCount();
          timeBeginOut = PTime() - PTimeInterval(msPerOut);
          doDalay = FALSE;
        }
      }
    }
    if( !redo ) break;
  }
  return 1;
}
///////////////////////////////////////////////////////////////
PBoolean T38Engine::HandlePacketLost(unsigned myPTRACE_PARAM(nLost))
{
  PWaitAndSignal mutexWaitIn(MutexIn);

  if (!IsT38Mode())
    return FALSE;

  myPTRACE(1, PTNAME "T38Engine::HandlePacketLost " << nLost);
  PWaitAndSignal mutexWait(Mutex);

  ModStream *modStream = modStreamIn;

  if( modStream == NULL || modStream->lastBuf == NULL ) {
    modStream = modStreamInSaved;
  }
  if( !(modStream == NULL || modStream->lastBuf == NULL) ) {
    if( modStream->ModPars.msgType == T38D(e_v21) ) {
      modStream->SetDiag(diagBadFcs);
    }
  }
  return TRUE;
}
///////////////////////////////////////////////////////////////
PBoolean T38Engine::HandlePacket(const T38_IFP & ifp)
{
#if PTRACING
  if (PTrace::CanTrace(3)) {
    PTRACE(3, PTNAME "T38Engine::HandlePacket Received ifp\n  "
             << setprecision(2) << ifp);
  }
  else {
    PTRACE(2, PTNAME "T38Engine::HandlePacket Received ifp type=" << ifp.m_type_of_msg.GetTagName());
  }
#endif

  PWaitAndSignal mutexWaitIn(MutexIn);
  if (!IsT38Mode())
    return FALSE;

  PWaitAndSignal mutexWait(Mutex);

  switch( ifp.m_type_of_msg.GetTag() ) {
    case T38_Type_of_msg::e_t30_indicator:
      if ((modStreamIn != NULL) && (modStreamIn->lastBuf != NULL &&
            modStreamIn->ModPars.ind == (T38_Type_of_msg_t30_indicator)ifp.m_type_of_msg) ||
          (modStreamInSaved != NULL) && (modStreamInSaved->lastBuf != NULL &&
            modStreamInSaved->ModPars.ind == (T38_Type_of_msg_t30_indicator)ifp.m_type_of_msg))
      {
        myPTRACE(3, PTNAME "T38Engine::HandlePacket ignored repeated indicator " << ifp.m_type_of_msg);
        break;
      }

      if( modStreamIn != NULL && modStreamIn->PutEof(diagOutOfOrder) )
        myPTRACE(1, PTNAME "T38Engine::HandlePacket indicator && modStreamIn->lastBuf != NULL");

      if( modStreamInSaved != NULL ) {
        myPTRACE(1, PTNAME "T38Engine::HandlePacket indicator && modStreamInSaved != NULL, clean");
        delete modStreamInSaved;
        modStreamInSaved = NULL;
      }

      switch( (T38_Type_of_msg_t30_indicator)ifp.m_type_of_msg ) {
        case T38I(e_no_signal):
        case T38I(e_cng):
        case T38I(e_ced):
          isCarrierIn = 0;
          break;
        case T38I(e_v21_preamble):
        case T38I(e_v27_2400_training):
        case T38I(e_v27_4800_training):
        case T38I(e_v29_7200_training):
        case T38I(e_v29_9600_training):
        case T38I(e_v17_7200_short_training):
        case T38I(e_v17_7200_long_training):
        case T38I(e_v17_9600_short_training):
        case T38I(e_v17_9600_long_training):
        case T38I(e_v17_12000_short_training):
        case T38I(e_v17_12000_long_training):
        case T38I(e_v17_14400_short_training):
        case T38I(e_v17_14400_long_training):
          isCarrierIn = 1;
          modStreamInSaved = new ModStream(GetModPars((T38_Type_of_msg_t30_indicator)ifp.m_type_of_msg, by_ind));
          modStreamInSaved->PushBuf();
#if PTRACING
          countIn = 0;
#endif

          if( stateModem == stmInWaitData ) {
            if( modStreamIn != NULL ) {
              if (modStreamIn->ModPars.IsEqual(modStreamInSaved->ModPars)) {
                modStreamIn->Move(*modStreamInSaved);
                delete modStreamInSaved;
                modStreamInSaved = NULL;
              } else {
                myPTRACE(2, PTNAME " T38Engine::HandlePacket modStreamIn->ModPars("
                  << modStreamIn->ModPars.val
                  << ") != modStreamInSaved->ModPars("
                  << modStreamInSaved->ModPars.val
                  << ")");
                modStreamIn->PushBuf();
                modStreamIn->PutEof(diagDiffSig);
              }
            } else {
              myPTRACE(1, PTNAME "T38Engine::HandlePacket modStreamIn == NULL");
            }
            stateModem = stmInReadyData;
            ModemCallbackWithUnlock(callbackParamIn);
          }
          break;
        default:
          myPTRACE(1, PTNAME "T38Engine::HandlePacket type_of_msg is bad !!! " << setprecision(2) << ifp);
      }
      break;
    case T38_Type_of_msg::e_data:
      {
        unsigned type_of_msg = (T38_Type_of_msg_data)ifp.m_type_of_msg;
        ModStream *modStream = modStreamIn;

        if( modStream == NULL || modStream->lastBuf == NULL ) {
          modStream = modStreamInSaved;
        }
        if( modStream == NULL || modStream->lastBuf == NULL ) {
          PTRACE(1, PTNAME "T38Engine::HandlePacket lastBuf == NULL");
          break;
        }
        if( modStream->ModPars.msgType != type_of_msg ) {
          myPTRACE(1, PTNAME "T38Engine::HandlePacket modStream->ModPars.msgType("
              << modStream->ModPars.msgType << ") != type_of_msg(" << type_of_msg << ")");
          modStream->PutEof(diagOutOfOrder);
          if( stateModem == stmInRecvData )
            ModemCallbackWithUnlock(callbackParamIn);
          break;
        }

        switch( type_of_msg ) {
          case T38D(e_v21):
          case T38D(e_v27_2400):
          case T38D(e_v27_4800):
          case T38D(e_v29_7200):
          case T38D(e_v29_9600):
          case T38D(e_v17_7200):
          case T38D(e_v17_9600):
          case T38D(e_v17_12000):
          case T38D(e_v17_14400):
            if( ifp.HasOptionalField(T38_IFPPacket::e_data_field) ) {
              PINDEX count = ifp.m_data_field.GetSize();
              for( PINDEX i = 0 ; i < count ; i++ ) {
                if( modStream == NULL ) {
                  PTRACE(1, PTNAME "T38Engine::HandlePacket modStream == NULL");
                  break;
                }
                const T38_DATA_FIELD &Data_Field = ifp.m_data_field[i];

                switch( Data_Field.m_field_type ) {	// Handle data
                  case T38F(e_hdlc_data):
                  case T38F(e_t4_non_ecm_data):
                  case T38F(e_hdlc_sig_end):
                  case T38F(e_hdlc_fcs_OK):
                  case T38F(e_hdlc_fcs_BAD):
                  case T38F(e_hdlc_fcs_OK_sig_end):
                  case T38F(e_hdlc_fcs_BAD_sig_end):
                  case T38F(e_t4_non_ecm_sig_end):
                    if (Data_Field.HasOptionalField(T38_Data_Field_subtype::e_field_data)) {
                      int size = Data_Field.m_field_data.GetSize();
                      modStream->PutData(Data_Field.m_field_data, size);
#if PTRACING
                      if (!countIn)
                        timeBeginIn = PTime();
                      countIn += size;
#endif
                    }
                    break;
                  default:
                    myPTRACE(1, PTNAME "T38Engine::HandlePacket field_type bad !!! " << setprecision(2) << ifp);
                }
                switch( Data_Field.m_field_type ) {	// Handle fcs
                  case T38F(e_hdlc_fcs_BAD):
                  case T38F(e_hdlc_fcs_BAD_sig_end):
                    modStream->SetDiag(diagBadFcs);
                    myPTRACE(1, PTNAME "T38Engine::HandlePacket bad FCS");
                  case T38F(e_hdlc_fcs_OK):
                  case T38F(e_hdlc_fcs_OK_sig_end):
                    modStream->PutEof();
                    modStream->PushBuf();
                    break;
                }
                switch( Data_Field.m_field_type ) {	// Handle sig_end
                  case T38F(e_t4_non_ecm_sig_end):
#if PTRACING
                    if (myCanTrace(2)) {
                      PInt64 msTime = (PTime() - timeBeginIn).GetMilliSeconds();
                      myPTRACE(2, PTNAME "Received " << countIn << " bytes in " << msTime << " ms ("
                        << (PInt64(countIn) * 8 * 1000)/(msTime ? msTime : 1) << " bits/s)");
                    }
#endif
                  case T38F(e_hdlc_fcs_OK_sig_end):
                  case T38F(e_hdlc_fcs_BAD_sig_end):
                  case T38F(e_hdlc_sig_end):
                    modStream->PutEof(diagNoCarrier);
                    modStream = NULL;
                    isCarrierIn = 0;
                    break;
                }
              }
            }
            break;
          default:
            myPTRACE(1, PTNAME "T38Engine::HandlePacket type_of_msg is bad !!! " << setprecision(2) << ifp);
        }
        if( stateModem == stmInRecvData )
          ModemCallbackWithUnlock(callbackParamIn);
        break;
      }
    default:
      myPTRACE(1, PTNAME "T38Engine::HandlePacket Tag is bad !!! " << setprecision(2) << ifp);
  }
  return TRUE;
}
///////////////////////////////////////////////////////////////

