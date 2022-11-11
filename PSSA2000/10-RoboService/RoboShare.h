#pragma once

#include <sddl.h>

// Message id between server and client:

enum ROBOMSG_et
{
	ROMOMSG_None = 0,

	ROBOMSG_STOPSERVICE      = 1,
	ROBOMSG_REMOVESERVICE    = 2,
	ROBOMSG_ERR              = 3,

	ROBOMSG_QUERYROBOTNAMES  = 10,
	ROBOMSG_ROBOTNAME        = 11, // Requires MsgDataName extra data

	ROBOMSG_CREATEROBOT      = 12, // Requires MsgDataName extra data
	ROBOMSG_DELETEROBOT      = 13, // Requires MsgDataName extra data
	ROBOMSG_ROBOTREMOVED     = 14, // Requires MsgDataName extra data

	ROBOMSG_CHANGENAME       = 15, // Requires an array of two MsgDataName

	ROBOMSG_LOCK             = 16, // Requires MsgDataName extra data

	ROBOMSG_QUERY            = 17, // Requires MsgDataName extra data
	ROBOMSG_ROBOTMSG         = 18, // Requires a following string

	ROBOMSG_ACTION           = 19, // Requires MsgDataName extra data

	ROBOMSG_QUERYSECURITY    = 20, // Requires MsgDataName extra data
	ROBOMSG_RETURNSECURITY   = 21, // Requires MsgDataSD extra data

	ROBOMSG_SETSECURITY      = 22, // Requires MsgDataName and MsgDataSD extra data
};

#define PIPENAME_LOCALPART TEXT("\\Pipe\\RoboService")

#define PIPENAME_LOCALFULL TEXT("\\\\.") PIPENAME_LOCALPART

#define MAXCONNECTIONS     100
#define PENDINGCONNECTIONS 10

// Error defines
#define ROBOERROR_NAMEEXISTS    1
#define ROBOERROR_ROBOTNOTFOUND 2
#define ROBOERROR_ACCESSDENIED  3

// Action defines
#define ROBOACTION_GATHER       1
#define ROBOACTION_ASSEMBLE     2

// What we can do to a robot
#define ROBOT_SETNAME      (0x0001)    // Change its name 
#define ROBOT_LOCK         (0x0002)    // Lock it 
#define ROBOT_GATHER       (0x0004)    // Gather Material 
#define ROBOT_ASSEMBLE     (0x0008)    // Assemble Material 
#define ROBOT_QUERY        (0x0010)    // Query Status 
#define ROBOT_OVERRIDELOCK (0x0020)    // Unlock it (even if not the "locker")
#define ROBOT_ALL_ACCESS   (STANDARD_RIGHTS_REQUIRED \
							| ROBOT_SETNAME \
							| ROBOT_LOCK \
							| ROBOT_GATHER \
							| ROBOT_ASSEMBLE \
							| ROBOT_QUERY \
							| ROBOT_OVERRIDELOCK)

// The base message
typedef struct _MessageBase {
	ROBOMSG_et m_lMsgType;
	ULONG m_lInfo;
	ULONG m_lExtraDataSize;
} MessageBase;
//
#define MSGSIZE sizeof(MessageBase)


// A data structure for a message with a robot name in the data
typedef struct _MsgDataName {
	TCHAR m_szName[256];
} MsgDataName;

// A data structure for a message with a security descriptor
typedef struct _MsgDataSD {
	SECURITY_DESCRIPTOR m_sdSecurity;
} MsgDataSD;




