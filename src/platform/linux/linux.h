#include <cinttypes>

#define Internal       static
#define LocalPersist   static
#define GlobalVariable static

#define FONT_NAME "-*-fixed-medium-r-*-*-14-*-*-*-*-*-*-*"

#define Pi32 3.14159265359f

typedef int8_t  int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef int32   bool32;

typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef float  real32;
typedef double real64;

#define SCREEN_WIDTH 1980
#define SCREEN_HEIGHT 1080

#define SCREEN_ORIGIN_X 0
#define SCREEN_ORIGIN_Y 0

#define SCREEN_BORDER_WIDTH 0

#ifndef MAX_PATH
#define MAX_PATH 255
#endif

struct BackBuffer
{
    // Pixel is 32-bit
    // Memory order BB GG RR XX
    void *Memory;
    int Width;
    int Height;
    int Pitch;
    int BytesPerPixel;
};

struct LinuxState
{
    uint64 TotalSize;
    void* GameMemory;
    /* InputReplayBuffer InputReplayBuffers[4]; */

    char  ExeFilepath[MAX_PATH];
    char* ExeBasename;

    int RecordingFileDescriptor;
    int InputRecordingIndex;

    int PlaybackFileDescriptor;
    int InputPlaybackIndex;
};
