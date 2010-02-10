
#include "os_current.h"

#include "os_base.h"

#ifdef __linux__
	#include "info_linux.cpp"
#elif defined(sgi) && sgi
	#include "info_sgi.cpp"
#elif defined(__FreeBSD__) || defined(__DragonFly__)
	#include "info_fbsd.cpp"
#elif defined(__hpux)
	#include "info_hpux.cpp"
#elif defined(__NetBSD__)
	#include "info_netbsd.cpp"
#elif defined(__OpenBSD__)
	#include "info_openbsd.cpp"
#elif defined(__svr4__) && defined(sun)
	#include "info_solaris.cpp"
#elif defined(__svr4__)
	#include "info_svr4.cpp"
#elif defined(_AIX)
	#include "info_aix.cpp"
#elif defined(__APPLE__)
	#include "info_osx.cpp"
#else
	#include "info_generic.cpp"	/* Default for unsupported systems.... */
#endif
