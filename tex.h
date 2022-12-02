#ifndef TEX_H_INCLUDED
#define TEX_H_INCLUDED

const char* TEX_START =   "\\documentclass[a4paper, 12pt]{article}\n"
                            "\\usepackage[a4paper,top=1.5cm, bottom=1.5cm, left=1cm, right=1cm]"
                            "{geometry}\n"
                            "\\usepackage[utf8]{inputenc}\n"
                            "\\usepackage{mathtext} \n"
                            "\\usepackage[english, russian]{babel} \n"
                            "\\title{Очко себе продифференцируй, дебил}\n"
                            "\\author{Саня, Б01-206}\n"
                            "\\date{\\today}\n";


char* FUNNY_WORDS[50] = {
    "Я знаю, звучит дико, но, ",
    "А вот следующий факт я нахожу просто уморительным: ",
    "Как же мне это надоело... НУ ЧТО НЕ ЯСНО ЧТО ЛИ, ЧТО ",
    "Твои родители случайно не кондитеры?) ДА МНЕ ПО.., ГЛАВНОЕ ЧТО ",
    "я сижу и пишу это в 2.46 ночи сукааааааааа"
};

#define texPr(str...)                                                    \
    fprintf(texFile, str);                                               \
    fflush(texFile)

FILE* texFile;
FILE *openTex();
void CloseTex(FILE* texFile);

FILE* openTex() {
    FILE *Tex = fopen("texFile.tex", "w");
    fprintf(Tex, TEX_START);
    fprintf(Tex, "\\begin{document}\n");
    fprintf(Tex, "\\maketitle\n");
    return Tex;
}

void CloseTex(FILE* texFile) {
    fprintf(texFile, "\\end{document}");
    fclose(texFile);
}

#endif
