/*
 * =====================================================================================
 *
 *       Filename: strprintf.h
 *        Created: 02/13/2018 19:31:33
 *    Description: 
 *                  try to support printf to a std::string or any sink
 *                  need to take care of va_list
 *
 *                  a). C++14 say nothing for va_list, it's from C
 *                  b). C99/11 states that(C99-7.15, C11-7.16)
 *                     1. va_list can be re-initialized after called va_end()
 *
 *                          void func_a(szFormat, ...)
 *                          {
 *                              va_list ap;
 *                              va_start(szFormat, ap);
 *                              ...
 *                              va_end(ap);
 *
 *                              va_start(szFormat, ap);
 *                              ...
 *                              va_end(ap);
 *                          }
 *
 *                     2. va_list declared in func_a can be passed to func_b as an
 *                        argument, if func_b access va_list via va_arg then va_list
 *                        in func_a is indeterminate and shall be passed to va_end()
 *                        before any further reference
 *
 *                          void func_b(va_list ap)
 *                          {
 *                              ...
 *                          }
 *
 *                          void func_a(szFormat, ...)
 *                          {
 *                              va_list ap;
 *                              va_start(szFormat, ap);
 *
 *                              func_b(ap)
 *
 *                              // now ap is indeterminate
 *                              // no reference to ap before va_end()
 *                              // means func_a() can't see any change of ap in func_b()
 *
 *                              va_end(ap);
 *                          }
 *
 *                     3. va_list declared in func_a can be passed to func_b via a pointer
 *                        to it, in which case the original function may make further use
 *                        of the original va_list after func_b returns
 *                      
 *                          void func_b(va_list *ap)
 *                          {
 *                              // pull out one argument from ap
 *                              // and handle it
 *                          }
 *
 *                          void func_a(szFormat, ...)
 *                          {
 *                              va_list ap;
 *                              va_start(szFormat, ap);
 *
 *                              func_b(&ap)
 *
 *                              // now ap is still ok
 *                              // func_a() can see changes of ap in func_b()
 *
 *                              func_b(&ap)
 *                              ...
 *
 *                              va_end(ap);
 *                          }
 *
 *                  if we want to use this va_list in C++ safely, make a .a with a C99/11
 *                  compiler and and call it from C++
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

#ifndef __STR_PRINTF_H__
#define __STR_PRINTF_H__

#ifdef __cplusplus
extern "C"
{
#endif

// function takes printf-format argument with customized consume handler
// arguments:
//      fnConsume   : consume the parsed c string, provide nullptr if parse failed
//      szFormat    : format
// return value:
//                  : >= 0  : return value from std::vsnprintf()
//                  :   -1  : parsing error
//                      -2  : internal buffer error
//                    other : unknown error
int __strprintf(void(*)(const char *), const char *, ...);

#ifdef __cplusplus
}
#endif

#endif
