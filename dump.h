#ifndef DUMP_H_INCLUDED
#define DUMP_H_INCLUDED

//#define NDEBUG
#define DUMP_MODE 1

#ifndef NDEBUG
    #define LIST_ASSERT_OK(List)                                                             \
        if (AssertOk(List) || DUMP_MODE)                                                \
        {                                                                               \
            printf("in %s(%d)\n", __PRETTY_FUNCTION__, __LINE__);                       \
            GraphListDump(List, __PRETTY_FUNCTION__, __LINE__);                         \
            ConsoleListDump(List);                                                      \
        }
#else
    #define LIST_ASSERT_OK(List) ;
#endif

#define dumpPrint(str...)                                                 \
    fprintf(DumpFile, str);                                               \
    fflush(DumpFile)

#ifndef DUMP_NAME
    #define DUMP_NAME "dump.dot"
#endif



FILE *openDump();

FILE* openDump() {
    FILE *Dump = fopen(DUMP_NAME, "w");
    if (Dump == NULL)
        printf("ERROR, CAN'T OPEN DUMP FILE\n");
    return Dump;
}

#endif // DUMP_H_INCLUDED
