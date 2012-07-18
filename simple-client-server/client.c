//susmit@cs.colostate.edu
//
//
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ccn/ccn.h>
#include <ccn/uri.h>
#include <ccn/keystore.h>
#include <ccn/signing.h>


#define DEBUG



int main (int argc, char **argv)
{

    int res;
    //check if user supplied uri to trace
    if(argv[1] == NULL)
    {
        printf("Usage: trace URI\n");
        exit(1);
    }
 

        
    //get the length of user provided URI
    int argv_length = strlen(argv[1]);

    //check first six chars for ccnx:/, if present, skip them
    int skip = 0;
    res = strncmp("ccnx:/", argv[1], 6);
    if(res == 0)
    {
        skip = 5;        
    } 

    //if URI does not begins with /, exit
    if (argv[1][skip] != '/')
    {
        printf("URI must begin with /\n");
        exit(1);
    }

    //check if uri ends with slash, append if missing
    char *slash = "";
    if (argv[1][argv_length-1] != '/')
    {
      slash = "/";
    }


    
    //allocate memory for trace URI = /trace/user_input/random_number
    char *URI = (char *) malloc(sizeof(char)* argv_length+1); //find size of rand
    if(URI == NULL)
    {
        fprintf(stderr, "Can not allocate memory for URI\n");
        exit(1);
    }
    
    //put together the trace URI, add a random number to end of URI
    srand ((unsigned int)time (NULL)*getpid());
    sprintf(URI, "%s%s", argv[1]+skip, slash);


   //allocate memory for interest 
    struct ccn_charbuf *ccnb = ccn_charbuf_create();
    if(ccnb == NULL)
    {
        fprintf(stderr, "Can not allocate memory for interest\n");
        exit(1);
    }
    
    
    //adding name to interest
    res = ccn_name_from_uri(ccnb, URI);
    if(res == -1)
    {
        fprintf(stderr, "Failed to assign name to interest");
        exit(1);
    }    

    //create the ccn handle
    struct ccn *ccn = ccn_create();
    if(ccn == NULL)
    {
        fprintf(stderr, "Can not create ccn handle\n");
        exit(1);
    }
 
    //connect to ccnd
    res = ccn_connect(ccn, NULL);
    if (res == -1) 
    {
        fprintf(stderr, "Could not connect to ccnd... exiting\n");
        exit(1);
    }

    #ifdef DEBUG
        printf("Connected to CCND, return code: %d\n", res);
    #endif
   
    //allocate buffer for response
    struct ccn_charbuf *resultbuf = ccn_charbuf_create();
    if(resultbuf == NULL)
    {
        fprintf(stderr, "Can not allocate memory for URI\n");
        exit(1);
    }

    //setting the parameters for ccn_get
    struct ccn_parsed_ContentObject pcobuf = { 0 };
    int timeout_ms = 6000;
    
    //express interest
    res = ccn_get(ccn, ccnb, NULL, timeout_ms, resultbuf, &pcobuf, NULL, 0);
    if (res == -1)
    {
        fprintf(stderr, "Did not receive answer for trace to %s\n", argv[1]);
	#ifdef DEBUG
            fprintf(stderr, "Did not receive answer for trace to URI: %s\n", URI);
        #endif
        exit(1);
    }

    //extract data from the response
    const unsigned char *ptr;
    size_t length;
    ptr = resultbuf->buf;
    length = resultbuf->length;
    ccn_content_get_value(ptr, length, &pcobuf, &ptr, &length);

    //check if received some data
    if(length == 0)
    {    
        fprintf(stderr, "Received empty answer for trace to %s\n", argv[1]);
	    #ifdef DEBUG
            fprintf(stderr, "Received empty answer for trace to URI: %s\n", URI);
        #endif
        exit(1);
    }

    //print the data
    printf("Reply: %s\n",  ptr);
    printf("Length of data: %Zu\n", length);
    exit(0);
}

