//
// Created by zhangql on 4/21/23.
//
#include "server.h"
#include "timestamp_log.h"

/* Custom timestamp log function for node alive check. */
void serverLogRawCustomLogfile(int level, const char *logfile, const char *msg) {
    const int syslogLevelMap[] = { LOG_DEBUG, LOG_INFO, LOG_NOTICE, LOG_WARNING };
    const char *c = ".-*#";
    FILE *fp;
    char buf[64];
    int rawmode = (level & LL_RAW);
    int log_to_stdout = logfile[0] == '\0';

    level &= 0xff; /* clear flags */
    if (level < server.verbosity) return;

    fp = log_to_stdout ? stdout : fopen(logfile,"w");
    if (!fp) return;

    if (rawmode) {
        fprintf(fp,"%s",msg);
    } else {
        int off;
        struct timeval tv;
        int role_char;
        pid_t pid = getpid();

        gettimeofday(&tv,NULL);
        struct tm tm;
        nolocks_localtime(&tm,tv.tv_sec,server.timezone,server.daylight_active);
        off = strftime(buf,sizeof(buf),"%d %b %Y %H:%M:%S",&tm);
        //snprintf(buf+off,sizeof(buf)-off,"%03d",(int)tv.tv_usec/1000);
        if (server.sentinel_mode) {
            role_char = 'X'; /* Sentinel. */
        } else if (pid != server.pid) {
            role_char = 'C'; /* RDB / AOF writing child. */
        } else {
            role_char = (server.masterhost ? 'S':'M'); /* Slave or Master. */
        }
        //demo: 9395:M 21 Feb 2023 17:09:03.087 . Initialize Background Job Type 0
        //fprintf(fp,"%d:%c %s %c %s\n",(int)getpid(),role_char, buf,c[level],msg);
        //demo: last time of node[M] alive check: 02 Mar 2023 17:41:58 ...
        fprintf(fp,"last time of node[%c] alive check ==> #datetime:%s %s\n", role_char, buf, msg);
    }
    fflush(fp);

    if (!log_to_stdout) fclose(fp);
    if (server.syslog_enabled) syslog(syslogLevelMap[level], "%s", msg);
}

/* Custom timestamp log function for node alive check. */
void _serverLogCustomLogfile(int level, const char *logfile, const char *fmt, ...) {
    va_list ap;
    char msg[LOG_MAX_LEN];

    va_start(ap, fmt);
    vsnprintf(msg, sizeof(msg), fmt, ap);
    va_end(ap);

    serverLogRawCustomLogfile(level,logfile,msg);
}