#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <stdarg.h>

#include "wrappedlibs.h"

#include "debug.h"
#include "wrapper.h"
#include "bridge.h"
#include "librarian/library_private.h"
#include "x86emu.h"
#include "emu/x86emu_private.h"
#include "callback.h"
#include "librarian.h"
#include "box86context.h"
#include "emu/x86emu_private.h"
#include "myalign.h"
#include "gtkclass.h"

const char* gio2Name = "libgio-2.0.so.0";
#define LIBNAME gio2

static char* libname = NULL;

typedef int(*iFv_t)(void);
typedef void*(*pFipppp_t)(int, void*, void*, void*, void*);

#define ADDED_FUNCTIONS()                           \
GO(g_dbus_object_manager_client_get_type, iFv_t)    \
GO(g_initable_new_valist, pFipppp_t)                \

#include "generated/wrappedgio2types.h"

#include "wrappercallback.h"

#define SUPER() \
GO(0)   \
GO(1)   \
GO(2)   \
GO(3)

// GAsyncReadyCallback
#define GO(A)   \
static uintptr_t my_GAsyncReadyCallback_fct_##A = 0;                                        \
static void my_GAsyncReadyCallback_##A(void* source, void* res, void* data)                 \
{                                                                                           \
    RunFunctionFmt(my_context, my_GAsyncReadyCallback_fct_##A, "ppp", source, res, data);   \
}
SUPER()
#undef GO
static void* findGAsyncReadyCallbackFct(void* fct)
{
    if(!fct) return fct;
    if(GetNativeFnc((uintptr_t)fct))  return GetNativeFnc((uintptr_t)fct);
    #define GO(A) if(my_GAsyncReadyCallback_fct_##A == (uintptr_t)fct) return my_GAsyncReadyCallback_##A;
    SUPER()
    #undef GO
    #define GO(A) if(my_GAsyncReadyCallback_fct_##A == 0) {my_GAsyncReadyCallback_fct_##A = (uintptr_t)fct; return my_GAsyncReadyCallback_##A; }
    SUPER()
    #undef GO
    printf_log(LOG_NONE, "Warning, no more slot for gio2 GAsyncReadyCallback callback\n");
    return NULL;
}

// GDestroyNotify
#define GO(A)   \
static uintptr_t my_GDestroyNotify_fct_##A = 0;                         \
static void my_GDestroyNotify_##A(void* data)                           \
{                                                                       \
    RunFunctionFmt(my_context, my_GDestroyNotify_fct_##A, "p", data);   \
}
SUPER()
#undef GO
static void* findGDestroyNotifyFct(void* fct)
{
    if(!fct) return fct;
    if(GetNativeFnc((uintptr_t)fct))  return GetNativeFnc((uintptr_t)fct);
    #define GO(A) if(my_GDestroyNotify_fct_##A == (uintptr_t)fct) return my_GDestroyNotify_##A;
    SUPER()
    #undef GO
    #define GO(A) if(my_GDestroyNotify_fct_##A == 0) {my_GDestroyNotify_fct_##A = (uintptr_t)fct; return my_GDestroyNotify_##A; }
    SUPER()
    #undef GO
    printf_log(LOG_NONE, "Warning, no more slot for gio2 GDestroyNotify callback\n");
    return NULL;
}

// GDBusProxyTypeFunc
#define GO(A)   \
static uintptr_t my_GDBusProxyTypeFunc_fct_##A = 0;                                                             \
static int my_GDBusProxyTypeFunc_##A(void* manager, void* path, void* name, void* data)                         \
{                                                                                                               \
    return (int)RunFunctionFmt(my_context, my_GDBusProxyTypeFunc_fct_##A, "pppp", manager, path, name, data);   \
}
SUPER()
#undef GO
static void* findGDBusProxyTypeFuncFct(void* fct)
{
    if(!fct) return fct;
    if(GetNativeFnc((uintptr_t)fct))  return GetNativeFnc((uintptr_t)fct);
    #define GO(A) if(my_GDBusProxyTypeFunc_fct_##A == (uintptr_t)fct) return my_GDBusProxyTypeFunc_##A;
    SUPER()
    #undef GO
    #define GO(A) if(my_GDBusProxyTypeFunc_fct_##A == 0) {my_GDBusProxyTypeFunc_fct_##A = (uintptr_t)fct; return my_GDBusProxyTypeFunc_##A; }
    SUPER()
    #undef GO
    printf_log(LOG_NONE, "Warning, no more slot for gio2 GDBusProxyTypeFunc callback\n");
    return NULL;
}

// GSimpleAsyncThreadFunc
#define GO(A)   \
static uintptr_t my_GSimpleAsyncThreadFunc_fct_##A = 0;                                             \
static void my_GSimpleAsyncThreadFunc_##A(void* res, void* object, void* cancellable)               \
{                                                                                                   \
    RunFunctionFmt(my_context, my_GSimpleAsyncThreadFunc_fct_##A, "ppp", res, object, cancellable); \
}
SUPER()
#undef GO
static void* findGSimpleAsyncThreadFuncFct(void* fct)
{
    if(!fct) return fct;
    if(GetNativeFnc((uintptr_t)fct))  return GetNativeFnc((uintptr_t)fct);
    #define GO(A) if(my_GSimpleAsyncThreadFunc_fct_##A == (uintptr_t)fct) return my_GSimpleAsyncThreadFunc_##A;
    SUPER()
    #undef GO
    #define GO(A) if(my_GSimpleAsyncThreadFunc_fct_##A == 0) {my_GSimpleAsyncThreadFunc_fct_##A = (uintptr_t)fct; return my_GSimpleAsyncThreadFunc_##A; }
    SUPER()
    #undef GO
    printf_log(LOG_NONE, "Warning, no more slot for gio2 GSimpleAsyncThreadFunc callback\n");
    return NULL;
}

// GCallback
#define GO(A)   \
static uintptr_t my_GCallback_fct_##A = 0;                                  \
static void my_GCallback_##A(void* a, void* b, void* c, void* d)            \
{                                                                           \
    RunFunctionFmt(my_context, my_GCallback_fct_##A, "pppp", a, b, c, d);   \
}
SUPER()
#undef GO
static void* findGCallbackFct(void* fct)
{
    if(!fct) return fct;
    if(GetNativeFnc((uintptr_t)fct))  return GetNativeFnc((uintptr_t)fct);
    #define GO(A) if(my_GCallback_fct_##A == (uintptr_t)fct) return my_GCallback_##A;
    SUPER()
    #undef GO
    #define GO(A) if(my_GCallback_fct_##A == 0) {my_GCallback_fct_##A = (uintptr_t)fct; return my_GCallback_##A; }
    SUPER()
    #undef GO
    printf_log(LOG_NONE, "Warning, no more slot for gio2 GCallback callback\n");
    return NULL;
}

// GDBusSignalCallback
#define GO(A)   \
static uintptr_t my_GDBusSignalCallback_fct_##A = 0;                                                                                            \
static void my_GDBusSignalCallback_##A(void* connection, void* sender, void* object, void* interface, void* signal, void* param, void* data)    \
{                                                                                                                                               \
    RunFunctionFmt(my_context, my_GDBusSignalCallback_fct_##A, "ppppppp", connection, sender, object, interface, signal, param, data);          \
}
SUPER()
#undef GO
static void* findGDBusSignalCallbackFct(void* fct)
{
    if(!fct) return fct;
    if(GetNativeFnc((uintptr_t)fct))  return GetNativeFnc((uintptr_t)fct);
    #define GO(A) if(my_GDBusSignalCallback_fct_##A == (uintptr_t)fct) return my_GDBusSignalCallback_##A;
    SUPER()
    #undef GO
    #define GO(A) if(my_GDBusSignalCallback_fct_##A == 0) {my_GDBusSignalCallback_fct_##A = (uintptr_t)fct; return my_GDBusSignalCallback_##A; }
    SUPER()
    #undef GO
    printf_log(LOG_NONE, "Warning, no more slot for gio2 GDBusSignalCallback callback\n");
    return NULL;
}

// GDBusMessageFilterFunction
#define GO(A)   \
static uintptr_t my_GDBusMessageFilterFunction_fct_##A = 0;                                                         \
static void my_GDBusMessageFilterFunction_##A(void* connection, void* message, int incoming, void* data)            \
{                                                                                                                   \
    RunFunctionFmt(my_context, my_GDBusMessageFilterFunction_fct_##A, "ppip", connection, message, incoming, data); \
}
SUPER()
#undef GO
static void* findGDBusMessageFilterFunctionFct(void* fct)
{
    if(!fct) return fct;
    if(GetNativeFnc((uintptr_t)fct))  return GetNativeFnc((uintptr_t)fct);
    #define GO(A) if(my_GDBusMessageFilterFunction_fct_##A == (uintptr_t)fct) return my_GDBusMessageFilterFunction_##A;
    SUPER()
    #undef GO
    #define GO(A) if(my_GDBusMessageFilterFunction_fct_##A == 0) {my_GDBusMessageFilterFunction_fct_##A = (uintptr_t)fct; return my_GDBusMessageFilterFunction_##A; }
    SUPER()
    #undef GO
    printf_log(LOG_NONE, "Warning, no more slot for gio2 GDBusMessageFilterFunction callback\n");
    return NULL;
}

// GBusNameAppearedCallback
#define GO(A)   \
static uintptr_t my_GBusNameAppearedCallback_fct_##A = 0;                                                   \
static void my_GBusNameAppearedCallback_##A(void* connection, void* name, void* owner, void* data)          \
{                                                                                                           \
    RunFunctionFmt(my_context, my_GBusNameAppearedCallback_fct_##A, "pppp", connection, name, owner, data); \
}
SUPER()
#undef GO
static void* findGBusNameAppearedCallbackFct(void* fct)
{
    if(!fct) return fct;
    if(GetNativeFnc((uintptr_t)fct))  return GetNativeFnc((uintptr_t)fct);
    #define GO(A) if(my_GBusNameAppearedCallback_fct_##A == (uintptr_t)fct) return my_GBusNameAppearedCallback_##A;
    SUPER()
    #undef GO
    #define GO(A) if(my_GBusNameAppearedCallback_fct_##A == 0) {my_GBusNameAppearedCallback_fct_##A = (uintptr_t)fct; return my_GBusNameAppearedCallback_##A; }
    SUPER()
    #undef GO
    printf_log(LOG_NONE, "Warning, no more slot for gio2 GBusNameAppearedCallback callback\n");
    return NULL;
}

// GBusNameVanishedCallback
#define GO(A)   \
static uintptr_t my_GBusNameVanishedCallback_fct_##A = 0;                                           \
static void my_GBusNameVanishedCallback_##A(void* connection, void* name, void* data)               \
{                                                                                                   \
    RunFunctionFmt(my_context, my_GBusNameVanishedCallback_fct_##A, "ppp", connection, name, data); \
}
SUPER()
#undef GO
static void* findGBusNameVanishedCallbackFct(void* fct)
{
    if(!fct) return fct;
    if(GetNativeFnc((uintptr_t)fct))  return GetNativeFnc((uintptr_t)fct);
    #define GO(A) if(my_GBusNameVanishedCallback_fct_##A == (uintptr_t)fct) return my_GBusNameVanishedCallback_##A;
    SUPER()
    #undef GO
    #define GO(A) if(my_GBusNameVanishedCallback_fct_##A == 0) {my_GBusNameVanishedCallback_fct_##A = (uintptr_t)fct; return my_GBusNameVanishedCallback_##A; }
    SUPER()
    #undef GO
    printf_log(LOG_NONE, "Warning, no more slot for gio2 GBusNameVanishedCallback callback\n");
    return NULL;
}

// GBusAcquiredCallback
#define GO(A)   \
static uintptr_t my_GBusAcquiredCallback_fct_##A = 0;                                           \
static void my_GBusAcquiredCallback_##A(void* connection, void* name, void* data)               \
{                                                                                               \
    RunFunctionFmt(my_context, my_GBusAcquiredCallback_fct_##A, "ppp", connection, name, data); \
}
SUPER()
#undef GO
static void* findGBusAcquiredCallbackFct(void* fct)
{
    if(!fct) return fct;
    if(GetNativeFnc((uintptr_t)fct))  return GetNativeFnc((uintptr_t)fct);
    #define GO(A) if(my_GBusAcquiredCallback_fct_##A == (uintptr_t)fct) return my_GBusAcquiredCallback_##A;
    SUPER()
    #undef GO
    #define GO(A) if(my_GBusAcquiredCallback_fct_##A == 0) {my_GBusAcquiredCallback_fct_##A = (uintptr_t)fct; return my_GBusAcquiredCallback_##A; }
    SUPER()
    #undef GO
    printf_log(LOG_NONE, "Warning, no more slot for gio2 GBusAcquiredCallback callback\n");
    return NULL;
}

// GBusNameAcquiredCallback
#define GO(A)   \
static uintptr_t my_GBusNameAcquiredCallback_fct_##A = 0;                                           \
static void my_GBusNameAcquiredCallback_##A(void* connection, void* name, void* data)               \
{                                                                                                   \
    RunFunctionFmt(my_context, my_GBusNameAcquiredCallback_fct_##A, "ppp", connection, name, data); \
}
SUPER()
#undef GO
static void* findGBusNameAcquiredCallbackFct(void* fct)
{
    if(!fct) return fct;
    if(GetNativeFnc((uintptr_t)fct))  return GetNativeFnc((uintptr_t)fct);
    #define GO(A) if(my_GBusNameAcquiredCallback_fct_##A == (uintptr_t)fct) return my_GBusNameAcquiredCallback_##A;
    SUPER()
    #undef GO
    #define GO(A) if(my_GBusNameAcquiredCallback_fct_##A == 0) {my_GBusNameAcquiredCallback_fct_##A = (uintptr_t)fct; return my_GBusNameAcquiredCallback_##A; }
    SUPER()
    #undef GO
    printf_log(LOG_NONE, "Warning, no more slot for gio2 GBusNameAcquiredCallback callback\n");
    return NULL;
}

// GBusNameLostCallback
#define GO(A)   \
static uintptr_t my_GBusNameLostCallback_fct_##A = 0;                                           \
static void my_GBusNameLostCallback_##A(void* connection, void* name, void* data)               \
{                                                                                               \
    RunFunctionFmt(my_context, my_GBusNameLostCallback_fct_##A, "ppp", connection, name, data); \
}
SUPER()
#undef GO
static void* findGBusNameLostCallbackFct(void* fct)
{
    if(!fct) return fct;
    if(GetNativeFnc((uintptr_t)fct))  return GetNativeFnc((uintptr_t)fct);
    #define GO(A) if(my_GBusNameLostCallback_fct_##A == (uintptr_t)fct) return my_GBusNameLostCallback_##A;
    SUPER()
    #undef GO
    #define GO(A) if(my_GBusNameLostCallback_fct_##A == 0) {my_GBusNameLostCallback_fct_##A = (uintptr_t)fct; return my_GBusNameLostCallback_##A; }
    SUPER()
    #undef GO
    printf_log(LOG_NONE, "Warning, no more slot for gio2 GBusNameLostCallback callback\n");
    return NULL;
}

// GDBusInterfaceVTable....
// First the structure GDBusInterfaceVTable statics, with paired x86 source pointer
typedef struct my_GDBusInterfaceVTable_s {
  void      (*method_call)    (void* connection, void* sender, void* object_path, void* interface_name, void* method_name, void* invocation, void* user_data);
  void*     (*get_property)   (void* connection, void* sender, void* object_path, void* interface_name, void* error, void* user_data);
  int       (*set_property)   (void* connection, void* sender, void* object_path, void* interface_name, void* value, void* error, void* user_data);
} my_GDBusInterfaceVTable_t;

#define GO(A) \
static my_GDBusInterfaceVTable_t     my_GDBusInterfaceVTable_##A = {0};   \
static my_GDBusInterfaceVTable_t   *ref_GDBusInterfaceVTable_##A = NULL;
SUPER()
#undef GO
// then the static functions callback that may be used with the structure, but dispatch also have a callback...
#define GO(A)   \
static uintptr_t fct_funcs_method_call_##A = 0;                   \
static void my_funcs_method_call_##A(void* connection, void* sender, void* object_path, void* interface_name, void* method_name, void* invocation, void* user_data) {   \
    RunFunctionFmt(my_context, fct_funcs_method_call_##A, "pppppppp", connection, sender, object_path, interface_name, method_name, invocation, user_data);             \
}   \
static uintptr_t fct_funcs_get_property_##A = 0;                        \
static void* my_funcs_get_property_##A(void* connection, void* sender, void* object_path, void* interface_name, void* error, void* user_data) {                     \
    return (void*)RunFunctionFmt(my_context, fct_funcs_get_property_##A, "ppppppp", connection, sender, object_path, interface_name, error, user_data);             \
}   \
static uintptr_t fct_funcs_set_property_##A = 0;                        \
static int my_funcs_set_property_##A(void* connection, void* sender, void* object_path, void* interface_name, void* value, void* error, void* user_data) {          \
    return (int)RunFunctionFmt(my_context, fct_funcs_set_property_##A, "pppppppp", connection, sender, object_path, interface_name, value, error, user_data);       \
}

SUPER()
#undef GO
// and now the get slot / assign... Taking into account that the desired callback may already be a wrapped one (so unwrapping it)
static my_GDBusInterfaceVTable_t* findFreeGDBusInterfaceVTable(my_GDBusInterfaceVTable_t* fcts)
{
    if(!fcts) return fcts;
    #define GO(A) if(ref_GDBusInterfaceVTable_##A == fcts) return &my_GDBusInterfaceVTable_##A;
    SUPER()
    #undef GO
    #define GO(A) if(ref_GDBusInterfaceVTable_##A == 0) {   \
        ref_GDBusInterfaceVTable_##A = fcts;                 \
        my_GDBusInterfaceVTable_##A.method_call = (fcts->method_call)?((GetNativeFnc((uintptr_t)fcts->method_call))?GetNativeFnc((uintptr_t)fcts->method_call):(void*)(uintptr_t)my_funcs_method_call_##A):NULL;    \
        fct_funcs_method_call_##A = (uintptr_t)fcts->method_call;                            \
        my_GDBusInterfaceVTable_##A.get_property = (fcts->get_property)?((GetNativeFnc((uintptr_t)fcts->get_property))?GetNativeFnc((uintptr_t)fcts->get_property):(void*)(uintptr_t)my_funcs_get_property_##A):NULL;    \
        fct_funcs_get_property_##A = (uintptr_t)fcts->get_property;                            \
        my_GDBusInterfaceVTable_##A.set_property = (fcts->set_property)?((GetNativeFnc((uintptr_t)fcts->set_property))?GetNativeFnc((uintptr_t)fcts->set_property):(void*)(uintptr_t)my_funcs_set_property_##A):NULL;    \
        fct_funcs_set_property_##A = (uintptr_t)fcts->set_property;                            \
        return &my_GDBusInterfaceVTable_##A;                \
    }
    SUPER()
    #undef GO
    printf_log(LOG_NONE, "Warning, no more slot for gio2 GDBusInterfaceVTable callback\n");
    return NULL;
}
#undef SUPER

EXPORT void my_g_dbus_proxy_new(x86emu_t* emu, void* connection, int flags, void* info, void* name, void* path, void* interface, void* cancellable, void* cb, void* data)
{
    (void)emu;
    my->g_dbus_proxy_new(connection, flags, info, name, path, interface, cancellable, findGAsyncReadyCallbackFct(cb), data);
}

EXPORT void my_g_dbus_proxy_new_for_bus(x86emu_t* emu, int bus_type, int flags, void* info, void* name, void* path, void* interface, void* cancellable, void* cb, void* data)
{
    (void)emu;
    my->g_dbus_proxy_new_for_bus(bus_type, flags, info, name, path, interface, cancellable, findGAsyncReadyCallbackFct(cb), data);
}

EXPORT void my_g_dbus_proxy_call(x86emu_t* emu, void* proxy, void* name, void* param, int flags, int timeout, void* cancellable, void* cb, void* data)
{
    (void)emu;
    my->g_dbus_proxy_call(proxy, name, param, flags, timeout, cancellable, findGAsyncReadyCallbackFct(cb), data);
}

EXPORT void my_g_dbus_proxy_call_with_unix_fd_list(x86emu_t* emu, void* proxy, void* name, void* param, int flags, int timeout, void* fd_list, void* cancellable, void* cb, void* data)
{
    (void)emu;
    my->g_dbus_proxy_call_with_unix_fd_list(proxy, name, param, flags, timeout, fd_list, cancellable, findGAsyncReadyCallbackFct(cb), data);
}

EXPORT void* my_g_dbus_object_manager_client_new_for_bus_sync(x86emu_t* emu, int bus, int flags, void* name, void* path, void* cb, void* data, void* destroy, void* cancellable, void* error)
{
    (void)emu;
    return my->g_dbus_object_manager_client_new_for_bus_sync(bus, flags, name, path, findGDBusProxyTypeFuncFct(cb), data, findGDestroyNotifyFct(destroy), cancellable, error);
}

EXPORT void* my_g_simple_async_result_new(x86emu_t* emu, void* source, void* cb, void* data, void* tag)
{
    (void)emu;
    return my->g_simple_async_result_new(source, findGAsyncReadyCallbackFct(cb), data, tag);
}

EXPORT void* my_g_simple_async_result_new_error(x86emu_t* emu, void* source, void* cb, void* data, uint32_t domain, int code, void* fmt, va_list b)
{
    (void)emu;
    char* tmp;
    int dummy = vasprintf(&tmp, fmt, b);
    (void)dummy;
    void* ret = my->g_simple_async_result_new_error(source, findGAsyncReadyCallbackFct(cb), data, domain, code, tmp);
    free(tmp);
    return ret;
}

EXPORT void* my_g_simple_async_result_new_from_error(x86emu_t* emu, void* source, void* cb, void* data, void* error)
{
    (void)emu;
    return my->g_simple_async_result_new_from_error(source, findGAsyncReadyCallbackFct(cb), data, error);
}

EXPORT void* my_g_simple_async_result_new_take_error(x86emu_t* emu, void* source, void* cb, void* data, void* error)
{
    (void)emu;
    return my->g_simple_async_result_new_take_error(source, findGAsyncReadyCallbackFct(cb), data, error);
}

EXPORT void my_g_simple_async_result_set_op_res_gpointer(x86emu_t* emu, void* simple, void* op, void* notify)
{
    (void)emu;
    my->g_simple_async_result_set_op_res_gpointer(simple, op, findGDestroyNotifyFct(notify));
}

EXPORT void my_g_simple_async_result_run_in_thread(x86emu_t* emu, void* simple, void* fnc, int priority, void* cancellable)
{
    (void)emu;
    return my->g_simple_async_result_run_in_thread(simple, findGSimpleAsyncThreadFuncFct(fnc), priority, cancellable);
}

EXPORT void my_g_simple_async_report_error_in_idle(x86emu_t* emu, void* object, void* cb, void* data, uint32_t domain, int code, void* fmt, va_list b)
{
    (void)emu;
    char* tmp;
    int dummy = vasprintf(&tmp, fmt, b);
    (void)dummy;
    my->g_simple_async_report_error_in_idle(object, findGAsyncReadyCallbackFct(cb), data, domain, code, tmp);
    free(tmp);
}

EXPORT void my_g_simple_async_report_gerror_in_idle(x86emu_t* emu, void* object, void* cb, void* data, void* error)
{
    (void)emu;
    my->g_simple_async_report_gerror_in_idle(object, findGAsyncReadyCallbackFct(cb), data, error);
}

EXPORT void my_g_simple_async_report_take_gerror_in_idle(x86emu_t* emu, void* object, void* cb, void* data, void* error)
{
    (void)emu;
    my->g_simple_async_report_take_gerror_in_idle(object, findGAsyncReadyCallbackFct(cb), data, error);
}

EXPORT unsigned long my_g_cancellable_connect(x86emu_t* emu, void* cancellable, void* cb, void* data, void* notify)
{
    (void)emu;
    return my->g_cancellable_connect(cancellable, findGCallbackFct(cb), data, findGDestroyNotifyFct(notify));
}

EXPORT void my_g_async_initable_init_async(x86emu_t* emu, void* initable, int priority, void* cancellable, void* cb, void* data)
{
    (void)emu;
    my->g_async_initable_init_async(initable, priority, cancellable, findGAsyncReadyCallbackFct(cb), data);
}

EXPORT void my_g_async_initable_new_valist_async(x86emu_t* emu, int type, void* first, void* var_args, int priority, void* cancellable, void* cb, void* data)
{
    (void)emu;
    my->g_async_initable_new_valist_async(type, first, var_args, priority, cancellable, findGAsyncReadyCallbackFct(cb), data);
}

EXPORT void my_g_async_initable_new_async(x86emu_t* emu, int type, int priority, void* cancellable, void* cb, void* data, void* first, void* b)
{
    my_g_async_initable_new_valist_async(emu, type, first, b, priority, cancellable, cb, data);
}

EXPORT void my_g_async_initable_newv_async(x86emu_t* emu, int type, uint32_t n, void* params, int priority, void* cancellable, void* cb, void* data)
{
    (void)emu;
    my->g_async_initable_newv_async(type, n, params, priority, cancellable, findGAsyncReadyCallbackFct(cb), data);
}

EXPORT void my_g_bus_get(x86emu_t* emu, int type, void* cancellable, void* cb, void* data)
{
    (void)emu;
    my->g_bus_get(type, cancellable, findGAsyncReadyCallbackFct(cb), data);
}

EXPORT void my_g_dbus_connection_new(x86emu_t* emu, void* stream, void* guid, int flags, void* observer, void* cancellable, void* cb, void* data)
{
    (void)emu;
    my->g_dbus_connection_new(stream, guid, flags, observer, cancellable, findGAsyncReadyCallbackFct(cb), data);
}

EXPORT void my_g_dbus_connection_new_for_address(x86emu_t* emu, void* address, int flags, void* observer, void* cancellable, void* cb, void* data)
{
    (void)emu;
    my->g_dbus_connection_new_for_address(address, flags, observer, cancellable, findGAsyncReadyCallbackFct(cb), data);
}

EXPORT void my_g_dbus_connection_close(x86emu_t* emu, void* connection, void* cancellable, void* cb, void* data)
{
    (void)emu;
    my->g_dbus_connection_close(connection, cancellable, findGAsyncReadyCallbackFct(cb), data);
}

EXPORT void my_g_dbus_connection_flush(x86emu_t* emu, void* connection, void* cancellable, void* cb, void* data)
{
    (void)emu;
    my->g_dbus_connection_flush(connection, cancellable, findGAsyncReadyCallbackFct(cb), data);
}

EXPORT void my_g_dbus_connection_call(x86emu_t* emu, void* connection, void* bus, void* object, void* interface, void* method, void* param, void* reply, int flags, int timeout, void* cancellable, void* cb, void* data)
{
    (void)emu;
    my->g_dbus_connection_call(connection, bus, object, interface, method, param, reply, flags, timeout, cancellable, findGAsyncReadyCallbackFct(cb), data);
}

EXPORT uint32_t my_g_dbus_connection_signal_subscribe(x86emu_t* emu, void* connection, void* sender, void* interface, void* member, void* object, void* arg0, int flags, void* cb, void* data, void* notify)
{
    (void)emu;
    return my->g_dbus_connection_signal_subscribe(connection, sender, interface, member, object, arg0, flags, findGDBusSignalCallbackFct(cb), data, findGDestroyNotifyFct(notify));
}

EXPORT void my_g_dbus_connection_send_message_with_reply(x86emu_t* emu, void* connection, void* message, int flags, int timeout, void* serial, void* cancellable, void* cb, void* data)
{
    (void)emu;
    my->g_dbus_connection_send_message_with_reply(connection, message, flags, timeout, serial, cancellable, findGAsyncReadyCallbackFct(cb), data);
}

EXPORT uint32_t my_g_dbus_connection_add_filter(x86emu_t* emu, void* connection, void* cb, void* data, void* notify)
{
    (void)emu;
    return my->g_dbus_connection_add_filter(connection, findGDBusMessageFilterFunctionFct(cb), data, findGDestroyNotifyFct(notify));
}

EXPORT uint32_t my_g_dbus_connection_register_object(x86emu_t* emu, void* connection, void* object, void* info, my_GDBusInterfaceVTable_t* vtable, void* data, void* notify, void* error)
{
    (void)emu;
    return my->g_dbus_connection_register_object(connection, object, info, findFreeGDBusInterfaceVTable(vtable), data, findGDestroyNotifyFct(notify), error);
}

EXPORT uint32_t my_g_bus_watch_name(x86emu_t* emu, int type, void* name, int flags, void* appeared, void* vanished, void* data, void* notify)
{
    (void)emu;
    // note that a mecanism with a new callback, cleaned with notify, is also possible here
    return my->g_bus_watch_name(type, name, flags, findGBusNameAppearedCallbackFct(appeared), findGBusNameVanishedCallbackFct(vanished), data, findGDestroyNotifyFct(notify));
}

EXPORT uint32_t my_g_bus_watch_name_on_connection(x86emu_t* emu, void* connection, void* name, int flags, void* appeared, void* vanished, void* data, void* notify)
{
    (void)emu;
    // note that a mecanism with a new callback, cleaned with notify, is also possible here
    return my->g_bus_watch_name_on_connection(connection, name, flags, findGBusNameAppearedCallbackFct(appeared), findGBusNameVanishedCallbackFct(vanished), data, findGDestroyNotifyFct(notify));
}

EXPORT uint32_t my_g_bus_own_name(x86emu_t* emu, int type, void* name, int flags, void* bus_acquired, void* name_acquired, void* name_lost, void* data, void* notify)
{
    (void)emu;
    // note that a mecanism with a new callback, cleaned with notify, is also possible here
    return my->g_bus_own_name(type, name, flags, findGBusAcquiredCallbackFct(bus_acquired), findGBusNameAcquiredCallbackFct(name_acquired), findGBusNameLostCallbackFct(name_lost), data, findGDestroyNotifyFct(notify));
}

EXPORT uint32_t my_g_bus_own_name_on_connection(x86emu_t* emu, void* connection, void* name, int flags, void* name_acquired, void* name_lost, void* data, void* notify)
{
    (void)emu;
    // note that a mecanism with a new callback, cleaned with notify, is also possible here
    return my->g_bus_own_name_on_connection(connection, name, flags, findGBusNameAcquiredCallbackFct(name_acquired), findGBusNameLostCallbackFct(name_lost), data, findGDestroyNotifyFct(notify));
}

EXPORT void* my_g_initable_new(x86emu_t* emu, int type, void* cancellable, void* error, void* first, void* va)
{
    (void)emu;
    return my->g_initable_new_valist(type, first, va, cancellable, error);
}

EXPORT void* my_g_memory_input_stream_new_from_data(x86emu_t* emu, void* data, ssize_t size, void* d)
{
    (void)emu;
    return my->g_memory_input_stream_new_from_data(data, size, findGDestroyNotifyFct(d));
}

#define PRE_INIT    \
    if(box86_nogtk) \
        return -1;

#define CUSTOM_INIT \
    libname = lib->name;                                                        \
    getMy(lib);                                                                 \
    SetGDBusObjectManagerClientID(my->g_dbus_object_manager_client_get_type()); \
    setNeededLibs(lib, 1, "libgmodule-2.0.so.0");

#define CUSTOM_FINI \
    freeMy();

#include "wrappedlib_init.h"
