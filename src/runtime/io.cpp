/*
Copyright (c) 2018 Microsoft Corporation. All rights reserved.
Released under Apache 2.0 license as described in the file LICENSE.

Authors: Leonardo de Moura, Sebastian Ullrich
*/
#if defined(LEAN_WINDOWS)
#include <windows.h>
#include <io.h>
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#include <unistd.h>
#else
#if defined(LEAN_EMSCRIPTEN)
#include <emscripten.h>
#endif
// Linux include files
#include <unistd.h> // NOLINT
#include <sys/mman.h>
#endif
#ifndef LEAN_WINDOWS
#include <csignal>
#endif
#include <dirent.h>
#include <fcntl.h>
#include <iostream>
#include <chrono>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <string>
#include <cstdlib>
#include <cctype>
#include <sys/stat.h>
#include "util/io.h"
#include <lean/alloc.h>
#include <lean/io.h>
#include <lean/utf8.h>
#include <lean/object.h>
#include <lean/thread.h>
#include <lean/allocprof.h>

#ifdef _MSC_VER
#define S_ISDIR(mode) ((mode & _S_IFDIR) != 0)
#else
#include <dirent.h>
#endif

namespace lean {

extern "C" lean_object* lean_mk_io_error_already_exists(uint32_t, lean_object*);
extern "C" lean_object* lean_mk_io_error_eof(lean_object*);
extern "C" lean_object* lean_mk_io_error_hardware_fault(uint32_t, lean_object*);
extern "C" lean_object* lean_mk_io_error_illegal_operation(uint32_t, lean_object*);
extern "C" lean_object* lean_mk_io_error_inappropriate_type(uint32_t, lean_object*);
extern "C" lean_object* lean_mk_io_error_inappropriate_type_file(lean_object*, uint32_t, lean_object*);
extern "C" lean_object* lean_mk_io_error_interrupted(lean_object*, uint32_t, lean_object*);
extern "C" lean_object* lean_mk_io_error_invalid_argument(uint32_t, lean_object*);
extern "C" lean_object* lean_mk_io_error_invalid_argument_file(lean_object*, uint32_t, lean_object*);
extern "C" lean_object* lean_mk_io_error_no_file_or_directory(lean_object*, uint32_t, lean_object*);
extern "C" lean_object* lean_mk_io_error_no_such_thing(uint32_t, lean_object*);
extern "C" lean_object* lean_mk_io_error_no_such_thing_file(lean_object*, uint32_t, lean_object*);
extern "C" lean_object* lean_mk_io_error_other_error(uint32_t, lean_object*);
extern "C" lean_object* lean_mk_io_error_permission_denied(uint32_t, lean_object*);
extern "C" lean_object* lean_mk_io_error_permission_denied_file(lean_object*, uint32_t, lean_object*);
extern "C" lean_object* lean_mk_io_error_protocol_error(uint32_t, lean_object*);
extern "C" lean_object* lean_mk_io_error_resource_busy(uint32_t, lean_object*);
extern "C" lean_object* lean_mk_io_error_resource_exhausted(uint32_t, lean_object*);
extern "C" lean_object* lean_mk_io_error_resource_exhausted_file(lean_object*, uint32_t, lean_object*);
extern "C" lean_object* lean_mk_io_error_resource_vanished(uint32_t, lean_object*);
extern "C" lean_object* lean_mk_io_error_time_expired(uint32_t, lean_object*);
extern "C" lean_object* lean_mk_io_error_unsatisfied_constraints(uint32_t, lean_object*);
extern "C" lean_object* lean_mk_io_error_unsupported_operation(uint32_t, lean_object*);


extern "C" void lean_io_result_show_error(b_obj_arg r) {
    object * err = io_result_get_error(r);
    inc_ref(err);
    object * str = lean_io_error_to_string(err);
    std::cerr << "uncaught exception: " << string_cstr(str) << std::endl;
    dec_ref(str);
}

extern "C" object * mk_io_user_error(object * str);

obj_res io_result_mk_error(char const * msg) {
    return io_result_mk_error(mk_io_user_error(mk_string(msg)));
}

obj_res io_result_mk_error(std::string const & msg) {
    return io_result_mk_error(mk_io_user_error(mk_string(msg)));
}

static bool g_initializing = true;
extern "C" void lean_io_mark_end_initialization() {
    g_initializing = false;
}
extern "C" obj_res lean_io_initializing(obj_arg) {
    return io_result_mk_ok(box(g_initializing));
}

static obj_res mk_file_not_found_error(b_obj_arg fname) {
    inc(fname);
    int errnum = ENOENT;
    object * details = mk_string("");
    return io_result_mk_error(lean_mk_io_error_no_file_or_directory(fname, errnum, details));
}

static lean_external_class * g_io_handle_external_class = nullptr;

static void io_handle_finalizer(void * h) {
    fclose(static_cast<FILE *>(h));
}

static void io_handle_foreach(void * /* mod */, b_obj_arg /* fn */) {
}

lean_object * io_wrap_handle(FILE *hfile) {
    return lean_alloc_external(g_io_handle_external_class, hfile);
}

extern "C" obj_res lean_stream_of_handle(obj_arg h);

static object * g_stream_stdin  = nullptr;
static object * g_stream_stdout = nullptr;
static object * g_stream_stderr = nullptr;
MK_THREAD_LOCAL_GET(object_ref, get_stream_current_stdin,  g_stream_stdin);
MK_THREAD_LOCAL_GET(object_ref, get_stream_current_stdout, g_stream_stdout);
MK_THREAD_LOCAL_GET(object_ref, get_stream_current_stderr, g_stream_stderr);

/* getStdin : IO FS.Stream */
extern "C" obj_res lean_get_stdin(obj_arg /* w */) {
    return io_result_mk_ok(get_stream_current_stdin().to_obj_arg());
}

/* getStdout : IO FS.Stream */
extern "C" obj_res lean_get_stdout(obj_arg /* w */) {
    return io_result_mk_ok(get_stream_current_stdout().to_obj_arg());
}

/* getStderr : IO FS.Stream */
extern "C" obj_res lean_get_stderr(obj_arg /* w */) {
    return io_result_mk_ok(get_stream_current_stderr().to_obj_arg());
}

/* setStdin  : FS.Stream -> IO FS.Stream */
extern "C" obj_res lean_get_set_stdin(obj_arg h, obj_arg /* w */) {
    object_ref & x = get_stream_current_stdin();
    object * r = x.steal();
    x = object_ref(h);
    return io_result_mk_ok(r);
}

/* setStdout  : FS.Stream -> IO FS.Stream */
extern "C" obj_res lean_get_set_stdout(obj_arg h, obj_arg /* w */) {
    object_ref & x = get_stream_current_stdout();
    object * r = x.steal();
    x = object_ref(h);
    return io_result_mk_ok(r);
}

/* setStderr  : FS.Stream -> IO FS.Stream */
extern "C" obj_res lean_get_set_stderr(obj_arg h, obj_arg /* w */) {
    object_ref & x = get_stream_current_stderr();
    object * r = x.steal();
    x = object_ref(h);
    return io_result_mk_ok(r);
}

static FILE * io_get_handle(lean_object * hfile) {
    return static_cast<FILE *>(lean_get_external_data(hfile));
}

obj_res decode_io_error(int errnum, b_obj_arg fname) {
    object * details = mk_string(strerror(errnum));
    switch (errnum) {
    case EINTR:
        lean_assert(fname != nullptr);
        inc_ref(fname);
        return lean_mk_io_error_interrupted(fname, errnum, details);
    case ELOOP: case ENAMETOOLONG: case EDESTADDRREQ:
    case EBADF: case EDOM: case EINVAL: case EILSEQ:
    case ENOEXEC: case ENOSTR: case ENOTCONN:
    case ENOTSOCK:
        if (fname == nullptr) {
            return lean_mk_io_error_invalid_argument(errnum, details);
        } else {
            inc_ref(fname);
            return lean_mk_io_error_invalid_argument_file(fname, errnum, details);
        }
    case ENOENT:
        lean_assert(fname != nullptr);
        inc_ref(fname);
        return lean_mk_io_error_no_file_or_directory(fname, errnum, details);
    case EACCES: case EROFS: case ECONNABORTED: case EFBIG:
    case EPERM:
        if (fname == nullptr) {
            return lean_mk_io_error_permission_denied(errnum, details);
        } else {
            inc_ref(fname);
            return lean_mk_io_error_permission_denied_file(fname, errnum, details);
        }
    case EMFILE: case ENFILE: case ENOSPC:
    case E2BIG:  case EAGAIN: case EMLINK:
    case EMSGSIZE: case ENOBUFS: case ENOLCK:
    case ENOMEM: case ENOSR:
        if (fname == nullptr) {
            return lean_mk_io_error_resource_exhausted(errnum, details);
        } else {
            inc_ref(fname);
            return lean_mk_io_error_resource_exhausted_file(fname, errnum, details);
        }
    case EISDIR: case EBADMSG: case ENOTDIR:
        if (fname == nullptr) {
            return lean_mk_io_error_inappropriate_type(errnum, details);
        } else {
            inc_ref(fname);
            return lean_mk_io_error_inappropriate_type_file(fname, errnum, details);
        }
    case ENXIO: case EHOSTUNREACH: case ENETUNREACH:
    case ECHILD: case ECONNREFUSED: case ENODATA:
    case ENOMSG: case ESRCH:
        if (fname == nullptr) {
            return lean_mk_io_error_no_such_thing(errnum, details);
        } else {
            inc_ref(fname);
            return lean_mk_io_error_no_such_thing_file(fname, errnum, details);
        }
    case EEXIST: case EINPROGRESS: case EISCONN:
        lean_assert(fname == nullptr);
        return lean_mk_io_error_already_exists(errnum, details);
    case EIO:
        lean_assert(fname == nullptr);
        return lean_mk_io_error_hardware_fault(errnum, details);
    case ENOTEMPTY:
        lean_assert(fname == nullptr);
        return lean_mk_io_error_unsatisfied_constraints(errnum, details);
    case ENOTTY:
        lean_assert(fname == nullptr);
        return lean_mk_io_error_illegal_operation(errnum, details);
    case ECONNRESET: case EIDRM: case ENETDOWN: case ENETRESET:
    case ENOLINK: case EPIPE:
        lean_assert(fname == nullptr);
        return lean_mk_io_error_resource_vanished(errnum, details);
    case EPROTO: case EPROTONOSUPPORT: case EPROTOTYPE:
        lean_assert(fname == nullptr);
        return lean_mk_io_error_protocol_error(errnum, details);
    case ETIME: case ETIMEDOUT:
        lean_assert(fname == nullptr);
        return lean_mk_io_error_time_expired(errnum, details);
    case EADDRINUSE: case EBUSY: case EDEADLK: case ETXTBSY:
        lean_assert(fname == nullptr);
        return lean_mk_io_error_resource_busy(errnum, details);
    case EADDRNOTAVAIL: case EAFNOSUPPORT: case ENODEV:
    case ENOPROTOOPT: case ENOSYS: case EOPNOTSUPP:
    case ERANGE: case ESPIPE: case EXDEV:
        lean_assert(fname == nullptr);
        return lean_mk_io_error_unsupported_operation(errnum, details);
    case EFAULT:
    default:
        lean_assert(fname == nullptr);
        return lean_mk_io_error_other_error(errnum, details);
    }
}

/* IO.setAccessRights (filename : @& String) (mode : UInt32) : IO Handle */
extern "C" obj_res lean_chmod (b_obj_arg filename, uint32_t mode, obj_arg /* w */) {
    if (!chmod(lean_string_cstr(filename), mode)) {
        return io_result_mk_ok(box(0));
    } else {
        return io_result_mk_error(decode_io_error(errno, filename));
    }
}

/* Handle.mk (filename : @& String) (mode : @& String) : IO Handle */
extern "C" obj_res lean_io_prim_handle_mk(b_obj_arg filename, b_obj_arg modeStr, obj_arg /* w */) {
    FILE *fp = fopen(lean_string_cstr(filename), lean_string_cstr(modeStr));
    if (!fp) {
        return io_result_mk_error(decode_io_error(errno, filename));
    } else {
        return io_result_mk_ok(io_wrap_handle(fp));
    }
}

/* Handle.isEof : (@& Handle) → IO Bool */
extern "C" obj_res lean_io_prim_handle_is_eof(b_obj_arg h, obj_arg /* w */) {
    FILE * fp = io_get_handle(h);
    return io_result_mk_ok(box(std::feof(fp) != 0));
}

/* Handle.flush : (@& Handle) → IO Bool */
extern "C" obj_res lean_io_prim_handle_flush(b_obj_arg h, obj_arg /* w */) {
    FILE * fp = io_get_handle(h);
    if (!std::fflush(fp)) {
        return io_result_mk_ok(box(0));
    } else {
        return io_result_mk_error(decode_io_error(errno, nullptr));
    }
}

/* Handle.read : (@& Handle) → USize → IO ByteArray */
extern "C" obj_res lean_io_prim_handle_read(b_obj_arg h, usize nbytes, obj_arg /* w */) {
    FILE * fp = io_get_handle(h);
    if (feof(fp)) {
        return io_result_mk_ok(alloc_sarray(1, 0, 0));
    }
    obj_res res = lean_alloc_sarray(1, 0, nbytes);
    usize n = std::fread(lean_sarray_cptr(res), 1, nbytes, fp);
    if (n > 0) {
        lean_sarray_set_size(res, n);
        return io_result_mk_ok(res);
    } else if (feof(fp)) {
        dec_ref(res);
        return io_result_mk_ok(alloc_sarray(1, 0, 0));
    } else {
        dec_ref(res);
        return io_result_mk_error(decode_io_error(errno, nullptr));
    }
}

/* Handle.write : (@& Handle) → (@& ByteArray) → IO unit */
extern "C" obj_res lean_io_prim_handle_write(b_obj_arg h, b_obj_arg buf, obj_arg /* w */) {
    FILE * fp = io_get_handle(h);
    usize n = lean_sarray_size(buf);
    usize m = std::fwrite(lean_sarray_cptr(buf), 1, n, fp);
    if (m == n) {
        return io_result_mk_ok(box(0));
    } else {
        return io_result_mk_error(decode_io_error(errno, nullptr));
    }
}

static object * g_io_error_getline = nullptr;

/*
  Handle.getLine : (@& Handle) → IO Unit
  The line returned by `lean_io_prim_handle_get_line`
  is truncated at the first '\0' character and the
  rest of the line is discarded. */
extern "C" obj_res lean_io_prim_handle_get_line(b_obj_arg h, obj_arg /* w */) {
    FILE * fp = io_get_handle(h);
    if (feof(fp)) {
        return io_result_mk_ok(mk_string(""));
    }
    const int buf_sz = 64;
    char buf_str[buf_sz]; // NOLINT
    std::string result;
    bool first = true;
    while (true) {
        char * out = std::fgets(buf_str, buf_sz, fp);
        if (out != nullptr) {
            if (strlen(buf_str) < buf_sz-1 || buf_str[buf_sz-2] == '\n') {
                if (first) {
                    return io_result_mk_ok(mk_string(out));
                } else {
                    result.append(out);
                    return io_result_mk_ok(mk_string(result));
                }
            }
            result.append(out);
        } else if (std::feof(fp)) {
            return io_result_mk_ok(mk_string(result));
        } else {
            return io_result_mk_error(g_io_error_getline);
        }
        first = false;
    }
}

/* Handle.putStr : (@& Handle) → (@& String) → IO Unit */
extern "C" obj_res lean_io_prim_handle_put_str(b_obj_arg h, b_obj_arg s, obj_arg /* w */) {
    FILE * fp = io_get_handle(h);
    if (std::fputs(lean_string_cstr(s), fp) != EOF) {
        return io_result_mk_ok(box(0));
    } else {
        return io_result_mk_error(decode_io_error(errno, nullptr));
    }
}

/* monoMsNow : IO Nat */
extern "C" obj_res lean_io_mono_ms_now(obj_arg /* w */) {
    auto now = std::chrono::steady_clock::now();
    auto tm = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    return io_result_mk_ok(usize_to_nat(tm.count()));
}

/* timeit {α : Type} (msg : @& String) (fn : IO α) : IO α */
extern "C" obj_res lean_io_timeit(b_obj_arg msg, obj_arg fn, obj_arg w) {
    auto start = std::chrono::steady_clock::now();
    w = apply_1(fn, w);
    auto end   = std::chrono::steady_clock::now();
    auto diff  = std::chrono::duration<double>(end - start);
    std::ostream & out = std::cerr; // TODO(Leo): replace?
    out << std::setprecision(3);
    if (diff < std::chrono::duration<double>(1)) {
        out << string_cstr(msg) << " " << std::chrono::duration<double, std::milli>(diff).count() << "ms\n";
    } else {
        out << string_cstr(msg) << " " << diff.count() << "s\n";
    }
    return w;
}

/* allocprof {α : Type} (msg : @& String) (fn : IO α) : IO α */
extern "C" obj_res lean_io_allocprof(b_obj_arg msg, obj_arg fn, obj_arg w) {
    std::ostream & out = std::cerr; // TODO(Leo): replace?
    allocprof prof(out, string_cstr(msg));
    return apply_1(fn, w);
}

/* getNumHeartbeats : IO Nat */
extern "C" obj_res lean_io_get_num_heartbeats(obj_arg /* w */) {
    return io_result_mk_ok(lean_uint64_to_nat(get_num_heartbeats()));
}

extern "C" obj_res lean_io_getenv(b_obj_arg env_var, obj_arg) {
#if defined(LEAN_EMSCRIPTEN)
    // HACK(WN): getenv doesn't seem to work in Emscripten even though it should
    // see https://emscripten.org/docs/porting/connecting_cpp_and_javascript/Interacting-with-code.html#interacting-with-code-environment-variables
    char* val = reinterpret_cast<char*>(EM_ASM_INT({
        var envVar = UTF8ToString($0);
        var val = ENV[envVar];
        if (val) {
            var lengthBytes = lengthBytesUTF8(val)+1;
            var valOnWasmHeap = _malloc(lengthBytes);
            stringToUTF8(val, valOnWasmHeap, lengthBytes);
            return valOnWasmHeap;
        } else {
            return 0;
        }
    }, string_cstr(env_var)));

    if (val) {
        object * valLean = mk_string(val);
        free(val);
        return io_result_mk_ok(mk_option_some(valLean));
    } else {
        return io_result_mk_ok(mk_option_none());
    }
#else
    char * val = std::getenv(string_cstr(env_var));
    if (val) {
        return io_result_mk_ok(mk_option_some(mk_string(val)));
    } else {
        return io_result_mk_ok(mk_option_none());
    }
#endif
}

extern "C" obj_res lean_io_realpath(obj_arg fname, obj_arg) {
#if defined(LEAN_WINDOWS)
    constexpr unsigned BufferSize = 8192;
    char buffer[BufferSize];
    DWORD retval = GetFullPathName(string_cstr(fname), BufferSize, buffer, nullptr);
    if (retval == 0 || retval > BufferSize) {
        return io_result_mk_ok(fname);
    } else {
        dec_ref(fname);
        // Hack for making sure disk is lower case
        // TODO(Leo): more robust solution
        if (strlen(buffer) >= 2 && buffer[1] == ':') {
            buffer[0] = tolower(buffer[0]);
        }
        return io_result_mk_ok(mk_string(buffer));
    }
#else
    char buffer[PATH_MAX];
    char * tmp = realpath(string_cstr(fname), buffer);
    if (tmp) {
        obj_res s = mk_string(tmp);
        dec_ref(fname);
        return io_result_mk_ok(s);
    } else {
        obj_res res = mk_file_not_found_error(fname);
        dec_ref(fname);
        return res;
    }
#endif
}

/*
structure DirEntry where
  root     : String
  filename : String

constant readDir : @& FilePath → IO (Array DirEntry)
*/
extern "C" obj_res lean_io_read_dir(b_obj_arg dirname, obj_arg) {
    object * arr = array_mk_empty();
    DIR * dp = opendir(string_cstr(dirname));
    if (!dp) {
        return io_result_mk_error(decode_io_error(errno, dirname));
    }
    while (dirent * entry = readdir(dp)) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        object * lentry = alloc_cnstr(0, 2, 0);
        lean_inc(dirname);
        cnstr_set(lentry, 0, dirname);
        cnstr_set(lentry, 1, lean_mk_string(entry->d_name));
        arr = lean_array_push(arr, lentry);
    }
    lean_always_assert(closedir(dp) == 0);
    return io_result_mk_ok(arr);
}

/*
inductive FileType where
  | dir
  | file
  | symlink
  | other

structure SystemTime where
  sec  : Int
  nsec : UInt32

structure Metadata where
  --permissions : ...
  accessed : SystemTime
  modified : SystemTime
  byteSize : UInt64
  type     : FileType

constant metadata : @& FilePath → IO IO.FS.Metadata
*/
static obj_res timespec_to_obj(timespec const & ts) {
    object * o = alloc_cnstr(0, 1, sizeof(uint32));
    cnstr_set(o, 0, lean_int64_to_int(ts.tv_sec));
    cnstr_set_uint32(o, sizeof(object *), ts.tv_nsec);
    return o;
}

extern "C" obj_res lean_io_metadata(b_obj_arg fname, obj_arg) {
    struct stat st;
    if (stat(string_cstr(fname), &st) != 0) {
        return io_result_mk_error(decode_io_error(errno, fname));
    }
    object * mdata = alloc_cnstr(0, 2, sizeof(uint64) + sizeof(uint8));
#ifdef __APPLE__
    cnstr_set(mdata, 0, timespec_to_obj(st.st_atimespec));
    cnstr_set(mdata, 1, timespec_to_obj(st.st_mtimespec));
#elif defined(LEAN_WINDOWS)
    // TOOD: sub-second precision on Windows
    cnstr_set(mdata, 0, timespec_to_obj(timespec { st.st_atime, 0 }));
    cnstr_set(mdata, 1, timespec_to_obj(timespec { st.st_mtime, 0 }));
#else
    cnstr_set(mdata, 0, timespec_to_obj(st.st_atim));
    cnstr_set(mdata, 1, timespec_to_obj(st.st_mtim));
#endif
    cnstr_set_uint64(mdata, 2 * sizeof(object *), st.st_size);
    cnstr_set_uint8(mdata, 2 * sizeof(object *) + sizeof(uint64),
                    S_ISDIR(st.st_mode) ? 0 :
                    S_ISREG(st.st_mode) ? 1 :
#ifndef LEAN_WINDOWS
                    S_ISLNK(st.st_mode) ? 2 :
#endif
                    3);
    return io_result_mk_ok(mdata);
}

extern "C" obj_res lean_io_create_dir(b_obj_arg p, obj_arg) {
#ifdef LEAN_WINDOWS
    if (mkdir(string_cstr(p)) == 0) {
#else
    if (mkdir(string_cstr(p), 0777) == 0) {
#endif
        return io_result_mk_ok(box(0));
    } else {
        return io_result_mk_error(decode_io_error(errno, p));
    }
}

extern "C" obj_res lean_io_remove_file(b_obj_arg fname, obj_arg) {
    if (std::remove(string_cstr(fname)) == 0) {
        return io_result_mk_ok(box(0));
    } else {
        return io_result_mk_error(decode_io_error(errno, nullptr));
    }
}

extern "C" obj_res lean_io_app_path(obj_arg) {
#if defined(LEAN_WINDOWS)
    HMODULE hModule = GetModuleHandleW(NULL);
    WCHAR path[MAX_PATH];
    GetModuleFileNameW(hModule, path, MAX_PATH);
    std::wstring pathwstr(path);
    std::string pathstr(pathwstr.begin(), pathwstr.end());
    // Hack for making sure disk is lower case
    // TODO(Leo): more robust solution
    if (pathstr.size() >= 2 && pathstr[1] == ':') {
        pathstr[0] = tolower(pathstr[0]);
    }
    return io_result_mk_ok(mk_string(pathstr));
#elif defined(__APPLE__)
    char buf1[PATH_MAX];
    char buf2[PATH_MAX];
    uint32_t bufsize = PATH_MAX;
    if (_NSGetExecutablePath(buf1, &bufsize) != 0)
        return io_result_mk_error("failed to locate application");
    if (!realpath(buf1, buf2))
        return io_result_mk_error("failed to resolve symbolic links when locating application");
    return io_result_mk_ok(mk_string(buf2));
#elif defined(LEAN_EMSCRIPTEN)
    // See https://emscripten.org/docs/api_reference/emscripten.h.html#c.EM_ASM_INT
    char* appPath = reinterpret_cast<char*>(EM_ASM_INT({
        if ((typeof process === "undefined") || (process.release.name !== "node")) {
            return 0;
        }

        var lengthBytes = lengthBytesUTF8(__filename)+1;
        var pathOnWasmHeap = _malloc(lengthBytes);
        stringToUTF8(__filename, pathOnWasmHeap, lengthBytes);
        return pathOnWasmHeap;
    }));
    if (appPath == nullptr) {
        return io_result_mk_error("no Lean executable file exists in WASM outside of Node.js");
    }

    object * appPathLean = mk_string(appPath);
    free(appPath);
    return io_result_mk_ok(appPathLean);
#else
    // Linux version
    char path[PATH_MAX];
    char dest[PATH_MAX];
    memset(dest, 0, PATH_MAX);
    pid_t pid = getpid();
    snprintf(path, PATH_MAX, "/proc/%d/exe", pid);
    if (readlink(path, dest, PATH_MAX) == -1) {
        return io_result_mk_error("failed to locate application");
    } else {
        return io_result_mk_ok(mk_string(dest));
    }
#endif
}

extern "C" obj_res lean_io_current_dir(obj_arg) {
    char buffer[PATH_MAX];
    char * cwd = getcwd(buffer, sizeof(buffer));
    if (cwd) {
        return io_result_mk_ok(mk_string(cwd));
    } else {
        return io_result_mk_error("failed to retrieve current working directory");
    }
}

// =======================================
// ST ref primitives
extern "C" obj_res lean_st_mk_ref(obj_arg a, obj_arg) {
    lean_ref_object * o = (lean_ref_object*)lean_alloc_small_object(sizeof(lean_ref_object));
    lean_set_st_header((lean_object*)o, LeanRef, 0);
    o->m_value = a;
    return io_result_mk_ok((lean_object*)o);
}

static object * g_io_error_nullptr_read = nullptr;

static inline atomic<object*> * mt_ref_val_addr(object * o) {
    return reinterpret_cast<atomic<object*> *>(&(lean_to_ref(o)->m_value));
}

/*
  Important: we have added support for initializing global constants
  at program startup. This feature is particularly useful for
  initializing `ST.Ref` values. Any `ST.Ref` value created during
  initialization will be marked as persistent. Thus, to make `ST.Ref`
  API thread-safe, we must treat persistent `ST.Ref` objects created
  during initialization as a multi-threaded object. Then, whenever we store
  a value `val` into a global `ST.Ref`, we have to mark `va`l as a multi-threaded
  object as we do for multi-threaded `ST.Ref`s. It makes sense since
  the global `ST.Ref` may be used to communicate data between threads.
*/
static inline bool ref_maybe_mt(b_obj_arg ref) { return lean_is_mt(ref) || lean_is_persistent(ref); }

extern "C" obj_res lean_st_ref_get(b_obj_arg ref, obj_arg) {
    if (ref_maybe_mt(ref)) {
        atomic<object *> * val_addr = mt_ref_val_addr(ref);
        while (true) {
            /*
              We cannot simply read `val` from the ref and `inc` it like in the `else` branch since someone else could
              write to the ref in between and remove the last owning reference to the object. Instead, we must take
              ownership of the RC token in the ref via `exchange`, duplicate it, then put one RC token back. */
            object * val = val_addr->exchange(nullptr);
            if (val != nullptr) {
                inc(val);
                object * tmp = val_addr->exchange(val);
                if (tmp != nullptr) {
                    /* this may happen if another thread wrote `ref` */
                    dec(tmp);
                }
                return io_result_mk_ok(val);
            }
        }
    } else {
        object * val = lean_to_ref(ref)->m_value;
        lean_assert(val != nullptr);
        inc(val);
        return io_result_mk_ok(val);
    }
}

extern "C" obj_res lean_st_ref_take(b_obj_arg ref, obj_arg) {
    if (ref_maybe_mt(ref)) {
        atomic<object *> * val_addr = mt_ref_val_addr(ref);
        while (true) {
            object * val = val_addr->exchange(nullptr);
            if (val != nullptr)
                return io_result_mk_ok(val);
        }
    } else {
        object * val = lean_to_ref(ref)->m_value;
        lean_assert(val != nullptr);
        lean_to_ref(ref)->m_value = nullptr;
        return io_result_mk_ok(val);
    }
}

static_assert(sizeof(atomic<unsigned short>) == sizeof(unsigned short), "`atomic<unsigned short>` and `unsigned short` must have the same size"); // NOLINT

extern "C" obj_res lean_st_ref_set(b_obj_arg ref, obj_arg a, obj_arg) {
    if (ref_maybe_mt(ref)) {
        /* We must mark `a` as multi-threaded if `ref` is marked as multi-threaded.
           Reason: our runtime relies on the fact that a single-threaded object
           cannot be reached from a multi-thread object. */
        mark_mt(a);
        atomic<object *> * val_addr = mt_ref_val_addr(ref);
        object * old_a = val_addr->exchange(a);
        if (old_a != nullptr)
            dec(old_a);
        return io_result_mk_ok(box(0));
    } else {
        if (lean_to_ref(ref)->m_value != nullptr)
            dec(lean_to_ref(ref)->m_value);
        lean_to_ref(ref)->m_value = a;
        return io_result_mk_ok(box(0));
    }
}

extern "C" obj_res lean_st_ref_swap(b_obj_arg ref, obj_arg a, obj_arg) {
    if (ref_maybe_mt(ref)) {
        /* See io_ref_write */
        mark_mt(a);
        atomic<object *> * val_addr = mt_ref_val_addr(ref);
        while (true) {
            object * old_a = val_addr->exchange(a);
            if (old_a != nullptr)
                return io_result_mk_ok(old_a);
        }
    } else {
        object * old_a = lean_to_ref(ref)->m_value;
        if (old_a == nullptr)
            return io_result_mk_error(g_io_error_nullptr_read);
        lean_to_ref(ref)->m_value = a;
        return io_result_mk_ok(old_a);
    }
}

extern "C" obj_res lean_st_ref_ptr_eq(b_obj_arg ref1, b_obj_arg ref2, obj_arg) {
    // TODO(Leo): ref_maybe_mt
    bool r = lean_to_ref(ref1)->m_value == lean_to_ref(ref2)->m_value;
    return io_result_mk_ok(box(r));
}

static obj_res lean_io_as_task_fn(obj_arg act, obj_arg) {
    object_ref r(apply_1(act, io_mk_world()));
    if (io_result_is_ok(r.raw())) {
        return mk_except_ok(object_ref(io_result_get_value(r.raw()), true));
    } else {
        return mk_except_error(object_ref(io_result_get_error(r.raw()), true));
    }
}

/* asTask {α : Type} (act : IO α) (prio : Nat) : IO (Task (Except IO.Error α)) */
extern "C" obj_res lean_io_as_task(obj_arg act, obj_arg prio, obj_arg) {
    object * c = lean_alloc_closure((void*)lean_io_as_task_fn, 2, 1);
    lean_closure_set(c, 0, act);
    object * t = lean_task_spawn_core(c, lean_unbox(prio), /* keep_alive */ true);
    return io_result_mk_ok(t);
}

static obj_res lean_io_map_task_fn(obj_arg f, obj_arg a) {
    object_ref r(apply_2(f, a, io_mk_world()));
    if (io_result_is_ok(r.raw())) {
        return mk_except_ok(object_ref(io_result_get_value(r.raw()), true));
    } else {
        return mk_except_error(object_ref(io_result_get_error(r.raw()), true));
    }
}

/*  mapTask {α β : Type} (f : α → IO β) (t : Task α) (prio : Nat) : IO (Task (Except IO.Error β)) */
extern "C" obj_res lean_io_map_task(obj_arg f, obj_arg t, obj_arg prio, obj_arg) {
    object * c = lean_alloc_closure((void*)lean_io_map_task_fn, 2, 1);
    lean_closure_set(c, 0, f);
    object * t2 = lean_task_map_core(c, t, lean_unbox(prio), /* keep_alive */ true);
    return io_result_mk_ok(t2);
}

static obj_res lean_io_bind_task_fn(obj_arg f, obj_arg a) {
    object_ref r(apply_2(f, a, io_mk_world()));
    if (io_result_is_ok(r.raw())) {
        return object_ref(io_result_get_value(r.raw()), true).steal();
    } else {
        return task_pure(mk_except_error(object_ref(io_result_get_error(r.raw()), true)));
    }
}

/*  bindTask {α β : Type} (t : Task α) (f : α → IO (Task (Except IO.Error β))) (prio : Nat) : IO (Task (Except IO.Error β)) */
extern "C" obj_res lean_io_bind_task(obj_arg t, obj_arg f, obj_arg prio, obj_arg) {
    object * c = lean_alloc_closure((void*)lean_io_bind_task_fn, 2, 1);
    lean_closure_set(c, 0, f);
    object * t2 = lean_task_bind_core(t, c, lean_unbox(prio), /* keep_alive */ true);
    return io_result_mk_ok(t2);
}

extern "C" obj_res lean_io_check_canceled(obj_arg) {
    return io_result_mk_ok(box(lean_io_check_canceled_core()));
}

extern "C" obj_res lean_io_cancel(b_obj_arg t, obj_arg) {
    lean_io_cancel_core(t);
    return io_result_mk_ok(box(0));
}

extern "C" obj_res lean_io_has_finished(b_obj_arg t, obj_arg) {
    return io_result_mk_ok(box(lean_io_has_finished_core(t)));
}

extern "C" obj_res lean_io_wait(obj_arg t, obj_arg) {
    return io_result_mk_ok(lean_task_get_own(t));
}

extern "C" obj_res lean_io_wait_any(b_obj_arg task_list, obj_arg) {
    object * t = lean_io_wait_any_core(task_list);
    object * v = lean_task_get(t);
    lean_inc(v);
    return io_result_mk_ok(v);
}

extern "C" obj_res lean_io_exit(obj_arg obj, obj_arg /* w */) {
    exit((size_t) obj);
}

void initialize_io() {
    g_io_error_nullptr_read = mk_io_user_error(mk_string("null reference read"));
    mark_persistent(g_io_error_nullptr_read);
    g_io_error_getline = mk_io_user_error(mk_string("getLine failed"));
    mark_persistent(g_io_error_getline);
    g_io_handle_external_class = lean_register_external_class(io_handle_finalizer, io_handle_foreach);
#if defined(LEAN_WINDOWS)
    _setmode(_fileno(stdout), _O_BINARY);
    _setmode(_fileno(stderr), _O_BINARY);
    _setmode(_fileno(stdin), _O_BINARY);
#endif
    g_stream_stdout = lean_stream_of_handle(io_wrap_handle(stdout));
    mark_persistent(g_stream_stdout);
    g_stream_stderr = lean_stream_of_handle(io_wrap_handle(stderr));
    mark_persistent(g_stream_stderr);
    g_stream_stdin  = lean_stream_of_handle(io_wrap_handle(stdin));
    mark_persistent(g_stream_stdin);
#if !defined(LEAN_WINDOWS) && !defined(LEAN_EMSCRIPTEN)
    // We want to handle SIGPIPE ourselves
    lean_always_assert(signal(SIGPIPE, SIG_IGN) != SIG_ERR);
#endif
}

void finalize_io() {
}
}
