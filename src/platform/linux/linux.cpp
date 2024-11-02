#include <stdio.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdint.h>
#include <stdint.h>
#include <string>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <chrono>
#include <thread>

#include "linux.h"

GlobalVariable bool32 RUNNING;
GlobalVariable bool32 PAUSE;

GlobalVariable BackBuffer BACK_BUFFER;

GlobalVariable uint32 MONITOR_REFRESH_HZ = 60;

GlobalVariable bool32 FULLSCREEN        = false;
GlobalVariable uint32 WINDOW_POSITION_X = 0;
GlobalVariable uint32 WINDOW_POSITION_Y = 0;
GlobalVariable uint32 WINDOW_WIDTH      = 1920;
GlobalVariable uint32 WINDOW_HEIGHT     = 1080;

GlobalVariable Cursor HIDDEN_CURSOR;

GlobalVariable uint32 EXPECTED_FRAMES_PER_UPDATE = 1;

enum
{
    _NET_WM_STATE_REMOVE = 0,
    _NET_WM_STATE_ADD = 1,
    _NET_WM_STATE_TOGGLE = 2
};

Internal inline struct timespec LinuxGetWallClock()
{
    struct timespec Clock;
    clock_gettime(CLOCK_MONOTONIC, &Clock);
    return Clock;
}

Internal inline real32 LinuxGetSecondsElapsed(
    struct timespec Start,
    struct timespec End
) {
    return (
	(real32)(End.tv_sec - Start.tv_sec) + ((real32)(End.tv_nsec - Start.tv_nsec) * 1e-9f)
    );
}

Internal void PlatformFreeFileMemory(read_file_result* File) {
    if (File->_content) munmap(File->_content, File->_contentSize);

    File->_contentSize = 0;
}

Internal read_file_result PlatformReadEntireFile(std::string filename) {
    read_file_result Result = {};
    sint32 FileHandle = open(filename.c_str(), O_RDONLY);

    if (FileHandle < 0) {
        off_t FileSize64 = lseek(FileHandle, 0, SEEK_END);
        lseek(FileHandle, 0, SEEK_SET);

        if (FileSize64 > 0) {
            uint32_t FileSize32 = SafeTruncateUInt64(FileSize64);
            Result._content = mmap(
                NULL,
                FileSize32,
                PROT_READ   | PROT_WRITE,
                MAP_PRIVATE | MAP_ANONYMOUS,
                -1,
                0
            );

            if (Result._content) {
                ssize_t BytesRead = read(
                    FileHandle,
                    Result._content,
                    Result._contentSize);

                if ((ssize_t)FileSize32 == BytesRead) {
                    // File read successfully
                    Result._contentSize = FileSize32;
                }
                else {
                    fprintf(
                        stderr,
                        "ERROR: Incorrectly read '%s'.\n",
                        filename.c_str()
                    );
                    PlatformFreeFileMemory(&Result);
                }
            } else {
                fprintf(
                    stderr,
                    "ERROR: Failed to allocate memory to read in '%s'.\n",
                    filename.c_str()
                );
            }
        }
        else {
            fprintf(
                stderr,
                "ERROR: '%s' is empty.\n",
                filename.c_str()
            );
       }

        close(FileHandle);
    }
    else {
        fprintf(
            stderr,
            "ERROR: Failed to open file handle to '%s'.\n",
            filename.c_str()
        );
    }

    return Result;
}

Internal bool32 PlatformWriteEntireFile(
    std::string filename,
    uint32_t memorySize,
    void* Memory
) {
    bool32 Result = false;

    sint32 FileHandle = open(
        filename.c_str(),
        O_WRONLY | O_CREAT,
        S_IRUSR  | S_IWUSR
    );

    if (FileHandle >= 0) {
        ssize_t BytesWritten = write(FileHandle, Memory, memorySize);
        if (fsync(BytesWritten) >= 0) {
            Result = (BytesWritten == (ssize_t)memorySize);
        }
        else {
            fprintf(
                stderr,
                "ERROR: Failed to correctly write to '%s'\n",
                filename.c_str()
            );
        }

        close(FileHandle);
    }
    else {
        fprintf(
            stderr,
            "ERROR: Failed to open file handle to '%s'.\n",
            filename.c_str()
        );
    }

    return Result;
}

Internal inline ino_t LinuxFileId(std::string filename) {
    struct stat Attr = {};
    if (stat(filename.c_str(), &Attr)) { Attr.st_ino = 0; }

    return Attr.st_ino;
}

Internal void *LinuxLoadLibrary(const std::string name) {
    void *Handle = NULL;

    Handle = dlopen(name.c_str(), RTLD_NOW | RTLD_LOCAL);
    if (! Handle) { fprintf(stderr, "ERROR: dlopen failed: %s\n", dlerror()); }

    return Handle;
}

Internal void LinuxUnloadLibrary(void *Handle) {
    if (Handle != NULL) {
        dlclose(Handle);
        Handle = NULL;
    }
}

Internal void *LinuxLoadFunction(void *LibHandle, const std::string name) {
    void *Symbol = dlsym(LibHandle, name.c_str());
    if (! Symbol) { fprintf(stderr, "ERROR dlsym failed: %s\n", dlerror()); }

    return Symbol;
}

Internal bool32 LinuxLoadEngineCode(
    linux_engine_code *EngineCode,
    std::string dllFilename,
    ino_t FileID
) {
    if (EngineCode->LibID != FileID) {
        LinuxUnloadLibrary(EngineCode->LibHandle);
        EngineCode->LibID   = FileID;
        EngineCode->IsValid = false;

        EngineCode->LibHandle = LinuxLoadLibrary(dllFilename.c_str());
        if (EngineCode->LibHandle) {
            *(void **)(&EngineCode->UpdateAndRender) = LinuxLoadFunction(
                EngineCode->LibHandle,
                "UpdateAndRender"
             );

            EngineCode->IsValid = EngineCode->UpdateAndRender ? true : false;
        }
    }

    if(!EngineCode->IsValid) {
        LinuxUnloadLibrary(EngineCode->LibHandle);
        EngineCode->LibID           = 0;
        EngineCode->UpdateAndRender = 0;
    }

    return(EngineCode->IsValid);
}

Internal void LinuxUnloadEngineCode(linux_engine_code *EngineCode) {
    LinuxUnloadLibrary(EngineCode->LibHandle);
    EngineCode->LibID           = 0;
    EngineCode->IsValid         = false;
    EngineCode->UpdateAndRender = 0;
}

Internal linux_offscreen_buffer CreateOffscreenBuffer(uint32_t w, uint32_t h) {
    linux_offscreen_buffer OffscreenBuffer = {};
    OffscreenBuffer._Width = w;
    OffscreenBuffer._Height = h;
    OffscreenBuffer._Pitch = Align16(OffscreenBuffer._Width * BYTES_PER_PIXEL);

    uint32_t Size = OffscreenBuffer._Pitch * OffscreenBuffer._Height;

    OffscreenBuffer._Memory = (uint8_t*)mmap(
        0,
        Size,
        PROT_READ   | PROT_WRITE,
        MAP_PRIVATE | MAP_ANONYMOUS,
        -1,
        0
    );

    if (OffscreenBuffer._Memory == MAP_FAILED) {
        OffscreenBuffer._Width  = 0;
        OffscreenBuffer._Height = 0;

        fprintf(
            stderr,
            "ERROR: Failed to allocate memory for off-screen buffer\n"
        );
    }

    return OffscreenBuffer;
}

Internal void ResizeOffscreenBuffer(
    linux_offscreen_buffer* Buffer,
    uint32_t Width,
    uint32_t Height
) {
    if(Buffer->_Memory) {
        munmap(Buffer->_Memory, Buffer->_Pitch * Buffer->_Height);
    }else {
        fprintf(
            stderr,
            "ERROR: Trying to resize off-screen buffer %s.\n",
            "with no allocated memory"
        );
    }

    Buffer->_Width = Width;
    Buffer->_Height = Height;
    Buffer->_Pitch = Buffer->_Width * BYTES_PER_PIXEL;

    uint32_t NewSize = Buffer->_Pitch * Buffer->_Height;
    Buffer->_Memory = (uint8_t*)mmap(
        NULL,
        NewSize,
        PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANONYMOUS,
        -1,
        0);

    if (Buffer->_Memory == MAP_FAILED) {
        Buffer->_Width = 0;
        Buffer->_Height = 0;

        fprintf(
            stderr,
            "ERROR: Failed to allocate memory for off-screen buffer resize.\n"
        );

        return;
    }
}

Internal linux_window_dimension LinuxGetWindowDimension(
    Display* display,
    Window window
) {
    XWindowAttributes WindowAttribs = {};
    XGetWindowAttributes(display, window, &WindowAttribs);

    linux_window_dimension Result = {};
    Result._Width = WindowAttribs.width;
    Result._Height = WindowAttribs.height;

    return Result;
}

Internal void LinuxCreateHiddenCursor(Display* display, Window window) {
    Pixmap Blank;
    XColor Dummy;
    char BlankBytes[1] = { 0x00 };
    Blank = XCreateBitmapFromData(display, window, BlankBytes, 1, 1);
    HIDDEN_CURSOR = XCreatePixmapCursor(
        display,
        Blank,
        Blank,
        &Dummy,
        &Dummy,
        0,
        0
    );

    XFreePixmap(display, Blank);
}

Internal void LinuxHideCursor(Display* display, Window window) {
    XDefineCursor(display, window, HIDDEN_CURSOR);
}

Internal void LinuxShowCursor(Display* display, Window window) {
    XUndefineCursor(display, window);
}

Internal inline Vector2* LinuxGetMousePosition(
    Display* display,
    Window window
) {
    Window RetRoot, RetWin;

    int32_t RootX, RootY;
    int32_t WinX, WinY;
    uint32_t Mask;

    bool32 QuerySuccess = XQueryPointer(
        display,
        window,
        &RetRoot,
        &RetWin,
        &RootX,
        &RootY,
        &WinX,
        &WinY,
        &Mask
    );

    Vector2* Result;
    if (QuerySuccess) {
        Result = new Vector2((real32)WinX, (real32)WinY);
    }

    return Result;
}

Internal void ToggleFullscreen(Display* display, Window window) {
    FULLSCREEN = !FULLSCREEN;
    Atom FullscreenAtom = XInternAtom(
        display,
        "_NET_WM_STATE_FULLSCREEN",
        False
    );

    Atom WindowState = XInternAtom(display, "_NET_WM_STATE", False);
    int Mask = SubstructureNotifyMask | SubstructureRedirectMask;
    XEvent event = {};
    event.xclient.type = ClientMessage;
    event.xclient.serial = 0;
    event.xclient.send_event = True;
    event.xclient.window = window;
    event.xclient.message_type = WindowState;
    event.xclient.format = 32;
    event.xclient.data.l[0] = (
        FULLSCREEN ?
            _NET_WM_STATE_ADD
            : _NET_WM_STATE_REMOVE
    );
    event.xclient.data.l[1] = (long)FullscreenAtom;
    event.xclient.data.l[2] = 0;

    XSendEvent(display, DefaultRootWindow(display), False, Mask, &event);
}

Internal void LinuxProcessKeyboardMessage(
    button_state* NewState,
    bool32 IsDown
) {
    if (NewState->_endedDown != IsDown) {
        NewState->_endedDown = IsDown;
        ++NewState->_halfTransitionCount;
    }
}

Internal void LinuxProcessPendingMessages(
    Display* display,
    Window window,
    Atom WmDeleteWindow,
    keyboard_controller* KeyboardController,
    user_input* Input
) {
    while(RUNNING && XPending(display)) {
        XEvent Event;
        XNextEvent(display, &Event);

        // Don't skip the scroll key Events
        if(Event.type == ButtonRelease) {
            if (
                (Event.xbutton.button != 4)
                && (Event.xbutton.button != 5)
                && XEventsQueued(display, QueuedAfterReading)
            ) {
                // Skip the auto repeat key
                XEvent NextEvent;
                XPeekEvent(display, &NextEvent);
                if (
                    (NextEvent.type == ButtonPress)
                    && (NextEvent.xbutton.time == Event.xbutton.time)
                    && (NextEvent.xbutton.button == Event.xbutton.button)
                ) { continue; }
            }
        }

        if(Event.type == KeyRelease
           && XEventsQueued(display, QueuedAfterReading)
        ) {
            XEvent NextEvent;
            XPeekEvent(display, &NextEvent);
            if (
                (NextEvent.type == KeyPress)
                && (NextEvent.xbutton.time == Event.xbutton.time)
                && (NextEvent.xbutton.button == Event.xbutton.button)
            ) { continue; }
        }

        switch (Event.type) {
            case DestroyNotify: {
                XDestroyWindowEvent *destroyEvent =
                    (XDestroyWindowEvent *) &Event;

                if (destroyEvent->window == window) { RUNNING = false; }
            } break;

            case ClientMessage: {
                if ((Atom)Event.xclient.data.l[0] == WmDeleteWindow) {
                    XDestroyWindow(display, window);
                    RUNNING = false;
                }
            } break;

            case ConfigureNotify: {
                XConfigureEvent *configureEvent = (XConfigureEvent *)&Event;
                ResizeOffscreenBuffer(
                    &BACK_BUFFER,
                    configureEvent->width,
                    configureEvent->height
                );
            } break;

            case ButtonRelease:
            case ButtonPress: {
                if (Event.xbutton.button == 1) {
                    LinuxProcessKeyboardMessage(
                        &Input->MouseButtons[MouseButton_Left],
                        Event.type == ButtonPress
                    );
                }
                else if (Event.xbutton.button == 2) {
                    LinuxProcessKeyboardMessage(
                        &Input->MouseButtons[MouseButton_Middle],
                        Event.type == ButtonPress
                    );
                }
                else if (Event.xbutton.button == 3) {
                    LinuxProcessKeyboardMessage(
                        &Input->MouseButtons[MouseButton_Right],
                        Event.type == ButtonPress
                    );
                }
                else if (Event.xbutton.button == 4) {
                    ++Input->_mouseZ;
                }
                else if (Event.xbutton.button == 5) {
                    --Input->_mouseZ;
                }
                else if (Event.xbutton.button == 8) {
                    LinuxProcessKeyboardMessage(
                        &Input->MouseButtons[MouseButton_Extended0],
                        Event.type == ButtonPress
                    );
                }
                else if (Event.xbutton.button == 9) {
                    LinuxProcessKeyboardMessage(
                        &Input->MouseButtons[MouseButton_Extended1],
                        Event.type == ButtonPress);
                }
            } break;

            case KeyRelease:
            case KeyPress: {
                bool32 AltKeyWasDown = Event.xkey.state & KEYCODE_ALT_MASK;
                bool32 ShiftKeyWasDown = Event.xkey.state & KEYCODE_SHIFT_MASK;
		bool32 isDown = (Event.type == KeyPress);

                if (Event.xkey.keycode == KEYCODE_W) {
                    LinuxProcessKeyboardMessage(
                        &KeyboardController->_Up,
                        isDown
                    );
                }
                else if (Event.xkey.keycode == KEYCODE_A) {
                    LinuxProcessKeyboardMessage(
                        &KeyboardController->_Left,
                        isDown
                    );
                }
                else if (Event.xkey.keycode == KEYCODE_S) {
                    LinuxProcessKeyboardMessage(
                        &KeyboardController->_Down,
                        isDown
                    );
                }
                else if (Event.xkey.keycode == KEYCODE_D) {
                    LinuxProcessKeyboardMessage(
                        &KeyboardController->_Right,
                        isDown
                    );
                }
                else if (Event.xkey.keycode == KEYCODE_Q) {
                    LinuxProcessKeyboardMessage(
                        &KeyboardController->_LeftShoulder,
                        isDown
                    );
                }
                else if (Event.xkey.keycode == KEYCODE_E) {
                    LinuxProcessKeyboardMessage(
                        &KeyboardController->_RightShoulder,
                        isDown
                    );
                }
                else if (Event.xkey.keycode == KEYCODE_ESCAPE) {
                    LinuxProcessKeyboardMessage(
                        &KeyboardController->_Escape,
                        isDown
                    );
                }
                else if (Event.xkey.keycode == KEYCODE_SPACE) {
                    LinuxProcessKeyboardMessage(
                        &KeyboardController->_Space,
                        isDown
                    );
                }
                else if (
                    (Event.xkey.keycode == KEYCODE_SHIFT_L)
                    || (Event.xkey.keycode == KEYCODE_SHIFT_R)
                ) { Input->_shiftDown = isDown; }
                else if (
                    (Event.xkey.keycode == KEYCODE_ALT_L)
                    || (Event.xkey.keycode == KEYCODE_ALT_R)
                ) { Input->_altDown = isDown; }
                else if (
                    (Event.xkey.keycode == KEYCODE_CTRL_L)
                    || (Event.xkey.keycode == KEYCODE_CTRL_R)
                ) { Input->_controlDown = isDown; }
                else if (Event.xkey.keycode == KEYCODE_P) {
                    if (isDown) { PAUSE = !PAUSE; }
                }

                if (Event.type == KeyPress) {
                    if (
                        (Event.xkey.keycode == KEYCODE_ENTER)
                        && AltKeyWasDown
                ) { ToggleFullscreen(display, window); }
                    else if (
                        (Event.xkey.keycode >= KEYCODE_F1)
                        && (Event.xkey.keycode <= KEYCODE_F10)
                    ) {
                        Input->_FKeyPressed[
                            Event.xkey.keycode - KEYCODE_F1
                        ] = true;
                    }
                    else if (
                        (Event.xkey.keycode >= KEYCODE_F11)
                        && (Event.xkey.keycode <= KEYCODE_F12)
                    ) {
                        // Because of X11 mapping we get to do the
                        // function keys in 2 steps
                        Input->_FKeyPressed[
                          Event.xkey.keycode - KEYCODE_F11
                        ] = true;
                    }
                }
            } break;

        default:
            break;
        }
    }
}

int main()
{
    LinuxState OSState;

    RUNNING = false;

    BACK_BUFFER = CreateOffscreenBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);

    Display* display = XOpenDisplay(NULL);
    if (! display) {
        fprintf(stderr, "Failed to open display\n");
        exit(1);
    }

    WINDOW_POSITION_X = 0;
    WINDOW_POSITION_Y = 0;
    real32 GameUpdateHz = (real32)(MONITOR_REFRESH_HZ / 2.0f);

    real32 TargetSecondsPerFrame = ((real32)EXPECTED_FRAMES_PER_UPDATE
        / GameUpdateHz);

    XVisualInfo Visual = {};
    Visual.screen = DefaultScreen(display);
    Visual.depth = 24;
    bool32 visualInfoTaken = XMatchVisualInfo(
       display,
       Visual.screen,
       Visual.depth,
       TrueColor,
       &Visual
    );
    if(! XMatchVisualInfo(
           display,
           Visual.screen,
           Visual.depth,
           TrueColor,
           &Visual
        )
    ) {
        fprintf(
            stderr,
            "Failed to get Visual information %s.\n",
            "from Frame buffer configurations"
        );
        exit(2);
    }

    Window RootWindow = RootWindow(display, Visual.screen);

    XSetWindowAttributes WindowAttribs = {};
    WindowAttribs.colormap = XCreateColormap(
        display,
        RootWindow,
        Visual.visual,
        AllocNone
    );
    WindowAttribs.border_pixel = 0;
    WindowAttribs.event_mask = (
        StructureNotifyMask |
        PropertyChangeMask  |
        ButtonPressMask     |
        ButtonReleaseMask   |
        KeyPressMask        |
        KeyReleaseMask
    );

    Window XWindow = XCreateWindow(
        display,
        RootWindow,
        WINDOW_POSITION_X, WINDOW_POSITION_Y,
        BACK_BUFFER._Width, BACK_BUFFER._Height,
        0,
        Visual.depth,
        InputOutput,
        Visual.visual,
        CWBorderPixel | CWBitGravity | CWBackPixel | CWColormap | CWEventMask,
        &WindowAttribs
    );

    if(! XWindow) {
        fprintf(stderr, "Failed to create window.\n");
        exit(2);
    }

    XSizeHints SizeHints = {};
    SizeHints.x      = WINDOW_POSITION_X;
    SizeHints.y      = WINDOW_POSITION_Y;
    SizeHints.width  = BACK_BUFFER._Width;
    SizeHints.height = BACK_BUFFER._Height;
    SizeHints.flags  = USSize | USPosition;

    XSetNormalHints(display, XWindow, &SizeHints);
    XSetStandardProperties(
        display,
        XWindow,
        "Game engine",
        "glsync text",
        None,
        0,
        0,
        &SizeHints
    );

    Atom WmDeleteWindow = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, XWindow, &WmDeleteWindow, 1);

    LinuxCreateHiddenCursor(display, XWindow);
    LinuxShowCursor(display, XWindow);
#if DEBUGGING
    // When DEBUGGING
    // Shows OS cursor
    LinuxHideCursor(display, XWindow);
#endif
    ToggleFullscreen(display, XWindow);

    void *BaseAddress = 0;
    engine_memory EngineMemory = {};
    EngineMemory._permanentStorageSize = MEGABYTES(64);
    EngineMemory._transientStorageSize = GIGABYTES(4);

    EngineMemory._PlatformReadEntireFile  = PlatformReadEntireFile;
    EngineMemory._PlatformFreeFileMemory  = PlatformFreeFileMemory;
    EngineMemory._PlatformWriteEntireFile = PlatformWriteEntireFile;

    OSState.setTotalSize(
        EngineMemory._permanentStorageSize
        + EngineMemory._transientStorageSize
    );
    OSState.mmapEngineMemoryBlock(BaseAddress, OSState.getTotalSize());

    EngineMemory._permanentStorage = OSState.getEngineMemoryBlock();
    EngineMemory._transientStorage = (
        (uint8_t *)EngineMemory._permanentStorage
        + EngineMemory._permanentStorageSize
    );

    if(! (EngineMemory._permanentStorage && EngineMemory._transientStorage)) {
        fprintf(stderr, "Failed to allocate memory.\n");
        exit(3);
    }

    linux_engine_code Engine = {};
    LinuxLoadEngineCode(
        &Engine,
        OSState.getSourceEngineCodeDLLAbsFilepath(),
        LinuxFileId(OSState.getSourceEngineCodeDLLAbsFilepath())
    );

    RUNNING = true;

    user_input Input[2] = {};
    user_input *NewInput = &Input[0];
    user_input *OldInput = &Input[1];

    struct timespec LastCounter = LinuxGetWallClock();
    struct timespec FlipWallClock = LinuxGetWallClock();

    XMapRaised(display, XWindow);
    XFlush(display);

    while (RUNNING) {
        ino_t EngineLibId = LinuxFileId(
            OSState.getSourceEngineCodeDLLAbsFilepath()
        );
        if(EngineLibId != Engine.LibID) {
            bool32 IsValid = false;
            for (uint32_t attempt = 0; !IsValid && (attempt < 100); ++attempt) {
            IsValid = LinuxLoadEngineCode(
                &Engine,
                OSState.getSourceEngineCodeDLLAbsFilepath(),
                EngineLibId
            );
            fprintf(stderr, "Reloading %s...\n", OSState.getSourceEngineCodeDLLAbsFilepath().c_str());
            if(IsValid) { fprintf(stderr, "  Success!\n"); }
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
          }

        }

        NewInput->_dtForFrame = TargetSecondsPerFrame;

        linux_window_dimension Dimension = LinuxGetWindowDimension(
            display,
            XWindow
        );

        NewInput->_keyboard = {};

        for (uint32_t ButtonIndex = 0;
             ButtonIndex < ArrayCount(NewInput->_keyboard._Buttons);
             ++ButtonIndex
        ) {
            NewInput->_keyboard._Buttons[ButtonIndex]._endedDown =
            OldInput->_keyboard._Buttons[ButtonIndex]._endedDown;
        }

        for (uint32_t ButtonIndex = 0;
             ButtonIndex < PlatformMouseButton_Count;
             ++ButtonIndex
        ) {
            NewInput->MouseButtons[ButtonIndex] = OldInput->MouseButtons[
                ButtonIndex
            ];
            NewInput->MouseButtons[ButtonIndex]._halfTransitionCount = 0;
        }

        Vector2 *MouseP = LinuxGetMousePosition(display, XWindow);
        NewInput->_mouseX = (real32)(MouseP->getX());
        NewInput->_mouseY = (real32)(MouseP->getY());
        NewInput->_mouseZ = 0.0f;

        keyboard_controller *KeyboardController = &NewInput->_keyboard;
        LinuxProcessPendingMessages(
            display,
            XWindow,
            WmDeleteWindow,
            &NewInput->_keyboard,
            NewInput
        );

        back_buffer Buffer = {};
        Buffer._memory        = BACK_BUFFER._Memory;
        Buffer._width         = BACK_BUFFER._Width;
        Buffer._height        = BACK_BUFFER._Height;
        Buffer._pitch         = BACK_BUFFER._Pitch;
        Buffer._bytesPerPixel = BYTES_PER_PIXEL;

        XImage *XWindowBuffer = XCreateImage(
            display,
            Visual.visual,
            Visual.depth,
            ZPixmap,
            0,
            (char *)BACK_BUFFER._Memory,
            BACK_BUFFER._Width, BACK_BUFFER._Height,
            32,
            0
        );
        GC DefaultGC = XDefaultGC(display, Visual.screen);

        // Wait frames
        struct timespec WorkCounter = LinuxGetWallClock();
        real32 WorkSecondsElapsed = LinuxGetSecondsElapsed(
            LastCounter,
            WorkCounter
        );

        real32 SecondsElapsedForFrame = WorkSecondsElapsed;
        if (SecondsElapsedForFrame < TargetSecondsPerFrame) {
            uint32_t Sleep = (uint32_t)(
                (TargetSecondsPerFrame - SecondsElapsedForFrame)
            );
            std::this_thread::sleep_for(std::chrono::milliseconds(Sleep));

            while (SecondsElapsedForFrame < TargetSecondsPerFrame) {
              SecondsElapsedForFrame = LinuxGetSecondsElapsed(
                  LastCounter,
                  LinuxGetWallClock()
              );
            }
        } else {
            fprintf(stderr, "ERROR: Missed frame rate!\n");
        }

        if(! PAUSE) {
            Engine.UpdateAndRender(&EngineMemory, Input, &Buffer);

            XPutImage(
                display,
                XWindow,
                DefaultGC,
                XWindowBuffer, 0, 0, 0, 0,
                Buffer._width, Buffer._height
            );

            if(Input->_quitRequested) RUNNING = false;
        }

        user_input *Temp = NewInput;
        NewInput = OldInput;
        OldInput = Temp;

	LastCounter = LinuxGetWallClock();

        delete MouseP;
    }

    LinuxUnloadEngineCode(&Engine);
    munmap(EngineMemory._permanentStorage, OSState.getTotalSize());
    XCloseDisplay(display);
    return 0;
}
