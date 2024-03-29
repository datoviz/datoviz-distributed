/*************************************************************************************************/
/*  Common macros                                                                                */
/*************************************************************************************************/

#ifndef DVZ_HEADER_MACROS
#define DVZ_HEADER_MACROS



/*************************************************************************************************/
/*  Includes                                                                                     */
/*************************************************************************************************/

#include <assert.h>
#include <inttypes.h>
#include <stdint.h>
#include <string.h>



/*************************************************************************************************/
/*  Symbol macros                                                                                */
/*************************************************************************************************/

#if CC_MSVC
#ifdef DVZ_SHARED
#define DVZ_EXPORT __declspec(dllexport)
#else
#define DVZ_EXPORT __declspec(dllimport)
#endif
#define DVZ_INLINE __forceinline
#else
#define DVZ_EXPORT __attribute__((visibility("default")))
#define DVZ_INLINE static inline __attribute((always_inline))
#endif

#ifndef __STDC_VERSION__
#define __STDC_VERSION__ 0
#endif



/*************************************************************************************************/
/*  C or C++                                                                                     */
/*************************************************************************************************/

#ifdef __cplusplus
#define LANG_CPP
#define EXTERN_C_ON  extern "C" {
#define EXTERN_C_OFF }
#else
#define LANG_C
#define EXTERN_C_ON
#define EXTERN_C_OFF
#endif



/*************************************************************************************************/
/*  Build macros                                                                                 */
/*************************************************************************************************/

#ifndef SPIRV_DIR
#define SPIRV_DIR ""
#endif

#ifndef SWIFTSHADER
#define SWIFTSHADER 0
#endif



/*************************************************************************************************/
/*  Mute macros                                                                                  */
/*************************************************************************************************/

#if CC_GCC
#define MUTE_ON                                                                                   \
    _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wsign-conversion\"")        \
        _Pragma("GCC diagnostic ignored \"-Wundef\"")                                             \
            _Pragma("GCC diagnostic ignored \"-Wcast-qual\"")                                     \
                _Pragma("GCC diagnostic ignored \"-Wredundant-decls\"")                           \
                    _Pragma("GCC diagnostic ignored \"-Wcast-qual\"")                             \
                        _Pragma("GCC diagnostic ignored \"-Wunused\"")                            \
                            _Pragma("GCC diagnostic ignored \"-Wunused-parameter\"")              \
                                _Pragma("GCC diagnostic ignored \"-Wstrict-overflow\"")           \
                                    _Pragma("GCC diagnostic ignored \"-Wswitch-default\"")        \
                                        _Pragma("GCC diagnostic ignored \"-Wmissing-braces\"")

#define MUTE_OFF _Pragma("GCC diagnostic pop")
#elif CC_CLANG
#define MUTE_ON                                                                                   \
    _Pragma("clang diagnostic push") _Pragma("clang diagnostic ignored \"-Wsign-conversion\"")    \
        _Pragma("clang diagnostic ignored \"-Wcast-qual\"")                                       \
            _Pragma("clang diagnostic ignored \"-Wredundant-decls\"") _Pragma(                    \
                "clang diagnostic ignored \"-Wcast-qual\"")                                       \
                _Pragma("clang diagnostic ignored \"-Wstrict-overflow\"") _Pragma(                \
                    "clang diagnostic ignored \"-Wswitch-default\"")                              \
                    _Pragma("clang diagnostic ignored \"-Wcast-align\"") _Pragma(                 \
                        "clang diagnostic ignored \"-Wundef\"")                                   \
                        _Pragma("clang diagnostic ignored \"-Wmissing-braces\"") _Pragma(         \
                            "clang diagnostic ignored \"-Wnullability-extension\"")               \
                            _Pragma("clang diagnostic ignored \"-Wunguarded-availability-new\"")  \
                                _Pragma("clang diagnostic ignored \"-Wdeprecated-declarations\"")

#define MUTE_OFF _Pragma("clang diagnostic pop")
#else
// MSVC: TODO
#define MUTE_ON
// #pragma warning(push)
#define MUTE_OFF
// #pragma warning(pop)
#endif



/*************************************************************************************************/
/*  Memory management                                                                            */
/*************************************************************************************************/

#define FREE(x)                                                                                   \
    if ((x) != NULL)                                                                              \
    {                                                                                             \
        free((x));                                                                                \
        (x) = NULL;                                                                               \
    }

#define ALIGNED_FREE(x)                                                                           \
    if (x.aligned)                                                                                \
        aligned_free(x.pointer);                                                                  \
    else                                                                                          \
        FREE(x.pointer)

#define REALLOC(x, s)                                                                             \
    {                                                                                             \
        void* _new = realloc((x), (s));                                                           \
        if (_new == NULL)                                                                         \
            exit(1);                                                                              \
        else                                                                                      \
            x = _new;                                                                             \
    }



/*************************************************************************************************/
/*  Misc                                                                                         */
/*************************************************************************************************/

#define ARRAY_COUNT(arr) sizeof((arr)) / sizeof((arr)[0])
#define ASSERT(x)        assert((x))
#define ANN(x)           ASSERT((x) != NULL);

#ifdef LANG_CPP
#define INIT(t, n) t n = {};
#else
#define INIT(t, n) t n = {0};
#endif

#define IF_VERBOSE if (getenv("DVZ_VERBOSE") && (strncmp(getenv("DVZ_VERBOSE"), "req", 3) == 0))

#define fsizeof(type, member) sizeof(((type*)0)->member)


#endif
