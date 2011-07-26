/** @file
  Implement the invalid functions to return failures.

  Copyright (c) 2011, Intel Corporation
  All rights reserved. This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/PosixLib.h>
#include <errno.h>
#include <sysexits.h>

passwd *getpwuid (uid_t uid)
{
  uid;
  errno = EPERM;
  return NULL;
}

char *getlogin (void)
{
  errno = EPERM;
  return NULL;
}

passwd *getpwnam (const char *name)
{
  name;
  errno = EPERM;
  return NULL;
}

uid_t getuid (void)
{
  return 0;
}

pid_t fork (void) 
{
  errno = EPERM;
  return (-1);
}

int chmod (const char *c, mode_t m) 
{
  errno = EPERM;
  return (-1);
}

pid_t   wait(int *stat_loc) {
  return 0;
}

FILE *popen (const char *cmd, const char *type)
{
  errno = EPERM;
	return NULL;
}

int pclose (FILE *stream)
{
  errno = EPERM;
	return -1;
}

int access (const char *path, int amode)
{
  return 0;
}
