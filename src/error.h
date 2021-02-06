#ifndef __ERROR_H
#define __ERROR_H
#define error(s)    {fprintf(stderr,"LINE:%d ",__LINE__);fprintf(stderr,s); fflush(stdout);}
#define info(s)     {printf(s); fflush(stdout);}
#define check_ptr(s) \
{   \
    if (NULL == (s)){ \
        error("OOM");   \
        exit(1);    \
    }   \
}
#endif