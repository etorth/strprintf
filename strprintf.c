/*
 * =====================================================================================
 *
 *       Filename: strprintf.c
 *        Created: 02/21/2018 11:58:50
 *    Description: 
 *
 *        Version: 1.0
 *       Revision: none
 *       Compiler: gcc
 *
 *         Author: ANHONG
 *          Email: anhonghe@gmail.com
 *   Organization: USTC
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "strprintf.h"
int __strvprintf(void (*fnConsume)(const char *), const char *szFormat, va_list ap)
{
    int nRet = -1;

    // need to prepare this copy
    // in case parsing with static buffer failed

    va_list ap_static;
    va_copy(ap_static, ap);

    // 1. try static buffer
    //    give an enough size so we can hopefully stop here
    {
        char szSBuf[2048];
        nRet = vsnprintf(szSBuf, sizeof(szSBuf), szFormat, ap_static);

        if(nRet >= 0){
            if((size_t)(nRet + 1) < sizeof(szSBuf)){
                if(fnConsume){
                    fnConsume(szSBuf);
                }
                va_end(ap_static);
                return nRet;
            }else{
                // do nothing
                // have to try the dynamic buffer method
            }
        }else{
            if(fnConsume){
                fnConsume(NULL);
            }
            va_end(ap_static);
            return -1;
        }
    }

    // 2. try dynamic buffer
    //    use the parsed buffer size above to get enough memory

    int nDLen = nRet + 1 + 64;
    while(1){
        char *szDBuf = (char *)(malloc(nDLen));
        if(szDBuf){
            va_list ap_dynamic;
            va_copy(ap_dynamic, ap);

            nRet = vsnprintf(szDBuf, nDLen, szFormat, ap_dynamic);
            va_end(ap_dynamic);

            if(nRet >= 0){
                if((size_t)(nRet + 1) < nDLen){
                    if(fnConsume){
                        fnConsume(szDBuf);
                    }
                    free(szDBuf);
                    return nRet;
                }else{
                    nDLen = nDLen * 2 + 1 + 64;
                }
            }else{
                if(fnConsume){
                    fnConsume(NULL);
                }
                free(szDBuf);
                return -1;
            }
        }else{
            if(fnConsume){
                fnConsume(NULL);
            }
            return -2;
        }
    }

    // would never be here
    // to make the compiler happy
    return -3;
}
