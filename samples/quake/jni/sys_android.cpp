/*
Copyright (C) 1996-1997 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

// System functions for Android OS.
// Based on sys_linux.c

#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
// #include <sys/ipc.h>
// #include <sys/shm.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <errno.h>
int errno_portable();
#include <dirent.h>

#include <android/log.h>

#include "quakedef.h"

#define LOG_TAG "Quake sys_android"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

qboolean			isDedicated;

int noconinput = 0;
int nostdout = 0;

// Look for data on either the sdcard or the internal data store.
// (We look at the sdcard first

static const char *basedir1 = "/sdcard/data/quake";
static const char *basedir2 = "/data/quake";

static const char *cachedir = "/tmp";

cvar_t  sys_linerefresh = CVAR2("sys_linerefresh","0");// set for entity display


//
// system dependent
//

struct stat_portable {
    unsigned long long st_dev;
    unsigned char __pad0[4];

    unsigned long __st_ino;
    unsigned int st_mode;
    unsigned int st_nlink;

    unsigned long st_uid;
    unsigned long st_gid;

    unsigned long long st_rdev;
    unsigned char __pad3[4];

    long long st_size;
    unsigned long st_blksize;
    unsigned long long st_blocks;

    unsigned long st_atime;
    unsigned long st_atime_nsec;

    unsigned long st_mtime;
    unsigned long st_mtime_nsec;

    unsigned long st_ctime;
    unsigned long st_ctime_nsec;

    unsigned long long st_ino;
};

#if defined(__mips__)
struct stat_mips {
    unsigned long st_dev;
    unsigned long __pad0[3];

    unsigned long long st_ino;

    unsigned int st_mode;
    unsigned int st_nlink;

    unsigned long st_uid;
    unsigned long st_gid;

    unsigned long st_rdev;
    unsigned long __pad1[3];

    long long st_size;

    unsigned long st_atime;
    unsigned long st_atime_nsec;

    unsigned long st_mtime;
    unsigned long st_mtime_nsec;

    unsigned long st_ctime;
    unsigned long st_ctime_nsec;

    unsigned long st_blksize;
    unsigned long __pad2;

    unsigned long long st_blocks;
};
#endif  //__mips__

int stat_portable(const char *path, struct stat_portable *st);
int fstat_portable(int fd, struct stat_portable *st);


#if !defined(__GDK__)

#if defined(__mips__)
static void __copy_mips_stat_to_portable(struct stat_portable *pst_portable, struct stat/*_mips*/ *pst_mips)
{
   pst_portable->st_dev = pst_mips->st_dev;  //ToDo: sizeof st_dev is different !
   pst_portable->__st_ino = 0;             //ToDo: missing in mips!
   pst_portable->st_mode = pst_mips->st_mode;
   pst_portable->st_nlink = pst_mips->st_nlink;
   pst_portable->st_uid = pst_mips->st_uid;
   pst_portable->st_gid = pst_mips->st_gid;
   pst_portable->st_rdev = pst_mips->st_rdev;  //ToDo: st_rdev is different !
   pst_portable->st_size = pst_mips->st_size;
   pst_portable->st_blksize = pst_mips->st_blksize;
   pst_portable->st_blocks = pst_mips->st_blocks;
   pst_portable->st_atime = pst_mips->st_atime;
   pst_portable->st_atime_nsec = pst_mips->st_atime_nsec;
   pst_portable->st_mtime = pst_mips->st_mtime;
   pst_portable->st_mtime_nsec = pst_mips->st_mtime_nsec;
   pst_portable->st_ctime = pst_mips->st_ctime;
   pst_portable->st_ctime_nsec = pst_mips->st_ctime_nsec;
   pst_portable->st_ino = pst_mips->st_ino;
}
#endif // __mips__

int stat_portable(const char *path, struct stat_portable *st)
{
#if !defined(__mips__)
   struct stat st_orig;
   //assert(sizeof(st_orig) == sizeof(*st));   //ToDo: and offset of each field !

   return stat(path, (struct stat*)st);
#else
   struct stat/*_mips*/ st_mips;
   //assert(sizeof(st_mips) == sizeof(*st));   //ToDo: and offset of each field !

   int ret = stat(path, &st_mips);
   __copy_mips_stat_to_portable((struct stat_portable *)st, &st_mips);

   return ret;
#endif
}

int fstat_portable(int fd, struct stat_portable *st)
{
#if !defined(__mips__)
   struct stat st_orig;
   //assert(sizeof(st_orig) == sizeof(*st));   //ToDo: and offset of each field !

   return fstat(fd, (struct stat*)st);
#else
   struct stat/*_mips*/ st_mips;
   //assert(sizeof(st_mips) == sizeof(*st));   //ToDo: and offset of each field !

   int ret = fstat(fd, &st_mips);
   __copy_mips_stat_to_portable((struct stat_portable *)st, &st_mips);

   return ret;
#endif
}

#endif // !__GDK__

// =======================================================================
// General routines
// =======================================================================

void Sys_DebugNumber(int y, int val)
{
}

/*
void Sys_Printf (char *fmt, ...)
{
  va_list		argptr;
  char		text[1024];

  va_start (argptr,fmt);
  vsprintf (text,fmt,argptr);
  va_end (argptr);
  fprintf(stderr, "%s", text);

  Con_Print (text);
}

void Sys_Printf (char *fmt, ...)
{

    va_list     argptr;
    char        text[1024], *t_p;
    int         l, r;

    if (nostdout)
        return;

    va_start (argptr,fmt);
    vsprintf (text,fmt,argptr);
    va_end (argptr);

    l = strlen(text);
    t_p = text;

// make sure everything goes through, even though we are non-blocking
    while (l)
    {
        r = write (1, text, l);
        if (r != l)
            sleep (0);
        if (r > 0)
        {
            t_p += r;
            l -= r;
        }
    }

}
*/

#define USE_PMPEVENT

void Sys_Printf (const char *fmt, ...)
{
  va_list		argptr;
  char		text[2048];
  unsigned char		*p;

  va_start (argptr,fmt);
  vsnprintf (text, sizeof(text), fmt,argptr);
  va_end (argptr);

  text[sizeof(text)-1] = 0;
  LOGI("%s", text);

#ifdef USE_PMPEVENT
    PMPEVENT(("%s", text));
#else
    if (nostdout)
        return;

  for (p = (unsigned char *)text; *p; p++)
    if ((*p > 128 || *p < 32) && *p != 10 && *p != 13 && *p != 9)
      printf("[%02x]", *p);
    else
      putc(*p, stdout);
#endif
}

qboolean soft_quit;

void Sys_Quit (void)
{
  Host_Shutdown();
#ifdef USE_PMPEVENT
  PMPERROR(("Sys_Quit - exiting."));
#else
  printf("Sys_Quit - exiting.\n");
#endif
    // fcntl (0, F_SETFL, fcntl (0, F_GETFL, 0) & ~FNDELAY);
  if (soft_quit) {
    return;
  }
    exit(0);
}

void Sys_Init(void)
{

}

void Sys_Error (const char *error, ...)
{
    va_list     argptr;
    char        string[1024];

// change stdin to non blocking
    // fcntl (0, F_SETFL, fcntl (0, F_GETFL, 0) & ~FNDELAY);

    va_start (argptr,error);
    vsprintf (string,error,argptr);
    va_end (argptr);
#ifdef USE_PMPEVENT
  PMPERROR(("Error: %s\n", string));
#else
  fprintf(stderr, "Error: %s\n", string);
#endif
  Host_Shutdown ();
#ifdef USE_PMPEVENT
  PMPERROR(("Sys_Error - exiting."));
#else
  printf("Sys_Error - exiting.\n");
#endif
  exit (1);

}

void Sys_Warn (const char *warning, ...)
{
    va_list     argptr;
    char        string[1024];

    va_start (argptr,warning);
    vsprintf (string,warning,argptr);
    va_end (argptr);
#ifdef USE_PMPEVENT
  PMPWARNING(("Warning: %s", string));
#else
  fprintf(stderr, "Warning: %s\n", string);
#endif
}

/*
============
Sys_FileTime

returns -1 if not present
============
*/
int	Sys_FileTime (const char *path)
{
  struct stat_portable buf;

  if (stat_portable(path,&buf) == -1)
    return -1;

  return buf.st_mtime;
}


void Sys_mkdir (const char *path)
{
    mkdir (path, 0777);
}

int Sys_FileOpenRead (const char *path, int *handle)
{
  int	h;
  struct stat_portable fileinfo;

  h = open(path, O_RDONLY, 0666);
  *handle = h;
  if (h == -1)
    return -1;

  if (fstat_portable(h,&fileinfo) == -1)
    Sys_Error ("Error fstating %s", path);

  return fileinfo.st_size;
}

int Sys_FileOpenWrite (const char *path)
{
  int     handle;

  umask (0);

  handle = open(path,O_RDWR | O_CREAT | O_TRUNC
  , 0666);

  if (handle == -1)
    Sys_Error ("Error opening %s: %s", path,strerror(errno_portable()));

  return handle;
}

int Sys_FileWrite (int handle, const void *src, int count)
{
  return write (handle, src, count);
}

void Sys_FileClose (int handle)
{
  close (handle);
}

void Sys_FileSeek (int handle, int position)
{
  lseek (handle, position, SEEK_SET);
}

int Sys_FileRead (int handle, void *dest, int count)
{
    return read (handle, dest, count);
}

void Sys_DebugLog(const char *file, char *fmt, ...)
{
    va_list argptr;
    static char data[1024];
    int fd;

    va_start(argptr, fmt);
    vsprintf(data, fmt, argptr);
    va_end(argptr);
//    fd = open(file, O_WRONLY | O_BINARY | O_CREAT | O_APPEND, 0666);
    fd = open(file, O_WRONLY | O_CREAT | O_APPEND, 0666);
    write(fd, data, strlen(data));
    close(fd);
}

void Sys_EditFile(const char *filename)
{

  char cmd[256];
  char *term;
  const char *editor;

  term = getenv("TERM");
  if (term && !strcmp(term, "xterm"))
  {
    editor = getenv("VISUAL");
    if (!editor)
      editor = getenv("EDITOR");
    if (!editor)
      editor = getenv("EDIT");
    if (!editor)
      editor = "vi";
    sprintf(cmd, "xterm -e %s %s", editor, filename);
    system(cmd);
  }

}

double Sys_FloatTime (void)
{
    struct timeval tp;
    struct timezone tzp;
    static int      secbase;

    gettimeofday(&tp, &tzp);

    if (!secbase)
    {
        secbase = tp.tv_sec;
        return tp.tv_usec/1000000.0;
    }

    return (tp.tv_sec - secbase) + tp.tv_usec/1000000.0;
}

// =======================================================================
// Sleeps for microseconds
// =======================================================================

static volatile int oktogo;

void alarm_handler(int x)
{
  oktogo=1;
}

void Sys_LineRefresh(void)
{
}

void floating_point_exception_handler(int whatever)
{
//	Sys_Warn("floating point exception\n");
  signal(SIGFPE, floating_point_exception_handler);
}

char *Sys_ConsoleInput(void)
{
#if 0
    static char text[256];
    int     len;

  if (cls.state == ca_dedicated) {
    len = read (0, text, sizeof(text));
    if (len < 1)
      return NULL;
    text[len-1] = 0;    // rip off the /n and terminate

    return text;
  }
#endif
  return NULL;
}

#if !id386
void Sys_HighFPPrecision (void)
{
}

void Sys_LowFPPrecision (void)
{
}
#endif

int		skipframes;

// The following APIs are called from the Java activity

double g_oldtime;

extern int scr_width;
extern int scr_height;

qboolean direxists(const char* path)
{
  struct stat_portable sb;
   
  if(stat_portable(path, &sb))
  {
LOGI("direxists 1: path=%s\n", path);
    return 0;	// error
  }
  if(sb.st_mode & S_IFDIR)
  {
LOGI("direxists 2: path=%s\n", path);
     return 1;
  }
LOGI("direxists 3: path=%s\n", path);
  return 0;
}

// Remove all files in path. Recurses into subdirectories

void rmDir(const char* path) {
  DIR* dir = opendir(path);
  if(!dir) {
    return;
  }
  struct dirent * dp;
  while((dp = readdir(dir)) != NULL) {
    const char* name = dp->d_name;
    if(strcmp(name, ".") == 0 || strcmp(name, "..") == 0) {
      continue;
    }
    char filePath[1024];
    if ((int) (sizeof(filePath)-1) < snprintf(filePath, sizeof(filePath), "%s/%s", path, name)) {
      continue; // buffer overflow
    }
    if(direxists(filePath)) {
      rmDir(filePath);
    }
    else {
      unlink(filePath);
    }
  }
  closedir(dir);
  rmdir(path);
}

// Increment this number whenever the data format of any of the files stored in glquake changes:

typedef unsigned long GLCacheVersion;

static const GLCacheVersion kCurrentCacheVersion = 0x3a914000; // The numbers mean nothing special

// #define FORCE_INVALIDATE_CACHE // Useful for testing

#define GLQUAKE_RELPATH "/id1/glquake"
void CheckGLCacheVersion(const char* baseDir)
{
  char cachePath[1024];
  if ((int) (sizeof(cachePath)-1) < snprintf(cachePath, sizeof(cachePath), "%s" GLQUAKE_RELPATH "/cacheversion", baseDir)) {
    return; // buffer overflow
  }
  bool validCache = false;
  {
    GLCacheVersion vernum = 0;
    FILE* f = fopen(cachePath, "rb");
    if (f) {
      if (1 == fread(&vernum, sizeof(vernum), 1, f)) {
        if (vernum == kCurrentCacheVersion) {
          validCache = true;
        }
      }
      fclose(f);
    }
  }

#ifdef FORCE_INVALIDATE_CACHE
  validCache = false;
#endif

  if(!validCache) {
    PMPLOG(("Invalidating glquake cache."));
    char cacheDirPath[1024];
    if ( (int)(sizeof(cacheDirPath)-1) < snprintf(cacheDirPath, sizeof(cacheDirPath), "%s" GLQUAKE_RELPATH, baseDir)) {
      return; // Ran out ot memory
    }
    rmDir(cacheDirPath);
    Sys_mkdir(cacheDirPath);
    FILE* f = fopen(cachePath, "wb");
    if (f) {
      GLCacheVersion vernum = kCurrentCacheVersion;
      fwrite(&vernum, sizeof(vernum), 1, f);
      fclose(f);
    } else {
        PMPLOG(("Could not write %s %d.\n", cachePath, errno_portable()));
    }
  }
}

static int gArgc;
static char** gArgv;

void AndroidInitArgs(int argc, char** argv) {
    gArgc = argc;
    gArgv = argv;
}

static qboolean gDoubleInitializeGuard;
static qboolean gInitialized;
void GL_ReInit();

#if !defined(__GDK__)
bool AndroidInit()
#else
extern "C" bool AndroidInit_LLVM()
#endif
{
  PMPLOG(("AndroidInit"));

  PMPLOG(("This function was compiled on " __DATE__ " at " __TIME__));

  if (gDoubleInitializeGuard && gInitialized)
  {
    GL_ReInit();
  }

  gDoubleInitializeGuard = true;
  return true;
}


// Note: Needs a valid OpenGL context

void AndroidInit2(int width, int height)
{
  PMPLOG(("AndroidInit2 %d,%d", width, height));

  gInitialized = true;
  PMPBEGIN(("AndroidInit2"));
  quakeparms_t parms;
  int j;
  int c = 0;
  const char* v[] = {"quake", (char*) 0};

  scr_width = width;
  scr_height = height;

//	static char cwd[1024];

//	signal(SIGFPE, floating_point_exception_handler);
//  signal(SIGFPE, SIG_IGN);

  memset(&parms, 0, sizeof(parms));

  if (gArgc) {
      COM_InitArgv(gArgc, (const char**) gArgv);
  }
  else {
      COM_InitArgv(c, (const char**) v);
  }

  parms.argc = com_argc;
  parms.argv = com_argv;

  parms.memsize = 16*1024*1024;

  j = COM_CheckParm("-mem");
  if (j)
    parms.memsize = (int) (Q_atof(com_argv[j+1]) * 1024 * 1024);
  parms.membase = malloc (parms.memsize);

  const char* basedir = basedir2;
  if(direxists(basedir1))
  {
    basedir = basedir1;
  }
  else if(direxists(basedir2))
  {
    basedir = basedir2;
  }
  else
  {
    Sys_Error("Could not find data directories %s or %s", basedir1, basedir2);
  }
  parms.basedir = basedir;

  CheckGLCacheVersion(basedir);

// caching is disabled by default, use -cachedir to enable
//	parms.cachedir = cachedir;

#if 0 // FNDELAY not implemented
  noconinput = COM_CheckParm("-noconinput");
  if (!noconinput)
    fcntl(0, F_SETFL, fcntl (0, F_GETFL, 0) | FNDELAY);
#endif

  if (COM_CheckParm("-nostdout"))
    nostdout = 1;

  Sys_Init();

    Host_Init(&parms);

    g_oldtime = Sys_FloatTime ();
  PMPEND(("AndroidInit2"));
}

static int currentFrame;
frameTime fastestFrame;
frameTime slowestFrame;

void InitFrameTimes()
{
    currentFrame = 0;
  fastestFrame.time = 1e6;
  fastestFrame.frame = 0;
  slowestFrame.time = -1;
  slowestFrame.frame = 0;
}

static void UpdateFrameTimes(float time)
{
    if (currentFrame > 0) {

    if (fastestFrame.time > time) {
      fastestFrame.time = time;
      fastestFrame.frame = currentFrame;
    }
    if (slowestFrame.time < time) {
      slowestFrame.time = time;
      slowestFrame.frame = currentFrame;
    }
  }
  currentFrame++;
}

int AndroidStepImp(int width, int height)
{
  // PMPBEGIN(("AndroidStep"));
  double time, newtime;
  static int TotalCount = 0;
  static double TotalFPS = 0.0;
  if(!gInitialized)
    AndroidInit2(width, height);

  scr_width = width;
  scr_height = height;

  // find time spent rendering last frame
  newtime = Sys_FloatTime ();
  time = newtime - g_oldtime;

  UpdateFrameTimes(time);
 #if 0
   // Disable the following because given a series of Ti representing time spent per frame
   // 1/(sum(Ti)/n) isn't quite the same as sum(1/Ti)/n, especially when Ti has large variance.
   // See LOGI in host.cpp::Host_Frame for better implementation
  double fps = 1.0/time;
  if (fps > 0.0 && fps < 200.0) { // Sometimes it
    TotalCount += 1;
    TotalFPS += fps;
    LOGI("Quake fps: %3.2lf, Average: %3.2lf", fps, TotalFPS/TotalCount);
  }
 #endif
  Host_Frame(time);
  g_oldtime = newtime;
  // PMPEND(("AndroidStep"));
  return key_dest == key_game;
}

#if !defined(__GDK__)
int AndroidStep(int width, int height)
#else
extern "C" int AndroidStep_LLVM(int width, int height)
#endif
{
  for(;;) {
    host_framethrottled = false;
    int result = AndroidStepImp(width, height);
    if (!host_framethrottled) {
        return result;
    }
    usleep(1000);
    //LOGI("%s", "host_framethrottled");
  }
}

extern void Host_Quit();


#if !defined(__GDK__)
void AndroidQuit() {
#else
extern "C" void AndroidQuit_LLVM() {
#endif
  soft_quit = true;
  Host_Quit();
  soft_quit = false; // In case we live on after returning.
}
