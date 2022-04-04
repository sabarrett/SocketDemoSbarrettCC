#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#define NOMINMAX
// If defined, the following flags inhibit definition of the indicated items.
//#define NOGDICAPMASKS     // CC_*, LC_*, PC_*, CP_*, TC_*, RC_
//#define NOVIRTUALKEYCODES // VK_*
//#define NOWINMESSAGES     // WM_*, EM_*, LB_*, CB_*
//#define NOWINSTYLES       // WS_*, CS_*, ES_*, LBS_*, SBS_*, CBS_*
//#define NOSYSMETRICS      // SM_*
//#define NOMENUS           // MF_*
//#define NOICONS           // IDI_*
//#define NOKEYSTATES       // MK_*
//#define NOSYSCOMMANDS     // SC_*
//#define NORASTEROPS       // Binary and Tertiary raster ops
//#define NOSHOWWINDOW      // SW_*
//#define OEMRESOURCE       // OEM Resource values
//#define NOATOM            // Atom Manager routines
//#define NOCLIPBOARD       // Clipboard routines
//#define NOCOLOR           // Screen colors
//#define NOCTLMGR          // Control and Dialog routines
//#define NODRAWTEXT        // DrawText() and DT_*
//#define NOGDI             // All GDI defines and routines
//#define NOKERNEL          // All KERNEL defines and routines
#define NOUSER            // All USER defines and routines
//#define NOMB              // MB_* and MessageBox()
//#define NOMEMMGR          // GMEM_*, LMEM_*, GHND, LHND, associated routines
//#define NOMETAFILE        // typedef METAFILEPICT
//#define NOMINMAX          // Macros min(a,b) and max(a,b)
//#define NOMSG             // typedef MSG and associated routines
//#define NOOPENFILE        // OpenFile(), OemToAnsi, AnsiToOem, and OF_*
//#define NOSCROLL          // SB_* and scrolling routines
//#define NOSERVICE         // All Service Controller routines, SERVICE_ equates, etc.
//#define NOSOUND           // Sound driver routines
//#define NOTEXTMETRIC      // typedef TEXTMETRIC and associated routines
//#define NOWH              // SetWindowsHook and WH_*
//#define NOWINOFFSETS      // GWL_*, GCL_*, associated routines
//#define NOCOMM            // COMM driver routines
//#define NOKANJI           // Kanji support stuff.
//#define NOHELP            // Help engine interface.
//#define NOPROFILER        // Profiler interface.
//#define NODEFERWINDOWPOS  // DeferWindowPos routines
//#define NOMCX             // Modem Configuration Extensions

	#include "Windows.h"
	#include "WinSock2.h"
	#include "Ws2tcpip.h"
	typedef int socklen_t;
	//typedef char* receiveBufer_t;
#else
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <sys/types.h>
	#include <netdb.h>
	#include <errno.h>
	#include <fcntl.h>
	#include <unistd.h>
	//typedef void* receiveBufer_t;
	typedef int SOCKET;
	const int NO_ERROR = 0;
	const int INVALID_SOCKET = -1;
	const int WSAECONNRESET = ECONNRESET;
	const int WSAEWOULDBLOCK = EAGAIN;
	const int SOCKET_ERROR = -1;
#endif


#include "memory"

#include "vector"
#include "unordered_map"
#include "string"
#include "list"
#include "queue"
#include "deque"
#include "unordered_set"
#include "cassert"

using std::shared_ptr;
using std::unique_ptr;
using std::vector;
using std::queue;
using std::list;
using std::deque;
using std::unordered_map;
using std::string;
using std::unordered_set;

class RoboCat;
class GameObject;

//#include "RoboMath.h"

#include "LinkingContext.h"
#include "ByteSwap.h"
#include "MemoryBitStream.h"

#include "StringUtils.h"
#include "SocketAddress.h"
#include "SocketAddressFactory.h"
#include "UDPSocket.h"
#include "TCPSocket.h"
#include "SocketUtil.h"
#include "OutputWindow.h"
