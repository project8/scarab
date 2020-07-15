/*
 * terminate.hh
 *
 *  Created on: May 23, 2017
 *      Author: N.S. Oblath
 * 
 *  The terminate() function is called in the event of an unhandled exception.
 *  It can also be called manually to abort with a stack trace printed.
 */

#ifndef SCARAB_TERMINATE_HH_
#define SCARAB_TERMINATE_HH_


#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#ifndef __USE_GNU
#define __USE_GNU
#endif


namespace scarab
{
    // Option 1: Use the terminate function
    void terminate();

    // Option 2: Handle SIGABRT
    //void crit_err_hdlr(int sig_num, siginfo_t * info, void * ucontext);

} /* namespace scarab */

#endif /* SCARAB_TERMINATE_HH_ */
