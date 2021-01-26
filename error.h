#ifndef __ERROR_H
#define __ERROR_H
#define error(s)    {fprintf(stderr,"LINE:%d ",__LINE__);fprintf(stderr,s); fflush(stdout);}
#define info(s)     {printf(s); fflush(stdout);}
#endif