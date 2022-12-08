#ifndef PLOT_H_INCLUDED
#define PLOT_H_INCLUDED

#define PlotPr(str...)                                                    \
    fprintf(PlotFile, str);                                               \
    fflush(PlotFile)

FILE* PlotFile;
FILE *openPlot();
void ClosePlot(FILE* PlotFile);

FILE* openPlot() {
    FILE *Plot = fopen("plot.py", "w");
    logprint("\n-------------------PlotFile is opened-------------------\n");
    return Plot;
}

void ClosePlot(FILE* texFile) {
    fclose(PlotFile);
    logprint("\n-------------------PlotFile is closed -------------------\n");
}

#endif