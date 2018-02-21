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
#include <stdlib.h>
#include "strprintf.h"
int __strvprintf(void (*fnConsume)(const char *, int, void *), void *pArg, const char *szFormat, va_list ap)
{
    int nRet = -1;

    // need to prepare this copy
    // in case parsing with static buffer failed

    va_list ap_static;
    va_copy(ap_static, ap);

    // 1. try static buffer
    //    give an enough size so we can hopefully stop here
    {
        char szSBuf[512];
        nRet = vsnprintf(szSBuf, sizeof(szSBuf), szFormat, ap_static);

        if(nRet >= 0){
            if((size_t)(nRet) < sizeof(szSBuf)){
                if(fnConsume){
                    fnConsume(szSBuf, nRet, pArg);
                }
                va_end(ap_static);
                return nRet;
            }else{
                // do nothing
                // have to try the dynamic buffer method
                va_end(ap_static);
            }
        }else{
            if(fnConsume){
                fnConsume(NULL, -1, pArg);
            }
            va_end(ap_static);
            return -1;
        }
    }

    // 2. try dynamic buffer
    //    use the parsed buffer size above to get enough memory

    int nDLen = nRet + 64;
    while(1){
        char *szDBuf = (char *)(malloc(nDLen));
        if(szDBuf){
            va_list ap_dynamic;
            va_copy(ap_dynamic, ap);

            nRet = vsnprintf(szDBuf, nDLen, szFormat, ap_dynamic);
            va_end(ap_dynamic);

            if(nRet >= 0){
                if(nRet < nDLen){
                    if(fnConsume){
                        fnConsume(szDBuf, nRet, pArg);
                    }
                    free(szDBuf);
                    return nRet;
                }else{
                    nDLen = nDLen * 2 + 64;
                }
            }else{
                if(fnConsume){
                    fnConsume(NULL, -1, pArg);
                }
                free(szDBuf);
                return -1;
            }
        }else{

            // get memroy failed
            // we can try more time here

            if(fnConsume){
                fnConsume(NULL, -2, pArg);
            }
            return -2;
        }
    }

    return -3;
}
