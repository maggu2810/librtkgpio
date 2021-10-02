//
// Created by de23a4 on 30.09.2021.
//

#ifndef LIB_RTKGPIO_DLL_H
#define LIB_RTKGPIO_DLL_H

#if defined _WIN32 || defined __CYGWIN__
/* Define calling convention in one place, for convenience. */
#define RTKGPIO_CALL __cdecl
#ifdef RTKGPIO_BUILDING_DLL
#ifdef __GNUC__
#define RTKGPIO_API __attribute__ ((dllexport))
#else
#define RTKGPIO_API __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
#endif
#else
#ifdef __GNUC__
#define RTKGPIO_API __attribute__ ((dllimport))
#else
#define RTKGPIO_API __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
#endif
#endif
#define RTKGPIO_NOAPI
#else
#if __GNUC__ >= 4
#define RTKGPIO_API __attribute__ ((visibility ("default")))
#define RTKGPIO_NOAPI  __attribute__ ((visibility ("hidden")))
#else
#define RTKGPIO_API
#define RTKGPIO_NOAPI
#endif
#endif

#endif //LIB_RTKGPIO_DLL_H
