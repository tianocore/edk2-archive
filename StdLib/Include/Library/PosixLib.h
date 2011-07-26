/**
  Copyright (c) 2011, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials                          
  are licensed and made available under the terms and conditions of the BSD License         
  which accompanies this distribution.  The full text of the license may be found at        
  http://opensource.org/licenses/bsd-license.php                                            
                                                                                            
  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED. 
**/

#ifndef _POSIX_LIB_H_
#define _POSIX_LIB_H_

#include <stdio.h>
#include <stddef.h>
#include <sys/types.h>
#include <setjmp.h>
#include <Library/UefiBootServicesTableLib.h>
#include <sys/time.h>

typedef struct __PASSWD {
  char *pw_dir;
  char *pw_name;
} passwd;

//
// always error functions
//
uid_t   getuid(void);
passwd  *getpwuid(uid_t);
passwd  *getpwnam(const char *);
char    *getlogin(void);
pid_t   fork(void);
int     chmod     (const char *, mode_t);
pid_t   wait(int *stat_loc);
FILE    *popen (const char *cmd, const char *type);
int     pclose (FILE *stream);

#endif