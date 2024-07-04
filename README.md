# CLog
CLog is a simple STB-style header-only logging library for C and C++.

## Features:
 - Six logging levels 
    - DEBUG 
    - TRACE 
    - INFO
    - WARNING
    - ERROR
    - FATAL
 - Custom log levels
 - Colored logging
 - Custom formatting
 - Printf style formatting for log messages
 - Logging to an arbitrary file descriptor
 - C99 Compatable
 - Logging file info
 - Timestamps

## Todo Features
 - Timestamps for other OS's
    - Linux(Almost done)
    - MacOS

# Usage
CLog work similarly to the stb libraries.
Wherever you define ``CLOG_IMPLEMENTATION`` the functions will be defined.

CLog uses a basic macro for logging, and an enum for the different levels of logging
```C
#define CLOG_IMPLEMENTATION
#include <clog.h>

int main(void) {
    clog(CLOG_DEBUG,   "Hello from CLog!");
    clog(CLOG_TRACE,   "Hello from CLog!");
    clog(CLOG_INFO,    "Hello from CLog!");
    clog(CLOG_WARNING, "Hello from CLog!");
    clog(CLOG_ERROR,   "Hello from CLog!");
    clog(CLOG_FATAL,   "Hello from CLog!");
}
```

## Muting log levels
If you add this simple line to the demo, any message with the level CLOG_DEBUG or CLOG_TRACE will not show
```C 
clog_mute_level(CLOG_INFO);
```

## Changing log output

This example will log "Hello, World" into a file called "log.log"

Clog does not handle any kind of files, all it needs is a file descriptor.

```C
#include <stdio.h>

#define CLOG_IMPLEMENTATION
#include <clog.h>

int main(void) {
    FILE *f = open("log.log");
    
    clog_set_output(f);
    clog(CLOG_INFO, "Hello, World");
    fclose(f);
}
```

## Preprocessor macros
If you want to disable specific parts of clog, you can define some preprocessor macros to disable those parts
| Macro | Description |
| --- | --- |
| CLOG_NO_TIME | Disables timestamps for clog |
| CLOG_NO_CDECL | Disables the usage of the "cdecl" attribute |

## Formatting
Just as some other logging libraries, this one also supports custom formatting of the output

There are two ways for setting the format. You can eiter set the ```clog_fmt``` variable to the format string you want, or, you can let the ```clog_set_fmt(fmt)``` macro do that for you.

The logger comes with an example format string already implemented:

If you dont want too use timestamps in your project, even when the library supports it, you can define the ```CLOG_NO_TIME``` macro. This will disable all timestamp functionality from the library
```c
"%t: %f:%l -> %c[%L]%r: %m"
```
| Format prefix | Description |
| --- | --- |
| %c | The ansi color escape character for the color of the current level |
| %r | The ansi color escape character to reset the color |
| %m | The message that you provided |
| %L | The log level string |
| %f | The file from which the log was called |
| %l | The line at which the log was called |
| %t | Timestamp |
| %% | The character '%' |

## Timestamps

Timestamps are specified with the format character 't'.

For now timestamps are only implemented for windows. The timestamps for Linux are on the way.

### Timestamp formatting

Timestamps can be formatted with either setting the ```clog_time_fmt``` variable, or calling the ``clog_set_time_fmt`` macro.

The default format string for timing in CLog is:
```c 
"%h:%m:%s.%u"
``` 

| Format prefix | Description |
| --- | --- |
| %h | The current hour |
| %m | The current minute |
| %s | The current second |
| %u | The current millisecond |

## Custom log levels
CLog now allows you to add custom Logging levels to your programs, and the best part? It's relatively easy too!

All you have to do is to use the macro `CLOG_REGISTER_LEVEL` like this:
```C
#include <clog.h>

const ClogLevel MY_CLOG_LEVEL = CLOG_REGISTER_LEVEL("MyClogLevel", "\e[32", 0)
```

Now, the parameters might look intimidating, but they are actually pretty simple! The parameters are:
 - `const char *name` (The name that appears in the log)
 - `const char *color_escape_char` (The color the log gets colored. Use ansi escape characters here, or leave it blank for no color at all)
 - `int severity` (The severity of the message, basically decides when its muted and when its not)

After this, you can just use it the same as you use any other CLog levels