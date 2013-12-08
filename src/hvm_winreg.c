#include "hvm_winreg.h"
#include "mem.h"
#include "types.h"
#include <string.h>
#if HEFESTO_TGT_OS == HEFESTO_WINDOWS
#include <windows.h>
#include <winreg.h>
#endif

#if HEFESTO_TGT_OS == HEFESTO_WINDOWS

static unsigned int get_key_handle_from_wreg_path(const char *path);

static char *get_value_from_wreg_path(const char *path);

static DWORD get_value_type_from_wreg_path(const char *path);

static LONG open_winreg_key(const char *value_fullpath, PHKEY key);

static char *get_subkey_from_winreg_path(const char *value_fullpath);

#endif


#if HEFESTO_TGT_OS == HEFESTO_WINDOWS

static unsigned int get_key_handle_from_wreg_path(const char *path) {

    const char *p, *pe;
    char handle[HEFESTO_MAX_BUFFER_SIZE], *h;
    for (p = path; *p != '\\' && *p != '/' && *p != 0; p++);
    pe = p;
    memset(handle, 0, sizeof(handle));
    for (h = handle, p = path; p != pe; p++, h++) {
        *h = *p;
    }
    if (strcmp(handle, "HKLM") == 0) {
        return ((unsigned int)HKEY_LOCAL_MACHINE);
    }
    if (strcmp(handle, "HKCU") == 0) {
        return ((unsigned int)HKEY_CURRENT_USER);
    }
    if (strcmp(handle, "HKU") == 0) {
        return ((unsigned int)HKEY_USERS);
    }
    if (strcmp(handle, "HKCR") == 0) {
        return ((unsigned int)HKEY_CLASSES_ROOT);
    }
    if (strcmp(handle, "HKCC") == 0) {
        return ((unsigned int)HKEY_CURRENT_CONFIG);
    }

    return ((unsigned int)-1);
}

static char *get_value_from_wreg_path(const char *path) {
    const char *p;
    char *result = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE);
    char *r;
    for (p = path; *p != 0; p++);
    for (p--; *p != '\\' && *p != '/' && p != path; p--);
    p++;
    memset(result, 0, HEFESTO_MAX_BUFFER_SIZE);
    for (r = result;
         r != (result + HEFESTO_MAX_BUFFER_SIZE-1) &&
         *p != 0 && *p != ':'; p++, r++) {
        *r = *p;
    }
    return result;
}

static DWORD get_value_type_from_wreg_path(const char *path) {
    const char *p;
    for (p = path; *p != 0; p++);
    for (p--; *p != ':'; p--);
    if (*p == ':') {
        if (strcmp(p + 1, "REG_SZ") == 0) return REG_SZ;
        if (strcmp(p + 1, "REG_NONE") == 0) return REG_NONE;
        if (strcmp(p + 1, "REG_EXPAND_SZ") == 0) return REG_EXPAND_SZ;
        if (strcmp(p + 1, "REG_BINARY") == 0) return REG_BINARY;
        if (strcmp(p + 1, "REG_DWORD") == 0) return REG_DWORD;
        if (strcmp(p + 1, "REG_LINK") == 0) return REG_LINK;
        if (strcmp(p + 1, "REG_MULTI_SZ") == 0) return REG_MULTI_SZ;
        if (strcmp(p + 1, "REG_QWORD") == 0) return REG_QWORD;
    }
    return REG_SZ;
}

static LONG open_winreg_key(const char *value_fullpath, PHKEY key) {
    char subpath[HEFESTO_MAX_BUFFER_SIZE];
    size_t s;
    const char *v, *ve;
    HKEY hk = (HKEY) get_key_handle_from_wreg_path(value_fullpath);
    for (v = value_fullpath; *v != 0; v++);
    for (v--; *v != '\\' && *v != '/' && v != value_fullpath; v--);
    ve = v;
    for (v = value_fullpath; *v != 0 &&
                             *v != '/' && *v != '\\'; v++);

    if (*v != 0) {
        for (v++, s = 0; s < HEFESTO_MAX_BUFFER_SIZE && v != ve; v++, s++) {
            subpath[s] = *v;
        }
        subpath[s] = 0;
        return RegOpenKeyEx(hk, subpath, 0, KEY_ALL_ACCESS, key);
    }

    return 1;
}

static char *get_subkey_from_winreg_path(const char *value_fullpath) {
    char *subpath = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE);
    size_t s;
    const char *v, *ve;
    for (v = value_fullpath; *v != 0; v++);
    for (v--; *v != '\\' && *v != '/' && v != value_fullpath; v--);
    ve = v;
    for (v = value_fullpath; *v != 0 &&
                             *v != '/' && *v != '\\'; v++);

    memset(subpath, 0, HEFESTO_MAX_BUFFER_SIZE);

    if (*v != 0) {
        for (v++, s = 0; s < HEFESTO_MAX_BUFFER_SIZE && v != ve; v++, s++) {
            subpath[s] = *v;
        }
        subpath[s] = 0;
    }

    return subpath;
}

#endif

char *get_value_from_winreg(const char *value_fullpath) {
    char *result = NULL;
#if HEFESTO_TGT_OS == HEFESTO_WINDOWS
    HKEY hk;
    char *value_name = get_value_from_wreg_path(value_fullpath);
    char *temp = NULL, xd[10], *t, *r;
    DWORD vtype, rsize = HEFESTO_MAX_BUFFER_SIZE;
    result = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE);
    memset(result, 0, HEFESTO_MAX_BUFFER_SIZE);
    if (open_winreg_key(value_fullpath, &hk) == ERROR_SUCCESS) {
        if (RegQueryValueEx(hk,
                            value_name,
                            NULL, &vtype, result,
                            &rsize) == 0) {
            switch (vtype) {
                case REG_DWORD:
                    rsize = *(DWORD *)result;
                    sprintf(result, "%d", rsize);
                    break;
                case REG_BINARY:
                    temp = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE);
                    memcpy(temp, result, rsize);
                    memset(result, 0, HEFESTO_MAX_BUFFER_SIZE);
                    for (t = temp; t != (temp + rsize); t++) {
                        sprintf(xd, "\\x%.2x", *t);
                        if (strlen(result) + 4 < HEFESTO_MAX_BUFFER_SIZE - 1) {
                            strcat(result, xd);
                        }
                    }
                    free(temp);
                    break;
            }
        }
        RegCloseKey(hk);
    }
    free(value_name);
#endif
    return result;
}

int set_value_from_winreg(const char *value_fullpath, const char *value) {
    int result = 0;
#if HEFESTO_TGT_OS == HEFESTO_WINDOWS
    HKEY hk;
    DWORD vtype, vlen;
    char *value_name = NULL;
    if ((result = open_winreg_key(value_fullpath, &hk) == ERROR_SUCCESS)) {
        vtype = get_value_type_from_wreg_path(value_fullpath);
        value_name = get_value_from_wreg_path(value_fullpath);
        vlen = strlen(value);
        result = RegSetValueEx(hk, value_name, 0, vtype, value, vlen);
        free(value_name);
        RegCloseKey(hk);
    } else {
        result = GetLastError();
    }
#endif
    return result;
}

int del_value_from_winreg(const char *value_fullpath) {
    int result = 0;
#if HEFESTO_TGT_OS == HEFESTO_WINDOWS
    HKEY hk;
    char *value_name = NULL;
    char *subkey = NULL;
    DWORD vtype;
    if ((result = open_winreg_key(value_fullpath, &hk) == ERROR_SUCCESS)) {
        value_name = get_value_from_wreg_path(value_fullpath);
        if (RegQueryValueEx(hk, 
                            value_name,
                            NULL, &vtype, NULL,
                            NULL) == 0) {
            subkey = get_subkey_from_winreg_path(value_fullpath);
            if (*subkey != 0) {
                result = RegDeleteValue(hk, value_name);
            }
            free(subkey);
        }
        free(value_name);
        RegCloseKey(hk);
    }
#endif
    return result;
}
