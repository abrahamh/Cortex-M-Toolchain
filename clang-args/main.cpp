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

int main(int argc, const char * argv[])
{
    int ret = 1;
    const char* target_clang = "arm-none-eabi-clang";
    std::string app_name;
    std::string app_path;
    std::string arguments;
    std::string sys_call;
    
    if (argc > 1)
    {
        app_name = GetBasename((const char*)argv[0]);
        app_path = GetDirname((const char*)argv[0]);
        if ( (0 == app_name.compare("clang-arg")) || (0 == app_name.compare("clang")) )
        {
            for (int i=1;i<argc;i++)
            {
                const char *p;
                
                p = (const char*)argv[i];
                
                if ( 0 == strcmp(p, "-arch") )
                {
                    i++;
                    continue;
                }
                
                if ( 0 == strcmp(p, "-isysroot") )
                {
                    i++;
                    continue;
                }
                
                if ( 0 == strncmp(p, "-mmacosx-version-min=", 21) )
                {
                    continue;
                }
                
                arguments += argv[i];
                arguments += " ";
            }
        }
        
        sys_call = app_path;
        sys_call += target_clang;
        sys_call += " ";
        sys_call += arguments;
        
#if 1
        FILE *fd;
        fd = fopen("/tmp/clang.txt", "a+");
        if (fd)
        {
            fprintf(fd, "%s\n", sys_call.c_str());
            fclose(fd);
        }
#endif
        
        ret = system(sys_call.c_str());
    }
    
    return ret;
}
