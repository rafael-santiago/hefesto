#ifndef _HEFESTO_HVM_WINREG_H
#define _HEFESTO_HVM_WINREG_H

char *get_value_from_winreg(const char *value_fullpath);
int set_value_from_winreg(const char *value_fullpath, const char *value);
int del_value_from_winreg(const char *value_fullpath);

#endif

