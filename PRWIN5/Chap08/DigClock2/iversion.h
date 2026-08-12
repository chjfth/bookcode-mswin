#ifndef __DigClock2_iversion_h_
#define __DigClock2_iversion_h_

#define DigClock2_VMAJOR 2
#define DigClock2_VMINOR 4
#define DigClock2_VPATCH 6
#define DigClock2_VTAIL  1

#define DigClock2str__(n) #n
#define DigClock2str(n) DigClock2str__(n)

// The following 4 are used in .rc
#define DigClock2_VMAJORs DigClock2str(DigClock2_VMAJOR)
#define DigClock2_VMINORs DigClock2str(DigClock2_VMINOR)
#define DigClock2_VPATCHs DigClock2str(DigClock2_VPATCH)
#define DigClock2_VTAILs  DigClock2str(DigClock2_VTAIL)

#define DigClock2_NAME "DigClock2"

enum {
	DigClock2_vmajor = DigClock2_VMAJOR,
	DigClock2_vminor = DigClock2_VMINOR,
	DigClock2_vpatch = DigClock2_VPATCH,
	DigClock2_vtail = DigClock2_VTAIL,
};


#endif
