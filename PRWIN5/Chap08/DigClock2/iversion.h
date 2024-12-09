#ifndef __iversion_h_
#define __iversion_h_

#define THISEXE_VMAJOR 1
#define THISEXE_VMINOR 5
#define THISEXE_VBUILD 0

#define THISEXEstr__(n) #n
#define THISEXEstr(n) THISEXEstr__(n)

// The following 3 are used in .rc
#define THISEXE_VMAJORs THISEXEstr(THISEXE_VMAJOR)
#define THISEXE_VMINORs THISEXEstr(THISEXE_VMINOR)
#define THISEXE_VBUILDs THISEXEstr(THISEXE_VBUILD)

#define THISEXE_NAME "DigClock2"

#endif
