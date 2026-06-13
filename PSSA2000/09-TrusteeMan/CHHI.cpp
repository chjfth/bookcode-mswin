#include <CHHI_DEBUG.h>

#define vaDbgTs_IMPL
#include <CHHI_vaDBG_is_vaDbgTs.h> // optional

#define CHHI_ALL_IMPL
//
#define UILAYOUT_IMPL
#include "..\ClassLib\UILayout.h"      // See Appendix B.

#define AUTOBUF_IMPL
#include "..\ClassLib\AutoBuf.h"       // See Appendix B.

#define PRINTBUF_IMPL
#include "..\ClassLib\PrintBuf.h"      // See Appendix B.


#include "../chjutils/chjutils.h"

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
