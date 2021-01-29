//
//  main.cpp
//  clang-args
//
//  Created by Heiko Abraham on 29.06.16.
//
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <string>
#include <sys/wait.h>
#include <errno.h>
#include <time.h>
#include <assert.h>
#include <fcntl.h>

std::string GetRealPath(const char *path)
{
    std::string real_path;
    char realpath_buffer[PATH_MAX+1];
    char *p;
    
    if (path)
    {
        memset(realpath_buffer, 0x0, sizeof(realpath_buffer));
        snprintf(realpath_buffer, sizeof(realpath_buffer)-1, "%s", path);
        
        p = realpath(path, realpath_buffer);
        
        if (p != NULL)
        {
            real_path = realpath_buffer;
        }
    }
    
    return real_path;
}

std::string GetDirname(const char *path)
{
    std::string real_path;
    char *p;
    
    real_path = GetRealPath(path);
    
    p = dirname( (char*)real_path.c_str());
    
    if (p != NULL)
    {
        real_path = p;
        real_path += "/";
    }
    
    return real_path;
}


std::string GetBasename(const std::string path)
{
    std::string ret;
    char *p;
    char basename_path[PATH_MAX+1];
    
    snprintf(basename_path, sizeof(basename_path), "%s", path.c_str());
    basename_path[sizeof(basename_path)-1] = 0;
    
    ret = path;
    p = basename(basename_path);
    if (p)
    {
        ret = p;
    }
    
    return ret;
}


std::string AppendFilelistContent(const std::string path)
{
   std::string ret;
   FILE *fd;
   struct stat buf;
   size_t len;
   size_t i;
   char *p;
   
   if (0 == stat(path.c_str(), &buf) )
   {
      len = buf.st_size;
      p = (char*)malloc(len+1);
      if (p)
      {
         p[len] = 0;
         
         fd = fopen(path.c_str(), "r");
         if (fd)
         {
            size_t ll;
            
            ll = fread(p, sizeof(char), len, fd);
            
            assert(ll == len);
            
            for (i=0;i<len;i++)
            {
               if ( p[i] == '\n' )
               {
                  p[i] = ' ';
               }
            }
            
            ret += " ";
            ret += p;
            ret += " ";
            
            fclose(fd);
         }
         
         free(p);
      }
   }
   
   return ret;
}


void CreateDependencyInfoFile(const std::string filelist, const std::string outfile, const std::string infofile)
{
    std::string blob;
    FILE *fd;
    int fd2;
    struct stat buf;
    size_t len;
    size_t i;
    char *p;
    
    /* User has read & write perms, group and others have read permission */
    const mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    
    if (0 == stat(filelist.c_str(), &buf) )
    {
        len = buf.st_size;
        p = (char*)malloc(len+1);
        if (p)
        {
            p[len] = 0;
            unlink(infofile.c_str());
            fd2 = open(infofile.c_str(), O_WRONLY | O_CREAT, mode);
            fd = fopen(filelist.c_str(), "r");
            if (fd && (fd2 >= 0))
            {
                size_t ll;
                
                write(fd2, "\0cctools-977.1\0", 15);
                ll = fread(p, sizeof(char), len, fd);
                
                assert(ll == len);
                
                blob = "";
                for (i=0;i<len;i++)
                {
                    if ( p[i] == '\n' )
                    {
                        write(fd2, "\x10", 1);
                        write(fd2, blob.c_str(), blob.length() );
                        write(fd2, "\0", 1);
                        blob = "";
                        continue;
                    }
                    blob += p[i];
                }
                
                write(fd2, "\x40", 1);
                write(fd2, outfile.c_str(), outfile.length() );
                write(fd2, "\0", 1);
                
                fclose(fd);
                close(fd2);
            }
            
            free(p);
        }
    }
}


int main(int argc, const char * argv[])
{
    int ret = 1;
    int clang_call_as_linker = 0;
    const char* target_clang = "arm-none-eabi-clang";
    const char* target_ld = "arm-none-eabi-ld";
    std::string app_name;
    std::string app_path;
    std::string sdk_path;
    std::string filelist_file;
    std::string arguments;
    std::string sys_call;
    int use_dependency_info = 0;
    std::string dependency_file;
    std::string output_file;
    
    if (argc > 1)
    {
        app_name = GetBasename((const char*)argv[0]);
        app_path = GetDirname((const char*)argv[0]);
        sdk_path = app_path;
        sdk_path += "/../..";
        if ( (0 == app_name.compare("clang-args")) || (0 == app_name.compare("clang")) )
        {
            for (int i=1;i<argc;i++)
            {
                const char *p;
                
                p = (const char*)argv[i];
                
                if ( 0 == strcmp(p, "-Xlinker") )
                {
                    clang_call_as_linker = 1;
                    continue;
                }
            }
            
            /* check and filter command arguments for cortex-m based toolchain */
            for (int i=1;i<argc;i++)
            {
                const char *p;
                
                p = (const char*)argv[i];
                
                /* skip architecture definition */
                if ( 0 == strcmp(p, "-arch") )
                {
                    i++;
                    continue;
                }
                if ( 0 == strcmp(p, "-target") )
                {
                    if (clang_call_as_linker == 0)
                    {
                        arguments += argv[i];
                        arguments += " arm-none-eabi ";
                    }
                    
                    i++;
                    continue;
                }
                if ( 0 == strcmp(p, "-Wquoted-include-in-framework-header") )
                {
                    continue;
                }
                if ( 0 == strcmp(p, "-index-store-path") )
                {
                    i++;
                    continue;
                }
                
                
                if ( 0 == strcmp(p, "-isysroot") )
                {
                    i++;
                    switch (clang_call_as_linker)
                    {
                        case 1:
                            arguments += "--sysroot=";
                            break;
                        case 0:
                        default:
                            arguments += "-isysroot ";
                    }
                    
                    arguments += sdk_path;
                    arguments += " ";
                    continue;
                }
                
                /* skip Framework path definition */
                if ( 0 == strncmp(p, "-F/", 3) )
                {
                    continue;
                }
                
                /* skip macOS/iOS version definition */
                if ( (0 == strncmp(p, "-mmacosx-version-min=", 17)) ||
                     (0 == strncmp(p, "-miphoneos-version-min=", 22)))
                {
                    continue;
                }
                
                /* read/build filelist entries */
                if ( 0 == strcmp(p, "-filelist") )
                {
                    i++;
                    filelist_file = argv[i];
                    arguments += AppendFilelistContent(filelist_file);
                    arguments += " ";
                    continue;
                }
                
                if (clang_call_as_linker)
                {
                    /* skip Xlinker arguments */
                    if ( 0 == strcmp(p, "-Xlinker") )
                    {
                        i++;
                        p = (const char*)argv[i];
                        if ( 0 == strcmp(p, "-dependency_info") )
                        {
                            use_dependency_info = 1;
                            i++;
                            p = (const char*)argv[i];
                            if ( 0 == strcmp(p, "-Xlinker") )
                            {
                                i++;
                            }
                            dependency_file = argv[i];
                        }
                        continue;
                    }
                    
                    /* skip DylibLinker arguments */
                    if ( 0 == strcmp(p, "-dynamiclib") )
                    {
                        continue;
                    }
                    
                    /* skip DylibLinker arguments */
                    if ( 0 == strcmp(p, "-install_name") )
                    {
                        i++;
                        continue;
                    }
                    
                    /* skip DylibLinker arguments */
                    if ( 0 == strcmp(p, "-single_module") )
                    {
                        continue;
                    }
                    
                    /* skip DylibLinker arguments */
                    if ( 0 == strcmp(p, "-compatibility_version") )
                    {
                        i++;
                        continue;
                    }
                    
                    /* skip DylibLinker arguments */
                    if ( 0 == strcmp(p, "-current_version") )
                    {
                        i++;
                        continue;
                    }
                    
                    /* skip unhandled linker args */
                    if ( 0 == strcmp(p, "-ftest-coverage") )
                    {
                        continue;
                    }
                }
                
                if ( 0 == strcmp(p, "-o") )
                {
                    arguments += argv[i];
                    arguments += " ";
                    i++;
                    arguments += argv[i];
                    arguments += " ";
                    output_file = argv[i];
                    continue;
                }
                
                arguments += argv[i];
                arguments += " ";
            }
        }
        
        /* build command line */
        switch (clang_call_as_linker)
        {
            case 1:
                /* call clang as linker - use binutils */
                /* typical :
                   arm-none-eabi-ld 
                     -Map filename.map -nostdlib -nodefaultlibs -nostartfiles
                     -L<path_linker_script> -T <LINKER_SCRIPT>
                     -L...  --gc-sections --undefined=uxTopUsedPriority --print-gc-sections *.o
                     --start-group -lgcc -lc --end-group -o main.axf */
                sys_call = app_path;
                sys_call += target_ld;
                sys_call += " ";
                sys_call += arguments;
                break;
                
            case 0:
            default:
                /* call clang as compiler */
                /* typical :
                   arm-none-eabi-clang -g -nostdlib -ffreestanding  -O0  
                       -target arm-none-eabi -mcpu=cortex-m3 -mfloat-abi=soft 
                       -mthumb -I...
                       -o main.o -c main.c
                 */
                sys_call = app_path;
                sys_call += target_clang;
                sys_call += " ";
                sys_call += arguments;
        }
        
        /* execute redirect call */
        fprintf(stdout, "%s\n", sys_call.c_str());
        fflush(stdout);
        ret = system(sys_call.c_str());
        if (ret)
        {
            ret = 1;
        }
        if (/*(ret == 0) && */use_dependency_info == 1)
        {
            CreateDependencyInfoFile(filelist_file, output_file, dependency_file);
        }
    }
    
    return ret;
}
