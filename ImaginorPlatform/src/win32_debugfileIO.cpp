#include "platform.h"
#include <Windows.h>

namespace IME {

    /**
     * debug file access
     */
    internal
    IME_DEBUG_RELEASE_FILE_MEMORY(win32_DEBUGreleaseFileMemory) {

        if(file->data) {
            VirtualFree(file->data, 0, MEM_RELEASE);
        }
        file->data = nullptr;
        file->size = 0;
    }

    internal 
    IME_DEBUG_READ_ENTIRE_FILE(win32_DEBUGreadEntireFile) {

        FileBuffer result;

        HANDLE filehandle = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
        if(filehandle ==  INVALID_HANDLE_VALUE) {
            return result;
        }

        if(fileinfo) {

            FILETIME lastaccessed;
            FILETIME created;
            FILETIME lastmodified;

            bool32 check;
            check = GetFileTime(
                filehandle,
                &created,
                &lastaccessed,
                &lastmodified
            );

            fileinfo->datelastaccessed = *((uint64*)&lastaccessed);
            fileinfo->datecreated = *((uint64*)&created);
            fileinfo->datelastmodified = *((uint64*)&lastmodified);
        }

        LARGE_INTEGER filesize;
        if(!GetFileSizeEx(filehandle, &filesize)) {
            CloseHandle(filehandle);
            return result;
        }

        result.size = (uint64)filesize.QuadPart;
        result.data = VirtualAlloc(0, filesize.QuadPart, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

        DWORD bytesread;
        if(!ReadFile(filehandle, result.data, filesize.QuadPart, &bytesread, 0)) {

            DWORD error = GetLastError();

            CloseHandle(filehandle);
            win32_DEBUGreleaseFileMemory(&result);
            return result;
        }

        CloseHandle(filehandle);
        return result;
    }

    internal 
    IME_DEBUG_GET_FILE_INFO(win32_DEBUGgetFileInfo) {

        HANDLE filehandle = CreateFileA(filepath, GENERIC_READ, FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
        if(filehandle ==  INVALID_HANDLE_VALUE) {
            DWORD error = GetLastError();
            return {0, 0, 0};
        }

        FileInfo result;

        FILETIME lastaccessed;
        FILETIME created;
        FILETIME lastmodified;

        bool32 check;
        check = GetFileTime(
            filehandle,
            &created,
            &lastaccessed,
            &lastmodified
        );

        result.datecreated = *((uint64*)&lastaccessed);
        result.datecreated = *((uint64*)&created);
        result.datelastmodified = *((uint64*)&lastmodified);

        CloseHandle(filehandle);

        return result;
    }

    internal
    IME_DEBUG_WRITE_ENTIRE_FILE(win32_DEBUGwriteEntireFile) {
        
        HANDLE filehandle = CreateFileA(filename, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
        if(filehandle ==  INVALID_HANDLE_VALUE) {
            return false;
        }

        DWORD nbyteswritten;
        if(!WriteFile(filehandle, data, filesize, &nbyteswritten, 0)) {
            CloseHandle(filehandle);
            return false;
        }
        
        CloseHandle(filehandle);
        return true;
    }


}