#include "streams.h"
#include <mmreg.h>
#include <dvdmedia.h>
#include <tchar.h>
#include <strsafe.h>

#define STREAMS_PROVIDE_CUSTOM_FACTORY

#pragma comment(lib, "WinMM.lib")

#include <initguid.h>
DEFINE_GUID(IID_IPin,0x56A86891,0xAD4,0x11CE,0xB0,0x3A,0x0,0x20,0xAF,0xB,0xA7,0x70);
DEFINE_GUID(IID_IEnumPins,0x56A86892,0xAD4,0x11CE,0xB0,0x3A,0x0,0x20,0xAF,0xB,0xA7,0x70);
DEFINE_GUID(IID_IEnumMediaTypes,0x89C31040,0x846B,0x11CE,0x97,0xD3,0x0,0xAA,0x0,0x55,0x59,0x5A);
DEFINE_GUID(IID_IMediaFilter,0x56A86899,0xAD4,0x11CE,0xB0,0x3A,0x0,0x20,0xAF,0xB,0xA7,0x70);
DEFINE_GUID(IID_IBaseFilter,0x56A86895,0xAD4,0x11CE,0xB0,0x3A,0x0,0x20,0xAF,0xB,0xA7,0x70);
DEFINE_GUID(IID_IMediaSample,0x56A8689A,0xAD4,0x11CE,0xB0,0x3A,0x0,0x20,0xAF,0xB,0xA7,0x70);
DEFINE_GUID(IID_IMediaSample2,0x36B73884,0xC2C8,0x11CF,0x8B,0x46,0x0,0x80,0x5F,0x6C,0xEF,0x60);
DEFINE_GUID(IID_IMemAllocator,0x56A8689C,0xAD4,0x11CE,0xB0,0x3A,0x0,0x20,0xAF,0xB,0xA7,0x70);
DEFINE_GUID(IID_IMemAllocatorCallbackTemp,0x379A0CF0,0xC1DE,0x11D2,0xAB,0xF5,0x0,0xA0,0xC9,0x5,0xF3,0x75);
DEFINE_GUID(IID_IMemInputPin,0x56A8689D,0xAD4,0x11CE,0xB0,0x3A,0x0,0x20,0xAF,0xB,0xA7,0x70);
DEFINE_GUID(IID_IAMovieSetup,0xA3D8CEC0,0x7E5A,0x11CF,0xBB,0xC5,0x0,0x80,0x5F,0x6C,0xEF,0x20);
DEFINE_GUID(IID_IFilterMapper,0x56A868A3,0xAD4,0x11CE,0xB0,0x3A,0x0,0x20,0xAF,0xB,0xA7,0x70);
DEFINE_GUID(IID_IFilterMapper2,0xB79BB0B0,0x33C1,0x11D1,0xAB,0xE1,0x0,0xA0,0xC9,0x5,0xF3,0x75);
DEFINE_GUID(IID_IMediaEventSink,0x56A868A2,0xAD4,0x11CE,0xB0,0x3A,0x0,0x20,0xAF,0xB,0xA7,0x70);
DEFINE_GUID(IID_IFilterGraph2,0x36B73882,0xC2C8,0x11CF,0x8B,0x46,0x0,0x80,0x5F,0x6C,0xEF,0x60);
DEFINE_GUID(IID_ISeekingPassThru,0x36B73883,0xC2C8,0x11CF,0x8B,0x46,0x0,0x80,0x5F,0x6C,0xEF,0x60);
DEFINE_GUID(IID_IPinConnection,0x4A9A62D3,0x27D4,0x403D,0x91,0xE9,0x89,0xF5,0x40,0xE5,0x55,0x34);
DEFINE_GUID(IID_IPinFlowControl,0xC56E9858,0xDBF3,0x4F6B,0x81,0x19,0x38,0x4A,0xF2,0x6,0xD,0xEB);
DEFINE_GUID(IID_IAMStreamConfig,0xc6e13340,0x30ac,0x11d0,0xa1,0x8c,0x00,0xa0,0xc9,0x11,0x89,0x56);
DEFINE_GUID(IID_IKsPropertySet,0x31efac30,0x515c,0x11d0,0xa9,0xaa,0x0,0xaa,0x0,0x61,0xbe,0x93);

#ifndef NUMELMS
#if _WIN32_WINNT < 0x0600
   #define NUMELMS(aa) (sizeof(aa)/sizeof((aa)[0]))
#else
   #define NUMELMS(aa) ARRAYSIZE(aa)
#endif
#endif

#pragma warning(push)
#pragma warning(disable: 4312 4244)
// _GetWindowLongPtr
// Templated version of GetWindowLongPtr, to suppress spurious compiler warning.
template <class T>
T _GetWindowLongPtr(HWND hwnd, int nIndex)
{
    return (T)GetWindowLongPtr(hwnd, nIndex);
}

// _SetWindowLongPtr
// Templated version of SetWindowLongPtr, to suppress spurious compiler warning.
template <class T>
LONG_PTR _SetWindowLongPtr(HWND hwnd, int nIndex, T p)
{
    return SetWindowLongPtr(hwnd, nIndex, (LONG_PTR)p);
}
#pragma warning(pop)

#ifdef DEBUG
#ifdef UNICODE
#ifndef _UNICODE
#define _UNICODE
#endif // _UNICODE
#endif // UNICODE
#endif // DEBUG

//------------------------------------------------------------------------------
// File: FourCC.h
//
// Desc: DirectShow base classes.
//
// Copyright (c) 1992-2001 Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------

// FOURCCMap
//
// provides a mapping between old-style multimedia format DWORDs
// and new-style GUIDs.
//
// A range of 4 billion GUIDs has been allocated to ensure that this
// mapping can be done straightforwardly one-to-one in both directions.
//
// January 95

#ifndef __FOURCC__
#define __FOURCC__


// Multimedia format types are marked with DWORDs built from four 8-bit
// chars and known as FOURCCs. New multimedia AM_MEDIA_TYPE definitions include
// a subtype GUID. In order to simplify the mapping, GUIDs in the range:
//    XXXXXXXX-0000-0010-8000-00AA00389B71
// are reserved for FOURCCs.

class FOURCCMap : public GUID
{

public:
    FOURCCMap();
    FOURCCMap(DWORD Fourcc);
    FOURCCMap(const GUID *);


    DWORD GetFOURCC(void);
    void SetFOURCC(DWORD fourcc);
    void SetFOURCC(const GUID *);

private:
    void InitGUID();
};

#define GUID_Data2      0
#define GUID_Data3     0x10
#define GUID_Data4_1   0xaa000080
#define GUID_Data4_2   0x719b3800

inline void
FOURCCMap::InitGUID() {
    Data2 = GUID_Data2;
    Data3 = GUID_Data3;
    ((DWORD *)Data4)[0] = GUID_Data4_1;
    ((DWORD *)Data4)[1] = GUID_Data4_2;
}

inline
FOURCCMap::FOURCCMap() {
    InitGUID();
    SetFOURCC( DWORD(0));
}

inline
FOURCCMap::FOURCCMap(DWORD fourcc)
{
    InitGUID();
    SetFOURCC(fourcc);
}

inline
FOURCCMap::FOURCCMap(const GUID * pGuid)
{
    InitGUID();
    SetFOURCC(pGuid);
}

inline void
FOURCCMap::SetFOURCC(const GUID * pGuid)
{
    FOURCCMap * p = (FOURCCMap*) pGuid;
    SetFOURCC(p->GetFOURCC());
}

inline void
FOURCCMap::SetFOURCC(DWORD fourcc)
{
    Data1 = fourcc;
}

inline DWORD
FOURCCMap::GetFOURCC(void)
{
    return Data1;
}

#endif /* __FOURCC__ */

//------------------------------------------------------------------------------
// File: MsgThrd.h
//
// Desc: DirectShow base classes - provides support for a worker thread 
//       class to which one can asynchronously post messages.
//
// Copyright (c) 1992-2001 Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------

#ifndef __MSGTHRD__
#define __MSGTHRD__

// Message class - really just a structure.
//
class CMsg {
public:
    UINT uMsg;
    DWORD dwFlags;
    LPVOID lpParam;
    CAMEvent *pEvent;

    CMsg(UINT u, DWORD dw, __inout_opt LPVOID lp, __in_opt CAMEvent *pEvnt)
        : uMsg(u), dwFlags(dw), lpParam(lp), pEvent(pEvnt) {}

    CMsg()
        : uMsg(0), dwFlags(0L), lpParam(NULL), pEvent(NULL) {}
};

// This is the actual thread class.  It exports all the usual thread control
// functions.  The created thread is different from a normal WIN32 thread in
// that it is prompted to perform particaular tasks by responding to messages
// posted to its message queue.
//
class AM_NOVTABLE CMsgThread {
private:
    static DWORD WINAPI DefaultThreadProc(__inout LPVOID lpParam);
    DWORD               m_ThreadId;
    HANDLE              m_hThread;

protected:

    // if you want to override GetThreadMsg to block on other things
    // as well as this queue, you need access to this
    CGenericList<CMsg>        m_ThreadQueue;
    CCritSec                  m_Lock;
    HANDLE                    m_hSem;
    LONG                      m_lWaiting;

public:
    CMsgThread()
        : m_ThreadId(0),
        m_hThread(NULL),
        m_lWaiting(0),
        m_hSem(NULL),
        // make a list with a cache of 5 items
        m_ThreadQueue(NAME("MsgThread list"), 5)
        {
        }

    ~CMsgThread();
    // override this if you want to block on other things as well
    // as the message loop
    void virtual GetThreadMsg(__out CMsg *msg);

    // override this if you want to do something on thread startup
    virtual void OnThreadInit() {
    };

    BOOL CreateThread();

    BOOL WaitForThreadExit(__out LPDWORD lpdwExitCode) {
        if (m_hThread != NULL) {
            WaitForSingleObject(m_hThread, INFINITE);
            return GetExitCodeThread(m_hThread, lpdwExitCode);
        }
        return FALSE;
    }

    DWORD ResumeThread() {
        return ::ResumeThread(m_hThread);
    }

    DWORD SuspendThread() {
        return ::SuspendThread(m_hThread);
    }

    int GetThreadPriority() {
        return ::GetThreadPriority(m_hThread);
    }

    BOOL SetThreadPriority(int nPriority) {
        return ::SetThreadPriority(m_hThread, nPriority);
    }

    HANDLE GetThreadHandle() {
        return m_hThread;
    }

    DWORD GetThreadId() {
        return m_ThreadId;
    }


    void PutThreadMsg(UINT uMsg, DWORD dwMsgFlags,
                      __in_opt LPVOID lpMsgParam, __in_opt CAMEvent *pEvent = NULL) {
        CAutoLock lck(&m_Lock);
        CMsg* pMsg = new CMsg(uMsg, dwMsgFlags, lpMsgParam, pEvent);
        m_ThreadQueue.AddTail(pMsg);
        if (m_lWaiting != 0) {
            ReleaseSemaphore(m_hSem, m_lWaiting, 0);
            m_lWaiting = 0;
        }
    }

    // This is the function prototype of the function that the client
    // supplies.  It is always called on the created thread, never on
    // the creator thread.
    //
    virtual LRESULT ThreadMessageProc(
        UINT uMsg, DWORD dwFlags, __inout_opt LPVOID lpParam, __in_opt CAMEvent *pEvent) = 0;
};

#endif /* __MSGTHRD__ */

//------------------------------------------------------------------------------
// File: Schedule.h
//
// Desc: DirectShow base classes.
//
// Copyright (c) 1996-2001 Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------

#ifndef __CAMSchedule__
#define __CAMSchedule__

class CAMSchedule : private CBaseObject
{
public:
    virtual ~CAMSchedule();
    // ev is the event we should fire if the advise time needs re-evaluating
    CAMSchedule( HANDLE ev );

    DWORD GetAdviseCount();
    REFERENCE_TIME GetNextAdviseTime();

    // We need a method for derived classes to add advise packets, we return the cookie
    DWORD_PTR AddAdvisePacket( const REFERENCE_TIME & time1, const REFERENCE_TIME & time2, HANDLE h, BOOL periodic );
    // And a way to cancel
    HRESULT Unadvise(DWORD_PTR dwAdviseCookie);

    // Tell us the time please, and we'll dispatch the expired events.  We return the time of the next event.
    // NB: The time returned will be "useless" if you start adding extra Advises.  But that's the problem of
    // whoever is using this helper class (typically a clock).
    REFERENCE_TIME Advise( const REFERENCE_TIME & rtTime );

    // Get the event handle which will be set if advise time requires re-evaluation.
    HANDLE GetEvent() const { return m_ev; }

private:
    // We define the nodes that will be used in our singly linked list
    // of advise packets.  The list is ordered by time, with the
    // elements that will expire first at the front.
    class CAdvisePacket
    {
    public:
        CAdvisePacket()
        {}

        CAdvisePacket * m_next;
        DWORD_PTR       m_dwAdviseCookie;
        REFERENCE_TIME  m_rtEventTime;      // Time at which event should be set
        REFERENCE_TIME  m_rtPeriod;         // Periodic time
        HANDLE          m_hNotify;          // Handle to event or semephore
        BOOL            m_bPeriodic;        // TRUE => Periodic event

        CAdvisePacket( __inout_opt CAdvisePacket * next, LONGLONG time ) : m_next(next), m_rtEventTime(time)
        {}

        void InsertAfter( __inout CAdvisePacket * p )
        {
            p->m_next = m_next;
            m_next    = p;
        }

        int IsZ() const // That is, is it the node that represents the end of the list
        { return m_next == 0; }

        CAdvisePacket * RemoveNext()
        {
            CAdvisePacket *const next = m_next;
            CAdvisePacket *const new_next = next->m_next;
            m_next = new_next;
            return next;
        }

        void DeleteNext()
        {
            delete RemoveNext();
        }

        CAdvisePacket * Next() const
        {
            CAdvisePacket * result = m_next;
            if (result->IsZ()) result = 0;
            return result;
        }

        DWORD_PTR Cookie() const
        { return m_dwAdviseCookie; }
    };

    // Structure is:
    // head -> elmt1 -> elmt2 -> z -> null
    // So an empty list is:       head -> z -> null
    // Having head & z as links makes insertaion,
    // deletion and shunting much easier.
    CAdvisePacket   head, z;            // z is both a tail and a sentry

    volatile DWORD_PTR  m_dwNextCookie;     // Strictly increasing
    volatile DWORD  m_dwAdviseCount;    // Number of elements on list

    CCritSec        m_Serialize;

    // AddAdvisePacket: adds the packet, returns the cookie (0 if failed)
    DWORD_PTR AddAdvisePacket( __inout CAdvisePacket * pPacket );
    // Event that we should set if the packed added above will be the next to fire.
    const HANDLE m_ev;

    // A Shunt is where we have changed the first element in the
    // list and want it re-evaluating (i.e. repositioned) in
    // the list.
    void ShuntHead();

    // Rather than delete advise packets, we cache them for future use
    CAdvisePacket * m_pAdviseCache;
    DWORD           m_dwCacheCount;
    enum { dwCacheMax = 5 };             // Don't bother caching more than five

    void Delete( __inout CAdvisePacket * pLink );// This "Delete" will cache the Link

// Attributes and methods for debugging
public:
#ifdef DEBUG
    void DumpLinkedList();
#else
    void DumpLinkedList() {}
#endif

};

#endif // __CAMSchedule__

//------------------------------------------------------------------------------
// File: RefClock.h
//
// Desc: DirectShow base classes - defines the IReferenceClock interface.
//
// Copyright (c) 1992-2001 Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------

#ifndef __BASEREFCLOCK__
#define __BASEREFCLOCK__

const UINT RESOLUTION = 1;                      /* High resolution timer */
const INT ADVISE_CACHE = 4;                     /* Default cache size */
const LONGLONG MAX_TIME = 0x7FFFFFFFFFFFFFFF;   /* Maximum LONGLONG value */

inline LONGLONG WINAPI ConvertToMilliseconds(const REFERENCE_TIME& RT)
{
    /* This converts an arbitrary value representing a reference time
       into a MILLISECONDS value for use in subsequent system calls */

    return (RT / (UNITS / MILLISECONDS));
}

/* This class hierarchy will support an IReferenceClock interface so
   that an audio card (or other externally driven clock) can update the
   system wide clock that everyone uses.

   The interface will be pretty thin with probably just one update method
   This interface has not yet been defined.
 */

/* This abstract base class implements the IReferenceClock
 * interface.  Classes that actually provide clock signals (from
 * whatever source) have to be derived from this class.
 *
 * The abstract class provides implementations for:
 *  CUnknown support
 *      locking support (CCritSec)
 *  client advise code (creates a thread)
 *
 * Question: what can we do about quality?  Change the timer
 * resolution to lower the system load?  Up the priority of the
 * timer thread to force more responsive signals?
 *
 * During class construction we create a worker thread that is destroyed during
 * destuction.  This thread executes a series of WaitForSingleObject calls,
 * waking up when a command is given to the thread or the next wake up point
 * is reached.  The wakeup points are determined by clients making Advise
 * calls.
 *
 * Each advise call defines a point in time when they wish to be notified.  A
 * periodic advise is a series of these such events.  We maintain a list of
 * advise links and calculate when the nearest event notification is due for.
 * We then call WaitForSingleObject with a timeout equal to this time.  The
 * handle we wait on is used by the class to signal that something has changed
 * and that we must reschedule the next event.  This typically happens when
 * someone comes in and asks for an advise link while we are waiting for an
 * event to timeout.
 *
 * While we are modifying the list of advise requests we
 * are protected from interference through a critical section.  Clients are NOT
 * advised through callbacks.  One shot clients have an event set, while
 * periodic clients have a semaphore released for each event notification.  A
 * semaphore allows a client to be kept up to date with the number of events
 * actually triggered and be assured that they can't miss multiple events being
 * set.
 *
 * Keeping track of advises is taken care of by the CAMSchedule class.
 */

class CBaseReferenceClock
: public CUnknown, public IReferenceClock, public CCritSec, public IReferenceClockTimerControl 
{
protected:
    virtual ~CBaseReferenceClock();     // Don't let me be created on the stack!
public:
    CBaseReferenceClock(__in_opt LPCTSTR pName, 
                        __inout_opt LPUNKNOWN pUnk, 
                        __inout HRESULT *phr, 
                        __inout_opt CAMSchedule * pSched = 0 );

    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, __deref_out void ** ppv);

    DECLARE_IUNKNOWN

    /* IReferenceClock methods */
    // Derived classes must implement GetPrivateTime().  All our GetTime
    // does is call GetPrivateTime and then check so that time does not
    // go backwards.  A return code of S_FALSE implies that the internal
    // clock has gone backwards and GetTime time has halted until internal
    // time has caught up. (Don't know if this will be much use to folk,
    // but it seems odd not to use the return code for something useful.)
    STDMETHODIMP GetTime(__out REFERENCE_TIME *pTime);
    // When this is called, it sets m_rtLastGotTime to the time it returns.

    /* Provide standard mechanisms for scheduling events */

    /* Ask for an async notification that a time has elapsed */
    STDMETHODIMP AdviseTime(
        REFERENCE_TIME baseTime,        // base reference time
        REFERENCE_TIME streamTime,      // stream offset time
        HEVENT hEvent,                  // advise via this event
        __out DWORD_PTR *pdwAdviseCookie// where your cookie goes
    );

    /* Ask for an asynchronous periodic notification that a time has elapsed */
    STDMETHODIMP AdvisePeriodic(
        REFERENCE_TIME StartTime,       // starting at this time
        REFERENCE_TIME PeriodTime,      // time between notifications
        HSEMAPHORE hSemaphore,          // advise via a semaphore
        __out DWORD_PTR *pdwAdviseCookie// where your cookie goes
    );

    /* Cancel a request for notification(s) - if the notification was
     * a one shot timer then this function doesn't need to be called
     * as the advise is automatically cancelled, however it does no
     * harm to explicitly cancel a one-shot advise.  It is REQUIRED that
     * clients call Unadvise to clear a Periodic advise setting.
     */

    STDMETHODIMP Unadvise(DWORD_PTR dwAdviseCookie);

    /* Methods for the benefit of derived classes or outer objects */

    // GetPrivateTime() is the REAL clock.  GetTime is just a cover for
    // it.  Derived classes will probably override this method but not
    // GetTime() itself.
    // The important point about GetPrivateTime() is it's allowed to go
    // backwards.  Our GetTime() will keep returning the LastGotTime
    // until GetPrivateTime() catches up.
    virtual REFERENCE_TIME GetPrivateTime();

    /* Provide a method for correcting drift */
    STDMETHODIMP SetTimeDelta( const REFERENCE_TIME& TimeDelta );

    CAMSchedule * GetSchedule() const { return m_pSchedule; }

    // IReferenceClockTimerControl methods
    //
    // Setting a default of 0 disables the default of 1ms
    STDMETHODIMP SetDefaultTimerResolution(
        REFERENCE_TIME timerResolution // in 100ns
    );
    STDMETHODIMP GetDefaultTimerResolution(
        __out REFERENCE_TIME* pTimerResolution // in 100ns
    );

private:
    REFERENCE_TIME m_rtPrivateTime;     // Current best estimate of time
    DWORD          m_dwPrevSystemTime;  // Last vaule we got from timeGetTime
    REFERENCE_TIME m_rtLastGotTime;     // Last time returned by GetTime
    REFERENCE_TIME m_rtNextAdvise;      // Time of next advise
    UINT           m_TimerResolution;

#ifdef PERF
    int m_idGetSystemTime;
#endif

// Thread stuff
public:
    void TriggerThread()    // Wakes thread up.  Need to do this if
    {                       // time to next advise needs reevaluating.
        EXECUTE_ASSERT(SetEvent(m_pSchedule->GetEvent()));
    }


private:
    BOOL           m_bAbort;            // Flag used for thread shutdown
    HANDLE         m_hThread;           // Thread handle

    HRESULT AdviseThread();             // Method in which the advise thread runs
    static DWORD __stdcall AdviseThreadFunction(__in LPVOID); // Function used to get there

protected:
    CAMSchedule * m_pSchedule;

    void Restart (IN REFERENCE_TIME rtMinTime = 0I64) ;
};

#endif

//------------------------------------------------------------------------------
// File: VideoCtl.h
//
// Desc: DirectShow base classes.
//
// Copyright (c) 1992-2001 Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------

#ifndef __VIDEOCTL__
#define __VIDEOCTL__

// These help with property page implementations. The first can be used to
// load any string from a resource file. The buffer to load into is passed
// as an input parameter. The same buffer is the return value if the string
// was found otherwise it returns TEXT(""). The GetDialogSize is passed the
// resource ID of a dialog box and returns the size of it in screen pixels

#define STR_MAX_LENGTH 256
LPTSTR WINAPI StringFromResource(__out_ecount(STR_MAX_LENGTH) LPTSTR pBuffer, int iResourceID);

#ifdef UNICODE
#define WideStringFromResource StringFromResource
LPSTR WINAPI StringFromResource(__out_ecount(STR_MAX_LENGTH) LPSTR pBuffer, int iResourceID);
#else
LPWSTR WINAPI WideStringFromResource(__out_ecount(STR_MAX_LENGTH) LPWSTR pBuffer, int iResourceID);
#endif


BOOL WINAPI GetDialogSize(int iResourceID,     // Dialog box resource identifier
                          DLGPROC pDlgProc,    // Pointer to dialog procedure
                          LPARAM lParam,       // Any user data wanted in pDlgProc
                          __out SIZE *pResult);// Returns the size of dialog box

// Class that aggregates an IDirectDraw interface

class CAggDirectDraw : public IDirectDraw, public CUnknown
{
protected:

    LPDIRECTDRAW m_pDirectDraw;

public:

    DECLARE_IUNKNOWN
    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid,__deref_out void **ppv);

    // Constructor and destructor

    CAggDirectDraw(__in_opt LPCTSTR pName,__inout_opt LPUNKNOWN pUnk) :
        CUnknown(pName,pUnk),
        m_pDirectDraw(NULL) { };

    virtual CAggDirectDraw::~CAggDirectDraw() { };

    // Set the object we should be aggregating
    void SetDirectDraw(__inout LPDIRECTDRAW pDirectDraw) {
        m_pDirectDraw = pDirectDraw;
    }

    // IDirectDraw methods

    STDMETHODIMP Compact();
    STDMETHODIMP CreateClipper(DWORD dwFlags,__deref_out LPDIRECTDRAWCLIPPER *lplpDDClipper,__inout_opt IUnknown *pUnkOuter);
    STDMETHODIMP CreatePalette(DWORD dwFlags,__in LPPALETTEENTRY lpColorTable,__deref_out LPDIRECTDRAWPALETTE *lplpDDPalette,__inout_opt IUnknown *pUnkOuter);
    STDMETHODIMP CreateSurface(__in LPDDSURFACEDESC lpDDSurfaceDesc,__deref_out LPDIRECTDRAWSURFACE *lplpDDSurface,__inout_opt IUnknown *pUnkOuter);
    STDMETHODIMP DuplicateSurface(__in LPDIRECTDRAWSURFACE lpDDSurface,__deref_out LPDIRECTDRAWSURFACE *lplpDupDDSurface);
    STDMETHODIMP EnumDisplayModes(DWORD dwSurfaceDescCount,__in LPDDSURFACEDESC lplpDDSurfaceDescList,__in LPVOID lpContext,__in LPDDENUMMODESCALLBACK lpEnumCallback);
    STDMETHODIMP EnumSurfaces(DWORD dwFlags,__in LPDDSURFACEDESC lpDDSD,__in LPVOID lpContext,__in LPDDENUMSURFACESCALLBACK lpEnumCallback);
    STDMETHODIMP FlipToGDISurface();
    STDMETHODIMP GetCaps(__out LPDDCAPS lpDDDriverCaps,__out LPDDCAPS lpDDHELCaps);
    STDMETHODIMP GetDisplayMode(__out LPDDSURFACEDESC lpDDSurfaceDesc);
    STDMETHODIMP GetFourCCCodes(__inout LPDWORD lpNumCodes,__out_ecount(*lpNumCodes) LPDWORD lpCodes);
    STDMETHODIMP GetGDISurface(__deref_out LPDIRECTDRAWSURFACE *lplpGDIDDSurface);
    STDMETHODIMP GetMonitorFrequency(__out LPDWORD lpdwFrequency);
    STDMETHODIMP GetScanLine(__out LPDWORD lpdwScanLine);
    STDMETHODIMP GetVerticalBlankStatus(__out LPBOOL lpblsInVB);
    STDMETHODIMP Initialize(__in GUID *lpGUID);
    STDMETHODIMP RestoreDisplayMode();
    STDMETHODIMP SetCooperativeLevel(HWND hWnd,DWORD dwFlags);
    STDMETHODIMP SetDisplayMode(DWORD dwWidth,DWORD dwHeight,DWORD dwBpp);
    STDMETHODIMP WaitForVerticalBlank(DWORD dwFlags,HANDLE hEvent);
};


// Class that aggregates an IDirectDrawSurface interface

class CAggDrawSurface : public IDirectDrawSurface, public CUnknown
{
protected:

    LPDIRECTDRAWSURFACE m_pDirectDrawSurface;

public:

    DECLARE_IUNKNOWN
    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid,__deref_out void **ppv);

    // Constructor and destructor

    CAggDrawSurface(__in_opt LPCTSTR pName,__inout_opt LPUNKNOWN pUnk) :
        CUnknown(pName,pUnk),
        m_pDirectDrawSurface(NULL) { };

    virtual ~CAggDrawSurface() { };

    // Set the object we should be aggregating
    void SetDirectDrawSurface(__inout LPDIRECTDRAWSURFACE pDirectDrawSurface) {
        m_pDirectDrawSurface = pDirectDrawSurface;
    }

    // IDirectDrawSurface methods

    STDMETHODIMP AddAttachedSurface(__in LPDIRECTDRAWSURFACE lpDDSAttachedSurface);
    STDMETHODIMP AddOverlayDirtyRect(__in LPRECT lpRect);
    STDMETHODIMP Blt(__in LPRECT lpDestRect,__in LPDIRECTDRAWSURFACE lpDDSrcSurface,__in LPRECT lpSrcRect,DWORD dwFlags,__in LPDDBLTFX lpDDBltFx);
    STDMETHODIMP BltBatch(__in_ecount(dwCount) LPDDBLTBATCH lpDDBltBatch,DWORD dwCount,DWORD dwFlags);
    STDMETHODIMP BltFast(DWORD dwX,DWORD dwY,__in LPDIRECTDRAWSURFACE lpDDSrcSurface,__in LPRECT lpSrcRect,DWORD dwTrans);
    STDMETHODIMP DeleteAttachedSurface(DWORD dwFlags,__in LPDIRECTDRAWSURFACE lpDDSAttachedSurface);
    STDMETHODIMP EnumAttachedSurfaces(__in LPVOID lpContext,__in LPDDENUMSURFACESCALLBACK lpEnumSurfacesCallback);
    STDMETHODIMP EnumOverlayZOrders(DWORD dwFlags,__in LPVOID lpContext,__in LPDDENUMSURFACESCALLBACK lpfnCallback);
    STDMETHODIMP Flip(__in LPDIRECTDRAWSURFACE lpDDSurfaceTargetOverride,DWORD dwFlags);
    STDMETHODIMP GetAttachedSurface(__in LPDDSCAPS lpDDSCaps,__deref_out LPDIRECTDRAWSURFACE *lplpDDAttachedSurface);
    STDMETHODIMP GetBltStatus(DWORD dwFlags);
    STDMETHODIMP GetCaps(__out LPDDSCAPS lpDDSCaps);
    STDMETHODIMP GetClipper(__deref_out LPDIRECTDRAWCLIPPER *lplpDDClipper);
    STDMETHODIMP GetColorKey(DWORD dwFlags,__out LPDDCOLORKEY lpDDColorKey);
    STDMETHODIMP GetDC(__out HDC *lphDC);
    STDMETHODIMP GetFlipStatus(DWORD dwFlags);
    STDMETHODIMP GetOverlayPosition(__out LPLONG lpdwX,__out LPLONG lpdwY);
    STDMETHODIMP GetPalette(__deref_out LPDIRECTDRAWPALETTE *lplpDDPalette);
    STDMETHODIMP GetPixelFormat(__out LPDDPIXELFORMAT lpDDPixelFormat);
    STDMETHODIMP GetSurfaceDesc(__out LPDDSURFACEDESC lpDDSurfaceDesc);
    STDMETHODIMP Initialize(__in LPDIRECTDRAW lpDD,__in LPDDSURFACEDESC lpDDSurfaceDesc);
    STDMETHODIMP IsLost();
    STDMETHODIMP Lock(__in LPRECT lpDestRect,__inout LPDDSURFACEDESC lpDDSurfaceDesc,DWORD dwFlags,HANDLE hEvent);
    STDMETHODIMP ReleaseDC(HDC hDC);
    STDMETHODIMP Restore();
    STDMETHODIMP SetClipper(__in LPDIRECTDRAWCLIPPER lpDDClipper);
    STDMETHODIMP SetColorKey(DWORD dwFlags,__in LPDDCOLORKEY lpDDColorKey);
    STDMETHODIMP SetOverlayPosition(LONG dwX,LONG dwY);
    STDMETHODIMP SetPalette(__in LPDIRECTDRAWPALETTE lpDDPalette);
    STDMETHODIMP Unlock(__in LPVOID lpSurfaceData);
    STDMETHODIMP UpdateOverlay(__in LPRECT lpSrcRect,__in LPDIRECTDRAWSURFACE lpDDDestSurface,__in LPRECT lpDestRect,DWORD dwFlags,__in LPDDOVERLAYFX lpDDOverlayFX);
    STDMETHODIMP UpdateOverlayDisplay(DWORD dwFlags);
    STDMETHODIMP UpdateOverlayZOrder(DWORD dwFlags,__in LPDIRECTDRAWSURFACE lpDDSReference);
};


class CLoadDirectDraw
{
    LPDIRECTDRAW m_pDirectDraw;     // The DirectDraw driver instance
    HINSTANCE m_hDirectDraw;        // Handle to the loaded library

public:

    CLoadDirectDraw();
    ~CLoadDirectDraw();

    HRESULT LoadDirectDraw(__in LPSTR szDevice);
    void ReleaseDirectDraw();
    HRESULT IsDirectDrawLoaded();
    LPDIRECTDRAW GetDirectDraw();
    BOOL IsDirectDrawVersion1();
};

#endif // __VIDEOCTL__

//------------------------------------------------------------------------------
// File: Measure.h
//
// Desc: DirectShow base classes.
//
// Copyright (c) 1992-2001 Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------

/*
   The idea is to pepper the source code with interesting measurements and
   have the last few thousand of these recorded in a circular buffer that
   can be post-processed to give interesting numbers.

   WHAT THE LOG LOOKS LIKE:

  Time (sec)   Type        Delta  Incident_Name
    0.055,41  NOTE      -.       Incident Nine  - Another note
    0.055,42  NOTE      0.000,01 Incident Nine  - Another note
    0.055,44  NOTE      0.000,02 Incident Nine  - Another note
    0.055,45  STOP      -.       Incident Eight - Also random
    0.055,47  START     -.       Incident Seven - Random
    0.055,49  NOTE      0.000,05 Incident Nine  - Another note
    ------- <etc.  there is a lot of this> ----------------
    0.125,60  STOP      0.000,03 Msr_Stop
    0.125,62  START     -.       Msr_Start
    0.125,63  START     -.       Incident Two   - Start/Stop
    0.125,65  STOP      0.000,03 Msr_Start
    0.125,66  START     -.       Msr_Stop
    0.125,68  STOP      0.000,05 Incident Two   - Start/Stop
    0.125,70  STOP      0.000,04 Msr_Stop
    0.125,72  START     -.       Msr_Start
    0.125,73  START     -.       Incident Two   - Start/Stop
    0.125,75  STOP      0.000,03 Msr_Start
    0.125,77  START     -.       Msr_Stop
    0.125,78  STOP      0.000,05 Incident Two   - Start/Stop
    0.125,80  STOP      0.000,03 Msr_Stop
    0.125,81  NOTE      -.       Incident Three - single Note
    0.125,83  START     -.       Incident Four  - Start, no stop
    0.125,85  START     -.       Incident Five  - Single Start/Stop
    0.125,87  STOP      0.000,02 Incident Five  - Single Start/Stop

Number      Average       StdDev     Smallest      Largest Incident_Name
    10     0.000,58     0.000,10     0.000,55     0.000,85 Incident One   - Note
    50     0.000,05     0.000,00     0.000,05     0.000,05 Incident Two   - Start/Stop
     1     -.           -.           -.           -.       Incident Three - single Note
     0     -.           -.           -.           -.       Incident Four  - Start, no stop
     1     0.000,02     -.           0.000,02     0.000,02 Incident Five  - Single Start/Stop
     0     -.           -.           -.           -.       Incident Six   - zero occurrences
   100     0.000,25     0.000,12     0.000,02     0.000,62 Incident Seven - Random
   100     0.000,79     0.000,48     0.000,02     0.001,92 Incident Eight - Also random
  5895     0.000,01     0.000,01     0.000,01     0.000,56 Incident Nine  - Another note
    10     0.000,03     0.000,00     0.000,03     0.000,04 Msr_Note
    50     0.000,03     0.000,00     0.000,03     0.000,04 Msr_Start
    50     0.000,04     0.000,03     0.000,03     0.000,31 Msr_Stop

  WHAT IT MEANS:
    The log shows what happened and when.  Each line shows the time at which
    something happened (see WHAT YOU CODE below) what it was that happened
    and (if approporate) the time since the corresponding previous event
    (that's the delta column).

    The statistics show how many times each event occurred, what the average
    delta time was, also the standard deviation, largest and smalles delta.

   WHAT YOU CODE:

   Before anything else executes: - register your ids

    int id1     = Msr_Register("Incident One   - Note");
    int id2     = Msr_Register("Incident Two   - Start/Stop");
    int id3     = Msr_Register("Incident Three - single Note");
    etc.

   At interesting moments:

       // To measure a repetitive event - e.g. end of bitblt to screen
       Msr_Note(Id9);             // e.g. "video frame hiting the screen NOW!"

           or

       // To measure an elapsed time e.g. time taken to decode an MPEG B-frame
       Msr_Start(Id2);            // e.g. "Starting to decode MPEG B-frame"
         . . .
       MsrStop(Id2);              //      "Finished MPEG decode"

   At the end:

       HANDLE hFile;
       hFile = CreateFile("Perf.log", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
       Msr_Dump(hFile);           // This writes the log out to the file
       CloseHandle(hFile);

           or

       Msr_Dump(NULL);            // This writes it to DbgLog((LOG_TRACE,0, ... ));
                                  // but if you are writing it out to the debugger
                                  // then the times are probably all garbage because
                                  // the debugger can make things run awfully slow.

    A given id should be used either for start / stop or Note calls.  If Notes
    are mixed in with Starts and Stops their statistics will be gibberish.

    If you code the calls in upper case i.e. MSR_START(idMunge); then you get
    macros which will turn into nothing unless PERF is defined.

    You can reset the statistical counts for a given id by calling Reset(Id).
    They are reset by default at the start.
    It logs Reset as a special incident, so you can see it in the log.

    The log is a circular buffer in storage (to try to minimise disk I/O).
    It overwrites the oldest entries once full.  The statistics include ALL
    incidents since the last Reset, whether still visible in the log or not.
*/

#ifndef __MEASURE__
#define __MEASURE__

#ifdef PERF
#define MSR_INIT() Msr_Init()
#define MSR_TERMINATE() Msr_Terminate()
#define MSR_REGISTER(a) Msr_Register(a)
#define MSR_RESET(a) Msr_Reset(a)
#define MSR_CONTROL(a) Msr_Control(a)
#define MSR_START(a) Msr_Start(a)
#define MSR_STOP(a) Msr_Stop(a)
#define MSR_NOTE(a) Msr_Note(a)
#define MSR_INTEGER(a,b) Msr_Integer(a,b)
#define MSR_DUMP(a) Msr_Dump(a)
#define MSR_DUMPSTATS(a) Msr_DumpStats(a)
#else
#define MSR_INIT() ((void)0)
#define MSR_TERMINATE() ((void)0)
#define MSR_REGISTER(a) 0
#define MSR_RESET(a) ((void)0)
#define MSR_CONTROL(a) ((void)0)
#define MSR_START(a) ((void)0)
#define MSR_STOP(a) ((void)0)
#define MSR_NOTE(a) ((void)0)
#define MSR_INTEGER(a,b) ((void)0)
#define MSR_DUMP(a) ((void)0)
#define MSR_DUMPSTATS(a) ((void)0)
#endif

#ifdef __cplusplus
extern "C" {
#endif

// This must be called first - (called by the DllEntry)

void WINAPI Msr_Init(void);


// Call this last to clean up (or just let it fall off the end - who cares?)

void WINAPI Msr_Terminate(void);


// Call this to get an Id for an "incident" that you can pass to Start, Stop or Note
// everything that's logged is called an "incident".

int  WINAPI Msr_Register(__in LPTSTR Incident);


// Reset the statistical counts for an incident

void WINAPI Msr_Reset(int Id);


// Reset all the counts for all incidents
#define MSR_RESET_ALL 0
#define MSR_PAUSE 1
#define MSR_RUN 2

void WINAPI Msr_Control(int iAction);


// log the start of an operation

void WINAPI Msr_Start(int Id);


// log the end of an operation

void WINAPI Msr_Stop(int Id);


// log a one-off or repetitive operation

void WINAPI Msr_Note(int Id);


// log an integer (on which we can see statistics later)
void WINAPI Msr_Integer(int Id, int n);


// print out all the vaialable log (it may have wrapped) and then the statistics.
// When the log wraps you lose log but the statistics are still complete.
// hFIle==NULL => use DbgLog
// otherwise hFile must have come from CreateFile or OpenFile.

void WINAPI Msr_Dump(HANDLE hFile);


// just dump the statistics - never mind the log

void WINAPI Msr_DumpStats(HANDLE hFile);

// Type definitions in case you want to declare a pointer to the dump functions
// (makes it a trifle easier to do dynamic linking
// i.e. LoadModule, GetProcAddress and call that)

// Typedefs so can declare MSR_DUMPPROC *MsrDumpStats; or whatever
typedef void WINAPI MSR_DUMPPROC(HANDLE hFile);
typedef void WINAPI MSR_CONTROLPROC(int iAction);


#ifdef __cplusplus
}
#endif

#endif // __MEASURE__

//------------------------------------------------------------------------------
// File: WinUtil.h
//
// Desc: DirectShow base classes - defines generic handler classes.
//
// Copyright (c) 1992-2001 Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------

// Make sure that you call PrepareWindow to initialise the window after
// the object has been constructed. It is a separate method so that
// derived classes can override useful methods like MessageLoop. Also
// any derived class must call DoneWithWindow in its destructor. If it
// doesn't a message may be retrieved and call a derived class member
// function while a thread is executing the base class destructor code

#ifndef __WINUTIL__
#define __WINUTIL__

const int DEFWIDTH = 320;                    // Initial window width
const int DEFHEIGHT = 240;                   // Initial window height
const int CAPTION = 256;                     // Maximum length of caption
const int TIMELENGTH = 50;                   // Maximum length of times
const int PROFILESTR = 128;                  // Normal profile string
const WORD PALVERSION = 0x300;               // GDI palette version
const LONG PALETTE_VERSION = (LONG) 1;       // Initial palette version
const COLORREF VIDEO_COLOUR = 0;             // Defaults to black background
const HANDLE hMEMORY = (HANDLE) (-1);        // Says to open as memory file

#define WIDTH(x) ((*(x)).right - (*(x)).left)
#define HEIGHT(x) ((*(x)).bottom - (*(x)).top)
#define SHOWSTAGE TEXT("WM_SHOWSTAGE")
#define SHOWSTAGETOP TEXT("WM_SHOWSTAGETOP")
#define REALIZEPALETTE TEXT("WM_REALIZEPALETTE")

class AM_NOVTABLE CBaseWindow
{
protected:

    HINSTANCE m_hInstance;          // Global module instance handle
    HWND m_hwnd;                    // Handle for our window
    HDC m_hdc;                      // Device context for the window
    LONG m_Width;                   // Client window width
    LONG m_Height;                  // Client window height
    BOOL m_bActivated;              // Has the window been activated
    LPTSTR m_pClassName;            // Static string holding class name
    DWORD m_ClassStyles;            // Passed in to our constructor
    DWORD m_WindowStyles;           // Likewise the initial window styles
    DWORD m_WindowStylesEx;         // And the extended window styles
    UINT m_ShowStageMessage;        // Have the window shown with focus
    UINT m_ShowStageTop;            // Makes the window WS_EX_TOPMOST
    UINT m_RealizePalette;          // Makes us realize our new palette
    HDC m_MemoryDC;                 // Used for fast BitBlt operations
    HPALETTE m_hPalette;            // Handle to any palette we may have
    BYTE m_bNoRealize;              // Don't realize palette now
    BYTE m_bBackground;             // Should we realise in background
    BYTE m_bRealizing;              // already realizing the palette
    CCritSec m_WindowLock;          // Serialise window object access
    BOOL m_bDoGetDC;                // Should this window get a DC
    bool m_bDoPostToDestroy;        // Use PostMessage to destroy
    CCritSec m_PaletteLock;         // This lock protects m_hPalette.
                                    // It should be held anytime the
                                    // program use the value of m_hPalette.

    // Maps windows message procedure into C++ methods
    friend LRESULT CALLBACK WndProc(HWND hwnd,      // Window handle
                                    UINT uMsg,      // Message ID
                                    WPARAM wParam,  // First parameter
                                    LPARAM lParam); // Other parameter

    virtual LRESULT OnPaletteChange(HWND hwnd, UINT Message);

public:

    CBaseWindow(BOOL bDoGetDC = TRUE, bool bPostToDestroy = false);

#ifdef DEBUG
    virtual ~CBaseWindow();
#endif

    virtual HRESULT DoneWithWindow();
    virtual HRESULT PrepareWindow();
    virtual HRESULT InactivateWindow();
    virtual HRESULT ActivateWindow();
    virtual BOOL OnSize(LONG Width, LONG Height);
    virtual BOOL OnClose();
    virtual RECT GetDefaultRect();
    virtual HRESULT UninitialiseWindow();
    virtual HRESULT InitialiseWindow(HWND hwnd);

    HRESULT CompleteConnect();
    HRESULT DoCreateWindow();

    HRESULT PerformanceAlignWindow();
    HRESULT DoShowWindow(LONG ShowCmd);
    void PaintWindow(BOOL bErase);
    void DoSetWindowForeground(BOOL bFocus);
    virtual HRESULT SetPalette(HPALETTE hPalette);
    void SetRealize(BOOL bRealize)
    {
        m_bNoRealize = !bRealize;
    }

    //  Jump over to the window thread to set the current palette
    HRESULT SetPalette();
    void UnsetPalette(void);
    virtual HRESULT DoRealisePalette(BOOL bForceBackground = FALSE);

    void LockPaletteLock();
    void UnlockPaletteLock();

    virtual BOOL PossiblyEatMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	    { return FALSE; };

    // Access our window information

    bool WindowExists();
    LONG GetWindowWidth();
    LONG GetWindowHeight();
    HWND GetWindowHWND();
    HDC GetMemoryHDC();
    HDC GetWindowHDC();

    #ifdef DEBUG
    HPALETTE GetPalette();
    #endif // DEBUG

    // This is the window procedure the derived object should override

    virtual LRESULT OnReceiveMessage(HWND hwnd,          // Window handle
                                     UINT uMsg,          // Message ID
                                     WPARAM wParam,      // First parameter
                                     LPARAM lParam);     // Other parameter

    // Must be overriden to return class and window styles

    virtual LPTSTR GetClassWindowStyles(
                            __out DWORD *pClassStyles,          // Class styles
                            __out DWORD *pWindowStyles,         // Window styles
                            __out DWORD *pWindowStylesEx) PURE; // Extended styles
};


// This helper class is entirely subservient to the owning CBaseWindow object
// All this object does is to split out the actual drawing operation from the
// main object (because it was becoming too large). We have a number of entry
// points to set things like the draw device contexts, to implement the actual
// drawing and to set the destination rectangle in the client window. We have
// no critical section locking in this class because we are used exclusively
// by the owning window object which looks after serialising calls into us

// If you want to use this class make sure you call NotifyAllocator once the
// allocate has been agreed, also call NotifyMediaType with a pointer to a
// NON stack based CMediaType once that has been set (we keep a pointer to
// the original rather than taking a copy). When the palette changes call
// IncrementPaletteVersion (easiest thing to do is to also call this method
// in the SetMediaType method most filters implement). Finally before you
// start rendering anything call SetDrawContext so that we can get the HDCs
// for drawing from the CBaseWindow object we are given during construction

class CDrawImage
{
protected:

    CBaseWindow *m_pBaseWindow;     // Owning video window object
    CRefTime m_StartSample;         // Start time for the current sample
    CRefTime m_EndSample;           // And likewise it's end sample time
    HDC m_hdc;                      // Main window device context
    HDC m_MemoryDC;                 // Offscreen draw device context
    RECT m_TargetRect;              // Target destination rectangle
    RECT m_SourceRect;              // Source image rectangle
    BOOL m_bStretch;                // Do we have to stretch the images
    BOOL m_bUsingImageAllocator;    // Are the samples shared DIBSECTIONs
    CMediaType *m_pMediaType;       // Pointer to the current format
    int m_perfidRenderTime;         // Time taken to render an image
    LONG m_PaletteVersion;          // Current palette version cookie

    // Draw the video images in the window

    void SlowRender(IMediaSample *pMediaSample);
    void FastRender(IMediaSample *pMediaSample);
    void DisplaySampleTimes(IMediaSample *pSample);
    void UpdateColourTable(HDC hdc,__in BITMAPINFOHEADER *pbmi);
    void SetStretchMode();

public:

    // Used to control the image drawing

    CDrawImage(__inout CBaseWindow *pBaseWindow);
    BOOL DrawImage(IMediaSample *pMediaSample);
    BOOL DrawVideoImageHere(HDC hdc, IMediaSample *pMediaSample,
                            __in LPRECT lprcSrc, __in LPRECT lprcDst);
    void SetDrawContext();
    void SetTargetRect(__in RECT *pTargetRect);
    void SetSourceRect(__in RECT *pSourceRect);
    void GetTargetRect(__out RECT *pTargetRect);
    void GetSourceRect(__out RECT *pSourceRect);
    virtual RECT ScaleSourceRect(const RECT *pSource);

    // Handle updating palettes as they change

    LONG GetPaletteVersion();
    void ResetPaletteVersion();
    void IncrementPaletteVersion();

    // Tell us media types and allocator assignments

    void NotifyAllocator(BOOL bUsingImageAllocator);
    void NotifyMediaType(__in CMediaType *pMediaType);
    BOOL UsingImageAllocator();

    // Called when we are about to draw an image

    void NotifyStartDraw() {
        MSR_START(m_perfidRenderTime);
    };

    // Called when we complete an image rendering

    void NotifyEndDraw() {
        MSR_STOP(m_perfidRenderTime);
    };
};


// This is the structure used to keep information about each GDI DIB. All the
// samples we create from our allocator will have a DIBSECTION allocated to
// them. When we receive the sample we know we can BitBlt straight to an HDC

typedef struct tagDIBDATA {

    LONG        PaletteVersion;     // Current palette version in use
    DIBSECTION  DibSection;         // Details of DIB section allocated
    HBITMAP     hBitmap;            // Handle to bitmap for drawing
    HANDLE      hMapping;           // Handle to shared memory block
    BYTE        *pBase;             // Pointer to base memory address

} DIBDATA;


// This class inherits from CMediaSample and uses all of it's methods but it
// overrides the constructor to initialise itself with the DIBDATA structure
// When we come to render an IMediaSample we will know if we are using our own
// allocator, and if we are, we can cast the IMediaSample to a pointer to one
// of these are retrieve the DIB section information and hence the HBITMAP

class CImageSample : public CMediaSample
{
protected:

    DIBDATA m_DibData;      // Information about the DIBSECTION
    BOOL m_bInit;           // Is the DIB information setup

public:

    // Constructor

    CImageSample(__inout CBaseAllocator *pAllocator,
                 __in_opt LPCTSTR pName,
                 __inout HRESULT *phr,
                 __in_bcount(length) LPBYTE pBuffer,
                 LONG length);

    // Maintain the DIB/DirectDraw state

    void SetDIBData(__in DIBDATA *pDibData);
    __out DIBDATA *GetDIBData();
};


// This is an allocator based on the abstract CBaseAllocator base class that
// allocates sample buffers in shared memory. The number and size of these
// are determined when the output pin calls Prepare on us. The shared memory
// blocks are used in subsequent calls to GDI CreateDIBSection, once that
// has been done the output pin can fill the buffers with data which will
// then be handed to GDI through BitBlt calls and thereby remove one copy

class CImageAllocator : public CBaseAllocator
{
protected:

    CBaseFilter *m_pFilter;   // Delegate reference counts to
    CMediaType *m_pMediaType;           // Pointer to the current format

    // Used to create and delete samples

    HRESULT Alloc();
    void Free();

    // Manage the shared DIBSECTION and DCI/DirectDraw buffers

    HRESULT CreateDIB(LONG InSize,DIBDATA &DibData);
    STDMETHODIMP CheckSizes(__in ALLOCATOR_PROPERTIES *pRequest);
    virtual CImageSample *CreateImageSample(__in_bcount(Length) LPBYTE pData,LONG Length);

public:

    // Constructor and destructor

    CImageAllocator(__inout CBaseFilter *pFilter,__in_opt LPCTSTR pName,__inout HRESULT *phr);
#ifdef DEBUG
    ~CImageAllocator();
#endif

    STDMETHODIMP_(ULONG) NonDelegatingAddRef();
    STDMETHODIMP_(ULONG) NonDelegatingRelease();
    void NotifyMediaType(__in CMediaType *pMediaType);

    // Agree the number of buffers to be used and their size

    STDMETHODIMP SetProperties(
        __in ALLOCATOR_PROPERTIES *pRequest,
        __out ALLOCATOR_PROPERTIES *pActual);
};


// This class is a fairly specialised helper class for image renderers that
// have to create and manage palettes. The CBaseWindow class looks after
// realising palettes once they have been installed. This class can be used
// to create the palette handles from a media format (which must contain a
// VIDEOINFO structure in the format block). We try to make the palette an
// identity palette to maximise performance and also only change palettes
// if actually required to (we compare palette colours before updating).
// All the methods are virtual so that they can be overriden if so required

class CImagePalette
{
protected:

    CBaseWindow *m_pBaseWindow;             // Window to realise palette in
    CBaseFilter *m_pFilter;                 // Media filter to send events
    CDrawImage *m_pDrawImage;               // Object who will be drawing
    HPALETTE m_hPalette;                    // The palette handle we own

public:

    CImagePalette(__inout CBaseFilter *pBaseFilter,
                  __inout CBaseWindow *pBaseWindow,
                  __inout CDrawImage *pDrawImage);

#ifdef DEBUG
    virtual ~CImagePalette();
#endif

    static HPALETTE MakePalette(const VIDEOINFOHEADER *pVideoInfo, __in LPSTR szDevice);
    HRESULT RemovePalette();
    static HRESULT MakeIdentityPalette(__inout_ecount_full(iColours) PALETTEENTRY *pEntry,INT iColours, __in LPSTR szDevice);
    HRESULT CopyPalette(const CMediaType *pSrc,__out CMediaType *pDest);
    BOOL ShouldUpdate(const VIDEOINFOHEADER *pNewInfo,const VIDEOINFOHEADER *pOldInfo);
    HRESULT PreparePalette(const CMediaType *pmtNew,const CMediaType *pmtOld,__in LPSTR szDevice);

    BOOL DrawVideoImageHere(HDC hdc, IMediaSample *pMediaSample, __in LPRECT lprcSrc, __in LPRECT lprcDst)
    {
        return m_pDrawImage->DrawVideoImageHere(hdc, pMediaSample, lprcSrc,lprcDst);
    }
};


// Another helper class really for video based renderers. Most such renderers
// need to know what the display format is to some degree or another. This
// class initialises itself with the display format. The format can be asked
// for through GetDisplayFormat and various other accessor functions. If a
// filter detects a display format change (perhaps it gets a WM_DEVMODECHANGE
// message then it can call RefreshDisplayType to reset that format). Also
// many video renderers will want to check formats as they are proposed by
// source filters. This class provides methods to check formats and only
// accept those video formats that can be efficiently drawn using GDI calls

class CImageDisplay : public CCritSec
{
protected:

    // This holds the display format; biSize should not be too big, so we can
    // safely use the VIDEOINFO structure
    VIDEOINFO m_Display;

    static DWORD CountSetBits(const DWORD Field);
    static DWORD CountPrefixBits(const DWORD Field);
    static BOOL CheckBitFields(const VIDEOINFO *pInput);

public:

    // Constructor and destructor

    CImageDisplay();

    // Used to manage BITMAPINFOHEADERs and the display format

    const VIDEOINFO *GetDisplayFormat();
    HRESULT RefreshDisplayType(__in_opt LPSTR szDeviceName);
    static BOOL CheckHeaderValidity(const VIDEOINFO *pInput);
    static BOOL CheckPaletteHeader(const VIDEOINFO *pInput);
    BOOL IsPalettised();
    WORD GetDisplayDepth();

    // Provide simple video format type checking

    HRESULT CheckMediaType(const CMediaType *pmtIn);
    HRESULT CheckVideoType(const VIDEOINFO *pInput);
    HRESULT UpdateFormat(__inout VIDEOINFO *pVideoInfo);
    const DWORD *GetBitMasks(const VIDEOINFO *pVideoInfo);

    BOOL GetColourMask(__out DWORD *pMaskRed,
                       __out DWORD *pMaskGreen,
                       __out DWORD *pMaskBlue);
};

//  Convert a FORMAT_VideoInfo to FORMAT_VideoInfo2
STDAPI ConvertVideoInfoToVideoInfo2(__inout AM_MEDIA_TYPE *pmt);

//  Check a media type containing VIDEOINFOHEADER
STDAPI CheckVideoInfoType(const AM_MEDIA_TYPE *pmt);

//  Check a media type containing VIDEOINFOHEADER
STDAPI CheckVideoInfo2Type(const AM_MEDIA_TYPE *pmt);

#endif // __WINUTIL__

//------------------------------------------------------------------------------
// File: DlleEntry.cpp
//
// Desc: DirectShow base classes - implements classes used to support dll
//       entry points for COM objects.
//
// Copyright (c) 1992-2001 Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------

extern CFactoryTemplate g_Templates[];
extern int g_cTemplates;

HINSTANCE g_hInst;
DWORD	  g_amPlatform;		// VER_PLATFORM_WIN32_WINDOWS etc... (from GetVersionEx)
OSVERSIONINFO g_osInfo;

//
// an instance of this is created by the DLLGetClassObject entrypoint
// it uses the CFactoryTemplate object it is given to support the
// IClassFactory interface

class CClassFactory : public IClassFactory, public CBaseObject
{

private:
    const CFactoryTemplate *const m_pTemplate;

    ULONG m_cRef;

    static int m_cLocked;
public:
    CClassFactory(const CFactoryTemplate *);

    // IUnknown
    STDMETHODIMP QueryInterface(REFIID riid, __deref_out void ** ppv);
    STDMETHODIMP_(ULONG)AddRef();
    STDMETHODIMP_(ULONG)Release();

    // IClassFactory
    STDMETHODIMP CreateInstance(LPUNKNOWN pUnkOuter, REFIID riid, __deref_out void **pv);
    STDMETHODIMP LockServer(BOOL fLock);

    // allow DLLGetClassObject to know about global server lock status
    static BOOL IsLocked() {
        return (m_cLocked > 0);
    };
};

// process-wide dll locked state
int CClassFactory::m_cLocked = 0;

CClassFactory::CClassFactory(const CFactoryTemplate *pTemplate)
: CBaseObject(NAME("Class Factory"))
, m_cRef(0)
, m_pTemplate(pTemplate)
{
}


STDMETHODIMP
CClassFactory::QueryInterface(REFIID riid,__deref_out void **ppv)
{
    CheckPointer(ppv,E_POINTER)
    ValidateReadWritePtr(ppv,sizeof(PVOID));
    *ppv = NULL;

    // any interface on this object is the object pointer.
    if ((riid == IID_IUnknown) || (riid == IID_IClassFactory)) {
        *ppv = (LPVOID) this;
	// AddRef returned interface pointer
        ((LPUNKNOWN) *ppv)->AddRef();
        return NOERROR;
    }

    return ResultFromScode(E_NOINTERFACE);
}


STDMETHODIMP_(ULONG)
CClassFactory::AddRef()
{
    return ++m_cRef;
}

STDMETHODIMP_(ULONG)
CClassFactory::Release()
{
    LONG lRef = InterlockedDecrement((volatile LONG *)&m_cRef);
    if (lRef == 0) {
        delete this;
        return 0;
    } else {
        return lRef;
    }
}

STDMETHODIMP
CClassFactory::CreateInstance(
    LPUNKNOWN pUnkOuter,
    REFIID riid,
    __deref_out void **pv)
{
    CheckPointer(pv,E_POINTER)
    ValidateReadWritePtr(pv,sizeof(void *));
    *pv = NULL;

    /* Enforce the normal OLE rules regarding interfaces and delegation */

    if (pUnkOuter != NULL) {
        if (IsEqualIID(riid,IID_IUnknown) == FALSE) {
            *pv = NULL;
            return ResultFromScode(E_NOINTERFACE);
        }
    }

    /* Create the new object through the derived class's create function */

    HRESULT hr = NOERROR;
#ifndef STREAMS_PROVIDE_CUSTOM_FACTORY
    CUnknown *pObj = m_pTemplate->CreateInstance(pUnkOuter, &hr);
#else
    CUnknown *CustomCreateInstance(int FactoryType, LPUNKNOWN pUnkOuter, HRESULT* hr);
    CUnknown *pObj = CustomCreateInstance((int)(m_pTemplate - ((CFactoryTemplate*)NULL)), pUnkOuter, &hr);
#endif

    if (pObj == NULL) {
        *pv = NULL;
	if (SUCCEEDED(hr)) {
	    hr = E_OUTOFMEMORY;
	}
	return hr;
    }

    /* Delete the object if we got a construction error */

    if (FAILED(hr)) {
        delete pObj;
        *pv = NULL;
        return hr;
    }

    /* Get a reference counted interface on the object */

    /* We wrap the non-delegating QI with NDAddRef & NDRelease. */
    /* This protects any outer object from being prematurely    */
    /* released by an inner object that may have to be created  */
    /* in order to supply the requested interface.              */
    pObj->NonDelegatingAddRef();
    hr = pObj->NonDelegatingQueryInterface(riid, pv);
    pObj->NonDelegatingRelease();
    /* Note that if NonDelegatingQueryInterface fails, it will  */
    /* not increment the ref count, so the NonDelegatingRelease */
    /* will drop the ref back to zero and the object will "self-*/
    /* destruct".  Hence we don't need additional tidy-up code  */
    /* to cope with NonDelegatingQueryInterface failing.        */

    if (SUCCEEDED(hr)) {
        ASSERT(*pv);
    }

    return hr;
}

STDMETHODIMP
CClassFactory::LockServer(BOOL fLock)
{
    if (fLock) {
        m_cLocked++;
    } else {
        m_cLocked--;
    }
    return NOERROR;
}


// --- COM entrypoints -----------------------------------------

//called by COM to get the class factory object for a given class
__control_entrypoint(DllExport) STDAPI
DllGetClassObject(
    __in REFCLSID rClsID,
    __in REFIID riid,
    __deref_out void **pv)
{
    *pv = NULL;
    if (!(riid == IID_IUnknown) && !(riid == IID_IClassFactory)) {
            return E_NOINTERFACE;
    }

#ifndef STREAMS_PROVIDE_CUSTOM_FACTORY
    // traverse the array of templates looking for one with this
    // class id
    for (int i = 0; i < g_cTemplates; i++) {
        const CFactoryTemplate * pT = &g_Templates[i];
        if (pT->IsClassID(rClsID)) {
#else
    {
        int CustomGetFactoryType(const IID &rClsID);
        const CFactoryTemplate * pT = ((CFactoryTemplate*)NULL) + CustomGetFactoryType(rClsID);
        if (pT) {
#endif
            // found a template - make a class factory based on this
            // template

            *pv = (LPVOID) (LPUNKNOWN) new CClassFactory(pT);
            if (*pv == NULL) {
                return E_OUTOFMEMORY;
            }
            ((LPUNKNOWN)*pv)->AddRef();
            return NOERROR;
        }
    }
    return CLASS_E_CLASSNOTAVAILABLE;
}

//
//  Call any initialization routines
//
void
DllInitClasses(BOOL bLoading)
{
#ifndef STREAMS_PROVIDE_CUSTOM_FACTORY
    int i;

    // traverse the array of templates calling the init routine
    // if they have one
    for (i = 0; i < g_cTemplates; i++) {
        const CFactoryTemplate * pT = &g_Templates[i];
        if (pT->m_lpfnInit != NULL) {
            (*pT->m_lpfnInit)(bLoading, pT->m_ClsID);
        }
    }

#endif
}

// called by COM to determine if this dll can be unloaded
// return ok unless there are outstanding objects or a lock requested
// by IClassFactory::LockServer
//
// CClassFactory has a static function that can tell us about the locks,
// and CCOMObject has a static function that can tell us about the active
// object count
STDAPI
DllCanUnloadNow()
{
    DbgLog((LOG_MEMORY,2,TEXT("DLLCanUnloadNow called - IsLocked = %d, Active objects = %d"),
        CClassFactory::IsLocked(),
        CBaseObject::ObjectsActive()));

    if (CClassFactory::IsLocked() || CBaseObject::ObjectsActive()) {
	return S_FALSE;
    } else {
        return S_OK;
    }
}


// --- standard WIN32 entrypoints --------------------------------------


extern "C" void __cdecl __security_init_cookie(void);
extern "C" BOOL WINAPI _DllEntryPoint(HINSTANCE, ULONG, __inout_opt LPVOID);
#pragma comment(linker, "/merge:.CRT=.rdata")

extern "C"
DECLSPEC_NOINLINE
BOOL 
WINAPI
DllEntryPoint(
    HINSTANCE hInstance, 
    ULONG ulReason, 
    __inout_opt LPVOID pv
    )
{
    if ( ulReason == DLL_PROCESS_ATTACH ) {
        // Must happen before any other code is executed.  Thankfully - it's re-entrant
        __security_init_cookie();
    }
    return _DllEntryPoint(hInstance, ulReason, pv);
}


DECLSPEC_NOINLINE
BOOL 
WINAPI
_DllEntryPoint(
    HINSTANCE hInstance, 
    ULONG ulReason, 
    __inout_opt LPVOID pv
    )
{
#ifdef DEBUG
    extern bool g_fDbgInDllEntryPoint;
    g_fDbgInDllEntryPoint = true;
#endif

    switch (ulReason)
    {

    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hInstance);
        DbgInitialise(hInstance);

    	{
    	    // The platform identifier is used to work out whether
    	    // full unicode support is available or not.  Hence the
    	    // default will be the lowest common denominator - i.e. N/A
                g_amPlatform = VER_PLATFORM_WIN32_WINDOWS; // win95 assumed in case GetVersionEx fails
    
                g_osInfo.dwOSVersionInfoSize = sizeof(g_osInfo);
#pragma warning(push)
#pragma warning(disable: 4996) //warning C4996: 'GetVersionExA': was declared deprecated
                if (GetVersionEx(&g_osInfo)) {
#pragma warning(pop)
            	g_amPlatform = g_osInfo.dwPlatformId;
    	    } else {
    		DbgLog((LOG_ERROR, 1, TEXT("Failed to get the OS platform, assuming Win95")));
    	    }
    	}

        g_hInst = hInstance;
        DllInitClasses(TRUE);
        break;

    case DLL_PROCESS_DETACH:
        DllInitClasses(FALSE);

#ifdef DEBUG
        if (CBaseObject::ObjectsActive()) {
            DbgSetModuleLevel(LOG_MEMORY, 2);
            TCHAR szInfo[512];
            extern TCHAR m_ModuleName[];     // Cut down module name

            TCHAR FullName[_MAX_PATH];      // Load the full path and module name
            TCHAR *pName;                   // Searches from the end for a backslash

            GetModuleFileName(NULL,FullName,_MAX_PATH);
            pName = _tcsrchr(FullName,'\\');
            if (pName == NULL) {
                pName = FullName;
            } else {
                pName++;
            }

            (void)StringCchPrintf(szInfo, NUMELMS(szInfo), TEXT("Executable: %s  Pid %x  Tid %x. "),
			    pName, GetCurrentProcessId(), GetCurrentThreadId());

            (void)StringCchPrintf(szInfo+lstrlen(szInfo), NUMELMS(szInfo) - lstrlen(szInfo), TEXT("Module %s, %d objects left active!"),
                     m_ModuleName, CBaseObject::ObjectsActive());
            DbgAssert(szInfo, TEXT(__FILE__),__LINE__);

	    // If running remotely wait for the Assert to be acknowledged
	    // before dumping out the object register
            DbgDumpObjectRegister();
        }
        DbgTerminate();
#endif
        break;
    }

#ifdef DEBUG
    g_fDbgInDllEntryPoint = false;
#endif
    return TRUE;
}

//------------------------------------------------------------------------------
// File: WXDebug.cpp
//
// Desc: DirectShow base classes - implements ActiveX system debugging
//       facilities.
//
// Copyright (c) 1992-2001 Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------

#ifdef DEBUG
static void DisplayBITMAPINFO(const BITMAPINFOHEADER* pbmi);
static void DisplayRECT(LPCTSTR szLabel, const RECT& rc);

// The Win32 wsprintf() function writes a maximum of 1024 characters to it's output buffer.
// See the documentation for wsprintf()'s lpOut parameter for more information.
const INT iDEBUGINFO = 1024;                 // Used to format strings

/* For every module and executable we store a debugging level for each of
   the five categories (eg LOG_ERROR and LOG_TIMING). This makes it easy
   to isolate and debug individual modules without seeing everybody elses
   spurious debug output. The keys are stored in the registry under the
   HKEY_LOCAL_MACHINE\SOFTWARE\Debug\<Module Name>\<KeyName> key values
   NOTE these must be in the same order as their enumeration definition */

const LPCTSTR pKeyNames[] = {
    TEXT("TIMING"),      // Timing and performance measurements
    TEXT("TRACE"),       // General step point call tracing
    TEXT("MEMORY"),      // Memory and object allocation/destruction
    TEXT("LOCKING"),     // Locking/unlocking of critical sections
    TEXT("ERROR"),       // Debug error notification
    TEXT("CUSTOM1"),
    TEXT("CUSTOM2"),
    TEXT("CUSTOM3"),
    TEXT("CUSTOM4"),
    TEXT("CUSTOM5")
    };

const TCHAR CAutoTrace::_szEntering[] = TEXT("->: %s");
const TCHAR CAutoTrace::_szLeaving[]  = TEXT("<-: %s");

const INT iMAXLEVELS = NUMELMS(pKeyNames);  // Maximum debug categories

HINSTANCE m_hInst;                          // Module instance handle
TCHAR m_ModuleName[iDEBUGINFO];             // Cut down module name
DWORD m_Levels[iMAXLEVELS];                 // Debug level per category
CRITICAL_SECTION m_CSDebug;                 // Controls access to list
DWORD m_dwNextCookie;                       // Next active object ID
ObjectDesc *pListHead = NULL;               // First active object
DWORD m_dwObjectCount;                      // Active object count
BOOL m_bInit = FALSE;                       // Have we been initialised
HANDLE m_hOutput = INVALID_HANDLE_VALUE;    // Optional output written here
DWORD dwWaitTimeout = INFINITE;             // Default timeout value
DWORD dwTimeOffset;			    // Time of first DbgLog call
bool g_fUseKASSERT = false;                 // don't create messagebox
bool g_fDbgInDllEntryPoint = false;
bool g_fAutoRefreshLevels = false;

LPCTSTR pBaseKey = TEXT("SOFTWARE\\Microsoft\\DirectShow\\Debug");
LPCTSTR pGlobalKey = TEXT("GLOBAL");
static CHAR *pUnknownName = "UNKNOWN";

LPCTSTR TimeoutName = TEXT("TIMEOUT");

/* This sets the instance handle that the debug library uses to find
   the module's file name from the Win32 GetModuleFileName function */

void WINAPI DbgInitialise(HINSTANCE hInst)
{
    InitializeCriticalSection(&m_CSDebug);
    m_bInit = TRUE;

    m_hInst = hInst;
    DbgInitModuleName();
    if (GetProfileInt(m_ModuleName, TEXT("BreakOnLoad"), 0))
       DebugBreak();
    DbgInitModuleSettings(false);
    DbgInitGlobalSettings(true);
    dwTimeOffset = timeGetTime();
}


/* This is called to clear up any resources the debug library uses - at the
   moment we delete our critical section and the object list. The values we
   retrieve from the registry are all done during initialisation but we don't
   go looking for update notifications while we are running, if the values
   are changed then the application has to be restarted to pick them up */

void WINAPI DbgTerminate()
{
    if (m_hOutput != INVALID_HANDLE_VALUE) {
       EXECUTE_ASSERT(CloseHandle(m_hOutput));
       m_hOutput = INVALID_HANDLE_VALUE;
    }
    DeleteCriticalSection(&m_CSDebug);
    m_bInit = FALSE;
}


/* This is called by DbgInitLogLevels to read the debug settings
   for each logging category for this module from the registry */

void WINAPI DbgInitKeyLevels(HKEY hKey, bool fTakeMax)
{
    LONG lReturn;               // Create key return value
    LONG lKeyPos;               // Current key category
    DWORD dwKeySize;            // Size of the key value
    DWORD dwKeyType;            // Receives it's type
    DWORD dwKeyValue;           // This fields value

    /* Try and read a value for each key position in turn */
    for (lKeyPos = 0;lKeyPos < iMAXLEVELS;lKeyPos++) {

        dwKeySize = sizeof(DWORD);
        lReturn = RegQueryValueEx(
            hKey,                       // Handle to an open key
            pKeyNames[lKeyPos],         // Subkey name derivation
            NULL,                       // Reserved field
            &dwKeyType,                 // Returns the field type
            (LPBYTE) &dwKeyValue,       // Returns the field's value
            &dwKeySize );               // Number of bytes transferred

        /* If either the key was not available or it was not a DWORD value
           then we ensure only the high priority debug logging is output
           but we try and update the field to a zero filled DWORD value */

        if (lReturn != ERROR_SUCCESS || dwKeyType != REG_DWORD)  {

            dwKeyValue = 0;
            lReturn = RegSetValueEx(
                hKey,                   // Handle of an open key
                pKeyNames[lKeyPos],     // Address of subkey name
                (DWORD) 0,              // Reserved field
                REG_DWORD,              // Type of the key field
                (PBYTE) &dwKeyValue,    // Value for the field
                sizeof(DWORD));         // Size of the field buffer

            if (lReturn != ERROR_SUCCESS) {
                DbgLog((LOG_ERROR,1,TEXT("Could not create subkey %s"),pKeyNames[lKeyPos]));
                dwKeyValue = 0;
            }
        }
        if(fTakeMax)
        {
            m_Levels[lKeyPos] = max(dwKeyValue,m_Levels[lKeyPos]);
        }
        else
        {
            if((m_Levels[lKeyPos] & LOG_FORCIBLY_SET) == 0) {
                m_Levels[lKeyPos] = dwKeyValue;
            }
        }
    }

    /*  Read the timeout value for catching hangs */
    dwKeySize = sizeof(DWORD);
    lReturn = RegQueryValueEx(
        hKey,                       // Handle to an open key
        TimeoutName,                // Subkey name derivation
        NULL,                       // Reserved field
        &dwKeyType,                 // Returns the field type
        (LPBYTE) &dwWaitTimeout,    // Returns the field's value
        &dwKeySize );               // Number of bytes transferred

    /* If either the key was not available or it was not a DWORD value
       then we ensure only the high priority debug logging is output
       but we try and update the field to a zero filled DWORD value */

    if (lReturn != ERROR_SUCCESS || dwKeyType != REG_DWORD)  {

        dwWaitTimeout = INFINITE;
        lReturn = RegSetValueEx(
            hKey,                   // Handle of an open key
            TimeoutName,            // Address of subkey name
            (DWORD) 0,              // Reserved field
            REG_DWORD,              // Type of the key field
            (PBYTE) &dwWaitTimeout, // Value for the field
            sizeof(DWORD));         // Size of the field buffer

        if (lReturn != ERROR_SUCCESS) {
            DbgLog((LOG_ERROR,1,TEXT("Could not create subkey %s"),pKeyNames[lKeyPos]));
            dwWaitTimeout = INFINITE;
        }
    }
}

void WINAPI DbgOutString(LPCTSTR psz)
{
    if (m_hOutput != INVALID_HANDLE_VALUE) {
        UINT  cb = lstrlen(psz);
        DWORD dw;
#ifdef UNICODE
        CHAR szDest[2048];
        WideCharToMultiByte(CP_ACP, 0, psz, -1, szDest, NUMELMS(szDest), 0, 0);
        WriteFile (m_hOutput, szDest, cb, &dw, NULL);
#else
        WriteFile (m_hOutput, psz, cb, &dw, NULL);
#endif
    } else {
        OutputDebugString (psz);
    }
}




HRESULT  DbgUniqueProcessName(LPCTSTR inName, LPTSTR outName)
{
    HRESULT hr = S_OK;
    const TCHAR *pIn = inName;
    int dotPos = -1;

    //scan the input and record the last '.' position
    while (*pIn && (pIn - inName) < MAX_PATH)
    {
        if ( TEXT('.') == *pIn )
            dotPos = (int)(pIn-inName);
        ++pIn;
    }

    if (*pIn) //input should be zero-terminated within MAX_PATH
        return E_INVALIDARG;

    DWORD dwProcessId = GetCurrentProcessId();

    if (dotPos < 0) 
    {
        //no extension in the input, appending process id to the input
        hr = StringCchPrintf(outName, MAX_PATH, TEXT("%s_%d"), inName, dwProcessId);
    }
    else
    {
        TCHAR pathAndBasename[MAX_PATH] = {0};
        
        //there's an extension  - zero-terminate the path and basename first by copying
        hr = StringCchCopyN(pathAndBasename, MAX_PATH, inName, (size_t)dotPos);

        //re-combine path, basename and extension with processId appended to a basename
        if (SUCCEEDED(hr))
            hr = StringCchPrintf(outName, MAX_PATH, TEXT("%s_%d%s"), pathAndBasename, dwProcessId, inName + dotPos);
    }

    return hr;
}


/* Called by DbgInitGlobalSettings to setup alternate logging destinations
 */

void WINAPI DbgInitLogTo (
    HKEY hKey)
{
    LONG  lReturn;
    DWORD dwKeyType;
    DWORD dwKeySize;
    TCHAR szFile[MAX_PATH] = {0};
    static const TCHAR cszKey[] = TEXT("LogToFile");

    dwKeySize = MAX_PATH;
    lReturn = RegQueryValueEx(
        hKey,                       // Handle to an open key
        cszKey,                     // Subkey name derivation
        NULL,                       // Reserved field
        &dwKeyType,                 // Returns the field type
        (LPBYTE) szFile,            // Returns the field's value
        &dwKeySize);                // Number of bytes transferred

    // create an empty key if it does not already exist
    //
    if (lReturn != ERROR_SUCCESS || dwKeyType != REG_SZ)
       {
       dwKeySize = sizeof(TCHAR);
       lReturn = RegSetValueEx(
            hKey,                   // Handle of an open key
            cszKey,                 // Address of subkey name
            (DWORD) 0,              // Reserved field
            REG_SZ,                 // Type of the key field
            (PBYTE)szFile,          // Value for the field
            dwKeySize);            // Size of the field buffer
       }

    // if an output-to was specified.  try to open it.
    //
    if (m_hOutput != INVALID_HANDLE_VALUE) {
       EXECUTE_ASSERT(CloseHandle (m_hOutput));
       m_hOutput = INVALID_HANDLE_VALUE;
    }
    if (szFile[0] != 0)
       {
       if (!lstrcmpi(szFile, TEXT("Console"))) {
          m_hOutput = GetStdHandle (STD_OUTPUT_HANDLE);
          if (m_hOutput == INVALID_HANDLE_VALUE) {
             AllocConsole ();
             m_hOutput = GetStdHandle (STD_OUTPUT_HANDLE);
          }
          SetConsoleTitle (TEXT("ActiveX Debug Output"));
       } else if (szFile[0] &&
                lstrcmpi(szFile, TEXT("Debug")) &&
                lstrcmpi(szFile, TEXT("Debugger")) &&
                lstrcmpi(szFile, TEXT("Deb")))
          {
            m_hOutput = CreateFile(szFile, GENERIC_WRITE,
                                 FILE_SHARE_READ,
                                 NULL, OPEN_ALWAYS,
                                 FILE_ATTRIBUTE_NORMAL,
                                 NULL);

            if (INVALID_HANDLE_VALUE == m_hOutput &&
                GetLastError() == ERROR_SHARING_VIOLATION)
            {
               TCHAR uniqueName[MAX_PATH] = {0};
               if (SUCCEEDED(DbgUniqueProcessName(szFile, uniqueName)))
               {
                    m_hOutput = CreateFile(uniqueName, GENERIC_WRITE,
                                         FILE_SHARE_READ,
                                         NULL, OPEN_ALWAYS,
                                         FILE_ATTRIBUTE_NORMAL,
                                         NULL);
               }
            }
               
            if (INVALID_HANDLE_VALUE != m_hOutput)
            {
              static const TCHAR cszBar[] = TEXT("\r\n\r\n=====DbgInitialize()=====\r\n\r\n");
              SetFilePointer (m_hOutput, 0, NULL, FILE_END);
              DbgOutString (cszBar);
            }
          }
       }
}



/* This is called by DbgInitLogLevels to read the global debug settings for
   each logging category for this module from the registry. Normally each
   module has it's own values set for it's different debug categories but
   setting the global SOFTWARE\Debug\Global applies them to ALL modules */

void WINAPI DbgInitGlobalSettings(bool fTakeMax)
{
    LONG lReturn;               // Create key return value
    TCHAR szInfo[iDEBUGINFO];   // Constructs key names
    HKEY hGlobalKey;            // Global override key

    /* Construct the global base key name */
    (void)StringCchPrintf(szInfo,NUMELMS(szInfo),TEXT("%s\\%s"),pBaseKey,pGlobalKey);

    /* Create or open the key for this module */
    lReturn = RegCreateKeyEx(HKEY_LOCAL_MACHINE,   // Handle of an open key
                             szInfo,               // Address of subkey name
                             (DWORD) 0,            // Reserved value
                             NULL,                 // Address of class name
                             (DWORD) 0,            // Special options flags
                             GENERIC_READ | GENERIC_WRITE,   // Desired security access
                             NULL,                 // Key security descriptor
                             &hGlobalKey,          // Opened handle buffer
                             NULL);                // What really happened

    if (lReturn != ERROR_SUCCESS) {
        lReturn = RegCreateKeyEx(HKEY_LOCAL_MACHINE,   // Handle of an open key
                                 szInfo,               // Address of subkey name
                                 (DWORD) 0,            // Reserved value
                                 NULL,                 // Address of class name
                                 (DWORD) 0,            // Special options flags
                                 GENERIC_READ,         // Desired security access
                                 NULL,                 // Key security descriptor
                                 &hGlobalKey,          // Opened handle buffer
                                 NULL);                // What really happened
        if (lReturn != ERROR_SUCCESS) {
            DbgLog((LOG_ERROR,1,TEXT("Could not access GLOBAL module key")));
            // CPlusSharp: in the original file, this "return" was after this if statement
            // => because HKEY_LOCAL_MACHINE is readonly since Vista, if we start without admin-rights
            // the Log-Levels would not be loaded!
            // I don't know if this was on purpose, but then the line above make no seens!
            return; 
        }
    }

    DbgInitKeyLevels(hGlobalKey, fTakeMax);
    RegCloseKey(hGlobalKey);
}


/* This sets the debugging log levels for the different categories. We start
   by opening (or creating if not already available) the SOFTWARE\Debug key
   that all these settings live under. We then look at the global values
   set under SOFTWARE\Debug\Global which apply on top of the individual
   module settings. We then load the individual module registry settings */

void WINAPI DbgInitModuleSettings(bool fTakeMax)
{
    LONG lReturn;               // Create key return value
    TCHAR szInfo[iDEBUGINFO];   // Constructs key names
    HKEY hModuleKey;            // Module key handle

    /* Construct the base key name */
    (void)StringCchPrintf(szInfo,NUMELMS(szInfo),TEXT("%s\\%s"),pBaseKey,m_ModuleName);

    /* Create or open the key for this module */
    lReturn = RegCreateKeyEx(HKEY_LOCAL_MACHINE,   // Handle of an open key
                             szInfo,               // Address of subkey name
                             (DWORD) 0,            // Reserved value
                             NULL,                 // Address of class name
                             (DWORD) 0,            // Special options flags
                             GENERIC_READ | GENERIC_WRITE, // Desired security access
                             NULL,                 // Key security descriptor
                             &hModuleKey,          // Opened handle buffer
                             NULL);                // What really happened

    if (lReturn != ERROR_SUCCESS) {
        lReturn = RegCreateKeyEx(HKEY_LOCAL_MACHINE,   // Handle of an open key
                                 szInfo,               // Address of subkey name
                                 (DWORD) 0,            // Reserved value
                                 NULL,                 // Address of class name
                                 (DWORD) 0,            // Special options flags
                                 GENERIC_READ,         // Desired security access
                                 NULL,                 // Key security descriptor
                                 &hModuleKey,          // Opened handle buffer
                                 NULL);                // What really happened
        if (lReturn != ERROR_SUCCESS) {
            DbgLog((LOG_ERROR,1,TEXT("Could not access module key")));
            // CPlusSharp: in the original file, this "return" was after this if statement
            // => because HKEY_LOCAL_MACHINE is readonly since Vista, if we start without admin-rights
            // the Log-Levels would not be loaded!
            // I don't know if this was on purpose, but then the line above make no seens!
            return;
        }
    }

    DbgInitLogTo(hModuleKey);
    DbgInitKeyLevels(hModuleKey, fTakeMax);
    RegCloseKey(hModuleKey);
}


/* Initialise the module file name */

void WINAPI DbgInitModuleName()
{
    TCHAR FullName[iDEBUGINFO];     // Load the full path and module name
    LPTSTR pName;                   // Searches from the end for a backslash

    GetModuleFileName(m_hInst,FullName,iDEBUGINFO);
    pName = _tcsrchr(FullName,'\\');
    if (pName == NULL) {
        pName = FullName;
    } else {
        pName++;
    }
    (void)StringCchCopy(m_ModuleName,NUMELMS(m_ModuleName), pName);
}

struct MsgBoxMsg
{
    HWND hwnd;
    LPCTSTR szTitle;
    LPCTSTR szMessage;
    DWORD dwFlags;
    INT iResult;
};

//
// create a thread to call MessageBox(). calling MessageBox() on
// random threads at bad times can confuse the host (eg IE).
//
DWORD WINAPI MsgBoxThread(
  __inout LPVOID lpParameter   // thread data
  )
{
    MsgBoxMsg *pmsg = (MsgBoxMsg *)lpParameter;
    pmsg->iResult = MessageBox(
        pmsg->hwnd,
        pmsg->szTitle,
        pmsg->szMessage,
        pmsg->dwFlags);

    return 0;
}

INT MessageBoxOtherThread(
    HWND hwnd,
    LPCTSTR szTitle,
    LPCTSTR szMessage,
    DWORD dwFlags)
{
    if(g_fDbgInDllEntryPoint)
    {
        // can't wait on another thread because we have the loader
        // lock held in the dll entry point.
        // This can crash sometimes so just skip it
        // return MessageBox(hwnd, szTitle, szMessage, dwFlags);
        return IDCANCEL;
    }
    else
    {
        MsgBoxMsg msg = {hwnd, szTitle, szMessage, dwFlags, 0};
        DWORD dwid;
        HANDLE hThread = CreateThread(
            0,                      // security
            0,                      // stack size
            MsgBoxThread,
            (void *)&msg,           // arg
            0,                      // flags
            &dwid);
        if(hThread)
        {
            WaitForSingleObject(hThread, INFINITE);
            CloseHandle(hThread);
            return msg.iResult;
        }

        // break into debugger on failure.
        return IDCANCEL;
    }
}

/* Displays a message box if the condition evaluated to FALSE */

void WINAPI DbgAssert(LPCTSTR pCondition,LPCTSTR pFileName,INT iLine)
{
    if(g_fUseKASSERT)
    {
        DbgKernelAssert(pCondition, pFileName, iLine);
    }
    else
    {

        TCHAR szInfo[iDEBUGINFO];

        (void)StringCchPrintf(szInfo, NUMELMS(szInfo),TEXT("%s \nAt line %d of %s\nContinue? (Cancel to debug)"),
                 pCondition, iLine, pFileName);

        INT MsgId = MessageBoxOtherThread(NULL,szInfo,TEXT("ASSERT Failed"),
                                          MB_SYSTEMMODAL |
                                          MB_ICONHAND |
                                          MB_YESNOCANCEL |
                                          MB_SETFOREGROUND);
        switch (MsgId)
        {
          case IDNO:              /* Kill the application */

              FatalAppExit(FALSE, TEXT("Application terminated"));
              break;

          case IDCANCEL:          /* Break into the debugger */

              DebugBreak();
              break;

          case IDYES:             /* Ignore assertion continue execution */
              break;
        }
    }
}

/* Displays a message box at a break point */

void WINAPI DbgBreakPoint(LPCTSTR pCondition,LPCTSTR pFileName,INT iLine)
{
    if(g_fUseKASSERT)
    {
        DbgKernelAssert(pCondition, pFileName, iLine);
    }
    else
    {
        TCHAR szInfo[iDEBUGINFO];

        (void)StringCchPrintf(szInfo, NUMELMS(szInfo),TEXT("%s \nAt line %d of %s\nContinue? (Cancel to debug)"),
                 pCondition, iLine, pFileName);

        INT MsgId = MessageBoxOtherThread(NULL,szInfo,TEXT("Hard coded break point"),
                                          MB_SYSTEMMODAL |
                                          MB_ICONHAND |
                                          MB_YESNOCANCEL |
                                          MB_SETFOREGROUND);
        switch (MsgId)
        {
          case IDNO:              /* Kill the application */

              FatalAppExit(FALSE, TEXT("Application terminated"));
              break;

          case IDCANCEL:          /* Break into the debugger */

              DebugBreak();
              break;

          case IDYES:             /* Ignore break point continue execution */
              break;
        }
    }
}

void WINAPI DbgBreakPoint(LPCTSTR pFileName,INT iLine,__format_string LPCTSTR szFormatString,...)
{
    // A debug break point message can have at most 2000 characters if
    // ANSI or UNICODE characters are being used.  A debug break point message
    // can have between 1000 and 2000 double byte characters in it.  If a
    // particular message needs more characters, then the value of this constant
    // should be increased.
    const DWORD MAX_BREAK_POINT_MESSAGE_SIZE = 2000;

    TCHAR szBreakPointMessage[MAX_BREAK_POINT_MESSAGE_SIZE];

    va_list va;
    va_start( va, szFormatString );

    HRESULT hr = StringCchVPrintf( szBreakPointMessage, NUMELMS(szBreakPointMessage), szFormatString, va );

    va_end(va);

    if( FAILED(hr) ) {
        DbgBreak( "ERROR in DbgBreakPoint().  The variable length debug message could not be displayed because StringCchVPrintf() failed." );
        return;
    }

    ::DbgBreakPoint( szBreakPointMessage, pFileName, iLine );
}


/* When we initialised the library we stored in the m_Levels array the current
   debug output level for this module for each of the five categories. When
   some debug logging is sent to us it can be sent with a combination of the
   categories (if it is applicable to many for example) in which case we map
   the type's categories into their current debug levels and see if any of
   them can be accepted. The function looks at each bit position in turn from
   the input type field and then compares it's debug level with the modules.

   A level of 0 means that output is always sent to the debugger.  This is
   due to producing output if the input level is <= m_Levels.
*/


BOOL WINAPI DbgCheckModuleLevel(DWORD Type,DWORD Level)
{
    if(g_fAutoRefreshLevels)
    {
        // re-read the registry every second. We cannot use RegNotify() to
        // notice registry changes because it's not available on win9x.
        static DWORD g_dwLastRefresh = 0;
        DWORD dwTime = timeGetTime();
        if(dwTime - g_dwLastRefresh > 1000) {
            g_dwLastRefresh = dwTime;

            // there's a race condition: multiple threads could update the
            // values. plus read and write not synchronized. no harm
            // though.
            DbgInitModuleSettings(false);
        }
    }


    DWORD Mask = 0x01;

    // If no valid bits are set return FALSE
    if ((Type & ((1<<iMAXLEVELS)-1))) {

	// speed up unconditional output.
	if (0==Level)
	    return(TRUE);
	
        for (LONG lKeyPos = 0;lKeyPos < iMAXLEVELS;lKeyPos++) {
            if (Type & Mask) {
                if (Level <= (m_Levels[lKeyPos] & ~LOG_FORCIBLY_SET)) {
                    return TRUE;
                }
            }
            Mask <<= 1;
        }
    }
    return FALSE;
}


/* Set debug levels to a given value */

void WINAPI DbgSetModuleLevel(DWORD Type, DWORD Level)
{
    DWORD Mask = 0x01;

    for (LONG lKeyPos = 0;lKeyPos < iMAXLEVELS;lKeyPos++) {
        if (Type & Mask) {
            m_Levels[lKeyPos] = Level | LOG_FORCIBLY_SET;
        }
        Mask <<= 1;
    }
}

/* whether to check registry values periodically. this isn't turned
   automatically because of the potential performance hit. */
void WINAPI DbgSetAutoRefreshLevels(bool fAuto)
{
    g_fAutoRefreshLevels = fAuto;
}

#ifdef UNICODE
//
// warning -- this function is implemented twice for ansi applications
// linking to the unicode library
//
void WINAPI DbgLogInfo(DWORD Type,DWORD Level,__format_string LPCSTR pFormat,...)
{
    /* Check the current level for this type combination */

    BOOL bAccept = DbgCheckModuleLevel(Type,Level);
    if (bAccept == FALSE) {
        return;
    }

    TCHAR szInfo[2000];

    /* Format the variable length parameter list */

    va_list va;
    va_start(va, pFormat);

    (void)StringCchPrintf(szInfo, NUMELMS(szInfo),
             TEXT("%s(tid %x) %8d : "),
             m_ModuleName,
             GetCurrentThreadId(), timeGetTime() - dwTimeOffset);

    CHAR szInfoA[2000];
    WideCharToMultiByte(CP_ACP, 0, szInfo, -1, szInfoA, NUMELMS(szInfoA), 0, 0);

    (void)StringCchVPrintfA(szInfoA + lstrlenA(szInfoA), NUMELMS(szInfoA) - lstrlenA(szInfoA), pFormat, va);
    (void)StringCchCatA(szInfoA, NUMELMS(szInfoA), "\r\n");

    WCHAR wszOutString[2000];
    MultiByteToWideChar(CP_ACP, 0, szInfoA, -1, wszOutString, NUMELMS(wszOutString));
    DbgOutString(wszOutString);

    va_end(va);
}

void WINAPI DbgAssert(LPCSTR pCondition,LPCSTR pFileName,INT iLine)
{
    if(g_fUseKASSERT)
    {
        DbgKernelAssert(pCondition, pFileName, iLine);
    }
    else
    {

        TCHAR szInfo[iDEBUGINFO];

        (void)StringCchPrintf(szInfo, NUMELMS(szInfo), TEXT("%hs \nAt line %d of %hs\nContinue? (Cancel to debug)"),
                 pCondition, iLine, pFileName);

        INT MsgId = MessageBoxOtherThread(NULL,szInfo,TEXT("ASSERT Failed"),
                                          MB_SYSTEMMODAL |
                                          MB_ICONHAND |
                                          MB_YESNOCANCEL |
                                          MB_SETFOREGROUND);
        switch (MsgId)
        {
          case IDNO:              /* Kill the application */

              FatalAppExit(FALSE, TEXT("Application terminated"));
              break;

          case IDCANCEL:          /* Break into the debugger */

              DebugBreak();
              break;

          case IDYES:             /* Ignore assertion continue execution */
              break;
        }
    }
}

/* Displays a message box at a break point */

void WINAPI DbgBreakPoint(LPCSTR pCondition,LPCSTR pFileName,INT iLine)
{
    if(g_fUseKASSERT)
    {
        DbgKernelAssert(pCondition, pFileName, iLine);
    }
    else
    {
        TCHAR szInfo[iDEBUGINFO];

        (void)StringCchPrintf(szInfo, NUMELMS(szInfo),TEXT("%hs \nAt line %d of %hs\nContinue? (Cancel to debug)"),
                 pCondition, iLine, pFileName);

        INT MsgId = MessageBoxOtherThread(NULL,szInfo,TEXT("Hard coded break point"),
                                          MB_SYSTEMMODAL |
                                          MB_ICONHAND |
                                          MB_YESNOCANCEL |
                                          MB_SETFOREGROUND);
        switch (MsgId)
        {
          case IDNO:              /* Kill the application */

              FatalAppExit(FALSE, TEXT("Application terminated"));
              break;

          case IDCANCEL:          /* Break into the debugger */

              DebugBreak();
              break;

          case IDYES:             /* Ignore break point continue execution */
              break;
        }
    }
}

void WINAPI DbgKernelAssert(LPCSTR pCondition,LPCSTR pFileName,INT iLine)
{
    DbgLog((LOG_ERROR,0,TEXT("Assertion FAILED (%hs) at line %d in file %hs"),
           pCondition, iLine, pFileName));
    DebugBreak();
}

#endif

/* Print a formatted string to the debugger prefixed with this module's name
   Because the COMBASE classes are linked statically every module loaded will
   have their own copy of this code. It therefore helps if the module name is
   included on the output so that the offending code can be easily found */

//
// warning -- this function is implemented twice for ansi applications
// linking to the unicode library
//
void WINAPI DbgLogInfo(DWORD Type,DWORD Level,LPCTSTR pFormat,...)
{

    /* Check the current level for this type combination */

    BOOL bAccept = DbgCheckModuleLevel(Type,Level);
    if (bAccept == FALSE) {
        return;
    }

    TCHAR szInfo[2000];

    /* Format the variable length parameter list */

    va_list va;
    va_start(va, pFormat);

    (void)StringCchPrintf(szInfo, NUMELMS(szInfo),
             TEXT("%s(tid %x) %8d : "),
             m_ModuleName,
             GetCurrentThreadId(), timeGetTime() - dwTimeOffset);

    (void)StringCchVPrintf(szInfo + lstrlen(szInfo), NUMELMS(szInfo) - lstrlen(szInfo), pFormat, va);
    (void)StringCchCat(szInfo, NUMELMS(szInfo), TEXT("\r\n"));
    DbgOutString(szInfo);

    va_end(va);
}


/* If we are executing as a pure kernel filter we cannot display message
   boxes to the user, this provides an alternative which puts the error
   condition on the debugger output with a suitable eye catching message */

void WINAPI DbgKernelAssert(LPCTSTR pCondition,LPCTSTR pFileName,INT iLine)
{
    DbgLog((LOG_ERROR,0,TEXT("Assertion FAILED (%s) at line %d in file %s"),
           pCondition, iLine, pFileName));
    DebugBreak();
}



/* Each time we create an object derived from CBaseObject the constructor will
   call us to register the creation of the new object. We are passed a string
   description which we store away. We return a cookie that the constructor
   uses to identify the object when it is destroyed later on. We update the
   total number of active objects in the DLL mainly for debugging purposes */

DWORD WINAPI DbgRegisterObjectCreation(LPCSTR szObjectName,
                                       LPCWSTR wszObjectName)
{
    /* If this fires you have a mixed DEBUG/RETAIL build */

    ASSERT(!!szObjectName ^ !!wszObjectName);

    /* Create a place holder for this object description */

    ObjectDesc *pObject = new ObjectDesc;
    ASSERT(pObject);

    /* It is valid to pass a NULL object name */
    if (pObject == NULL) {
        return FALSE;
    }

    /* Check we have been initialised - we may not be initialised when we are
       being pulled in from an executable which has globally defined objects
       as they are created by the C++ run time before WinMain is called */

    if (m_bInit == FALSE) {
        DbgInitialise(GetModuleHandle(NULL));
    }

    /* Grab the list critical section */
    EnterCriticalSection(&m_CSDebug);

    /* If no name then default to UNKNOWN */
    if (!szObjectName && !wszObjectName) {
        szObjectName = pUnknownName;
    }

    /* Put the new description at the head of the list */

    pObject->m_szName = szObjectName;
    pObject->m_wszName = wszObjectName;
    pObject->m_dwCookie = ++m_dwNextCookie;
    pObject->m_pNext = pListHead;

    pListHead = pObject;
    m_dwObjectCount++;

    DWORD ObjectCookie = pObject->m_dwCookie;
    ASSERT(ObjectCookie);

    if(wszObjectName) {
        DbgLog((LOG_MEMORY,2,TEXT("Object created   %d (%ls) %d Active"),
                pObject->m_dwCookie, wszObjectName, m_dwObjectCount));
    } else {
        DbgLog((LOG_MEMORY,2,TEXT("Object created   %d (%hs) %d Active"),
                pObject->m_dwCookie, szObjectName, m_dwObjectCount));
    }

    LeaveCriticalSection(&m_CSDebug);
    return ObjectCookie;
}


/* This is called by the CBaseObject destructor when an object is about to be
   destroyed, we are passed the cookie we returned during construction that
   identifies this object. We scan the object list for a matching cookie and
   remove the object if successful. We also update the active object count */

BOOL WINAPI DbgRegisterObjectDestruction(DWORD dwCookie)
{
    /* Grab the list critical section */
    EnterCriticalSection(&m_CSDebug);

    ObjectDesc *pObject = pListHead;
    ObjectDesc *pPrevious = NULL;

    /* Scan the object list looking for a cookie match */

    while (pObject) {
        if (pObject->m_dwCookie == dwCookie) {
            break;
        }
        pPrevious = pObject;
        pObject = pObject->m_pNext;
    }

    if (pObject == NULL) {
        DbgBreak("Apparently destroying a bogus object");
        LeaveCriticalSection(&m_CSDebug);
        return FALSE;
    }

    /* Is the object at the head of the list */

    if (pPrevious == NULL) {
        pListHead = pObject->m_pNext;
    } else {
        pPrevious->m_pNext = pObject->m_pNext;
    }

    /* Delete the object and update the housekeeping information */

    m_dwObjectCount--;

    if(pObject->m_wszName) {
        DbgLog((LOG_MEMORY,2,TEXT("Object destroyed %d (%ls) %d Active"),
                pObject->m_dwCookie, pObject->m_wszName, m_dwObjectCount));
    } else {
        DbgLog((LOG_MEMORY,2,TEXT("Object destroyed %d (%hs) %d Active"),
                pObject->m_dwCookie, pObject->m_szName, m_dwObjectCount));
    }

    delete pObject;
    LeaveCriticalSection(&m_CSDebug);
    return TRUE;
}


/* This runs through the active object list displaying their details */

void WINAPI DbgDumpObjectRegister()
{
    TCHAR szInfo[iDEBUGINFO];

    /* Grab the list critical section */

    EnterCriticalSection(&m_CSDebug);
    ObjectDesc *pObject = pListHead;

    /* Scan the object list displaying the name and cookie */

    DbgLog((LOG_MEMORY,2,TEXT("")));
    DbgLog((LOG_MEMORY,2,TEXT("   ID             Object Description")));
    DbgLog((LOG_MEMORY,2,TEXT("")));

    while (pObject) {
        if(pObject->m_wszName) {
            (void)StringCchPrintf(szInfo,NUMELMS(szInfo),TEXT("%5d (%p) %30ls"),pObject->m_dwCookie, &pObject, pObject->m_wszName);
        } else {
            (void)StringCchPrintf(szInfo,NUMELMS(szInfo),TEXT("%5d (%p) %30hs"),pObject->m_dwCookie, &pObject, pObject->m_szName);
        }
        DbgLog((LOG_MEMORY,2,szInfo));
        pObject = pObject->m_pNext;
    }

    (void)StringCchPrintf(szInfo,NUMELMS(szInfo),TEXT("Total object count %5d"),m_dwObjectCount);
    DbgLog((LOG_MEMORY,2,TEXT("")));
    DbgLog((LOG_MEMORY,1,szInfo));
    LeaveCriticalSection(&m_CSDebug);
}

/*  Debug infinite wait stuff */
DWORD WINAPI DbgWaitForSingleObject(HANDLE h)
{
    DWORD dwWaitResult;
    do {
        dwWaitResult = WaitForSingleObject(h, dwWaitTimeout);
        ASSERT(dwWaitResult == WAIT_OBJECT_0);
    } while (dwWaitResult == WAIT_TIMEOUT);
    return dwWaitResult;
}
DWORD WINAPI DbgWaitForMultipleObjects(DWORD nCount,
                                __in_ecount(nCount) CONST HANDLE *lpHandles,
                                BOOL bWaitAll)
{
    DWORD dwWaitResult;
    do {
        dwWaitResult = WaitForMultipleObjects(nCount,
                                              lpHandles,
                                              bWaitAll,
                                              dwWaitTimeout);
        ASSERT((DWORD)(dwWaitResult - WAIT_OBJECT_0) < MAXIMUM_WAIT_OBJECTS);
    } while (dwWaitResult == WAIT_TIMEOUT);
    return dwWaitResult;
}

void WINAPI DbgSetWaitTimeout(DWORD dwTimeout)
{
    dwWaitTimeout = dwTimeout;
}

#endif /* DEBUG */

#ifdef _OBJBASE_H_

    /*  Stuff for printing out our GUID names */

    GUID_STRING_ENTRY g_GuidNames[] = {
    #define OUR_GUID_ENTRY(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    { #name, { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } } },
        #include <uuids.h>
    };

    CGuidNameList GuidNames;
    int g_cGuidNames = sizeof(g_GuidNames) / sizeof(g_GuidNames[0]);

    char *CGuidNameList::operator [] (const GUID &guid)
    {
        for (int i = 0; i < g_cGuidNames; i++) {
            if (g_GuidNames[i].guid == guid) {
                return g_GuidNames[i].szName;
            }
        }
        if (guid == GUID_NULL) {
            return "GUID_NULL";
        }

	// !!! add something to print FOURCC guids?
	
	// shouldn't this print the hex CLSID?
        return "Unknown GUID Name";
    }

#endif /* _OBJBASE_H_ */

/*  CDisp class - display our data types */

// clashes with REFERENCE_TIME
CDisp::CDisp(LONGLONG ll, int Format)
{
    // note: this could be combined with CDisp(LONGLONG) by
    // introducing a default format of CDISP_REFTIME
    LARGE_INTEGER li;
    li.QuadPart = ll;
    switch (Format) {
	case CDISP_DEC:
	{
	    TCHAR  temp[20];
	    int pos=20;
	    temp[--pos] = 0;
	    int digit;
	    // always output at least one digit
	    do {
		// Get the rightmost digit - we only need the low word
	        digit = li.LowPart % 10;
		li.QuadPart /= 10;
		temp[--pos] = (TCHAR) digit+L'0';
	    } while (li.QuadPart);
	    (void)StringCchCopy(m_String, NUMELMS(m_String), temp+pos);
	    break;
	}
	case CDISP_HEX:
	default:
	    (void)StringCchPrintf(m_String, NUMELMS(m_String), TEXT("0x%X%8.8X"), li.HighPart, li.LowPart);
    }
};

CDisp::CDisp(REFCLSID clsid)
{
#ifdef UNICODE 
    (void)StringFromGUID2(clsid, m_String, NUMELMS(m_String));
#else
    WCHAR wszTemp[50];
    (void)StringFromGUID2(clsid, wszTemp, NUMELMS(wszTemp));
    (void)StringCchPrintf(m_String, NUMELMS(m_String), TEXT("%S"), wszTemp);
#endif
};

#ifdef __STREAMS__
/*  Display stuff */
CDisp::CDisp(CRefTime llTime)
{
    LONGLONG llDiv;
    if (llTime < 0) {
        llTime = -llTime;
        (void)StringCchCopy(m_String, NUMELMS(m_String), TEXT("-"));
    }
    llDiv = (LONGLONG)24 * 3600 * 10000000;
    if (llTime >= llDiv) {
        (void)StringCchPrintf(m_String + lstrlen(m_String), NUMELMS(m_String) - lstrlen(m_String), TEXT("%d days "), (LONG)(llTime / llDiv));
        llTime = llTime % llDiv;
    }
    llDiv = (LONGLONG)3600 * 10000000;
    if (llTime >= llDiv) {
        (void)StringCchPrintf(m_String + lstrlen(m_String), NUMELMS(m_String) - lstrlen(m_String), TEXT("%d hrs "), (LONG)(llTime / llDiv));
        llTime = llTime % llDiv;
    }
    llDiv = (LONGLONG)60 * 10000000;
    if (llTime >= llDiv) {
        (void)StringCchPrintf(m_String + lstrlen(m_String), NUMELMS(m_String) - lstrlen(m_String), TEXT("%d mins "), (LONG)(llTime / llDiv));
        llTime = llTime % llDiv;
    }
    (void)StringCchPrintf(m_String + lstrlen(m_String), NUMELMS(m_String) - lstrlen(m_String), TEXT("%d.%3.3d sec"),
             (LONG)llTime / 10000000,
             (LONG)((llTime % 10000000) / 10000));
};

#endif // __STREAMS__


/*  Display pin */
CDisp::CDisp(IPin *pPin)
{
    PIN_INFO pi;
    TCHAR str[MAX_PIN_NAME];
    CLSID clsid;

    if (pPin) {
       pPin->QueryPinInfo(&pi);
       pi.pFilter->GetClassID(&clsid);
       QueryPinInfoReleaseFilter(pi);
      #ifndef UNICODE
       WideCharToMultiByte(GetACP(), 0, pi.achName, lstrlenW(pi.achName) + 1,
                           str, MAX_PIN_NAME, NULL, NULL);
      #else
       (void)StringCchCopy(str, NUMELMS(str), pi.achName);
      #endif
    } else {
       (void)StringCchCopy(str, NUMELMS(str), TEXT("NULL IPin"));
    }

    m_pString = (PTCHAR) new TCHAR[lstrlen(str)+64];
    if (!m_pString) {
	return;
    }

    (void)StringCchPrintf(m_pString, lstrlen(str) + 64, TEXT("%hs(%s)"), GuidNames[clsid], str);
}

/*  Display filter or pin */
CDisp::CDisp(IUnknown *pUnk)
{
    IBaseFilter *pf;
    HRESULT hr = pUnk->QueryInterface(IID_IBaseFilter, (void **)&pf);
    if(SUCCEEDED(hr))
    {
        FILTER_INFO fi;
        hr = pf->QueryFilterInfo(&fi);
        if(SUCCEEDED(hr))
        {
            QueryFilterInfoReleaseGraph(fi);

            size_t len = lstrlenW(fi.achName)  + 1;

            m_pString = new TCHAR[len];
            if(m_pString)
            {
#ifdef UNICODE
                (void)StringCchCopy(m_pString, len, fi.achName);
#else
                (void)StringCchPrintf(m_pString, len, "%S", fi.achName);
#endif
            }
        }

        pf->Release();

        return;
    }

    IPin *pp;
    hr = pUnk->QueryInterface(IID_IPin, (void **)&pp);
    if(SUCCEEDED(hr))
    {
        CDisp::CDisp(pp);
        pp->Release();
        return;
    }
}


CDisp::~CDisp()
{
}

CDispBasic::~CDispBasic()
{
    if (m_pString != m_String) {
	delete [] m_pString;
    }
}

CDisp::CDisp(double d)
{
    (void)StringCchPrintf(m_String, NUMELMS(m_String), TEXT("%d.%03d"), (int) d, (int) ((d - (int) d) * 1000));
}


/* If built for debug this will display the media type details. We convert the
   major and subtypes into strings and also ask the base classes for a string
   description of the subtype, so MEDIASUBTYPE_RGB565 becomes RGB 565 16 bit
   We also display the fields in the BITMAPINFOHEADER structure, this should
   succeed as we do not accept input types unless the format is big enough */

#ifdef DEBUG
void WINAPI DisplayType(LPCTSTR label, const AM_MEDIA_TYPE *pmtIn)
{

    /* Dump the GUID types and a short description */

    DbgLog((LOG_TRACE,5,TEXT("")));
    DbgLog((LOG_TRACE,2,TEXT("%s  M type %hs  S type %hs"), label,
	    GuidNames[pmtIn->majortype],
	    GuidNames[pmtIn->subtype]));
    DbgLog((LOG_TRACE,5,TEXT("Subtype description %s"),GetSubtypeName(&pmtIn->subtype)));

    /* Dump the generic media types */

    if (pmtIn->bTemporalCompression) {
        DbgLog((LOG_TRACE,5,TEXT("Temporally compressed")));
    } else {
        DbgLog((LOG_TRACE,5,TEXT("Not temporally compressed")));
    }

    if (pmtIn->bFixedSizeSamples) {
        DbgLog((LOG_TRACE,5,TEXT("Sample size %d"),pmtIn->lSampleSize));
    } else {
        DbgLog((LOG_TRACE,5,TEXT("Variable size samples")));
    }

    if (pmtIn->formattype == FORMAT_VideoInfo) {

        VIDEOINFOHEADER *pVideoInfo = (VIDEOINFOHEADER *)pmtIn->pbFormat;

        DisplayRECT(TEXT("Source rectangle"),pVideoInfo->rcSource);
        DisplayRECT(TEXT("Target rectangle"),pVideoInfo->rcTarget);
        DisplayBITMAPINFO(HEADER(pmtIn->pbFormat));

    } if (pmtIn->formattype == FORMAT_VideoInfo2) {

        VIDEOINFOHEADER2 *pVideoInfo2 = (VIDEOINFOHEADER2 *)pmtIn->pbFormat;

        DisplayRECT(TEXT("Source rectangle"),pVideoInfo2->rcSource);
        DisplayRECT(TEXT("Target rectangle"),pVideoInfo2->rcTarget);
        DbgLog((LOG_TRACE, 5, TEXT("Aspect Ratio: %d:%d"),
            pVideoInfo2->dwPictAspectRatioX,
            pVideoInfo2->dwPictAspectRatioY));
        DisplayBITMAPINFO(&pVideoInfo2->bmiHeader);

    } else if (pmtIn->majortype == MEDIATYPE_Audio) {
        DbgLog((LOG_TRACE,2,TEXT("     Format type %hs"),
            GuidNames[pmtIn->formattype]));
        DbgLog((LOG_TRACE,2,TEXT("     Subtype %hs"),
            GuidNames[pmtIn->subtype]));

        if ((pmtIn->subtype != MEDIASUBTYPE_MPEG1Packet)
          && (pmtIn->cbFormat >= sizeof(PCMWAVEFORMAT)))
        {
            /* Dump the contents of the WAVEFORMATEX type-specific format structure */

            WAVEFORMATEX *pwfx = (WAVEFORMATEX *) pmtIn->pbFormat;
            DbgLog((LOG_TRACE,2,TEXT("wFormatTag %u"), pwfx->wFormatTag));
            DbgLog((LOG_TRACE,2,TEXT("nChannels %u"), pwfx->nChannels));
            DbgLog((LOG_TRACE,2,TEXT("nSamplesPerSec %lu"), pwfx->nSamplesPerSec));
            DbgLog((LOG_TRACE,2,TEXT("nAvgBytesPerSec %lu"), pwfx->nAvgBytesPerSec));
            DbgLog((LOG_TRACE,2,TEXT("nBlockAlign %u"), pwfx->nBlockAlign));
            DbgLog((LOG_TRACE,2,TEXT("wBitsPerSample %u"), pwfx->wBitsPerSample));

            /* PCM uses a WAVEFORMAT and does not have the extra size field */

            if (pmtIn->cbFormat >= sizeof(WAVEFORMATEX)) {
                DbgLog((LOG_TRACE,2,TEXT("cbSize %u"), pwfx->cbSize));
            }
        } else {
        }

    } else {
        DbgLog((LOG_TRACE,2,TEXT("     Format type %hs"),
            GuidNames[pmtIn->formattype]));
    }
}


void DisplayBITMAPINFO(const BITMAPINFOHEADER* pbmi)
{
    DbgLog((LOG_TRACE,5,TEXT("Size of BITMAPINFO structure %d"),pbmi->biSize));
    if (pbmi->biCompression < 256) {
        DbgLog((LOG_TRACE,2,TEXT("%dx%dx%d bit  (%d)"),
                pbmi->biWidth, pbmi->biHeight,
                pbmi->biBitCount, pbmi->biCompression));
    } else {
        DbgLog((LOG_TRACE,2,TEXT("%dx%dx%d bit '%4.4hs'"),
                pbmi->biWidth, pbmi->biHeight,
                pbmi->biBitCount, &pbmi->biCompression));
    }

    DbgLog((LOG_TRACE,2,TEXT("Image size %d"),pbmi->biSizeImage));
    DbgLog((LOG_TRACE,5,TEXT("Planes %d"),pbmi->biPlanes));
    DbgLog((LOG_TRACE,5,TEXT("X Pels per metre %d"),pbmi->biXPelsPerMeter));
    DbgLog((LOG_TRACE,5,TEXT("Y Pels per metre %d"),pbmi->biYPelsPerMeter));
    DbgLog((LOG_TRACE,5,TEXT("Colours used %d"),pbmi->biClrUsed));
}


void DisplayRECT(LPCTSTR szLabel, const RECT& rc)
{
    DbgLog((LOG_TRACE,5,TEXT("%s (Left %d Top %d Right %d Bottom %d)"),
            szLabel,
            rc.left,
            rc.top,
            rc.right,
            rc.bottom));
}


void WINAPI DumpGraph(IFilterGraph *pGraph, DWORD dwLevel)
{
    if( !pGraph )
    {
        return;
    }

    IEnumFilters *pFilters;

    DbgLog((LOG_TRACE,dwLevel,TEXT("DumpGraph [%x]"), pGraph));

    if (FAILED(pGraph->EnumFilters(&pFilters))) {
	DbgLog((LOG_TRACE,dwLevel,TEXT("EnumFilters failed!")));
    }

    IBaseFilter *pFilter;
    ULONG	n;
    while (pFilters->Next(1, &pFilter, &n) == S_OK) {
	FILTER_INFO	info;

	if (FAILED(pFilter->QueryFilterInfo(&info))) {
	    DbgLog((LOG_TRACE,dwLevel,TEXT("    Filter [%p]  -- failed QueryFilterInfo"), pFilter));
	} else {
	    QueryFilterInfoReleaseGraph(info);

	    // !!! should QueryVendorInfo here!
	
	    DbgLog((LOG_TRACE,dwLevel,TEXT("    Filter [%p]  '%ls'"), pFilter, info.achName));

	    IEnumPins *pins;

	    if (FAILED(pFilter->EnumPins(&pins))) {
		DbgLog((LOG_TRACE,dwLevel,TEXT("EnumPins failed!")));
	    } else {

		IPin *pPin;
		while (pins->Next(1, &pPin, &n) == S_OK) {
		    PIN_INFO	pinInfo;

		    if (FAILED(pPin->QueryPinInfo(&pinInfo))) {
			DbgLog((LOG_TRACE,dwLevel,TEXT("          Pin [%x]  -- failed QueryPinInfo"), pPin));
		    } else {
			QueryPinInfoReleaseFilter(pinInfo);

			IPin *pPinConnected = NULL;

			HRESULT hr = pPin->ConnectedTo(&pPinConnected);

			if (pPinConnected) {
			    DbgLog((LOG_TRACE,dwLevel,TEXT("          Pin [%p]  '%ls' [%sput]")
							   TEXT("  Connected to pin [%p]"),
				    pPin, pinInfo.achName,
				    pinInfo.dir == PINDIR_INPUT ? TEXT("In") : TEXT("Out"),
				    pPinConnected));

			    pPinConnected->Release();

			    // perhaps we should really dump the type both ways as a sanity
			    // check?
			    if (pinInfo.dir == PINDIR_OUTPUT) {
				AM_MEDIA_TYPE mt;

				hr = pPin->ConnectionMediaType(&mt);

				if (SUCCEEDED(hr)) {
				    DisplayType(TEXT("Connection type"), &mt);

				    FreeMediaType(mt);
				}
			    }
			} else {
			    DbgLog((LOG_TRACE,dwLevel,
				    TEXT("          Pin [%x]  '%ls' [%sput]"),
				    pPin, pinInfo.achName,
				    pinInfo.dir == PINDIR_INPUT ? TEXT("In") : TEXT("Out")));

			}
		    }

		    pPin->Release();

		}

		pins->Release();
	    }

	}
	
	pFilter->Release();
    }

    pFilters->Release();

}

#endif

//------------------------------------------------------------------------------
// File: AMVideo.cpp
//
// Desc: DirectShow base classes - implements helper functions for
//       bitmap formats.
//
// Copyright (c) 1992-2001 Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------

// These are bit field masks for true colour devices

const DWORD bits555[] = {0x007C00,0x0003E0,0x00001F};
const DWORD bits565[] = {0x00F800,0x0007E0,0x00001F};
const DWORD bits888[] = {0xFF0000,0x00FF00,0x0000FF};

// This maps bitmap subtypes into a bits per pixel value and also a
// name. unicode and ansi versions are stored because we have to
// return a pointer to a static string.
const struct {
    const GUID *pSubtype;
    WORD BitCount;
    CHAR *pName;
    WCHAR *wszName;
} BitCountMap[] =  { &MEDIASUBTYPE_RGB1,        1,   "RGB Monochrome",     L"RGB Monochrome",   
                     &MEDIASUBTYPE_RGB4,        4,   "RGB VGA",            L"RGB VGA",          
                     &MEDIASUBTYPE_RGB8,        8,   "RGB 8",              L"RGB 8",            
                     &MEDIASUBTYPE_RGB565,      16,  "RGB 565 (16 bit)",   L"RGB 565 (16 bit)", 
                     &MEDIASUBTYPE_RGB555,      16,  "RGB 555 (16 bit)",   L"RGB 555 (16 bit)", 
                     &MEDIASUBTYPE_RGB24,       24,  "RGB 24",             L"RGB 24",           
                     &MEDIASUBTYPE_RGB32,       32,  "RGB 32",             L"RGB 32",
                     &MEDIASUBTYPE_ARGB32,    32,  "ARGB 32",             L"ARGB 32",
                     &MEDIASUBTYPE_Overlay,     0,   "Overlay",            L"Overlay",          
                     &GUID_NULL,                0,   "UNKNOWN",            L"UNKNOWN"           
};

// Return the size of the bitmap as defined by this header

STDAPI_(DWORD) GetBitmapSize(const BITMAPINFOHEADER *pHeader)
{
    return DIBSIZE(*pHeader);
}


// This is called if the header has a 16 bit colour depth and needs to work
// out the detailed type from the bit fields (either RGB 565 or RGB 555)

STDAPI_(const GUID) GetTrueColorType(const BITMAPINFOHEADER *pbmiHeader)
{
    BITMAPINFO *pbmInfo = (BITMAPINFO *) pbmiHeader;
    ASSERT(pbmiHeader->biBitCount == 16);

    // If its BI_RGB then it's RGB 555 by default

    if (pbmiHeader->biCompression == BI_RGB) {
        return MEDIASUBTYPE_RGB555;
    }

    // Compare the bit fields with RGB 555

    DWORD *pMask = (DWORD *) pbmInfo->bmiColors;
    if (pMask[0] == bits555[0]) {
        if (pMask[1] == bits555[1]) {
            if (pMask[2] == bits555[2]) {
                return MEDIASUBTYPE_RGB555;
            }
        }
    }

    // Compare the bit fields with RGB 565

    pMask = (DWORD *) pbmInfo->bmiColors;
    if (pMask[0] == bits565[0]) {
        if (pMask[1] == bits565[1]) {
            if (pMask[2] == bits565[2]) {
                return MEDIASUBTYPE_RGB565;
            }
        }
    }
    return GUID_NULL;
}


// Given a BITMAPINFOHEADER structure this returns the GUID sub type that is
// used to describe it in format negotiations. For example a video codec fills
// in the format block with a VIDEOINFO structure, it also fills in the major
// type with MEDIATYPE_VIDEO and the subtype with a GUID that matches the bit
// count, for example if it is an eight bit image then MEDIASUBTYPE_RGB8

STDAPI_(const GUID) GetBitmapSubtype(const BITMAPINFOHEADER *pbmiHeader)
{
    ASSERT(pbmiHeader);

    // If it's not RGB then create a GUID from the compression type

    if (pbmiHeader->biCompression != BI_RGB) {
        if (pbmiHeader->biCompression != BI_BITFIELDS) {
            FOURCCMap FourCCMap(pbmiHeader->biCompression);
            return (const GUID) FourCCMap;
        }
    }

    // Map the RGB DIB bit depth to a image GUID

    switch(pbmiHeader->biBitCount) {
        case 1    :   return MEDIASUBTYPE_RGB1;
        case 4    :   return MEDIASUBTYPE_RGB4;
        case 8    :   return MEDIASUBTYPE_RGB8;
        case 16   :   return GetTrueColorType(pbmiHeader);
        case 24   :   return MEDIASUBTYPE_RGB24;
        case 32   :   return MEDIASUBTYPE_RGB32;
    }
    return GUID_NULL;
}


// Given a video bitmap subtype we return the number of bits per pixel it uses
// We return a WORD bit count as thats what the BITMAPINFOHEADER uses. If the
// GUID subtype is not found in the table we return an invalid USHRT_MAX

STDAPI_(WORD) GetBitCount(const GUID *pSubtype)
{
    ASSERT(pSubtype);
    const GUID *pMediaSubtype;
    INT iPosition = 0;

    // Scan the mapping list seeing if the source GUID matches any known
    // bitmap subtypes, the list is terminated by a GUID_NULL entry

    while (TRUE) {
        pMediaSubtype = BitCountMap[iPosition].pSubtype;
        if (IsEqualGUID(*pMediaSubtype,GUID_NULL)) {
            return USHRT_MAX;
        }
        if (IsEqualGUID(*pMediaSubtype,*pSubtype)) {
            return BitCountMap[iPosition].BitCount;
        }
        iPosition++;
    }
}


// Given a bitmap subtype we return a description name that can be used for
// debug purposes. In a retail build this function still returns the names
// If the subtype isn't found in the lookup table we return string UNKNOWN

int LocateSubtype(const GUID *pSubtype)
{
    ASSERT(pSubtype);
    const GUID *pMediaSubtype;
    INT iPosition = 0;

    // Scan the mapping list seeing if the source GUID matches any known
    // bitmap subtypes, the list is terminated by a GUID_NULL entry

    while (TRUE) {
        pMediaSubtype = BitCountMap[iPosition].pSubtype;
        if (IsEqualGUID(*pMediaSubtype,*pSubtype) ||
            IsEqualGUID(*pMediaSubtype,GUID_NULL)
            )
        {
            break;
        }
        
        iPosition++;
    }

    return iPosition;
}



STDAPI_(WCHAR *) GetSubtypeNameW(const GUID *pSubtype)
{
    return BitCountMap[LocateSubtype(pSubtype)].wszName;
}

STDAPI_(CHAR *) GetSubtypeNameA(const GUID *pSubtype)
{
    return BitCountMap[LocateSubtype(pSubtype)].pName;
}

#ifndef GetSubtypeName
#error wxutil.h should have defined GetSubtypeName
#endif
#undef GetSubtypeName

// this is here for people that linked to it directly; most people
// would use the header file that picks the A or W version.
STDAPI_(CHAR *) GetSubtypeName(const GUID *pSubtype)
{
    return GetSubtypeNameA(pSubtype);
}


// The mechanism for describing a bitmap format is with the BITMAPINFOHEADER
// This is really messy to deal with because it invariably has fields that
// follow it holding bit fields, palettes and the rest. This function gives
// the number of bytes required to hold a VIDEOINFO that represents it. This
// count includes the prefix information (like the rcSource rectangle) the
// BITMAPINFOHEADER field, and any other colour information on the end.
//
// WARNING If you want to copy a BITMAPINFOHEADER into a VIDEOINFO always make
// sure that you use the HEADER macro because the BITMAPINFOHEADER field isn't
// right at the start of the VIDEOINFO (there are a number of other fields),
//
//     CopyMemory(HEADER(pVideoInfo),pbmi,sizeof(BITMAPINFOHEADER));
//

STDAPI_(LONG) GetBitmapFormatSize(const BITMAPINFOHEADER *pHeader)
{
    // Everyone has this to start with this  
    LONG Size = SIZE_PREHEADER + pHeader->biSize;

    ASSERT(pHeader->biSize >= sizeof(BITMAPINFOHEADER));
    
    // Does this format use a palette, if the number of colours actually used
    // is zero then it is set to the maximum that are allowed for that colour
    // depth (an example is 256 for eight bits). Truecolour formats may also
    // pass a palette with them in which case the used count is non zero

    // This would scare me.
    ASSERT(pHeader->biBitCount <= iPALETTE || pHeader->biClrUsed == 0);

    if (pHeader->biBitCount <= iPALETTE || pHeader->biClrUsed) {
        LONG Entries = (DWORD) 1 << pHeader->biBitCount;
        if (pHeader->biClrUsed) {
            Entries = pHeader->biClrUsed;
        }
        Size += Entries * sizeof(RGBQUAD);
    }

    // Truecolour formats may have a BI_BITFIELDS specifier for compression
    // type which means that room for three DWORDs should be allocated that
    // specify where in each pixel the RGB colour components may be found

    if (pHeader->biCompression == BI_BITFIELDS) {
        Size += SIZE_MASKS;
    }

    // A BITMAPINFO for a palettised image may also contain a palette map that
    // provides the information to map from a source palette to a destination
    // palette during a BitBlt for example, because this information is only
    // ever processed during drawing you don't normally store the palette map
    // nor have any way of knowing if it is present in the data structure

    return Size;
}


// Returns TRUE if the VIDEOINFO contains a palette

STDAPI_(BOOL) ContainsPalette(const VIDEOINFOHEADER *pVideoInfo)
{
    if (PALETTISED(pVideoInfo) == FALSE) {
        if (pVideoInfo->bmiHeader.biClrUsed == 0) {
            return FALSE;
        }
    }
    return TRUE;
}


// Return a pointer to the first entry in a palette

STDAPI_(const RGBQUAD *) GetBitmapPalette(const VIDEOINFOHEADER *pVideoInfo)
{
    if (pVideoInfo->bmiHeader.biCompression == BI_BITFIELDS) {
        return TRUECOLOR(pVideoInfo)->bmiColors;
    }
    return COLORS(pVideoInfo);
}

//------------------------------------------------------------------------------
// File: ComBase.cpp
//
// Desc: DirectShow base classes - implements class hierarchy for creating
//       COM objects.
//
// Copyright (c) 1992-2001 Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------

/* Define the static member variable */

LONG CBaseObject::m_cObjects = 0;


/* Constructor */

CBaseObject::CBaseObject(__in_opt LPCTSTR pName)
{
    /* Increment the number of active objects */
    InterlockedIncrement(&m_cObjects);

#ifdef DEBUG

#ifdef UNICODE
    m_dwCookie = DbgRegisterObjectCreation(0, pName);
#else
    m_dwCookie = DbgRegisterObjectCreation(pName, 0);
#endif

#endif
}

#ifdef UNICODE
CBaseObject::CBaseObject(const char *pName)
{
    /* Increment the number of active objects */
    InterlockedIncrement(&m_cObjects);

#ifdef DEBUG
    m_dwCookie = DbgRegisterObjectCreation(pName, 0);
#endif
}
#endif

HINSTANCE	hlibOLEAut32;

/* Destructor */

CBaseObject::~CBaseObject()
{
    /* Decrement the number of objects active */
    if (InterlockedDecrement(&m_cObjects) == 0) {
	if (hlibOLEAut32) {
	    FreeLibrary(hlibOLEAut32);

	    hlibOLEAut32 = 0;
	}
    };


#ifdef DEBUG
    DbgRegisterObjectDestruction(m_dwCookie);
#endif
}

static const TCHAR szOle32Aut[]   = TEXT("OleAut32.dll");

HINSTANCE LoadOLEAut32()
{
    if (hlibOLEAut32 == 0) {

	hlibOLEAut32 = LoadLibrary(szOle32Aut);
    }

    return hlibOLEAut32;
}


/* Constructor */

// We know we use "this" in the initialization list, we also know we don't modify *phr.
CUnknown::CUnknown(__in_opt LPCTSTR pName, __in_opt LPUNKNOWN pUnk)
: CBaseObject(pName)
/* Start the object with a reference count of zero - when the      */
/* object is queried for it's first interface this may be          */
/* incremented depending on whether or not this object is          */
/* currently being aggregated upon                                 */
, m_cRef(0)
/* Set our pointer to our IUnknown interface.                      */
/* If we have an outer, use its, otherwise use ours.               */
/* This pointer effectivly points to the owner of                  */
/* this object and can be accessed by the GetOwner() method.       */
, m_pUnknown( pUnk != 0 ? pUnk : reinterpret_cast<LPUNKNOWN>( static_cast<PNDUNKNOWN>(this) ) )
 /* Why the double cast?  Well, the inner cast is a type-safe cast */
 /* to pointer to a type from which we inherit.  The second is     */
 /* type-unsafe but works because INonDelegatingUnknown "behaves   */
 /* like" IUnknown. (Only the names on the methods change.)        */
{
    // Everything we need to do has been done in the initializer list
}

// This does the same as above except it has a useless HRESULT argument
// use the previous constructor, this is just left for compatibility...
CUnknown::CUnknown(__in_opt LPCTSTR pName, __in_opt LPUNKNOWN pUnk, __inout_opt HRESULT *phr) :
    CBaseObject(pName),
    m_cRef(0),
    m_pUnknown( pUnk != 0 ? pUnk : reinterpret_cast<LPUNKNOWN>( static_cast<PNDUNKNOWN>(this) ) )
{
}

#ifdef UNICODE
CUnknown::CUnknown(__in_opt LPCSTR pName, __in_opt LPUNKNOWN pUnk)
: CBaseObject(pName), m_cRef(0),
    m_pUnknown( pUnk != 0 ? pUnk : reinterpret_cast<LPUNKNOWN>( static_cast<PNDUNKNOWN>(this) ) )
{ }

CUnknown::CUnknown(__in_opt LPCSTR pName, __in_opt LPUNKNOWN pUnk, __inout_opt HRESULT *phr) :
    CBaseObject(pName), m_cRef(0),
    m_pUnknown( pUnk != 0 ? pUnk : reinterpret_cast<LPUNKNOWN>( static_cast<PNDUNKNOWN>(this) ) )
{ }

#endif

/* QueryInterface */

STDMETHODIMP CUnknown::NonDelegatingQueryInterface(REFIID riid, __deref_out void ** ppv)
{
    CheckPointer(ppv,E_POINTER);
    ValidateReadWritePtr(ppv,sizeof(PVOID));

    /* We know only about IUnknown */

    if (riid == IID_IUnknown) {
        GetInterface((LPUNKNOWN) (PNDUNKNOWN) this, ppv);
        return NOERROR;
    } else {
        *ppv = NULL;
        return E_NOINTERFACE;
    }
}

/* We have to ensure that we DON'T use a max macro, since these will typically   */
/* lead to one of the parameters being evaluated twice.  Since we are worried    */
/* about concurrency, we can't afford to access the m_cRef twice since we can't  */
/* afford to run the risk that its value having changed between accesses.        */

template<class T> inline static T ourmax( const T & a, const T & b )
{
    return a > b ? a : b;
}

/* AddRef */

STDMETHODIMP_(ULONG) CUnknown::NonDelegatingAddRef()
{
    LONG lRef = InterlockedIncrement( &m_cRef );
    ASSERT(lRef > 0);
    DbgLog((LOG_MEMORY,3,TEXT("    Obj %d ref++ = %d"),
           m_dwCookie, m_cRef));
    return ourmax(ULONG(m_cRef), 1ul);
}


/* Release */

STDMETHODIMP_(ULONG) CUnknown::NonDelegatingRelease()
{
    /* If the reference count drops to zero delete ourselves */

    LONG lRef = InterlockedDecrement( &m_cRef );
    ASSERT(lRef >= 0);

    DbgLog((LOG_MEMORY,3,TEXT("    Object %d ref-- = %d"),
	    m_dwCookie, m_cRef));
    if (lRef == 0) {

        // COM rules say we must protect against re-entrancy.
        // If we are an aggregator and we hold our own interfaces
        // on the aggregatee, the QI for these interfaces will
        // addref ourselves. So after doing the QI we must release
        // a ref count on ourselves. Then, before releasing the
        // private interface, we must addref ourselves. When we do
        // this from the destructor here it will result in the ref
        // count going to 1 and then back to 0 causing us to
        // re-enter the destructor. Hence we add an extra refcount here
        // once we know we will delete the object.
        // for an example aggregator see filgraph\distrib.cpp.

        m_cRef++;

        delete this;
        return ULONG(0);
    } else {
        //  Don't touch m_cRef again even in this leg as the object
        //  may have just been released on another thread too
        return ourmax(ULONG(lRef), 1ul);
    }
}


/* Return an interface pointer to a requesting client
   performing a thread safe AddRef as necessary */

STDAPI GetInterface(LPUNKNOWN pUnk, __out void **ppv)
{
    CheckPointer(ppv, E_POINTER);
    *ppv = pUnk;
    pUnk->AddRef();
    return NOERROR;
}


/* Compares two interfaces and returns TRUE if they are on the same object */

BOOL WINAPI IsEqualObject(IUnknown *pFirst, IUnknown *pSecond)
{
    /*  Different objects can't have the same interface pointer for
        any interface
    */
    if (pFirst == pSecond) {
        return TRUE;
    }
    /*  OK - do it the hard way - check if they have the same
        IUnknown pointers - a single object can only have one of these
    */
    LPUNKNOWN pUnknown1;     // Retrieve the IUnknown interface
    LPUNKNOWN pUnknown2;     // Retrieve the other IUnknown interface
    HRESULT hr;              // General OLE return code

    ASSERT(pFirst);
    ASSERT(pSecond);

    /* See if the IUnknown pointers match */

    hr = pFirst->QueryInterface(IID_IUnknown,(void **) &pUnknown1);
    if (FAILED(hr)) {
        return FALSE;
    }
    ASSERT(pUnknown1);

    /* Release the extra interface we hold */

    pUnknown1->Release();

    hr = pSecond->QueryInterface(IID_IUnknown,(void **) &pUnknown2);
    if (FAILED(hr)) {
        return FALSE;
    }
    ASSERT(pUnknown2);

    /* Release the extra interface we hold */

    pUnknown2->Release();
    return (pUnknown1 == pUnknown2);
}

//------------------------------------------------------------------------------
// File: WXUtil.cpp
//
// Desc: DirectShow base classes - implements helper classes for building
//       multimedia filters.
//
// Copyright (c) 1992-2001 Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------

// --- CAMEvent -----------------------
CAMEvent::CAMEvent(BOOL fManualReset, __inout_opt HRESULT *phr)
{
    m_hEvent = CreateEvent(NULL, fManualReset, FALSE, NULL);
    if (NULL == m_hEvent) {
        if (NULL != phr && SUCCEEDED(*phr)) {
            *phr = E_OUTOFMEMORY;
        }
    }
}

CAMEvent::CAMEvent(__inout_opt HRESULT *phr)
{
    m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (NULL == m_hEvent) {
        if (NULL != phr && SUCCEEDED(*phr)) {
            *phr = E_OUTOFMEMORY;
        }
    }
}

CAMEvent::~CAMEvent()
{
    if (m_hEvent) {
	EXECUTE_ASSERT(CloseHandle(m_hEvent));
    }
}


// --- CAMMsgEvent -----------------------
// One routine.  The rest is handled in CAMEvent

CAMMsgEvent::CAMMsgEvent(__inout_opt HRESULT *phr) : CAMEvent(FALSE, phr)
{
}

BOOL CAMMsgEvent::WaitMsg(DWORD dwTimeout)
{
    // wait for the event to be signalled, or for the
    // timeout (in MS) to expire.  allow SENT messages
    // to be processed while we wait
    DWORD dwWait;
    DWORD dwStartTime;

    // set the waiting period.
    DWORD dwWaitTime = dwTimeout;

    // the timeout will eventually run down as we iterate
    // processing messages.  grab the start time so that
    // we can calculate elapsed times.
    if (dwWaitTime != INFINITE) {
        dwStartTime = timeGetTime();
    }

    do {
        dwWait = MsgWaitForMultipleObjects(1,&m_hEvent,FALSE, dwWaitTime, QS_SENDMESSAGE);
        if (dwWait == WAIT_OBJECT_0 + 1) {
	    MSG Message;
            PeekMessage(&Message,NULL,0,0,PM_NOREMOVE);

	    // If we have an explicit length of time to wait calculate
	    // the next wake up point - which might be now.
	    // If dwTimeout is INFINITE, it stays INFINITE
	    if (dwWaitTime != INFINITE) {

		DWORD dwElapsed = timeGetTime()-dwStartTime;

		dwWaitTime =
		    (dwElapsed >= dwTimeout)
			? 0  // wake up with WAIT_TIMEOUT
			: dwTimeout-dwElapsed;
	    }
        }
    } while (dwWait == WAIT_OBJECT_0 + 1);

    // return TRUE if we woke on the event handle,
    //        FALSE if we timed out.
    return (dwWait == WAIT_OBJECT_0);
}

// --- CAMThread ----------------------


CAMThread::CAMThread(__inout_opt HRESULT *phr)
    : m_EventSend(TRUE, phr),     // must be manual-reset for CheckRequest()
      m_EventComplete(FALSE, phr)
{
    m_hThread = NULL;
}

CAMThread::~CAMThread() {
    Close();
}


// when the thread starts, it calls this function. We unwrap the 'this'
//pointer and call ThreadProc.
DWORD WINAPI
CAMThread::InitialThreadProc(__inout LPVOID pv)
{
    HRESULT hrCoInit = CAMThread::CoInitializeHelper();
    if(FAILED(hrCoInit)) {
        DbgLog((LOG_ERROR, 1, TEXT("CoInitializeEx failed.")));
    }

    CAMThread * pThread = (CAMThread *) pv;

    HRESULT hr = pThread->ThreadProc();

    if(SUCCEEDED(hrCoInit)) {
        CoUninitialize();
    }

    return hr;
}

BOOL
CAMThread::Create()
{
    DWORD threadid;

    CAutoLock lock(&m_AccessLock);

    if (ThreadExists()) {
	return FALSE;
    }

    m_hThread = CreateThread(
		    NULL,
		    0,
		    CAMThread::InitialThreadProc,
		    this,
		    0,
		    &threadid);

    if (!m_hThread) {
	return FALSE;
    }

    return TRUE;
}

DWORD
CAMThread::CallWorker(DWORD dwParam)
{
    // lock access to the worker thread for scope of this object
    CAutoLock lock(&m_AccessLock);

    if (!ThreadExists()) {
	return (DWORD) E_FAIL;
    }

    // set the parameter
    m_dwParam = dwParam;

    // signal the worker thread
    m_EventSend.Set();

    // wait for the completion to be signalled
    m_EventComplete.Wait();

    // done - this is the thread's return value
    return m_dwReturnVal;
}

// Wait for a request from the client
DWORD
CAMThread::GetRequest()
{
    m_EventSend.Wait();
    return m_dwParam;
}

// is there a request?
BOOL
CAMThread::CheckRequest(__out_opt DWORD * pParam)
{
    if (!m_EventSend.Check()) {
	return FALSE;
    } else {
	if (pParam) {
	    *pParam = m_dwParam;
	}
	return TRUE;
    }
}

// reply to the request
void
CAMThread::Reply(DWORD dw)
{
    m_dwReturnVal = dw;

    // The request is now complete so CheckRequest should fail from
    // now on
    //
    // This event should be reset BEFORE we signal the client or
    // the client may Set it before we reset it and we'll then
    // reset it (!)

    m_EventSend.Reset();

    // Tell the client we're finished

    m_EventComplete.Set();
}

HRESULT CAMThread::CoInitializeHelper()
{
    // call CoInitializeEx and tell OLE not to create a window (this
    // thread probably won't dispatch messages and will hang on
    // broadcast msgs o/w).
    //
    // If CoInitEx is not available, threads that don't call CoCreate
    // aren't affected. Threads that do will have to handle the
    // failure. Perhaps we should fall back to CoInitialize and risk
    // hanging?
    //

    // older versions of ole32.dll don't have CoInitializeEx

    HRESULT hr = E_FAIL;
    HINSTANCE hOle = GetModuleHandle(TEXT("ole32.dll"));
    if(hOle)
    {
        typedef HRESULT (STDAPICALLTYPE *PCoInitializeEx)(
            LPVOID pvReserved, DWORD dwCoInit);
        PCoInitializeEx pCoInitializeEx =
            (PCoInitializeEx)(GetProcAddress(hOle, "CoInitializeEx"));
        if(pCoInitializeEx)
        {
            hr = (*pCoInitializeEx)(0, COINIT_DISABLE_OLE1DDE );
        }
    }
    else
    {
        // caller must load ole32.dll
        DbgBreak("couldn't locate ole32.dll");
    }

    return hr;
}


// destructor for CMsgThread  - cleans up any messages left in the
// queue when the thread exited
CMsgThread::~CMsgThread()
{
    if (m_hThread != NULL) {
        WaitForSingleObject(m_hThread, INFINITE);
        EXECUTE_ASSERT(CloseHandle(m_hThread));
    }

    POSITION pos = m_ThreadQueue.GetHeadPosition();
    while (pos) {
        CMsg * pMsg = m_ThreadQueue.GetNext(pos);
        delete pMsg;
    }
    m_ThreadQueue.RemoveAll();

    if (m_hSem != NULL) {
        EXECUTE_ASSERT(CloseHandle(m_hSem));
    }
}

BOOL
CMsgThread::CreateThread(
    )
{
    m_hSem = CreateSemaphore(NULL, 0, 0x7FFFFFFF, NULL);
    if (m_hSem == NULL) {
        return FALSE;
    }

    m_hThread = ::CreateThread(NULL, 0, DefaultThreadProc,
			       (LPVOID)this, 0, &m_ThreadId);
    return m_hThread != NULL;
}


// This is the threads message pump.  Here we get and dispatch messages to
// clients thread proc until the client refuses to process a message.
// The client returns a non-zero value to stop the message pump, this
// value becomes the threads exit code.

DWORD WINAPI
CMsgThread::DefaultThreadProc(
    __inout LPVOID lpParam
    )
{
    CMsgThread *lpThis = (CMsgThread *)lpParam;
    CMsg msg;
    LRESULT lResult;

    // !!!
    CoInitialize(NULL);

    // allow a derived class to handle thread startup
    lpThis->OnThreadInit();

    do {
	lpThis->GetThreadMsg(&msg);
	lResult = lpThis->ThreadMessageProc(msg.uMsg,msg.dwFlags,
					    msg.lpParam, msg.pEvent);
    } while (lResult == 0L);

    // !!!
    CoUninitialize();

    return (DWORD)lResult;
}


// Block until the next message is placed on the list m_ThreadQueue.
// copies the message to the message pointed to by *pmsg
void
CMsgThread::GetThreadMsg(__out CMsg *msg)
{
    CMsg * pmsg = NULL;

    // keep trying until a message appears
    while (TRUE) {
        {
            CAutoLock lck(&m_Lock);
            pmsg = m_ThreadQueue.RemoveHead();
            if (pmsg == NULL) {
                m_lWaiting++;
            } else {
                break;
            }
        }
        // the semaphore will be signalled when it is non-empty
        WaitForSingleObject(m_hSem, INFINITE);
    }
    // copy fields to caller's CMsg
    *msg = *pmsg;

    // this CMsg was allocated by the 'new' in PutThreadMsg
    delete pmsg;

}

// Helper function - convert int to WSTR
void WINAPI IntToWstr(int i, __out_ecount(12) LPWSTR wstr)
{
#ifdef UNICODE
    if (FAILED(StringCchPrintf(wstr, 12, L"%d", i))) {
        wstr[0] = 0;
    }
#else
    TCHAR temp[12];
    if (FAILED(StringCchPrintf(temp, NUMELMS(temp), "%d", i))) {
        wstr[0] = 0;
    } else {
        MultiByteToWideChar(CP_ACP, 0, temp, -1, wstr, 12);
    }
#endif
} // IntToWstr


#define MEMORY_ALIGNMENT        4
#define MEMORY_ALIGNMENT_LOG2   2
#define MEMORY_ALIGNMENT_MASK   MEMORY_ALIGNMENT - 1

void * __stdcall memmoveInternal(void * dst, const void * src, size_t count)
{
    void * ret = dst;

#ifdef _X86_
    if (dst <= src || (char *)dst >= ((char *)src + count)) {

        /*
         * Non-Overlapping Buffers
         * copy from lower addresses to higher addresses
         */
        _asm {
            mov     esi,src
            mov     edi,dst
            mov     ecx,count
            cld
            mov     edx,ecx
            and     edx,MEMORY_ALIGNMENT_MASK
            shr     ecx,MEMORY_ALIGNMENT_LOG2
            rep     movsd
            or      ecx,edx
            jz      memmove_done
            rep     movsb
memmove_done:
        }
    }
    else {

        /*
         * Overlapping Buffers
         * copy from higher addresses to lower addresses
         */
        _asm {
            mov     esi,src
            mov     edi,dst
            mov     ecx,count
            std
            add     esi,ecx
            add     edi,ecx
            dec     esi
            dec     edi
            rep     movsb
            cld
        }
    }
#else
    MoveMemory(dst, src, count);
#endif

    return ret;
}

HRESULT AMSafeMemMoveOffset(
    __in_bcount(dst_size) void * dst,
    __in size_t dst_size,
    __in DWORD cb_dst_offset,
    __in_bcount(src_size) const void * src,
    __in size_t src_size,
    __in DWORD cb_src_offset,
    __in size_t count)
{
    // prevent read overruns
    if( count + cb_src_offset < count ||   // prevent integer overflow
        count + cb_src_offset > src_size)  // prevent read overrun
    {
        return E_INVALIDARG;
    }

    // prevent write overruns
    if( count + cb_dst_offset < count ||   // prevent integer overflow
        count + cb_dst_offset > dst_size)  // prevent write overrun
    {
        return E_INVALIDARG;
    }

    memmoveInternal( (BYTE *)dst+cb_dst_offset, (BYTE *)src+cb_src_offset, count);
    return S_OK;
}


#ifdef DEBUG
/******************************Public*Routine******************************\
* Debug CCritSec helpers
*
* We provide debug versions of the Constructor, destructor, Lock and Unlock
* routines.  The debug code tracks who owns each critical section by
* maintaining a depth count.
*
* History:
*
\**************************************************************************/

CCritSec::CCritSec()
{
    InitializeCriticalSection(&m_CritSec);
    m_currentOwner = m_lockCount = 0;
    m_fTrace = FALSE;
}

CCritSec::~CCritSec()
{
    DeleteCriticalSection(&m_CritSec);
}

void CCritSec::Lock()
{
    UINT tracelevel=3;
    DWORD us = GetCurrentThreadId();
    DWORD currentOwner = m_currentOwner;
    if (currentOwner && (currentOwner != us)) {
        // already owned, but not by us
        if (m_fTrace) {
            DbgLog((LOG_LOCKING, 2, TEXT("Thread %d about to wait for lock %x owned by %d"),
                GetCurrentThreadId(), &m_CritSec, currentOwner));
            tracelevel=2;
	        // if we saw the message about waiting for the critical
	        // section we ensure we see the message when we get the
	        // critical section
        }
    }
    EnterCriticalSection(&m_CritSec);
    if (0 == m_lockCount++) {
        // we now own it for the first time.  Set owner information
        m_currentOwner = us;

        if (m_fTrace) {
            DbgLog((LOG_LOCKING, tracelevel, TEXT("Thread %d now owns lock %x"), m_currentOwner, &m_CritSec));
        }
    }
}

void CCritSec::Unlock() {
    if (0 == --m_lockCount) {
        // about to be unowned
        if (m_fTrace) {
            DbgLog((LOG_LOCKING, 3, TEXT("Thread %d releasing lock %x"), m_currentOwner, &m_CritSec));
        }

        m_currentOwner = 0;
    }
    LeaveCriticalSection(&m_CritSec);
}

void WINAPI DbgLockTrace(CCritSec * pcCrit, BOOL fTrace)
{
    pcCrit->m_fTrace = fTrace;
}

BOOL WINAPI CritCheckIn(CCritSec * pcCrit)
{
    return (GetCurrentThreadId() == pcCrit->m_currentOwner);
}

BOOL WINAPI CritCheckIn(const CCritSec * pcCrit)
{
    return (GetCurrentThreadId() == pcCrit->m_currentOwner);
}

BOOL WINAPI CritCheckOut(CCritSec * pcCrit)
{
    return (GetCurrentThreadId() != pcCrit->m_currentOwner);
}

BOOL WINAPI CritCheckOut(const CCritSec * pcCrit)
{
    return (GetCurrentThreadId() != pcCrit->m_currentOwner);
}
#endif


STDAPI WriteBSTR(__deref_out BSTR *pstrDest, LPCWSTR szSrc)
{
    *pstrDest = SysAllocString( szSrc );
    if( !(*pstrDest) ) return E_OUTOFMEMORY;
    return NOERROR;
}


STDAPI FreeBSTR(__deref_in BSTR* pstr)
{
    if( (PVOID)*pstr == NULL ) return S_FALSE;
    SysFreeString( *pstr );
    return NOERROR;
}


// Return a wide string - allocating memory for it
// Returns:
//    S_OK          - no error
//    E_POINTER     - ppszReturn == NULL
//    E_OUTOFMEMORY - can't allocate memory for returned string
STDAPI AMGetWideString(LPCWSTR psz, __deref_out LPWSTR *ppszReturn)
{
    CheckPointer(ppszReturn, E_POINTER);
    ValidateReadWritePtr(ppszReturn, sizeof(LPWSTR));
    *ppszReturn = NULL;
    size_t nameLen;
    HRESULT hr = StringCbLengthW(psz, 100000, &nameLen);
    if (FAILED(hr)) {
        return hr;
    }
    *ppszReturn = (LPWSTR)CoTaskMemAlloc(nameLen + sizeof(WCHAR));
    if (*ppszReturn == NULL) {
       return E_OUTOFMEMORY;
    }
    CopyMemory(*ppszReturn, psz, nameLen + sizeof(WCHAR));
    return NOERROR;
}

// Waits for the HANDLE hObject.  While waiting messages sent
// to windows on our thread by SendMessage will be processed.
// Using this function to do waits and mutual exclusion
// avoids some deadlocks in objects with windows.
// Return codes are the same as for WaitForSingleObject
DWORD WINAPI WaitDispatchingMessages(
    HANDLE hObject,
    DWORD dwWait,
    HWND hwnd,
    UINT uMsg,
    HANDLE hEvent)
{
    BOOL bPeeked = FALSE;
    DWORD dwResult;
    DWORD dwStart;
    DWORD dwThreadPriority;

    static UINT uMsgId = 0;

    HANDLE hObjects[2] = { hObject, hEvent };
    if (dwWait != INFINITE && dwWait != 0) {
        dwStart = GetTickCount();
    }
    for (; ; ) {
        DWORD nCount = NULL != hEvent ? 2 : 1;

        //  Minimize the chance of actually dispatching any messages
        //  by seeing if we can lock immediately.
        dwResult = WaitForMultipleObjects(nCount, hObjects, FALSE, 0);
        if (dwResult < WAIT_OBJECT_0 + nCount) {
            break;
        }

        DWORD dwTimeOut = dwWait;
        if (dwTimeOut > 10) {
            dwTimeOut = 10;
        }
        dwResult = MsgWaitForMultipleObjects(
                             nCount,
                             hObjects,
                             FALSE,
                             dwTimeOut,
                             hwnd == NULL ? QS_SENDMESSAGE :
                                            QS_SENDMESSAGE + QS_POSTMESSAGE);
        if (dwResult == WAIT_OBJECT_0 + nCount ||
            dwResult == WAIT_TIMEOUT && dwTimeOut != dwWait) {
            MSG msg;
            if (hwnd != NULL) {
                while (PeekMessage(&msg, hwnd, uMsg, uMsg, PM_REMOVE)) {
                    DispatchMessage(&msg);
                }
            }
            // Do this anyway - the previous peek doesn't flush out the
            // messages
            PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);

            if (dwWait != INFINITE && dwWait != 0) {
                DWORD dwNow = GetTickCount();

                // Working with differences handles wrap-around
                DWORD dwDiff = dwNow - dwStart;
                if (dwDiff > dwWait) {
                    dwWait = 0;
                } else {
                    dwWait -= dwDiff;
                }
                dwStart = dwNow;
            }
            if (!bPeeked) {
                //  Raise our priority to prevent our message queue
                //  building up
                dwThreadPriority = GetThreadPriority(GetCurrentThread());
                if (dwThreadPriority < THREAD_PRIORITY_HIGHEST) {
                    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
                }
                bPeeked = TRUE;
            }
        } else {
            break;
        }
    }
    if (bPeeked) {
        SetThreadPriority(GetCurrentThread(), dwThreadPriority);
        if (HIWORD(GetQueueStatus(QS_POSTMESSAGE)) & QS_POSTMESSAGE) {
            if (uMsgId == 0) {
                uMsgId = RegisterWindowMessage(TEXT("AMUnblock"));
            }
            if (uMsgId != 0) {
                MSG msg;
                //  Remove old ones
                while (PeekMessage(&msg, (HWND)-1, uMsgId, uMsgId, PM_REMOVE)) {
                }
            }
            PostThreadMessage(GetCurrentThreadId(), uMsgId, 0, 0);
        }
    }
    return dwResult;
}

HRESULT AmGetLastErrorToHResult()
{
    DWORD dwLastError = GetLastError();
    if(dwLastError != 0)
    {
        return HRESULT_FROM_WIN32(dwLastError);
    }
    else
    {
        return E_FAIL;
    }
}

IUnknown* QzAtlComPtrAssign(__deref_inout_opt IUnknown** pp, __in_opt IUnknown* lp)
{
    if (lp != NULL)
        lp->AddRef();
    if (*pp)
        (*pp)->Release();
    *pp = lp;
    return lp;
}

/******************************************************************************

CompatibleTimeSetEvent

    CompatibleTimeSetEvent() sets the TIME_KILL_SYNCHRONOUS flag before calling
timeSetEvent() if the current operating system supports it.  TIME_KILL_SYNCHRONOUS
is supported on Windows XP and later operating systems.

Parameters:
- The same parameters as timeSetEvent().  See timeSetEvent()'s documentation in 
the Platform SDK for more information.

Return Value:
- The same return value as timeSetEvent().  See timeSetEvent()'s documentation in 
the Platform SDK for more information.

******************************************************************************/
MMRESULT CompatibleTimeSetEvent( UINT uDelay, UINT uResolution, __in LPTIMECALLBACK lpTimeProc, DWORD_PTR dwUser, UINT fuEvent )
{
    #if WINVER >= 0x0501
    {
        static bool fCheckedVersion = false;
        static bool fTimeKillSynchronousFlagAvailable = false; 

        if( !fCheckedVersion ) {
            fTimeKillSynchronousFlagAvailable = TimeKillSynchronousFlagAvailable();
            fCheckedVersion = true;
        }

        if( fTimeKillSynchronousFlagAvailable ) {
            fuEvent = fuEvent | TIME_KILL_SYNCHRONOUS;
        }
    }
    #endif // WINVER >= 0x0501

    return timeSetEvent( uDelay, uResolution, lpTimeProc, dwUser, fuEvent );
}

bool TimeKillSynchronousFlagAvailable( void )
{
    OSVERSIONINFO osverinfo;

    osverinfo.dwOSVersionInfoSize = sizeof(osverinfo);

#pragma warning(push)
#pragma warning(disable: 4996) //warning C4996: 'GetVersionExA': was declared deprecated
    if( GetVersionEx( &osverinfo ) ) {
#pragma warning(pop)
        
        // Windows XP's major version is 5 and its' minor version is 1.
        // timeSetEvent() started supporting the TIME_KILL_SYNCHRONOUS flag
        // in Windows XP.
        if( (osverinfo.dwMajorVersion > 5) || 
            ( (osverinfo.dwMajorVersion == 5) && (osverinfo.dwMinorVersion >= 1) ) ) {
            return true;
        }
    }

    return false;
}

//------------------------------------------------------------------------------
// File: MType.cpp
//
// Desc: DirectShow base classes - implements a class that holds and 
//       manages media type information.
//
// Copyright (c) 1992-2001 Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------

// helper class that derived pin objects can use to compare media
// types etc. Has same data members as the struct AM_MEDIA_TYPE defined
// in the streams IDL file, but also has (non-virtual) functions

CMediaType::~CMediaType(){
    FreeMediaType(*this);
}


CMediaType::CMediaType()
{
    InitMediaType();
}


CMediaType::CMediaType(const GUID * type)
{
    InitMediaType();
    majortype = *type;
}


// copy constructor does a deep copy of the format block

CMediaType::CMediaType(const AM_MEDIA_TYPE& rt, __out_opt HRESULT* phr)
{
    HRESULT hr = CopyMediaType(this, &rt);
    if (FAILED(hr) && (NULL != phr)) {
        *phr = hr;
    }
}


CMediaType::CMediaType(const CMediaType& rt, __out_opt HRESULT* phr)
{
    HRESULT hr = CopyMediaType(this, &rt);
    if (FAILED(hr) && (NULL != phr)) {
        *phr = hr;
    }
}


// this class inherits publicly from AM_MEDIA_TYPE so the compiler could generate
// the following assignment operator itself, however it could introduce some
// memory conflicts and leaks in the process because the structure contains
// a dynamically allocated block (pbFormat) which it will not copy correctly

CMediaType&
CMediaType::operator=(const AM_MEDIA_TYPE& rt)
{
    Set(rt);
    return *this;
}

CMediaType&
CMediaType::operator=(const CMediaType& rt)
{
    *this = (AM_MEDIA_TYPE &) rt;
    return *this;
}

BOOL
CMediaType::operator == (const CMediaType& rt) const
{
    // I don't believe we need to check sample size or
    // temporal compression flags, since I think these must
    // be represented in the type, subtype and format somehow. They
    // are pulled out as separate flags so that people who don't understand
    // the particular format representation can still see them, but
    // they should duplicate information in the format block.

    return ((IsEqualGUID(majortype,rt.majortype) == TRUE) &&
        (IsEqualGUID(subtype,rt.subtype) == TRUE) &&
        (IsEqualGUID(formattype,rt.formattype) == TRUE) &&
        (cbFormat == rt.cbFormat) &&
        ( (cbFormat == 0) ||
          pbFormat != NULL && rt.pbFormat != NULL &&
          (memcmp(pbFormat, rt.pbFormat, cbFormat) == 0)));
}


BOOL
CMediaType::operator != (const CMediaType& rt) const
{
    /* Check to see if they are equal */

    if (*this == rt) {
        return FALSE;
    }
    return TRUE;
}


HRESULT
CMediaType::Set(const CMediaType& rt)
{
    return Set((AM_MEDIA_TYPE &) rt);
}


HRESULT
CMediaType::Set(const AM_MEDIA_TYPE& rt)
{
    if (&rt != this) {
        FreeMediaType(*this);
        HRESULT hr = CopyMediaType(this, &rt);
        if (FAILED(hr)) {
            return E_OUTOFMEMORY;
        }
    }

    return S_OK;    
}


BOOL
CMediaType::IsValid() const
{
    return (!IsEqualGUID(majortype,GUID_NULL));
}


void
CMediaType::SetType(const GUID* ptype)
{
    majortype = *ptype;
}


void
CMediaType::SetSubtype(const GUID* ptype)
{
    subtype = *ptype;
}


ULONG
CMediaType::GetSampleSize() const {
    if (IsFixedSize()) {
        return lSampleSize;
    } else {
        return 0;
    }
}


void
CMediaType::SetSampleSize(ULONG sz) {
    if (sz == 0) {
        SetVariableSize();
    } else {
        bFixedSizeSamples = TRUE;
        lSampleSize = sz;
    }
}


void
CMediaType::SetVariableSize() {
    bFixedSizeSamples = FALSE;
}


void
CMediaType::SetTemporalCompression(BOOL bCompressed) {
    bTemporalCompression = bCompressed;
}

BOOL
CMediaType::SetFormat(__in_bcount(cb) BYTE * pformat, ULONG cb)
{
    if (NULL == AllocFormatBuffer(cb))
	return(FALSE);

    ASSERT(pbFormat);
    memcpy(pbFormat, pformat, cb);
    return(TRUE);
}


// set the type of the media type format block, this type defines what you
// will actually find in the format pointer. For example FORMAT_VideoInfo or
// FORMAT_WaveFormatEx. In the future this may be an interface pointer to a
// property set. Before sending out media types this should be filled in.

void
CMediaType::SetFormatType(const GUID *pformattype)
{
    formattype = *pformattype;
}


// reset the format buffer

void CMediaType::ResetFormatBuffer()
{
    if (cbFormat) {
        CoTaskMemFree((PVOID)pbFormat);
    }
    cbFormat = 0;
    pbFormat = NULL;
}


// allocate length bytes for the format and return a read/write pointer
// If we cannot allocate the new block of memory we return NULL leaving
// the original block of memory untouched (as does ReallocFormatBuffer)

BYTE*
CMediaType::AllocFormatBuffer(ULONG length)
{
    ASSERT(length);

    // do the types have the same buffer size

    if (cbFormat == length) {
        return pbFormat;
    }

    // allocate the new format buffer

    BYTE *pNewFormat = (PBYTE)CoTaskMemAlloc(length);
    if (pNewFormat == NULL) {
        if (length <= cbFormat) return pbFormat; //reuse the old block anyway.
        return NULL;
    }

    // delete the old format

    if (cbFormat != 0) {
        ASSERT(pbFormat);
        CoTaskMemFree((PVOID)pbFormat);
    }

    cbFormat = length;
    pbFormat = pNewFormat;
    return pbFormat;
}


// reallocate length bytes for the format and return a read/write pointer
// to it. We keep as much information as we can given the new buffer size
// if this fails the original format buffer is left untouched. The caller
// is responsible for ensuring the size of memory required is non zero

BYTE*
CMediaType::ReallocFormatBuffer(ULONG length)
{
    ASSERT(length);

    // do the types have the same buffer size

    if (cbFormat == length) {
        return pbFormat;
    }

    // allocate the new format buffer

    BYTE *pNewFormat = (PBYTE)CoTaskMemAlloc(length);
    if (pNewFormat == NULL) {
        if (length <= cbFormat) return pbFormat; //reuse the old block anyway.
        return NULL;
    }

    // copy any previous format (or part of if new is smaller)
    // delete the old format and replace with the new one

    if (cbFormat != 0) {
        ASSERT(pbFormat);
        memcpy(pNewFormat,pbFormat,min(length,cbFormat));
        CoTaskMemFree((PVOID)pbFormat);
    }

    cbFormat = length;
    pbFormat = pNewFormat;
    return pNewFormat;
}

// initialise a media type structure

void CMediaType::InitMediaType()
{
    ZeroMemory((PVOID)this, sizeof(*this));
    lSampleSize = 1;
    bFixedSizeSamples = TRUE;
}


// a partially specified media type can be passed to IPin::Connect
// as a constraint on the media type used in the connection.
// the type, subtype or format type can be null.
BOOL
CMediaType::IsPartiallySpecified(void) const
{
    if ((majortype == GUID_NULL) ||
        (formattype == GUID_NULL)) {
            return TRUE;
    } else {
        return FALSE;
    }
}

BOOL
CMediaType::MatchesPartial(const CMediaType* ppartial) const
{
    if ((ppartial->majortype != GUID_NULL) &&
        (majortype != ppartial->majortype)) {
            return FALSE;
    }
    if ((ppartial->subtype != GUID_NULL) &&
        (subtype != ppartial->subtype)) {
            return FALSE;
    }

    if (ppartial->formattype != GUID_NULL) {
        // if the format block is specified then it must match exactly
        if (formattype != ppartial->formattype) {
            return FALSE;
        }
        if (cbFormat != ppartial->cbFormat) {
            return FALSE;
        }
        if ((cbFormat != 0) &&
            (memcmp(pbFormat, ppartial->pbFormat, cbFormat) != 0)) {
                return FALSE;
        }
    }

    return TRUE;

}



// general purpose function to delete a heap allocated AM_MEDIA_TYPE structure
// which is useful when calling IEnumMediaTypes::Next as the interface
// implementation allocates the structures which you must later delete
// the format block may also be a pointer to an interface to release

void WINAPI DeleteMediaType(__inout_opt AM_MEDIA_TYPE *pmt)
{
    // allow NULL pointers for coding simplicity

    if (pmt == NULL) {
        return;
    }

    FreeMediaType(*pmt);
    CoTaskMemFree((PVOID)pmt);
}


// this also comes in useful when using the IEnumMediaTypes interface so
// that you can copy a media type, you can do nearly the same by creating
// a CMediaType object but as soon as it goes out of scope the destructor
// will delete the memory it allocated (this takes a copy of the memory)

AM_MEDIA_TYPE * WINAPI CreateMediaType(AM_MEDIA_TYPE const *pSrc)
{
    ASSERT(pSrc);

    // Allocate a block of memory for the media type

    AM_MEDIA_TYPE *pMediaType =
        (AM_MEDIA_TYPE *)CoTaskMemAlloc(sizeof(AM_MEDIA_TYPE));

    if (pMediaType == NULL) {
        return NULL;
    }
    // Copy the variable length format block

    HRESULT hr = CopyMediaType(pMediaType,pSrc);
    if (FAILED(hr)) {
        CoTaskMemFree((PVOID)pMediaType);
        return NULL;
    }

    return pMediaType;
}


//  Copy 1 media type to another

HRESULT WINAPI CopyMediaType(__out AM_MEDIA_TYPE *pmtTarget, const AM_MEDIA_TYPE *pmtSource)
{
    //  We'll leak if we copy onto one that already exists - there's one
    //  case we can check like that - copying to itself.
    ASSERT(pmtSource != pmtTarget);
    *pmtTarget = *pmtSource;
    if (pmtSource->cbFormat != 0) {
        ASSERT(pmtSource->pbFormat != NULL);
        pmtTarget->pbFormat = (PBYTE)CoTaskMemAlloc(pmtSource->cbFormat);
        if (pmtTarget->pbFormat == NULL) {
            pmtTarget->cbFormat = 0;
            return E_OUTOFMEMORY;
        } else {
            CopyMemory((PVOID)pmtTarget->pbFormat, (PVOID)pmtSource->pbFormat,
                       pmtTarget->cbFormat);
        }
    }
    if (pmtTarget->pUnk != NULL) {
        pmtTarget->pUnk->AddRef();
    }

    return S_OK;
}

//  Free an existing media type (ie free resources it holds)

void WINAPI FreeMediaType(__inout AM_MEDIA_TYPE& mt)
{
    if (mt.cbFormat != 0) {
        CoTaskMemFree((PVOID)mt.pbFormat);

        // Strictly unnecessary but tidier
        mt.cbFormat = 0;
        mt.pbFormat = NULL;
    }
    if (mt.pUnk != NULL) {
        mt.pUnk->Release();
        mt.pUnk = NULL;
    }
}

//  Initialize a media type from a WAVEFORMATEX

STDAPI CreateAudioMediaType(
    const WAVEFORMATEX *pwfx,
    __out AM_MEDIA_TYPE *pmt,
    BOOL bSetFormat
)
{
    pmt->majortype            = MEDIATYPE_Audio;
    if (pwfx->wFormatTag == WAVE_FORMAT_EXTENSIBLE) {
        pmt->subtype = ((PWAVEFORMATEXTENSIBLE)pwfx)->SubFormat;
    } else {
        pmt->subtype              = FOURCCMap(pwfx->wFormatTag);
    }
    pmt->formattype           = FORMAT_WaveFormatEx;
    pmt->bFixedSizeSamples    = TRUE;
    pmt->bTemporalCompression = FALSE;
    pmt->lSampleSize          = pwfx->nBlockAlign;
    pmt->pUnk                 = NULL;
    if (bSetFormat) {
        if (pwfx->wFormatTag == WAVE_FORMAT_PCM) {
            pmt->cbFormat         = sizeof(WAVEFORMATEX);
        } else {
            pmt->cbFormat         = sizeof(WAVEFORMATEX) + pwfx->cbSize;
        }
        pmt->pbFormat             = (PBYTE)CoTaskMemAlloc(pmt->cbFormat);
        if (pmt->pbFormat == NULL) {
            return E_OUTOFMEMORY;
        }
        if (pwfx->wFormatTag == WAVE_FORMAT_PCM) {
            CopyMemory(pmt->pbFormat, pwfx, sizeof(PCMWAVEFORMAT));
            ((WAVEFORMATEX *)pmt->pbFormat)->cbSize = 0;
        } else {
            CopyMemory(pmt->pbFormat, pwfx, pmt->cbFormat);
        }
    }
    return S_OK;
}

//------------------------------------------------------------------------------
// File: WXList.cpp
//
// Desc: DirectShow base classes - implements a non-MFC based generic list
//       template class.
// Copyright (c) 1992-2001 Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------

/* A generic list of pointers to objects.
   Objectives: avoid using MFC libraries in ndm kernel mode and
   provide a really useful list type.

   The class is thread safe in that separate threads may add and
   delete items in the list concurrently although the application
   must ensure that constructor and destructor access is suitably
   synchronised.

   The list name must not conflict with MFC classes as an
   application may use both

   The nodes form a doubly linked, NULL terminated chain with an anchor
   block (the list object per se) holding pointers to the first and last
   nodes and a count of the nodes.
   There is a node cache to reduce the allocation and freeing overhead.
   It optionally (determined at construction time) has an Event which is
   set whenever the list becomes non-empty and reset whenever it becomes
   empty.
   It optionally (determined at construction time) has a Critical Section
   which is entered during the important part of each operation.  (About
   all you can do outside it is some parameter checking).

   The node cache is a repository of nodes that are NOT in the list to speed
   up storage allocation.  Each list has its own cache to reduce locking and
   serialising.  The list accesses are serialised anyway for a given list - a
   common cache would mean that we would have to separately serialise access
   of all lists within the cache.  Because the cache only stores nodes that are
   not in the list, releasing the cache does not release any list nodes.  This
   means that list nodes can be copied or rechained from one list to another
   without danger of creating a dangling reference if the original cache goes
   away.

   Questionable design decisions:
   1. Retaining the warts for compatibility
   2. Keeping an element count -i.e. counting whenever we do anything
      instead of only when we want the count.
   3. Making the chain pointers NULL terminated.  If the list object
      itself looks just like a node and the list is kept as a ring then
      it reduces the number of special cases.  All inserts look the same.
*/

/* set cursor to the position of each element of list in turn  */
#define INTERNALTRAVERSELIST(list, cursor)               \
for ( cursor = (list).GetHeadPositionI()           \
    ; cursor!=NULL                               \
    ; cursor = (list).Next(cursor)                \
    )


/* set cursor to the position of each element of list in turn
   in reverse order
*/
#define INTERNALREVERSETRAVERSELIST(list, cursor)        \
for ( cursor = (list).GetTailPositionI()           \
    ; cursor!=NULL                               \
    ; cursor = (list).Prev(cursor)                \
    )

/* Constructor calls a separate initialisation function that
   creates a node cache, optionally creates a lock object
   and optionally creates a signaling object.

   By default we create a locking object, a DEFAULTCACHE sized
   cache but no event object so the list cannot be used in calls
   to WaitForSingleObject
*/
CBaseList::CBaseList(__in_opt LPCTSTR pName,    // Descriptive list name
                     INT iItems) :    // Node cache size
#ifdef DEBUG
    CBaseObject(pName),
#endif
    m_pFirst(NULL),
    m_pLast(NULL),
    m_Count(0),
    m_Cache(iItems)
{
} // constructor

CBaseList::CBaseList(__in_opt LPCTSTR pName) :  // Descriptive list name
#ifdef DEBUG
    CBaseObject(pName),
#endif
    m_pFirst(NULL),
    m_pLast(NULL),
    m_Count(0),
    m_Cache(DEFAULTCACHE)
{
} // constructor

#ifdef UNICODE
CBaseList::CBaseList(__in_opt LPCSTR pName,    // Descriptive list name
                     INT iItems) :    // Node cache size
#ifdef DEBUG
    CBaseObject(pName),
#endif
    m_pFirst(NULL),
    m_pLast(NULL),
    m_Count(0),
    m_Cache(iItems)
{
} // constructor

CBaseList::CBaseList(__in_opt LPCSTR pName) :  // Descriptive list name
#ifdef DEBUG
    CBaseObject(pName),
#endif
    m_pFirst(NULL),
    m_pLast(NULL),
    m_Count(0),
    m_Cache(DEFAULTCACHE)
{
} // constructor

#endif

/* The destructor enumerates all the node objects in the list and
   in the cache deleting each in turn. We do not do any processing
   on the objects that the list holds (i.e. points to) so if they
   represent interfaces for example the creator of the list should
   ensure that each of them is released before deleting us
*/
CBaseList::~CBaseList()
{
    /* Delete all our list nodes */

    RemoveAll();

} // destructor

/* Remove all the nodes from the list but don't do anything
   with the objects that each node looks after (this is the
   responsibility of the creator).
   Aa a last act we reset the signalling event
   (if available) to indicate to clients that the list
   does not have any entries in it.
*/
void CBaseList::RemoveAll()
{
    /* Free up all the CNode objects NOTE we don't bother putting the
       deleted nodes into the cache as this method is only really called
       in serious times of change such as when we are being deleted at
       which point the cache will be deleted anway */

    CNode *pn = m_pFirst;
    while (pn) {
        CNode *op = pn;
        pn = pn->Next();
        delete op;
    }

    /* Reset the object count and the list pointers */

    m_Count = 0;
    m_pFirst = m_pLast = NULL;

} // RemoveAll



/* Return a position enumerator for the entire list.
   A position enumerator is a pointer to a node object cast to a
   transparent type so all we do is return the head/tail node
   pointer in the list.
   WARNING because the position is a pointer to a node there is
   an implicit assumption for users a the list class that after
   deleting an object from the list that any other position
   enumerators that you have may be invalid (since the node
   may be gone).
*/
__out_opt POSITION CBaseList::GetHeadPositionI() const
{
    return (POSITION) m_pFirst;
} // GetHeadPosition



__out_opt POSITION CBaseList::GetTailPositionI() const
{
    return (POSITION) m_pLast;
} // GetTailPosition



/* Get the number of objects in the list,
   Get the lock before accessing the count.
   Locking may not be entirely necessary but it has the side effect
   of making sure that all operations are complete before we get it.
   So for example if a list is being added to this list then that
   will have completed in full before we continue rather than seeing
   an intermediate albeit valid state
*/
int CBaseList::GetCountI() const
{
    return m_Count;
} // GetCount



/* Return the object at rp, update rp to the next object from
   the list or NULL if you have moved over the last object.
   You may still call this function once we return NULL but
   we will continue to return a NULL position value
*/
__out void *CBaseList::GetNextI(__inout POSITION& rp) const
{
    /* have we reached the end of the list */

    if (rp == NULL) {
        return NULL;
    }

    /* Lock the object before continuing */

    void *pObject;

    /* Copy the original position then step on */

    CNode *pn = (CNode *) rp;
    ASSERT(pn != NULL);
    rp = (POSITION) pn->Next();

    /* Get the object at the original position from the list */

    pObject = pn->GetData();
    // ASSERT(pObject != NULL);    // NULL pointers in the list are allowed.
    return pObject;
} //GetNext



/* Return the object at p.
   Asking for the object at NULL ASSERTs then returns NULL
   The object is NOT locked.  The list is not being changed
   in any way.  If another thread is busy deleting the object
   then locking would only result in a change from one bad
   behaviour to another.
*/
__out_opt void *CBaseList::GetI(__in_opt POSITION p) const
{
    if (p == NULL) {
        return NULL;
    }

    CNode * pn = (CNode *) p;
    void *pObject = pn->GetData();
    // ASSERT(pObject != NULL);    // NULL pointers in the list are allowed.
    return pObject;
} //Get

__out void *CBaseList::GetValidI(__in POSITION p) const
{
    CNode * pn = (CNode *) p;
    void *pObject = pn->GetData();
    // ASSERT(pObject != NULL);    // NULL pointers in the list are allowed.
    return pObject;
} //Get


/* Return the first position in the list which holds the given pointer.
   Return NULL if it's not found.
*/
__out_opt POSITION CBaseList::FindI( __in void * pObj) const
{
    POSITION pn;
    INTERNALTRAVERSELIST(*this, pn){
        if (GetI(pn)==pObj) {
            return pn;
        }
    }
    return NULL;
} // Find



/* Remove the first node in the list (deletes the pointer to its object
   from the list, does not free the object itself).
   Return the pointer to its object or NULL if empty
*/
__out_opt void *CBaseList::RemoveHeadI()
{
    /* All we do is get the head position and ask for that to be deleted.
       We could special case this since some of the code path checking
       in Remove() is redundant as we know there is no previous
       node for example but it seems to gain little over the
       added complexity
    */

    return RemoveI((POSITION)m_pFirst);
} // RemoveHead



/* Remove the last node in the list (deletes the pointer to its object
   from the list, does not free the object itself).
   Return the pointer to its object or NULL if empty
*/
__out_opt void *CBaseList::RemoveTailI()
{
    /* All we do is get the tail position and ask for that to be deleted.
       We could special case this since some of the code path checking
       in Remove() is redundant as we know there is no previous
       node for example but it seems to gain little over the
       added complexity
    */

    return RemoveI((POSITION)m_pLast);
} // RemoveTail



/* Remove the pointer to the object in this position from the list.
   Deal with all the chain pointers
   Return a pointer to the object removed from the list.
   The node object that is freed as a result
   of this operation is added to the node cache where
   it can be used again.
   Remove(NULL) is a harmless no-op - but probably is a wart.
*/
__out_opt void *CBaseList::RemoveI(__in_opt POSITION pos)
{
    /* Lock the critical section before continuing */

    // ASSERT (pos!=NULL);     // Removing NULL is to be harmless!
    if (pos==NULL) return NULL;


    CNode *pCurrent = (CNode *) pos;
    ASSERT(pCurrent != NULL);

    /* Update the previous node */

    CNode *pNode = pCurrent->Prev();
    if (pNode == NULL) {
        m_pFirst = pCurrent->Next();
    } else {
        pNode->SetNext(pCurrent->Next());
    }

    /* Update the following node */

    pNode = pCurrent->Next();
    if (pNode == NULL) {
        m_pLast = pCurrent->Prev();
    } else {
        pNode->SetPrev(pCurrent->Prev());
    }

    /* Get the object this node was looking after */

    void *pObject = pCurrent->GetData();

    // ASSERT(pObject != NULL);    // NULL pointers in the list are allowed.

    /* Try and add the node object to the cache -
       a NULL return code from the cache means we ran out of room.
       The cache size is fixed by a constructor argument when the
       list is created and defaults to DEFAULTCACHE.
       This means that the cache will have room for this many
       node objects. So if you have a list of media samples
       and you know there will never be more than five active at
       any given time of them for example then override the default
       constructor
    */

    m_Cache.AddToCache(pCurrent);

    /* If the list is empty then reset the list event */

    --m_Count;
    ASSERT(m_Count >= 0);
    return pObject;
} // Remove



/* Add this object to the tail end of our list
   Return the new tail position.
*/

__out_opt POSITION CBaseList::AddTailI(__in void *pObject)
{
    /* Lock the critical section before continuing */

    CNode *pNode;
    // ASSERT(pObject);   // NULL pointers in the list are allowed.

    /* If there is a node objects in the cache then use
       that otherwise we will have to create a new one */

    pNode = (CNode *) m_Cache.RemoveFromCache();
    if (pNode == NULL) {
        pNode = new CNode;
    }

    /* Check we have a valid object */

    if (pNode == NULL) {
        return NULL;
    }

    /* Initialise all the CNode object
       just in case it came from the cache
    */

    pNode->SetData(pObject);
    pNode->SetNext(NULL);
    pNode->SetPrev(m_pLast);

    if (m_pLast == NULL) {
        m_pFirst = pNode;
    } else {
        m_pLast->SetNext(pNode);
    }

    /* Set the new last node pointer and also increment the number
       of list entries, the critical section is unlocked when we
       exit the function
    */

    m_pLast = pNode;
    ++m_Count;

    return (POSITION) pNode;
} // AddTail(object)



/* Add this object to the head end of our list
   Return the new head position.
*/
__out_opt POSITION CBaseList::AddHeadI(__in void *pObject)
{
    CNode *pNode;
    // ASSERT(pObject);  // NULL pointers in the list are allowed.

    /* If there is a node objects in the cache then use
       that otherwise we will have to create a new one */

    pNode = (CNode *) m_Cache.RemoveFromCache();
    if (pNode == NULL) {
        pNode = new CNode;
    }

    /* Check we have a valid object */

    if (pNode == NULL) {
        return NULL;
    }

    /* Initialise all the CNode object
       just in case it came from the cache
    */

    pNode->SetData(pObject);

    /* chain it in (set four pointers) */
    pNode->SetPrev(NULL);
    pNode->SetNext(m_pFirst);

    if (m_pFirst == NULL) {
        m_pLast = pNode;
    } else {
        m_pFirst->SetPrev(pNode);
    }
    m_pFirst = pNode;

    ++m_Count;

    return (POSITION) pNode;
} // AddHead(object)



/* Add all the elements in *pList to the tail of this list.
   Return TRUE if it all worked, FALSE if it didn't.
   If it fails some elements may have been added.
*/
BOOL CBaseList::AddTail(__in CBaseList *pList)
{
    /* lock the object before starting then enumerate
       each entry in the source list and add them one by one to
       our list (while still holding the object lock)
       Lock the other list too.
    */
    POSITION pos = pList->GetHeadPositionI();

    while (pos) {
       if (NULL == AddTailI(pList->GetNextI(pos))) {
           return FALSE;
       }
    }
    return TRUE;
} // AddTail(list)



/* Add all the elements in *pList to the head of this list.
   Return TRUE if it all worked, FALSE if it didn't.
   If it fails some elements may have been added.
*/
BOOL CBaseList::AddHead(__in CBaseList *pList)
{
    /* lock the object before starting then enumerate
       each entry in the source list and add them one by one to
       our list (while still holding the object lock)
       Lock the other list too.

       To avoid reversing the list, traverse it backwards.
    */

    POSITION pos;

    INTERNALREVERSETRAVERSELIST(*pList, pos) {
        if (NULL== AddHeadI(pList->GetValidI(pos))){
            return FALSE;
        }
    }
    return TRUE;
} // AddHead(list)



/* Add the object after position p
   p is still valid after the operation.
   AddAfter(NULL,x) adds x to the start - same as AddHead
   Return the position of the new object, NULL if it failed
*/
__out_opt POSITION  CBaseList::AddAfterI(__in_opt POSITION pos, __in void * pObj)
{
    if (pos==NULL)
        return AddHeadI(pObj);

    /* As someone else might be furkling with the list -
       Lock the critical section before continuing
    */
    CNode *pAfter = (CNode *) pos;
    ASSERT(pAfter != NULL);
    if (pAfter==m_pLast)
        return AddTailI(pObj);

    /* set pnode to point to a new node, preferably from the cache */

    CNode *pNode = (CNode *) m_Cache.RemoveFromCache();
    if (pNode == NULL) {
        pNode = new CNode;
    }

    /* Check we have a valid object */

    if (pNode == NULL) {
        return NULL;
    }

    /* Initialise all the CNode object
       just in case it came from the cache
    */

    pNode->SetData(pObj);

    /* It is to be added to the middle of the list - there is a before
       and after node.  Chain it after pAfter, before pBefore.
    */
    CNode * pBefore = pAfter->Next();
    ASSERT(pBefore != NULL);

    /* chain it in (set four pointers) */
    pNode->SetPrev(pAfter);
    pNode->SetNext(pBefore);
    pBefore->SetPrev(pNode);
    pAfter->SetNext(pNode);

    ++m_Count;

    return (POSITION) pNode;

} // AddAfter(object)



BOOL CBaseList::AddAfter(__in_opt POSITION p, __in CBaseList *pList)
{
    POSITION pos;
    INTERNALTRAVERSELIST(*pList, pos) {
        /* p follows along the elements being added */
        p = AddAfterI(p, pList->GetValidI(pos));
        if (p==NULL) return FALSE;
    }
    return TRUE;
} // AddAfter(list)



/* Mirror images:
   Add the element or list after position p.
   p is still valid after the operation.
   AddBefore(NULL,x) adds x to the end - same as AddTail
*/
__out_opt POSITION CBaseList::AddBeforeI(__in_opt POSITION pos, __in void * pObj)
{
    if (pos==NULL)
        return AddTailI(pObj);

    /* set pnode to point to a new node, preferably from the cache */

    CNode *pBefore = (CNode *) pos;
    ASSERT(pBefore != NULL);
    if (pBefore==m_pFirst)
        return AddHeadI(pObj);

    CNode * pNode = (CNode *) m_Cache.RemoveFromCache();
    if (pNode == NULL) {
        pNode = new CNode;
    }

    /* Check we have a valid object */

    if (pNode == NULL) {
        return NULL;
    }

    /* Initialise all the CNode object
       just in case it came from the cache
    */

    pNode->SetData(pObj);

    /* It is to be added to the middle of the list - there is a before
       and after node.  Chain it after pAfter, before pBefore.
    */

    CNode * pAfter = pBefore->Prev();
    ASSERT(pAfter != NULL);

    /* chain it in (set four pointers) */
    pNode->SetPrev(pAfter);
    pNode->SetNext(pBefore);
    pBefore->SetPrev(pNode);
    pAfter->SetNext(pNode);

    ++m_Count;

    return (POSITION) pNode;

} // Addbefore(object)



BOOL CBaseList::AddBefore(__in_opt POSITION p, __in CBaseList *pList)
{
    POSITION pos;
    INTERNALREVERSETRAVERSELIST(*pList, pos) {
        /* p follows along the elements being added */
        p = AddBeforeI(p, pList->GetValidI(pos));
        if (p==NULL) return FALSE;
    }
    return TRUE;
} // AddBefore(list)



/* Split *this after position p in *this
   Retain as *this the tail portion of the original *this
   Add the head portion to the tail end of *pList
   Return TRUE if it all worked, FALSE if it didn't.

   e.g.
      foo->MoveToTail(foo->GetHeadPosition(), bar);
          moves one element from the head of foo to the tail of bar
      foo->MoveToTail(NULL, bar);
          is a no-op
      foo->MoveToTail(foo->GetTailPosition, bar);
          concatenates foo onto the end of bar and empties foo.

   A better, except excessively long name might be
       MoveElementsFromHeadThroughPositionToOtherTail
*/
BOOL CBaseList::MoveToTail
        (__in_opt POSITION pos, __in CBaseList *pList)
{
    /* Algorithm:
       Note that the elements (including their order) in the concatenation
       of *pList to the head of *this is invariant.
       1. Count elements to be moved
       2. Join *pList onto the head of this to make one long chain
       3. Set first/Last pointers in *this and *pList
       4. Break the chain at the new place
       5. Adjust counts
       6. Set/Reset any events
    */

    if (pos==NULL) return TRUE;  // no-op.  Eliminates special cases later.


    /* Make cMove the number of nodes to move */
    CNode * p = (CNode *)pos;
    int cMove = 0;            // number of nodes to move
    while(p!=NULL) {
       p = p->Prev();
       ++cMove;
    }


    /* Join the two chains together */
    if (pList->m_pLast!=NULL)
        pList->m_pLast->SetNext(m_pFirst);
    if (m_pFirst!=NULL)
        m_pFirst->SetPrev(pList->m_pLast);


    /* set first and last pointers */
    p = (CNode *)pos;

    if (pList->m_pFirst==NULL)
        pList->m_pFirst = m_pFirst;
    m_pFirst = p->Next();
    if (m_pFirst==NULL)
        m_pLast = NULL;
    pList->m_pLast = p;


    /* Break the chain after p to create the new pieces */
    if (m_pFirst!=NULL)
        m_pFirst->SetPrev(NULL);
    p->SetNext(NULL);


    /* Adjust the counts */
    m_Count -= cMove;
    pList->m_Count += cMove;

    return TRUE;

} // MoveToTail



/* Mirror image of MoveToTail:
   Split *this before position p in *this.
   Retain in *this the head portion of the original *this
   Add the tail portion to the start (i.e. head) of *pList
   Return TRUE if it all worked, FALSE if it didn't.

   e.g.
      foo->MoveToHead(foo->GetTailPosition(), bar);
          moves one element from the tail of foo to the head of bar
      foo->MoveToHead(NULL, bar);
          is a no-op
      foo->MoveToHead(foo->GetHeadPosition, bar);
          concatenates foo onto the start of bar and empties foo.
*/
BOOL CBaseList::MoveToHead
        (__in_opt POSITION pos, __in CBaseList *pList)
{

    /* See the comments on the algorithm in MoveToTail */

    if (pos==NULL) return TRUE;  // no-op.  Eliminates special cases later.

    /* Make cMove the number of nodes to move */
    CNode * p = (CNode *)pos;
    int cMove = 0;            // number of nodes to move
    while(p!=NULL) {
       p = p->Next();
       ++cMove;
    }


    /* Join the two chains together */
    if (pList->m_pFirst!=NULL)
        pList->m_pFirst->SetPrev(m_pLast);
    if (m_pLast!=NULL)
        m_pLast->SetNext(pList->m_pFirst);


    /* set first and last pointers */
    p = (CNode *)pos;


    if (pList->m_pLast==NULL)
        pList->m_pLast = m_pLast;

    m_pLast = p->Prev();
    if (m_pLast==NULL)
        m_pFirst = NULL;
    pList->m_pFirst = p;


    /* Break the chain after p to create the new pieces */
    if (m_pLast!=NULL)
        m_pLast->SetNext(NULL);
    p->SetPrev(NULL);


    /* Adjust the counts */
    m_Count -= cMove;
    pList->m_Count += cMove;

    return TRUE;

} // MoveToHead



/* Reverse the order of the [pointers to] objects in *this
*/
void CBaseList::Reverse()
{
    /* algorithm:
       The obvious booby trap is that you flip pointers around and lose
       addressability to the node that you are going to process next.
       The easy way to avoid this is do do one chain at a time.

       Run along the forward chain,
       For each node, set the reverse pointer to the one ahead of us.
       The reverse chain is now a copy of the old forward chain, including
       the NULL termination.

       Run along the reverse chain (i.e. old forward chain again)
       For each node set the forward pointer of the node ahead to point back
       to the one we're standing on.
       The first node needs special treatment,
       it's new forward pointer is NULL.
       Finally set the First/Last pointers

    */
    CNode * p;

    // Yes we COULD use a traverse, but it would look funny!
    p = m_pFirst;
    while (p!=NULL) {
        CNode * q;
        q = p->Next();
        p->SetNext(p->Prev());
        p->SetPrev(q);
        p = q;
    }

    p = m_pFirst;
    m_pFirst = m_pLast;
    m_pLast = p;


#if 0     // old version

    if (m_pFirst==NULL) return;          // empty list
    if (m_pFirst->Next()==NULL) return;  // single node list


    /* run along forward chain */
    for ( p = m_pFirst
        ; p!=NULL
        ; p = p->Next()
        ){
        p->SetPrev(p->Next());
    }


    /* special case first element */
    m_pFirst->SetNext(NULL);     // fix the old first element


    /* run along new reverse chain i.e. old forward chain again */
    for ( p = m_pFirst           // start at the old first element
        ; p->Prev()!=NULL        // while there's a node still to be set
        ; p = p->Prev()          // work in the same direction as before
        ){
        p->Prev()->SetNext(p);
    }


    /* fix forward and reverse pointers
       - the triple XOR swap would work but all the casts look hideous */
    p = m_pFirst;
    m_pFirst = m_pLast;
    m_pLast = p;
#endif

} // Reverse

//------------------------------------------------------------------------------
// File: AMFilter.cpp
//
// Desc: DirectShow base classes - implements class hierarchy for streams
//       architecture.
//
// Copyright (c) 1992-2001 Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------

#ifdef DXMPERF
#include "dxmperf.h"
#endif // DXMPERF

//=====================================================================
//=====================================================================
// The following classes are declared in this header:
//
//
// CBaseMediaFilter            Basic IMediaFilter support (abstract class)
// CBaseFilter                 Support for IBaseFilter (incl. IMediaFilter)
// CEnumPins                   Enumerate input and output pins
// CEnumMediaTypes             Enumerate the preferred pin formats
// CBasePin                    Abstract base class for IPin interface
//    CBaseOutputPin           Adds data provider member functions
//    CBaseInputPin            Implements IMemInputPin interface
// CMediaSample                Basic transport unit for IMemInputPin
// CBaseAllocator              General list guff for most allocators
//    CMemAllocator            Implements memory buffer allocation
//
//=====================================================================
//=====================================================================

//=====================================================================
// Helpers
//=====================================================================
STDAPI CreateMemoryAllocator(__deref_out IMemAllocator **ppAllocator)
{
    return CoCreateInstance(CLSID_MemoryAllocator,
                            0,
                            CLSCTX_INPROC_SERVER,
                            IID_IMemAllocator,
                            (void **)ppAllocator);
}

//  Put this one here rather than in ctlutil.cpp to avoid linking
//  anything brought in by ctlutil.cpp
STDAPI CreatePosPassThru(
    __in_opt LPUNKNOWN pAgg,
    BOOL bRenderer,
    IPin *pPin,
    __deref_out IUnknown **ppPassThru
)
{
    *ppPassThru = NULL;
    IUnknown *pUnkSeek;
    HRESULT hr = CoCreateInstance(CLSID_SeekingPassThru,
                                  pAgg,
                                  CLSCTX_INPROC_SERVER,
                                  IID_IUnknown,
                                  (void **)&pUnkSeek
                                 );
    if (FAILED(hr)) {
        return hr;
    }

    ISeekingPassThru *pPassThru;
    hr = pUnkSeek->QueryInterface(IID_ISeekingPassThru, (void**)&pPassThru);
    if (FAILED(hr)) {
        pUnkSeek->Release();
        return hr;
    }
    hr = pPassThru->Init(bRenderer, pPin);
    pPassThru->Release();
    if (FAILED(hr)) {
        pUnkSeek->Release();
        return hr;
    }
    *ppPassThru = pUnkSeek;
    return S_OK;
}



#define CONNECT_TRACE_LEVEL 3

//=====================================================================
//=====================================================================
// Implements CBaseMediaFilter
//=====================================================================
//=====================================================================


/* Constructor */

CBaseMediaFilter::CBaseMediaFilter(__in_opt LPCTSTR pName,
                   __inout_opt LPUNKNOWN    pUnk,
                   __in CCritSec *pLock,
                   REFCLSID clsid) :
    CUnknown(pName, pUnk),
    m_pLock(pLock),
    m_clsid(clsid),
    m_State(State_Stopped),
    m_pClock(NULL)
{
}


/* Destructor */

CBaseMediaFilter::~CBaseMediaFilter()
{
    // must be stopped, but can't call Stop here since
    // our critsec has been destroyed.

    /* Release any clock we were using */

    if (m_pClock) {
        m_pClock->Release();
        m_pClock = NULL;
    }
}


/* Override this to say what interfaces we support and where */

STDMETHODIMP
CBaseMediaFilter::NonDelegatingQueryInterface(
    REFIID riid,
    __deref_out void ** ppv)
{
    if (riid == IID_IMediaFilter) {
        return GetInterface((IMediaFilter *) this, ppv);
    } else if (riid == IID_IPersist) {
        return GetInterface((IPersist *) this, ppv);
    } else {
        return CUnknown::NonDelegatingQueryInterface(riid, ppv);
    }
}

/* Return the filter's clsid */
STDMETHODIMP
CBaseMediaFilter::GetClassID(__out CLSID *pClsID)
{
    CheckPointer(pClsID,E_POINTER);
    ValidateReadWritePtr(pClsID,sizeof(CLSID));
    *pClsID = m_clsid;
    return NOERROR;
}

/* Override this if your state changes are not done synchronously */

STDMETHODIMP
CBaseMediaFilter::GetState(DWORD dwMSecs, __out FILTER_STATE *State)
{
    UNREFERENCED_PARAMETER(dwMSecs);
    CheckPointer(State,E_POINTER);
    ValidateReadWritePtr(State,sizeof(FILTER_STATE));

    *State = m_State;
    return S_OK;
}


/* Set the clock we will use for synchronisation */

STDMETHODIMP
CBaseMediaFilter::SetSyncSource(__inout_opt IReferenceClock *pClock)
{
    CAutoLock cObjectLock(m_pLock);

    // Ensure the new one does not go away - even if the same as the old
    if (pClock) {
        pClock->AddRef();
    }

    // if we have a clock, release it
    if (m_pClock) {
        m_pClock->Release();
    }

    // Set the new reference clock (might be NULL)
    // Should we query it to ensure it is a clock?  Consider for a debug build.
    m_pClock = pClock;

    return NOERROR;
}

/* Return the clock we are using for synchronisation */
STDMETHODIMP
CBaseMediaFilter::GetSyncSource(__deref_out_opt IReferenceClock **pClock)
{
    CheckPointer(pClock,E_POINTER);
    ValidateReadWritePtr(pClock,sizeof(IReferenceClock *));
    CAutoLock cObjectLock(m_pLock);

    if (m_pClock) {
        // returning an interface... addref it...
        m_pClock->AddRef();
    }
    *pClock = (IReferenceClock*)m_pClock;
    return NOERROR;
}


/* Put the filter into a stopped state */

STDMETHODIMP
CBaseMediaFilter::Stop()
{
    CAutoLock cObjectLock(m_pLock);

    m_State = State_Stopped;
    return S_OK;
}


/* Put the filter into a paused state */

STDMETHODIMP
CBaseMediaFilter::Pause()
{
    CAutoLock cObjectLock(m_pLock);

    m_State = State_Paused;
    return S_OK;
}


// Put the filter into a running state.

// The time parameter is the offset to be added to the samples'
// stream time to get the reference time at which they should be presented.
//
// you can either add these two and compare it against the reference clock,
// or you can call CBaseMediaFilter::StreamTime and compare that against
// the sample timestamp.

STDMETHODIMP
CBaseMediaFilter::Run(REFERENCE_TIME tStart)
{
    CAutoLock cObjectLock(m_pLock);

    // remember the stream time offset
    m_tStart = tStart;

    if (m_State == State_Stopped){
        HRESULT hr = Pause();

        if (FAILED(hr)) {
            return hr;
        }
    }
    m_State = State_Running;
    return S_OK;
}


//
// return the current stream time - samples with start timestamps of this
// time or before should be rendered by now
HRESULT
CBaseMediaFilter::StreamTime(CRefTime& rtStream)
{
    // Caller must lock for synchronization
    // We can't grab the filter lock because we want to be able to call
    // this from worker threads without deadlocking

    if (m_pClock == NULL) {
        return VFW_E_NO_CLOCK;
    }

    // get the current reference time
    HRESULT hr = m_pClock->GetTime((REFERENCE_TIME*)&rtStream);
    if (FAILED(hr)) {
        return hr;
    }

    // subtract the stream offset to get stream time
    rtStream -= m_tStart;

    return S_OK;
}


//=====================================================================
//=====================================================================
// Implements CBaseFilter
//=====================================================================
//=====================================================================


/* Override this to say what interfaces we support and where */

STDMETHODIMP CBaseFilter::NonDelegatingQueryInterface(REFIID riid,
                                                      __deref_out void **ppv)
{
    /* Do we have this interface */

    if (riid == IID_IBaseFilter) {
        return GetInterface((IBaseFilter *) this, ppv);
    } else if (riid == IID_IMediaFilter) {
        return GetInterface((IMediaFilter *) this, ppv);
    } else if (riid == IID_IPersist) {
        return GetInterface((IPersist *) this, ppv);
    } else if (riid == IID_IAMovieSetup) {
        return GetInterface((IAMovieSetup *) this, ppv);
    } else {
        return CUnknown::NonDelegatingQueryInterface(riid, ppv);
    }
}

#ifdef DEBUG
STDMETHODIMP_(ULONG) CBaseFilter::NonDelegatingRelease()
{
    if (m_cRef == 1) {
        KASSERT(m_pGraph == NULL);
    }
    return CUnknown::NonDelegatingRelease();
}
#endif


/* Constructor */

CBaseFilter::CBaseFilter(__in_opt LPCTSTR pName,
             __inout_opt LPUNKNOWN  pUnk,
             __in CCritSec   *pLock,
             REFCLSID   clsid) :
    CUnknown( pName, pUnk ),
    m_pLock(pLock),
    m_clsid(clsid),
    m_State(State_Stopped),
    m_pClock(NULL),
    m_pGraph(NULL),
    m_pSink(NULL),
    m_pName(NULL),
    m_PinVersion(1)
{
#ifdef DXMPERF
    PERFLOG_CTOR( pName ? pName : L"CBaseFilter", (IBaseFilter *) this );
#endif // DXMPERF

    ASSERT(pLock != NULL);
}

/* Passes in a redundant HRESULT argument */

CBaseFilter::CBaseFilter(__in_opt LPCTSTR pName,
                         __in_opt LPUNKNOWN  pUnk,
                         __in CCritSec  *pLock,
                         REFCLSID   clsid,
                         __inout HRESULT   *phr) :
    CUnknown( pName, pUnk ),
    m_pLock(pLock),
    m_clsid(clsid),
    m_State(State_Stopped),
    m_pClock(NULL),
    m_pGraph(NULL),
    m_pSink(NULL),
    m_pName(NULL),
    m_PinVersion(1)
{
#ifdef DXMPERF
    PERFLOG_CTOR( pName ? pName : L"CBaseFilter", (IBaseFilter *) this );
#endif // DXMPERF

    ASSERT(pLock != NULL);
    UNREFERENCED_PARAMETER(phr);
}

#ifdef UNICODE
CBaseFilter::CBaseFilter(__in_opt LPCSTR pName,
             __in_opt LPUNKNOWN  pUnk,
             __in CCritSec   *pLock,
             REFCLSID   clsid) :
    CUnknown( pName, pUnk ),
    m_pLock(pLock),
    m_clsid(clsid),
    m_State(State_Stopped),
    m_pClock(NULL),
    m_pGraph(NULL),
    m_pSink(NULL),
    m_pName(NULL),
    m_PinVersion(1)
{
#ifdef DXMPERF
    PERFLOG_CTOR( L"CBaseFilter", (IBaseFilter *) this );
#endif // DXMPERF

    ASSERT(pLock != NULL);
}
CBaseFilter::CBaseFilter(__in_opt LPCSTR pName,
                         __in_opt LPUNKNOWN  pUnk,
                         __in CCritSec  *pLock,
                         REFCLSID   clsid,
                         __inout HRESULT   *phr) :
    CUnknown( pName, pUnk ),
    m_pLock(pLock),
    m_clsid(clsid),
    m_State(State_Stopped),
    m_pClock(NULL),
    m_pGraph(NULL),
    m_pSink(NULL),
    m_pName(NULL),
    m_PinVersion(1)
{
#ifdef DXMPERF
    PERFLOG_CTOR( L"CBaseFilter", (IBaseFilter *) this );
#endif // DXMPERF

    ASSERT(pLock != NULL);
    UNREFERENCED_PARAMETER(phr);
}
#endif

/* Destructor */

CBaseFilter::~CBaseFilter()
{
#ifdef DXMPERF
    PERFLOG_DTOR( L"CBaseFilter", (IBaseFilter *) this );
#endif // DXMPERF

    // NOTE we do NOT hold references on the filtergraph for m_pGraph or m_pSink
    // When we did we had the circular reference problem.  Nothing would go away.

    delete[] m_pName;

    // must be stopped, but can't call Stop here since
    // our critsec has been destroyed.

    /* Release any clock we were using */
    if (m_pClock) {
        m_pClock->Release();
        m_pClock = NULL;
    }
}

/* Return the filter's clsid */
STDMETHODIMP
CBaseFilter::GetClassID(__out CLSID *pClsID)
{
    CheckPointer(pClsID,E_POINTER);
    ValidateReadWritePtr(pClsID,sizeof(CLSID));
    *pClsID = m_clsid;
    return NOERROR;
}

/* Override this if your state changes are not done synchronously */
STDMETHODIMP
CBaseFilter::GetState(DWORD dwMSecs, __out FILTER_STATE *State)
{
    UNREFERENCED_PARAMETER(dwMSecs);
    CheckPointer(State,E_POINTER);
    ValidateReadWritePtr(State,sizeof(FILTER_STATE));

    *State = m_State;
    return S_OK;
}


/* Set the clock we will use for synchronisation */

STDMETHODIMP
CBaseFilter::SetSyncSource(__in_opt IReferenceClock *pClock)
{
    CAutoLock cObjectLock(m_pLock);

    // Ensure the new one does not go away - even if the same as the old
    if (pClock) {
        pClock->AddRef();
    }

    // if we have a clock, release it
    if (m_pClock) {
        m_pClock->Release();
    }

    // Set the new reference clock (might be NULL)
    // Should we query it to ensure it is a clock?  Consider for a debug build.
    m_pClock = pClock;

    return NOERROR;
}

/* Return the clock we are using for synchronisation */
STDMETHODIMP
CBaseFilter::GetSyncSource(__deref_out_opt IReferenceClock **pClock)
{
    CheckPointer(pClock,E_POINTER);
    ValidateReadWritePtr(pClock,sizeof(IReferenceClock *));
    CAutoLock cObjectLock(m_pLock);

    if (m_pClock) {
        // returning an interface... addref it...
        m_pClock->AddRef();
    }
    *pClock = (IReferenceClock*)m_pClock;
    return NOERROR;
}



// override CBaseMediaFilter Stop method, to deactivate any pins this
// filter has.
STDMETHODIMP
CBaseFilter::Stop()
{
    CAutoLock cObjectLock(m_pLock);
    HRESULT hr = NOERROR;

    // notify all pins of the state change
    if (m_State != State_Stopped) {
        int cPins = GetPinCount();
        for (int c = 0; c < cPins; c++) {

            CBasePin *pPin = GetPin(c);
            if (NULL == pPin) {
                break;
            }

            // Disconnected pins are not activated - this saves pins worrying
            // about this state themselves. We ignore the return code to make
            // sure everyone is inactivated regardless. The base input pin
            // class can return an error if it has no allocator but Stop can
            // be used to resync the graph state after something has gone bad

            if (pPin->IsConnected()) {
                HRESULT hrTmp = pPin->Inactive();
                if (FAILED(hrTmp) && SUCCEEDED(hr)) {
                    hr = hrTmp;
                }
            }
        }
    }

#ifdef DXMPERF
    PERFLOG_STOP( m_pName ? m_pName : L"CBaseFilter", (IBaseFilter *) this, m_State );
#endif // DXMPERF

    m_State = State_Stopped;
    return hr;
}


// override CBaseMediaFilter Pause method to activate any pins
// this filter has (also called from Run)

STDMETHODIMP
CBaseFilter::Pause()
{
    CAutoLock cObjectLock(m_pLock);

    // notify all pins of the change to active state
    if (m_State == State_Stopped) {
        int cPins = GetPinCount();
        for (int c = 0; c < cPins; c++) {

            CBasePin *pPin = GetPin(c);
            if (NULL == pPin) {
                break;
            }

            // Disconnected pins are not activated - this saves pins
            // worrying about this state themselves

            if (pPin->IsConnected()) {
                HRESULT hr = pPin->Active();
                if (FAILED(hr)) {
                    return hr;
                }
            }
        }
    }


#ifdef DXMPERF
    PERFLOG_PAUSE( m_pName ? m_pName : L"CBaseFilter", (IBaseFilter *) this, m_State );
#endif // DXMPERF

    m_State = State_Paused;
    return S_OK;
}

// Put the filter into a running state.

// The time parameter is the offset to be added to the samples'
// stream time to get the reference time at which they should be presented.
//
// you can either add these two and compare it against the reference clock,
// or you can call CBaseFilter::StreamTime and compare that against
// the sample timestamp.

STDMETHODIMP
CBaseFilter::Run(REFERENCE_TIME tStart)
{
    CAutoLock cObjectLock(m_pLock);

    // remember the stream time offset
    m_tStart = tStart;

    if (m_State == State_Stopped){
    HRESULT hr = Pause();

    if (FAILED(hr)) {
        return hr;
    }
    }
    // notify all pins of the change to active state
    if (m_State != State_Running) {
        int cPins = GetPinCount();
        for (int c = 0; c < cPins; c++) {

            CBasePin *pPin = GetPin(c);
            if (NULL == pPin) {
                break;
            }

            // Disconnected pins are not activated - this saves pins
            // worrying about this state themselves

            if (pPin->IsConnected()) {
                HRESULT hr = pPin->Run(tStart);
                if (FAILED(hr)) {
                    return hr;
                }
            }
        }
    }

#ifdef DXMPERF
    PERFLOG_RUN( m_pName ? m_pName : L"CBaseFilter", (IBaseFilter *) this, tStart, m_State );
#endif // DXMPERF

    m_State = State_Running;
    return S_OK;
}

//
// return the current stream time - samples with start timestamps of this
// time or before should be rendered by now
HRESULT
CBaseFilter::StreamTime(CRefTime& rtStream)
{
    // Caller must lock for synchronization
    // We can't grab the filter lock because we want to be able to call
    // this from worker threads without deadlocking

    if (m_pClock == NULL) {
        return VFW_E_NO_CLOCK;
    }

    // get the current reference time
    HRESULT hr = m_pClock->GetTime((REFERENCE_TIME*)&rtStream);
    if (FAILED(hr)) {
        return hr;
    }

    // subtract the stream offset to get stream time
    rtStream -= m_tStart;

    return S_OK;
}


/* Create an enumerator for the pins attached to this filter */

STDMETHODIMP
CBaseFilter::EnumPins(__deref_out IEnumPins **ppEnum)
{
    CheckPointer(ppEnum,E_POINTER);
    ValidateReadWritePtr(ppEnum,sizeof(IEnumPins *));

    /* Create a new ref counted enumerator */

    *ppEnum = new CEnumPins(this,
                        NULL);

    return *ppEnum == NULL ? E_OUTOFMEMORY : NOERROR;
}


// default behaviour of FindPin is to assume pins are named
// by their pin names
STDMETHODIMP
CBaseFilter::FindPin(
    LPCWSTR Id,
    __deref_out IPin ** ppPin
)
{
    CheckPointer(ppPin,E_POINTER);
    ValidateReadWritePtr(ppPin,sizeof(IPin *));

    //  We're going to search the pin list so maintain integrity
    CAutoLock lck(m_pLock);
    int iCount = GetPinCount();
    for (int i = 0; i < iCount; i++) {
        CBasePin *pPin = GetPin(i);
        if (NULL == pPin) {
            break;
        }

        if (0 == lstrcmpW(pPin->Name(), Id)) {
            //  Found one that matches
            //
            //  AddRef() and return it
            *ppPin = pPin;
            pPin->AddRef();
            return S_OK;
        }
    }
    *ppPin = NULL;
    return VFW_E_NOT_FOUND;
}

/* Return information about this filter */

STDMETHODIMP
CBaseFilter::QueryFilterInfo(__out FILTER_INFO * pInfo)
{
    CheckPointer(pInfo,E_POINTER);
    ValidateReadWritePtr(pInfo,sizeof(FILTER_INFO));

    if (m_pName) {
        (void)StringCchCopyW(pInfo->achName, NUMELMS(pInfo->achName), m_pName);
    } else {
        pInfo->achName[0] = L'\0';
    }
    pInfo->pGraph = m_pGraph;
    if (m_pGraph)
        m_pGraph->AddRef();
    return NOERROR;
}


/* Provide the filter with a filter graph */

STDMETHODIMP
CBaseFilter::JoinFilterGraph(
    __inout_opt IFilterGraph * pGraph,
    __in_opt LPCWSTR pName)
{
    CAutoLock cObjectLock(m_pLock);

    // NOTE: we no longer hold references on the graph (m_pGraph, m_pSink)

    m_pGraph = pGraph;
    if (m_pGraph) {
        HRESULT hr = m_pGraph->QueryInterface(IID_IMediaEventSink,
                        (void**) &m_pSink);
        if (FAILED(hr)) {
            ASSERT(m_pSink == NULL);
        }
        else m_pSink->Release();        // we do NOT keep a reference on it.
    } else {
        // if graph pointer is null, then we should
        // also release the IMediaEventSink on the same object - we don't
        // refcount it, so just set it to null
        m_pSink = NULL;
    }


    if (m_pName) {
        delete[] m_pName;
        m_pName = NULL;
    }

    if (pName) {
        size_t namelen;
        HRESULT hr = StringCchLengthW(pName, STRSAFE_MAX_CCH, &namelen);
        if (FAILED(hr)) {
            return hr;
        }
        m_pName = new WCHAR[namelen + 1];
        if (m_pName) {
            (void)StringCchCopyW(m_pName, namelen + 1, pName);
        } else {
            return E_OUTOFMEMORY;
        }
    }

#ifdef DXMPERF
    PERFLOG_JOINGRAPH( m_pName ? m_pName : L"CBaseFilter",(IBaseFilter *) this, pGraph );
#endif // DXMPERF

    return NOERROR;
}


// return a Vendor information string. Optional - may return E_NOTIMPL.
// memory returned should be freed using CoTaskMemFree
// default implementation returns E_NOTIMPL
STDMETHODIMP
CBaseFilter::QueryVendorInfo(
    __deref_out LPWSTR* pVendorInfo)
{
    UNREFERENCED_PARAMETER(pVendorInfo);
    return E_NOTIMPL;
}


// send an event notification to the filter graph if we know about it.
// returns S_OK if delivered, S_FALSE if the filter graph does not sink
// events, or an error otherwise.
HRESULT
CBaseFilter::NotifyEvent(
    long EventCode,
    LONG_PTR EventParam1,
    LONG_PTR EventParam2)
{
    // Snapshot so we don't have to lock up
    IMediaEventSink *pSink = m_pSink;
    if (pSink) {
        if (EC_COMPLETE == EventCode) {
            EventParam2 = (LONG_PTR)(IBaseFilter*)this;
        }

        return pSink->Notify(EventCode, EventParam1, EventParam2);
    } else {
        return E_NOTIMPL;
    }
}

// Request reconnect
// pPin is the pin to reconnect
// pmt is the type to reconnect with - can be NULL
// Calls ReconnectEx on the filter graph
HRESULT
CBaseFilter::ReconnectPin(
    IPin *pPin,
    __in_opt AM_MEDIA_TYPE const *pmt
)
{
    IFilterGraph2 *pGraph2;
    if (m_pGraph != NULL) {
        HRESULT hr = m_pGraph->QueryInterface(IID_IFilterGraph2, (void **)&pGraph2);
        if (SUCCEEDED(hr)) {
            hr = pGraph2->ReconnectEx(pPin, pmt);
            pGraph2->Release();
            return hr;
        } else {
            return m_pGraph->Reconnect(pPin);
        }
    } else {
        return E_NOINTERFACE;
    }
}



/* This is the same idea as the media type version does for type enumeration
   on pins but for the list of pins available. So if the list of pins you
   provide changes dynamically then either override this virtual function
   to provide the version number, or more simply call IncrementPinVersion */

LONG CBaseFilter::GetPinVersion()
{
    return m_PinVersion;
}


/* Increment the current pin version cookie */

void CBaseFilter::IncrementPinVersion()
{
    InterlockedIncrement(&m_PinVersion);
}

/* register filter */

STDMETHODIMP CBaseFilter::Register()
{
    // get setup data, if it exists
    //
    LPAMOVIESETUP_FILTER psetupdata = GetSetupData();

    // check we've got data
    //
    if( NULL == psetupdata ) return S_FALSE;

    // init is ref counted so call just in case
    // we're being called cold.
    //
    HRESULT hr = CoInitialize( (LPVOID)NULL );
    ASSERT( SUCCEEDED(hr) );

    // get hold of IFilterMapper
    //
    IFilterMapper *pIFM;
    hr = CoCreateInstance( CLSID_FilterMapper
                             , NULL
                             , CLSCTX_INPROC_SERVER
                             , IID_IFilterMapper
                             , (void **)&pIFM       );
    if( SUCCEEDED(hr) )
    {
        hr = AMovieSetupRegisterFilter( psetupdata, pIFM, TRUE );
        pIFM->Release();
    }

    // and clear up
    //
    CoFreeUnusedLibraries();
    CoUninitialize();

    return NOERROR;
}


/* unregister filter */

STDMETHODIMP CBaseFilter::Unregister()
{
    // get setup data, if it exists
    //
    LPAMOVIESETUP_FILTER psetupdata = GetSetupData();

    // check we've got data
    //
    if( NULL == psetupdata ) return S_FALSE;

    // OLE init is ref counted so call
    // just in case we're being called cold.
    //
    HRESULT hr = CoInitialize( (LPVOID)NULL );
    ASSERT( SUCCEEDED(hr) );

    // get hold of IFilterMapper
    //
    IFilterMapper *pIFM;
    hr = CoCreateInstance( CLSID_FilterMapper
                             , NULL
                             , CLSCTX_INPROC_SERVER
                             , IID_IFilterMapper
                             , (void **)&pIFM       );
    if( SUCCEEDED(hr) )
    {
        hr = AMovieSetupRegisterFilter( psetupdata, pIFM, FALSE );

        // release interface
        //
        pIFM->Release();
    }

    // clear up
    //
    CoFreeUnusedLibraries();
    CoUninitialize();

    // handle one acceptable "error" - that
    // of filter not being registered!
    // (couldn't find a suitable #define'd
    // name for the error!)
    //
    if( 0x80070002 == hr)
      return NOERROR;
    else
      return hr;
}


//=====================================================================
//=====================================================================
// Implements CEnumPins
//=====================================================================
//=====================================================================


CEnumPins::CEnumPins(__in CBaseFilter *pFilter,
                     __in_opt CEnumPins *pEnumPins) :
    m_Position(0),
    m_PinCount(0),
    m_pFilter(pFilter),
    m_cRef(1),               // Already ref counted
    m_PinCache(NAME("Pin Cache"))
{

#ifdef DEBUG
    m_dwCookie = DbgRegisterObjectCreation("CEnumPins", 0);
#endif

    /* We must be owned by a filter derived from CBaseFilter */

    ASSERT(pFilter != NULL);

    /* Hold a reference count on our filter */
    m_pFilter->AddRef();

    /* Are we creating a new enumerator */

    if (pEnumPins == NULL) {
        m_Version = m_pFilter->GetPinVersion();
        m_PinCount = m_pFilter->GetPinCount();
    } else {
        ASSERT(m_Position <= m_PinCount);
        m_Position = pEnumPins->m_Position;
        m_PinCount = pEnumPins->m_PinCount;
        m_Version = pEnumPins->m_Version;
        m_PinCache.AddTail(&(pEnumPins->m_PinCache));
    }
}


/* Destructor releases the reference count on our filter NOTE since we hold
   a reference count on the filter who created us we know it is safe to
   release it, no access can be made to it afterwards though as we have just
   caused the last reference count to go and the object to be deleted */

CEnumPins::~CEnumPins()
{
    m_pFilter->Release();

#ifdef DEBUG
    DbgRegisterObjectDestruction(m_dwCookie);
#endif
}


/* Override this to say what interfaces we support where */

STDMETHODIMP
CEnumPins::QueryInterface(REFIID riid, __deref_out void **ppv)
{
    CheckPointer(ppv, E_POINTER);

    /* Do we have this interface */

    if (riid == IID_IEnumPins || riid == IID_IUnknown) {
        return GetInterface((IEnumPins *) this, ppv);
    } else {
        *ppv = NULL;
        return E_NOINTERFACE;
    }
}

STDMETHODIMP_(ULONG)
CEnumPins::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG)
CEnumPins::Release()
{
    ULONG cRef = InterlockedDecrement(&m_cRef);
    if (cRef == 0) {
        delete this;
    }
    return cRef;
}

/* One of an enumerator's basic member functions allows us to create a cloned
   interface that initially has the same state. Since we are taking a snapshot
   of an object (current position and all) we must lock access at the start */

STDMETHODIMP 
CEnumPins::Clone(__deref_out IEnumPins **ppEnum)
{
    CheckPointer(ppEnum,E_POINTER);
    ValidateReadWritePtr(ppEnum,sizeof(IEnumPins *));
    HRESULT hr = NOERROR;

    /* Check we are still in sync with the filter */
    if (AreWeOutOfSync() == TRUE) {
        *ppEnum = NULL;
        hr =  VFW_E_ENUM_OUT_OF_SYNC;
    } else {
        *ppEnum = new CEnumPins(m_pFilter, 
                                this);
        if (*ppEnum == NULL) {
            hr = E_OUTOFMEMORY;
        }
    }
    return hr;
}


/* Return the next pin after the current position */

STDMETHODIMP
CEnumPins::Next(ULONG cPins,        // place this many pins...
        __out_ecount(cPins) IPin **ppPins,      // ...in this array
        __out_opt ULONG *pcFetched)   // actual count passed returned here
{
    CheckPointer(ppPins,E_POINTER);
    ValidateReadWritePtr(ppPins,cPins * sizeof(IPin *));

    ASSERT(ppPins);

    if (pcFetched!=NULL) {
        ValidateWritePtr(pcFetched, sizeof(ULONG));
        *pcFetched = 0;           // default unless we succeed
    }
    // now check that the parameter is valid
    else if (cPins>1) {   // pcFetched == NULL
        return E_INVALIDARG;
    }
    ULONG cFetched = 0;           // increment as we get each one.

    /* Check we are still in sync with the filter */
    if (AreWeOutOfSync() == TRUE) {
        // If we are out of sync, we should refresh the enumerator.
        // This will reset the position and update the other members, but
        // will not clear cache of pins we have already returned.
        Refresh();
    }

    /* Return each pin interface NOTE GetPin returns CBasePin * not addrefed
       so we must QI for the IPin (which increments its reference count)
       If while we are retrieving a pin from the filter an error occurs we
       assume that our internal state is stale with respect to the filter
       (for example someone has deleted a pin) so we
       return VFW_E_ENUM_OUT_OF_SYNC                            */

    while (cFetched < cPins && m_PinCount > m_Position) {

        /* Get the next pin object from the filter */

        CBasePin *pPin = m_pFilter->GetPin(m_Position++);
        if (pPin == NULL) {
            // If this happend, and it's not the first time through, then we've got a problem,
            // since we should really go back and release the iPins, which we have previously
            // AddRef'ed.
            ASSERT( cFetched==0 );
            return VFW_E_ENUM_OUT_OF_SYNC;
        }

        /* We only want to return this pin, if it is not in our cache */
        if (0 == m_PinCache.Find(pPin))
        {
            /* From the object get an IPin interface */

            *ppPins = pPin;
            pPin->AddRef();

            cFetched++;
            ppPins++;

            m_PinCache.AddTail(pPin);
        }
    }

    if (pcFetched!=NULL) {
        *pcFetched = cFetched;
    }

    return (cPins==cFetched ? NOERROR : S_FALSE);
}


/* Skip over one or more entries in the enumerator */

STDMETHODIMP
CEnumPins::Skip(ULONG cPins)
{
    /* Check we are still in sync with the filter */
    if (AreWeOutOfSync() == TRUE) {
        return VFW_E_ENUM_OUT_OF_SYNC;
    }

    /* Work out how many pins are left to skip over */
    /* We could position at the end if we are asked to skip too many... */
    /* ..which would match the base implementation for CEnumMediaTypes::Skip */

    ULONG PinsLeft = m_PinCount - m_Position;
    if (cPins > PinsLeft) {
        return S_FALSE;
    }
    m_Position += cPins;
    return NOERROR;
}


/* Set the current position back to the start */
/* Reset has 4 simple steps:
 *
 * Set position to head of list
 * Sync enumerator with object being enumerated
 * Clear the cache of pins already returned
 * return S_OK
 */

STDMETHODIMP
CEnumPins::Reset()
{
    m_Version = m_pFilter->GetPinVersion();
    m_PinCount = m_pFilter->GetPinCount();

    m_Position = 0;

    // Clear the cache
    m_PinCache.RemoveAll();

    return S_OK;
}


/* Set the current position back to the start */
/* Refresh has 3 simple steps:
 *
 * Set position to head of list
 * Sync enumerator with object being enumerated
 * return S_OK
 */

STDMETHODIMP
CEnumPins::Refresh()
{
    m_Version = m_pFilter->GetPinVersion();
    m_PinCount = m_pFilter->GetPinCount();

    m_Position = 0;
    return S_OK;
}


//=====================================================================
//=====================================================================
// Implements CEnumMediaTypes
//=====================================================================
//=====================================================================


CEnumMediaTypes::CEnumMediaTypes(__in CBasePin *pPin,
                                 __in_opt CEnumMediaTypes *pEnumMediaTypes) :
    m_Position(0),
    m_pPin(pPin),
    m_cRef(1)
{

#ifdef DEBUG
    m_dwCookie = DbgRegisterObjectCreation("CEnumMediaTypes", 0);
#endif

    /* We must be owned by a pin derived from CBasePin */

    ASSERT(pPin != NULL);

    /* Hold a reference count on our pin */
    m_pPin->AddRef();

    /* Are we creating a new enumerator */

    if (pEnumMediaTypes == NULL) {
        m_Version = m_pPin->GetMediaTypeVersion();
        return;
    }

    m_Position = pEnumMediaTypes->m_Position;
    m_Version = pEnumMediaTypes->m_Version;
}


/* Destructor releases the reference count on our base pin. NOTE since we hold
   a reference count on the pin who created us we know it is safe to release
   it, no access can be made to it afterwards though as we might have just
   caused the last reference count to go and the object to be deleted */

CEnumMediaTypes::~CEnumMediaTypes()
{
#ifdef DEBUG
    DbgRegisterObjectDestruction(m_dwCookie);
#endif
    m_pPin->Release();
}


/* Override this to say what interfaces we support where */

STDMETHODIMP
CEnumMediaTypes::QueryInterface(REFIID riid, __deref_out void **ppv)
{
    CheckPointer(ppv, E_POINTER);

    /* Do we have this interface */

    if (riid == IID_IEnumMediaTypes || riid == IID_IUnknown) {
        return GetInterface((IEnumMediaTypes *) this, ppv);
    } else {
        *ppv = NULL;
        return E_NOINTERFACE;
    }
}

STDMETHODIMP_(ULONG)
CEnumMediaTypes::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG)
CEnumMediaTypes::Release()
{
    ULONG cRef = InterlockedDecrement(&m_cRef);
    if (cRef == 0) {
        delete this;
    }
    return cRef;
}

/* One of an enumerator's basic member functions allows us to create a cloned
   interface that initially has the same state. Since we are taking a snapshot
   of an object (current position and all) we must lock access at the start */

STDMETHODIMP
CEnumMediaTypes::Clone(__deref_out IEnumMediaTypes **ppEnum)
{
    CheckPointer(ppEnum,E_POINTER);
    ValidateReadWritePtr(ppEnum,sizeof(IEnumMediaTypes *));
    HRESULT hr = NOERROR;

    /* Check we are still in sync with the pin */
    if (AreWeOutOfSync() == TRUE) {
        *ppEnum = NULL;
        hr = VFW_E_ENUM_OUT_OF_SYNC;
    } else {

        *ppEnum = new CEnumMediaTypes(m_pPin,
                                      this);

        if (*ppEnum == NULL) {
            hr =  E_OUTOFMEMORY;
        }
    }
    return hr;
}


/* Enumerate the next pin(s) after the current position. The client using this
   interface passes in a pointer to an array of pointers each of which will
   be filled in with a pointer to a fully initialised media type format
   Return NOERROR if it all works,
          S_FALSE if fewer than cMediaTypes were enumerated.
          VFW_E_ENUM_OUT_OF_SYNC if the enumerator has been broken by
                                 state changes in the filter
   The actual count always correctly reflects the number of types in the array.
*/

STDMETHODIMP
CEnumMediaTypes::Next(ULONG cMediaTypes,          // place this many types...
                      __out_ecount(cMediaTypes) AM_MEDIA_TYPE **ppMediaTypes,   // ...in this array
                      __out ULONG *pcFetched)           // actual count passed
{
    CheckPointer(ppMediaTypes,E_POINTER);
    ValidateReadWritePtr(ppMediaTypes,cMediaTypes * sizeof(AM_MEDIA_TYPE *));
    /* Check we are still in sync with the pin */
    if (AreWeOutOfSync() == TRUE) {
        return VFW_E_ENUM_OUT_OF_SYNC;
    }

    if (pcFetched!=NULL) {
        ValidateWritePtr(pcFetched, sizeof(ULONG));
        *pcFetched = 0;           // default unless we succeed
    }
    // now check that the parameter is valid
    else if (cMediaTypes>1) {     // pcFetched == NULL
        return E_INVALIDARG;
    }
    ULONG cFetched = 0;           // increment as we get each one.

    /* Return each media type by asking the filter for them in turn - If we
       have an error code retured to us while we are retrieving a media type
       we assume that our internal state is stale with respect to the filter
       (for example the window size changing) so we return
       VFW_E_ENUM_OUT_OF_SYNC */

    while (cMediaTypes) {

        CMediaType cmt;

        HRESULT hr = m_pPin->GetMediaType(m_Position++, &cmt);
        if (S_OK != hr) {
            break;
        }

        /* We now have a CMediaType object that contains the next media type
           but when we assign it to the array position we CANNOT just assign
           the AM_MEDIA_TYPE structure because as soon as the object goes out of
           scope it will delete the memory we have just copied. The function
           we use is CreateMediaType which allocates a task memory block */

        /*  Transfer across the format block manually to save an allocate
            and free on the format block and generally go faster */

        *ppMediaTypes = (AM_MEDIA_TYPE *)CoTaskMemAlloc(sizeof(AM_MEDIA_TYPE));
        if (*ppMediaTypes == NULL) {
            break;
        }

        /*  Do a regular copy */
        **ppMediaTypes = cmt;

        /*  Make sure the destructor doesn't free these */
        cmt.pbFormat = NULL;
        cmt.cbFormat = NULL;
        cmt.pUnk     = NULL;


        ppMediaTypes++;
        cFetched++;
        cMediaTypes--;
    }

    if (pcFetched!=NULL) {
        *pcFetched = cFetched;
    }

    return ( cMediaTypes==0 ? NOERROR : S_FALSE );
}


/* Skip over one or more entries in the enumerator */

STDMETHODIMP
CEnumMediaTypes::Skip(ULONG cMediaTypes)
{
    //  If we're skipping 0 elements we're guaranteed to skip the
    //  correct number of elements
    if (cMediaTypes == 0) {
        return S_OK;
    }

    /* Check we are still in sync with the pin */
    if (AreWeOutOfSync() == TRUE) {
        return VFW_E_ENUM_OUT_OF_SYNC;
    }

    m_Position += cMediaTypes;

    /*  See if we're over the end */
    CMediaType cmt;
    return S_OK == m_pPin->GetMediaType(m_Position - 1, &cmt) ? S_OK : S_FALSE;
}


/* Set the current position back to the start */
/* Reset has 3 simple steps:
 *
 * set position to head of list
 * sync enumerator with object being enumerated
 * return S_OK
 */

STDMETHODIMP
CEnumMediaTypes::Reset()

{
    m_Position = 0;

    // Bring the enumerator back into step with the current state.  This
    // may be a noop but ensures that the enumerator will be valid on the
    // next call.
    m_Version = m_pPin->GetMediaTypeVersion();
    return NOERROR;
}


//=====================================================================
//=====================================================================
// Implements CBasePin
//=====================================================================
//=====================================================================


/* NOTE The implementation of this class calls the CUnknown constructor with
   a NULL outer unknown pointer. This has the effect of making us a self
   contained class, ie any QueryInterface, AddRef or Release calls will be
   routed to the class's NonDelegatingUnknown methods. You will typically
   find that the classes that do this then override one or more of these
   virtual functions to provide more specialised behaviour. A good example
   of this is where a class wants to keep the QueryInterface internal but
   still wants its lifetime controlled by the external object */

/* Constructor */

CBasePin::CBasePin(__in_opt LPCTSTR pObjectName,
           __in CBaseFilter *pFilter,
           __in CCritSec *pLock,
           __inout HRESULT *phr,
           __in_opt LPCWSTR pName,
           PIN_DIRECTION dir) :
    CUnknown( pObjectName, NULL ),
    m_pFilter(pFilter),
    m_pLock(pLock),
    m_pName(NULL),
    m_Connected(NULL),
    m_dir(dir),
    m_bRunTimeError(FALSE),
    m_pQSink(NULL),
    m_TypeVersion(1),
    m_tStart(),
    m_tStop(MAX_TIME),
    m_bCanReconnectWhenActive(false),
    m_bTryMyTypesFirst(false),
    m_dRate(1.0)
{
    /*  WARNING - pFilter is often not a properly constituted object at
        this state (in particular QueryInterface may not work) - this
        is because its owner is often its containing object and we
        have been called from the containing object's constructor so
        the filter's owner has not yet had its CUnknown constructor
        called
    */
#ifdef DXMPERF
    PERFLOG_CTOR( pName ? pName : L"CBasePin", (IPin *) this );
#endif // DXMPERF

    ASSERT(pFilter != NULL);
    ASSERT(pLock != NULL);

    if (pName) {
        size_t cchName;
        HRESULT hr = StringCchLengthW(pName, STRSAFE_MAX_CCH, &cchName);
        if (SUCCEEDED(hr)) {
            m_pName = new WCHAR[cchName + 1];
            if (m_pName) {
                (void)StringCchCopyW(m_pName, cchName + 1, pName);
            }
        }
    }

#ifdef DEBUG
    m_cRef = 0;
#endif
}

#ifdef UNICODE
CBasePin::CBasePin(__in_opt LPCSTR pObjectName,
           __in CBaseFilter *pFilter,
           __in CCritSec *pLock,
           __inout HRESULT *phr,
           __in_opt LPCWSTR pName,
           PIN_DIRECTION dir) :
    CUnknown( pObjectName, NULL ),
    m_pFilter(pFilter),
    m_pLock(pLock),
    m_pName(NULL),
    m_Connected(NULL),
    m_dir(dir),
    m_bRunTimeError(FALSE),
    m_pQSink(NULL),
    m_TypeVersion(1),
    m_tStart(),
    m_tStop(MAX_TIME),
    m_bCanReconnectWhenActive(false),
    m_bTryMyTypesFirst(false),
    m_dRate(1.0)
{
    /*  WARNING - pFilter is often not a properly constituted object at
        this state (in particular QueryInterface may not work) - this
        is because its owner is often its containing object and we
        have been called from the containing object's constructor so
        the filter's owner has not yet had its CUnknown constructor
        called
    */
#ifdef DXMPERF
    PERFLOG_CTOR( pName ? pName : L"CBasePin", (IPin *) this );
#endif // DXMPERF

    ASSERT(pFilter != NULL);
    ASSERT(pLock != NULL);

    if (pName) {
        size_t cchName;
        HRESULT hr = StringCchLengthW(pName, STRSAFE_MAX_CCH, &cchName);
        if (SUCCEEDED(hr)) {
            m_pName = new WCHAR[cchName + 1];
            if (m_pName) {
                (void)StringCchCopyW(m_pName, cchName + 1, pName);
            }
        }
    }


#ifdef DEBUG
    m_cRef = 0;
#endif
}
#endif

/* Destructor since a connected pin holds a reference count on us there is
   no way that we can be deleted unless we are not currently connected */

CBasePin::~CBasePin()
{
#ifdef DXMPERF
    PERFLOG_DTOR( m_pName ? m_pName : L"CBasePin", (IPin *) this );
#endif // DXMPERF

    //  We don't call disconnect because if the filter is going away
    //  all the pins must have a reference count of zero so they must
    //  have been disconnected anyway - (but check the assumption)
    ASSERT(m_Connected == FALSE);

    delete[] m_pName;

    // check the internal reference count is consistent
    ASSERT(m_cRef == 0);
}


/* Override this to say what interfaces we support and where */

STDMETHODIMP
CBasePin::NonDelegatingQueryInterface(REFIID riid, __deref_out void ** ppv)
{
    /* Do we have this interface */

    if (riid == IID_IPin) {
        return GetInterface((IPin *) this, ppv);
    } else if (riid == IID_IQualityControl) {
        return GetInterface((IQualityControl *) this, ppv);
    } else {
        return CUnknown::NonDelegatingQueryInterface(riid, ppv);
    }
}


/* Override to increment the owning filter's reference count */

STDMETHODIMP_(ULONG)
CBasePin::NonDelegatingAddRef()
{
    ASSERT(InterlockedIncrement(&m_cRef) > 0);
    return m_pFilter->AddRef();
}


/* Override to decrement the owning filter's reference count */

STDMETHODIMP_(ULONG)
CBasePin::NonDelegatingRelease()
{
    ASSERT(InterlockedDecrement(&m_cRef) >= 0);
    return m_pFilter->Release();
}


/* Displays pin connection information */

#ifdef DEBUG
void
CBasePin::DisplayPinInfo(IPin *pReceivePin)
{

    if (DbgCheckModuleLevel(LOG_TRACE, CONNECT_TRACE_LEVEL)) {
        PIN_INFO ConnectPinInfo;
        PIN_INFO ReceivePinInfo;

        if (FAILED(QueryPinInfo(&ConnectPinInfo))) {
            StringCchCopyW(ConnectPinInfo.achName, sizeof(ConnectPinInfo.achName)/sizeof(WCHAR), L"Bad Pin");
        } else {
            QueryPinInfoReleaseFilter(ConnectPinInfo);
        }

        if (FAILED(pReceivePin->QueryPinInfo(&ReceivePinInfo))) {
            StringCchCopyW(ReceivePinInfo.achName, sizeof(ReceivePinInfo.achName)/sizeof(WCHAR), L"Bad Pin");
        } else {
            QueryPinInfoReleaseFilter(ReceivePinInfo);
        }

        DbgLog((LOG_TRACE, CONNECT_TRACE_LEVEL, TEXT("Trying to connect Pins :")));
        DbgLog((LOG_TRACE, CONNECT_TRACE_LEVEL, TEXT("    <%ls>"), ConnectPinInfo.achName));
        DbgLog((LOG_TRACE, CONNECT_TRACE_LEVEL, TEXT("    <%ls>"), ReceivePinInfo.achName));
    }
}
#endif


/* Displays general information on the pin media type */

#ifdef DEBUG
void CBasePin::DisplayTypeInfo(IPin *pPin, const CMediaType *pmt)
{
    UNREFERENCED_PARAMETER(pPin);
    if (DbgCheckModuleLevel(LOG_TRACE, CONNECT_TRACE_LEVEL)) {
        DbgLog((LOG_TRACE, CONNECT_TRACE_LEVEL, TEXT("Trying media type:")));
        DbgLog((LOG_TRACE, CONNECT_TRACE_LEVEL, TEXT("    major type:  %hs"),
               GuidNames[*pmt->Type()]));
        DbgLog((LOG_TRACE, CONNECT_TRACE_LEVEL, TEXT("    sub type  :  %hs"),
               GuidNames[*pmt->Subtype()]));
    }
}
#endif

/* Asked to connect to a pin. A pin is always attached to an owning filter
   object so we always delegate our locking to that object. We first of all
   retrieve a media type enumerator for the input pin and see if we accept
   any of the formats that it would ideally like, failing that we retrieve
   our enumerator and see if it will accept any of our preferred types */

STDMETHODIMP
CBasePin::Connect(
    IPin * pReceivePin,
    __in_opt const AM_MEDIA_TYPE *pmt   // optional media type
)
{
    CheckPointer(pReceivePin,E_POINTER);
    ValidateReadPtr(pReceivePin,sizeof(IPin));
    CAutoLock cObjectLock(m_pLock);
    DisplayPinInfo(pReceivePin);

    /* See if we are already connected */

    if (m_Connected) {
        DbgLog((LOG_TRACE, CONNECT_TRACE_LEVEL, TEXT("Already connected")));
        return VFW_E_ALREADY_CONNECTED;
    }

    /* See if the filter is active */
    if (!IsStopped() && !m_bCanReconnectWhenActive) {
        return VFW_E_NOT_STOPPED;
    }


    // Find a mutually agreeable media type -
    // Pass in the template media type. If this is partially specified,
    // each of the enumerated media types will need to be checked against
    // it. If it is non-null and fully specified, we will just try to connect
    // with this.

    const CMediaType * ptype = (CMediaType*)pmt;
    HRESULT hr = AgreeMediaType(pReceivePin, ptype);
    if (FAILED(hr)) {
        DbgLog((LOG_TRACE, CONNECT_TRACE_LEVEL, TEXT("Failed to agree type")));

        // Since the procedure is already returning an error code, there
        // is nothing else this function can do to report the error.
        EXECUTE_ASSERT( SUCCEEDED( BreakConnect() ) );

#ifdef DXMPERF
        PERFLOG_CONNECT( (IPin *) this, pReceivePin, hr, pmt );
#endif // DXMPERF

        return hr;
    }

    DbgLog((LOG_TRACE, CONNECT_TRACE_LEVEL, TEXT("Connection succeeded")));

#ifdef DXMPERF
    PERFLOG_CONNECT( (IPin *) this, pReceivePin, NOERROR, pmt );
#endif // DXMPERF

    return NOERROR;
}

// given a specific media type, attempt a connection (includes
// checking that the type is acceptable to this pin)
HRESULT
CBasePin::AttemptConnection(
    IPin* pReceivePin,      // connect to this pin
    const CMediaType* pmt   // using this type
)
{
    // The caller should hold the filter lock becasue this function
    // uses m_Connected.  The caller should also hold the filter lock
    // because this function calls SetMediaType(), IsStopped() and
    // CompleteConnect().
    ASSERT(CritCheckIn(m_pLock));

    // Check that the connection is valid  -- need to do this for every
    // connect attempt since BreakConnect will undo it.
    HRESULT hr = CheckConnect(pReceivePin);
    if (FAILED(hr)) {
        DbgLog((LOG_TRACE, CONNECT_TRACE_LEVEL, TEXT("CheckConnect failed")));

        // Since the procedure is already returning an error code, there
        // is nothing else this function can do to report the error.
        EXECUTE_ASSERT( SUCCEEDED( BreakConnect() ) );

        return hr;
    }

    DisplayTypeInfo(pReceivePin, pmt);

    /* Check we will accept this media type */

    hr = CheckMediaType(pmt);
    if (hr == NOERROR) {

        /*  Make ourselves look connected otherwise ReceiveConnection
            may not be able to complete the connection
        */
        m_Connected = pReceivePin;
        m_Connected->AddRef();
        hr = SetMediaType(pmt);
        if (SUCCEEDED(hr)) {
            /* See if the other pin will accept this type */

            hr = pReceivePin->ReceiveConnection((IPin *)this, pmt);
            if (SUCCEEDED(hr)) {
                /* Complete the connection */

                hr = CompleteConnect(pReceivePin);
                if (SUCCEEDED(hr)) {
                    return hr;
                } else {
                    DbgLog((LOG_TRACE,
                            CONNECT_TRACE_LEVEL,
                            TEXT("Failed to complete connection")));
                    pReceivePin->Disconnect();
                }
            }
        }
    } else {
        // we cannot use this media type

        // return a specific media type error if there is one
        // or map a general failure code to something more helpful
        // (in particular S_FALSE gets changed to an error code)
        if (SUCCEEDED(hr) ||
            (hr == E_FAIL) ||
            (hr == E_INVALIDARG)) {
            hr = VFW_E_TYPE_NOT_ACCEPTED;
        }
    }

    // BreakConnect and release any connection here in case CheckMediaType
    // failed, or if we set anything up during a call back during
    // ReceiveConnection.

    // Since the procedure is already returning an error code, there
    // is nothing else this function can do to report the error.
    EXECUTE_ASSERT( SUCCEEDED( BreakConnect() ) );

    /*  If failed then undo our state */
    if (m_Connected) {
        m_Connected->Release();
        m_Connected = NULL;
    }

    return hr;
}

/* Given an enumerator we cycle through all the media types it proposes and
   firstly suggest them to our derived pin class and if that succeeds try
   them with the pin in a ReceiveConnection call. This means that if our pin
   proposes a media type we still check in here that we can support it. This
   is deliberate so that in simple cases the enumerator can hold all of the
   media types even if some of them are not really currently available */

HRESULT CBasePin::TryMediaTypes(
    IPin *pReceivePin,
    __in_opt const CMediaType *pmt,
    IEnumMediaTypes *pEnum)
{
    /* Reset the current enumerator position */

    HRESULT hr = pEnum->Reset();
    if (FAILED(hr)) {
        return hr;
    }

    CMediaType *pMediaType = NULL;
    ULONG ulMediaCount = 0;

    // attempt to remember a specific error code if there is one
    HRESULT hrFailure = S_OK;

    for (;;) {

        /* Retrieve the next media type NOTE each time round the loop the
           enumerator interface will allocate another AM_MEDIA_TYPE structure
           If we are successful then we copy it into our output object, if
           not then we must delete the memory allocated before returning */

        hr = pEnum->Next(1, (AM_MEDIA_TYPE**)&pMediaType,&ulMediaCount);
        if (hr != S_OK) {
            if (S_OK == hrFailure) {
                hrFailure = VFW_E_NO_ACCEPTABLE_TYPES;
            }
            return hrFailure;
        }


        ASSERT(ulMediaCount == 1);
        ASSERT(pMediaType);

        // check that this matches the partial type (if any)

        if (pMediaType &&
            ((pmt == NULL) ||
            pMediaType->MatchesPartial(pmt))) {

            hr = AttemptConnection(pReceivePin, pMediaType);

            // attempt to remember a specific error code
            if (FAILED(hr) &&
            SUCCEEDED(hrFailure) &&
            (hr != E_FAIL) &&
            (hr != E_INVALIDARG) &&
            (hr != VFW_E_TYPE_NOT_ACCEPTED)) {
                hrFailure = hr;
            }
        } else {
            hr = VFW_E_NO_ACCEPTABLE_TYPES;
        }

        if(pMediaType) {
            DeleteMediaType(pMediaType);
            pMediaType = NULL;
        }

        if (S_OK == hr) {
            return hr;
        }
    }
}


/* This is called to make the connection, including the taask of finding
   a media type for the pin connection. pmt is the proposed media type
   from the Connect call: if this is fully specified, we will try that.
   Otherwise we enumerate and try all the input pin's types first and
   if that fails we then enumerate and try all our preferred media types.
   For each media type we check it against pmt (if non-null and partially
   specified) as well as checking that both pins will accept it.
 */

HRESULT CBasePin::AgreeMediaType(
    IPin *pReceivePin,
    const CMediaType *pmt)
{
    ASSERT(pReceivePin);
    IEnumMediaTypes *pEnumMediaTypes = NULL;

    // if the media type is fully specified then use that
    if ( (pmt != NULL) && (!pmt->IsPartiallySpecified())) {

        // if this media type fails, then we must fail the connection
        // since if pmt is nonnull we are only allowed to connect
        // using a type that matches it.

        return AttemptConnection(pReceivePin, pmt);
    }


    /* Try the other pin's enumerator */

    HRESULT hrFailure = VFW_E_NO_ACCEPTABLE_TYPES;

    for (int i = 0; i < 2; i++) {
        HRESULT hr;
        if (i == (int)m_bTryMyTypesFirst) {
            hr = pReceivePin->EnumMediaTypes(&pEnumMediaTypes);
        } else {
            hr = EnumMediaTypes(&pEnumMediaTypes);
        }
        if (SUCCEEDED(hr)) {
            ASSERT(pEnumMediaTypes);
            hr = TryMediaTypes(pReceivePin,pmt,pEnumMediaTypes);
            pEnumMediaTypes->Release();
            if (SUCCEEDED(hr)) {
                return NOERROR;
            } else {
                // try to remember specific error codes if there are any
                if ((hr != E_FAIL) &&
                    (hr != E_INVALIDARG) &&
                    (hr != VFW_E_TYPE_NOT_ACCEPTED)) {
                    hrFailure = hr;
                }
            }
        }
    }

    return hrFailure;
}


/* Called when we want to complete a connection to another filter. Failing
   this will also fail the connection and disconnect the other pin as well */

HRESULT
CBasePin::CompleteConnect(IPin *pReceivePin)
{
    UNREFERENCED_PARAMETER(pReceivePin);
    return NOERROR;
}


/* This is called to set the format for a pin connection - CheckMediaType
   will have been called to check the connection format and if it didn't
   return an error code then this (virtual) function will be invoked */

HRESULT
CBasePin::SetMediaType(const CMediaType *pmt)
{
    HRESULT hr = m_mt.Set(*pmt);
    if (FAILED(hr)) {
        return hr;
    }

    return NOERROR;
}


/* This is called during Connect() to provide a virtual method that can do
   any specific check needed for connection such as QueryInterface. This
   base class method just checks that the pin directions don't match */

HRESULT
CBasePin::CheckConnect(IPin * pPin)
{
    /* Check that pin directions DONT match */

    PIN_DIRECTION pd;
    pPin->QueryDirection(&pd);

    ASSERT((pd == PINDIR_OUTPUT) || (pd == PINDIR_INPUT));
    ASSERT((m_dir == PINDIR_OUTPUT) || (m_dir == PINDIR_INPUT));

    // we should allow for non-input and non-output connections?
    if (pd == m_dir) {
        return VFW_E_INVALID_DIRECTION;
    }
    return NOERROR;
}


/* This is called when we realise we can't make a connection to the pin and
   must undo anything we did in CheckConnect - override to release QIs done */

HRESULT
CBasePin::BreakConnect()
{
    return NOERROR;
}


/* Called normally by an output pin on an input pin to try and establish a
   connection.
*/

STDMETHODIMP
CBasePin::ReceiveConnection(
    IPin * pConnector,   // this is the pin who we will connect to
    const AM_MEDIA_TYPE *pmt  // this is the media type we will exchange
)
{
    CheckPointer(pConnector,E_POINTER);
    CheckPointer(pmt,E_POINTER);
    ValidateReadPtr(pConnector,sizeof(IPin));
    ValidateReadPtr(pmt,sizeof(AM_MEDIA_TYPE));
    CAutoLock cObjectLock(m_pLock);

    /* Are we already connected */
    if (m_Connected) {
        return VFW_E_ALREADY_CONNECTED;
    }

    /* See if the filter is active */
    if (!IsStopped() && !m_bCanReconnectWhenActive) {
        return VFW_E_NOT_STOPPED;
    }

    HRESULT hr = CheckConnect(pConnector);
    if (FAILED(hr)) {
        // Since the procedure is already returning an error code, there
        // is nothing else this function can do to report the error.
        EXECUTE_ASSERT( SUCCEEDED( BreakConnect() ) );

#ifdef DXMPERF
        PERFLOG_RXCONNECT( pConnector, (IPin *) this, hr, pmt );
#endif // DXMPERF

        return hr;
    }

    /* Ask derived class if this media type is ok */

    CMediaType * pcmt = (CMediaType*) pmt;
    hr = CheckMediaType(pcmt);
    if (hr != NOERROR) {
        // no -we don't support this media type

        // Since the procedure is already returning an error code, there
        // is nothing else this function can do to report the error.
        EXECUTE_ASSERT( SUCCEEDED( BreakConnect() ) );

        // return a specific media type error if there is one
        // or map a general failure code to something more helpful
        // (in particular S_FALSE gets changed to an error code)
        if (SUCCEEDED(hr) ||
            (hr == E_FAIL) ||
            (hr == E_INVALIDARG)) {
            hr = VFW_E_TYPE_NOT_ACCEPTED;
        }

#ifdef DXMPERF
        PERFLOG_RXCONNECT( pConnector, (IPin *) this, hr, pmt );
#endif // DXMPERF

        return hr;
    }

    /* Complete the connection */

    m_Connected = pConnector;
    m_Connected->AddRef();
    hr = SetMediaType(pcmt);
    if (SUCCEEDED(hr)) {
        hr = CompleteConnect(pConnector);
        if (SUCCEEDED(hr)) {

#ifdef DXMPERF
            PERFLOG_RXCONNECT( pConnector, (IPin *) this, NOERROR, pmt );
#endif // DXMPERF

            return NOERROR;
        }
    }

    DbgLog((LOG_TRACE, CONNECT_TRACE_LEVEL, TEXT("Failed to set the media type or failed to complete the connection.")));
    m_Connected->Release();
    m_Connected = NULL;

    // Since the procedure is already returning an error code, there
    // is nothing else this function can do to report the error.
    EXECUTE_ASSERT( SUCCEEDED( BreakConnect() ) );

#ifdef DXMPERF
    PERFLOG_RXCONNECT( pConnector, (IPin *) this, hr, pmt );
#endif // DXMPERF

    return hr;
}


/* Called when we want to terminate a pin connection */

STDMETHODIMP
CBasePin::Disconnect()
{
    CAutoLock cObjectLock(m_pLock);

    /* See if the filter is active */
    if (!IsStopped()) {
        return VFW_E_NOT_STOPPED;
    }

    return DisconnectInternal();
}

STDMETHODIMP
CBasePin::DisconnectInternal()
{
    ASSERT(CritCheckIn(m_pLock));

    if (m_Connected) {
        HRESULT hr = BreakConnect();
        if( FAILED( hr ) ) {

#ifdef DXMPERF
            PERFLOG_DISCONNECT( (IPin *) this, m_Connected, hr );
#endif // DXMPERF

            // There is usually a bug in the program if BreakConnect() fails.
            DbgBreak( "WARNING: BreakConnect() failed in CBasePin::Disconnect()." );
            return hr;
        }

        m_Connected->Release();
        m_Connected = NULL;

#ifdef DXMPERF
        PERFLOG_DISCONNECT( (IPin *) this, m_Connected, S_OK );
#endif // DXMPERF

        return S_OK;
    } else {
        // no connection - not an error

#ifdef DXMPERF
        PERFLOG_DISCONNECT( (IPin *) this, m_Connected, S_FALSE );
#endif // DXMPERF

        return S_FALSE;
    }
}


/* Return an AddRef()'d pointer to the connected pin if there is one */
STDMETHODIMP
CBasePin::ConnectedTo(
    __deref_out IPin **ppPin
)
{
    CheckPointer(ppPin,E_POINTER);
    ValidateReadWritePtr(ppPin,sizeof(IPin *));
    //
    //  It's pointless to lock here.
    //  The caller should ensure integrity.
    //

    IPin *pPin = m_Connected;
    *ppPin = pPin;
    if (pPin != NULL) {
        pPin->AddRef();
        return S_OK;
    } else {
        ASSERT(*ppPin == NULL);
        return VFW_E_NOT_CONNECTED;
    }
}

/* Return the media type of the connection */
STDMETHODIMP
CBasePin::ConnectionMediaType(
    __out AM_MEDIA_TYPE *pmt
)
{
    CheckPointer(pmt,E_POINTER);
    ValidateReadWritePtr(pmt,sizeof(AM_MEDIA_TYPE));
    CAutoLock cObjectLock(m_pLock);

    /*  Copy constructor of m_mt allocates the memory */
    if (IsConnected()) {
        CopyMediaType( pmt, &m_mt );
        return S_OK;
    } else {
        ((CMediaType *)pmt)->InitMediaType();
        return VFW_E_NOT_CONNECTED;
    }
}

/* Return information about the filter we are connect to */

STDMETHODIMP
CBasePin::QueryPinInfo(
    __out PIN_INFO * pInfo
)
{
    CheckPointer(pInfo,E_POINTER);
    ValidateReadWritePtr(pInfo,sizeof(PIN_INFO));

    pInfo->pFilter = m_pFilter;
    if (m_pFilter) {
        m_pFilter->AddRef();
    }

    if (m_pName) {
        (void)StringCchCopyW(pInfo->achName, NUMELMS(pInfo->achName), m_pName);
    } else {
        pInfo->achName[0] = L'\0';
    }

    pInfo->dir = m_dir;

    return NOERROR;
}

STDMETHODIMP
CBasePin::QueryDirection(
    __out PIN_DIRECTION * pPinDir
)
{
    CheckPointer(pPinDir,E_POINTER);
    ValidateReadWritePtr(pPinDir,sizeof(PIN_DIRECTION));

    *pPinDir = m_dir;
    return NOERROR;
}

// Default QueryId to return the pin's name
STDMETHODIMP
CBasePin::QueryId(
    __deref_out LPWSTR * Id
)
{
    //  We're not going away because someone's got a pointer to us
    //  so there's no need to lock

    return AMGetWideString(Name(), Id);
}

/* Does this pin support this media type WARNING this interface function does
   not lock the main object as it is meant to be asynchronous by nature - if
   the media types you support depend on some internal state that is updated
   dynamically then you will need to implement locking in a derived class */

STDMETHODIMP
CBasePin::QueryAccept(
    const AM_MEDIA_TYPE *pmt
)
{
    CheckPointer(pmt,E_POINTER);
    ValidateReadPtr(pmt,sizeof(AM_MEDIA_TYPE));

    /* The CheckMediaType method is valid to return error codes if the media
       type is horrible, an example might be E_INVALIDARG. What we do here
       is map all the error codes into either S_OK or S_FALSE regardless */

    HRESULT hr = CheckMediaType((CMediaType*)pmt);
    if (FAILED(hr)) {
        return S_FALSE;
    }
    // note that the only defined success codes should be S_OK and S_FALSE...
    return hr;
}


/* This can be called to return an enumerator for the pin's list of preferred
   media types. An input pin is not obliged to have any preferred formats
   although it can do. For example, the window renderer has a preferred type
   which describes a video image that matches the current window size. All
   output pins should expose at least one preferred format otherwise it is
   possible that neither pin has any types and so no connection is possible */

STDMETHODIMP
CBasePin::EnumMediaTypes(
    __deref_out IEnumMediaTypes **ppEnum
)
{
    CheckPointer(ppEnum,E_POINTER);
    ValidateReadWritePtr(ppEnum,sizeof(IEnumMediaTypes *));

    /* Create a new ref counted enumerator */

    *ppEnum = new CEnumMediaTypes(this,
                              NULL);

    if (*ppEnum == NULL) {
        return E_OUTOFMEMORY;
    }

    return NOERROR;
}



/* This is a virtual function that returns a media type corresponding with
   place iPosition in the list. This base class simply returns an error as
   we support no media types by default but derived classes should override */

HRESULT CBasePin::GetMediaType(int iPosition, __inout CMediaType *pMediaType)
{
    UNREFERENCED_PARAMETER(iPosition);
    UNREFERENCED_PARAMETER(pMediaType);
    return E_UNEXPECTED;
}


/* This is a virtual function that returns the current media type version.
   The base class initialises the media type enumerators with the value 1
   By default we always returns that same value. A Derived class may change
   the list of media types available and after doing so it should increment
   the version either in a method derived from this, or more simply by just
   incrementing the m_TypeVersion base pin variable. The type enumerators
   call this when they want to see if their enumerations are out of date */

LONG CBasePin::GetMediaTypeVersion()
{
    return m_TypeVersion;
}


/* Increment the cookie representing the current media type version */

void CBasePin::IncrementTypeVersion()
{
    InterlockedIncrement(&m_TypeVersion);
}


/* Called by IMediaFilter implementation when the state changes from Stopped
   to either paused or running and in derived classes could do things like
   commit memory and grab hardware resource (the default is to do nothing) */

HRESULT
CBasePin::Active(void)
{
    return NOERROR;
}

/* Called by IMediaFilter implementation when the state changes from
   to either paused to running and in derived classes could do things like
   commit memory and grab hardware resource (the default is to do nothing) */

HRESULT
CBasePin::Run(REFERENCE_TIME tStart)
{
    UNREFERENCED_PARAMETER(tStart);
    return NOERROR;
}


/* Also called by the IMediaFilter implementation when the state changes to
   Stopped at which point you should decommit allocators and free hardware
   resources you grabbed in the Active call (default is also to do nothing) */

HRESULT
CBasePin::Inactive(void)
{
    m_bRunTimeError = FALSE;
    return NOERROR;
}


// Called when no more data will arrive
STDMETHODIMP
CBasePin::EndOfStream(void)
{
    return S_OK;
}


STDMETHODIMP
CBasePin::SetSink(IQualityControl * piqc)
{
    CAutoLock cObjectLock(m_pLock);
    if (piqc) ValidateReadPtr(piqc,sizeof(IQualityControl));
    m_pQSink = piqc;
    return NOERROR;
} // SetSink


STDMETHODIMP
CBasePin::Notify(IBaseFilter * pSender, Quality q)
{
    UNREFERENCED_PARAMETER(q);
    UNREFERENCED_PARAMETER(pSender);
    DbgBreak("IQualityControl::Notify not over-ridden from CBasePin.  (IGNORE is OK)");
    return E_NOTIMPL;
} //Notify


// NewSegment notifies of the start/stop/rate applying to the data
// about to be received. Default implementation records data and
// returns S_OK.
// Override this to pass downstream.
STDMETHODIMP
CBasePin::NewSegment(
                REFERENCE_TIME tStart,
                REFERENCE_TIME tStop,
                double dRate)
{
    m_tStart = tStart;
    m_tStop = tStop;
    m_dRate = dRate;

    return S_OK;
}


//=====================================================================
//=====================================================================
// Implements CBaseOutputPin
//=====================================================================
//=====================================================================


CBaseOutputPin::CBaseOutputPin(__in_opt LPCTSTR pObjectName,
                   __in CBaseFilter *pFilter,
                   __in CCritSec *pLock,
                   __inout HRESULT *phr,
                   __in_opt LPCWSTR pName) :
    CBasePin(pObjectName, pFilter, pLock, phr, pName, PINDIR_OUTPUT),
    m_pAllocator(NULL),
    m_pInputPin(NULL)
{
    ASSERT(pFilter);
}

#ifdef UNICODE
CBaseOutputPin::CBaseOutputPin(__in_opt LPCSTR pObjectName,
                   __in CBaseFilter *pFilter,
                   __in CCritSec *pLock,
                   __inout HRESULT *phr,
                   __in_opt LPCWSTR pName) :
    CBasePin(pObjectName, pFilter, pLock, phr, pName, PINDIR_OUTPUT),
    m_pAllocator(NULL),
    m_pInputPin(NULL)
{
    ASSERT(pFilter);
}
#endif

/*   This is called after a media type has been proposed

     Try to complete the connection by agreeing the allocator
*/
HRESULT
CBaseOutputPin::CompleteConnect(IPin *pReceivePin)
{
    UNREFERENCED_PARAMETER(pReceivePin);
    return DecideAllocator(m_pInputPin, &m_pAllocator);
}


/* This method is called when the output pin is about to try and connect to
   an input pin. It is at this point that you should try and grab any extra
   interfaces that you need, in this case IMemInputPin. Because this is
   only called if we are not currently connected we do NOT need to call
   BreakConnect. This also makes it easier to derive classes from us as
   BreakConnect is only called when we actually have to break a connection
   (or a partly made connection) and not when we are checking a connection */

/* Overriden from CBasePin */

HRESULT
CBaseOutputPin::CheckConnect(IPin * pPin)
{
    HRESULT hr = CBasePin::CheckConnect(pPin);
    if (FAILED(hr)) {
    return hr;
    }

    // get an input pin and an allocator interface
    hr = pPin->QueryInterface(IID_IMemInputPin, (void **) &m_pInputPin);
    if (FAILED(hr)) {
        return hr;
    }
    return NOERROR;
}


/* Overriden from CBasePin */

HRESULT
CBaseOutputPin::BreakConnect()
{
    /* Release any allocator we hold */

    if (m_pAllocator) {
        // Always decommit the allocator because a downstream filter may or
        // may not decommit the connection's allocator.  A memory leak could
        // occur if the allocator is not decommited when a connection is broken.
        HRESULT hr = m_pAllocator->Decommit();
        if( FAILED( hr ) ) {
            return hr;
        }

        m_pAllocator->Release();
        m_pAllocator = NULL;
    }

    /* Release any input pin interface we hold */

    if (m_pInputPin) {
        m_pInputPin->Release();
        m_pInputPin = NULL;
    }
    return NOERROR;
}


/* This is called when the input pin didn't give us a valid allocator */

HRESULT
CBaseOutputPin::InitAllocator(__deref_out IMemAllocator **ppAlloc)
{
    return CreateMemoryAllocator(ppAlloc);
}


/* Decide on an allocator, override this if you want to use your own allocator
   Override DecideBufferSize to call SetProperties. If the input pin fails
   the GetAllocator call then this will construct a CMemAllocator and call
   DecideBufferSize on that, and if that fails then we are completely hosed.
   If the you succeed the DecideBufferSize call, we will notify the input
   pin of the selected allocator. NOTE this is called during Connect() which
   therefore looks after grabbing and locking the object's critical section */

// We query the input pin for its requested properties and pass this to
// DecideBufferSize to allow it to fulfill requests that it is happy
// with (eg most people don't care about alignment and are thus happy to
// use the downstream pin's alignment request).

HRESULT
CBaseOutputPin::DecideAllocator(IMemInputPin *pPin, __deref_out IMemAllocator **ppAlloc)
{
    HRESULT hr = NOERROR;
    *ppAlloc = NULL;

    // get downstream prop request
    // the derived class may modify this in DecideBufferSize, but
    // we assume that he will consistently modify it the same way,
    // so we only get it once
    ALLOCATOR_PROPERTIES prop;
    ZeroMemory(&prop, sizeof(prop));

    // whatever he returns, we assume prop is either all zeros
    // or he has filled it out.
    pPin->GetAllocatorRequirements(&prop);

    // if he doesn't care about alignment, then set it to 1
    if (prop.cbAlign == 0) {
        prop.cbAlign = 1;
    }

    /* Try the allocator provided by the input pin */

    hr = pPin->GetAllocator(ppAlloc);
    if (SUCCEEDED(hr)) {

        hr = DecideBufferSize(*ppAlloc, &prop);
        if (SUCCEEDED(hr)) {
            hr = pPin->NotifyAllocator(*ppAlloc, FALSE);
            if (SUCCEEDED(hr)) {
                return NOERROR;
            }
        }
    }

    /* If the GetAllocator failed we may not have an interface */

    if (*ppAlloc) {
        (*ppAlloc)->Release();
        *ppAlloc = NULL;
    }

    /* Try the output pin's allocator by the same method */

    hr = InitAllocator(ppAlloc);
    if (SUCCEEDED(hr)) {

        // note - the properties passed here are in the same
        // structure as above and may have been modified by
        // the previous call to DecideBufferSize
        hr = DecideBufferSize(*ppAlloc, &prop);
        if (SUCCEEDED(hr)) {
            hr = pPin->NotifyAllocator(*ppAlloc, FALSE);
            if (SUCCEEDED(hr)) {
                return NOERROR;
            }
        }
    }

    /* Likewise we may not have an interface to release */

    if (*ppAlloc) {
        (*ppAlloc)->Release();
        *ppAlloc = NULL;
    }
    return hr;
}


/* This returns an empty sample buffer from the allocator WARNING the same
   dangers and restrictions apply here as described below for Deliver() */

HRESULT
CBaseOutputPin::GetDeliveryBuffer(__deref_out IMediaSample ** ppSample,
                                  __in_opt REFERENCE_TIME * pStartTime,
                                  __in_opt REFERENCE_TIME * pEndTime,
                                  DWORD dwFlags)
{
    if (m_pAllocator != NULL) {
        return m_pAllocator->GetBuffer(ppSample,pStartTime,pEndTime,dwFlags);
    } else {
        return E_NOINTERFACE;
    }
}


/* Deliver a filled-in sample to the connected input pin. NOTE the object must
   have locked itself before calling us otherwise we may get halfway through
   executing this method only to find the filter graph has got in and
   disconnected us from the input pin. If the filter has no worker threads
   then the lock is best applied on Receive(), otherwise it should be done
   when the worker thread is ready to deliver. There is a wee snag to worker
   threads that this shows up. The worker thread must lock the object when
   it is ready to deliver a sample, but it may have to wait until a state
   change has completed, but that may never complete because the state change
   is waiting for the worker thread to complete. The way to handle this is for
   the state change code to grab the critical section, then set an abort event
   for the worker thread, then release the critical section and wait for the
   worker thread to see the event we set and then signal that it has finished
   (with another event). At which point the state change code can complete */

// note (if you've still got any breath left after reading that) that you
// need to release the sample yourself after this call. if the connected
// input pin needs to hold onto the sample beyond the call, it will addref
// the sample itself.

// of course you must release this one and call GetDeliveryBuffer for the
// next. You cannot reuse it directly.

HRESULT
CBaseOutputPin::Deliver(IMediaSample * pSample)
{
    if (m_pInputPin == NULL) {
        return VFW_E_NOT_CONNECTED;
    }

#ifdef DXMPERF
    PERFLOG_DELIVER( m_pName ? m_pName : L"CBaseOutputPin", (IPin *) this, (IPin  *) m_pInputPin, pSample, &m_mt );
#endif // DXMPERF

    return m_pInputPin->Receive(pSample);
}


// called from elsewhere in our filter to pass EOS downstream to
// our connected input pin
HRESULT
CBaseOutputPin::DeliverEndOfStream(void)
{
    // remember this is on IPin not IMemInputPin
    if (m_Connected == NULL) {
        return VFW_E_NOT_CONNECTED;
    }
    return m_Connected->EndOfStream();
}


/* Commit the allocator's memory, this is called through IMediaFilter
   which is responsible for locking the object before calling us */

HRESULT
CBaseOutputPin::Active(void)
{
    if (m_pAllocator == NULL) {
        return VFW_E_NO_ALLOCATOR;
    }
    return m_pAllocator->Commit();
}


/* Free up or unprepare allocator's memory, this is called through
   IMediaFilter which is responsible for locking the object first */

HRESULT
CBaseOutputPin::Inactive(void)
{
    m_bRunTimeError = FALSE;
    if (m_pAllocator == NULL) {
        return VFW_E_NO_ALLOCATOR;
    }
    return m_pAllocator->Decommit();
}

// we have a default handling of EndOfStream which is to return
// an error, since this should be called on input pins only
STDMETHODIMP
CBaseOutputPin::EndOfStream(void)
{
    return E_UNEXPECTED;
}


// BeginFlush should be called on input pins only
STDMETHODIMP
CBaseOutputPin::BeginFlush(void)
{
    return E_UNEXPECTED;
}

// EndFlush should be called on input pins only
STDMETHODIMP
CBaseOutputPin::EndFlush(void)
{
    return E_UNEXPECTED;
}

// call BeginFlush on the connected input pin
HRESULT
CBaseOutputPin::DeliverBeginFlush(void)
{
    // remember this is on IPin not IMemInputPin
    if (m_Connected == NULL) {
        return VFW_E_NOT_CONNECTED;
    }
    return m_Connected->BeginFlush();
}

// call EndFlush on the connected input pin
HRESULT
CBaseOutputPin::DeliverEndFlush(void)
{
    // remember this is on IPin not IMemInputPin
    if (m_Connected == NULL) {
        return VFW_E_NOT_CONNECTED;
    }
    return m_Connected->EndFlush();
}
// deliver NewSegment to connected pin
HRESULT
CBaseOutputPin::DeliverNewSegment(
    REFERENCE_TIME tStart,
    REFERENCE_TIME tStop,
    double dRate)
{
    if (m_Connected == NULL) {
        return VFW_E_NOT_CONNECTED;
    }
    return m_Connected->NewSegment(tStart, tStop, dRate);
}


//=====================================================================
//=====================================================================
// Implements CBaseInputPin
//=====================================================================
//=====================================================================


/* Constructor creates a default allocator object */

CBaseInputPin::CBaseInputPin(__in_opt LPCTSTR pObjectName,
                 __in CBaseFilter *pFilter,
                 __in CCritSec *pLock,
                 __inout HRESULT *phr,
                 __in_opt LPCWSTR pPinName) :
    CBasePin(pObjectName, pFilter, pLock, phr, pPinName, PINDIR_INPUT),
    m_pAllocator(NULL),
    m_bReadOnly(FALSE),
    m_bFlushing(FALSE)
{
    ZeroMemory(&m_SampleProps, sizeof(m_SampleProps));
}

#ifdef UNICODE
CBaseInputPin::CBaseInputPin(__in LPCSTR pObjectName,
                 __in CBaseFilter *pFilter,
                 __in CCritSec *pLock,
                 __inout HRESULT *phr,
                 __in_opt LPCWSTR pPinName) :
    CBasePin(pObjectName, pFilter, pLock, phr, pPinName, PINDIR_INPUT),
    m_pAllocator(NULL),
    m_bReadOnly(FALSE),
    m_bFlushing(FALSE)
{
    ZeroMemory(&m_SampleProps, sizeof(m_SampleProps));
}
#endif

/* Destructor releases it's reference count on the default allocator */

CBaseInputPin::~CBaseInputPin()
{
    if (m_pAllocator != NULL) {
    m_pAllocator->Release();
    m_pAllocator = NULL;
    }
}


// override this to publicise our interfaces
STDMETHODIMP
CBaseInputPin::NonDelegatingQueryInterface(REFIID riid, __deref_out void **ppv)
{
    /* Do we know about this interface */

    if (riid == IID_IMemInputPin) {
        return GetInterface((IMemInputPin *) this, ppv);
    } else {
        return CBasePin::NonDelegatingQueryInterface(riid, ppv);
    }
}


/* Return the allocator interface that this input pin would like the output
   pin to use. NOTE subsequent calls to GetAllocator should all return an
   interface onto the SAME object so we create one object at the start

   Note:
       The allocator is Release()'d on disconnect and replaced on
       NotifyAllocator().

   Override this to provide your own allocator.
*/

STDMETHODIMP
CBaseInputPin::GetAllocator(
    __deref_out IMemAllocator **ppAllocator)
{
    CheckPointer(ppAllocator,E_POINTER);
    ValidateReadWritePtr(ppAllocator,sizeof(IMemAllocator *));
    CAutoLock cObjectLock(m_pLock);

    if (m_pAllocator == NULL) {
        HRESULT hr = CreateMemoryAllocator(&m_pAllocator);
        if (FAILED(hr)) {
            return hr;
        }
    }
    ASSERT(m_pAllocator != NULL);
    *ppAllocator = m_pAllocator;
    m_pAllocator->AddRef();
    return NOERROR;
}


/* Tell the input pin which allocator the output pin is actually going to use
   Override this if you care - NOTE the locking we do both here and also in
   GetAllocator is unnecessary but derived classes that do something useful
   will undoubtedly have to lock the object so this might help remind people */

STDMETHODIMP
CBaseInputPin::NotifyAllocator(
    IMemAllocator * pAllocator,
    BOOL bReadOnly)
{
    CheckPointer(pAllocator,E_POINTER);
    ValidateReadPtr(pAllocator,sizeof(IMemAllocator));
    CAutoLock cObjectLock(m_pLock);

    IMemAllocator *pOldAllocator = m_pAllocator;
    pAllocator->AddRef();
    m_pAllocator = pAllocator;

    if (pOldAllocator != NULL) {
        pOldAllocator->Release();
    }

    // the readonly flag indicates whether samples from this allocator should
    // be regarded as readonly - if true, then inplace transforms will not be
    // allowed.
    m_bReadOnly = (BYTE)bReadOnly;
    return NOERROR;
}


HRESULT
CBaseInputPin::BreakConnect()
{
    /* We don't need our allocator any more */
    if (m_pAllocator) {
        // Always decommit the allocator because a downstream filter may or
        // may not decommit the connection's allocator.  A memory leak could
        // occur if the allocator is not decommited when a pin is disconnected.
        HRESULT hr = m_pAllocator->Decommit();
        if( FAILED( hr ) ) {
            return hr;
        }

        m_pAllocator->Release();
        m_pAllocator = NULL;
    }

    return S_OK;
}


/* Do something with this media sample - this base class checks to see if the
   format has changed with this media sample and if so checks that the filter
   will accept it, generating a run time error if not. Once we have raised a
   run time error we set a flag so that no more samples will be accepted

   It is important that any filter should override this method and implement
   synchronization so that samples are not processed when the pin is
   disconnected etc
*/

STDMETHODIMP
CBaseInputPin::Receive(IMediaSample *pSample)
{
    CheckPointer(pSample,E_POINTER);
    ValidateReadPtr(pSample,sizeof(IMediaSample));
    ASSERT(pSample);

    HRESULT hr = CheckStreaming();
    if (S_OK != hr) {
        return hr;
    }

#ifdef DXMPERF
    PERFLOG_RECEIVE( m_pName ? m_pName : L"CBaseInputPin", (IPin *) m_Connected, (IPin *) this, pSample, &m_mt );
#endif // DXMPERF


    /* Check for IMediaSample2 */
    IMediaSample2 *pSample2;
    if (SUCCEEDED(pSample->QueryInterface(IID_IMediaSample2, (void **)&pSample2))) {
        hr = pSample2->GetProperties(sizeof(m_SampleProps), (PBYTE)&m_SampleProps);
        pSample2->Release();
        if (FAILED(hr)) {
            return hr;
        }
    } else {
        /*  Get the properties the hard way */
        m_SampleProps.cbData = sizeof(m_SampleProps);
        m_SampleProps.dwTypeSpecificFlags = 0;
        m_SampleProps.dwStreamId = AM_STREAM_MEDIA;
        m_SampleProps.dwSampleFlags = 0;
        if (S_OK == pSample->IsDiscontinuity()) {
            m_SampleProps.dwSampleFlags |= AM_SAMPLE_DATADISCONTINUITY;
        }
        if (S_OK == pSample->IsPreroll()) {
            m_SampleProps.dwSampleFlags |= AM_SAMPLE_PREROLL;
        }
        if (S_OK == pSample->IsSyncPoint()) {
            m_SampleProps.dwSampleFlags |= AM_SAMPLE_SPLICEPOINT;
        }
        if (SUCCEEDED(pSample->GetTime(&m_SampleProps.tStart,
                                       &m_SampleProps.tStop))) {
            m_SampleProps.dwSampleFlags |= AM_SAMPLE_TIMEVALID |
                                           AM_SAMPLE_STOPVALID;
        }
        if (S_OK == pSample->GetMediaType(&m_SampleProps.pMediaType)) {
            m_SampleProps.dwSampleFlags |= AM_SAMPLE_TYPECHANGED;
        }
        pSample->GetPointer(&m_SampleProps.pbBuffer);
        m_SampleProps.lActual = pSample->GetActualDataLength();
        m_SampleProps.cbBuffer = pSample->GetSize();
    }

    /* Has the format changed in this sample */

    if (!(m_SampleProps.dwSampleFlags & AM_SAMPLE_TYPECHANGED)) {
        return NOERROR;
    }

    /* Check the derived class accepts this format */
    /* This shouldn't fail as the source must call QueryAccept first */

    hr = CheckMediaType((CMediaType *)m_SampleProps.pMediaType);

    if (hr == NOERROR) {
        return NOERROR;
    }

    /* Raise a runtime error if we fail the media type */

    m_bRunTimeError = TRUE;
    EndOfStream();
    m_pFilter->NotifyEvent(EC_ERRORABORT,VFW_E_TYPE_NOT_ACCEPTED,0);
    return VFW_E_INVALIDMEDIATYPE;
}


/*  Receive multiple samples */
STDMETHODIMP
CBaseInputPin::ReceiveMultiple (
    __in_ecount(nSamples) IMediaSample **pSamples,
    long nSamples,
    __out long *nSamplesProcessed)
{
    CheckPointer(pSamples,E_POINTER);
    ValidateReadPtr(pSamples,nSamples * sizeof(IMediaSample *));

    HRESULT hr = S_OK;
    *nSamplesProcessed = 0;
    while (nSamples-- > 0) {
         hr = Receive(pSamples[*nSamplesProcessed]);

         /*  S_FALSE means don't send any more */
         if (hr != S_OK) {
             break;
         }
         (*nSamplesProcessed)++;
    }
    return hr;
}

/*  See if Receive() might block */
STDMETHODIMP
CBaseInputPin::ReceiveCanBlock()
{
    /*  Ask all the output pins if they block
        If there are no output pin assume we do block
    */
    int cPins = m_pFilter->GetPinCount();
    int cOutputPins = 0;
    for (int c = 0; c < cPins; c++) {
        CBasePin *pPin = m_pFilter->GetPin(c);
        if (NULL == pPin) {
            break;
        }
        PIN_DIRECTION pd;
        HRESULT hr = pPin->QueryDirection(&pd);
        if (FAILED(hr)) {
            return hr;
        }

        if (pd == PINDIR_OUTPUT) {

            IPin *pConnected;
            hr = pPin->ConnectedTo(&pConnected);
            if (SUCCEEDED(hr)) {
                ASSERT(pConnected != NULL);
                cOutputPins++;
                IMemInputPin *pInputPin;
                hr = pConnected->QueryInterface(
                                              IID_IMemInputPin,
                                              (void **)&pInputPin);
                pConnected->Release();
                if (SUCCEEDED(hr)) {
                    hr = pInputPin->ReceiveCanBlock();
                    pInputPin->Release();
                    if (hr != S_FALSE) {
                        return S_OK;
                    }
                } else {
                    /*  There's a transport we don't understand here */
                    return S_OK;
                }
            }
        }
    }
    return cOutputPins == 0 ? S_OK : S_FALSE;
}

// Default handling for BeginFlush - call at the beginning
// of your implementation (makes sure that all Receive calls
// fail). After calling this, you need to free any queued data
// and then call downstream.
STDMETHODIMP
CBaseInputPin::BeginFlush(void)
{
    //  BeginFlush is NOT synchronized with streaming but is part of
    //  a control action - hence we synchronize with the filter
    CAutoLock lck(m_pLock);

    // if we are already in mid-flush, this is probably a mistake
    // though not harmful - try to pick it up for now so I can think about it
    ASSERT(!m_bFlushing);

    // first thing to do is ensure that no further Receive calls succeed
    m_bFlushing = TRUE;

    // now discard any data and call downstream - must do that
    // in derived classes
    return S_OK;
}

// default handling for EndFlush - call at end of your implementation
// - before calling this, ensure that there is no queued data and no thread
// pushing any more without a further receive, then call downstream,
// then call this method to clear the m_bFlushing flag and re-enable
// receives
STDMETHODIMP
CBaseInputPin::EndFlush(void)
{
    //  Endlush is NOT synchronized with streaming but is part of
    //  a control action - hence we synchronize with the filter
    CAutoLock lck(m_pLock);

    // almost certainly a mistake if we are not in mid-flush
    ASSERT(m_bFlushing);

    // before calling, sync with pushing thread and ensure
    // no more data is going downstream, then call EndFlush on
    // downstream pins.

    // now re-enable Receives
    m_bFlushing = FALSE;

    // No more errors
    m_bRunTimeError = FALSE;

    return S_OK;
}


STDMETHODIMP
CBaseInputPin::Notify(IBaseFilter * pSender, Quality q)
{
    UNREFERENCED_PARAMETER(q);
    CheckPointer(pSender,E_POINTER);
    ValidateReadPtr(pSender,sizeof(IBaseFilter));
    DbgBreak("IQuality::Notify called on an input pin");
    return NOERROR;
} // Notify

/* Free up or unprepare allocator's memory, this is called through
   IMediaFilter which is responsible for locking the object first */

HRESULT
CBaseInputPin::Inactive(void)
{
    m_bRunTimeError = FALSE;
    if (m_pAllocator == NULL) {
        return VFW_E_NO_ALLOCATOR;
    }

    m_bFlushing = FALSE;

    return m_pAllocator->Decommit();
}

// what requirements do we have of the allocator - override if you want
// to support other people's allocators but need a specific alignment
// or prefix.
STDMETHODIMP
CBaseInputPin::GetAllocatorRequirements(__out ALLOCATOR_PROPERTIES*pProps)
{
    UNREFERENCED_PARAMETER(pProps);
    return E_NOTIMPL;
}

//  Check if it's OK to process data
//
HRESULT
CBaseInputPin::CheckStreaming()
{
    //  Shouldn't be able to get any data if we're not connected!
    ASSERT(IsConnected());

    //  Don't process stuff in Stopped state
    if (IsStopped()) {
        return VFW_E_WRONG_STATE;
    }
    if (m_bFlushing) {
        return S_FALSE;
    }
    if (m_bRunTimeError) {
        return VFW_E_RUNTIME_ERROR;
    }
    return S_OK;
}

// Pass on the Quality notification q to
// a. Our QualityControl sink (if we have one) or else
// b. to our upstream filter
// and if that doesn't work, throw it away with a bad return code
HRESULT
CBaseInputPin::PassNotify(Quality& q)
{
    // We pass the message on, which means that we find the quality sink
    // for our input pin and send it there

    DbgLog((LOG_TRACE,3,TEXT("Passing Quality notification through transform")));
    if (m_pQSink!=NULL) {
        return m_pQSink->Notify(m_pFilter, q);
    } else {
        // no sink set, so pass it upstream
        HRESULT hr;
        IQualityControl * pIQC;

        hr = VFW_E_NOT_FOUND;                   // default
        if (m_Connected) {
            m_Connected->QueryInterface(IID_IQualityControl, (void**)&pIQC);

            if (pIQC!=NULL) {
                hr = pIQC->Notify(m_pFilter, q);
                pIQC->Release();
            }
        }
        return hr;
    }

} // PassNotify

//=====================================================================
//=====================================================================
// Memory allocation class, implements CMediaSample
//=====================================================================
//=====================================================================


/* NOTE The implementation of this class calls the CUnknown constructor with
   a NULL outer unknown pointer. This has the effect of making us a self
   contained class, ie any QueryInterface, AddRef or Release calls will be
   routed to the class's NonDelegatingUnknown methods. You will typically
   find that the classes that do this then override one or more of these
   virtual functions to provide more specialised behaviour. A good example
   of this is where a class wants to keep the QueryInterface internal but
   still wants it's lifetime controlled by the external object */

/* The last two parameters have default values of NULL and zero */

CMediaSample::CMediaSample(__in_opt LPCTSTR pName,
               __in_opt CBaseAllocator *pAllocator,
               __inout_opt HRESULT *phr,
               __in_bcount_opt(length) LPBYTE pBuffer,
               LONG length) :
    m_pBuffer(pBuffer),             // Initialise the buffer
    m_cbBuffer(length),             // And it's length
    m_lActual(length),              // By default, actual = length
    m_pMediaType(NULL),             // No media type change
    m_dwFlags(0),                   // Nothing set
    m_cRef(0),                      // 0 ref count
    m_dwTypeSpecificFlags(0),       // Type specific flags
    m_dwStreamId(AM_STREAM_MEDIA),  // Stream id
    m_pAllocator(pAllocator)        // Allocator
{
#ifdef DXMPERF
    PERFLOG_CTOR( pName ? pName : L"CMediaSample", (IMediaSample *) this );
#endif // DXMPERF

    /* We must have an owner and it must also be derived from class
       CBaseAllocator BUT we do not hold a reference count on it */

    ASSERT(pAllocator);

    if (length < 0) {
        *phr = VFW_E_BUFFER_OVERFLOW;
        m_cbBuffer = 0;
    }
}

#ifdef UNICODE
CMediaSample::CMediaSample(__in_opt LPCSTR pName,
               __in_opt CBaseAllocator *pAllocator,
               __inout_opt HRESULT *phr,
               __in_bcount_opt(length) LPBYTE pBuffer,
               LONG length) :
    m_pBuffer(pBuffer),             // Initialise the buffer
    m_cbBuffer(length),             // And it's length
    m_lActual(length),              // By default, actual = length
    m_pMediaType(NULL),             // No media type change
    m_dwFlags(0),                   // Nothing set
    m_cRef(0),                      // 0 ref count
    m_dwTypeSpecificFlags(0),       // Type specific flags
    m_dwStreamId(AM_STREAM_MEDIA),  // Stream id
    m_pAllocator(pAllocator)        // Allocator
{
#ifdef DXMPERF
    PERFLOG_CTOR( L"CMediaSample", (IMediaSample *) this );
#endif // DXMPERF

    /* We must have an owner and it must also be derived from class
       CBaseAllocator BUT we do not hold a reference count on it */

    ASSERT(pAllocator);
}
#endif

/* Destructor deletes the media type memory */

CMediaSample::~CMediaSample()
{
#ifdef DXMPERF
    PERFLOG_DTOR( L"CMediaSample", (IMediaSample *) this );
#endif // DXMPERF

    if (m_pMediaType) {
    DeleteMediaType(m_pMediaType);
    }
}

/* Override this to publicise our interfaces */

STDMETHODIMP
CMediaSample::QueryInterface(REFIID riid, __deref_out void **ppv)
{
    if (riid == IID_IMediaSample ||
        riid == IID_IMediaSample2 ||
        riid == IID_IUnknown) {
        return GetInterface((IMediaSample *) this, ppv);
    } else {
        *ppv = NULL;
        return E_NOINTERFACE;
    }
}

STDMETHODIMP_(ULONG)
CMediaSample::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}


// --  CMediaSample lifetimes --
//
// On final release of this sample buffer it is not deleted but
// returned to the freelist of the owning memory allocator
//
// The allocator may be waiting for the last buffer to be placed on the free
// list in order to decommit all the memory, so the ReleaseBuffer() call may
// result in this sample being deleted. We also need to hold a refcount on
// the allocator to stop that going away until we have finished with this.
// However, we cannot release the allocator before the ReleaseBuffer, as the
// release may cause us to be deleted. Similarly we can't do it afterwards.
//
// Thus we must leave it to the allocator to hold an addref on our behalf.
// When he issues us in GetBuffer, he addref's himself. When ReleaseBuffer
// is called, he releases himself, possibly causing us and him to be deleted.


STDMETHODIMP_(ULONG)
CMediaSample::Release()
{
    /* Decrement our own private reference count */
    LONG lRef;
    if (m_cRef == 1) {
        lRef = 0;
        m_cRef = 0;
    } else {
        lRef = InterlockedDecrement(&m_cRef);
    }
    ASSERT(lRef >= 0);

    DbgLog((LOG_MEMORY,3,TEXT("    Unknown %X ref-- = %d"),
        this, m_cRef));

    /* Did we release our final reference count */
    if (lRef == 0) {
        /* Free all resources */
        if (m_dwFlags & Sample_TypeChanged) {
            SetMediaType(NULL);
        }
        ASSERT(m_pMediaType == NULL);
        m_dwFlags = 0;
        m_dwTypeSpecificFlags = 0;
        m_dwStreamId = AM_STREAM_MEDIA;

        /* This may cause us to be deleted */
        // Our refcount is reliably 0 thus no-one will mess with us
        m_pAllocator->ReleaseBuffer(this);
    }
    return (ULONG)lRef;
}


// set the buffer pointer and length. Used by allocators that
// want variable sized pointers or pointers into already-read data.
// This is only available through a CMediaSample* not an IMediaSample*
// and so cannot be changed by clients.
HRESULT
CMediaSample::SetPointer(__in_bcount(cBytes) BYTE * ptr, LONG cBytes)
{
    if (cBytes < 0) {
        return VFW_E_BUFFER_OVERFLOW;
    }
    m_pBuffer = ptr;            // new buffer area (could be null)
    m_cbBuffer = cBytes;        // length of buffer
    m_lActual = cBytes;         // length of data in buffer (assume full)

    return S_OK;
}


// get me a read/write pointer to this buffer's memory. I will actually
// want to use sizeUsed bytes.
STDMETHODIMP
CMediaSample::GetPointer(__deref_out BYTE ** ppBuffer)
{
    ValidateReadWritePtr(ppBuffer,sizeof(BYTE *));

    // creator must have set pointer either during
    // constructor or by SetPointer
    ASSERT(m_pBuffer);

    *ppBuffer = m_pBuffer;
    return NOERROR;
}


// return the size in bytes of this buffer
STDMETHODIMP_(LONG)
CMediaSample::GetSize(void)
{
    return m_cbBuffer;
}


// get the stream time at which this sample should start and finish.
STDMETHODIMP
CMediaSample::GetTime(
    __out REFERENCE_TIME * pTimeStart,     // put time here
    __out REFERENCE_TIME * pTimeEnd
)
{
    ValidateReadWritePtr(pTimeStart,sizeof(REFERENCE_TIME));
    ValidateReadWritePtr(pTimeEnd,sizeof(REFERENCE_TIME));

    if (!(m_dwFlags & Sample_StopValid)) {
        if (!(m_dwFlags & Sample_TimeValid)) {
            return VFW_E_SAMPLE_TIME_NOT_SET;
        } else {
            *pTimeStart = m_Start;

            //  Make sure old stuff works
            *pTimeEnd = m_Start + 1;
            return VFW_S_NO_STOP_TIME;
        }
    }

    *pTimeStart = m_Start;
    *pTimeEnd = m_End;
    return NOERROR;
}


// Set the stream time at which this sample should start and finish.
// NULL pointers means the time is reset
STDMETHODIMP
CMediaSample::SetTime(
    __in_opt REFERENCE_TIME * pTimeStart,
    __in_opt REFERENCE_TIME * pTimeEnd
)
{
    if (pTimeStart == NULL) {
        ASSERT(pTimeEnd == NULL);
        m_dwFlags &= ~(Sample_TimeValid | Sample_StopValid);
    } else {
        if (pTimeEnd == NULL) {
            m_Start = *pTimeStart;
            m_dwFlags |= Sample_TimeValid;
            m_dwFlags &= ~Sample_StopValid;
        } else {
            ValidateReadPtr(pTimeStart,sizeof(REFERENCE_TIME));
            ValidateReadPtr(pTimeEnd,sizeof(REFERENCE_TIME));
            ASSERT(*pTimeEnd >= *pTimeStart);

            m_Start = *pTimeStart;
            m_End = *pTimeEnd;
            m_dwFlags |= Sample_TimeValid | Sample_StopValid;
        }
    }
    return NOERROR;
}


// get the media times (eg bytes) for this sample
STDMETHODIMP
CMediaSample::GetMediaTime(
    __out LONGLONG * pTimeStart,
    __out LONGLONG * pTimeEnd
)
{
    ValidateReadWritePtr(pTimeStart,sizeof(LONGLONG));
    ValidateReadWritePtr(pTimeEnd,sizeof(LONGLONG));

    if (!(m_dwFlags & Sample_MediaTimeValid)) {
        return VFW_E_MEDIA_TIME_NOT_SET;
    }

    *pTimeStart = m_MediaStart;
    *pTimeEnd = (m_MediaStart + m_MediaEnd);
    return NOERROR;
}


// Set the media times for this sample
STDMETHODIMP
CMediaSample::SetMediaTime(
    __in_opt LONGLONG * pTimeStart,
    __in_opt LONGLONG * pTimeEnd
)
{
    if (pTimeStart == NULL) {
        ASSERT(pTimeEnd == NULL);
        m_dwFlags &= ~Sample_MediaTimeValid;
    } else {
        if (NULL == pTimeEnd) {
            return E_POINTER;
        }
        ValidateReadPtr(pTimeStart,sizeof(LONGLONG));
        ValidateReadPtr(pTimeEnd,sizeof(LONGLONG));
        ASSERT(*pTimeEnd >= *pTimeStart);

        m_MediaStart = *pTimeStart;
        m_MediaEnd = (LONG)(*pTimeEnd - *pTimeStart);
        m_dwFlags |= Sample_MediaTimeValid;
    }
    return NOERROR;
}


STDMETHODIMP
CMediaSample::IsSyncPoint(void)
{
    if (m_dwFlags & Sample_SyncPoint) {
        return S_OK;
    } else {
        return S_FALSE;
    }
}


STDMETHODIMP
CMediaSample::SetSyncPoint(BOOL bIsSyncPoint)
{
    if (bIsSyncPoint) {
        m_dwFlags |= Sample_SyncPoint;
    } else {
        m_dwFlags &= ~Sample_SyncPoint;
    }
    return NOERROR;
}

// returns S_OK if there is a discontinuity in the data (this same is
// not a continuation of the previous stream of data
// - there has been a seek).
STDMETHODIMP
CMediaSample::IsDiscontinuity(void)
{
    if (m_dwFlags & Sample_Discontinuity) {
        return S_OK;
    } else {
        return S_FALSE;
    }
}

// set the discontinuity property - TRUE if this sample is not a
// continuation, but a new sample after a seek.
STDMETHODIMP
CMediaSample::SetDiscontinuity(BOOL bDiscont)
{
    // should be TRUE or FALSE
    if (bDiscont) {
        m_dwFlags |= Sample_Discontinuity;
    } else {
        m_dwFlags &= ~Sample_Discontinuity;
    }
    return S_OK;
}

STDMETHODIMP
CMediaSample::IsPreroll(void)
{
    if (m_dwFlags & Sample_Preroll) {
        return S_OK;
    } else {
        return S_FALSE;
    }
}


STDMETHODIMP
CMediaSample::SetPreroll(BOOL bIsPreroll)
{
    if (bIsPreroll) {
        m_dwFlags |= Sample_Preroll;
    } else {
        m_dwFlags &= ~Sample_Preroll;
    }
    return NOERROR;
}

STDMETHODIMP_(LONG)
CMediaSample::GetActualDataLength(void)
{
    return m_lActual;
}


STDMETHODIMP
CMediaSample::SetActualDataLength(LONG lActual)
{
    if (lActual > m_cbBuffer || lActual < 0) {
        ASSERT(lActual <= GetSize());
        return VFW_E_BUFFER_OVERFLOW;
    }
    m_lActual = lActual;
    return NOERROR;
}


/* These allow for limited format changes in band */

STDMETHODIMP
CMediaSample::GetMediaType(__deref_out AM_MEDIA_TYPE **ppMediaType)
{
    ValidateReadWritePtr(ppMediaType,sizeof(AM_MEDIA_TYPE *));
    ASSERT(ppMediaType);

    /* Do we have a new media type for them */

    if (!(m_dwFlags & Sample_TypeChanged)) {
        ASSERT(m_pMediaType == NULL);
        *ppMediaType = NULL;
        return S_FALSE;
    }

    ASSERT(m_pMediaType);

    /* Create a copy of our media type */

    *ppMediaType = CreateMediaType(m_pMediaType);
    if (*ppMediaType == NULL) {
        return E_OUTOFMEMORY;
    }
    return NOERROR;
}


/* Mark this sample as having a different format type */

STDMETHODIMP
CMediaSample::SetMediaType(__in_opt AM_MEDIA_TYPE *pMediaType)
{
    /* Delete the current media type */

    if (m_pMediaType) {
        DeleteMediaType(m_pMediaType);
        m_pMediaType = NULL;
    }

    /* Mechanism for resetting the format type */

    if (pMediaType == NULL) {
        m_dwFlags &= ~Sample_TypeChanged;
        return NOERROR;
    }

    ASSERT(pMediaType);
    ValidateReadPtr(pMediaType,sizeof(AM_MEDIA_TYPE));

    /* Take a copy of the media type */

    m_pMediaType = CreateMediaType(pMediaType);
    if (m_pMediaType == NULL) {
        m_dwFlags &= ~Sample_TypeChanged;
        return E_OUTOFMEMORY;
    }

    m_dwFlags |= Sample_TypeChanged;
    return NOERROR;
}

// Set and get properties (IMediaSample2)
STDMETHODIMP CMediaSample::GetProperties(
    DWORD cbProperties,
    __out_bcount(cbProperties) BYTE * pbProperties
)
{
    if (0 != cbProperties) {
        CheckPointer(pbProperties, E_POINTER);
        //  Return generic stuff up to the length
        AM_SAMPLE2_PROPERTIES Props;
        Props.cbData     = min(cbProperties, sizeof(Props));
        Props.dwSampleFlags = m_dwFlags & ~Sample_MediaTimeValid;
        Props.dwTypeSpecificFlags = m_dwTypeSpecificFlags;
        Props.pbBuffer   = m_pBuffer;
        Props.cbBuffer   = m_cbBuffer;
        Props.lActual    = m_lActual;
        Props.tStart     = m_Start;
        Props.tStop      = m_End;
        Props.dwStreamId = m_dwStreamId;
        if (m_dwFlags & AM_SAMPLE_TYPECHANGED) {
            Props.pMediaType = m_pMediaType;
        } else {
            Props.pMediaType = NULL;
        }
        CopyMemory(pbProperties, &Props, Props.cbData);
    }
    return S_OK;
}

#define CONTAINS_FIELD(type, field, offset) \
    ((FIELD_OFFSET(type, field) + sizeof(((type *)0)->field)) <= offset)

HRESULT CMediaSample::SetProperties(
    DWORD cbProperties,
    __in_bcount(cbProperties) const BYTE * pbProperties
)
{

    /*  Generic properties */
    AM_MEDIA_TYPE *pMediaType = NULL;

    if (CONTAINS_FIELD(AM_SAMPLE2_PROPERTIES, cbData, cbProperties)) {
        CheckPointer(pbProperties, E_POINTER);
        AM_SAMPLE2_PROPERTIES *pProps =
            (AM_SAMPLE2_PROPERTIES *)pbProperties;

        /*  Don't use more data than is actually there */
        if (pProps->cbData < cbProperties) {
            cbProperties = pProps->cbData;
        }
        /*  We only handle IMediaSample2 */
        if (cbProperties > sizeof(*pProps) ||
            pProps->cbData > sizeof(*pProps)) {
            return E_INVALIDARG;
        }
        /*  Do checks first, the assignments (for backout) */
        if (CONTAINS_FIELD(AM_SAMPLE2_PROPERTIES, dwSampleFlags, cbProperties)) {
            /*  Check the flags */
            if (pProps->dwSampleFlags &
                    (~Sample_ValidFlags | Sample_MediaTimeValid)) {
                return E_INVALIDARG;
            }
            /*  Check a flag isn't being set for a property
                not being provided
            */
            if ((pProps->dwSampleFlags & AM_SAMPLE_TIMEVALID) &&
                 !(m_dwFlags & AM_SAMPLE_TIMEVALID) &&
                 !CONTAINS_FIELD(AM_SAMPLE2_PROPERTIES, tStop, cbProperties)) {
                 return E_INVALIDARG;
            }
        }
        /*  NB - can't SET the pointer or size */
        if (CONTAINS_FIELD(AM_SAMPLE2_PROPERTIES, pbBuffer, cbProperties)) {

            /*  Check pbBuffer */
            if (pProps->pbBuffer != 0 && pProps->pbBuffer != m_pBuffer) {
                return E_INVALIDARG;
            }
        }
        if (CONTAINS_FIELD(AM_SAMPLE2_PROPERTIES, cbBuffer, cbProperties)) {

            /*  Check cbBuffer */
            if (pProps->cbBuffer != 0 && pProps->cbBuffer != m_cbBuffer) {
                return E_INVALIDARG;
            }
        }
        if (CONTAINS_FIELD(AM_SAMPLE2_PROPERTIES, cbBuffer, cbProperties) &&
            CONTAINS_FIELD(AM_SAMPLE2_PROPERTIES, lActual, cbProperties)) {

            /*  Check lActual */
            if (pProps->cbBuffer < pProps->lActual) {
                return E_INVALIDARG;
            }
        }

        if (CONTAINS_FIELD(AM_SAMPLE2_PROPERTIES, pMediaType, cbProperties)) {

            /*  Check pMediaType */
            if (pProps->dwSampleFlags & AM_SAMPLE_TYPECHANGED) {
                CheckPointer(pProps->pMediaType, E_POINTER);
                pMediaType = CreateMediaType(pProps->pMediaType);
                if (pMediaType == NULL) {
                    return E_OUTOFMEMORY;
                }
            }
        }

        /*  Now do the assignments */
        if (CONTAINS_FIELD(AM_SAMPLE2_PROPERTIES, dwStreamId, cbProperties)) {
            m_dwStreamId = pProps->dwStreamId;
        }
        if (CONTAINS_FIELD(AM_SAMPLE2_PROPERTIES, dwSampleFlags, cbProperties)) {
            /*  Set the flags */
            m_dwFlags = pProps->dwSampleFlags |
                                (m_dwFlags & Sample_MediaTimeValid);
            m_dwTypeSpecificFlags = pProps->dwTypeSpecificFlags;
        } else {
            if (CONTAINS_FIELD(AM_SAMPLE2_PROPERTIES, dwTypeSpecificFlags, cbProperties)) {
                m_dwTypeSpecificFlags = pProps->dwTypeSpecificFlags;
            }
        }

        if (CONTAINS_FIELD(AM_SAMPLE2_PROPERTIES, lActual, cbProperties)) {
            /*  Set lActual */
            m_lActual = pProps->lActual;
        }

        if (CONTAINS_FIELD(AM_SAMPLE2_PROPERTIES, tStop, cbProperties)) {

            /*  Set the times */
            m_End   = pProps->tStop;
        }
        if (CONTAINS_FIELD(AM_SAMPLE2_PROPERTIES, tStart, cbProperties)) {

            /*  Set the times */
            m_Start = pProps->tStart;
        }

        if (CONTAINS_FIELD(AM_SAMPLE2_PROPERTIES, pMediaType, cbProperties)) {
            /*  Set pMediaType */
            if (pProps->dwSampleFlags & AM_SAMPLE_TYPECHANGED) {
                if (m_pMediaType != NULL) {
                    DeleteMediaType(m_pMediaType);
                }
                m_pMediaType = pMediaType;
            }
        }

        /*  Fix up the type changed flag to correctly reflect the current state
            If, for instance the input contained no type change but the
            output does then if we don't do this we'd lose the
            output media type.
        */
        if (m_pMediaType) {
            m_dwFlags |= Sample_TypeChanged;
        } else {
            m_dwFlags &= ~Sample_TypeChanged;
        }
    }

    return S_OK;
}


//
// The streaming thread calls IPin::NewSegment(), IPin::EndOfStream(),
// IMemInputPin::Receive() and IMemInputPin::ReceiveMultiple() on the
// connected input pin.  The application thread calls Block().  The
// following class members can only be called by the streaming thread.
//
//    Deliver()
//    DeliverNewSegment()
//    StartUsingOutputPin()
//    StopUsingOutputPin()
//    ChangeOutputFormat()
//    ChangeMediaType()
//    DynamicReconnect()
//
// The following class members can only be called by the application thread.
//
//    Block()
//    SynchronousBlockOutputPin()
//    AsynchronousBlockOutputPin()
//

CDynamicOutputPin::CDynamicOutputPin(
    __in_opt LPCTSTR pObjectName,
    __in CBaseFilter *pFilter,
    __in CCritSec *pLock,
    __inout HRESULT *phr,
    __in_opt LPCWSTR pName) :
        CBaseOutputPin(pObjectName, pFilter, pLock, phr, pName),
        m_hStopEvent(NULL),
        m_pGraphConfig(NULL),
        m_bPinUsesReadOnlyAllocator(FALSE),
        m_BlockState(NOT_BLOCKED),
        m_hUnblockOutputPinEvent(NULL),
        m_hNotifyCallerPinBlockedEvent(NULL),
        m_dwBlockCallerThreadID(0),
        m_dwNumOutstandingOutputPinUsers(0)
{
    HRESULT hr = Initialize();
    if( FAILED( hr ) ) {
        *phr = hr;
        return;
    }
}

#ifdef UNICODE
CDynamicOutputPin::CDynamicOutputPin(
    __in_opt LPCSTR pObjectName,
    __in CBaseFilter *pFilter,
    __in CCritSec *pLock,
    __inout HRESULT *phr,
    __in_opt LPCWSTR pName) :
        CBaseOutputPin(pObjectName, pFilter, pLock, phr, pName),
        m_hStopEvent(NULL),
        m_pGraphConfig(NULL),
        m_bPinUsesReadOnlyAllocator(FALSE),
        m_BlockState(NOT_BLOCKED),
        m_hUnblockOutputPinEvent(NULL),
        m_hNotifyCallerPinBlockedEvent(NULL),
        m_dwBlockCallerThreadID(0),
        m_dwNumOutstandingOutputPinUsers(0)
{
    HRESULT hr = Initialize();
    if( FAILED( hr ) ) {
        *phr = hr;
        return;
    }
}
#endif

CDynamicOutputPin::~CDynamicOutputPin()
{
    if(NULL != m_hUnblockOutputPinEvent) {
        // This call should not fail because we have access to m_hUnblockOutputPinEvent
        // and m_hUnblockOutputPinEvent is a valid event.
        EXECUTE_ASSERT(::CloseHandle(m_hUnblockOutputPinEvent));
    }

    if(NULL != m_hNotifyCallerPinBlockedEvent) {
        // This call should not fail because we have access to m_hNotifyCallerPinBlockedEvent
        // and m_hNotifyCallerPinBlockedEvent is a valid event.
        EXECUTE_ASSERT(::CloseHandle(m_hNotifyCallerPinBlockedEvent));
    }
}

HRESULT CDynamicOutputPin::Initialize(void)
{
    m_hUnblockOutputPinEvent = ::CreateEvent( NULL,   // The event will have the default security descriptor.
                                              TRUE,   // This is a manual reset event.
                                              TRUE,   // The event is initially signaled.
                                              NULL ); // The event is not named.

    // CreateEvent() returns NULL if an error occurs.
    if(NULL == m_hUnblockOutputPinEvent) {
        return AmGetLastErrorToHResult();
    }

    //  Set flag to say we can reconnect while streaming.
    SetReconnectWhenActive(true);

    return S_OK;
}

STDMETHODIMP CDynamicOutputPin::NonDelegatingQueryInterface(REFIID riid, __deref_out void **ppv)
{
    if(riid == IID_IPinFlowControl) {
        return GetInterface(static_cast<IPinFlowControl*>(this), ppv);
    } else {
        return CBaseOutputPin::NonDelegatingQueryInterface(riid, ppv);
    }
}

STDMETHODIMP CDynamicOutputPin::Disconnect(void)
{
    CAutoLock cObjectLock(m_pLock);
    return DisconnectInternal();
}

STDMETHODIMP CDynamicOutputPin::Block(DWORD dwBlockFlags, HANDLE hEvent)
{
    const DWORD VALID_FLAGS = AM_PIN_FLOW_CONTROL_BLOCK;

    // Check for illegal flags.
    if(dwBlockFlags & ~VALID_FLAGS) {
        return E_INVALIDARG;
    }

    // Make sure the event is unsignaled.
    if((dwBlockFlags & AM_PIN_FLOW_CONTROL_BLOCK) && (NULL != hEvent)) {
        if( !::ResetEvent( hEvent ) ) {
            return AmGetLastErrorToHResult();
        }
    }

    // No flags are set if we are unblocking the output pin.
    if(0 == dwBlockFlags) {

        // This parameter should be NULL because unblock operations are always synchronous.
        // There is no need to notify the caller when the event is done.
        if(NULL != hEvent) {
            return E_INVALIDARG;
        }
    }

    #ifdef DEBUG
    AssertValid();
    #endif // DEBUG

    HRESULT hr;

    if(dwBlockFlags & AM_PIN_FLOW_CONTROL_BLOCK) {
        // IPinFlowControl::Block()'s hEvent parameter is NULL if the block is synchronous.
        // If hEvent is not NULL, the block is asynchronous.
        if(NULL == hEvent) {
            hr = SynchronousBlockOutputPin();
        } else {
            hr = AsynchronousBlockOutputPin(hEvent);
        }
    } else {
        hr = UnblockOutputPin();
    }

    #ifdef DEBUG
    AssertValid();
    #endif // DEBUG

    if(FAILED(hr)) {
        return hr;
    }

    return S_OK;
}

HRESULT CDynamicOutputPin::SynchronousBlockOutputPin(void)
{
    HANDLE hNotifyCallerPinBlockedEvent = :: CreateEvent( NULL,   // The event will have the default security attributes.
                                                          FALSE,  // This is an automatic reset event.
                                                          FALSE,  // The event is initially unsignaled.
                                                          NULL ); // The event is not named.

    // CreateEvent() returns NULL if an error occurs.
    if(NULL == hNotifyCallerPinBlockedEvent) {
        return AmGetLastErrorToHResult();
    }

    HRESULT hr = AsynchronousBlockOutputPin(hNotifyCallerPinBlockedEvent);
    if(FAILED(hr)) {
        // This call should not fail because we have access to hNotifyCallerPinBlockedEvent
        // and hNotifyCallerPinBlockedEvent is a valid event.
        EXECUTE_ASSERT(::CloseHandle(hNotifyCallerPinBlockedEvent));

        return hr;
    }

    hr = WaitEvent(hNotifyCallerPinBlockedEvent);

    // This call should not fail because we have access to hNotifyCallerPinBlockedEvent
    // and hNotifyCallerPinBlockedEvent is a valid event.
    EXECUTE_ASSERT(::CloseHandle(hNotifyCallerPinBlockedEvent));

    if(FAILED(hr)) {
        return hr;
    }

    return S_OK;
}

HRESULT CDynamicOutputPin::AsynchronousBlockOutputPin(HANDLE hNotifyCallerPinBlockedEvent)
{
    // This function holds the m_BlockStateLock because it uses
    // m_dwBlockCallerThreadID, m_BlockState and
    // m_hNotifyCallerPinBlockedEvent.
    CAutoLock alBlockStateLock(&m_BlockStateLock);

    if(NOT_BLOCKED != m_BlockState) {
        if(m_dwBlockCallerThreadID == ::GetCurrentThreadId()) {
            return VFW_E_PIN_ALREADY_BLOCKED_ON_THIS_THREAD;
        } else {
            return VFW_E_PIN_ALREADY_BLOCKED;
        }
    }

    BOOL fSuccess = ::DuplicateHandle( ::GetCurrentProcess(),
                                       hNotifyCallerPinBlockedEvent,
                                       ::GetCurrentProcess(),
                                       &m_hNotifyCallerPinBlockedEvent,
                                       EVENT_MODIFY_STATE,
                                       FALSE,
                                       0 );
    if( !fSuccess ) {
        return AmGetLastErrorToHResult();
    }

    m_BlockState = PENDING;
    m_dwBlockCallerThreadID = ::GetCurrentThreadId();

    // The output pin cannot be blocked if the streaming thread is
    // calling IPin::NewSegment(), IPin::EndOfStream(), IMemInputPin::Receive()
    // or IMemInputPin::ReceiveMultiple() on the connected input pin.  Also, it
    // cannot be blocked if the streaming thread is calling DynamicReconnect(),
    // ChangeMediaType() or ChangeOutputFormat().
    if(!StreamingThreadUsingOutputPin()) {

        // The output pin can be immediately blocked.
        BlockOutputPin();
    }

    return S_OK;
}

void CDynamicOutputPin::BlockOutputPin(void)
{
    // The caller should always hold the m_BlockStateLock because this function
    // uses m_BlockState and m_hNotifyCallerPinBlockedEvent.
    ASSERT(CritCheckIn(&m_BlockStateLock));

    // This function should not be called if the streaming thread is modifying
    // the connection state or it's passing data downstream.
    ASSERT(!StreamingThreadUsingOutputPin());

    // This should not fail because we successfully created the event
    // and we have the security permissions to change it's state.
    EXECUTE_ASSERT(::ResetEvent(m_hUnblockOutputPinEvent));

    // This event should not fail because AsynchronousBlockOutputPin() successfully
    // duplicated this handle and we have the appropriate security permissions.
    EXECUTE_ASSERT(::SetEvent(m_hNotifyCallerPinBlockedEvent));
    EXECUTE_ASSERT(::CloseHandle(m_hNotifyCallerPinBlockedEvent));

    m_BlockState = BLOCKED;
    m_hNotifyCallerPinBlockedEvent = NULL;
}

HRESULT CDynamicOutputPin::UnblockOutputPin(void)
{
    // UnblockOutputPin() holds the m_BlockStateLock because it
    // uses m_BlockState, m_dwBlockCallerThreadID and
    // m_hNotifyCallerPinBlockedEvent.
    CAutoLock alBlockStateLock(&m_BlockStateLock);

    if(NOT_BLOCKED == m_BlockState) {
        return S_FALSE;
    }

    // This should not fail because we successfully created the event
    // and we have the security permissions to change it's state.
    EXECUTE_ASSERT(::SetEvent(m_hUnblockOutputPinEvent));

    // Cancel the block operation if it's still pending.
    if(NULL != m_hNotifyCallerPinBlockedEvent) {
        // This event should not fail because AsynchronousBlockOutputPin() successfully
        // duplicated this handle and we have the appropriate security permissions.
        EXECUTE_ASSERT(::SetEvent(m_hNotifyCallerPinBlockedEvent));
        EXECUTE_ASSERT(::CloseHandle(m_hNotifyCallerPinBlockedEvent));
    }

    m_BlockState = NOT_BLOCKED;
    m_dwBlockCallerThreadID = 0;
    m_hNotifyCallerPinBlockedEvent = NULL;

    return S_OK;
}

HRESULT CDynamicOutputPin::StartUsingOutputPin(void)
{
    // The caller should not hold m_BlockStateLock.  If the caller does,
    // a deadlock could occur.
    ASSERT(CritCheckOut(&m_BlockStateLock));

    CAutoLock alBlockStateLock(&m_BlockStateLock);

    #ifdef DEBUG
    AssertValid();
    #endif // DEBUG

    // Are we in the middle of a block operation?
    while(BLOCKED == m_BlockState) {
        m_BlockStateLock.Unlock();

        // If this ASSERT fires, a deadlock could occur.  The caller should make sure
        // that this thread never acquires the Block State lock more than once.
        ASSERT(CritCheckOut( &m_BlockStateLock ));

        // WaitForMultipleObjects() returns WAIT_OBJECT_0 if the unblock event
        // is fired.  It returns WAIT_OBJECT_0 + 1 if the stop event if fired.
        // See the Windows SDK documentation for more information on
        // WaitForMultipleObjects().
        const DWORD UNBLOCK = WAIT_OBJECT_0;
        const DWORD STOP = WAIT_OBJECT_0 + 1;

        HANDLE ahWaitEvents[] = { m_hUnblockOutputPinEvent, m_hStopEvent };
        DWORD dwNumWaitEvents = sizeof(ahWaitEvents)/sizeof(HANDLE);

        DWORD dwReturnValue = ::WaitForMultipleObjects( dwNumWaitEvents, ahWaitEvents, FALSE, INFINITE );

        m_BlockStateLock.Lock();

        #ifdef DEBUG
        AssertValid();
        #endif // DEBUG

        switch( dwReturnValue ) {
        case UNBLOCK:
            break;

        case STOP:
            return VFW_E_STATE_CHANGED;

        case WAIT_FAILED:
            return AmGetLastErrorToHResult();

        default:
            DbgBreak( "An Unexpected case occured in CDynamicOutputPin::StartUsingOutputPin()." );
            return E_UNEXPECTED;
        }
    }

    m_dwNumOutstandingOutputPinUsers++;

    #ifdef DEBUG
    AssertValid();
    #endif // DEBUG

    return S_OK;
}

void CDynamicOutputPin::StopUsingOutputPin(void)
{
    CAutoLock alBlockStateLock(&m_BlockStateLock);

    #ifdef DEBUG
    AssertValid();
    #endif // DEBUG

    m_dwNumOutstandingOutputPinUsers--;

    if((m_dwNumOutstandingOutputPinUsers == 0) && (NOT_BLOCKED != m_BlockState)) {
        BlockOutputPin();
    }

    #ifdef DEBUG
    AssertValid();
    #endif // DEBUG
}

bool CDynamicOutputPin::StreamingThreadUsingOutputPin(void)
{
    CAutoLock alBlockStateLock(&m_BlockStateLock);

    return (m_dwNumOutstandingOutputPinUsers > 0);
}

void CDynamicOutputPin::SetConfigInfo(IGraphConfig *pGraphConfig, HANDLE hStopEvent)
{
    // This pointer is not addrefed because filters are not allowed to
    // hold references to the filter graph manager.  See the documentation for
    // IBaseFilter::JoinFilterGraph() in the Direct Show SDK for more information.
    m_pGraphConfig = pGraphConfig;

    m_hStopEvent = hStopEvent;
}

HRESULT CDynamicOutputPin::Active(void)
{
    // Make sure the user initialized the object by calling SetConfigInfo().
    if((NULL == m_hStopEvent) || (NULL == m_pGraphConfig)) {
        DbgBreak( ERROR: CDynamicOutputPin::Active() failed because m_pGraphConfig and m_hStopEvent were not initialized.  Call SetConfigInfo() to initialize them. );
        return E_FAIL;
    }

    // If this ASSERT fires, the user may have passed an invalid event handle to SetConfigInfo().
    // The ASSERT can also fire if the event if destroyed and then Active() is called.  An event
    // handle is invalid if 1) the event does not exist or the user does not have the security
    // permissions to use the event.
    EXECUTE_ASSERT(ResetEvent(m_hStopEvent));

    return CBaseOutputPin::Active();
}

HRESULT CDynamicOutputPin::Inactive(void)
{
    // If this ASSERT fires, the user may have passed an invalid event handle to SetConfigInfo().
    // The ASSERT can also fire if the event if destroyed and then Active() is called.  An event
    // handle is invalid if 1) the event does not exist or the user does not have the security
    // permissions to use the event.
    EXECUTE_ASSERT(SetEvent(m_hStopEvent));

    return CBaseOutputPin::Inactive();
}

HRESULT CDynamicOutputPin::DeliverBeginFlush(void)
{
    // If this ASSERT fires, the user may have passed an invalid event handle to SetConfigInfo().
    // The ASSERT can also fire if the event if destroyed and then DeliverBeginFlush() is called.
    // An event handle is invalid if 1) the event does not exist or the user does not have the security
    // permissions to use the event.
    EXECUTE_ASSERT(SetEvent(m_hStopEvent));

    return CBaseOutputPin::DeliverBeginFlush();
}

HRESULT CDynamicOutputPin::DeliverEndFlush(void)
{
    // If this ASSERT fires, the user may have passed an invalid event handle to SetConfigInfo().
    // The ASSERT can also fire if the event if destroyed and then DeliverBeginFlush() is called.
    // An event handle is invalid if 1) the event does not exist or the user does not have the security
    // permissions to use the event.
    EXECUTE_ASSERT(ResetEvent(m_hStopEvent));

    return CBaseOutputPin::DeliverEndFlush();
}


// ChangeOutputFormat() either dynamicly changes the connection's format type or it dynamicly
// reconnects the output pin.
HRESULT CDynamicOutputPin::ChangeOutputFormat
    (
    const AM_MEDIA_TYPE *pmt,
    REFERENCE_TIME tSegmentStart,
    REFERENCE_TIME tSegmentStop,
    double dSegmentRate
    )
{
    // The caller should call StartUsingOutputPin() before calling this
    // method.
    ASSERT(StreamingThreadUsingOutputPin());

    // Callers should always pass a valid media type to ChangeOutputFormat() .
    ASSERT(NULL != pmt);

    CMediaType cmt(*pmt);
    HRESULT hr = ChangeMediaType(&cmt);
    if (FAILED(hr)) {
        return hr;
    }

    hr = DeliverNewSegment(tSegmentStart, tSegmentStop, dSegmentRate);
    if( FAILED( hr ) ) {
        return hr;
    }

    return S_OK;
}

HRESULT CDynamicOutputPin::ChangeMediaType(const CMediaType *pmt)
{
    // The caller should call StartUsingOutputPin() before calling this
    // method.
    ASSERT(StreamingThreadUsingOutputPin());

    // This function assumes the filter graph is running.
    ASSERT(!IsStopped());

    if(!IsConnected()) {
        return VFW_E_NOT_CONNECTED;
    }

    /*  First check if the downstream pin will accept a dynamic
        format change
    */
    QzCComPtr<IPinConnection> pConnection;

    m_Connected->QueryInterface(IID_IPinConnection, (void **)&pConnection);
    if(pConnection != NULL) {

        if(S_OK == pConnection->DynamicQueryAccept(pmt)) {

            HRESULT hr = ChangeMediaTypeHelper(pmt);
            if(FAILED(hr)) {
                return hr;
            }

            return S_OK;
        }
    }

    /*  Can't do the dynamic connection */
    return DynamicReconnect(pmt);
}

HRESULT CDynamicOutputPin::ChangeMediaTypeHelper(const CMediaType *pmt)
{
    // The caller should call StartUsingOutputPin() before calling this
    // method.
    ASSERT(StreamingThreadUsingOutputPin());

    HRESULT hr = m_Connected->ReceiveConnection(this, pmt);
    if(FAILED(hr)) {
        return hr;
    }

    hr = SetMediaType(pmt);
    if(FAILED(hr)) {
        return hr;
    }

    // Does this pin use the local memory transport?
    if(NULL != m_pInputPin) {
        // This function assumes that m_pInputPin and m_Connected are
        // two different interfaces to the same object.
        ASSERT(::IsEqualObject(m_Connected, m_pInputPin));

        ALLOCATOR_PROPERTIES apInputPinRequirements;
        apInputPinRequirements.cbAlign = 0;
        apInputPinRequirements.cbBuffer = 0;
        apInputPinRequirements.cbPrefix = 0;
        apInputPinRequirements.cBuffers = 0;

        m_pInputPin->GetAllocatorRequirements(&apInputPinRequirements);

        // A zero allignment does not make any sense.
        if(0 == apInputPinRequirements.cbAlign) {
            apInputPinRequirements.cbAlign = 1;
        }

        hr = m_pAllocator->Decommit();
        if(FAILED(hr)) {
            return hr;
        }

        hr = DecideBufferSize(m_pAllocator,  &apInputPinRequirements);
        if(FAILED(hr)) {
            return hr;
        }

        hr = m_pAllocator->Commit();
        if(FAILED(hr)) {
            return hr;
        }

        hr = m_pInputPin->NotifyAllocator(m_pAllocator, m_bPinUsesReadOnlyAllocator);
        if(FAILED(hr)) {
            return hr;
        }
    }

    return S_OK;
}

// this method has to be called from the thread that is pushing data,
// and it's the caller's responsibility to make sure that the thread
// has no outstand samples because they cannot be delivered after a
// reconnect
//
HRESULT CDynamicOutputPin::DynamicReconnect( const CMediaType* pmt )
{
    // The caller should call StartUsingOutputPin() before calling this
    // method.
    ASSERT(StreamingThreadUsingOutputPin());

    if((m_pGraphConfig == NULL) || (NULL == m_hStopEvent)) {
        return E_FAIL;
    }

    HRESULT hr = m_pGraphConfig->Reconnect(
        this,
        NULL,
        pmt,
        NULL,
        m_hStopEvent,
        AM_GRAPH_CONFIG_RECONNECT_CACHE_REMOVED_FILTERS );

    return hr;
}

HRESULT CDynamicOutputPin::CompleteConnect(IPin *pReceivePin)
{
    HRESULT hr = CBaseOutputPin::CompleteConnect(pReceivePin);
    if(SUCCEEDED(hr)) {
        if(!IsStopped() && m_pAllocator) {
            hr = m_pAllocator->Commit();
            ASSERT(hr != VFW_E_ALREADY_COMMITTED);
        }
    }

    return hr;
}

#ifdef DEBUG
void CDynamicOutputPin::AssertValid(void)
{
    // Make sure the object was correctly initialized.

    // This ASSERT only fires if the object failed to initialize
    // and the user ignored the constructor's return code (phr).
    ASSERT(NULL != m_hUnblockOutputPinEvent);

    // If either of these ASSERTs fire, the user did not correctly call
    // SetConfigInfo().
    ASSERT(NULL != m_hStopEvent);
    ASSERT(NULL != m_pGraphConfig);

    // Make sure the block state is consistent.

    CAutoLock alBlockStateLock(&m_BlockStateLock);

    // BLOCK_STATE variables only have three legal values: PENDING, BLOCKED and NOT_BLOCKED.
    ASSERT((NOT_BLOCKED == m_BlockState) || (PENDING == m_BlockState) || (BLOCKED == m_BlockState));

    // m_hNotifyCallerPinBlockedEvent is only needed when a block operation cannot complete
    // immediately.
    ASSERT(((NULL == m_hNotifyCallerPinBlockedEvent) && (PENDING != m_BlockState)) ||
           ((NULL != m_hNotifyCallerPinBlockedEvent) && (PENDING == m_BlockState)) );

    // m_dwBlockCallerThreadID should always be 0 if the pin is not blocked and
    // the user is not trying to block the pin.
    ASSERT((0 == m_dwBlockCallerThreadID) || (NOT_BLOCKED != m_BlockState));

    // If this ASSERT fires, the streaming thread is using the output pin and the
    // output pin is blocked.
    ASSERT(((0 != m_dwNumOutstandingOutputPinUsers) && (BLOCKED != m_BlockState)) ||
           ((0 == m_dwNumOutstandingOutputPinUsers) && (NOT_BLOCKED != m_BlockState)) ||
           ((0 == m_dwNumOutstandingOutputPinUsers) && (NOT_BLOCKED == m_BlockState)) );
}
#endif // DEBUG

HRESULT CDynamicOutputPin::WaitEvent(HANDLE hEvent)
{
    const DWORD EVENT_SIGNALED = WAIT_OBJECT_0;

    DWORD dwReturnValue = ::WaitForSingleObject(hEvent, INFINITE);

    switch( dwReturnValue ) {
    case EVENT_SIGNALED:
        return S_OK;

    case WAIT_FAILED:
        return AmGetLastErrorToHResult();

    default:
        DbgBreak( "An Unexpected case occured in CDynamicOutputPin::WaitEvent()." );
        return E_UNEXPECTED;
    }
}

//=====================================================================
//=====================================================================
// Implements CBaseAllocator
//=====================================================================
//=====================================================================


/* Constructor overrides the default settings for the free list to request
   that it be alertable (ie the list can be cast to a handle which can be
   passed to WaitForSingleObject). Both of the allocator lists also ask for
   object locking, the all list matches the object default settings but I
   have included them here just so it is obvious what kind of list it is */

CBaseAllocator::CBaseAllocator(__in_opt LPCTSTR pName,
                               __inout_opt LPUNKNOWN pUnk,
                               __inout HRESULT *phr,
                               BOOL bEvent,
                               BOOL fEnableReleaseCallback
                               ) :
    CUnknown(pName, pUnk),
    m_lAllocated(0),
    m_bChanged(FALSE),
    m_bCommitted(FALSE),
    m_bDecommitInProgress(FALSE),
    m_lSize(0),
    m_lCount(0),
    m_lAlignment(0),
    m_lPrefix(0),
    m_hSem(NULL),
    m_lWaiting(0),
    m_fEnableReleaseCallback(fEnableReleaseCallback),
    m_pNotify(NULL)
{
#ifdef DXMPERF
    PERFLOG_CTOR( pName ? pName : L"CBaseAllocator", (IMemAllocator *) this );
#endif // DXMPERF

    if (bEvent) {
        m_hSem = CreateSemaphore(NULL, 0, 0x7FFFFFFF, NULL);
        if (m_hSem == NULL) {
            *phr = E_OUTOFMEMORY;
            return;
        }
    }
}

#ifdef UNICODE
CBaseAllocator::CBaseAllocator(__in_opt LPCSTR pName,
                               __inout_opt LPUNKNOWN pUnk,
                               __inout HRESULT *phr,
                               BOOL bEvent,
                               BOOL fEnableReleaseCallback) :
    CUnknown(pName, pUnk),
    m_lAllocated(0),
    m_bChanged(FALSE),
    m_bCommitted(FALSE),
    m_bDecommitInProgress(FALSE),
    m_lSize(0),
    m_lCount(0),
    m_lAlignment(0),
    m_lPrefix(0),
    m_hSem(NULL),
    m_lWaiting(0),
    m_fEnableReleaseCallback(fEnableReleaseCallback),
    m_pNotify(NULL)
{
#ifdef DXMPERF
    PERFLOG_CTOR( L"CBaseAllocator", (IMemAllocator *) this );
#endif // DXMPERF

    if (bEvent) {
        m_hSem = CreateSemaphore(NULL, 0, 0x7FFFFFFF, NULL);
        if (m_hSem == NULL) {
            *phr = E_OUTOFMEMORY;
            return;
        }
    }
}
#endif

/* Destructor */

CBaseAllocator::~CBaseAllocator()
{
    // we can't call Decommit here since that would mean a call to a
    // pure virtual in destructor.
    // We must assume that the derived class has gone into decommit state in
    // its destructor.
#ifdef DXMPERF
    PERFLOG_DTOR( L"CBaseAllocator", (IMemAllocator *) this );
#endif // DXMPERF

    ASSERT(!m_bCommitted);
    if (m_hSem != NULL) {
        EXECUTE_ASSERT(CloseHandle(m_hSem));
    }
    if (m_pNotify) {
        m_pNotify->Release();
    }
}


/* Override this to publicise our interfaces */

STDMETHODIMP
CBaseAllocator::NonDelegatingQueryInterface(REFIID riid, __deref_out void **ppv)
{
    /* Do we know about this interface */

    if (riid == IID_IMemAllocator ||
        riid == IID_IMemAllocatorCallbackTemp && m_fEnableReleaseCallback) {
        return GetInterface((IMemAllocatorCallbackTemp *) this, ppv);
    } else {
        return CUnknown::NonDelegatingQueryInterface(riid, ppv);
    }
}


/* This sets the size and count of the required samples. The memory isn't
   actually allocated until Commit() is called, if memory has already been
   allocated then assuming no samples are outstanding the user may call us
   to change the buffering, the memory will be released in Commit() */

STDMETHODIMP
CBaseAllocator::SetProperties(
                __in ALLOCATOR_PROPERTIES* pRequest,
                __out ALLOCATOR_PROPERTIES* pActual)
{
    CheckPointer(pRequest, E_POINTER);
    CheckPointer(pActual, E_POINTER);
    ValidateReadWritePtr(pActual, sizeof(ALLOCATOR_PROPERTIES));
    CAutoLock cObjectLock(this);

    ZeroMemory(pActual, sizeof(ALLOCATOR_PROPERTIES));

    ASSERT(pRequest->cbBuffer > 0);

    /*  Check the alignment requested */
    if (pRequest->cbAlign != 1) {
        DbgLog((LOG_ERROR, 2, TEXT("Alignment requested was 0x%x, not 1"),
               pRequest->cbAlign));
        return VFW_E_BADALIGN;
    }

    /* Can't do this if already committed, there is an argument that says we
       should not reject the SetProperties call if there are buffers still
       active. However this is called by the source filter, which is the same
       person who is holding the samples. Therefore it is not unreasonable
       for them to free all their samples before changing the requirements */

    if (m_bCommitted) {
        return VFW_E_ALREADY_COMMITTED;
    }

    /* Must be no outstanding buffers */

    if (m_lAllocated != m_lFree.GetCount()) {
        return VFW_E_BUFFERS_OUTSTANDING;
    }

    /* There isn't any real need to check the parameters as they
       will just be rejected when the user finally calls Commit */

    pActual->cbBuffer = m_lSize = pRequest->cbBuffer;
    pActual->cBuffers = m_lCount = pRequest->cBuffers;
    pActual->cbAlign = m_lAlignment = pRequest->cbAlign;
    pActual->cbPrefix = m_lPrefix = pRequest->cbPrefix;

    m_bChanged = TRUE;
    return NOERROR;
}

STDMETHODIMP
CBaseAllocator::GetProperties(
    __out ALLOCATOR_PROPERTIES * pActual)
{
    CheckPointer(pActual,E_POINTER);
    ValidateReadWritePtr(pActual,sizeof(ALLOCATOR_PROPERTIES));

    CAutoLock cObjectLock(this);
    pActual->cbBuffer = m_lSize;
    pActual->cBuffers = m_lCount;
    pActual->cbAlign = m_lAlignment;
    pActual->cbPrefix = m_lPrefix;
    return NOERROR;
}

// get container for a sample. Blocking, synchronous call to get the
// next free buffer (as represented by an IMediaSample interface).
// on return, the time etc properties will be invalid, but the buffer
// pointer and size will be correct.

HRESULT CBaseAllocator::GetBuffer(__deref_out IMediaSample **ppBuffer,
                                  __in_opt REFERENCE_TIME *pStartTime,
                                  __in_opt REFERENCE_TIME *pEndTime,
                                  DWORD dwFlags
                                  )
{
    UNREFERENCED_PARAMETER(pStartTime);
    UNREFERENCED_PARAMETER(pEndTime);
    UNREFERENCED_PARAMETER(dwFlags);
    CMediaSample *pSample;

    *ppBuffer = NULL;
    for (;;)
    {
        {  // scope for lock
            CAutoLock cObjectLock(this);

            /* Check we are committed */
            if (!m_bCommitted) {
                return VFW_E_NOT_COMMITTED;
            }
            pSample = (CMediaSample *) m_lFree.RemoveHead();
            if (pSample == NULL) {
                SetWaiting();
            }
        }

        /* If we didn't get a sample then wait for the list to signal */

        if (pSample) {
            break;
        }
        if (dwFlags & AM_GBF_NOWAIT) {
            return VFW_E_TIMEOUT;
        }
        ASSERT(m_hSem != NULL);
        WaitForSingleObject(m_hSem, INFINITE);
    }

    /* Addref the buffer up to one. On release
       back to zero instead of being deleted, it will requeue itself by
       calling the ReleaseBuffer member function. NOTE the owner of a
       media sample must always be derived from CBaseAllocator */


    ASSERT(pSample->m_cRef == 0);
    pSample->m_cRef = 1;
    *ppBuffer = pSample;

#ifdef DXMPERF
    PERFLOG_GETBUFFER( (IMemAllocator *) this, pSample );
#endif // DXMPERF

    return NOERROR;
}


/* Final release of a CMediaSample will call this */

STDMETHODIMP
CBaseAllocator::ReleaseBuffer(IMediaSample * pSample)
{
    CheckPointer(pSample,E_POINTER);
    ValidateReadPtr(pSample,sizeof(IMediaSample));

#ifdef DXMPERF
    PERFLOG_RELBUFFER( (IMemAllocator *) this, pSample );
#endif // DXMPERF


    BOOL bRelease = FALSE;
    {
        CAutoLock cal(this);

        /* Put back on the free list */

        m_lFree.Add((CMediaSample *)pSample);
        if (m_lWaiting != 0) {
            NotifySample();
        }

        // if there is a pending Decommit, then we need to complete it by
        // calling Free() when the last buffer is placed on the free list

        LONG l1 = m_lFree.GetCount();
        if (m_bDecommitInProgress && (l1 == m_lAllocated)) {
            Free();
            m_bDecommitInProgress = FALSE;
            bRelease = TRUE;
        }
    }

    if (m_pNotify) {

        ASSERT(m_fEnableReleaseCallback);

        //
        // Note that this is not synchronized with setting up a notification
        // method.
        //
        m_pNotify->NotifyRelease();
    }

    /* For each buffer there is one AddRef, made in GetBuffer and released
       here. This may cause the allocator and all samples to be deleted */

    if (bRelease) {
        Release();
    }
    return NOERROR;
}

STDMETHODIMP
CBaseAllocator::SetNotify(
    IMemAllocatorNotifyCallbackTemp* pNotify
    )
{
    ASSERT(m_fEnableReleaseCallback);
    CAutoLock lck(this);
    if (pNotify) {
        pNotify->AddRef();
    }
    if (m_pNotify) {
        m_pNotify->Release();
    }
    m_pNotify = pNotify;
    return S_OK;
}

STDMETHODIMP
CBaseAllocator::GetFreeCount(
    __out LONG* plBuffersFree
    )
{
    ASSERT(m_fEnableReleaseCallback);
    CAutoLock cObjectLock(this);
    *plBuffersFree = m_lCount - m_lAllocated + m_lFree.GetCount();
    return NOERROR;
}

void
CBaseAllocator::NotifySample()
{
    if (m_lWaiting != 0) {
        ASSERT(m_hSem != NULL);
        ReleaseSemaphore(m_hSem, m_lWaiting, 0);
        m_lWaiting = 0;
    }
}

STDMETHODIMP
CBaseAllocator::Commit()
{
    /* Check we are not decommitted */
    CAutoLock cObjectLock(this);

    // cannot need to alloc or re-alloc if we are committed
    if (m_bCommitted) {
        return NOERROR;
    }

    /* Allow GetBuffer calls */

    m_bCommitted = TRUE;

    // is there a pending decommit ? if so, just cancel it
    if (m_bDecommitInProgress) {
        m_bDecommitInProgress = FALSE;

        // don't call Alloc at this point. He cannot allow SetProperties
        // between Decommit and the last free, so the buffer size cannot have
        // changed. And because some of the buffers are not free yet, he
        // cannot re-alloc anyway.
        return NOERROR;
    }

    DbgLog((LOG_MEMORY, 1, TEXT("Allocating: %ldx%ld"), m_lCount, m_lSize));

    // actually need to allocate the samples
    HRESULT hr = Alloc();
    if (FAILED(hr)) {
        m_bCommitted = FALSE;
        return hr;
    }
    AddRef();
    return NOERROR;
}


STDMETHODIMP
CBaseAllocator::Decommit()
{
    BOOL bRelease = FALSE;
    {
        /* Check we are not already decommitted */
        CAutoLock cObjectLock(this);
        if (m_bCommitted == FALSE) {
            if (m_bDecommitInProgress == FALSE) {
                return NOERROR;
            }
        }

        /* No more GetBuffer calls will succeed */
        m_bCommitted = FALSE;

        // are any buffers outstanding?
        if (m_lFree.GetCount() < m_lAllocated) {
            // please complete the decommit when last buffer is freed
            m_bDecommitInProgress = TRUE;
        } else {
            m_bDecommitInProgress = FALSE;

            // need to complete the decommit here as there are no
            // outstanding buffers

            Free();
            bRelease = TRUE;
        }

        // Tell anyone waiting that they can go now so we can
        // reject their call
#pragma warning(push)
#ifndef _PREFAST_
#pragma warning(disable:4068)
#endif
#pragma prefast(suppress:__WARNING_DEREF_NULL_PTR, "Suppress warning related to Free() invalidating 'this' which is no applicable to CBaseAllocator::Free()")
        NotifySample();

#pragma warning(pop)
    }

    if (bRelease) {
        Release();
    }
    return NOERROR;
}


/* Base definition of allocation which checks we are ok to go ahead and do
   the full allocation. We return S_FALSE if the requirements are the same */

HRESULT
CBaseAllocator::Alloc(void)
{
    /* Error if he hasn't set the size yet */
    if (m_lCount <= 0 || m_lSize <= 0 || m_lAlignment <= 0) {
        return VFW_E_SIZENOTSET;
    }

    /* should never get here while buffers outstanding */
    ASSERT(m_lFree.GetCount() == m_lAllocated);

    /* If the requirements haven't changed then don't reallocate */
    if (m_bChanged == FALSE) {
        return S_FALSE;
    }

    return NOERROR;
}

/*  Implement CBaseAllocator::CSampleList::Remove(pSample)
    Removes pSample from the list
*/
void
CBaseAllocator::CSampleList::Remove(__inout CMediaSample * pSample)
{
    CMediaSample **pSearch;
    for (pSearch = &m_List;
         *pSearch != NULL;
         pSearch = &(CBaseAllocator::NextSample(*pSearch))) {
       if (*pSearch == pSample) {
           *pSearch = CBaseAllocator::NextSample(pSample);
           CBaseAllocator::NextSample(pSample) = NULL;
           m_nOnList--;
           return;
       }
    }
    DbgBreak("Couldn't find sample in list");
}

//=====================================================================
//=====================================================================
// Implements CMemAllocator
//=====================================================================
//=====================================================================


/* This goes in the factory template table to create new instances */
CUnknown *CMemAllocator::CreateInstance(__inout_opt LPUNKNOWN pUnk, __inout HRESULT *phr)
{
    CUnknown *pUnkRet = new CMemAllocator(NAME("CMemAllocator"), pUnk, phr);
    return pUnkRet;
}

CMemAllocator::CMemAllocator(
    __in_opt LPCTSTR pName,
    __inout_opt LPUNKNOWN pUnk,
    __inout HRESULT *phr)
    : CBaseAllocator(pName, pUnk, phr, TRUE, TRUE),
    m_pBuffer(NULL)
{
}

#ifdef UNICODE
CMemAllocator::CMemAllocator(
    __in_opt LPCSTR pName,
    __inout_opt LPUNKNOWN pUnk,
    __inout HRESULT *phr)
    : CBaseAllocator(pName, pUnk, phr, TRUE, TRUE),
    m_pBuffer(NULL)
{
}
#endif

/* This sets the size and count of the required samples. The memory isn't
   actually allocated until Commit() is called, if memory has already been
   allocated then assuming no samples are outstanding the user may call us
   to change the buffering, the memory will be released in Commit() */
STDMETHODIMP
CMemAllocator::SetProperties(
                __in ALLOCATOR_PROPERTIES* pRequest,
                __out ALLOCATOR_PROPERTIES* pActual)
{
    CheckPointer(pActual,E_POINTER);
    ValidateReadWritePtr(pActual,sizeof(ALLOCATOR_PROPERTIES));
    CAutoLock cObjectLock(this);

    ZeroMemory(pActual, sizeof(ALLOCATOR_PROPERTIES));

    ASSERT(pRequest->cbBuffer > 0);

    SYSTEM_INFO SysInfo;
    GetSystemInfo(&SysInfo);

    /*  Check the alignment request is a power of 2 */
    if ((-pRequest->cbAlign & pRequest->cbAlign) != pRequest->cbAlign) {
        DbgLog((LOG_ERROR, 1, TEXT("Alignment requested 0x%x not a power of 2!"),
               pRequest->cbAlign));
    }
    /*  Check the alignment requested */
    if (pRequest->cbAlign == 0 ||
    (SysInfo.dwAllocationGranularity & (pRequest->cbAlign - 1)) != 0) {
        DbgLog((LOG_ERROR, 1, TEXT("Invalid alignment 0x%x requested - granularity = 0x%x"),
               pRequest->cbAlign, SysInfo.dwAllocationGranularity));
        return VFW_E_BADALIGN;
    }

    /* Can't do this if already committed, there is an argument that says we
       should not reject the SetProperties call if there are buffers still
       active. However this is called by the source filter, which is the same
       person who is holding the samples. Therefore it is not unreasonable
       for them to free all their samples before changing the requirements */

    if (m_bCommitted == TRUE) {
        return VFW_E_ALREADY_COMMITTED;
    }

    /* Must be no outstanding buffers */

    if (m_lFree.GetCount() < m_lAllocated) {
        return VFW_E_BUFFERS_OUTSTANDING;
    }

    /* There isn't any real need to check the parameters as they
       will just be rejected when the user finally calls Commit */

    // round length up to alignment - remember that prefix is included in
    // the alignment
    LONG lSize = pRequest->cbBuffer + pRequest->cbPrefix;
    LONG lRemainder = lSize % pRequest->cbAlign;
    if (lRemainder != 0) {
        lSize = lSize - lRemainder + pRequest->cbAlign;
    }
    pActual->cbBuffer = m_lSize = (lSize - pRequest->cbPrefix);

    pActual->cBuffers = m_lCount = pRequest->cBuffers;
    pActual->cbAlign = m_lAlignment = pRequest->cbAlign;
    pActual->cbPrefix = m_lPrefix = pRequest->cbPrefix;

    m_bChanged = TRUE;
    return NOERROR;
}

// override this to allocate our resources when Commit is called.
//
// note that our resources may be already allocated when this is called,
// since we don't free them on Decommit. We will only be called when in
// decommit state with all buffers free.
//
// object locked by caller
HRESULT
CMemAllocator::Alloc(void)
{
    CAutoLock lck(this);

    /* Check he has called SetProperties */
    HRESULT hr = CBaseAllocator::Alloc();
    if (FAILED(hr)) {
        return hr;
    }

    /* If the requirements haven't changed then don't reallocate */
    if (hr == S_FALSE) {
        ASSERT(m_pBuffer);
        return NOERROR;
    }
    ASSERT(hr == S_OK); // we use this fact in the loop below

    /* Free the old resources */
    if (m_pBuffer) {
        ReallyFree();
    }

    /* Make sure we've got reasonable values */
    if ( m_lSize < 0 || m_lPrefix < 0 || m_lCount < 0 ) {
        return E_OUTOFMEMORY;
    }

    /* Compute the aligned size */
    LONG lAlignedSize = m_lSize + m_lPrefix;

    /*  Check overflow */
    if (lAlignedSize < m_lSize) {
        return E_OUTOFMEMORY;
    }

    if (m_lAlignment > 1) {
        LONG lRemainder = lAlignedSize % m_lAlignment;
        if (lRemainder != 0) {
            LONG lNewSize = lAlignedSize + m_lAlignment - lRemainder;
            if (lNewSize < lAlignedSize) {
                return E_OUTOFMEMORY;
            }
            lAlignedSize = lNewSize;
        }
    }

    /* Create the contiguous memory block for the samples
       making sure it's properly aligned (64K should be enough!)
    */
    ASSERT(lAlignedSize % m_lAlignment == 0);

    LONGLONG lToAllocate = m_lCount * (LONGLONG)lAlignedSize;

    /*  Check overflow */
    if (lToAllocate > MAXLONG) {
        return E_OUTOFMEMORY;
    }

    m_pBuffer = (PBYTE)VirtualAlloc(NULL,
                    (LONG)lToAllocate,
                    MEM_COMMIT,
                    PAGE_READWRITE);

    if (m_pBuffer == NULL) {
        return E_OUTOFMEMORY;
    }

    LPBYTE pNext = m_pBuffer;
    CMediaSample *pSample;

    ASSERT(m_lAllocated == 0);

    // Create the new samples - we have allocated m_lSize bytes for each sample
    // plus m_lPrefix bytes per sample as a prefix. We set the pointer to
    // the memory after the prefix - so that GetPointer() will return a pointer
    // to m_lSize bytes.
    for (; m_lAllocated < m_lCount; m_lAllocated++, pNext += lAlignedSize) {


        pSample = new CMediaSample(
                            NAME("Default memory media sample"),
                this,
                            &hr,
                            pNext + m_lPrefix,      // GetPointer() value
                            m_lSize);               // not including prefix

            ASSERT(SUCCEEDED(hr));
        if (pSample == NULL) {
            return E_OUTOFMEMORY;
        }

        // This CANNOT fail
        m_lFree.Add(pSample);
    }

    m_bChanged = FALSE;
    return NOERROR;
}


// override this to free up any resources we have allocated.
// called from the base class on Decommit when all buffers have been
// returned to the free list.
//
// caller has already locked the object.

// in our case, we keep the memory until we are deleted, so
// we do nothing here. The memory is deleted in the destructor by
// calling ReallyFree()
void
CMemAllocator::Free(void)
{
    return;
}


// called from the destructor (and from Alloc if changing size/count) to
// actually free up the memory
void
CMemAllocator::ReallyFree(void)
{
    /* Should never be deleting this unless all buffers are freed */

    ASSERT(m_lAllocated == m_lFree.GetCount());

    /* Free up all the CMediaSamples */

    CMediaSample *pSample;
    for (;;) {
        pSample = m_lFree.RemoveHead();
        if (pSample != NULL) {
            delete pSample;
        } else {
            break;
        }
    }

    m_lAllocated = 0;

    // free the block of buffer memory
    if (m_pBuffer) {
        EXECUTE_ASSERT(VirtualFree(m_pBuffer, 0, MEM_RELEASE));
        m_pBuffer = NULL;
    }
}


/* Destructor frees our memory resources */

CMemAllocator::~CMemAllocator()
{
    Decommit();
    ReallyFree();
}

// ------------------------------------------------------------------------
// filter registration through IFilterMapper. used if IFilterMapper is
// not found (Quartz 1.0 install)

STDAPI
AMovieSetupRegisterFilter( const AMOVIESETUP_FILTER * const psetupdata
                         , IFilterMapper *                  pIFM
                         , BOOL                             bRegister  )
{
  DbgLog((LOG_TRACE, 3, TEXT("= AMovieSetupRegisterFilter")));

  // check we've got data
  //
  if( NULL == psetupdata ) return S_FALSE;


  // unregister filter
  // (as pins are subkeys of filter's CLSID key
  // they do not need to be removed separately).
  //
  DbgLog((LOG_TRACE, 3, TEXT("= = unregister filter")));
  HRESULT hr = pIFM->UnregisterFilter( *(psetupdata->clsID) );


  if( bRegister )
  {
    // register filter
    //
    DbgLog((LOG_TRACE, 3, TEXT("= = register filter")));
    hr = pIFM->RegisterFilter( *(psetupdata->clsID)
                             , psetupdata->strName
                             , psetupdata->dwMerit    );
    if( SUCCEEDED(hr) )
    {
      // all its pins
      //
      DbgLog((LOG_TRACE, 3, TEXT("= = register filter pins")));
      for( UINT m1=0; m1 < psetupdata->nPins; m1++ )
      {
        hr = pIFM->RegisterPin( *(psetupdata->clsID)
                              , psetupdata->lpPin[m1].strName
                              , psetupdata->lpPin[m1].bRendered
                              , psetupdata->lpPin[m1].bOutput
                              , psetupdata->lpPin[m1].bZero
                              , psetupdata->lpPin[m1].bMany
                              , *(psetupdata->lpPin[m1].clsConnectsToFilter)
                              , psetupdata->lpPin[m1].strConnectsToPin );

        if( SUCCEEDED(hr) )
        {
          // and each pin's media types
          //
          DbgLog((LOG_TRACE, 3, TEXT("= = register filter pin types")));
          for( UINT m2=0; m2 < psetupdata->lpPin[m1].nMediaTypes; m2++ )
          {
            hr = pIFM->RegisterPinType( *(psetupdata->clsID)
                                      , psetupdata->lpPin[m1].strName
                                      , *(psetupdata->lpPin[m1].lpMediaType[m2].clsMajorType)
                                      , *(psetupdata->lpPin[m1].lpMediaType[m2].clsMinorType) );
            if( FAILED(hr) ) break;
          }
          if( FAILED(hr) ) break;
        }
        if( FAILED(hr) ) break;
      }
    }
  }

  // handle one acceptable "error" - that
  // of filter not being registered!
  // (couldn't find a suitable #define'd
  // name for the error!)
  //
  if( 0x80070002 == hr)
    return NOERROR;
  else
    return hr;
}

//  Remove warnings about unreferenced inline functions
#pragma warning(disable:4514)

//------------------------------------------------------------------------------
// File: Source.cpp
//
// Desc: DirectShow  base classes - implements CSource, which is a Quartz
//       source filter 'template.'
//
// Copyright (c) 1992-2001 Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------

// Locking Strategy.
//
// Hold the filter critical section (m_pFilter->pStateLock()) to serialise
// access to functions. Note that, in general, this lock may be held
// by a function when the worker thread may want to hold it. Therefore
// if you wish to access shared state from the worker thread you will
// need to add another critical section object. The execption is during
// the threads processing loop, when it is safe to get the filter critical
// section from within FillBuffer().

//
// CSource::Constructor
//
// Initialise the pin count for the filter. The user will create the pins in
// the derived class.
CSource::CSource(__in_opt LPCTSTR pName, __inout_opt LPUNKNOWN lpunk, CLSID clsid)
    : CBaseFilter(pName, lpunk, &m_cStateLock, clsid),
      m_iPins(0),
      m_paStreams(NULL)
{
}

CSource::CSource(__in_opt LPCTSTR pName, __inout_opt LPUNKNOWN lpunk, CLSID clsid, __inout HRESULT *phr)
    : CBaseFilter(pName, lpunk, &m_cStateLock, clsid),
      m_iPins(0),
      m_paStreams(NULL)
{
    UNREFERENCED_PARAMETER(phr);
}

#ifdef UNICODE
CSource::CSource(__in_opt LPCSTR pName, __inout_opt LPUNKNOWN lpunk, CLSID clsid)
    : CBaseFilter(pName, lpunk, &m_cStateLock, clsid),
      m_iPins(0),
      m_paStreams(NULL)
{
}

CSource::CSource(__in_opt LPCSTR pName, __inout_opt LPUNKNOWN lpunk, CLSID clsid, __inout HRESULT *phr)
    : CBaseFilter(pName, lpunk, &m_cStateLock, clsid),
      m_iPins(0),
      m_paStreams(NULL)
{
    UNREFERENCED_PARAMETER(phr);
}
#endif

//
// CSource::Destructor
//
CSource::~CSource()
{
    /*  Free our pins and pin array */
    while (m_iPins != 0) {
	// deleting the pins causes them to be removed from the array...
	delete m_paStreams[m_iPins - 1];
    }

    ASSERT(m_paStreams == NULL);
}


//
//  Add a new pin
//
HRESULT CSource::AddPin(__in CSourceStream *pStream)
{
    CAutoLock lock(&m_cStateLock);

    /*  Allocate space for this pin and the old ones */
    CSourceStream **paStreams = new CSourceStream *[m_iPins + 1];
    if (paStreams == NULL) {
        return E_OUTOFMEMORY;
    }
    if (m_paStreams != NULL) {
        CopyMemory((PVOID)paStreams, (PVOID)m_paStreams,
                   m_iPins * sizeof(m_paStreams[0]));
        paStreams[m_iPins] = pStream;
        delete [] m_paStreams;
    }
    m_paStreams = paStreams;
    m_paStreams[m_iPins] = pStream;
    m_iPins++;
    return S_OK;
}

//
//  Remove a pin - pStream is NOT deleted
//
HRESULT CSource::RemovePin(__in CSourceStream *pStream)
{
    int i;
    for (i = 0; i < m_iPins; i++) {
        if (m_paStreams[i] == pStream) {
            if (m_iPins == 1) {
                delete [] m_paStreams;
                m_paStreams = NULL;
            } else {
                /*  no need to reallocate */
		while (++i < m_iPins)
		    m_paStreams[i - 1] = m_paStreams[i];
            }
            m_iPins--;
            return S_OK;
        }
    }
    return S_FALSE;
}

//
// FindPin
//
// Set *ppPin to the IPin* that has the id Id.
// or to NULL if the Id cannot be matched.
STDMETHODIMP CSource::FindPin(LPCWSTR Id, __deref_out IPin **ppPin)
{
    CheckPointer(ppPin,E_POINTER);
    ValidateReadWritePtr(ppPin,sizeof(IPin *));
    // The -1 undoes the +1 in QueryId and ensures that totally invalid
    // strings (for which WstrToInt delivers 0) give a deliver a NULL pin.
    int i = WstrToInt(Id) -1;
    *ppPin = GetPin(i);
    if (*ppPin!=NULL){
        (*ppPin)->AddRef();
        return NOERROR;
    } else {
        return VFW_E_NOT_FOUND;
    }
}

//
// FindPinNumber
//
// return the number of the pin with this IPin* or -1 if none
int CSource::FindPinNumber(__in IPin *iPin) {
    int i;
    for (i=0; i<m_iPins; ++i) {
        if ((IPin *)(m_paStreams[i])==iPin) {
            return i;
        }
    }
    return -1;
}

//
// GetPinCount
//
// Returns the number of pins this filter has
int CSource::GetPinCount(void) {

    CAutoLock lock(&m_cStateLock);
    return m_iPins;
}


//
// GetPin
//
// Return a non-addref'd pointer to pin n
// needed by CBaseFilter
CBasePin *CSource::GetPin(int n) {

    CAutoLock lock(&m_cStateLock);

    // n must be in the range 0..m_iPins-1
    // if m_iPins>n  && n>=0 it follows that m_iPins>0
    // which is what used to be checked (i.e. checking that we have a pin)
    if ((n >= 0) && (n < m_iPins)) {

        ASSERT(m_paStreams[n]);
	return m_paStreams[n];
    }
    return NULL;
}


//


// *
// * --- CSourceStream ----
// *

//
// Set Id to point to a CoTaskMemAlloc'd
STDMETHODIMP CSourceStream::QueryId(__deref_out LPWSTR *Id) {
    CheckPointer(Id,E_POINTER);
    ValidateReadWritePtr(Id,sizeof(LPWSTR));

    // We give the pins id's which are 1,2,...
    // FindPinNumber returns -1 for an invalid pin
    int i = 1+ m_pFilter->FindPinNumber(this);
    if (i<1) return VFW_E_NOT_FOUND;
    *Id = (LPWSTR)CoTaskMemAlloc(sizeof(WCHAR) * 12);
    if (*Id==NULL) {
       return E_OUTOFMEMORY;
    }
    IntToWstr(i, *Id);
    return NOERROR;
}



//
// CSourceStream::Constructor
//
// increments the number of pins present on the filter
CSourceStream::CSourceStream(
    __in_opt LPCTSTR pObjectName,
    __inout HRESULT *phr,
    __inout CSource *ps,
    __in_opt LPCWSTR pPinName)
    : CBaseOutputPin(pObjectName, ps, ps->pStateLock(), phr, pPinName),
      m_pFilter(ps) {

     *phr = m_pFilter->AddPin(this);
}

#ifdef UNICODE
CSourceStream::CSourceStream(
    __in_opt LPCSTR pObjectName,
    __inout HRESULT *phr,
    __inout CSource *ps,
    __in_opt LPCWSTR pPinName)
    : CBaseOutputPin(pObjectName, ps, ps->pStateLock(), phr, pPinName),
      m_pFilter(ps) {

     *phr = m_pFilter->AddPin(this);
}
#endif
//
// CSourceStream::Destructor
//
// Decrements the number of pins on this filter
CSourceStream::~CSourceStream(void) {

     m_pFilter->RemovePin(this);
}


//
// CheckMediaType
//
// Do we support this type? Provides the default support for 1 type.
HRESULT CSourceStream::CheckMediaType(const CMediaType *pMediaType) {

    CAutoLock lock(m_pFilter->pStateLock());

    CMediaType mt;
    GetMediaType(&mt);

    if (mt == *pMediaType) {
        return NOERROR;
    }

    return E_FAIL;
}


//
// GetMediaType/3
//
// By default we support only one type
// iPosition indexes are 0-n
HRESULT CSourceStream::GetMediaType(int iPosition, __inout CMediaType *pMediaType) {

    CAutoLock lock(m_pFilter->pStateLock());

    if (iPosition<0) {
        return E_INVALIDARG;
    }
    if (iPosition>0) {
        return VFW_S_NO_MORE_ITEMS;
    }
    return GetMediaType(pMediaType);
}


//
// Active
//
// The pin is active - start up the worker thread
HRESULT CSourceStream::Active(void) {

    CAutoLock lock(m_pFilter->pStateLock());

    HRESULT hr;

    if (m_pFilter->IsActive()) {
	return S_FALSE;	// succeeded, but did not allocate resources (they already exist...)
    }

    // do nothing if not connected - its ok not to connect to
    // all pins of a source filter
    if (!IsConnected()) {
        return NOERROR;
    }

    hr = CBaseOutputPin::Active();
    if (FAILED(hr)) {
        return hr;
    }

    ASSERT(!ThreadExists());

    // start the thread
    if (!Create()) {
        return E_FAIL;
    }

    // Tell thread to initialize. If OnThreadCreate Fails, so does this.
    hr = Init();
    if (FAILED(hr))
	return hr;

    return Pause();
}


//
// Inactive
//
// Pin is inactive - shut down the worker thread
// Waits for the worker to exit before returning.
HRESULT CSourceStream::Inactive(void) {

    CAutoLock lock(m_pFilter->pStateLock());

    HRESULT hr;

    // do nothing if not connected - its ok not to connect to
    // all pins of a source filter
    if (!IsConnected()) {
        return NOERROR;
    }

    // !!! need to do this before trying to stop the thread, because
    // we may be stuck waiting for our own allocator!!!

    hr = CBaseOutputPin::Inactive();  // call this first to Decommit the allocator
    if (FAILED(hr)) {
	return hr;
    }

    if (ThreadExists()) {
	hr = Stop();

	if (FAILED(hr)) {
	    return hr;
	}

	hr = Exit();
	if (FAILED(hr)) {
	    return hr;
	}

	Close();	// Wait for the thread to exit, then tidy up.
    }

    // hr = CBaseOutputPin::Inactive();  // call this first to Decommit the allocator
    //if (FAILED(hr)) {
    //	return hr;
    //}

    return NOERROR;
}


//
// ThreadProc
//
// When this returns the thread exits
// Return codes > 0 indicate an error occured
DWORD CSourceStream::ThreadProc(void) {

    HRESULT hr;  // the return code from calls
    Command com;

    do {
	com = GetRequest();
	if (com != CMD_INIT) {
	    DbgLog((LOG_ERROR, 1, TEXT("Thread expected init command")));
	    Reply((DWORD) E_UNEXPECTED);
	}
    } while (com != CMD_INIT);

    DbgLog((LOG_TRACE, 1, TEXT("CSourceStream worker thread initializing")));

    hr = OnThreadCreate(); // perform set up tasks
    if (FAILED(hr)) {
        DbgLog((LOG_ERROR, 1, TEXT("CSourceStream::OnThreadCreate failed. Aborting thread.")));
        OnThreadDestroy();
	Reply(hr);	// send failed return code from OnThreadCreate
        return 1;
    }

    // Initialisation suceeded
    Reply(NOERROR);

    Command cmd;
    do {
	cmd = GetRequest();

	switch (cmd) {

	case CMD_EXIT:
	    Reply(NOERROR);
	    break;

	case CMD_RUN:
	    DbgLog((LOG_ERROR, 1, TEXT("CMD_RUN received before a CMD_PAUSE???")));
	    // !!! fall through???
	
	case CMD_PAUSE:
	    Reply(NOERROR);
	    DoBufferProcessingLoop();
	    break;

	case CMD_STOP:
	    Reply(NOERROR);
	    break;

	default:
	    DbgLog((LOG_ERROR, 1, TEXT("Unknown command %d received!"), cmd));
	    Reply((DWORD) E_NOTIMPL);
	    break;
	}
    } while (cmd != CMD_EXIT);

    hr = OnThreadDestroy();	// tidy up.
    if (FAILED(hr)) {
        DbgLog((LOG_ERROR, 1, TEXT("CSourceStream::OnThreadDestroy failed. Exiting thread.")));
        return 1;
    }

    DbgLog((LOG_TRACE, 1, TEXT("CSourceStream worker thread exiting")));
    return 0;
}


//
// DoBufferProcessingLoop
//
// Grabs a buffer and calls the users processing function.
// Overridable, so that different delivery styles can be catered for.
HRESULT CSourceStream::DoBufferProcessingLoop(void) {

    Command com;

    OnThreadStartPlay();

    do {
	while (!CheckRequest(&com)) {

	    IMediaSample *pSample;

	    HRESULT hr = GetDeliveryBuffer(&pSample,NULL,NULL,0);
	    if (FAILED(hr)) {
                Sleep(1);
		continue;	// go round again. Perhaps the error will go away
			    // or the allocator is decommited & we will be asked to
			    // exit soon.
	    }

	    // Virtual function user will override.
	    hr = FillBuffer(pSample);

	    if (hr == S_OK) {
		hr = Deliver(pSample);
                pSample->Release();

                // downstream filter returns S_FALSE if it wants us to
                // stop or an error if it's reporting an error.
                if(hr != S_OK)
                {
                  DbgLog((LOG_TRACE, 2, TEXT("Deliver() returned %08x; stopping"), hr));
                  return S_OK;
                }

	    } else if (hr == S_FALSE) {
                // derived class wants us to stop pushing data
		pSample->Release();
		DeliverEndOfStream();
		return S_OK;
	    } else {
                // derived class encountered an error
                pSample->Release();
		DbgLog((LOG_ERROR, 1, TEXT("Error %08lX from FillBuffer!!!"), hr));
                DeliverEndOfStream();
                m_pFilter->NotifyEvent(EC_ERRORABORT, hr, 0);
                return hr;
	    }

            // all paths release the sample
	}

        // For all commands sent to us there must be a Reply call!

	if (com == CMD_RUN || com == CMD_PAUSE) {
	    Reply(NOERROR);
	} else if (com != CMD_STOP) {
	    Reply((DWORD) E_UNEXPECTED);
	    DbgLog((LOG_ERROR, 1, TEXT("Unexpected command!!!")));
	}
    } while (com != CMD_STOP);

    return S_FALSE;
}

//------------------------------------------------------------------------------
// File: CProp.cpp
//
// Desc: DirectShow base classes - implements CBasePropertyPage class.
//
// Copyright (c) 1992-2001 Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------

// Constructor for the base property page class. As described in the header
// file we must be initialised with dialog and title resource identifiers.
// The class supports IPropertyPage and overrides AddRef and Release calls
// to keep track of the reference counts. When the last count is released
// we call SetPageSite(NULL) and SetObjects(0,NULL) to release interfaces
// previously obtained by the property page when it had SetObjects called

CBasePropertyPage::CBasePropertyPage(__in_opt LPCTSTR pName,   // Debug only name
                                     __inout_opt LPUNKNOWN pUnk, // COM Delegator
                                     int DialogId,      // Resource ID
                                     int TitleId) :     // To get tital
    CUnknown(pName,pUnk),
    m_DialogId(DialogId),
    m_TitleId(TitleId),
    m_hwnd(NULL),
    m_Dlg(NULL),
    m_pPageSite(NULL),
    m_bObjectSet(FALSE),
    m_bDirty(FALSE)
{
}

#ifdef UNICODE
CBasePropertyPage::CBasePropertyPage(__in_opt LPCSTR pName,     // Debug only name
                                     __inout_opt LPUNKNOWN pUnk,  // COM Delegator
                                     int DialogId,      // Resource ID
                                     int TitleId) :     // To get tital
    CUnknown(pName,pUnk),
    m_DialogId(DialogId),
    m_TitleId(TitleId),
    m_hwnd(NULL),
    m_Dlg(NULL),
    m_pPageSite(NULL),
    m_bObjectSet(FALSE),
    m_bDirty(FALSE)
{
}
#endif

// Increment our reference count

STDMETHODIMP_(ULONG) CBasePropertyPage::NonDelegatingAddRef()
{
    LONG lRef = InterlockedIncrement(&m_cRef);
    ASSERT(lRef > 0);
    return max(ULONG(m_cRef),1ul);
}


// Release a reference count and protect against reentrancy

STDMETHODIMP_(ULONG) CBasePropertyPage::NonDelegatingRelease()
{
    // If the reference count drops to zero delete ourselves

    LONG lRef = InterlockedDecrement(&m_cRef);
    if (lRef == 0) {
        m_cRef++;
        SetPageSite(NULL);
        SetObjects(0,NULL);
        delete this;
        return ULONG(0);
    } else {
        //  Don't touch m_cRef again here!
        return max(ULONG(lRef),1ul);
    }
}


// Expose our IPropertyPage interface

STDMETHODIMP
CBasePropertyPage::NonDelegatingQueryInterface(REFIID riid,__deref_out void **ppv)
{
    if (riid == IID_IPropertyPage) {
        return GetInterface((IPropertyPage *)this,ppv);
    } else {
        return CUnknown::NonDelegatingQueryInterface(riid,ppv);
    }
}


// Get the page info so that the page site can size itself

STDMETHODIMP CBasePropertyPage::GetPageInfo(__out LPPROPPAGEINFO pPageInfo)
{
    CheckPointer(pPageInfo,E_POINTER);
    WCHAR wszTitle[STR_MAX_LENGTH];
    WideStringFromResource(wszTitle,m_TitleId);

    // Allocate dynamic memory for the property page title

    LPOLESTR pszTitle;
    HRESULT hr = AMGetWideString(wszTitle, &pszTitle);
    if (FAILED(hr)) {
        NOTE("No caption memory");
        return hr;
    }

    pPageInfo->cb               = sizeof(PROPPAGEINFO);
    pPageInfo->pszTitle         = pszTitle;
    pPageInfo->pszDocString     = NULL;
    pPageInfo->pszHelpFile      = NULL;
    pPageInfo->dwHelpContext    = 0;

    // Set defaults in case GetDialogSize fails
    pPageInfo->size.cx          = 340;
    pPageInfo->size.cy          = 150;

    GetDialogSize(m_DialogId, DialogProc,0L,&pPageInfo->size);
    return NOERROR;
}


// Handles the messages for our property window

INT_PTR CALLBACK CBasePropertyPage::DialogProc(HWND hwnd,
                                            UINT uMsg,
                                            WPARAM wParam,
                                            LPARAM lParam)
{
    CBasePropertyPage *pPropertyPage;

    switch (uMsg) {

        case WM_INITDIALOG:

            _SetWindowLongPtr(hwnd, DWLP_USER, lParam);

            // This pointer may be NULL when calculating size

            pPropertyPage = (CBasePropertyPage *) lParam;
            if (pPropertyPage == NULL) {
                return (LRESULT) 1;
            }
            pPropertyPage->m_Dlg = hwnd;
    }

    // This pointer may be NULL when calculating size

    pPropertyPage = _GetWindowLongPtr<CBasePropertyPage*>(hwnd, DWLP_USER);
    if (pPropertyPage == NULL) {
        return (LRESULT) 1;
    }
    return pPropertyPage->OnReceiveMessage(hwnd,uMsg,wParam,lParam);
}


// Tells us the object that should be informed of the property changes

STDMETHODIMP CBasePropertyPage::SetObjects(ULONG cObjects,__in_ecount_opt(cObjects) LPUNKNOWN *ppUnk)
{
    if (cObjects == 1) {

        if ((ppUnk == NULL) || (*ppUnk == NULL)) {
            return E_POINTER;
        }

        // Set a flag to say that we have set the Object
        m_bObjectSet = TRUE ;
        return OnConnect(*ppUnk);

    } else if (cObjects == 0) {

        // Set a flag to say that we have not set the Object for the page
        m_bObjectSet = FALSE ;
        return OnDisconnect();
    }

    DbgBreak("No support for more than one object");
    return E_UNEXPECTED;
}


// Create the window we will use to edit properties

STDMETHODIMP CBasePropertyPage::Activate(HWND hwndParent,
                                         LPCRECT pRect,
                                         BOOL fModal)
{
    CheckPointer(pRect,E_POINTER);

    // Return failure if SetObject has not been called.
    if (m_bObjectSet == FALSE) {
        return E_UNEXPECTED;
    }

    if (m_hwnd) {
        return E_UNEXPECTED;
    }

    m_hwnd = CreateDialogParam(g_hInst,
                               MAKEINTRESOURCE(m_DialogId),
                               hwndParent,
                               DialogProc,
                               (LPARAM) this);
    if (m_hwnd == NULL) {
        return E_OUTOFMEMORY;
    }

    OnActivate();
    Move(pRect);
    return Show(SW_SHOWNORMAL);
}


// Set the position of the property page

STDMETHODIMP CBasePropertyPage::Move(LPCRECT pRect)
{
    CheckPointer(pRect,E_POINTER);

    if (m_hwnd == NULL) {
        return E_UNEXPECTED;
    }

    MoveWindow(m_hwnd,              // Property page handle
               pRect->left,         // x coordinate
               pRect->top,          // y coordinate
               WIDTH(pRect),        // Overall window width
               HEIGHT(pRect),       // And likewise height
               TRUE);               // Should we repaint it

    return NOERROR;
}


// Display the property dialog

STDMETHODIMP CBasePropertyPage::Show(UINT nCmdShow)
{
   // Have we been activated yet

    if (m_hwnd == NULL) {
        return E_UNEXPECTED;
    }

    // Ignore wrong show flags

    if ((nCmdShow != SW_SHOW) && (nCmdShow != SW_SHOWNORMAL) && (nCmdShow != SW_HIDE)) {
        return E_INVALIDARG;
    }

    ShowWindow(m_hwnd,nCmdShow);
    InvalidateRect(m_hwnd,NULL,TRUE);
    return NOERROR;
}


// Destroy the property page dialog

STDMETHODIMP CBasePropertyPage::Deactivate(void)
{
    if (m_hwnd == NULL) {
        return E_UNEXPECTED;
    }

    // Remove WS_EX_CONTROLPARENT before DestroyWindow call

    DWORD dwStyle = GetWindowLong(m_hwnd, GWL_EXSTYLE);
    dwStyle = dwStyle & (~WS_EX_CONTROLPARENT);

    //  Set m_hwnd to be NULL temporarily so the message handler
    //  for WM_STYLECHANGING doesn't add the WS_EX_CONTROLPARENT
    //  style back in
    HWND hwnd = m_hwnd;
    m_hwnd = NULL;
    SetWindowLong(hwnd, GWL_EXSTYLE, dwStyle);
    m_hwnd = hwnd;

    OnDeactivate();

    // Destroy the dialog window

    DestroyWindow(m_hwnd);
    m_hwnd = NULL;
    return NOERROR;
}


// Tells the application property page site

STDMETHODIMP CBasePropertyPage::SetPageSite(__in_opt LPPROPERTYPAGESITE pPageSite)
{
    if (pPageSite) {

        if (m_pPageSite) {
            return E_UNEXPECTED;
        }

        m_pPageSite = pPageSite;
        m_pPageSite->AddRef();

    } else {

        if (m_pPageSite == NULL) {
            return E_UNEXPECTED;
        }

        m_pPageSite->Release();
        m_pPageSite = NULL;
    }
    return NOERROR;
}


// Apply any changes so far made

STDMETHODIMP CBasePropertyPage::Apply()
{
    // In ActiveMovie 1.0 we used to check whether we had been activated or
    // not. This is too constrictive. Apply should be allowed as long as
    // SetObject was called to set an object. So we will no longer check to
    // see if we have been activated (ie., m_hWnd != NULL), but instead
    // make sure that m_bObjectSet is TRUE (ie., SetObject has been called).

    if (m_bObjectSet == FALSE) {
        return E_UNEXPECTED;
    }

    // Must have had a site set

    if (m_pPageSite == NULL) {
        return E_UNEXPECTED;
    }

    // Has anything changed

    if (m_bDirty == FALSE) {
        return NOERROR;
    }

    // Commit derived class changes

    HRESULT hr = OnApplyChanges();
    if (SUCCEEDED(hr)) {
        m_bDirty = FALSE;
    }
    return hr;
}


// Base class definition for message handling

INT_PTR CBasePropertyPage::OnReceiveMessage(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
    // we would like the TAB key to move around the tab stops in our property
    // page, but for some reason OleCreatePropertyFrame clears the CONTROLPARENT
    // style behind our back, so we need to switch it back on now behind its
    // back.  Otherwise the tab key will be useless in every page.
    //

    CBasePropertyPage *pPropertyPage;
    {
        pPropertyPage = _GetWindowLongPtr<CBasePropertyPage*>(hwnd, DWLP_USER);

        if (pPropertyPage->m_hwnd == NULL) {
            return 0;
        }
        switch (uMsg) {
          case WM_STYLECHANGING:
              if (wParam == GWL_EXSTYLE) {
                  LPSTYLESTRUCT lpss = (LPSTYLESTRUCT)lParam;
                  lpss->styleNew |= WS_EX_CONTROLPARENT;
                  return 0;
              }
        }
    }
		
    return DefWindowProc(hwnd,uMsg,wParam,lParam);
}

//------------------------------------------------------------------------------
// File: DDMM.h
//
// Desc: DirectShow base classes - efines routines for using DirectDraw 
//       on a multimonitor system.
//
// Copyright (c) 1995-2001 Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif  /* __cplusplus */

// DDRAW.H might not include these
#ifndef DDENUM_ATTACHEDSECONDARYDEVICES
#define DDENUM_ATTACHEDSECONDARYDEVICES     0x00000001L
#endif

typedef HRESULT (*PDRAWCREATE)(IID *,LPDIRECTDRAW *,LPUNKNOWN);
typedef HRESULT (*PDRAWENUM)(LPDDENUMCALLBACKA, LPVOID);

IDirectDraw * DirectDrawCreateFromDevice(__in_opt LPSTR, PDRAWCREATE, PDRAWENUM);
IDirectDraw * DirectDrawCreateFromDeviceEx(__in_opt LPSTR, PDRAWCREATE, LPDIRECTDRAWENUMERATEEXA);

#ifdef __cplusplus
}
#endif	/* __cplusplus */

//------------------------------------------------------------------------------
// File: DDMM.cpp
//
// Desc: DirectShow base classes - implements routines for using DirectDraw
//       on a multimonitor system.
//
// Copyright (c) 1995-2001 Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------

/*
 * FindDeviceCallback
 */
typedef struct {
	LPSTR   szDevice;
	GUID*   lpGUID;
	GUID    GUID;
	BOOL    fFound;
}   FindDeviceData;

BOOL CALLBACK FindDeviceCallback(__in_opt GUID* lpGUID, __in LPSTR szName, __in LPSTR szDevice, __in LPVOID lParam)
{
	FindDeviceData *p = (FindDeviceData*)lParam;

	if (lstrcmpiA(p->szDevice, szDevice) == 0) {
	    if (lpGUID) {
		p->GUID = *lpGUID;
		p->lpGUID = &p->GUID;
	    } else {
		p->lpGUID = NULL;
	    }
	    p->fFound = TRUE;
	    return FALSE;
	}
	return TRUE;
}


BOOL CALLBACK FindDeviceCallbackEx(__in_opt GUID* lpGUID, __in LPSTR szName, __in LPSTR szDevice, __in LPVOID lParam, HMONITOR hMonitor)
{
	FindDeviceData *p = (FindDeviceData*)lParam;

	if (lstrcmpiA(p->szDevice, szDevice) == 0) {
	    if (lpGUID) {
		p->GUID = *lpGUID;
		p->lpGUID = &p->GUID;
	    } else {
		p->lpGUID = NULL;
	    }
	    p->fFound = TRUE;
	    return FALSE;
	}
	return TRUE;
}


/*
 * DirectDrawCreateFromDevice
 *
 * create a DirectDraw object for a particular device
 */
IDirectDraw * DirectDrawCreateFromDevice(__in_opt LPSTR szDevice, PDRAWCREATE DirectDrawCreateP, PDRAWENUM DirectDrawEnumerateP)
{
	IDirectDraw*    pdd = NULL;
	FindDeviceData  find;

	if (szDevice == NULL) {
		DirectDrawCreateP(NULL, &pdd, NULL);
		return pdd;
	}

	find.szDevice = szDevice;
	find.fFound   = FALSE;
	DirectDrawEnumerateP(FindDeviceCallback, (LPVOID)&find);

	if (find.fFound)
	{
		//
		// In 4bpp mode the following DDraw call causes a message box to be popped
		// up by DDraw (!?!).  It's DDraw's fault, but we don't like it.  So we
		// make sure it doesn't happen.
		//
		UINT ErrorMode = SetErrorMode(SEM_FAILCRITICALERRORS);
		DirectDrawCreateP(find.lpGUID, &pdd, NULL);
		SetErrorMode(ErrorMode);
	}

	return pdd;
}


/*
 * DirectDrawCreateFromDeviceEx
 *
 * create a DirectDraw object for a particular device
 */
IDirectDraw * DirectDrawCreateFromDeviceEx(__in_opt LPSTR szDevice, PDRAWCREATE DirectDrawCreateP, LPDIRECTDRAWENUMERATEEXA DirectDrawEnumerateExP)
{
	IDirectDraw*    pdd = NULL;
	FindDeviceData  find;

	if (szDevice == NULL) {
		DirectDrawCreateP(NULL, &pdd, NULL);
		return pdd;
	}

	find.szDevice = szDevice;
	find.fFound   = FALSE;
	DirectDrawEnumerateExP(FindDeviceCallbackEx, (LPVOID)&find,
					DDENUM_ATTACHEDSECONDARYDEVICES);

	if (find.fFound)
	{
		//
		// In 4bpp mode the following DDraw call causes a message box to be popped
		// up by DDraw (!?!).  It's DDraw's fault, but we don't like it.  So we
		// make sure it doesn't happen.
		//
		UINT ErrorMode = SetErrorMode(SEM_FAILCRITICALERRORS);
		DirectDrawCreateP(find.lpGUID, &pdd, NULL);
		SetErrorMode(ErrorMode);
	}

	return pdd;
}

//------------------------------------------------------------------------------
// File: VideoCtl.cpp
//
// Desc: DirectShow base classes.
//
// Copyright (c) 1992-2001 Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------

// Load a string from the resource file string table. The buffer must be at
// least STR_MAX_LENGTH bytes. The easiest way to use this is to declare a
// buffer in the property page class and use it for all string loading. It
// cannot be static as multiple property pages may be active simultaneously

LPTSTR WINAPI StringFromResource(__out_ecount(STR_MAX_LENGTH) LPTSTR pBuffer, int iResourceID)
{
    if (LoadString(g_hInst,iResourceID,pBuffer,STR_MAX_LENGTH) == 0) {
        return TEXT("");
    }
    return pBuffer;
}

#ifdef UNICODE
LPSTR WINAPI StringFromResource(__out_ecount(STR_MAX_LENGTH) LPSTR pBuffer, int iResourceID)
{
    if (LoadStringA(g_hInst,iResourceID,pBuffer,STR_MAX_LENGTH) == 0) {
        return "";
    }
    return pBuffer;
}
#endif



// Property pages typically are called through their OLE interfaces. These
// use UNICODE strings regardless of how the binary is built. So when we
// load strings from the resource file we sometimes want to convert them
// to UNICODE. This method is passed the target UNICODE buffer and does a
// convert after loading the string (if built UNICODE this is not needed)
// On WinNT we can explicitly call LoadStringW which saves two conversions

#ifndef UNICODE

LPWSTR WINAPI WideStringFromResource(__out_ecount(STR_MAX_LENGTH) LPWSTR pBuffer, int iResourceID)
{
    *pBuffer = 0;

    if (g_amPlatform == VER_PLATFORM_WIN32_NT) {
	LoadStringW(g_hInst,iResourceID,pBuffer,STR_MAX_LENGTH);
    } else {

	CHAR szBuffer[STR_MAX_LENGTH];
	DWORD dwStringLength = LoadString(g_hInst,iResourceID,szBuffer,STR_MAX_LENGTH);
	// if we loaded a string convert it to wide characters, ensuring
	// that we also null terminate the result.
	if (dwStringLength++) {
	    MultiByteToWideChar(CP_ACP,0,szBuffer,dwStringLength,pBuffer,STR_MAX_LENGTH);
	}
    }
    return pBuffer;
}

#endif


// Helper function to calculate the size of the dialog

BOOL WINAPI GetDialogSize(int iResourceID,
                          DLGPROC pDlgProc,
                          LPARAM lParam,
                          __out SIZE *pResult)
{
    RECT rc;
    HWND hwnd;

    // Create a temporary property page

    hwnd = CreateDialogParam(g_hInst,
                             MAKEINTRESOURCE(iResourceID),
                             GetDesktopWindow(),
                             pDlgProc,
                             lParam);
    if (hwnd == NULL) {
        return FALSE;
    }

    GetWindowRect(hwnd, &rc);
    pResult->cx = rc.right - rc.left;
    pResult->cy = rc.bottom - rc.top;

    DestroyWindow(hwnd);
    return TRUE;
}


// Class that aggregates on the IDirectDraw interface. Although DirectDraw
// has the ability in its interfaces to be aggregated they're not currently
// implemented. This makes it difficult for various parts of Quartz that want
// to aggregate these interfaces. In particular the video renderer passes out
// media samples that expose IDirectDraw and IDirectDrawSurface. The filter
// graph manager also exposes IDirectDraw as a plug in distributor. For these
// objects we provide these aggregation classes that republish the interfaces

STDMETHODIMP CAggDirectDraw::NonDelegatingQueryInterface(REFIID riid, __deref_out void **ppv)
{
    ASSERT(m_pDirectDraw);

    // Do we have this interface

    if (riid == IID_IDirectDraw) {
        return GetInterface((IDirectDraw *)this,ppv);
    } else {
        return CUnknown::NonDelegatingQueryInterface(riid,ppv);
    }
}


STDMETHODIMP CAggDirectDraw::Compact()
{
    ASSERT(m_pDirectDraw);
    return m_pDirectDraw->Compact();
}


STDMETHODIMP CAggDirectDraw::CreateClipper(DWORD dwFlags, __deref_out LPDIRECTDRAWCLIPPER *lplpDDClipper, __inout_opt IUnknown *pUnkOuter)
{
    ASSERT(m_pDirectDraw);
    return m_pDirectDraw->CreateClipper(dwFlags,lplpDDClipper,pUnkOuter);
}


STDMETHODIMP CAggDirectDraw::CreatePalette(DWORD dwFlags,
                                           __in LPPALETTEENTRY lpColorTable,
                                           __deref_out LPDIRECTDRAWPALETTE *lplpDDPalette,
                                           __inout_opt IUnknown *pUnkOuter)
{
    ASSERT(m_pDirectDraw);
    return m_pDirectDraw->CreatePalette(dwFlags,lpColorTable,lplpDDPalette,pUnkOuter);
}


STDMETHODIMP CAggDirectDraw::CreateSurface(__in LPDDSURFACEDESC lpDDSurfaceDesc,
                                           __deref_out LPDIRECTDRAWSURFACE *lplpDDSurface,
                                           __inout_opt IUnknown *pUnkOuter)
{
    ASSERT(m_pDirectDraw);
    return m_pDirectDraw->CreateSurface(lpDDSurfaceDesc,lplpDDSurface,pUnkOuter);
}


STDMETHODIMP CAggDirectDraw::DuplicateSurface(__in LPDIRECTDRAWSURFACE lpDDSurface,
                                              __deref_out LPDIRECTDRAWSURFACE *lplpDupDDSurface)
{
    ASSERT(m_pDirectDraw);
    return m_pDirectDraw->DuplicateSurface(lpDDSurface,lplpDupDDSurface);
}


STDMETHODIMP CAggDirectDraw::EnumDisplayModes(DWORD dwSurfaceDescCount,
                                              __in LPDDSURFACEDESC lplpDDSurfaceDescList,
                                              __in LPVOID lpContext,
                                              __in LPDDENUMMODESCALLBACK lpEnumCallback)
{
    ASSERT(m_pDirectDraw);
    return m_pDirectDraw->EnumDisplayModes(dwSurfaceDescCount,lplpDDSurfaceDescList,lpContext,lpEnumCallback);
}


STDMETHODIMP CAggDirectDraw::EnumSurfaces(DWORD dwFlags,
                                          __in LPDDSURFACEDESC lpDDSD,
                                          __in LPVOID lpContext,
                                          __in LPDDENUMSURFACESCALLBACK lpEnumCallback)
{
    ASSERT(m_pDirectDraw);
    return m_pDirectDraw->EnumSurfaces(dwFlags,lpDDSD,lpContext,lpEnumCallback);
}


STDMETHODIMP CAggDirectDraw::FlipToGDISurface()
{
    ASSERT(m_pDirectDraw);
    return m_pDirectDraw->FlipToGDISurface();
}


STDMETHODIMP CAggDirectDraw::GetCaps(__out LPDDCAPS lpDDDriverCaps,__out LPDDCAPS lpDDHELCaps)
{
    ASSERT(m_pDirectDraw);
    return m_pDirectDraw->GetCaps(lpDDDriverCaps,lpDDHELCaps);
}


STDMETHODIMP CAggDirectDraw::GetDisplayMode(__out LPDDSURFACEDESC lpDDSurfaceDesc)
{
    ASSERT(m_pDirectDraw);
    return m_pDirectDraw->GetDisplayMode(lpDDSurfaceDesc);
}


STDMETHODIMP CAggDirectDraw::GetFourCCCodes(__inout LPDWORD lpNumCodes,__out_ecount(*lpNumCodes) LPDWORD lpCodes)
{
    ASSERT(m_pDirectDraw);
    return m_pDirectDraw->GetFourCCCodes(lpNumCodes,lpCodes);
}


STDMETHODIMP CAggDirectDraw::GetGDISurface(__deref_out LPDIRECTDRAWSURFACE *lplpGDIDDSurface)
{
    ASSERT(m_pDirectDraw);
    return m_pDirectDraw->GetGDISurface(lplpGDIDDSurface);
}


STDMETHODIMP CAggDirectDraw::GetMonitorFrequency(__out LPDWORD lpdwFrequency)
{
    ASSERT(m_pDirectDraw);
    return m_pDirectDraw->GetMonitorFrequency(lpdwFrequency);
}


STDMETHODIMP CAggDirectDraw::GetScanLine(__out LPDWORD lpdwScanLine)
{
    ASSERT(m_pDirectDraw);
    return m_pDirectDraw->GetScanLine(lpdwScanLine);
}


STDMETHODIMP CAggDirectDraw::GetVerticalBlankStatus(__out LPBOOL lpblsInVB)
{
    ASSERT(m_pDirectDraw);
    return m_pDirectDraw->GetVerticalBlankStatus(lpblsInVB);
}


STDMETHODIMP CAggDirectDraw::Initialize(__in GUID *lpGUID)
{
    ASSERT(m_pDirectDraw);
    return m_pDirectDraw->Initialize(lpGUID);
}


STDMETHODIMP CAggDirectDraw::RestoreDisplayMode()
{
    ASSERT(m_pDirectDraw);
    return m_pDirectDraw->RestoreDisplayMode();
}


STDMETHODIMP CAggDirectDraw::SetCooperativeLevel(HWND hWnd,DWORD dwFlags)
{
    ASSERT(m_pDirectDraw);
    return m_pDirectDraw->SetCooperativeLevel(hWnd,dwFlags);
}


STDMETHODIMP CAggDirectDraw::SetDisplayMode(DWORD dwWidth,DWORD dwHeight,DWORD dwBpp)
{
    ASSERT(m_pDirectDraw);
    return m_pDirectDraw->SetDisplayMode(dwWidth,dwHeight,dwBpp);
}


STDMETHODIMP CAggDirectDraw::WaitForVerticalBlank(DWORD dwFlags,HANDLE hEvent)
{
    ASSERT(m_pDirectDraw);
    return m_pDirectDraw->WaitForVerticalBlank(dwFlags,hEvent);
}


// Class that aggregates an IDirectDrawSurface interface. Although DirectDraw
// has the ability in its interfaces to be aggregated they're not currently
// implemented. This makes it difficult for various parts of Quartz that want
// to aggregate these interfaces. In particular the video renderer passes out
// media samples that expose IDirectDraw and IDirectDrawSurface. The filter
// graph manager also exposes IDirectDraw as a plug in distributor. For these
// objects we provide these aggregation classes that republish the interfaces

STDMETHODIMP CAggDrawSurface::NonDelegatingQueryInterface(REFIID riid, __deref_out void **ppv)
{
    ASSERT(m_pDirectDrawSurface);

    // Do we have this interface

    if (riid == IID_IDirectDrawSurface) {
        return GetInterface((IDirectDrawSurface *)this,ppv);
    } else {
        return CUnknown::NonDelegatingQueryInterface(riid,ppv);
    }
}


STDMETHODIMP CAggDrawSurface::AddAttachedSurface(__in LPDIRECTDRAWSURFACE lpDDSAttachedSurface)
{
    ASSERT(m_pDirectDrawSurface);
    return m_pDirectDrawSurface->AddAttachedSurface(lpDDSAttachedSurface);
}


STDMETHODIMP CAggDrawSurface::AddOverlayDirtyRect(__in LPRECT lpRect)
{
    ASSERT(m_pDirectDrawSurface);
    return m_pDirectDrawSurface->AddOverlayDirtyRect(lpRect);
}


STDMETHODIMP CAggDrawSurface::Blt(__in LPRECT lpDestRect,
                                  __in LPDIRECTDRAWSURFACE lpDDSrcSurface,
                                  __in LPRECT lpSrcRect,
                                  DWORD dwFlags,
                                  __in LPDDBLTFX lpDDBltFx)
{
    ASSERT(m_pDirectDrawSurface);
    return m_pDirectDrawSurface->Blt(lpDestRect,lpDDSrcSurface,lpSrcRect,dwFlags,lpDDBltFx);
}


STDMETHODIMP CAggDrawSurface::BltBatch(__in_ecount(dwCount) LPDDBLTBATCH lpDDBltBatch,DWORD dwCount,DWORD dwFlags)
{
    ASSERT(m_pDirectDrawSurface);
    return m_pDirectDrawSurface->BltBatch(lpDDBltBatch,dwCount,dwFlags);
}


STDMETHODIMP CAggDrawSurface::BltFast(DWORD dwX,DWORD dwY,
                                      __in LPDIRECTDRAWSURFACE lpDDSrcSurface,
                                      __in LPRECT lpSrcRect,
                                      DWORD dwTrans)
{
    ASSERT(m_pDirectDrawSurface);
    return m_pDirectDrawSurface->BltFast(dwX,dwY,lpDDSrcSurface,lpSrcRect,dwTrans);
}


STDMETHODIMP CAggDrawSurface::DeleteAttachedSurface(DWORD dwFlags,
                                                    __in LPDIRECTDRAWSURFACE lpDDSAttachedSurface)
{
    ASSERT(m_pDirectDrawSurface);
    return m_pDirectDrawSurface->DeleteAttachedSurface(dwFlags,lpDDSAttachedSurface);
}


STDMETHODIMP CAggDrawSurface::EnumAttachedSurfaces(__in LPVOID lpContext,
                                                   __in LPDDENUMSURFACESCALLBACK lpEnumSurfacesCallback)
{
    ASSERT(m_pDirectDrawSurface);
    return m_pDirectDrawSurface->EnumAttachedSurfaces(lpContext,lpEnumSurfacesCallback);
}


STDMETHODIMP CAggDrawSurface::EnumOverlayZOrders(DWORD dwFlags,
                                                 __in LPVOID lpContext,
                                                 __in LPDDENUMSURFACESCALLBACK lpfnCallback)
{
    ASSERT(m_pDirectDrawSurface);
    return m_pDirectDrawSurface->EnumOverlayZOrders(dwFlags,lpContext,lpfnCallback);
}


STDMETHODIMP CAggDrawSurface::Flip(__in LPDIRECTDRAWSURFACE lpDDSurfaceTargetOverride,DWORD dwFlags)
{
    ASSERT(m_pDirectDrawSurface);
    return m_pDirectDrawSurface->Flip(lpDDSurfaceTargetOverride,dwFlags);
}


STDMETHODIMP CAggDrawSurface::GetAttachedSurface(__in LPDDSCAPS lpDDSCaps,
                                                 __deref_out LPDIRECTDRAWSURFACE *lplpDDAttachedSurface)
{
    ASSERT(m_pDirectDrawSurface);
    return m_pDirectDrawSurface->GetAttachedSurface(lpDDSCaps,lplpDDAttachedSurface);
}


STDMETHODIMP CAggDrawSurface::GetBltStatus(DWORD dwFlags)
{
    ASSERT(m_pDirectDrawSurface);
    return m_pDirectDrawSurface->GetBltStatus(dwFlags);
}


STDMETHODIMP CAggDrawSurface::GetCaps(__out LPDDSCAPS lpDDSCaps)
{
    ASSERT(m_pDirectDrawSurface);
    return m_pDirectDrawSurface->GetCaps(lpDDSCaps);
}


STDMETHODIMP CAggDrawSurface::GetClipper(__deref_out LPDIRECTDRAWCLIPPER *lplpDDClipper)
{
    ASSERT(m_pDirectDrawSurface);
    return m_pDirectDrawSurface->GetClipper(lplpDDClipper);
}


STDMETHODIMP CAggDrawSurface::GetColorKey(DWORD dwFlags,__out LPDDCOLORKEY lpDDColorKey)
{
    ASSERT(m_pDirectDrawSurface);
    return m_pDirectDrawSurface->GetColorKey(dwFlags,lpDDColorKey);
}


STDMETHODIMP CAggDrawSurface::GetDC(__out HDC *lphDC)
{
    ASSERT(m_pDirectDrawSurface);
    return m_pDirectDrawSurface->GetDC(lphDC);
}


STDMETHODIMP CAggDrawSurface::GetFlipStatus(DWORD dwFlags)
{
    ASSERT(m_pDirectDrawSurface);
    return m_pDirectDrawSurface->GetFlipStatus(dwFlags);
}


STDMETHODIMP CAggDrawSurface::GetOverlayPosition(__out LPLONG lpdwX,__out LPLONG lpdwY)
{
    ASSERT(m_pDirectDrawSurface);
    return m_pDirectDrawSurface->GetOverlayPosition(lpdwX,lpdwY);
}


STDMETHODIMP CAggDrawSurface::GetPalette(__deref_out LPDIRECTDRAWPALETTE *lplpDDPalette)
{
    ASSERT(m_pDirectDrawSurface);
    return m_pDirectDrawSurface->GetPalette(lplpDDPalette);
}


STDMETHODIMP CAggDrawSurface::GetPixelFormat(__out LPDDPIXELFORMAT lpDDPixelFormat)
{
    ASSERT(m_pDirectDrawSurface);
    return m_pDirectDrawSurface->GetPixelFormat(lpDDPixelFormat);
}


// A bit of a warning here: Our media samples in DirectShow aggregate on
// IDirectDraw and IDirectDrawSurface (ie are available through IMediaSample
// by QueryInterface). Unfortunately the underlying DirectDraw code cannot
// be aggregated so we have to use these classes. The snag is that when we
// call a different surface and pass in this interface as perhaps the source
// surface the call will fail because DirectDraw dereferences the pointer to
// get at its private data structures. Therefore we supply this workaround to give
// access to the real IDirectDraw surface. A filter can call GetSurfaceDesc
// and we will fill in the lpSurface pointer with the real underlying surface

STDMETHODIMP CAggDrawSurface::GetSurfaceDesc(__out LPDDSURFACEDESC lpDDSurfaceDesc)
{
    ASSERT(m_pDirectDrawSurface);

    // First call down to the underlying DirectDraw

    HRESULT hr = m_pDirectDrawSurface->GetSurfaceDesc(lpDDSurfaceDesc);
    if (FAILED(hr)) {
        return hr;
    }

    // Store the real DirectDrawSurface interface
    lpDDSurfaceDesc->lpSurface = m_pDirectDrawSurface;
    return hr;
}


STDMETHODIMP CAggDrawSurface::Initialize(__in LPDIRECTDRAW lpDD,__in LPDDSURFACEDESC lpDDSurfaceDesc)
{
    ASSERT(m_pDirectDrawSurface);
    return m_pDirectDrawSurface->Initialize(lpDD,lpDDSurfaceDesc);
}


STDMETHODIMP CAggDrawSurface::IsLost()
{
    ASSERT(m_pDirectDrawSurface);
    return m_pDirectDrawSurface->IsLost();
}


STDMETHODIMP CAggDrawSurface::Lock(__in LPRECT lpDestRect,
                                   __inout LPDDSURFACEDESC lpDDSurfaceDesc,
                                   DWORD dwFlags,
                                   HANDLE hEvent)
{
    ASSERT(m_pDirectDrawSurface);
    return m_pDirectDrawSurface->Lock(lpDestRect,lpDDSurfaceDesc,dwFlags,hEvent);
}


STDMETHODIMP CAggDrawSurface::ReleaseDC(HDC hDC)
{
    ASSERT(m_pDirectDrawSurface);
    return m_pDirectDrawSurface->ReleaseDC(hDC);
}


STDMETHODIMP CAggDrawSurface::Restore()
{
    ASSERT(m_pDirectDrawSurface);
    return m_pDirectDrawSurface->Restore();
}


STDMETHODIMP CAggDrawSurface::SetClipper(__in LPDIRECTDRAWCLIPPER lpDDClipper)
{
    ASSERT(m_pDirectDrawSurface);
    return m_pDirectDrawSurface->SetClipper(lpDDClipper);
}


STDMETHODIMP CAggDrawSurface::SetColorKey(DWORD dwFlags,__in LPDDCOLORKEY lpDDColorKey)
{
    ASSERT(m_pDirectDrawSurface);
    return m_pDirectDrawSurface->SetColorKey(dwFlags,lpDDColorKey);
}


STDMETHODIMP CAggDrawSurface::SetOverlayPosition(LONG dwX,LONG dwY)
{
    ASSERT(m_pDirectDrawSurface);
    return m_pDirectDrawSurface->SetOverlayPosition(dwX,dwY);
}


STDMETHODIMP CAggDrawSurface::SetPalette(__in LPDIRECTDRAWPALETTE lpDDPalette)
{
    ASSERT(m_pDirectDrawSurface);
    return m_pDirectDrawSurface->SetPalette(lpDDPalette);
}


STDMETHODIMP CAggDrawSurface::Unlock(__in LPVOID lpSurfaceData)
{
    ASSERT(m_pDirectDrawSurface);
    return m_pDirectDrawSurface->Unlock(lpSurfaceData);
}


STDMETHODIMP CAggDrawSurface::UpdateOverlay(__in LPRECT lpSrcRect,
                                            __in LPDIRECTDRAWSURFACE lpDDDestSurface,
                                            __in LPRECT lpDestRect,
                                            DWORD dwFlags,
                                            __in LPDDOVERLAYFX lpDDOverlayFX)
{
    ASSERT(m_pDirectDrawSurface);
    return m_pDirectDrawSurface->UpdateOverlay(lpSrcRect,lpDDDestSurface,lpDestRect,dwFlags,lpDDOverlayFX);
}


STDMETHODIMP CAggDrawSurface::UpdateOverlayDisplay(DWORD dwFlags)
{
    ASSERT(m_pDirectDrawSurface);
    return m_pDirectDrawSurface->UpdateOverlayDisplay(dwFlags);
}


STDMETHODIMP CAggDrawSurface::UpdateOverlayZOrder(DWORD dwFlags,__in LPDIRECTDRAWSURFACE lpDDSReference)
{
    ASSERT(m_pDirectDrawSurface);
    return m_pDirectDrawSurface->UpdateOverlayZOrder(dwFlags,lpDDSReference);
}


// DirectShow must work on multiple platforms.  In particular, it also runs on
// Windows NT 3.51 which does not have DirectDraw capabilities. The filters
// cannot therefore link statically to the DirectDraw library. To make their
// lives that little bit easier we provide this class that manages loading
// and unloading the library and creating the initial IDirectDraw interface

CLoadDirectDraw::CLoadDirectDraw() :
    m_pDirectDraw(NULL),
    m_hDirectDraw(NULL)
{
}


// Destructor forces unload

CLoadDirectDraw::~CLoadDirectDraw()
{
    ReleaseDirectDraw();

    if (m_hDirectDraw) {
        NOTE("Unloading library");
        FreeLibrary(m_hDirectDraw);
    }
}


// We can't be sure that DirectDraw is always available so we can't statically
// link to the library. Therefore we load the library, get the function entry
// point addresses and call them to create the driver objects. We return S_OK
// if we manage to load DirectDraw correctly otherwise we return E_NOINTERFACE
// We initialise a DirectDraw instance by explicitely loading the library and
// calling GetProcAddress on the DirectDrawCreate entry point that it exports

// On a multi monitor system, we can get the DirectDraw object for any
// monitor (device) with the optional szDevice parameter

HRESULT CLoadDirectDraw::LoadDirectDraw(__in LPSTR szDevice)
{
    PDRAWCREATE pDrawCreate;
    PDRAWENUM pDrawEnum;
    LPDIRECTDRAWENUMERATEEXA pDrawEnumEx;
    HRESULT hr = NOERROR;

    NOTE("Entering DoLoadDirectDraw");

    // Is DirectDraw already loaded

    if (m_pDirectDraw) {
        NOTE("Already loaded");
        ASSERT(m_hDirectDraw);
        return NOERROR;
    }

    // Make sure the library is available

    if(!m_hDirectDraw)
    {
        UINT ErrorMode = SetErrorMode(SEM_NOOPENFILEERRORBOX);
        m_hDirectDraw = LoadLibrary(TEXT("DDRAW.DLL"));
        SetErrorMode(ErrorMode);

        if (m_hDirectDraw == NULL) {
            DbgLog((LOG_ERROR,1,TEXT("Can't load DDRAW.DLL")));
            NOTE("No library");
            return E_NOINTERFACE;
        }
    }

    // Get the DLL address for the creator function

    pDrawCreate = (PDRAWCREATE)GetProcAddress(m_hDirectDraw,"DirectDrawCreate");
    // force ANSI, we assume it
    pDrawEnum = (PDRAWENUM)GetProcAddress(m_hDirectDraw,"DirectDrawEnumerateA");
    pDrawEnumEx = (LPDIRECTDRAWENUMERATEEXA)GetProcAddress(m_hDirectDraw,
						"DirectDrawEnumerateExA");

    // We don't NEED DirectDrawEnumerateEx, that's just for multimon stuff
    if (pDrawCreate == NULL || pDrawEnum == NULL) {
        DbgLog((LOG_ERROR,1,TEXT("Can't get functions: Create=%x Enum=%x"),
			pDrawCreate, pDrawEnum));
        NOTE("No entry point");
        ReleaseDirectDraw();
        return E_NOINTERFACE;
    }

    DbgLog((LOG_TRACE,3,TEXT("Creating DDraw for device %s"),
					szDevice ? szDevice : "<NULL>"));

    // Create a DirectDraw display provider for this device, using the fancy
    // multimon-aware version, if it exists
    if (pDrawEnumEx)
        m_pDirectDraw = DirectDrawCreateFromDeviceEx(szDevice, pDrawCreate,
								pDrawEnumEx);
    else
        m_pDirectDraw = DirectDrawCreateFromDevice(szDevice, pDrawCreate,
								pDrawEnum);

    if (m_pDirectDraw == NULL) {
            DbgLog((LOG_ERROR,1,TEXT("Can't create DDraw")));
            NOTE("No instance");
            ReleaseDirectDraw();
            return E_NOINTERFACE;
    }
    return NOERROR;
}


// Called to release any DirectDraw provider we previously loaded. We may be
// called at any time especially when something goes horribly wrong and when
// we need to clean up before returning so we can't guarantee that all state
// variables are consistent so free only those really allocated allocated
// This should only be called once all reference counts have been released

void CLoadDirectDraw::ReleaseDirectDraw()
{
    NOTE("Releasing DirectDraw driver");

    // Release any DirectDraw provider interface

    if (m_pDirectDraw) {
        NOTE("Releasing instance");
        m_pDirectDraw->Release();
        m_pDirectDraw = NULL;
    }

}


// Return NOERROR (S_OK) if DirectDraw has been loaded by this object

HRESULT CLoadDirectDraw::IsDirectDrawLoaded()
{
    NOTE("Entering IsDirectDrawLoaded");

    if (m_pDirectDraw == NULL) {
        NOTE("DirectDraw not loaded");
        return S_FALSE;
    }
    return NOERROR;
}


// Return the IDirectDraw interface we look after

LPDIRECTDRAW CLoadDirectDraw::GetDirectDraw()
{
    NOTE("Entering GetDirectDraw");

    if (m_pDirectDraw == NULL) {
        NOTE("No DirectDraw");
        return NULL;
    }

    NOTE("Returning DirectDraw");
    m_pDirectDraw->AddRef();
    return m_pDirectDraw;
}


// Are we running on Direct Draw version 1?  We need to find out as
// we rely on specific bug fixes in DirectDraw 2 for fullscreen playback. To
// find out, we simply see if it supports IDirectDraw2.  Only version 2 and
// higher support this.

BOOL CLoadDirectDraw::IsDirectDrawVersion1()
{

    if (m_pDirectDraw == NULL)
	return FALSE;

    IDirectDraw2 *p = NULL;
    HRESULT hr = m_pDirectDraw->QueryInterface(IID_IDirectDraw2, (void **)&p);
    if (p)
	p->Release();
    if (hr == NOERROR) {
        DbgLog((LOG_TRACE,3,TEXT("Direct Draw Version 2 or greater")));
	return FALSE;
    } else {
        DbgLog((LOG_TRACE,3,TEXT("Direct Draw Version 1")));
	return TRUE;
    }
}

//------------------------------------------------------------------------------
// File: DllSetup.cpp
//
// Desc: DirectShow base classes.
//
// Copyright (c) 1992-2001 Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
// defines

#define MAX_KEY_LEN  260


//---------------------------------------------------------------------------
// externally defined functions/variable

extern int g_cTemplates;
extern CFactoryTemplate g_Templates[];

//---------------------------------------------------------------------------
//
// EliminateSubKey
//
// Try to enumerate all keys under this one.
// if we find anything, delete it completely.
// Otherwise just delete it.
//
// note - this was pinched/duplicated from
// Filgraph\Mapper.cpp - so should it be in
// a lib somewhere?
//
//---------------------------------------------------------------------------

STDAPI
EliminateSubKey( HKEY hkey, LPCTSTR strSubKey )
{
  HKEY hk;
  if (0 == lstrlen(strSubKey) ) {
      // defensive approach
      return E_FAIL;
  }

  LONG lreturn = RegOpenKeyEx( hkey
                             , strSubKey
                             , 0
                             , MAXIMUM_ALLOWED
                             , &hk );

  ASSERT(    lreturn == ERROR_SUCCESS
          || lreturn == ERROR_FILE_NOT_FOUND
          || lreturn == ERROR_INVALID_HANDLE );

  if( ERROR_SUCCESS == lreturn )
  {
    // Keep on enumerating the first (zero-th)
    // key and deleting that

    for( ; ; )
    {
      TCHAR Buffer[MAX_KEY_LEN];
      DWORD dw = MAX_KEY_LEN;
      FILETIME ft;

      lreturn = RegEnumKeyEx( hk
                            , 0
                            , Buffer
                            , &dw
                            , NULL
                            , NULL
                            , NULL
                            , &ft);

      ASSERT(    lreturn == ERROR_SUCCESS
              || lreturn == ERROR_NO_MORE_ITEMS );

      if( ERROR_SUCCESS == lreturn )
      {
        EliminateSubKey(hk, Buffer);
      }
      else
      {
        break;
      }
    }

    RegCloseKey(hk);
    RegDeleteKey(hkey, strSubKey);
  }

  return NOERROR;
}


//---------------------------------------------------------------------------
//
// AMovieSetupRegisterServer()
//
// registers specfied file "szFileName" as server for
// CLSID "clsServer".  A description is also required.
// The ThreadingModel and ServerType are optional, as
// they default to InprocServer32 (i.e. dll) and Both.
//
//---------------------------------------------------------------------------

STDAPI
AMovieSetupRegisterServer( CLSID   clsServer
                         , LPCWSTR szDescription
                         , LPCWSTR szFileName
                         , LPCWSTR szThreadingModel = L"Both"
                         , LPCWSTR szServerType     = L"InprocServer32" )
{
  // temp buffer
  //
  TCHAR achTemp[MAX_PATH];

  // convert CLSID uuid to string and write
  // out subkey as string - CLSID\{}
  //
  OLECHAR szCLSID[CHARS_IN_GUID];
  HRESULT hr = StringFromGUID2( clsServer
                              , szCLSID
                              , CHARS_IN_GUID );
  ASSERT( SUCCEEDED(hr) );

  // create key
  //
  HKEY hkey;
  (void)StringCchPrintf( achTemp, NUMELMS(achTemp), TEXT("CLSID\\%ls"), szCLSID );
  LONG lreturn = RegCreateKey( HKEY_CLASSES_ROOT
                             , (LPCTSTR)achTemp
                             , &hkey              );
  if( ERROR_SUCCESS != lreturn )
  {
    return AmHresultFromWin32(lreturn);
  }

  // set description string
  //

  (void)StringCchPrintf( achTemp, NUMELMS(achTemp), TEXT("%ls"), szDescription );
  lreturn = RegSetValue( hkey
                       , (LPCTSTR)NULL
                       , REG_SZ
                       , achTemp
                       , sizeof(achTemp) );
  if( ERROR_SUCCESS != lreturn )
  {
    RegCloseKey( hkey );
    return AmHresultFromWin32(lreturn);
  }

  // create CLSID\\{"CLSID"}\\"ServerType" key,
  // using key to CLSID\\{"CLSID"} passed back by
  // last call to RegCreateKey().
  //
  HKEY hsubkey;

  (void)StringCchPrintf( achTemp, NUMELMS(achTemp), TEXT("%ls"), szServerType );
  lreturn = RegCreateKey( hkey
                        , achTemp
                        , &hsubkey     );
  if( ERROR_SUCCESS != lreturn )
  {
    RegCloseKey( hkey );
    return AmHresultFromWin32(lreturn);
  }

  // set Server string
  //
  (void)StringCchPrintf( achTemp, NUMELMS(achTemp), TEXT("%ls"), szFileName );
  lreturn = RegSetValue( hsubkey
                       , (LPCTSTR)NULL
                       , REG_SZ
                       , (LPCTSTR)achTemp
                       , sizeof(TCHAR) * (lstrlen(achTemp)+1) );
  if( ERROR_SUCCESS != lreturn )
  {
    RegCloseKey( hkey );
    RegCloseKey( hsubkey );
    return AmHresultFromWin32(lreturn);
  }

  (void)StringCchPrintf( achTemp, NUMELMS(achTemp), TEXT("%ls"), szThreadingModel );
  lreturn = RegSetValueEx( hsubkey
                         , TEXT("ThreadingModel")
                         , 0L
                         , REG_SZ
                         , (CONST BYTE *)achTemp
                         , sizeof(TCHAR) * (lstrlen(achTemp)+1) );

  // close hkeys
  //
  RegCloseKey( hkey );
  RegCloseKey( hsubkey );

  // and return
  //
  return HRESULT_FROM_WIN32(lreturn);

}


//---------------------------------------------------------------------------
//
// AMovieSetupUnregisterServer()
//
// default ActiveMovie dll setup function
// - to use must be called from an exported
//   function named DllRegisterServer()
//
//---------------------------------------------------------------------------

STDAPI
AMovieSetupUnregisterServer( CLSID clsServer )
{
  // convert CLSID uuid to string and write
  // out subkey CLSID\{}
  //
  OLECHAR szCLSID[CHARS_IN_GUID];
  HRESULT hr = StringFromGUID2( clsServer
                              , szCLSID
                              , CHARS_IN_GUID );
  ASSERT( SUCCEEDED(hr) );

  TCHAR achBuffer[MAX_KEY_LEN];
  (void)StringCchPrintf( achBuffer, NUMELMS(achBuffer), TEXT("CLSID\\%ls"), szCLSID );

  // delete subkey
  //

  hr = EliminateSubKey( HKEY_CLASSES_ROOT, achBuffer );
  ASSERT( SUCCEEDED(hr) );

  // return
  //
  return NOERROR;
}


//---------------------------------------------------------------------------
//
// AMovieSetupRegisterFilter through IFilterMapper2
//
//---------------------------------------------------------------------------

STDAPI
AMovieSetupRegisterFilter2( const AMOVIESETUP_FILTER * const psetupdata
                          , IFilterMapper2 *                 pIFM2
                          , BOOL                             bRegister  )
{
  DbgLog((LOG_TRACE, 3, TEXT("= AMovieSetupRegisterFilter")));

  // check we've got data
  //
  if( NULL == psetupdata ) return S_FALSE;


  // unregister filter
  // (as pins are subkeys of filter's CLSID key
  // they do not need to be removed separately).
  //
  DbgLog((LOG_TRACE, 3, TEXT("= = unregister filter")));
  HRESULT hr = pIFM2->UnregisterFilter(
      0,                        // default category
      0,                        // default instance name
      *psetupdata->clsID );


  if( bRegister )
  {
    REGFILTER2 rf2;
    rf2.dwVersion = 1;
    rf2.dwMerit = psetupdata->dwMerit;
    rf2.cPins = psetupdata->nPins;
    rf2.rgPins = psetupdata->lpPin;
    
    // register filter
    //
    DbgLog((LOG_TRACE, 3, TEXT("= = register filter")));
    hr = pIFM2->RegisterFilter(*psetupdata->clsID
                             , psetupdata->strName
                             , 0 // moniker
                             , 0 // category
                             , NULL // instance
                             , &rf2);
  }

  // handle one acceptable "error" - that
  // of filter not being registered!
  // (couldn't find a suitable #define'd
  // name for the error!)
  //
  if( 0x80070002 == hr)
    return NOERROR;
  else
    return hr;
}

#ifndef STREAMS_PROVIDE_CUSTOM_FACTORY

//---------------------------------------------------------------------------
//
// RegisterAllServers()
//
//---------------------------------------------------------------------------

STDAPI
RegisterAllServers( LPCWSTR szFileName, BOOL bRegister )
{
  HRESULT hr = NOERROR;

  for( int i = 0; i < g_cTemplates; i++ )
  {
    // get i'th template
    //
    const CFactoryTemplate *pT = &g_Templates[i];

    DbgLog((LOG_TRACE, 2, TEXT("- - register %ls"),
           (LPCWSTR)pT->m_Name ));

    // register CLSID and InprocServer32
    //
    if( bRegister )
    {
      hr = AMovieSetupRegisterServer( *(pT->m_ClsID)
                                    , (LPCWSTR)pT->m_Name
                                    , szFileName );
    }
    else
    {
      hr = AMovieSetupUnregisterServer( *(pT->m_ClsID) );
    }

    // check final error for this pass
    // and break loop if we failed
    //
    if( FAILED(hr) )
      break;
  }

  return hr;
}


//---------------------------------------------------------------------------
//
// AMovieDllRegisterServer2()
//
// default ActiveMovie dll setup function
// - to use must be called from an exported
//   function named DllRegisterServer()
//
// this function is table driven using the
// static members of the CFactoryTemplate
// class defined in the dll.
//
// it registers the Dll as the InprocServer32
// and then calls the IAMovieSetup.Register
// method.
//
//---------------------------------------------------------------------------

STDAPI
AMovieDllRegisterServer2( BOOL bRegister )
{
  HRESULT hr = NOERROR;

  DbgLog((LOG_TRACE, 2, TEXT("AMovieDllRegisterServer2()")));

  // get file name (where g_hInst is the
  // instance handle of the filter dll)
  //
  WCHAR achFileName[MAX_PATH];

  // WIN95 doesn't support GetModuleFileNameW
  //
  {
    char achTemp[MAX_PATH];

    DbgLog((LOG_TRACE, 2, TEXT("- get module file name")));

    // g_hInst handle is set in our dll entry point. Make sure
    // DllEntryPoint in dllentry.cpp is called
    ASSERT(g_hInst != 0);

    if( 0 == GetModuleFileNameA( g_hInst
                              , achTemp
                              , sizeof(achTemp) ) )
    {
      // we've failed!
      DWORD dwerr = GetLastError();
      return AmHresultFromWin32(dwerr);
    }

    MultiByteToWideChar( CP_ACP
                       , 0L
                       , achTemp
                       , lstrlenA(achTemp) + 1
                       , achFileName
                       , NUMELMS(achFileName) );
  }

  //
  // first registering, register all OLE servers
  //
  if( bRegister )
  {
    DbgLog((LOG_TRACE, 2, TEXT("- register OLE Servers")));
    hr = RegisterAllServers( achFileName, TRUE );
  }

  //
  // next, register/unregister all filters
  //

  if( SUCCEEDED(hr) )
  {
    // init is ref counted so call just in case
    // we're being called cold.
    //
    DbgLog((LOG_TRACE, 2, TEXT("- CoInitialize")));
    hr = CoInitialize( (LPVOID)NULL );
    ASSERT( SUCCEEDED(hr) );

    // get hold of IFilterMapper2
    //
    DbgLog((LOG_TRACE, 2, TEXT("- obtain IFilterMapper2")));
    IFilterMapper2 *pIFM2 = 0;
    IFilterMapper *pIFM = 0;
    hr = CoCreateInstance( CLSID_FilterMapper2
                         , NULL
                         , CLSCTX_INPROC_SERVER
                         , IID_IFilterMapper2
                         , (void **)&pIFM2       );
    if(FAILED(hr))
    {
        DbgLog((LOG_TRACE, 2, TEXT("- trying IFilterMapper instead")));

        hr = CoCreateInstance(
            CLSID_FilterMapper,
            NULL,
            CLSCTX_INPROC_SERVER,
            IID_IFilterMapper,
            (void **)&pIFM);
    }
    if( SUCCEEDED(hr) )
    {
      // scan through array of CFactoryTemplates
      // registering servers and filters.
      //
      DbgLog((LOG_TRACE, 2, TEXT("- register Filters")));
      for( int i = 0; i < g_cTemplates; i++ )
      {
        // get i'th template
        //
        const CFactoryTemplate *pT = &g_Templates[i];

        if( NULL != pT->m_pAMovieSetup_Filter )
        {
          DbgLog((LOG_TRACE, 2, TEXT("- - register %ls"), (LPCWSTR)pT->m_Name ));

          if(pIFM2)
          {
              hr = AMovieSetupRegisterFilter2( pT->m_pAMovieSetup_Filter, pIFM2, bRegister );
          }
          else
          {
              hr = AMovieSetupRegisterFilter( pT->m_pAMovieSetup_Filter, pIFM, bRegister );
          }
        }

        // check final error for this pass
        // and break loop if we failed
        //
        if( FAILED(hr) )
          break;
      }

      // release interface
      //
      if(pIFM2)
          pIFM2->Release();
      else
          pIFM->Release();

    }

    // and clear up
    //
    CoFreeUnusedLibraries();
    CoUninitialize();
  }

  //
  // if unregistering, unregister all OLE servers
  //
  if( SUCCEEDED(hr) && !bRegister )
  {
    DbgLog((LOG_TRACE, 2, TEXT("- register OLE Servers")));
    hr = RegisterAllServers( achFileName, FALSE );
  }

  DbgLog((LOG_TRACE, 2, TEXT("- return %0x"), hr));
  return hr;
}


//---------------------------------------------------------------------------
//
// AMovieDllRegisterServer()
//
// default ActiveMovie dll setup function
// - to use must be called from an exported
//   function named DllRegisterServer()
//
// this function is table driven using the
// static members of the CFactoryTemplate
// class defined in the dll.
//
// it registers the Dll as the InprocServer32
// and then calls the IAMovieSetup.Register
// method.
//
//---------------------------------------------------------------------------


STDAPI
AMovieDllRegisterServer( void )
{
  HRESULT hr = NOERROR;

  // get file name (where g_hInst is the
  // instance handle of the filter dll)
  //
  WCHAR achFileName[MAX_PATH];

  {
    // WIN95 doesn't support GetModuleFileNameW
    //
    char achTemp[MAX_PATH];

    if( 0 == GetModuleFileNameA( g_hInst
                              , achTemp
                              , sizeof(achTemp) ) )
    {
      // we've failed!
      DWORD dwerr = GetLastError();
      return AmHresultFromWin32(dwerr);
    }

    MultiByteToWideChar( CP_ACP
                       , 0L
                       , achTemp
                       , lstrlenA(achTemp) + 1
                       , achFileName
                       , NUMELMS(achFileName) );
  }

  // scan through array of CFactoryTemplates
  // registering servers and filters.
  //
  for( int i = 0; i < g_cTemplates; i++ )
  {
    // get i'th template
    //
    const CFactoryTemplate *pT = &g_Templates[i];

    // register CLSID and InprocServer32
    //
    hr = AMovieSetupRegisterServer( *(pT->m_ClsID)
                                  , (LPCWSTR)pT->m_Name
                                  , achFileName );

    // instantiate all servers and get hold of
    // IAMovieSetup, if implemented, and call
    // IAMovieSetup.Register() method
    //
    if( SUCCEEDED(hr) && (NULL != pT->m_lpfnNew) )
    {
      // instantiate object
      //
      PAMOVIESETUP psetup;
      hr = CoCreateInstance( *(pT->m_ClsID)
                           , 0
                           , CLSCTX_INPROC_SERVER
                           , IID_IAMovieSetup
                           , reinterpret_cast<void**>(&psetup) );
      if( SUCCEEDED(hr) )
      {
        hr = psetup->Unregister();
        if( SUCCEEDED(hr) )
          hr = psetup->Register();
        psetup->Release();
      }
      else
      {
        if(    (E_NOINTERFACE      == hr )
            || (VFW_E_NEED_OWNER == hr ) )
          hr = NOERROR;
      }
    }

    // check final error for this pass
    // and break loop if we failed
    //
    if( FAILED(hr) )
      break;

  } // end-for

  return hr;
}


//---------------------------------------------------------------------------
//
// AMovieDllUnregisterServer()
//
// default ActiveMovie dll uninstall function
// - to use must be called from an exported
//   function named DllRegisterServer()
//
// this function is table driven using the
// static members of the CFactoryTemplate
// class defined in the dll.
//
// it calls the IAMovieSetup.Unregister
// method and then unregisters the Dll
// as the InprocServer32
//
//---------------------------------------------------------------------------

STDAPI
AMovieDllUnregisterServer()
{
  // initialize return code
  //
  HRESULT hr = NOERROR;

  // scan through CFactory template and unregister
  // all OLE servers and filters.
  //
  for( int i = g_cTemplates; i--; )
  {
    // get i'th template
    //
    const CFactoryTemplate *pT = &g_Templates[i];

    // check method exists
    //
    if( NULL != pT->m_lpfnNew )
    {
      // instantiate object
      //
      PAMOVIESETUP psetup;
      hr = CoCreateInstance( *(pT->m_ClsID)
                           , 0
                           , CLSCTX_INPROC_SERVER
                           , IID_IAMovieSetup
                           , reinterpret_cast<void**>(&psetup) );
      if( SUCCEEDED(hr) )
      {
        hr = psetup->Unregister();
        psetup->Release();
      }
      else
      {
        if(    (E_NOINTERFACE      == hr )
            || (VFW_E_NEED_OWNER == hr ) )
           hr = NOERROR;
      }
    }

    // unregister CLSID and InprocServer32
    //
    if( SUCCEEDED(hr) )
    {
      hr = AMovieSetupUnregisterServer( *(pT->m_ClsID) );
    }

    // check final error for this pass
    // and break loop if we failed
    //
    if( FAILED(hr) )
      break;
  }

  return hr;
}
#endif
