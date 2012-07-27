#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <ccn/ccn.h>
#include <ccn/uri.h>
#include <ccn/keystore.h>
#include <ccn/signing.h>
#include <ccn/charbuf.h>
#include <ccn/reg_mgmt.h>
#include <ccn/ccn_private.h>
#include <ccn/ccnd.h>


#define DEBUG


enum ccn_upcall_res incoming_interest(struct ccn_closure *selfp,
        enum ccn_upcall_kind kind, struct ccn_upcall_info *info)
{
    int res = 0;
    switch (kind) 
    {
    case CCN_UPCALL_FINAL:
        return CCN_UPCALL_RESULT_OK;    
        break;
    
    case CCN_UPCALL_INTEREST:
        printf("hello\n");
        return(CCN_UPCALL_FINAL);
        break;

    default:
        printf("Don't care\n");
        break;
    }
    return(CCN_UPCALL_RESULT_OK);
}


int main(int argc, char **argv)
{
    //create ccn handle
    struct ccn *ccn = ccn_create();
    if (ccn_connect(ccn, NULL) == -1) 
    {
        fprintf(stderr, "Could not connect to ccnd");
        exit(1);
    }

    // specify prefix we are interested in, register in FIB
    int res;
    struct ccn_charbuf *prefix = ccn_charbuf_create();
    res = ccn_name_from_uri(prefix, "/");
    if (res < 0) 
    {
        fprintf(stderr, "Can not convert name to URI\n");
        exit(1);
    }

    //handle for upcalls, receive notifications of incoming interests and content.
    //specify where the reply will go
    struct ccn_closure in_interest = {.p = &incoming_interest};
    in_interest.data = &prefix;

    //set the interest filter for
    res = ccn_set_interest_filter(ccn, prefix, &in_interest); 
    if (res < 0) 
    {
        fprintf(stderr, "Failed to register interest (res == %d)\n", res);
        exit(1);
    }

    //listen infinitely
    res = ccn_run(ccn, -1);

    //cleanup
    
    ccn_destroy(&ccn);
    ccn_charbuf_destroy(&prefix);
    exit(0);
}

