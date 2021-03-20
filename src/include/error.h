#ifndef __ERROR_H
#define __ERROR_H
#define error(s)    {fprintf(stderr,"%s:LINE:%d ",__FILE__,__LINE__);fprintf(stderr,s); fflush(stdout);}
#define info(s)     {printf(s); fflush(stdout);}
#define check_ptr(s) \
{   \
    if (NULL == (s)){ \
        error("OOM");   \
        exit(1);    \
    }   \
}
#endif