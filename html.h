#ifndef HTML_H_INCLUDED
#define HTML_H_INCLUDED

#define printHTML(str...)                                                 \
    fprintf(htmlFile, str);                                               \
    fflush(htmlFile)


#ifndef HTML_NAME
    #define HTML_NAME "dump.html"
#endif

FILE *openHTML();
void CloseHTML();

FILE* htmlFile = openHTML();

FILE* openHTML() {
    FILE *HTML = fopen(HTML_NAME, "w");
    atexit(CloseHTML);
    return HTML;
}

void CloseHTML() {
    fclose(htmlFile);
}

#endif // HTML_H_INCLUDED
