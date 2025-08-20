//------------------------------------------------------------------------------
// File: streams.h
//
// Desc: DirectShow base classes - defines overall streams architecture.
//
// Copyright (c) 1992-2001 Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------

#ifndef __STREAMS__
#define __STREAMS__

#define _HAS_EXCEPTIONS 0
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#if defined(_MSC_VER) && _MSC_VER>=1100
#define AM_NOVTABLE __declspec(novtable)
#else
#define AM_NOVTABLE
#endif

#if defined(_DEBUG) && !defined(DEBUG)
#define DEBUG
#endif

#include <mmsystem.h>
#include <strmif.h>     // Generated IDL header file for streams interfaces
#include <intsafe.h>    // required by amvideo.h
#include <amvideo.h>    // ActiveMovie video interfaces and definitions
#include <errors.h>     // HRESULT status and error definitions
#include <uuids.h>      // declaration of type GUIDs and well-known clsids
#include <evcode.h>     // event code definitions

//// -- UNUSED FOR THIS PROJECT --
//include amaudio.h explicitly if you need it.  it requires the DX SDK.
//#include <amaudio.h>    // ActiveMovie audio interfaces and definitions
//#include <comlite.h>    // Light weight com function prototypes
//#include <control.h>    // generated from control.odl
//#include <edevdefs.h>   // External device control interface defines
//#include <audevcod.h>   // audio filter device error event codes
//#include "dllsetup.h"   // Filter registration support functions
//#include "cache.h"      // Simple cache container class
//#include "ctlutil.h"    // control interface utility classes
//#include "transfrm.h"   // Generic transform filter
//#include "transip.h"    // Generic transform-in-place filter
//#include "outputq.h"    // Output pin queueing
//#include "renbase.h"    // Base class for writing ActiveX renderers
//#include "winctrl.h"    // Implements the IVideoWindow interface
//#include "sysclock.h"   // System clock
//#include "pstream.h"    // IPersistStream helper class
//#include "vtrans.h"     // Video Transform Filter base class
//#include "amextra.h"
//#include "strmctl.h"    // IAMStreamControl support

//// -- EMBEDDED BELOW --
//#include "reftime.h"    // Helper class for REFERENCE_TIME management
//#include "wxdebug.h"    // Debug support for logging and ASSERTs
//#include "wxutil.h"     // General helper classes for threads etc
//#include "combase.h"    // Base COM classes to support IUnknown
//#include "wxlist.h"     // Non MFC generic list class
//#include "mtype.h"      // Helper class for managing media types
//#include "amfilter.h"   // Main streams architecture class hierachy
//#include "source.h"     // Generic source filter
//#include "cprop.h"      // Base property page class

//// -- EMBEDDED IN streams.cpp --
//#include "fourcc.h"     // conversions between FOURCCs and GUIDs
//#include "msgthrd.h"    // CMsgThread
//#include "refclock.h"   // Base clock class
//#include "videoctl.h"   // Specifically video related classes
//#include "measure.h"    // Performance measurement
//#include "winutil.h"    // Helps with filters that manage windows

#endif // __STREAMS__

//------------------------------------------------------------------------------
// File: RefTime.h
//
// Desc: DirectShow base classes - defines CRefTime, a class that manages
//       reference times.
//
// Copyright (c) 1992-2001 Microsoft Corporation. All rights reserved.
//------------------------------------------------------------------------------


//
// CRefTime
//
// Manage reference times.
// Shares same data layout as REFERENCE_TIME, but adds some (nonvirtual)
// functions providing simple comparison, conversion and arithmetic.
//
// A reference time (at the moment) is a unit of seconds represented in
// 100ns units as is used in the Win32 FILETIME structure. BUT the time
// a REFERENCE_TIME represents is NOT the time elapsed since 1/1/1601 it
// will either be stream time or reference time depending upon context
//
// This class provides simple arithmetic operations on reference times
//
// keep non-virtual otherwise the data layout will not be the same as
// REFERENCE_TIME


// -----
// note that you are safe to cast a CRefTime* to a REFERENCE_TIME*, but
// you will need to do so explicitly
// -----


#ifndef __REFTIME__
#define __REFTIME__


const LONGLONG MILLISECONDS = (1000);            // 10 ^ 3
const LONGLONG NANOSECONDS = (1000000000);       // 10 ^ 9
const LONGLONG UNITS = (NANOSECONDS / 100);      // 10 ^ 7

/*  Unfortunately an inline function here generates a call to __allmul
    - even for constants!
*/
#define MILLISECONDS_TO_100NS_UNITS(lMs) \
    Int32x32To64((lMs), (UNITS / MILLISECONDS))

class CRefTime
{
public:

    // *MUST* be the only data member so that this class is exactly
    // equivalent to a REFERENCE_TIME.
    // Also, must be *no virtual functions*

    REFERENCE_TIME m_time;

    inline CRefTime()
    {
        // default to 0 time
        m_time = 0;
    };

    inline CRefTime(LONG msecs)
    {
        m_time = MILLISECONDS_TO_100NS_UNITS(msecs);
    };

    inline CRefTime(REFERENCE_TIME rt)
    {
        m_time = rt;
    };

    inline operator REFERENCE_TIME() const
    {
        return m_time;
    };

    inline CRefTime& operator=(const CRefTime& rt)
    {
        m_time = rt.m_time;
        return *this;
    };

    inline CRefTime& operator=(const LONGLONG ll)
    {
        m_time = ll;
        return *this;
    };

    inline CRefTime& operator+=(const CRefTime& rt)
    {
        return (*this = *this + rt);
    };

    inline CRefTime& operator-=(const CRefTime& rt)
    {
        return (*this = *this - rt);
    };

    inline LONG Millisecs(void)
    {
        return (LONG)(m_time / (UNITS / MILLISECONDS));
    };

    inline LONGLONG GetUnits(void)
    {
        return m_time;
    };
};

const LONGLONG TimeZero = 0;

#endif /* __REFTIME__ */

//------------------------------------------------------------------------------
// File: WXDebug.h
//
// Desc: DirectShow base classes - provides debugging facilities.
//
// Copyright (c) 1992-2001 Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------


#ifndef __WXDEBUG__
#define __WXDEBUG__

// This library provides fairly straight forward debugging functionality, this
// is split into two main sections. The first is assertion handling, there are
// three types of assertions provided here. The most commonly used one is the
// ASSERT(condition) macro which will pop up a message box including the file
// and line number if the condition evaluates to FALSE. Then there is the
// EXECUTE_ASSERT macro which is the same as ASSERT except the condition will
// still be executed in NON debug builds. The final type of assertion is the
// KASSERT macro which is more suitable for pure (perhaps kernel) filters as
// the condition is printed onto the debugger rather than in a message box.
//
// The other part of the debug module facilties is general purpose logging.
// This is accessed by calling DbgLog(). The function takes a type and level
// field which define the type of informational string you are presenting and
// it's relative importance. The type field can be a combination (one or more)
// of LOG_TIMING, LOG_TRACE, LOG_MEMORY, LOG_LOCKING and LOG_ERROR. The level
// is a DWORD value where zero defines highest important. Use of zero as the
// debug logging level is to be encouraged ONLY for major errors or events as
// they will ALWAYS be displayed on the debugger. Other debug output has it's
// level matched against the current debug output level stored in the registry
// for this module and if less than the current setting it will be displayed.
//
// Each module or executable has it's own debug output level for each of the
// five types. These are read in when the DbgInitialise function is called
// for DLLs linking to STRMBASE.LIB this is done automatically when the DLL
// is loaded, executables must call it explicitely with the module instance
// handle given to them through the WINMAIN entry point. An executable must
// also call DbgTerminate when they have finished to clean up the resources
// the debug library uses, once again this is done automatically for DLLs

// These are the five different categories of logging information

enum {  LOG_TIMING = 0x01,    // Timing and performance measurements
        LOG_TRACE = 0x02,     // General step point call tracing
        LOG_MEMORY =  0x04,   // Memory and object allocation/destruction
        LOG_LOCKING = 0x08,   // Locking/unlocking of critical sections
        LOG_ERROR = 0x10,     // Debug error notification
        LOG_CUSTOM1 = 0x20,
        LOG_CUSTOM2 = 0x40,
        LOG_CUSTOM3 = 0x80,
        LOG_CUSTOM4 = 0x100,
        LOG_CUSTOM5 = 0x200,
};

#define LOG_FORCIBLY_SET 0x80000000

enum {  CDISP_HEX = 0x01,
        CDISP_DEC = 0x02};

// For each object created derived from CBaseObject (in debug builds) we
// create a descriptor that holds it's name (statically allocated memory)
// and a cookie we assign it. We keep a list of all the active objects
// we have registered so that we can dump a list of remaining objects

typedef struct tag_ObjectDesc {
    LPCSTR m_szName;
    LPCWSTR m_wszName;
    DWORD m_dwCookie;
    tag_ObjectDesc *m_pNext;
} ObjectDesc;

#define DLLIMPORT __declspec(dllimport)
#define DLLEXPORT __declspec(dllexport)

#ifdef DEBUG

    #define NAME(x) TEXT(x)

    // These are used internally by the debug library (PRIVATE)

    void WINAPI DbgInitKeyLevels(HKEY hKey, bool fTakeMax);
    void WINAPI DbgInitGlobalSettings(bool fTakeMax);
    void WINAPI DbgInitModuleSettings(bool fTakeMax);
    void WINAPI DbgInitModuleName();
    DWORD WINAPI DbgRegisterObjectCreation(
        LPCSTR szObjectName, LPCWSTR wszObjectName);

    BOOL WINAPI DbgRegisterObjectDestruction(DWORD dwCookie);

    // These are the PUBLIC entry points

    BOOL WINAPI DbgCheckModuleLevel(DWORD Type,DWORD Level);
    void WINAPI DbgSetModuleLevel(DWORD Type,DWORD Level);
    void WINAPI DbgSetAutoRefreshLevels(bool fAuto);

    // Initialise the library with the module handle

    void WINAPI DbgInitialise(HINSTANCE hInst);
    void WINAPI DbgTerminate();

    void WINAPI DbgDumpObjectRegister();

    // Display error and logging to the user

    void WINAPI DbgAssert(LPCTSTR pCondition,LPCTSTR pFileName,INT iLine);
    void WINAPI DbgBreakPoint(LPCTSTR pCondition,LPCTSTR pFileName,INT iLine);
    void WINAPI DbgBreakPoint(LPCTSTR pFileName,INT iLine,__format_string LPCTSTR  szFormatString,...);

    void WINAPI DbgKernelAssert(LPCTSTR pCondition,LPCTSTR pFileName,INT iLine);
    void WINAPI DbgLogInfo(DWORD Type,DWORD Level,__format_string LPCTSTR pFormat,...);
#ifdef UNICODE
    void WINAPI DbgLogInfo(DWORD Type,DWORD Level,__format_string LPCSTR pFormat,...);
    void WINAPI DbgAssert(LPCSTR pCondition,LPCSTR pFileName,INT iLine);
    void WINAPI DbgBreakPoint(LPCSTR pCondition,LPCSTR pFileName,INT iLine);
    void WINAPI DbgKernelAssert(LPCSTR pCondition,LPCSTR pFileName,INT iLine);
#endif
    void WINAPI DbgOutString(LPCTSTR psz);

    //  Debug infinite wait stuff
    DWORD WINAPI DbgWaitForSingleObject(HANDLE h);
    DWORD WINAPI DbgWaitForMultipleObjects(DWORD nCount,
                                    __in_ecount(nCount) CONST HANDLE *lpHandles,
                                    BOOL bWaitAll);
    void WINAPI DbgSetWaitTimeout(DWORD dwTimeout);

#ifdef __strmif_h__
    // Display a media type: Terse at level 2, verbose at level 5
    void WINAPI DisplayType(LPCTSTR label, const AM_MEDIA_TYPE *pmtIn);

    // Dump lots of information about a filter graph
    void WINAPI DumpGraph(IFilterGraph *pGraph, DWORD dwLevel);
#endif

    #define KASSERT(_x_) if (!(_x_))         \
        DbgKernelAssert(TEXT(#_x_),TEXT(__FILE__),__LINE__)

    //  Break on the debugger without putting up a message box
    //  message goes to debugger instead

    #define KDbgBreak(_x_)                   \
        DbgKernelAssert(TEXT(#_x_),TEXT(__FILE__),__LINE__)

    // We chose a common name for our ASSERT macro, MFC also uses this name
    // So long as the implementation evaluates the condition and handles it
    // then we will be ok. Rather than override the behaviour expected we
    // will leave whatever first defines ASSERT as the handler (i.e. MFC)
    #ifndef ASSERT
        #define ASSERT(_x_) if (!(_x_))         \
            DbgAssert(TEXT(#_x_),TEXT(__FILE__),__LINE__)
    #endif

    #define DbgAssertAligned( _ptr_, _alignment_ ) ASSERT( ((DWORD_PTR) (_ptr_)) % (_alignment_) == 0)

    //  Put up a message box informing the user of a halt
    //  condition in the program

    #define DbgBreak(_x_)                   \
        DbgBreakPoint(TEXT(#_x_),TEXT(__FILE__),__LINE__)

    #define EXECUTE_ASSERT(_x_) ASSERT(_x_)
    #define DbgLog(_x_) DbgLogInfo _x_
    // MFC style trace macros

    #define NOTE(_x_)             DbgLog((LOG_TRACE,5,TEXT(_x_)))
    #define NOTE1(_x_,a)          DbgLog((LOG_TRACE,5,TEXT(_x_),a))
    #define NOTE2(_x_,a,b)        DbgLog((LOG_TRACE,5,TEXT(_x_),a,b))
    #define NOTE3(_x_,a,b,c)      DbgLog((LOG_TRACE,5,TEXT(_x_),a,b,c))
    #define NOTE4(_x_,a,b,c,d)    DbgLog((LOG_TRACE,5,TEXT(_x_),a,b,c,d))
    #define NOTE5(_x_,a,b,c,d,e)  DbgLog((LOG_TRACE,5,TEXT(_x_),a,b,c,d,e))

#else

    // Retail builds make public debug functions inert  - WARNING the source
    // files do not define or build any of the entry points in debug builds
    // (public entry points compile to nothing) so if you go trying to call
    // any of the private entry points in your source they won't compile

    #define NAME(_x_) ((LPTSTR) NULL)

    #define DbgInitialise(hInst)
    #define DbgTerminate()
    #define DbgLog(_x_) 0
    #define DbgOutString(psz)
    #define DbgAssertAligned( _ptr_, _alignment_ ) 0

    #define DbgRegisterObjectCreation(pObjectName)
    #define DbgRegisterObjectDestruction(dwCookie)
    #define DbgDumpObjectRegister()

    #define DbgCheckModuleLevel(Type,Level)
    #define DbgSetModuleLevel(Type,Level)
    #define DbgSetAutoRefreshLevels(fAuto)

    #define DbgWaitForSingleObject(h)  WaitForSingleObject(h, INFINITE)
    #define DbgWaitForMultipleObjects(nCount, lpHandles, bWaitAll)     \
               WaitForMultipleObjects(nCount, lpHandles, bWaitAll, INFINITE)
    #define DbgSetWaitTimeout(dwTimeout)

    #define KDbgBreak(_x_)
    #define DbgBreak(_x_)

    #define KASSERT(_x_) ((void)0)
    #ifndef ASSERT
	#define ASSERT(_x_) ((void)0)
    #endif
    #define EXECUTE_ASSERT(_x_) ((void)(_x_))

    // MFC style trace macros

    #define NOTE(_x_) ((void)0)
    #define NOTE1(_x_,a) ((void)0)
    #define NOTE2(_x_,a,b) ((void)0)
    #define NOTE3(_x_,a,b,c) ((void)0)
    #define NOTE4(_x_,a,b,c,d) ((void)0)
    #define NOTE5(_x_,a,b,c,d,e) ((void)0)

    #define DisplayType(label, pmtIn) ((void)0)
    #define DumpGraph(pGraph, label) ((void)0)
#endif


// Checks a pointer which should be non NULL - can be used as follows.

#define CheckPointer(p,ret) {if((p)==NULL) return (ret);}

//   HRESULT Foo(VOID *pBar)
//   {
//       CheckPointer(pBar,E_INVALIDARG)
//   }
//
//   Or if the function returns a boolean
//
//   BOOL Foo(VOID *pBar)
//   {
//       CheckPointer(pBar,FALSE)
//   }

#define ValidateReadPtr(p,cb) 0
#define ValidateWritePtr(p,cb) 0
#define ValidateReadWritePtr(p,cb) 0
#define ValidateStringPtr(p) 0
#define ValidateStringPtrA(p) 0
#define ValidateStringPtrW(p) 0


#ifdef _OBJBASE_H_

    //  Outputting GUID names.  If you want to include the name
    //  associated with a GUID (eg CLSID_...) then
    //
    //      GuidNames[yourGUID]
    //
    //  Returns the name defined in uuids.h as a string

    typedef struct {
        CHAR   *szName;
        GUID    guid;
    } GUID_STRING_ENTRY;

    class CGuidNameList {
    public:
        CHAR *operator [] (const GUID& guid);
    };

    extern CGuidNameList GuidNames;

#endif

#ifndef REMIND
    //  REMIND macro - generates warning as reminder to complete coding
    //  (eg) usage:
    //
    //  #pragma message (REMIND("Add automation support"))


    #define QUOTE(x) #x
    #define QQUOTE(y) QUOTE(y)
    #define REMIND(str) __FILE__ "(" QQUOTE(__LINE__) ") :  " str
#endif

//  Method to display objects in a useful format
//
//  eg If you want to display a LONGLONG ll in a debug string do (eg)
//
//  DbgLog((LOG_TRACE, n, TEXT("Value is %s"), (LPCTSTR)CDisp(ll, CDISP_HEX)));


class CDispBasic
{
public:
    CDispBasic() { m_pString = m_String; };
    ~CDispBasic();
protected:
    PTCHAR m_pString;  // normally points to m_String... unless too much data
    TCHAR m_String[50];
};
class CDisp : public CDispBasic
{
public:
    CDisp(LONGLONG ll, int Format = CDISP_HEX); // Display a LONGLONG in CDISP_HEX or CDISP_DEC form
    CDisp(REFCLSID clsid);      // Display a GUID
    CDisp(double d);            // Display a floating point number
#ifdef __strmif_h__
#ifdef __STREAMS__
    CDisp(CRefTime t);          // Display a Reference Time
#endif
    CDisp(IPin *pPin);          // Display a pin as {filter clsid}(pin name)
    CDisp(IUnknown *pUnk);      // Display a filter or pin
#endif // __strmif_h__
    ~CDisp();

    //  Implement cast to (LPCTSTR) as parameter to logger
    operator LPCTSTR()
    {
        return (LPCTSTR)m_pString;
    };
};


#if defined(DEBUG)
class CAutoTrace
{
private:
    LPCTSTR  _szBlkName;
    const int _level;
    static const TCHAR _szEntering[];
    static const TCHAR _szLeaving[];
public:
    CAutoTrace(LPCTSTR szBlkName, const int level = 15)
        : _szBlkName(szBlkName), _level(level)
    {DbgLog((LOG_TRACE, _level, _szEntering, _szBlkName));}

    ~CAutoTrace()
    {DbgLog((LOG_TRACE, _level, _szLeaving, _szBlkName));}
};

#if defined (__FUNCTION__)

#define AMTRACEFN()  CAutoTrace __trace(TEXT(__FUNCTION__))
#define AMTRACE(_x_) CAutoTrace __trace(TEXT(__FUNCTION__))

#else

#define AMTRACE(_x_) CAutoTrace __trace _x_
#define AMTRACEFN()

#endif

#else

#define AMTRACE(_x_)
#define AMTRACEFN()

#endif

#endif // __WXDEBUG__

//------------------------------------------------------------------------------
// File: WXUtil.h
//
// Desc: DirectShow base classes - defines helper classes and functions for
//       building multimedia filters.
//
// Copyright (c) 1992-2001 Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------


#ifndef __WXUTIL__
#define __WXUTIL__

// eliminate spurious "statement has no effect" warnings.
#pragma warning(disable: 4705)

// wrapper for whatever critical section we have
class CCritSec {

    // make copy constructor and assignment operator inaccessible

    CCritSec(const CCritSec &refCritSec);
    CCritSec &operator=(const CCritSec &refCritSec);

    CRITICAL_SECTION m_CritSec;

#ifdef DEBUG
public:
    DWORD   m_currentOwner;
    DWORD   m_lockCount;
    BOOL    m_fTrace;        // Trace this one
public:
    CCritSec();
    ~CCritSec();
    void Lock();
    void Unlock();
#else

public:
    CCritSec() {
        InitializeCriticalSection(&m_CritSec);
    };

    ~CCritSec() {
        DeleteCriticalSection(&m_CritSec);
    };

    void Lock() {
        EnterCriticalSection(&m_CritSec);
    };

    void Unlock() {
        LeaveCriticalSection(&m_CritSec);
    };
#endif
};

//
// To make deadlocks easier to track it is useful to insert in the
// code an assertion that says whether we own a critical section or
// not.  We make the routines that do the checking globals to avoid
// having different numbers of member functions in the debug and
// retail class implementations of CCritSec.  In addition we provide
// a routine that allows usage of specific critical sections to be
// traced.  This is NOT on by default - there are far too many.
//

#ifdef DEBUG
    BOOL WINAPI CritCheckIn(CCritSec * pcCrit);
    BOOL WINAPI CritCheckIn(const CCritSec * pcCrit);
    BOOL WINAPI CritCheckOut(CCritSec * pcCrit);
    BOOL WINAPI CritCheckOut(const CCritSec * pcCrit);
    void WINAPI DbgLockTrace(CCritSec * pcCrit, BOOL fTrace);
#else
    #define CritCheckIn(x) TRUE
    #define CritCheckOut(x) TRUE
    #define DbgLockTrace(pc, fT)
#endif


// locks a critical section, and unlocks it automatically
// when the lock goes out of scope
class CAutoLock {

    // make copy constructor and assignment operator inaccessible

    CAutoLock(const CAutoLock &refAutoLock);
    CAutoLock &operator=(const CAutoLock &refAutoLock);

protected:
    CCritSec * m_pLock;

public:
    CAutoLock(CCritSec * plock)
    {
        m_pLock = plock;
        m_pLock->Lock();
    };

    ~CAutoLock() {
        m_pLock->Unlock();
    };
};



// wrapper for event objects
class CAMEvent
{

    // make copy constructor and assignment operator inaccessible

    CAMEvent(const CAMEvent &refEvent);
    CAMEvent &operator=(const CAMEvent &refEvent);

protected:
    HANDLE m_hEvent;
public:
    CAMEvent(BOOL fManualReset = FALSE, __inout_opt HRESULT *phr = NULL);
    CAMEvent(__inout_opt HRESULT *phr);
    ~CAMEvent();

    // Cast to HANDLE - we don't support this as an lvalue
    operator HANDLE () const { return m_hEvent; };

    void Set() {EXECUTE_ASSERT(SetEvent(m_hEvent));};
    BOOL Wait(DWORD dwTimeout = INFINITE) {
	return (WaitForSingleObject(m_hEvent, dwTimeout) == WAIT_OBJECT_0);
    };
    void Reset() { ResetEvent(m_hEvent); };
    BOOL Check() { return Wait(0); };
};


// wrapper for event objects that do message processing
// This adds ONE method to the CAMEvent object to allow sent
// messages to be processed while waiting

class CAMMsgEvent : public CAMEvent
{

public:

    CAMMsgEvent(__inout_opt HRESULT *phr = NULL);

    // Allow SEND messages to be processed while waiting
    BOOL WaitMsg(DWORD dwTimeout = INFINITE);
};

// old name supported for the time being
#define CTimeoutEvent CAMEvent

// support for a worker thread

#ifdef AM_NOVTABLE
// simple thread class supports creation of worker thread, synchronization
// and communication. Can be derived to simplify parameter passing
class AM_NOVTABLE CAMThread {

    // make copy constructor and assignment operator inaccessible

    CAMThread(const CAMThread &refThread);
    CAMThread &operator=(const CAMThread &refThread);

    CAMEvent m_EventSend;
    CAMEvent m_EventComplete;

    DWORD m_dwParam;
    DWORD m_dwReturnVal;

protected:
    HANDLE m_hThread;

    // thread will run this function on startup
    // must be supplied by derived class
    virtual DWORD ThreadProc() = 0;

public:
    CAMThread(__inout_opt HRESULT *phr = NULL);
    virtual ~CAMThread();

    CCritSec m_AccessLock;	// locks access by client threads
    CCritSec m_WorkerLock;	// locks access to shared objects

    // thread initially runs this. param is actually 'this'. function
    // just gets this and calls ThreadProc
    static DWORD WINAPI InitialThreadProc(__inout LPVOID pv);

    // start thread running  - error if already running
    BOOL Create();

    // signal the thread, and block for a response
    //
    DWORD CallWorker(DWORD);

    // accessor thread calls this when done with thread (having told thread
    // to exit)
    void Close() {

        // Disable warning: Conversion from LONG to PVOID of greater size
#pragma warning(push)
#pragma warning(disable: 4312)
        HANDLE hThread = (HANDLE)InterlockedExchangePointer(&m_hThread, 0);
#pragma warning(pop)

        if (hThread) {
            WaitForSingleObject(hThread, INFINITE);
            CloseHandle(hThread);
        }
    };

    // ThreadExists
    // Return TRUE if the thread exists. FALSE otherwise
    BOOL ThreadExists(void) const
    {
        if (m_hThread == 0) {
            return FALSE;
        } else {
            return TRUE;
        }
    }

    // wait for the next request
    DWORD GetRequest();

    // is there a request?
    BOOL CheckRequest(__out_opt DWORD * pParam);

    // reply to the request
    void Reply(DWORD);

    // If you want to do WaitForMultipleObjects you'll need to include
    // this handle in your wait list or you won't be responsive
    HANDLE GetRequestHandle() const { return m_EventSend; };

    // Find out what the request was
    DWORD GetRequestParam() const { return m_dwParam; };

    // call CoInitializeEx (COINIT_DISABLE_OLE1DDE) if
    // available. S_FALSE means it's not available.
    static HRESULT CoInitializeHelper();
};
#endif // AM_NOVTABLE


// CQueue
//
// Implements a simple Queue ADT.  The queue contains a finite number of
// objects, access to which is controlled by a semaphore.  The semaphore
// is created with an initial count (N).  Each time an object is added
// a call to WaitForSingleObject is made on the semaphore's handle.  When
// this function returns a slot has been reserved in the queue for the new
// object.  If no slots are available the function blocks until one becomes
// available.  Each time an object is removed from the queue ReleaseSemaphore
// is called on the semaphore's handle, thus freeing a slot in the queue.
// If no objects are present in the queue the function blocks until an
// object has been added.

#define DEFAULT_QUEUESIZE   2

template <class T> class CQueue {
private:
    HANDLE          hSemPut;        // Semaphore controlling queue "putting"
    HANDLE          hSemGet;        // Semaphore controlling queue "getting"
    CRITICAL_SECTION CritSect;      // Thread seriallization
    int             nMax;           // Max objects allowed in queue
    int             iNextPut;       // Array index of next "PutMsg"
    int             iNextGet;       // Array index of next "GetMsg"
    T              *QueueObjects;   // Array of objects (ptr's to void)

    void Initialize(int n) {
        iNextPut = iNextGet = 0;
        nMax = n;
        InitializeCriticalSection(&CritSect);
        hSemPut = CreateSemaphore(NULL, n, n, NULL);
        hSemGet = CreateSemaphore(NULL, 0, n, NULL);
        QueueObjects = new T[n];
    }


public:
    CQueue(int n) {
        Initialize(n);
    }

    CQueue() {
        Initialize(DEFAULT_QUEUESIZE);
    }

    ~CQueue() {
        delete [] QueueObjects;
        DeleteCriticalSection(&CritSect);
        CloseHandle(hSemPut);
        CloseHandle(hSemGet);
    }

    T GetQueueObject() {
        int iSlot;
        T Object;
        LONG lPrevious;

        // Wait for someone to put something on our queue, returns straight
        // away is there is already an object on the queue.
        //
        WaitForSingleObject(hSemGet, INFINITE);

        EnterCriticalSection(&CritSect);
        iSlot = iNextGet++ % nMax;
        Object = QueueObjects[iSlot];
        LeaveCriticalSection(&CritSect);

        // Release anyone waiting to put an object onto our queue as there
        // is now space available in the queue.
        //
        ReleaseSemaphore(hSemPut, 1L, &lPrevious);
        return Object;
    }

    void PutQueueObject(T Object) {
        int iSlot;
        LONG lPrevious;

        // Wait for someone to get something from our queue, returns straight
        // away is there is already an empty slot on the queue.
        //
        WaitForSingleObject(hSemPut, INFINITE);

        EnterCriticalSection(&CritSect);
        iSlot = iNextPut++ % nMax;
        QueueObjects[iSlot] = Object;
        LeaveCriticalSection(&CritSect);

        // Release anyone waiting to remove an object from our queue as there
        // is now an object available to be removed.
        //
        ReleaseSemaphore(hSemGet, 1L, &lPrevious);
    }
};

// Ensures that memory is not read past the length source buffer
// and that memory is not written past the length of the dst buffer
//   dst - buffer to copy to
//   dst_size - total size of destination buffer
//   cb_dst_offset - offset, first byte copied to dst+cb_dst_offset
//   src - buffer to copy from
//   src_size - total size of source buffer
//   cb_src_offset - offset, first byte copied from src+cb_src_offset
//   count - number of bytes to copy
//
// Returns:
//    S_OK          - no error
//    E_INVALIDARG  - values passed would lead to overrun
HRESULT AMSafeMemMoveOffset(
    __in_bcount(dst_size) void * dst,
    __in size_t dst_size,
    __in DWORD cb_dst_offset,
    __in_bcount(src_size) const void * src,
    __in size_t src_size,
    __in DWORD cb_src_offset,
    __in size_t count);

extern "C"
void * __stdcall memmoveInternal(void *, const void *, size_t);

inline void * __cdecl memchrInternal(const void *buf, int chr, size_t cnt)
{
#ifdef _X86_
    void *pRet = NULL;

    _asm {
        cld                 // make sure we get the direction right
        mov     ecx, cnt    // num of bytes to scan
        mov     edi, buf    // pointer byte stream
        mov     eax, chr    // byte to scan for
        repne   scasb       // look for the byte in the byte stream
        jnz     exit_memchr // Z flag set if byte found
        dec     edi         // scasb always increments edi even when it
                            // finds the required byte
        mov     pRet, edi
exit_memchr:
    }
    return pRet;

#else
    while ( cnt && (*(unsigned char *)buf != (unsigned char)chr) ) {
        buf = (unsigned char *)buf + 1;
        cnt--;
    }

    return(cnt ? (void *)buf : NULL);
#endif
}

void WINAPI IntToWstr(int i, __out_ecount(12) LPWSTR wstr);

#define WstrToInt(sz) _wtoi(sz)
#define atoiW(sz) _wtoi(sz)
#define atoiA(sz) atoi(sz)

// These are available to help managing bitmap VIDEOINFOHEADER media structures

extern const DWORD bits555[3];
extern const DWORD bits565[3];
extern const DWORD bits888[3];

// These help convert between VIDEOINFOHEADER and BITMAPINFO structures

STDAPI_(const GUID) GetTrueColorType(const BITMAPINFOHEADER *pbmiHeader);
STDAPI_(const GUID) GetBitmapSubtype(const BITMAPINFOHEADER *pbmiHeader);
STDAPI_(WORD) GetBitCount(const GUID *pSubtype);

// strmbase.lib implements this for compatibility with people who
// managed to link to this directly.  we don't want to advertise it.
//
// STDAPI_(/* T */ CHAR *) GetSubtypeName(const GUID *pSubtype);

STDAPI_(CHAR *) GetSubtypeNameA(const GUID *pSubtype);
STDAPI_(WCHAR *) GetSubtypeNameW(const GUID *pSubtype);

#ifdef UNICODE
#define GetSubtypeName GetSubtypeNameW
#else
#define GetSubtypeName GetSubtypeNameA
#endif

STDAPI_(LONG) GetBitmapFormatSize(const BITMAPINFOHEADER *pHeader);
STDAPI_(DWORD) GetBitmapSize(const BITMAPINFOHEADER *pHeader);

#ifdef __AMVIDEO__
STDAPI_(BOOL) ContainsPalette(const VIDEOINFOHEADER *pVideoInfo);
STDAPI_(const RGBQUAD *) GetBitmapPalette(const VIDEOINFOHEADER *pVideoInfo);
#endif // __AMVIDEO__


// Compares two interfaces and returns TRUE if they are on the same object
BOOL WINAPI IsEqualObject(IUnknown *pFirst, IUnknown *pSecond);

// This is for comparing pins
#define EqualPins(pPin1, pPin2) IsEqualObject(pPin1, pPin2)


// Arithmetic helper functions

// Compute (a * b + rnd) / c
LONGLONG WINAPI llMulDiv(LONGLONG a, LONGLONG b, LONGLONG c, LONGLONG rnd);
LONGLONG WINAPI Int64x32Div32(LONGLONG a, LONG b, LONG c, LONG rnd);


// Avoids us dyna-linking to SysAllocString to copy BSTR strings
STDAPI WriteBSTR(__deref_out BSTR * pstrDest, LPCWSTR szSrc);
STDAPI FreeBSTR(__deref_in BSTR* pstr);

// Return a wide string - allocating memory for it
// Returns:
//    S_OK          - no error
//    E_POINTER     - ppszReturn == NULL
//    E_OUTOFMEMORY - can't allocate memory for returned string
STDAPI AMGetWideString(LPCWSTR pszString, __deref_out LPWSTR *ppszReturn);

// Special wait for objects owning windows
DWORD WINAPI WaitDispatchingMessages(
    HANDLE hObject,
    DWORD dwWait,
    HWND hwnd = NULL,
    UINT uMsg = 0,
    HANDLE hEvent = NULL);

// HRESULT_FROM_WIN32 converts ERROR_SUCCESS to a success code, but in
// our use of HRESULT_FROM_WIN32, it typically means a function failed
// to call SetLastError(), and we still want a failure code.
//
#define AmHresultFromWin32(x) (MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, x))

// call GetLastError and return an HRESULT value that will fail the
// SUCCEEDED() macro.
HRESULT AmGetLastErrorToHResult(void);

// duplicate of ATL's CComPtr to avoid linker conflicts.

IUnknown* QzAtlComPtrAssign(__deref_inout_opt IUnknown** pp, __in_opt IUnknown* lp);

template <class T>
class QzCComPtr
{
public:
	typedef T _PtrClass;
	QzCComPtr() {p=NULL;}
	QzCComPtr(T* lp)
	{
		if ((p = lp) != NULL)
			p->AddRef();
	}
	QzCComPtr(const QzCComPtr<T>& lp)
	{
		if ((p = lp.p) != NULL)
			p->AddRef();
	}
	~QzCComPtr() {if (p) p->Release();}
	void Release() {if (p) p->Release(); p=NULL;}
	operator T*() {return (T*)p;}
	T& operator*() {ASSERT(p!=NULL); return *p; }
	//The assert on operator& usually indicates a bug.  If this is really
	//what is needed, however, take the address of the p member explicitly.
	T** operator&() { ASSERT(p==NULL); return &p; }
	T* operator->() { ASSERT(p!=NULL); return p; }
	T* operator=(T* lp){return (T*)QzAtlComPtrAssign((IUnknown**)&p, lp);}
	T* operator=(const QzCComPtr<T>& lp)
	{
		return (T*)QzAtlComPtrAssign((IUnknown**)&p, lp.p);
	}
#if _MSC_VER>1020
	bool operator!(){return (p == NULL);}
#else
	BOOL operator!(){return (p == NULL) ? TRUE : FALSE;}
#endif
	T* p;
};

MMRESULT CompatibleTimeSetEvent( UINT uDelay, UINT uResolution, __in LPTIMECALLBACK lpTimeProc, DWORD_PTR dwUser, UINT fuEvent );
bool TimeKillSynchronousFlagAvailable( void );

//  Helper to replace lstrcpmi
__inline int lstrcmpiLocaleIndependentW(LPCWSTR lpsz1, LPCWSTR lpsz2)
{
    return  CompareStringW(LOCALE_INVARIANT, NORM_IGNORECASE, lpsz1, -1, lpsz2, -1) - CSTR_EQUAL;
}
__inline int lstrcmpiLocaleIndependentA(LPCSTR lpsz1, LPCSTR lpsz2)
{
    return  CompareStringA(LOCALE_INVARIANT, NORM_IGNORECASE, lpsz1, -1, lpsz2, -1) - CSTR_EQUAL;
}

#endif /* __WXUTIL__ */

//------------------------------------------------------------------------------
// File: ComBase.h
//
// Desc: DirectShow base classes - defines a class hierarchy for creating
//       COM objects.
//
// Copyright (c) 1992-2001 Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------


/*

a. Derive your COM object from CUnknown

b. Make a static CreateInstance function that takes an LPUNKNOWN, an HRESULT *
   and a TCHAR *. The LPUNKNOWN defines the object to delegate IUnknown calls
   to. The HRESULT * allows error codes to be passed around constructors and
   the TCHAR * is a descriptive name that can be printed on the debugger.

   It is important that constructors only change the HRESULT * if they have
   to set an ERROR code, if it was successful then leave it alone or you may
   overwrite an error code from an object previously created.

   When you call a constructor the descriptive name should be in static store
   as we do not copy the string. To stop large amounts of memory being used
   in retail builds by all these static strings use the NAME macro,

   CMyFilter = new CImplFilter(NAME("My filter"),pUnknown,phr);
   if (FAILED(hr)) {
       return hr;
   }

   In retail builds NAME(_x_) compiles to NULL, the base CBaseObject class
   knows not to do anything with objects that don't have a name.

c. Have a constructor for your object that passes the LPUNKNOWN, HRESULT * and
   TCHAR * to the CUnknown constructor. You can set the HRESULT if you have an
   error, or just simply pass it through to the constructor.

   The object creation will fail in the class factory if the HRESULT indicates
   an error (ie FAILED(HRESULT) == TRUE)

d. Create a FactoryTemplate with your object's class id and CreateInstance
   function.

Then (for each interface) either

Multiple inheritance

1. Also derive it from ISomeInterface
2. Include DECLARE_IUNKNOWN in your class definition to declare
   implementations of QueryInterface, AddRef and Release that
   call the outer unknown
3. Override NonDelegatingQueryInterface to expose ISomeInterface by
   code something like

     if (riid == IID_ISomeInterface) {
         return GetInterface((ISomeInterface *) this, ppv);
     } else {
         return CUnknown::NonDelegatingQueryInterface(riid, ppv);
     }

4. Declare and implement the member functions of ISomeInterface.

or: Nested interfaces

1. Declare a class derived from CUnknown
2. Include DECLARE_IUNKNOWN in your class definition
3. Override NonDelegatingQueryInterface to expose ISomeInterface by
   code something like

     if (riid == IID_ISomeInterface) {
         return GetInterface((ISomeInterface *) this, ppv);
     } else {
         return CUnknown::NonDelegatingQueryInterface(riid, ppv);
     }

4. Implement the member functions of ISomeInterface. Use GetOwner() to
   access the COM object class.

And in your COM object class:

5. Make the nested class a friend of the COM object class, and declare
   an instance of the nested class as a member of the COM object class.

   NOTE that because you must always pass the outer unknown and an hResult
   to the CUnknown constructor you cannot use a default constructor, in
   other words you will have to make the member variable a pointer to the
   class and make a NEW call in your constructor to actually create it.

6. override the NonDelegatingQueryInterface with code like this:

     if (riid == IID_ISomeInterface) {
         return m_pImplFilter->
            NonDelegatingQueryInterface(IID_ISomeInterface, ppv);
     } else {
         return CUnknown::NonDelegatingQueryInterface(riid, ppv);
     }

You can have mixed classes which support some interfaces via multiple
inheritance and some via nested classes

*/

#ifndef __COMBASE__
#define __COMBASE__

// Filter Setup data structures no defined in axextend.idl

typedef REGPINTYPES
AMOVIESETUP_MEDIATYPE, * PAMOVIESETUP_MEDIATYPE, * FAR LPAMOVIESETUP_MEDIATYPE;

typedef REGFILTERPINS
AMOVIESETUP_PIN, * PAMOVIESETUP_PIN, * FAR LPAMOVIESETUP_PIN;

typedef struct _AMOVIESETUP_FILTER
{
  const CLSID * clsID;
  const WCHAR * strName;
  DWORD      dwMerit;
  UINT       nPins;
  const AMOVIESETUP_PIN * lpPin;
}
AMOVIESETUP_FILTER, * PAMOVIESETUP_FILTER, * FAR LPAMOVIESETUP_FILTER;

/* The DLLENTRY module initialises the module handle on loading */

extern HINSTANCE g_hInst;

/* On DLL load remember which platform we are running on */

extern DWORD g_amPlatform;
extern OSVERSIONINFO g_osInfo;     // Filled in by GetVersionEx

/* Version of IUnknown that is renamed to allow a class to support both
   non delegating and delegating IUnknowns in the same COM object */

#ifndef INONDELEGATINGUNKNOWN_DEFINED
DECLARE_INTERFACE(INonDelegatingUnknown)
{
    STDMETHOD(NonDelegatingQueryInterface) (THIS_ REFIID, LPVOID *) PURE;
    STDMETHOD_(ULONG, NonDelegatingAddRef)(THIS) PURE;
    STDMETHOD_(ULONG, NonDelegatingRelease)(THIS) PURE;
};
#define INONDELEGATINGUNKNOWN_DEFINED
#endif

typedef INonDelegatingUnknown *PNDUNKNOWN;


/* This is the base object class that supports active object counting. As
   part of the debug facilities we trace every time a C++ object is created
   or destroyed. The name of the object has to be passed up through the class
   derivation list during construction as you cannot call virtual functions
   in the constructor. The downside of all this is that every single object
   constructor has to take an object name parameter that describes it */

class CBaseObject
{

private:

    // Disable the copy constructor and assignment by default so you will get
    //   compiler errors instead of unexpected behaviour if you pass objects
    //   by value or assign objects.
    CBaseObject(const CBaseObject& objectSrc);          // no implementation
    void operator=(const CBaseObject& objectSrc);       // no implementation

private:
    static LONG m_cObjects;     /* Total number of objects active */

protected:
#ifdef DEBUG
    DWORD m_dwCookie;           /* Cookie identifying this object */
#endif


public:

    /* These increment and decrement the number of active objects */

    CBaseObject(__in_opt LPCTSTR pName);
#ifdef UNICODE
    CBaseObject(__in_opt LPCSTR pName);
#endif
    ~CBaseObject();

    /* Call this to find if there are any CUnknown derived objects active */

    static LONG ObjectsActive() {
        return m_cObjects;
    };
};


/* An object that supports one or more COM interfaces will be based on
   this class. It supports counting of total objects for DLLCanUnloadNow
   support, and an implementation of the core non delegating IUnknown */

class AM_NOVTABLE CUnknown : public INonDelegatingUnknown,
                 public CBaseObject
{
private:
    const LPUNKNOWN m_pUnknown; /* Owner of this object */

protected:                      /* So we can override NonDelegatingRelease() */
    volatile LONG m_cRef;       /* Number of reference counts */

public:

    CUnknown(__in_opt LPCTSTR pName, __in_opt LPUNKNOWN pUnk);
    virtual ~CUnknown() {};

    // This is redundant, just use the other constructor
    //   as we never touch the HRESULT in this anyway
    CUnknown(__in_opt LPCTSTR Name, __in_opt LPUNKNOWN pUnk, __inout_opt HRESULT *phr);
#ifdef UNICODE
    CUnknown(__in_opt LPCSTR pName, __in_opt LPUNKNOWN pUnk);
    CUnknown(__in_opt LPCSTR pName, __in_opt LPUNKNOWN pUnk,__inout_opt HRESULT *phr);
#endif

    /* Return the owner of this object */

    LPUNKNOWN GetOwner() const {
        return m_pUnknown;
    };

    /* Called from the class factory to create a new instance, it is
       pure virtual so it must be overriden in your derived class */

    /* static CUnknown *CreateInstance(LPUNKNOWN, HRESULT *) */

    /* Non delegating unknown implementation */

    STDMETHODIMP NonDelegatingQueryInterface(REFIID, __deref_out void **);
    STDMETHODIMP_(ULONG) NonDelegatingAddRef();
    STDMETHODIMP_(ULONG) NonDelegatingRelease();
};

/* Return an interface pointer to a requesting client
   performing a thread safe AddRef as necessary */

STDAPI GetInterface(LPUNKNOWN pUnk, __out void **ppv);

/* A function that can create a new COM object */

typedef CUnknown *(CALLBACK *LPFNNewCOMObject)(__in_opt LPUNKNOWN pUnkOuter, __inout_opt HRESULT *phr);

/*  A function (can be NULL) which is called from the DLL entrypoint
    routine for each factory template:

    bLoading - TRUE on DLL load, FALSE on DLL unload
    rclsid   - the m_ClsID of the entry
*/
typedef void (CALLBACK *LPFNInitRoutine)(BOOL bLoading, const CLSID *rclsid);

/* Create one of these per object class in an array so that
   the default class factory code can create new instances */

class CFactoryTemplate {

public:

    const WCHAR *              m_Name;
    const CLSID *              m_ClsID;
    LPFNNewCOMObject           m_lpfnNew;
    LPFNInitRoutine            m_lpfnInit;
    const AMOVIESETUP_FILTER * m_pAMovieSetup_Filter;

    BOOL IsClassID(REFCLSID rclsid) const {
        return (IsEqualCLSID(*m_ClsID,rclsid));
    };

    CUnknown *CreateInstance(__inout_opt LPUNKNOWN pUnk, __inout_opt HRESULT *phr) const {
        CheckPointer(phr,NULL);
        return m_lpfnNew(pUnk, phr);
    };
};


/* You must override the (pure virtual) NonDelegatingQueryInterface to return
   interface pointers (using GetInterface) to the interfaces your derived
   class supports (the default implementation only supports IUnknown) */

#define DECLARE_IUNKNOWN                                        \
    STDMETHODIMP QueryInterface(REFIID riid, __deref_out void **ppv) {      \
        return GetOwner()->QueryInterface(riid,ppv);            \
    };                                                          \
    STDMETHODIMP_(ULONG) AddRef() {                             \
        return GetOwner()->AddRef();                            \
    };                                                          \
    STDMETHODIMP_(ULONG) Release() {                            \
        return GetOwner()->Release();                           \
    };



HINSTANCE	LoadOLEAut32();


#endif /* __COMBASE__ */

//------------------------------------------------------------------------------
// File: WXList.h
//
// Desc: DirectShow base classes - defines a non-MFC generic template list
//       class.
//
// Copyright (c) 1992-2001 Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------


/* A generic list of pointers to objects.
   No storage management or copying is done on the objects pointed to.
   Objectives: avoid using MFC libraries in ndm kernel mode and
   provide a really useful list type.

   The class is thread safe in that separate threads may add and
   delete items in the list concurrently although the application
   must ensure that constructor and destructor access is suitably
   synchronised. An application can cause deadlock with operations
   which use two lists by simultaneously calling
   list1->Operation(list2) and list2->Operation(list1).  So don't!

   The names must not conflict with MFC classes as an application
   may use both.
   */

#ifndef __WXLIST__
#define __WXLIST__

   /* A POSITION represents (in some fashion that's opaque) a cursor
      on the list that can be set to identify any element.  NULL is
      a valid value and several operations regard NULL as the position
      "one step off the end of the list".  (In an n element list there
      are n+1 places to insert and NULL is that "n+1-th" value).
      The POSITION of an element in the list is only invalidated if
      that element is deleted.  Move operations may mean that what
      was a valid POSITION in one list is now a valid POSITION in
      a different list.

      Some operations which at first sight are illegal are allowed as
      harmless no-ops.  For instance RemoveHead is legal on an empty
      list and it returns NULL.  This allows an atomic way to test if
      there is an element there, and if so, get it.  The two operations
      AddTail and RemoveHead thus implement a MONITOR (See Hoare's paper).

      Single element operations return POSITIONs, non-NULL means it worked.
      whole list operations return a BOOL.  TRUE means it all worked.

      This definition is the same as the POSITION type for MFCs, so we must
      avoid defining it twice.
   */
#ifndef __AFX_H__
struct __POSITION { int unused; };
typedef __POSITION* POSITION;
#endif

const int DEFAULTCACHE = 10;    /* Default node object cache size */

/* A class representing one node in a list.
   Each node knows a pointer to it's adjacent nodes and also a pointer
   to the object that it looks after.
   All of these pointers can be retrieved or set through member functions.
*/
class CBaseList 
#ifdef DEBUG
    : public CBaseObject
#endif
{
    /* Making these classes inherit from CBaseObject does nothing
       functionally but it allows us to check there are no memory
       leaks in debug builds. 
    */

public:

#ifdef DEBUG
    class CNode : public CBaseObject {
#else
    class CNode {
#endif

        CNode *m_pPrev;         /* Previous node in the list */
        CNode *m_pNext;         /* Next node in the list */
        void *m_pObject;      /* Pointer to the object */

    public:

        /* Constructor - initialise the object's pointers */
        CNode()
#ifdef DEBUG
            : CBaseObject(NAME("List node"))
#endif
        {
        };


        /* Return the previous node before this one */
        __out CNode *Prev() const { return m_pPrev; };


        /* Return the next node after this one */
        __out CNode *Next() const { return m_pNext; };


        /* Set the previous node before this one */
        void SetPrev(__in_opt CNode *p) { m_pPrev = p; };


        /* Set the next node after this one */
        void SetNext(__in_opt CNode *p) { m_pNext = p; };


        /* Get the pointer to the object for this node */
        __out void *GetData() const { return m_pObject; };


        /* Set the pointer to the object for this node */
        void SetData(__in void *p) { m_pObject = p; };
    };

    class CNodeCache
    {
    public:
        CNodeCache(INT iCacheSize) : m_iCacheSize(iCacheSize),
                                     m_pHead(NULL),
                                     m_iUsed(0)
                                     {};
        ~CNodeCache() {
            CNode *pNode = m_pHead;
            while (pNode) {
                CNode *pCurrent = pNode;
                pNode = pNode->Next();
                delete pCurrent;
            }
        };
        void AddToCache(__inout CNode *pNode)
        {
            if (m_iUsed < m_iCacheSize) {
                pNode->SetNext(m_pHead);
                m_pHead = pNode;
                m_iUsed++;
            } else {
                delete pNode;
            }
        };
        CNode *RemoveFromCache()
        {
            CNode *pNode = m_pHead;
            if (pNode != NULL) {
                m_pHead = pNode->Next();
                m_iUsed--;
                ASSERT(m_iUsed >= 0);
            } else {
                ASSERT(m_iUsed == 0);
            }
            return pNode;
        };
    private:
        INT m_iCacheSize;
        INT m_iUsed;
        CNode *m_pHead;
    };

protected:

    CNode* m_pFirst;    /* Pointer to first node in the list */
    CNode* m_pLast;     /* Pointer to the last node in the list */
    LONG m_Count;       /* Number of nodes currently in the list */

private:

    CNodeCache m_Cache; /* Cache of unused node pointers */

private:

    /* These override the default copy constructor and assignment
       operator for all list classes. They are in the private class
       declaration section so that anybody trying to pass a list
       object by value will generate a compile time error of
       "cannot access the private member function". If these were
       not here then the compiler will create default constructors
       and assignment operators which when executed first take a
       copy of all member variables and then during destruction
       delete them all. This must not be done for any heap
       allocated data.
    */
    CBaseList(const CBaseList &refList);
    CBaseList &operator=(const CBaseList &refList);

public:

    CBaseList(__in_opt LPCTSTR pName,
              INT iItems);

    CBaseList(__in_opt LPCTSTR pName);
#ifdef UNICODE
    CBaseList(__in_opt LPCSTR pName,
              INT iItems);

    CBaseList(__in_opt LPCSTR pName);
#endif
    ~CBaseList();

    /* Remove all the nodes from *this i.e. make the list empty */
    void RemoveAll();


    /* Return a cursor which identifies the first element of *this */
    __out_opt POSITION GetHeadPositionI() const;


    /* Return a cursor which identifies the last element of *this */
    __out_opt POSITION GetTailPositionI() const;


    /* Return the number of objects in *this */
    int GetCountI() const;

protected:
    /* Return the pointer to the object at rp,
       Update rp to the next node in *this
       but make it NULL if it was at the end of *this.
       This is a wart retained for backwards compatibility.
       GetPrev is not implemented.
       Use Next, Prev and Get separately.
    */
    __out void *GetNextI(__inout POSITION& rp) const;


    /* Return a pointer to the object at p
       Asking for the object at NULL will return NULL harmlessly.
    */
    __out_opt void *GetI(__in_opt POSITION p) const;
    __out void *GetValidI(__in POSITION p) const;

public:
    /* return the next / prev position in *this
       return NULL when going past the end/start.
       Next(NULL) is same as GetHeadPosition()
       Prev(NULL) is same as GetTailPosition()
       An n element list therefore behaves like a n+1 element
       cycle with NULL at the start/end.

       !!WARNING!! - This handling of NULL is DIFFERENT from GetNext.

       Some reasons are:
       1. For a list of n items there are n+1 positions to insert
          These are conveniently encoded as the n POSITIONs and NULL.
       2. If you are keeping a list sorted (fairly common) and you
          search forward for an element to insert before and don't
          find it you finish up with NULL as the element before which
          to insert.  You then want that NULL to be a valid POSITION
          so that you can insert before it and you want that insertion
          point to mean the (n+1)-th one that doesn't have a POSITION.
          (symmetrically if you are working backwards through the list).
       3. It simplifies the algebra which the methods generate.
          e.g. AddBefore(p,x) is identical to AddAfter(Prev(p),x)
          in ALL cases.  All the other arguments probably are reflections
          of the algebraic point.
    */
    __out_opt POSITION Next(__in_opt POSITION pos) const
    {
        if (pos == NULL) {
            return (POSITION) m_pFirst;
        }
        CNode *pn = (CNode *) pos;
        return (POSITION) pn->Next();
    } //Next

    // See Next
    __out_opt POSITION Prev(__in_opt POSITION pos) const
    {
        if (pos == NULL) {
            return (POSITION) m_pLast;
        }
        CNode *pn = (CNode *) pos;
        return (POSITION) pn->Prev();
    } //Prev


    /* Return the first position in *this which holds the given
       pointer.  Return NULL if the pointer was not not found.
    */
protected:
    __out_opt POSITION FindI( __in void * pObj) const;

    // ??? Should there be (or even should there be only)
    // ??? POSITION FindNextAfter(void * pObj, POSITION p)
    // ??? And of course FindPrevBefore too.
    // ??? List.Find(&Obj) then becomes List.FindNextAfter(&Obj, NULL)


    /* Remove the first node in *this (deletes the pointer to its
       object from the list, does not free the object itself).
       Return the pointer to its object.
       If *this was already empty it will harmlessly return NULL.
    */
    __out_opt void *RemoveHeadI();


    /* Remove the last node in *this (deletes the pointer to its
       object from the list, does not free the object itself).
       Return the pointer to its object.
       If *this was already empty it will harmlessly return NULL.
    */
    __out_opt void *RemoveTailI();


    /* Remove the node identified by p from the list (deletes the pointer
       to its object from the list, does not free the object itself).
       Asking to Remove the object at NULL will harmlessly return NULL.
       Return the pointer to the object removed.
    */
    __out_opt void *RemoveI(__in_opt POSITION p);

    /* Add single object *pObj to become a new last element of the list.
       Return the new tail position, NULL if it fails.
       If you are adding a COM objects, you might want AddRef it first.
       Other existing POSITIONs in *this are still valid
    */
    __out_opt POSITION AddTailI(__in void * pObj);
public:


    /* Add all the elements in *pList to the tail of *this.
       This duplicates all the nodes in *pList (i.e. duplicates
       all its pointers to objects).  It does not duplicate the objects.
       If you are adding a list of pointers to a COM object into the list
       it's a good idea to AddRef them all  it when you AddTail it.
       Return TRUE if it all worked, FALSE if it didn't.
       If it fails some elements may have been added.
       Existing POSITIONs in *this are still valid

       If you actually want to MOVE the elements, use MoveToTail instead.
    */
    BOOL AddTail(__in CBaseList *pList);


    /* Mirror images of AddHead: */

    /* Add single object to become a new first element of the list.
       Return the new head position, NULL if it fails.
       Existing POSITIONs in *this are still valid
    */
protected:
    __out_opt POSITION AddHeadI(__in void * pObj);
public:

    /* Add all the elements in *pList to the head of *this.
       Same warnings apply as for AddTail.
       Return TRUE if it all worked, FALSE if it didn't.
       If it fails some of the objects may have been added.

       If you actually want to MOVE the elements, use MoveToHead instead.
    */
    BOOL AddHead(__in CBaseList *pList);


    /* Add the object *pObj to *this after position p in *this.
       AddAfter(NULL,x) adds x to the start - equivalent to AddHead
       Return the position of the object added, NULL if it failed.
       Existing POSITIONs in *this are undisturbed, including p.
    */
protected:
    __out_opt POSITION AddAfterI(__in_opt POSITION p, __in void * pObj);
public:

    /* Add the list *pList to *this after position p in *this
       AddAfter(NULL,x) adds x to the start - equivalent to AddHead
       Return TRUE if it all worked, FALSE if it didn't.
       If it fails, some of the objects may be added
       Existing POSITIONs in *this are undisturbed, including p.
    */
    BOOL AddAfter(__in_opt POSITION p, __in CBaseList *pList);


    /* Mirror images:
       Add the object *pObj to this-List after position p in *this.
       AddBefore(NULL,x) adds x to the end - equivalent to AddTail
       Return the position of the new object, NULL if it fails
       Existing POSITIONs in *this are undisturbed, including p.
    */
    protected:
    __out_opt POSITION AddBeforeI(__in_opt POSITION p, __in void * pObj);
    public:

    /* Add the list *pList to *this before position p in *this
       AddAfter(NULL,x) adds x to the start - equivalent to AddHead
       Return TRUE if it all worked, FALSE if it didn't.
       If it fails, some of the objects may be added
       Existing POSITIONs in *this are undisturbed, including p.
    */
    BOOL AddBefore(__in_opt POSITION p, __in CBaseList *pList);


    /* Note that AddAfter(p,x) is equivalent to AddBefore(Next(p),x)
       even in cases where p is NULL or Next(p) is NULL.
       Similarly for mirror images etc.
       This may make it easier to argue about programs.
    */



    /* The following operations do not copy any elements.
       They move existing blocks of elements around by switching pointers.
       They are fairly efficient for long lists as for short lists.
       (Alas, the Count slows things down).

       They split the list into two parts.
       One part remains as the original list, the other part
       is appended to the second list.  There are eight possible
       variations:
       Split the list {after/before} a given element
       keep the {head/tail} portion in the original list
       append the rest to the {head/tail} of the new list.

       Since After is strictly equivalent to Before Next
       we are not in serious need of the Before/After variants.
       That leaves only four.

       If you are processing a list left to right and dumping
       the bits that you have processed into another list as
       you go, the Tail/Tail variant gives the most natural result.
       If you are processing in reverse order, Head/Head is best.

       By using NULL positions and empty lists judiciously either
       of the other two can be built up in two operations.

       The definition of NULL (see Next/Prev etc) means that
       degenerate cases include
          "move all elements to new list"
          "Split a list into two lists"
          "Concatenate two lists"
          (and quite a few no-ops)

       !!WARNING!! The type checking won't buy you much if you get list
       positions muddled up - e.g. use a POSITION that's in a different
       list and see what a mess you get!
    */

    /* Split *this after position p in *this
       Retain as *this the tail portion of the original *this
       Add the head portion to the tail end of *pList
       Return TRUE if it all worked, FALSE if it didn't.

       e.g.
          foo->MoveToTail(foo->GetHeadPosition(), bar);
              moves one element from the head of foo to the tail of bar
          foo->MoveToTail(NULL, bar);
              is a no-op, returns NULL
          foo->MoveToTail(foo->GetTailPosition, bar);
              concatenates foo onto the end of bar and empties foo.

       A better, except excessively long name might be
           MoveElementsFromHeadThroughPositionToOtherTail
    */
    BOOL MoveToTail(__in_opt POSITION pos, __in CBaseList *pList);


    /* Mirror image:
       Split *this before position p in *this.
       Retain in *this the head portion of the original *this
       Add the tail portion to the start (i.e. head) of *pList

       e.g.
          foo->MoveToHead(foo->GetTailPosition(), bar);
              moves one element from the tail of foo to the head of bar
          foo->MoveToHead(NULL, bar);
              is a no-op, returns NULL
          foo->MoveToHead(foo->GetHeadPosition, bar);
              concatenates foo onto the start of bar and empties foo.
    */
    BOOL MoveToHead(__in_opt POSITION pos, __in CBaseList *pList);


    /* Reverse the order of the [pointers to] objects in *this
    */
    void Reverse();


    /* set cursor to the position of each element of list in turn  */
    #define TRAVERSELIST(list, cursor)               \
    for ( cursor = (list).GetHeadPosition()           \
        ; cursor!=NULL                               \
        ; cursor = (list).Next(cursor)                \
        )


    /* set cursor to the position of each element of list in turn
       in reverse order
    */
    #define REVERSETRAVERSELIST(list, cursor)        \
    for ( cursor = (list).GetTailPosition()           \
        ; cursor!=NULL                               \
        ; cursor = (list).Prev(cursor)                \
        )

}; // end of class declaration

template<class OBJECT> class CGenericList : public CBaseList
{
public:
    CGenericList(__in_opt LPCTSTR pName,
                 INT iItems,
                 BOOL bLock = TRUE,
                 BOOL bAlert = FALSE) :
                     CBaseList(pName, iItems) {
        UNREFERENCED_PARAMETER(bAlert);
        UNREFERENCED_PARAMETER(bLock);
    };
    CGenericList(__in_opt LPCTSTR pName) :
                     CBaseList(pName) {
    };

    __out_opt POSITION GetHeadPosition() const { return (POSITION)m_pFirst; }
    __out_opt POSITION GetTailPosition() const { return (POSITION)m_pLast; }
    int GetCount() const { return m_Count; }

    __out OBJECT *GetNext(__inout POSITION& rp) const { return (OBJECT *) GetNextI(rp); }

    __out_opt OBJECT *Get(__in_opt POSITION p) const { return (OBJECT *) GetI(p); }
    __out OBJECT *GetValid(__in POSITION p) const { return (OBJECT *) GetValidI(p); }
    __out_opt OBJECT *GetHead() const  { return Get(GetHeadPosition()); }

    __out_opt OBJECT *RemoveHead() { return (OBJECT *) RemoveHeadI(); }

    __out_opt OBJECT *RemoveTail() { return (OBJECT *) RemoveTailI(); }

    __out_opt OBJECT *Remove(__in_opt POSITION p) { return (OBJECT *) RemoveI(p); }
    __out_opt POSITION AddBefore(__in_opt POSITION p, __in OBJECT * pObj) { return AddBeforeI(p, pObj); }
    __out_opt POSITION AddAfter(__in_opt POSITION p, __in OBJECT * pObj)  { return AddAfterI(p, pObj); }
    __out_opt POSITION AddHead(__in OBJECT * pObj) { return AddHeadI(pObj); }
    __out_opt POSITION AddTail(__in OBJECT * pObj)  { return AddTailI(pObj); }
    BOOL AddTail(__in CGenericList<OBJECT> *pList)
            { return CBaseList::AddTail((CBaseList *) pList); }
    BOOL AddHead(__in CGenericList<OBJECT> *pList)
            { return CBaseList::AddHead((CBaseList *) pList); }
    BOOL AddAfter(__in_opt POSITION p, __in CGenericList<OBJECT> *pList)
            { return CBaseList::AddAfter(p, (CBaseList *) pList); };
    BOOL AddBefore(__in_opt POSITION p, __in CGenericList<OBJECT> *pList)
            { return CBaseList::AddBefore(p, (CBaseList *) pList); };
    __out_opt POSITION Find( __in OBJECT * pObj) const { return FindI(pObj); }
}; // end of class declaration



/* These define the standard list types */

typedef CGenericList<CBaseObject> CBaseObjectList;
typedef CGenericList<IUnknown> CBaseInterfaceList;

#endif /* __WXLIST__ */

//------------------------------------------------------------------------------
// File: MtType.h
//
// Desc: DirectShow base classes - defines a class that holds and manages
//       media type information.
//
// Copyright (c) 1992-2001 Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------


#ifndef __MTYPE__
#define __MTYPE__

/* Helper class that derived pin objects can use to compare media
   types etc. Has same data members as the struct AM_MEDIA_TYPE defined
   in the streams IDL file, but also has (non-virtual) functions */

class CMediaType : public _AMMediaType {

public:

    ~CMediaType();
    CMediaType();
    CMediaType(const GUID * majortype);
    CMediaType(const AM_MEDIA_TYPE&, __out_opt HRESULT* phr = NULL);
    CMediaType(const CMediaType&, __out_opt HRESULT* phr = NULL);

    CMediaType& operator=(const CMediaType&);
    CMediaType& operator=(const AM_MEDIA_TYPE&);

    BOOL operator == (const CMediaType&) const;
    BOOL operator != (const CMediaType&) const;

    HRESULT Set(const CMediaType& rt);
    HRESULT Set(const AM_MEDIA_TYPE& rt);

    BOOL IsValid() const;

    const GUID *Type() const { return &majortype;} ;
    void SetType(const GUID *);
    const GUID *Subtype() const { return &subtype;} ;
    void SetSubtype(const GUID *);

    BOOL IsFixedSize() const {return bFixedSizeSamples; };
    BOOL IsTemporalCompressed() const {return bTemporalCompression; };
    ULONG GetSampleSize() const;

    void SetSampleSize(ULONG sz);
    void SetVariableSize();
    void SetTemporalCompression(BOOL bCompressed);

    // read/write pointer to format - can't change length without
    // calling SetFormat, AllocFormatBuffer or ReallocFormatBuffer

    BYTE*   Format() const {return pbFormat; };
    ULONG   FormatLength() const { return cbFormat; };

    void SetFormatType(const GUID *);
    const GUID *FormatType() const {return &formattype; };
    BOOL SetFormat(__in_bcount(length) BYTE *pFormat, ULONG length);
    void ResetFormatBuffer();
    BYTE* AllocFormatBuffer(ULONG length);
    BYTE* ReallocFormatBuffer(ULONG length);

    void InitMediaType();

    BOOL MatchesPartial(const CMediaType* ppartial) const;
    BOOL IsPartiallySpecified(void) const;
};


/* General purpose functions to copy and delete a task allocated AM_MEDIA_TYPE
   structure which is useful when using the IEnumMediaFormats interface as
   the implementation allocates the structures which you must later delete */

void WINAPI DeleteMediaType(__inout_opt AM_MEDIA_TYPE *pmt);
AM_MEDIA_TYPE * WINAPI CreateMediaType(AM_MEDIA_TYPE const *pSrc);
HRESULT WINAPI CopyMediaType(__out AM_MEDIA_TYPE *pmtTarget, const AM_MEDIA_TYPE *pmtSource);
void WINAPI FreeMediaType(__inout AM_MEDIA_TYPE& mt);

//  Initialize a media type from a WAVEFORMATEX

STDAPI CreateAudioMediaType(
    const WAVEFORMATEX *pwfx,
    __out AM_MEDIA_TYPE *pmt,
    BOOL bSetFormat);

#endif /* __MTYPE__ */

//------------------------------------------------------------------------------
// File: AMFilter.h
//
// Desc: DirectShow base classes - efines class hierarchy for streams
//       architecture.
//
// Copyright (c) 1992-2001 Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------


#ifndef __FILTER__
#define __FILTER__

/* The following classes are declared in this header: */

class CBaseMediaFilter;     // IMediaFilter support
class CBaseFilter;          // IBaseFilter,IMediaFilter support
class CBasePin;             // Abstract base class for IPin interface
class CEnumPins;            // Enumerate input and output pins
class CEnumMediaTypes;      // Enumerate the pin's preferred formats
class CBaseOutputPin;       // Adds data provider member functions
class CBaseInputPin;        // Implements IMemInputPin interface
class CMediaSample;         // Basic transport unit for IMemInputPin
class CBaseAllocator;       // General list guff for most allocators
class CMemAllocator;        // Implements memory buffer allocation


//=====================================================================
//=====================================================================
//
// QueryFilterInfo and QueryPinInfo AddRef the interface pointers
// they return.  You can use the macro below to release the interface.
//
//=====================================================================
//=====================================================================

#define QueryFilterInfoReleaseGraph(fi) if ((fi).pGraph) (fi).pGraph->Release();

#define QueryPinInfoReleaseFilter(pi) if ((pi).pFilter) (pi).pFilter->Release();

//=====================================================================
//=====================================================================
// Defines CBaseMediaFilter
//
// Abstract base class implementing IMediaFilter.
//
// Typically you will derive your filter from CBaseFilter rather than
// this,  unless you are implementing an object such as a plug-in
// distributor that needs to support IMediaFilter but not IBaseFilter.
//
// Note that IMediaFilter is derived from IPersist to allow query of
// class id.
//=====================================================================
//=====================================================================

class AM_NOVTABLE CBaseMediaFilter : public CUnknown,
                                     public IMediaFilter
{

protected:

    FILTER_STATE    m_State;            // current state: running, paused
    IReferenceClock *m_pClock;          // this filter's reference clock
    // note: all filters in a filter graph use the same clock

    // offset from stream time to reference time
    CRefTime        m_tStart;

    CLSID	    m_clsid;            // This filters clsid
                                        // used for serialization
    CCritSec        *m_pLock;           // Object we use for locking

public:

    CBaseMediaFilter(
        __in_opt LPCTSTR pName,
        __inout_opt LPUNKNOWN pUnk,
        __in CCritSec  *pLock,
	REFCLSID   clsid);

    virtual ~CBaseMediaFilter();

    DECLARE_IUNKNOWN

    // override this to say what interfaces we support where
    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, __deref_out void ** ppv);

    //
    // --- IPersist method ---
    //

    STDMETHODIMP GetClassID(__out CLSID *pClsID);

    // --- IMediaFilter methods ---

    STDMETHODIMP GetState(DWORD dwMSecs, __out FILTER_STATE *State);

    STDMETHODIMP SetSyncSource(__inout_opt IReferenceClock *pClock);

    STDMETHODIMP GetSyncSource(__deref_out_opt IReferenceClock **pClock);

    // default implementation of Stop and Pause just record the
    // state. Override to activate or de-activate your filter.
    // Note that Run when called from Stopped state will call Pause
    // to ensure activation, so if you are a source or transform
    // you will probably not need to override Run.
    STDMETHODIMP Stop();
    STDMETHODIMP Pause();


    // the start parameter is the difference to be added to the
    // sample's stream time to get the reference time for
    // its presentation
    STDMETHODIMP Run(REFERENCE_TIME tStart);

    // --- helper methods ---

    // return the current stream time - ie find out what
    // stream time should be appearing now
    virtual HRESULT StreamTime(CRefTime& rtStream);

    // Is the filter currently active? (running or paused)
    BOOL IsActive() {
        CAutoLock cObjectLock(m_pLock);
        return ((m_State == State_Paused) || (m_State == State_Running));
    };
};

//=====================================================================
//=====================================================================
// Defines CBaseFilter
//
// An abstract class providing basic IBaseFilter support for pin
// enumeration and filter information reading.
//
// We cannot derive from CBaseMediaFilter since methods in IMediaFilter
// are also in IBaseFilter and would be ambiguous. Since much of the code
// assumes that they derive from a class that has m_State and other state
// directly available, we duplicate code from CBaseMediaFilter rather than
// having a member variable.
//
// Derive your filter from this, or from a derived object such as
// CTransformFilter.
//=====================================================================
//=====================================================================


class AM_NOVTABLE CBaseFilter : public CUnknown,        // Handles an IUnknown
                    public IBaseFilter,     // The Filter Interface
                    public IAMovieSetup     // For un/registration
{

friend class CBasePin;

protected:
    FILTER_STATE    m_State;            // current state: running, paused
    IReferenceClock *m_pClock;          // this graph's ref clock
    CRefTime        m_tStart;           // offset from stream time to reference time
    CLSID	    m_clsid;            // This filters clsid
                                        // used for serialization
    CCritSec        *m_pLock;           // Object we use for locking

    WCHAR           *m_pName;           // Full filter name
    IFilterGraph    *m_pGraph;          // Graph we belong to
    IMediaEventSink *m_pSink;           // Called with notify events
    LONG            m_PinVersion;       // Current pin version

public:

    CBaseFilter(
        __in_opt LPCTSTR pName,   // Object description
        __inout_opt LPUNKNOWN pUnk,  // IUnknown of delegating object
        __in CCritSec  *pLock,    // Object who maintains lock
	REFCLSID   clsid);        // The clsid to be used to serialize this filter

    CBaseFilter(
        __in_opt LPCTSTR pName,    // Object description
        __in_opt LPUNKNOWN pUnk,  // IUnknown of delegating object
        __in CCritSec  *pLock,    // Object who maintains lock
	REFCLSID   clsid,         // The clsid to be used to serialize this filter
        __inout HRESULT   *phr);  // General OLE return code
#ifdef UNICODE
    CBaseFilter(
        __in_opt LPCSTR pName,    // Object description
        __in_opt LPUNKNOWN pUnk,  // IUnknown of delegating object
        __in CCritSec  *pLock,    // Object who maintains lock
	REFCLSID   clsid);        // The clsid to be used to serialize this filter

    CBaseFilter(
        __in_opt LPCSTR pName,     // Object description
        __in_opt LPUNKNOWN pUnk,  // IUnknown of delegating object
        __in CCritSec  *pLock,    // Object who maintains lock
	REFCLSID   clsid,         // The clsid to be used to serialize this filter
        __inout HRESULT   *phr);  // General OLE return code
#endif
    ~CBaseFilter();

    DECLARE_IUNKNOWN

    // override this to say what interfaces we support where
    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, __deref_out void ** ppv);
#ifdef DEBUG
    STDMETHODIMP_(ULONG) NonDelegatingRelease();
#endif

    //
    // --- IPersist method ---
    //

    STDMETHODIMP GetClassID(__out CLSID *pClsID);

    // --- IMediaFilter methods ---

    STDMETHODIMP GetState(DWORD dwMSecs, __out FILTER_STATE *State);

    STDMETHODIMP SetSyncSource(__in_opt IReferenceClock *pClock);

    STDMETHODIMP GetSyncSource(__deref_out_opt IReferenceClock **pClock);


    // override Stop and Pause so we can activate the pins.
    // Note that Run will call Pause first if activation needed.
    // Override these if you want to activate your filter rather than
    // your pins.
    STDMETHODIMP Stop();
    STDMETHODIMP Pause();

    // the start parameter is the difference to be added to the
    // sample's stream time to get the reference time for
    // its presentation
    STDMETHODIMP Run(REFERENCE_TIME tStart);

    // --- helper methods ---

    // return the current stream time - ie find out what
    // stream time should be appearing now
    virtual HRESULT StreamTime(CRefTime& rtStream);

    // Is the filter currently active?
    BOOL IsActive() {
        CAutoLock cObjectLock(m_pLock);
        return ((m_State == State_Paused) || (m_State == State_Running));
    };

    // Is this filter stopped (without locking)
    BOOL IsStopped() {
        return (m_State == State_Stopped);
    };

    //
    // --- IBaseFilter methods ---
    //

    // pin enumerator
    STDMETHODIMP EnumPins(
                    __deref_out IEnumPins ** ppEnum);


    // default behaviour of FindPin assumes pin ids are their names
    STDMETHODIMP FindPin(
        LPCWSTR Id,
        __deref_out IPin ** ppPin
    );

    STDMETHODIMP QueryFilterInfo(
                    __out FILTER_INFO * pInfo);

    STDMETHODIMP JoinFilterGraph(
                    __inout_opt IFilterGraph * pGraph,
                    __in_opt LPCWSTR pName);

    // return a Vendor information string. Optional - may return E_NOTIMPL.
    // memory returned should be freed using CoTaskMemFree
    // default implementation returns E_NOTIMPL
    STDMETHODIMP QueryVendorInfo(
                    __deref_out LPWSTR* pVendorInfo
            );

    // --- helper methods ---

    // send an event notification to the filter graph if we know about it.
    // returns S_OK if delivered, S_FALSE if the filter graph does not sink
    // events, or an error otherwise.
    HRESULT NotifyEvent(
        long EventCode,
        LONG_PTR EventParam1,
        LONG_PTR EventParam2);

    // return the filter graph we belong to
    __out_opt IFilterGraph *GetFilterGraph() {
        return m_pGraph;
    }

    // Request reconnect
    // pPin is the pin to reconnect
    // pmt is the type to reconnect with - can be NULL
    // Calls ReconnectEx on the filter graph
    HRESULT ReconnectPin(IPin *pPin, __in_opt AM_MEDIA_TYPE const *pmt);

    // find out the current pin version (used by enumerators)
    virtual LONG GetPinVersion();
    void IncrementPinVersion();

    // you need to supply these to access the pins from the enumerator
    // and for default Stop and Pause/Run activation.
    virtual int GetPinCount() PURE;
    virtual CBasePin *GetPin(int n) PURE;

    // --- IAMovieSetup methods ---

    STDMETHODIMP Register();    // ask filter to register itself
    STDMETHODIMP Unregister();  // and unregister itself

    // --- setup helper methods ---
    // (override to return filters setup data)

    virtual __out_opt LPAMOVIESETUP_FILTER GetSetupData(){ return NULL; }

};


//=====================================================================
//=====================================================================
// Defines CBasePin
//
// Abstract class that supports the basics of IPin
//=====================================================================
//=====================================================================

class  AM_NOVTABLE CBasePin : public CUnknown, public IPin, public IQualityControl
{

protected:

    WCHAR *         m_pName;		        // This pin's name
    IPin            *m_Connected;               // Pin we have connected to
    PIN_DIRECTION   m_dir;                      // Direction of this pin
    CCritSec        *m_pLock;                   // Object we use for locking
    bool            m_bRunTimeError;            // Run time error generated
    bool            m_bCanReconnectWhenActive;  // OK to reconnect when active
    bool            m_bTryMyTypesFirst;         // When connecting enumerate
                                                // this pin's types first
    CBaseFilter    *m_pFilter;                  // Filter we were created by
    IQualityControl *m_pQSink;                  // Target for Quality messages
    LONG            m_TypeVersion;              // Holds current type version
    CMediaType      m_mt;                       // Media type of connection

    CRefTime        m_tStart;                   // time from NewSegment call
    CRefTime        m_tStop;                    // time from NewSegment
    double          m_dRate;                    // rate from NewSegment

#ifdef DEBUG
    LONG            m_cRef;                     // Ref count tracing
#endif

    // displays pin connection information

#ifdef DEBUG
    void DisplayPinInfo(IPin *pReceivePin);
    void DisplayTypeInfo(IPin *pPin, const CMediaType *pmt);
#else
    void DisplayPinInfo(IPin *pReceivePin) {};
    void DisplayTypeInfo(IPin *pPin, const CMediaType *pmt) {};
#endif

    // used to agree a media type for a pin connection

    // given a specific media type, attempt a connection (includes
    // checking that the type is acceptable to this pin)
    HRESULT
    AttemptConnection(
        IPin* pReceivePin,      // connect to this pin
        const CMediaType* pmt   // using this type
    );

    // try all the media types in this enumerator - for each that
    // we accept, try to connect using ReceiveConnection.
    HRESULT TryMediaTypes(
                        IPin *pReceivePin,          // connect to this pin
                        __in_opt const CMediaType *pmt,  // proposed type from Connect
                        IEnumMediaTypes *pEnum);    // try this enumerator

    // establish a connection with a suitable mediatype. Needs to
    // propose a media type if the pmt pointer is null or partially
    // specified - use TryMediaTypes on both our and then the other pin's
    // enumerator until we find one that works.
    HRESULT AgreeMediaType(
                        IPin *pReceivePin,      // connect to this pin
                        const CMediaType *pmt);      // proposed type from Connect

public:

    CBasePin(
        __in_opt LPCTSTR pObjectName,         // Object description
        __in CBaseFilter *pFilter,       // Owning filter who knows about pins
        __in CCritSec *pLock,            // Object who implements the lock
        __inout HRESULT *phr,               // General OLE return code
        __in_opt LPCWSTR pName,              // Pin name for us
        PIN_DIRECTION dir);         // Either PINDIR_INPUT or PINDIR_OUTPUT
#ifdef UNICODE
    CBasePin(
        __in_opt LPCSTR pObjectName,         // Object description
        __in CBaseFilter *pFilter,       // Owning filter who knows about pins
        __in CCritSec *pLock,            // Object who implements the lock
        __inout HRESULT *phr,               // General OLE return code
        __in_opt LPCWSTR pName,              // Pin name for us
        PIN_DIRECTION dir);         // Either PINDIR_INPUT or PINDIR_OUTPUT
#endif
    virtual ~CBasePin();

    DECLARE_IUNKNOWN

    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, __deref_out void ** ppv);
    STDMETHODIMP_(ULONG) NonDelegatingRelease();
    STDMETHODIMP_(ULONG) NonDelegatingAddRef();

    // --- IPin methods ---

    // take lead role in establishing a connection. Media type pointer
    // may be null, or may point to partially-specified mediatype
    // (subtype or format type may be GUID_NULL).
    STDMETHODIMP Connect(
        IPin * pReceivePin,
        __in_opt const AM_MEDIA_TYPE *pmt   // optional media type
    );

    // (passive) accept a connection from another pin
    STDMETHODIMP ReceiveConnection(
        IPin * pConnector,      // this is the initiating connecting pin
        const AM_MEDIA_TYPE *pmt   // this is the media type we will exchange
    );

    STDMETHODIMP Disconnect();

    STDMETHODIMP ConnectedTo(__deref_out IPin **pPin);

    STDMETHODIMP ConnectionMediaType(__out AM_MEDIA_TYPE *pmt);

    STDMETHODIMP QueryPinInfo(
        __out PIN_INFO * pInfo
    );

    STDMETHODIMP QueryDirection(
    	__out PIN_DIRECTION * pPinDir
    );

    STDMETHODIMP QueryId(
        __deref_out LPWSTR * Id
    );

    // does the pin support this media type
    STDMETHODIMP QueryAccept(
        const AM_MEDIA_TYPE *pmt
    );

    // return an enumerator for this pins preferred media types
    STDMETHODIMP EnumMediaTypes(
        __deref_out IEnumMediaTypes **ppEnum
    );

    // return an array of IPin* - the pins that this pin internally connects to
    // All pins put in the array must be AddReffed (but no others)
    // Errors: "Can't say" - FAIL, not enough slots - return S_FALSE
    // Default: return E_NOTIMPL
    // The filter graph will interpret NOT_IMPL as any input pin connects to
    // all visible output pins and vice versa.
    // apPin can be NULL if nPin==0 (not otherwise).
    STDMETHODIMP QueryInternalConnections(
        __out_ecount_part(*nPin,*nPin) IPin* *apPin,     // array of IPin*
        __inout ULONG *nPin                  // on input, the number of slots
                                             // on output  the number of pins
    ) { return E_NOTIMPL; }

    // Called when no more data will be sent
    STDMETHODIMP EndOfStream(void);

    // Begin/EndFlush still PURE

    // NewSegment notifies of the start/stop/rate applying to the data
    // about to be received. Default implementation records data and
    // returns S_OK.
    // Override this to pass downstream.
    STDMETHODIMP NewSegment(
                    REFERENCE_TIME tStart,
                    REFERENCE_TIME tStop,
                    double dRate);

    //================================================================================
    // IQualityControl methods
    //================================================================================

    STDMETHODIMP Notify(IBaseFilter * pSender, Quality q);

    STDMETHODIMP SetSink(IQualityControl * piqc);

    // --- helper methods ---

    // Returns true if the pin is connected. false otherwise.
    BOOL IsConnected(void) {return (m_Connected != NULL); };
    // Return the pin this is connected to (if any)
    IPin * GetConnected() { return m_Connected; };

    // Check if our filter is currently stopped
    BOOL IsStopped() {
        return (m_pFilter->m_State == State_Stopped);
    };

    // find out the current type version (used by enumerators)
    virtual LONG GetMediaTypeVersion();
    void IncrementTypeVersion();

    // switch the pin to active (paused or running) mode
    // not an error to call this if already active
    virtual HRESULT Active(void);

    // switch the pin to inactive state - may already be inactive
    virtual HRESULT Inactive(void);

    // Notify of Run() from filter
    virtual HRESULT Run(REFERENCE_TIME tStart);

    // check if the pin can support this specific proposed type and format
    virtual HRESULT CheckMediaType(const CMediaType *) PURE;

    // set the connection to use this format (previously agreed)
    virtual HRESULT SetMediaType(const CMediaType *);

    // check that the connection is ok before verifying it
    // can be overridden eg to check what interfaces will be supported.
    virtual HRESULT CheckConnect(IPin *);

    // Set and release resources required for a connection
    virtual HRESULT BreakConnect();
    virtual HRESULT CompleteConnect(IPin *pReceivePin);

    // returns the preferred formats for a pin
    virtual HRESULT GetMediaType(int iPosition, __inout CMediaType *pMediaType);

    // access to NewSegment values
    REFERENCE_TIME CurrentStopTime() {
        return m_tStop;
    }
    REFERENCE_TIME CurrentStartTime() {
        return m_tStart;
    }
    double CurrentRate() {
        return m_dRate;
    }

    //  Access name
    LPWSTR Name() { return m_pName; };

    //  Can reconnectwhen active?
    void SetReconnectWhenActive(bool bCanReconnect)
    {
        m_bCanReconnectWhenActive = bCanReconnect;
    }

    bool CanReconnectWhenActive()
    {
        return m_bCanReconnectWhenActive;
    }

protected:
    STDMETHODIMP DisconnectInternal();
};


//=====================================================================
//=====================================================================
// Defines CEnumPins
//
// Pin enumerator class that works by calling CBaseFilter. This interface
// is provided by CBaseFilter::EnumPins and calls GetPinCount() and
// GetPin() to enumerate existing pins. Needs to be a separate object so
// that it can be cloned (creating an existing object at the same
// position in the enumeration)
//
//=====================================================================
//=====================================================================

class CEnumPins : public IEnumPins      // The interface we support
{
    int m_Position;                 // Current ordinal position
    int m_PinCount;                 // Number of pins available
    CBaseFilter *m_pFilter;         // The filter who owns us
    LONG m_Version;                 // Pin version information
    LONG m_cRef;

    typedef CGenericList<CBasePin> CPinList;

    CPinList m_PinCache;	    // These pointers have not been AddRef'ed and
				    // so they should not be dereferenced.  They are
				    // merely kept to ID which pins have been enumerated.

#ifdef DEBUG
    DWORD m_dwCookie;
#endif

    /* If while we are retrieving a pin for example from the filter an error
       occurs we assume that our internal state is stale with respect to the
       filter (someone may have deleted all the pins). We can check before
       starting whether or not the operation is likely to fail by asking the
       filter what it's current version number is. If the filter has not
       overriden the GetPinVersion method then this will always match */

    BOOL AreWeOutOfSync() {
        return (m_pFilter->GetPinVersion() == m_Version ? FALSE : TRUE);
    };

    /* This method performs the same operations as Reset, except is does not clear
       the cache of pins already enumerated. */

    STDMETHODIMP Refresh();

public:

    CEnumPins(
        __in CBaseFilter *pFilter,
        __in_opt CEnumPins *pEnumPins);

    virtual ~CEnumPins();

    // IUnknown
    STDMETHODIMP QueryInterface(REFIID riid, __deref_out void **ppv);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();

    // IEnumPins
    STDMETHODIMP Next(
        ULONG cPins,         // place this many pins...
        __out_ecount(cPins) IPin ** ppPins,    // ...in this array of IPin*
        __out_opt ULONG * pcFetched    // actual count passed returned here
    );

    STDMETHODIMP Skip(ULONG cPins);
    STDMETHODIMP Reset();
    STDMETHODIMP Clone(__deref_out IEnumPins **ppEnum);


};


//=====================================================================
//=====================================================================
// Defines CEnumMediaTypes
//
// Enumerates the preferred formats for input and output pins
//=====================================================================
//=====================================================================

class CEnumMediaTypes : public IEnumMediaTypes    // The interface we support
{
    int m_Position;           // Current ordinal position
    CBasePin *m_pPin;         // The pin who owns us
    LONG m_Version;           // Media type version value
    LONG m_cRef;
#ifdef DEBUG
    DWORD m_dwCookie;
#endif

    /* The media types a filter supports can be quite dynamic so we add to
       the general IEnumXXXX interface the ability to be signaled when they
       change via an event handle the connected filter supplies. Until the
       Reset method is called after the state changes all further calls to
       the enumerator (except Reset) will return E_UNEXPECTED error code */

    BOOL AreWeOutOfSync() {
        return (m_pPin->GetMediaTypeVersion() == m_Version ? FALSE : TRUE);
    };

public:

    CEnumMediaTypes(
        __in CBasePin *pPin,
        __in_opt CEnumMediaTypes *pEnumMediaTypes);

    virtual ~CEnumMediaTypes();

    // IUnknown
    STDMETHODIMP QueryInterface(REFIID riid, __deref_out void **ppv);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();

    // IEnumMediaTypes
    STDMETHODIMP Next(
        ULONG cMediaTypes,          // place this many pins...
        __out_ecount(cMediaTypes) AM_MEDIA_TYPE ** ppMediaTypes,  // ...in this array
        __out_opt ULONG * pcFetched           // actual count passed
    );

    STDMETHODIMP Skip(ULONG cMediaTypes);
    STDMETHODIMP Reset();
    STDMETHODIMP Clone(__deref_out IEnumMediaTypes **ppEnum);
};




//=====================================================================
//=====================================================================
// Defines CBaseOutputPin
//
// class derived from CBasePin that can pass buffers to a connected pin
// that supports IMemInputPin. Supports IPin.
//
// Derive your output pin from this.
//
//=====================================================================
//=====================================================================

class  AM_NOVTABLE CBaseOutputPin : public CBasePin
{

protected:

    IMemAllocator *m_pAllocator;
    IMemInputPin *m_pInputPin;        // interface on the downstreaminput pin
                                      // set up in CheckConnect when we connect.

public:

    CBaseOutputPin(
        __in_opt LPCTSTR pObjectName,
        __in CBaseFilter *pFilter,
        __in CCritSec *pLock,
        __inout HRESULT *phr,
        __in_opt LPCWSTR pName);
#ifdef UNICODE
    CBaseOutputPin(
        __in_opt LPCSTR pObjectName,
        __in CBaseFilter *pFilter,
        __in CCritSec *pLock,
        __inout HRESULT *phr,
        __in_opt LPCWSTR pName);
#endif
    // override CompleteConnect() so we can negotiate an allocator
    virtual HRESULT CompleteConnect(IPin *pReceivePin);

    // negotiate the allocator and its buffer size/count and other properties
    // Calls DecideBufferSize to set properties
    virtual HRESULT DecideAllocator(IMemInputPin * pPin, __deref_out IMemAllocator ** pAlloc);

    // override this to set the buffer size and count. Return an error
    // if the size/count is not to your liking.
    // The allocator properties passed in are those requested by the
    // input pin - use eg the alignment and prefix members if you have
    // no preference on these.
    virtual HRESULT DecideBufferSize(
        IMemAllocator * pAlloc,
        __inout ALLOCATOR_PROPERTIES * ppropInputRequest
    ) PURE;

    // returns an empty sample buffer from the allocator
    virtual HRESULT GetDeliveryBuffer(__deref_out IMediaSample ** ppSample,
                                      __in_opt REFERENCE_TIME * pStartTime,
                                      __in_opt REFERENCE_TIME * pEndTime,
                                      DWORD dwFlags);

    // deliver a filled-in sample to the connected input pin
    // note - you need to release it after calling this. The receiving
    // pin will addref the sample if it needs to hold it beyond the
    // call.
    virtual HRESULT Deliver(IMediaSample *);

    // override this to control the connection
    virtual HRESULT InitAllocator(__deref_out IMemAllocator **ppAlloc);
    HRESULT CheckConnect(IPin *pPin);
    HRESULT BreakConnect();

    // override to call Commit and Decommit
    HRESULT Active(void);
    HRESULT Inactive(void);

    // we have a default handling of EndOfStream which is to return
    // an error, since this should be called on input pins only
    STDMETHODIMP EndOfStream(void);

    // called from elsewhere in our filter to pass EOS downstream to
    // our connected input pin
    virtual HRESULT DeliverEndOfStream(void);

    // same for Begin/EndFlush - we handle Begin/EndFlush since it
    // is an error on an output pin, and we have Deliver methods to
    // call the methods on the connected pin
    STDMETHODIMP BeginFlush(void);
    STDMETHODIMP EndFlush(void);
    virtual HRESULT DeliverBeginFlush(void);
    virtual HRESULT DeliverEndFlush(void);

    // deliver NewSegment to connected pin - you will need to
    // override this if you queue any data in your output pin.
    virtual HRESULT DeliverNewSegment(
                        REFERENCE_TIME tStart,
                        REFERENCE_TIME tStop,
                        double dRate);

    //================================================================================
    // IQualityControl methods
    //================================================================================

    // All inherited from CBasePin and not overridden here.
    // STDMETHODIMP Notify(IBaseFilter * pSender, Quality q);
    // STDMETHODIMP SetSink(IQualityControl * piqc);
};


//=====================================================================
//=====================================================================
// Defines CBaseInputPin
//
// derive your standard input pin from this.
// you need to supply GetMediaType and CheckConnect etc (see CBasePin),
// and you need to supply Receive to do something more useful.
//
//=====================================================================
//=====================================================================

class AM_NOVTABLE CBaseInputPin : public CBasePin,
                                  public IMemInputPin
{

protected:

    IMemAllocator *m_pAllocator;    // Default memory allocator

    // allocator is read-only, so received samples
    // cannot be modified (probably only relevant to in-place
    // transforms
    BYTE m_bReadOnly;

    // in flushing state (between BeginFlush and EndFlush)
    // if TRUE, all Receives are returned with S_FALSE
    BYTE m_bFlushing;

    // Sample properties - initalized in Receive
    AM_SAMPLE2_PROPERTIES m_SampleProps;

public:

    CBaseInputPin(
        __in_opt LPCTSTR pObjectName,
        __in CBaseFilter *pFilter,
        __in CCritSec *pLock,
        __inout HRESULT *phr,
        __in_opt LPCWSTR pName);
#ifdef UNICODE
    CBaseInputPin(
        __in_opt LPCSTR pObjectName,
        __in CBaseFilter *pFilter,
        __in CCritSec *pLock,
        __inout HRESULT *phr,
        __in_opt LPCWSTR pName);
#endif
    virtual ~CBaseInputPin();

    DECLARE_IUNKNOWN

    // override this to publicise our interfaces
    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, __deref_out void **ppv);

    // return the allocator interface that this input pin
    // would like the output pin to use
    STDMETHODIMP GetAllocator(__deref_out IMemAllocator ** ppAllocator);

    // tell the input pin which allocator the output pin is actually
    // going to use.
    STDMETHODIMP NotifyAllocator(
                    IMemAllocator * pAllocator,
                    BOOL bReadOnly);

    // do something with this media sample
    STDMETHODIMP Receive(IMediaSample *pSample);

    // do something with these media samples
    STDMETHODIMP ReceiveMultiple (
        __in_ecount(nSamples) IMediaSample **pSamples,
        long nSamples,
        __out long *nSamplesProcessed);

    // See if Receive() blocks
    STDMETHODIMP ReceiveCanBlock();

    // Default handling for BeginFlush - call at the beginning
    // of your implementation (makes sure that all Receive calls
    // fail). After calling this, you need to free any queued data
    // and then call downstream.
    STDMETHODIMP BeginFlush(void);

    // default handling for EndFlush - call at end of your implementation
    // - before calling this, ensure that there is no queued data and no thread
    // pushing any more without a further receive, then call downstream,
    // then call this method to clear the m_bFlushing flag and re-enable
    // receives
    STDMETHODIMP EndFlush(void);

    // this method is optional (can return E_NOTIMPL).
    // default implementation returns E_NOTIMPL. Override if you have
    // specific alignment or prefix needs, but could use an upstream
    // allocator
    STDMETHODIMP GetAllocatorRequirements(__out ALLOCATOR_PROPERTIES*pProps);

    // Release the pin's allocator.
    HRESULT BreakConnect();

    // helper method to check the read-only flag
    BOOL IsReadOnly() {
        return m_bReadOnly;
    };

    // helper method to see if we are flushing
    BOOL IsFlushing() {
        return m_bFlushing;
    };

    //  Override this for checking whether it's OK to process samples
    //  Also call this from EndOfStream.
    virtual HRESULT CheckStreaming();

    // Pass a Quality notification on to the appropriate sink
    HRESULT PassNotify(Quality& q);


    //================================================================================
    // IQualityControl methods (from CBasePin)
    //================================================================================

    STDMETHODIMP Notify(IBaseFilter * pSender, Quality q);

    // no need to override:
    // STDMETHODIMP SetSink(IQualityControl * piqc);


    // switch the pin to inactive state - may already be inactive
    virtual HRESULT Inactive(void);

    // Return sample properties pointer
    AM_SAMPLE2_PROPERTIES * SampleProps() {
        ASSERT(m_SampleProps.cbData != 0);
        return &m_SampleProps;
    }

};

///////////////////////////////////////////////////////////////////////////
// CDynamicOutputPin
//

class CDynamicOutputPin : public CBaseOutputPin,
                          public IPinFlowControl
{
public:
#ifdef UNICODE
    CDynamicOutputPin(
        __in_opt LPCSTR pObjectName,
        __in CBaseFilter *pFilter,
        __in CCritSec *pLock,
        __inout HRESULT *phr,
        __in_opt LPCWSTR pName);
#endif

    CDynamicOutputPin(
        __in_opt LPCTSTR pObjectName,
        __in CBaseFilter *pFilter,
        __in CCritSec *pLock,
        __inout HRESULT *phr,
        __in_opt LPCWSTR pName);

    ~CDynamicOutputPin();

    // IUnknown Methods
    DECLARE_IUNKNOWN
    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, __deref_out void **ppv);

    // IPin Methods
    STDMETHODIMP Disconnect(void);

    // IPinFlowControl Methods
    STDMETHODIMP Block(DWORD dwBlockFlags, HANDLE hEvent);

    //  Set graph config info
    void SetConfigInfo(IGraphConfig *pGraphConfig, HANDLE hStopEvent);

    #ifdef DEBUG
    virtual HRESULT Deliver(IMediaSample *pSample);
    virtual HRESULT DeliverEndOfStream(void);
    virtual HRESULT DeliverNewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate);
    #endif // DEBUG

    HRESULT DeliverBeginFlush(void);
    HRESULT DeliverEndFlush(void);

    HRESULT Inactive(void);
    HRESULT Active(void);
    virtual HRESULT CompleteConnect(IPin *pReceivePin);

    virtual HRESULT StartUsingOutputPin(void);
    virtual void StopUsingOutputPin(void);
    virtual bool StreamingThreadUsingOutputPin(void);

    HRESULT ChangeOutputFormat
        (
        const AM_MEDIA_TYPE *pmt,
        REFERENCE_TIME tSegmentStart,
        REFERENCE_TIME tSegmentStop,
        double dSegmentRate
        );
    HRESULT ChangeMediaType(const CMediaType *pmt);
    HRESULT DynamicReconnect(const CMediaType *pmt);

protected:
    HRESULT SynchronousBlockOutputPin(void);
    HRESULT AsynchronousBlockOutputPin(HANDLE hNotifyCallerPinBlockedEvent);
    HRESULT UnblockOutputPin(void);

    void BlockOutputPin(void);
    void ResetBlockState(void);

    static HRESULT WaitEvent(HANDLE hEvent);

    enum BLOCK_STATE
    {
        NOT_BLOCKED,
        PENDING,
        BLOCKED
    };

    // This lock should be held when the following class members are
    // being used: m_hNotifyCallerPinBlockedEvent, m_BlockState,
    // m_dwBlockCallerThreadID and m_dwNumOutstandingOutputPinUsers.
    CCritSec m_BlockStateLock;

    // This event should be signaled when the output pin is
    // not blocked.  This is a manual reset event.  For more
    // information on events, see the documentation for
    // CreateEvent() in the Windows SDK.
    HANDLE m_hUnblockOutputPinEvent;

    // This event will be signaled when block operation succeedes or
    // when the user cancels the block operation.  The block operation
    // can be canceled by calling IPinFlowControl2::Block( 0, NULL )
    // while the block operation is pending.
    HANDLE m_hNotifyCallerPinBlockedEvent;

    // The state of the current block operation.
    BLOCK_STATE m_BlockState;

    // The ID of the thread which last called IPinFlowControl::Block().
    // For more information on thread IDs, see the documentation for
    // GetCurrentThreadID() in the Windows SDK.
    DWORD m_dwBlockCallerThreadID;

    // The number of times StartUsingOutputPin() has been sucessfully
    // called and a corresponding call to StopUsingOutputPin() has not
    // been made.  When this variable is greater than 0, the streaming
    // thread is calling IPin::NewSegment(), IPin::EndOfStream(),
    // IMemInputPin::Receive() or IMemInputPin::ReceiveMultiple().  The
    // streaming thread could also be calling: DynamicReconnect(),
    // ChangeMediaType() or ChangeOutputFormat().  The output pin cannot
    // be blocked while the output pin is being used.
    DWORD m_dwNumOutstandingOutputPinUsers;

    // This event should be set when the IMediaFilter::Stop() is called.
    // This is a manual reset event.  It is also set when the output pin
    // delivers a flush to the connected input pin.
    HANDLE m_hStopEvent;
    IGraphConfig* m_pGraphConfig;

    // TRUE if the output pin's allocator's samples are read only.
    // Otherwise FALSE.  For more information, see the documentation
    // for IMemInputPin::NotifyAllocator().
    BOOL m_bPinUsesReadOnlyAllocator;

private:
    HRESULT Initialize(void);
    HRESULT ChangeMediaTypeHelper(const CMediaType *pmt);

    #ifdef DEBUG
    void AssertValid(void);
    #endif // DEBUG
};

class CAutoUsingOutputPin
{
public:
    CAutoUsingOutputPin( __in CDynamicOutputPin* pOutputPin, __inout HRESULT* phr );
    ~CAutoUsingOutputPin();

private:
    CDynamicOutputPin* m_pOutputPin;
};

inline CAutoUsingOutputPin::CAutoUsingOutputPin( __in CDynamicOutputPin* pOutputPin, __inout HRESULT* phr ) :
    m_pOutputPin(NULL)
{
    // The caller should always pass in valid pointers.
    ASSERT( NULL != pOutputPin );
    ASSERT( NULL != phr );

    // Make sure the user initialized phr.
    ASSERT( S_OK == *phr );

    HRESULT hr = pOutputPin->StartUsingOutputPin();
    if( FAILED( hr ) )
    {
        *phr = hr;
        return;
    }

    m_pOutputPin = pOutputPin;
}

inline CAutoUsingOutputPin::~CAutoUsingOutputPin()
{
    if( NULL != m_pOutputPin )
    {
        m_pOutputPin->StopUsingOutputPin();
    }
}

#ifdef DEBUG

inline HRESULT CDynamicOutputPin::Deliver(IMediaSample *pSample)
{
    // The caller should call StartUsingOutputPin() before calling this
    // method.
    ASSERT(StreamingThreadUsingOutputPin());

    return CBaseOutputPin::Deliver(pSample);
}

inline HRESULT CDynamicOutputPin::DeliverEndOfStream(void)
{
    // The caller should call StartUsingOutputPin() before calling this
    // method.
    ASSERT( StreamingThreadUsingOutputPin() );

    return CBaseOutputPin::DeliverEndOfStream();
}

inline HRESULT CDynamicOutputPin::DeliverNewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate)
{
    // The caller should call StartUsingOutputPin() before calling this
    // method.
    ASSERT(StreamingThreadUsingOutputPin());

    return CBaseOutputPin::DeliverNewSegment(tStart, tStop, dRate);
}

#endif // DEBUG

//=====================================================================
//=====================================================================
// Memory allocators
//
// the shared memory transport between pins requires the input pin
// to provide a memory allocator that can provide sample objects. A
// sample object supports the IMediaSample interface.
//
// CBaseAllocator handles the management of free and busy samples. It
// allocates CMediaSample objects. CBaseAllocator is an abstract class:
// in particular it has no method of initializing the list of free
// samples. CMemAllocator is derived from CBaseAllocator and initializes
// the list of samples using memory from the standard IMalloc interface.
//
// If you want your buffers to live in some special area of memory,
// derive your allocator object from CBaseAllocator. If you derive your
// IMemInputPin interface object from CBaseMemInputPin, you will get
// CMemAllocator-based allocation etc for free and will just need to
// supply the Receive handling, and media type / format negotiation.
//=====================================================================
//=====================================================================


//=====================================================================
//=====================================================================
// Defines CMediaSample
//
// an object of this class supports IMediaSample and represents a buffer
// for media data with some associated properties. Releasing it returns
// it to a freelist managed by a CBaseAllocator derived object.
//=====================================================================
//=====================================================================

class CMediaSample : public IMediaSample2    // The interface we support
{

protected:

    friend class CBaseAllocator;

    /*  Values for dwFlags - these are used for backward compatiblity
        only now - use AM_SAMPLE_xxx
    */
    enum { Sample_SyncPoint       = 0x01,   /* Is this a sync point */
           Sample_Preroll         = 0x02,   /* Is this a preroll sample */
           Sample_Discontinuity   = 0x04,   /* Set if start of new segment */
           Sample_TypeChanged     = 0x08,   /* Has the type changed */
           Sample_TimeValid       = 0x10,   /* Set if time is valid */
           Sample_MediaTimeValid  = 0x20,   /* Is the media time valid */
           Sample_TimeDiscontinuity = 0x40, /* Time discontinuity */
           Sample_StopValid       = 0x100,  /* Stop time valid */
           Sample_ValidFlags      = 0x1FF
         };

    /* Properties, the media sample class can be a container for a format
       change in which case we take a copy of a type through the SetMediaType
       interface function and then return it when GetMediaType is called. As
       we do no internal processing on it we leave it as a pointer */

    DWORD            m_dwFlags;         /* Flags for this sample */
                                        /* Type specific flags are packed
                                           into the top word
                                        */
    DWORD            m_dwTypeSpecificFlags; /* Media type specific flags */
    __field_ecount_opt(m_cbBuffer) LPBYTE           m_pBuffer;         /* Pointer to the complete buffer */
    LONG             m_lActual;         /* Length of data in this sample */
    LONG             m_cbBuffer;        /* Size of the buffer */
    CBaseAllocator  *m_pAllocator;      /* The allocator who owns us */
    CMediaSample     *m_pNext;          /* Chaining in free list */
    REFERENCE_TIME   m_Start;           /* Start sample time */
    REFERENCE_TIME   m_End;             /* End sample time */
    LONGLONG         m_MediaStart;      /* Real media start position */
    LONG             m_MediaEnd;        /* A difference to get the end */
    AM_MEDIA_TYPE    *m_pMediaType;     /* Media type change data */
    DWORD            m_dwStreamId;      /* Stream id */
public:
    LONG             m_cRef;            /* Reference count */


public:

    CMediaSample(
        __in_opt LPCTSTR pName,
        __in_opt CBaseAllocator *pAllocator,
        __inout_opt HRESULT *phr,
        __in_bcount_opt(length) LPBYTE pBuffer = NULL,
        LONG length = 0);
#ifdef UNICODE
    CMediaSample(
        __in_opt LPCSTR pName,
        __in_opt CBaseAllocator *pAllocator,
        __inout_opt HRESULT *phr,
        __in_bcount_opt(length) LPBYTE pBuffer = NULL,
        LONG length = 0);
#endif

    virtual ~CMediaSample();

    /* Note the media sample does not delegate to its owner */

    STDMETHODIMP QueryInterface(REFIID riid, __deref_out void **ppv);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();

    // set the buffer pointer and length. Used by allocators that
    // want variable sized pointers or pointers into already-read data.
    // This is only available through a CMediaSample* not an IMediaSample*
    // and so cannot be changed by clients.
    HRESULT SetPointer(__in_bcount(cBytes) BYTE * ptr, LONG cBytes);

    // Get me a read/write pointer to this buffer's memory.
    STDMETHODIMP GetPointer(__deref_out BYTE ** ppBuffer);

    STDMETHODIMP_(LONG) GetSize(void);

    // get the stream time at which this sample should start and finish.
    STDMETHODIMP GetTime(
        __out REFERENCE_TIME * pTimeStart,     // put time here
        __out REFERENCE_TIME * pTimeEnd
    );

    // Set the stream time at which this sample should start and finish.
    STDMETHODIMP SetTime(
        __in_opt REFERENCE_TIME * pTimeStart,     // put time here
        __in_opt REFERENCE_TIME * pTimeEnd
    );
    STDMETHODIMP IsSyncPoint(void);
    STDMETHODIMP SetSyncPoint(BOOL bIsSyncPoint);
    STDMETHODIMP IsPreroll(void);
    STDMETHODIMP SetPreroll(BOOL bIsPreroll);

    STDMETHODIMP_(LONG) GetActualDataLength(void);
    STDMETHODIMP SetActualDataLength(LONG lActual);

    // these allow for limited format changes in band

    STDMETHODIMP GetMediaType(__deref_out AM_MEDIA_TYPE **ppMediaType);
    STDMETHODIMP SetMediaType(__in_opt AM_MEDIA_TYPE *pMediaType);

    // returns S_OK if there is a discontinuity in the data (this same is
    // not a continuation of the previous stream of data
    // - there has been a seek).
    STDMETHODIMP IsDiscontinuity(void);
    // set the discontinuity property - TRUE if this sample is not a
    // continuation, but a new sample after a seek.
    STDMETHODIMP SetDiscontinuity(BOOL bDiscontinuity);

    // get the media times for this sample
    STDMETHODIMP GetMediaTime(
    	__out LONGLONG * pTimeStart,
	    __out LONGLONG * pTimeEnd
    );

    // Set the media times for this sample
    STDMETHODIMP SetMediaTime(
    	__in_opt LONGLONG * pTimeStart,
	    __in_opt LONGLONG * pTimeEnd
    );

    // Set and get properties (IMediaSample2)
    STDMETHODIMP GetProperties(
        DWORD cbProperties,
        __out_bcount(cbProperties) BYTE * pbProperties
    );

    STDMETHODIMP SetProperties(
        DWORD cbProperties,
        __in_bcount(cbProperties) const BYTE * pbProperties
    );
};


//=====================================================================
//=====================================================================
// Defines CBaseAllocator
//
// Abstract base class that manages a list of media samples
//
// This class provides support for getting buffers from the free list,
// including handling of commit and (asynchronous) decommit.
//
// Derive from this class and override the Alloc and Free functions to
// allocate your CMediaSample (or derived) objects and add them to the
// free list, preparing them as necessary.
//=====================================================================
//=====================================================================

class AM_NOVTABLE CBaseAllocator : public CUnknown,// A non delegating IUnknown
                       public IMemAllocatorCallbackTemp, // The interface we support
                       public CCritSec             // Provides object locking
{
    class CSampleList;
    friend class CSampleList;

    /*  Trick to get at protected member in CMediaSample */
    static CMediaSample * &NextSample(__in CMediaSample *pSample)
    {
        return pSample->m_pNext;
    };

    /*  Mini list class for the free list */
    class CSampleList
    {
    public:
        CSampleList() : m_List(NULL), m_nOnList(0) {};
#ifdef DEBUG
        ~CSampleList()
        {
            ASSERT(m_nOnList == 0);
        };
#endif
        CMediaSample *Head() const { return m_List; };
        CMediaSample *Next(__in CMediaSample *pSample) const { return CBaseAllocator::NextSample(pSample); };
        int GetCount() const { return m_nOnList; };
        void Add(__inout CMediaSample *pSample)
        {
            ASSERT(pSample != NULL);
            CBaseAllocator::NextSample(pSample) = m_List;
            m_List = pSample;
            m_nOnList++;
        };
        CMediaSample *RemoveHead()
        {
            CMediaSample *pSample = m_List;
            if (pSample != NULL) {
                m_List = CBaseAllocator::NextSample(m_List);
                m_nOnList--;
            }
            return pSample;
        };
        void Remove(__inout CMediaSample *pSample);

    public:
        CMediaSample *m_List;
        int           m_nOnList;
    };
protected:

    CSampleList m_lFree;        // Free list

    /*  Note to overriders of CBaseAllocator.

        We use a lazy signalling mechanism for waiting for samples.
        This means we don't call the OS if no waits occur.

        In order to implement this:

        1. When a new sample is added to m_lFree call NotifySample() which
           calls ReleaseSemaphore on m_hSem with a count of m_lWaiting and
           sets m_lWaiting to 0.
           This must all be done holding the allocator's critical section.

        2. When waiting for a sample call SetWaiting() which increments
           m_lWaiting BEFORE leaving the allocator's critical section.

        3. Actually wait by calling WaitForSingleObject(m_hSem, INFINITE)
           having left the allocator's critical section.  The effect of
           this is to remove 1 from the semaphore's count.  You MUST call
           this once having incremented m_lWaiting.

        The following are then true when the critical section is not held :
            (let nWaiting = number about to wait or waiting)

            (1) if (m_lFree.GetCount() != 0) then (m_lWaiting == 0)
            (2) m_lWaiting + Semaphore count == nWaiting

        We would deadlock if
           nWaiting != 0 &&
           m_lFree.GetCount() != 0 &&
           Semaphore count == 0

           But from (1) if m_lFree.GetCount() != 0 then m_lWaiting == 0 so
           from (2) Semaphore count == nWaiting (which is non-0) so the
           deadlock can't happen.
    */

    HANDLE m_hSem;              // For signalling
    long m_lWaiting;            // Waiting for a free element
    long m_lCount;              // how many buffers we have agreed to provide
    long m_lAllocated;          // how many buffers are currently allocated
    long m_lSize;               // agreed size of each buffer
    long m_lAlignment;          // agreed alignment
    long m_lPrefix;             // agreed prefix (preceeds GetPointer() value)
    BOOL m_bChanged;            // Have the buffer requirements changed

    // if true, we are decommitted and can't allocate memory
    BOOL m_bCommitted;
    // if true, the decommit has happened, but we haven't called Free yet
    // as there are still outstanding buffers
    BOOL m_bDecommitInProgress;

    //  Notification interface
    IMemAllocatorNotifyCallbackTemp *m_pNotify;

    BOOL m_fEnableReleaseCallback;

    // called to decommit the memory when the last buffer is freed
    // pure virtual - need to override this
    virtual void Free(void) PURE;

    // override to allocate the memory when commit called
    virtual HRESULT Alloc(void);

public:

    CBaseAllocator(
        __in_opt LPCTSTR , __inout_opt LPUNKNOWN, __inout HRESULT *,
        BOOL bEvent = TRUE, BOOL fEnableReleaseCallback = FALSE);
#ifdef UNICODE
    CBaseAllocator(
        __in_opt LPCSTR , __inout_opt LPUNKNOWN, __inout HRESULT *,
        BOOL bEvent = TRUE, BOOL fEnableReleaseCallback = FALSE);
#endif
    virtual ~CBaseAllocator();

    DECLARE_IUNKNOWN

    // override this to publicise our interfaces
    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, __deref_out void **ppv);

    STDMETHODIMP SetProperties(
		    __in ALLOCATOR_PROPERTIES* pRequest,
		    __out ALLOCATOR_PROPERTIES* pActual);

    // return the properties actually being used on this allocator
    STDMETHODIMP GetProperties(
		    __out ALLOCATOR_PROPERTIES* pProps);

    // override Commit to allocate memory. We handle the GetBuffer
    //state changes
    STDMETHODIMP Commit();

    // override this to handle the memory freeing. We handle any outstanding
    // GetBuffer calls
    STDMETHODIMP Decommit();

    // get container for a sample. Blocking, synchronous call to get the
    // next free buffer (as represented by an IMediaSample interface).
    // on return, the time etc properties will be invalid, but the buffer
    // pointer and size will be correct. The two time parameters are
    // optional and either may be NULL, they may alternatively be set to
    // the start and end times the sample will have attached to it
    // bPrevFramesSkipped is not used (used only by the video renderer's
    // allocator where it affects quality management in direct draw).

    STDMETHODIMP GetBuffer(__deref_out IMediaSample **ppBuffer,
                           __in_opt REFERENCE_TIME * pStartTime,
                           __in_opt REFERENCE_TIME * pEndTime,
                           DWORD dwFlags);

    // final release of a CMediaSample will call this
    STDMETHODIMP ReleaseBuffer(IMediaSample *pBuffer);
    // obsolete:: virtual void PutOnFreeList(CMediaSample * pSample);

    STDMETHODIMP SetNotify(IMemAllocatorNotifyCallbackTemp *pNotify);

    STDMETHODIMP GetFreeCount(__out LONG *plBuffersFree);

    // Notify that a sample is available
    void NotifySample();

    // Notify that we're waiting for a sample
    void SetWaiting() { m_lWaiting++; };
};


//=====================================================================
//=====================================================================
// Defines CMemAllocator
//
// this is an allocator based on CBaseAllocator that allocates sample
// buffers in main memory (from 'new'). You must call SetProperties
// before calling Commit.
//
// we don't free the memory when going into Decommit state. The simplest
// way to implement this without complicating CBaseAllocator is to
// have a Free() function, called to go into decommit state, that does
// nothing and a ReallyFree function called from our destructor that
// actually frees the memory.
//=====================================================================
//=====================================================================

//  Make me one from quartz.dll
STDAPI CreateMemoryAllocator(__deref_out IMemAllocator **ppAllocator);

class CMemAllocator : public CBaseAllocator
{

protected:

    LPBYTE m_pBuffer;   // combined memory for all buffers

    // override to free the memory when decommit completes
    // - we actually do nothing, and save the memory until deletion.
    void Free(void);

    // called from the destructor (and from Alloc if changing size/count) to
    // actually free up the memory
    void ReallyFree(void);

    // overriden to allocate the memory when commit called
    HRESULT Alloc(void);

public:
    /* This goes in the factory template table to create new instances */
    static CUnknown *CreateInstance(__inout_opt LPUNKNOWN, __inout HRESULT *);

    STDMETHODIMP SetProperties(
		    __in ALLOCATOR_PROPERTIES* pRequest,
		    __out ALLOCATOR_PROPERTIES* pActual);

    CMemAllocator(__in_opt LPCTSTR , __inout_opt LPUNKNOWN, __inout HRESULT *);
#ifdef UNICODE
    CMemAllocator(__in_opt LPCSTR , __inout_opt LPUNKNOWN, __inout HRESULT *);
#endif
    ~CMemAllocator();
};

// helper used by IAMovieSetup implementation
STDAPI
AMovieSetupRegisterFilter( const AMOVIESETUP_FILTER * const psetupdata
                         , IFilterMapper *                  pIFM
                         , BOOL                             bRegister  );


///////////////////////////////////////////////////////////////////////////
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////

#endif /* __FILTER__ */

//------------------------------------------------------------------------------
// File: Source.h
//
// Desc: DirectShow base classes - defines classes to simplify creation of
//       ActiveX source filters that support continuous generation of data.
//       No support is provided for IMediaControl or IMediaPosition.
//
// Copyright (c) 1992-2001 Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------


//
// Derive your source filter from CSource.
// During construction either:
//    Create some CSourceStream objects to manage your pins
//    Provide the user with a means of doing so eg, an IPersistFile interface.
//
// CSource provides:
//    IBaseFilter interface management
//    IMediaFilter interface management, via CBaseFilter
//    Pin counting for CBaseFilter
//
// Derive a class from CSourceStream to manage your output pin types
//  Implement GetMediaType/1 to return the type you support. If you support multiple
//   types then overide GetMediaType/3, CheckMediaType and GetMediaTypeCount.
//  Implement Fillbuffer() to put data into one buffer.
//
// CSourceStream provides:
//    IPin management via CBaseOutputPin
//    Worker thread management

#ifndef __CSOURCE__
#define __CSOURCE__

class CSourceStream;  // The class that will handle each pin


//
// CSource
//
// Override construction to provide a means of creating
// CSourceStream derived objects - ie a way of creating pins.
class CSource : public CBaseFilter {
public:

    CSource(__in_opt LPCTSTR pName, __inout_opt LPUNKNOWN lpunk, CLSID clsid, __inout HRESULT *phr);
    CSource(__in_opt LPCTSTR pName, __inout_opt LPUNKNOWN lpunk, CLSID clsid);
#ifdef UNICODE
    CSource(__in_opt LPCSTR pName, __inout_opt LPUNKNOWN lpunk, CLSID clsid, __inout HRESULT *phr);
    CSource(__in_opt LPCSTR pName, __inout_opt LPUNKNOWN lpunk, CLSID clsid);
#endif
    ~CSource();

    int       GetPinCount(void);
    CBasePin *GetPin(int n);

    // -- Utilities --

    CCritSec*	pStateLock(void) { return &m_cStateLock; }	// provide our critical section

    HRESULT     AddPin(__in CSourceStream *);
    HRESULT     RemovePin(__in CSourceStream *);

    STDMETHODIMP FindPin(
        LPCWSTR Id,
        __deref_out IPin ** ppPin
    );

    int FindPinNumber(__in IPin *iPin);
    
protected:

    int             m_iPins;       // The number of pins on this filter. Updated by CSourceStream
    	   			   // constructors & destructors.
    CSourceStream **m_paStreams;   // the pins on this filter.

    CCritSec m_cStateLock;	// Lock this to serialize function accesses to the filter state

};


//
// CSourceStream
//
// Use this class to manage a stream of data that comes from a
// pin.
// Uses a worker thread to put data on the pin.
class CSourceStream : public CAMThread, public CBaseOutputPin {
public:

    CSourceStream(__in_opt LPCTSTR pObjectName,
                  __inout HRESULT *phr,
                  __inout CSource *pms,
                  __in_opt LPCWSTR pName);
#ifdef UNICODE
    CSourceStream(__in_opt LPCSTR pObjectName,
                  __inout HRESULT *phr,
                  __inout CSource *pms,
                  __in_opt LPCWSTR pName);
#endif
    virtual ~CSourceStream(void);  // virtual destructor ensures derived class destructors are called too.

protected:

    CSource *m_pFilter;	// The parent of this stream

    // *
    // * Data Source
    // *
    // * The following three functions: FillBuffer, OnThreadCreate/Destroy, are
    // * called from within the ThreadProc. They are used in the creation of
    // * the media samples this pin will provide
    // *

    // Override this to provide the worker thread a means
    // of processing a buffer
    virtual HRESULT FillBuffer(IMediaSample *pSamp) PURE;

    // Called as the thread is created/destroyed - use to perform
    // jobs such as start/stop streaming mode
    // If OnThreadCreate returns an error the thread will exit.
    virtual HRESULT OnThreadCreate(void) {return NOERROR;};
    virtual HRESULT OnThreadDestroy(void) {return NOERROR;};
    virtual HRESULT OnThreadStartPlay(void) {return NOERROR;};

    // *
    // * Worker Thread
    // *

    HRESULT Active(void);    // Starts up the worker thread
    HRESULT Inactive(void);  // Exits the worker thread.

public:
    // thread commands
    enum Command {CMD_INIT, CMD_PAUSE, CMD_RUN, CMD_STOP, CMD_EXIT};
    HRESULT Init(void) { return CallWorker(CMD_INIT); }
    HRESULT Exit(void) { return CallWorker(CMD_EXIT); }
    HRESULT Run(void) { return CallWorker(CMD_RUN); }
    HRESULT Pause(void) { return CallWorker(CMD_PAUSE); }
    HRESULT Stop(void) { return CallWorker(CMD_STOP); }

protected:
    Command GetRequest(void) { return (Command) CAMThread::GetRequest(); }
    BOOL    CheckRequest(Command *pCom) { return CAMThread::CheckRequest( (DWORD *) pCom); }

    // override these if you want to add thread commands
    virtual DWORD ThreadProc(void);  		// the thread function

    virtual HRESULT DoBufferProcessingLoop(void);    // the loop executed whilst running


    // *
    // * AM_MEDIA_TYPE support
    // *

    // If you support more than one media type then override these 2 functions
    virtual HRESULT CheckMediaType(const CMediaType *pMediaType);
    virtual HRESULT GetMediaType(int iPosition, __inout CMediaType *pMediaType);  // List pos. 0-n

    // If you support only one type then override this fn.
    // This will only be called by the default implementations
    // of CheckMediaType and GetMediaType(int, CMediaType*)
    // You must override this fn. or the above 2!
    virtual HRESULT GetMediaType(__inout CMediaType *pMediaType) {return E_UNEXPECTED;}

    STDMETHODIMP QueryId(
        __deref_out LPWSTR * Id
    );
};

#endif // __CSOURCE__

//------------------------------------------------------------------------------
// File: CProp.h
//
// Desc: DirectShow base classes.
//
// Copyright (c) 1992-2001 Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------

#ifndef __CPROP__
#define __CPROP__

// Base property page class. Filters typically expose custom properties by
// implementing special control interfaces, examples are IDirectDrawVideo
// and IQualProp on renderers. This allows property pages to be built that
// use the given interface. Applications such as the ActiveMovie OCX query
// filters for the property pages they support and expose them to the user
//
// This class provides all the framework for a property page. A property
// page is a COM object that supports IPropertyPage. We should be created
// with a resource ID for the dialog which we will load when required. We
// should also be given in the constructor a resource ID for a title string
// we will load from the DLLs STRINGTABLE. The property page titles must be
// stored in resource files so that they can be easily internationalised
//
// We have a number of virtual methods (not PURE) that may be overriden in
// derived classes to query for interfaces and so on. These functions have
// simple implementations here that just return NOERROR. Derived classes
// will almost definately have to override the message handler method called
// OnReceiveMessage. We have a static dialog procedure that calls the method
// so that derived classes don't have to fiddle around with the this pointer

class AM_NOVTABLE CBasePropertyPage : public IPropertyPage, public CUnknown
{
protected:

    LPPROPERTYPAGESITE m_pPageSite;       // Details for our property site
    HWND m_hwnd;                          // Window handle for the page
    HWND m_Dlg;                           // Actual dialog window handle
    BOOL m_bDirty;                        // Has anything been changed
    int m_TitleId;                        // Resource identifier for title
    int m_DialogId;                       // Dialog resource identifier

    static INT_PTR CALLBACK DialogProc(HWND hwnd,
                                       UINT uMsg,
                                       WPARAM wParam,
                                       LPARAM lParam);

private:
    BOOL m_bObjectSet ;                  // SetObject has been called or not.
public:

    CBasePropertyPage(__in_opt LPCTSTR pName,      // Debug only name
                      __inout_opt LPUNKNOWN pUnk, // COM Delegator
                      int DialogId,               // Resource ID
                      int TitleId);               // To get tital

#ifdef UNICODE
    CBasePropertyPage(__in_opt LPCSTR pName,
                      __inout_opt LPUNKNOWN pUnk,
                      int DialogId,  
                      int TitleId);
#endif
    virtual ~CBasePropertyPage() { };
    DECLARE_IUNKNOWN

    // Override these virtual methods

    virtual HRESULT OnConnect(IUnknown *pUnknown) { return NOERROR; };
    virtual HRESULT OnDisconnect() { return NOERROR; };
    virtual HRESULT OnActivate() { return NOERROR; };
    virtual HRESULT OnDeactivate() { return NOERROR; };
    virtual HRESULT OnApplyChanges() { return NOERROR; };
    virtual INT_PTR OnReceiveMessage(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);

    // These implement an IPropertyPage interface

    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, __deref_out void **ppv);
    STDMETHODIMP_(ULONG) NonDelegatingRelease();
    STDMETHODIMP_(ULONG) NonDelegatingAddRef();
    STDMETHODIMP SetPageSite(__in_opt LPPROPERTYPAGESITE pPageSite);
    STDMETHODIMP Activate(HWND hwndParent, LPCRECT prect,BOOL fModal);
    STDMETHODIMP Deactivate(void);
    STDMETHODIMP GetPageInfo(__out LPPROPPAGEINFO pPageInfo);
    STDMETHODIMP SetObjects(ULONG cObjects, __in_ecount_opt(cObjects) LPUNKNOWN *ppUnk);
    STDMETHODIMP Show(UINT nCmdShow);
    STDMETHODIMP Move(LPCRECT prect);
    STDMETHODIMP IsPageDirty(void) { return m_bDirty ? S_OK : S_FALSE; }
    STDMETHODIMP Apply(void);
    STDMETHODIMP Help(LPCWSTR lpszHelpDir) { return E_NOTIMPL; }
    STDMETHODIMP TranslateAccelerator(__inout LPMSG lpMsg) { return E_NOTIMPL; }
};

#endif // __CPROP__
