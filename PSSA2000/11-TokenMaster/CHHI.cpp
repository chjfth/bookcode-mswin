#include <CHHI_DEBUG.h>

#define vaDbgTs_IMPL
#include <CHHI_vaDBG_is_vaDbgTs.h> // optional

#define CHHI_ALL_IMPL
//
//#include "..\chjutils\ch10-DumpSD.h"

#define PRINTBUF_IMPL
//#include "..\ClassLib\PrintBuf.h"

#define AUTOBUF_IMPL
#include "..\ClassLib\AutoBuf.h"       // See Appendix B.

#include "shareinc.h"
#include "../chjutils/chjutils.h"

#include <mswin/JULayout2.h>
#include <mswin/Combobox_EnableWideDrop.h>
//

#include <mswin/utils_env.h>
#include <mswin/utils_wingui.h>
//
#include <mswin/WinError.itc.h>
#include <mswin/winuser.itc.h>
#include <mswin/commctrl.itc.h>
#include <mswin/winnt.itc.h>
#include <mswin/AclUI.itc.h>
#include <mswin/AccCtrl.itc.h>
#include <mswin/PrSht.itc.h>
