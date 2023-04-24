//
// Created by zhangql on 4/21/23.
//

#ifndef __TIMESTAMP_LOG_H
#define __TIMESTAMP_LOG_H

#ifdef __GNUC__
void _serverLogCustomLogfile(int level, const char *logfile, const char *fmt, ...)
    __attribute__((format(printf, 3, 4)));
#else
void _serverLogCustomLogfile(int level, const char *logfile, const char *fmt, ...);
#endif

void serverLogRawCustomLogfile(int level, const char *msg, const char *logfile);

/* The default level of timestamp log function is LL_WARNING. */
#define serverTimestampLog(...) do {\
        _serverLogCustomLogfile(LL_WARNING, "redis-timestamp.log", __VA_ARGS__);\
    } while(0)

#endif
