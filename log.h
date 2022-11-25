#ifndef LOG_H_INCLUDED
#define LOG_H_INCLUDED


#ifndef NDEBUG
    #define ASSERT(condition)                                           \
    if(!(condition))                                                    \
    {                                                                   \
    printf("Error in %s\n Place: %s:%d\n FUNC: %s\n",                   \
             #condition, __FILE__, __LINE__, __PRETTY_FUNCTION__);      \
    }
#else
    #define ASSERT(condition) ;
#endif

#ifndef LOG_NAME
    #define LOG_NAME "log.txt"
#endif

#define logprint(str...)                                                 \
    fprintf(LogFile, str);                                               \
    fflush(LogFile)                                                      \


FILE *openLog();
void CloseLog();

FILE* LogFile = openLog();

FILE* openLog() {
    FILE *Log = fopen(LOG_NAME, "w");
    fprintf(Log, "--------------------------------------------------\n");
    fprintf(Log, "log file is opened\n\n");
    atexit(CloseLog);
    return Log;
}

void CloseLog() {
    fprintf(LogFile, "\n\nlog file is closed\n");
    fprintf(LogFile, "--------------------------------------------------\n");
    fclose(LogFile);

}


#endif // LOG_H_INCLUDED
