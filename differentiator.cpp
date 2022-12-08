#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "log.h"
#include "dump.h"
#include "html.h"
#include "tex.h"
#include "plot.h"


#define dL Diff(node->left)
#define dR Diff(node->right)
#define cL CopyNode(node->left)
#define cR CopyNode(node->right)


#define ADD(nodeL, nodeR) NEW_OP_NODE(OP_ADD, nodeL, nodeR)
#define SUB(nodeL, nodeR) NEW_OP_NODE(OP_SUB, nodeL, nodeR)
#define MUL(nodeL, nodeR) NEW_OP_NODE(OP_MUL, nodeL, nodeR)
#define DIV(nodeL, nodeR) NEW_OP_NODE(OP_DIV, nodeL, nodeR)
#define DEG(nodeL, nodeR) NEW_OP_NODE(OP_DEG, nodeL, nodeR)

#define SIN(nodeL) NEW_OP_NODE(OP_SIN, nodeL, NULL)
#define COS(nodeL) NEW_OP_NODE(OP_COS, nodeL, NULL)
#define TG(nodeL) NEW_OP_NODE(OP_TG, nodeL, NULL)
#define CTG(nodeL) NEW_OP_NODE(OP_CTG, nodeL, NULL)

#define NEW_OP_NODE(OP, nodeL, nodeR) CreateNode(OP_TYPE, OP, 0, NULL, nodeL, nodeR)

#define VAL(val) CreateNode(VAL_TYPE, -1, val, NULL, NULL, NULL)

#define IS_OP  node->value->type == OP_TYPE
#define IS_VAL node->value->type == VAL_TYPE
#define IS_VAR node->value->type == VAR_TYPE 

#define IS_UN (node->value->op >= OP_SIN && node->value->op <= OP_CTG) 

#define CUR_OP   node->value->op
#define CUR_VAL  node->value->dbl
#define CUR_VAR  node->value->varValue
#define CUR_TYPE node->value->type

#define R_VAL node->right->value->dbl
#define L_VAL node->left->value->dbl

#define L_TYPE node->left->value->type
#define R_TYPE node->right->value->type

#define IS_VAL_L node->left->value->type == VAL_TYPE
#define IS_VAL_R node->right->value->type == VAL_TYPE

#define IS_LEFT node->parent->left == node
#define IS_RIGHT node->parent->right == node
                                              
struct node* GetG(char* DiffData);
struct node* GetE();
struct node* GetN();
struct node* GetT();
struct node* GetP();
struct node* GetD();

int GetOperator();
char* GetOpName(int op);

const char* s = NULL;
const char png[5] = ".png";

void TreeCtor(struct Tree* tree, struct DiffNode* rootData);
void TreeDtor(struct Tree* tree);
void NodesDtor(struct node* node);

struct node* CreateNode(int type, int op, double dbl, char *varValue, struct node* nodeL, struct node* nodeR);

size_t SetDiffValue(struct node* node, const char* DiffData);

void readDiffData(struct Tree* tree);
void ScanDiffData(const char* DiffData, struct node* node);

void setHTML(const char* image);
void DrawTree(struct node* node, FILE* DumpFile);
void GraphTreeDump(struct node* root);
void ConsoleOutput(struct node* node);

char* IntToString(int num);

struct node* Diff(const struct node* node);
struct node* CopyNode(struct node* node);

void RecursiveOptimize(struct node* node);
void OptimizeTree(struct node** node_ptr);
double CalculateValue(int OP, double val1, double val2);
void replaceNodes(struct node* node, struct node* newNode);

int CalcTreeHeight(struct node* node, int curHeight);

void PrintTex(struct node* node);
void recursTex(struct node* node);
void TexUnOp(struct node* node);
void TexCalcOp(struct node* node);

void PrintPlot(struct node* node, double leftX, double rightX);
void recursPlot(struct node* node);
void TexUnOp(struct node* node);
void TexCalcOp(struct node* node);

void SubstituteX(struct node* node, double value);
void freeSubTree(struct node* node);
void freeNode(struct node* node);

char *TEX_COMMANDS[50] = {
    "\\sin{",
    "\\cos{",                            //DONT CHANGE THE ORDER, ESSENTIAL FOR PRINT TEX
    "\\tg{", 
    "\\ctg{",
    "+{",
    "-{",
    "\\cdot{",
    "^{"
};

char *PLOT_COMMANDS[50] = {
    "np.sin(",
    "np.cos(",
    "np.tg(",
    "np.ctg(",                           //DONT CHANGE THE ORDER, ESSENTIAL FOR PRINT PLOT
    "+",
    "-",
    "*",
    "**",
    "/"
};


static size_t imageNum = 1;

struct node {
    struct DiffNode*  value;
    struct node*     parent;
    struct node*       left;
    struct node*      right;
};

struct Tree {
    struct node* root;
};

enum Types
{
    OP_TYPE,
    VAL_TYPE,
    VAR_TYPE
};

enum Op_types {
    OP_SIN,
    OP_COS,
    OP_TG,
    OP_CTG,                                    //DONT CHANGE THE ORDER, ESSENTIAL FOR PRINT TEX
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DEG,
    OP_DIV,
};

struct DiffNode {
    int        type;
    double      dbl;
    int          op;
    char*  varValue;
};



int main() {
    
    struct Tree tree = {nullptr};
    TreeCtor(&tree, NULL);
    printf("done ctoring tree\n\n");

    readDiffData(&tree);
    printf("done reading tree\n\n");

    PrintPlot(tree.root, -10, 10);
    PrintTex(tree.root);

    TreeDtor(&tree);
    printf("done dtoring tree\n");
}

void freeNode(struct node* node) {
    assert(node != NULL);

    free(node->value);
    logprint("done free node->value\n");
    free(node);
    logprint("done freeNode\n\n");
}

void freeSubTree(struct node* node) {
    assert(node != NULL);

    if (node->left)  freeSubTree(node->left);
    if (node->right) freeSubTree(node->right);

    //freeNode(node);
}

void SubstituteX(struct node* node, double value) {
    if (IS_VAR) {
        struct node* valNode = VAL(value);
        replaceNodes(node, valNode);
        //freeNode(node);
    }
    if (node->left) SubstituteX(node->left, value);
    if (node->right) SubstituteX(node->right, value);
}

void TexUnOp(struct node* node) {
    texPr(TEX_COMMANDS[CUR_OP]);
    if (node->left->left)
        texPr("(");
    recursTex(node->left);
    if (node->left->left)
        texPr(")");
    texPr("}");
}

void TexCalcOp(struct node* node) {
    recursTex(node->left);
    texPr(TEX_COMMANDS[CUR_OP]);
    recursTex(node->right);
    texPr("}");
}

void PlotUnOp(struct node* node) {
    logprint("enter in PlotUnOp, cur op is %d\n", CUR_OP);
    PlotPr(PLOT_COMMANDS[CUR_OP]);
    logprint("in PlotUnOp output is %s\n", PLOT_COMMANDS[CUR_OP]);
    recursPlot(node->left);
    PlotPr(")\n");
}

void PlotCalcOp(struct node* node) {
    recursPlot(node->left);
    PlotPr(PLOT_COMMANDS[CUR_OP]);
    logprint("in PlotCalcOp output is %s\n", PLOT_COMMANDS[CUR_OP]);
    PlotPr("(");
    recursPlot(node->right);
    PlotPr(")");
}

void PrintPlot(struct node* node, double leftX, double rightX) {
    PlotFile = openPlot();
    PlotPr("import numpy as np\n");
    PlotPr("import matplotlib.pyplot as plt\n");
    PlotPr("x = np.linspace(%lf, %lf, 200)\n", leftX, rightX);
    PlotPr("y = ");
    recursPlot(node);
    PlotPr("\n");
    logprint("done recurs node\n");

    PlotPr("plt.figure(figsize=(8,6), dpi=100)\n");
    PlotPr("plt.grid(True, linestyle=\"--\")\n");
    PlotPr("plt.axis([%lf, %lf, np.min(y)-0.25, np.max(y)+0.25])\n", leftX, rightX);
    PlotPr("plt.plot(x, y, \"-m\",linewidth=1)\n");
    PlotPr("plt.savefig('graphFunc.png')");

    system("py plot.py");

    ClosePlot(PlotFile);
}

void recursPlot(struct node* node) {
    if (IS_OP) {
        if (IS_UN) {
            logprint("in plot op is UN\n");
            PlotUnOp(node);
        } else { 
            logprint("in plot op NOT is un\n");
            PlotCalcOp(node);
        }
    } else if (IS_VAL) {
        logprint("in plot is VAL\n");
        if (CUR_VAL < 0.0) PlotPr("(");
        PlotPr("%.1lf", CUR_VAL);
        if (CUR_VAL < 0.0) PlotPr(")");
    } else if (IS_VAR) {
        logprint("in plot is VAR\n");
        PlotPr("%s", CUR_VAR);
    }
}

void recursTex(struct node* node) {
     if (IS_OP) {
        if (IS_UN)
            TexUnOp(node);
        else if (CUR_OP == OP_DIV) {
            texPr("\\frac{");
            recursTex(node->left);
            texPr("}{");
            recursTex(node->right);
            texPr("}");
        } else 
            TexCalcOp(node);
    } else if (IS_VAL) {
        if (CUR_VAL < 0.0) texPr("(");
        texPr("%lg", CUR_VAL);
        if (CUR_VAL < 0.0) texPr(")");
    } else if (IS_VAR) {
        texPr("%s", CUR_VAR);
    }
}

void PrintTex(struct node* node) {
    texFile = openTex();

    OptimizeTree(&node);
    logprint("**original node is optimized**\n");
    GraphTreeDump(node);

    struct node* origin = CopyNode(node);
    struct node* origin1 = CopyNode(node);

    texPr("Ну что там опять... Ещё одна говно-функция? Как же ты меня зае...");
    texPr("\\section{Функция}");
    texPr("Давай взглянем, что же ты там приготовил в этот раз.");

    texPr("$ ");
    texPr("f(x) = ");
    recursTex(origin);
    texPr(" $\n");
    texPr("\\par\n");

    texPr("Знаешь, я буду с тобой абсолютно честен. Я вот смотрю на эту функцию, и у меня какая-то агрессия, зубы скрипят...\n");
    texPr(" \\par\nНу всё. Вывалил. Давай, удачи. \\par \n");
    
    int n = 5;
    double* dirVals = (double*)calloc(n+1, sizeof(double));

    texPr("В СМЫСЛЕ БЛ ВЗЯТЬ %d ПРОИЗВОДНУЮ, ТЫ ВАЩЕ ПОПУТАЛ ЧЕРТ ЛЫСЫЙ? Я Ж ТЕБЯ ЩАС ЗАХ...\n", n);
    texPr("\\section{Так... Вдох-выдох... Производные}");
    texPr("Постарайся воспринимать с первого раза, сосунок, дважды повторять не буду.\\par\n");


    for (int i = 1; i <= n; i++) {
        struct node* diff = Diff(node);
        //GraphTreeDump(diff);

        logprint("\n**trying to optimize %d diff**\n", i);
        OptimizeTree(&diff);
        logprint("**%d th diff is optimized**\n\n", i);
        
        texPr("%s\\par \n", FUNNY_WORDS[i-1]);
        texPr("$");
        texPr("f^{(%d)}(x) = ", i);
        recursTex(diff);
        texPr("$\n");
        texPr("\\par\n");

        SubstituteX(diff, 0);  
        //GraphTreeDump(diff);
        OptimizeTree(&diff);

        dirVals[i] = diff->value->dbl;
        
        node = Diff(node);
    }

    texPr("Фуххххх, давно я так не потел... Ну ладно, всё хорошо, что хорошо кончается. \\par (не) Рад был помочь, бывай. ");
    texPr("бл что? какой нах маклорен...");

    texPr("\\section{Ряд Маклорена}");
    texPr("братишка, я так больше не могу... Мне вставать завтра рано. Отпусти, пожалуйста...\\par\n");
    texPr("Я бл не всегда до $o(x^{1})$-то могу вспомнить, а ты меня просишь до $o(x^{%d})$. Эх, попробуем.\\par\n", n);
    texPr("Производные я уже все нашёл, поэтому осталось просто подставить в них x = 0. Я надеюсь хоть с этим твой наномозг справится... \\par\n");
    texPr("Нет? мдааааа... Ну что ж, внимай.\\par\n");
    
    texPr("Для начала подставляем x = 0 в f(x): \\par\n");

    SubstituteX(origin, 0);

    texPr("$ ");
    texPr("f(0) = ");
    recursTex(origin);
    texPr("$\n");
    texPr("\\par\n");

    texPr("Калькулируем в уме за 3 наносек:\\par \n");
    OptimizeTree(&origin);
    dirVals[0] = origin->value->dbl;

    texPr("$ ");
    texPr("f(0) = ");
    recursTex(origin);
    texPr("$\n");
    texPr("\\par\n");

    texPr("Теперь подставляем x = 0 во все %d производных. \\par\n", n);
    texPr("И да, я уже понял, что ты не сильно сообразительнее вон того кирпича, поэтому вот тебе сразу готовые значения:\\par\n");

    for (int i = 1; i <= n; i++) {
        texPr("$f^{(%d)}(0) = %lg$\\par\n", i, dirVals[i]);
    }
    texPr("\\par\n");

    texPr("Ну всё, теперь всё готово для разложения f(x) в Маклорена\\par\n");
    texPr("$");
    recursTex(origin1);
    texPr(" = %lg ", dirVals[0]);
    for (int i = 1; i <= n; i++) {
        texPr(" + \\frac{%lg \\cdot x^{%d}}{%d!}", dirVals[i], i, i);
    }
    texPr(" +o(x^{%d})", n);
    texPr("$\n");

    texPr("Ладно, давай по старой дружбе приведу в божеский вид, чтобы ты, бедняжка, в трёх циферках не запутался.\\par\n");
    texPr("$");
    recursTex(origin1);
    texPr(" = %lg ", dirVals[0]);
    for (int i = 1; i <= n; i++) {
        if (dirVals[i] != 0)
            texPr(" + \\frac{%lg \\cdot x^{%d}}{%d!}", dirVals[i], i, i);
    }
    texPr(" +o(x^{%d})", n);
    texPr("$\n");
    texPr("\\par");

    logprint("done recurs tex\n");

    texPr("\\section{Ну что, давай взглянем на эту чушь}\n");
    texPr("\\begin{center}\n");
    texPr("\\includegraphics{graphFunc.png}\n");
    texPr("\\end{center}\n");
    texPr("\\par");
    texPr("Ну всё, бездарь, на этом мои полномочия всё. Надеюсь, мы никогда больше не увидимся. Всего плохого.");

    CloseTex(texFile);

    system("pdflatex texFile.tex");
}

void replaceNodes(struct node* node, struct node* newNode) {
    if (node->parent) {
            newNode->parent = node->parent;
            if (IS_LEFT) 
                node->parent->left  = newNode;
            else
                node->parent->right = newNode;
    }
}

void OptimizeTree(struct node** node_ptr) {
    struct node* node = *node_ptr;
    int height = CalcTreeHeight(node, 1);
    logprint("Height is %d\n", height);

    while (height > 0) {
        height--;
        RecursiveOptimize(node);
        //GraphTreeDump(node);
    }

    logprint("done recursive optimize\n");
    if (IS_OP && IS_VAL_L) {
        double res = 0.0;
        if (IS_UN){
            res = CalculateValue(CUR_OP, L_VAL, 0.0);
            *node_ptr = VAL(res);
        }
        else if (IS_VAL_R){
            res = CalculateValue(CUR_OP, L_VAL, R_VAL);
            *node_ptr = VAL(res);
        }
    }
}

double CalculateValue(int OP, double val1, double val2) {
    switch (OP) {
    case OP_ADD:
        return val1+val2;
    case OP_SUB:
        return val1-val2;
    case OP_MUL:
        return val1*val2;
    case OP_DIV:
        return val1/val2;
    case OP_DEG:
        return pow(val1, val2);
    case OP_SIN:
        return sin(val1);
    case OP_COS:
        return cos(val1);
    case OP_TG:
        return tan(val1);
    case OP_CTG:
        return 1/tan(val1); 
    }
}

void RecursiveOptimize(struct node* node) {
    if (IS_OP) {
        if (IS_UN) {
            if (IS_VAL_L) {
                double res = CalculateValue(CUR_OP, L_VAL, 0);
                struct node* valNode = VAL(res);
                replaceNodes(node, valNode);

                logprint("free on %d\n", __LINE__);
                //freeNode(node);
                return;
            }
            else RecursiveOptimize(node->left);
        } else {

            if ((IS_VAL_L) && (IS_VAL_R)) {

                double res = CalculateValue(CUR_OP, L_VAL, R_VAL);
                struct node* valNode = VAL(res);
                replaceNodes(node, valNode);

                logprint("free on %d\n", __LINE__);
                //freeNode(node);
                node = valNode;
                return ;
            }
            if (IS_VAL_L && L_VAL == 0.0 && CUR_OP == OP_MUL ||
                IS_VAL_R && R_VAL == 0.0 && CUR_OP == OP_MUL) {

                struct node* nullNode = VAL(0.0);
                replaceNodes(node, nullNode);

                logprint("free on %d\n", __LINE__);
                //freeSubTree(node);
                node = nullNode;
                return;
            }

            if (IS_VAL_R && R_VAL == 1.0 && (CUR_OP == OP_MUL || CUR_OP == OP_DIV || CUR_OP == OP_DEG) ||
                IS_VAL_R && R_VAL == 0.0 && (CUR_OP == OP_ADD || CUR_OP == OP_SUB                   )) {

                replaceNodes(node, node->left);
                
                logprint("free on %d\n", __LINE__);
                logprint("cur node->right->value->dbl = %lf\n", node->right->value->dbl);
                //freeNode(node->right);
                node = node->left;
                RecursiveOptimize(node);
            } 
            else if 
                (IS_VAL_L && L_VAL == 1.0 && CUR_OP == OP_MUL ||
                IS_VAL_L && L_VAL == 0.0 && CUR_OP == OP_ADD) {

                replaceNodes(node, node->right);

                logprint("free on %d\n", __LINE__);
                //freeNode(node->left);
                node = node->right;
                RecursiveOptimize(node);
            }

        if (node->left)  RecursiveOptimize(node->left);
        if (node->right) RecursiveOptimize(node->right);
        }
        
    } else 
        return;
    
}

struct node* Diff(const struct node* node) {

    switch (CUR_TYPE) {

    case VAL_TYPE:
        return CreateNode(VAL_TYPE, -1, 0.0, NULL, NULL, NULL);

    case VAR_TYPE:
        return CreateNode(VAL_TYPE, -1, 1.0, NULL, NULL, NULL);

    case OP_TYPE:

        switch (CUR_OP) {

        case OP_ADD:
            return ADD(dL, dR);

        case OP_SUB:
            return SUB(dL, dR);

        case OP_MUL:
            return ADD(MUL(dL, cR), MUL(cL, dR));

        case OP_DIV:
            return DIV(SUB(MUL(dL, cR), MUL(cL, dR)), DEG(cR, VAL(2.0)));

        case OP_DEG:
            if (R_TYPE == VAL_TYPE)                     //case (f(x))^num
                return MUL(MUL(VAL(R_VAL), DEG(cL, VAL(R_VAL-1))), dL); 
            else if (L_TYPE == VAL_TYPE)                //case num^(f(x))
                return MUL(MUL(DEG(cL, cR), VAL(log(L_VAL))), dR);
    
        case OP_SIN:
            return MUL(COS(cL), dL);

        case OP_COS:
            return MUL(VAL(-1.0), MUL(SIN(cL), dL));

        case OP_TG:
            return MUL(DIV(VAL(1.0), DEG(COS(cL), VAL(2.0))), dL);
        
        case OP_CTG:
            return MUL(VAL(-1.0), MUL(DIV(VAL(1.0), DEG(SIN(cL), VAL(2.0))), dL));
        }
    break;
    }
}

void readDiffData(struct Tree* tree) {

    FILE* SourceFile = NULL;
    if ((SourceFile = fopen("SourceFile.txt", "r")) == NULL)
        printf("cant't open source file\n");

    char* DiffData = (char*)(calloc(50, sizeof(char)));
    assert(DiffData != NULL);

    size_t chars = fread(DiffData, sizeof(char), 100, SourceFile);
    DiffData[chars] = '\0';
    for (int i = 0; i < strlen(DiffData); i++) {
        putchar(DiffData[i]);
    }
    printf("\nsymbols: %d\n", chars);
    tree->root = GetG(DiffData); 
}

void TreeCtor(struct Tree* tree, struct DiffNode* rootData) {
    assert(tree != nullptr);
    //assert(rootData != nullptr);

    struct node* node1 = (struct node*)(calloc(1, sizeof(struct node)));
    node1->value  = rootData;
    node1->left   = nullptr;
    node1->right  = nullptr;
    node1->parent = nullptr;
    tree->root    = node1;
}

void NodesDtor(struct node* node) {
    assert(node != nullptr);

    if (node->left) {
        NodesDtor(node->left); 
    }
    if (node->right) {
        NodesDtor(node->right);
    }

    free(node->value);
    node->value= nullptr;
    node->left = nullptr;
    node->right = nullptr;
    node->parent = nullptr;
}

void TreeDtor(struct Tree* tree) {
    assert(tree != nullptr);

    NodesDtor(tree->root);
    tree->root = nullptr;
}

void GraphTreeDump(struct node* root) {
    assert(root != nullptr);

    FILE* DumpFile = openDump();

    dumpPrint("digraph {\n");
    dumpPrint("graph [dpi = 100]");
    dumpPrint("rankdir = TB;\n");
    dumpPrint("node [ shape=record ];\n");

    DrawTree(root, DumpFile);

    dumpPrint("}");

    fclose(DumpFile);

    char cmd[100] = "dot -T png dump.dot -o ";
    char name[30] = "graphCode";
    char* num = NULL;

    num = IntToString(imageNum);

    strcat(num, png);
    strcat(name, num);
    strcat(cmd, name);

    logprint("%d\n", system("cd C:\\Users\\alexm\\Documents\\Differentiator"));
    logprint("%d\n", system(cmd));

    logprint("RESULT NAME IS %s\n", name);

    setHTML(name);

    imageNum++;
}

void DrawTree(struct node* node,  FILE* DumpFile) {
    assert(node != nullptr);
    assert(DumpFile != nullptr);

    if (IS_OP) {
        char *op = GetOpName(node->value->op);
        dumpPrint("struct%p [\nlabel = \"{op: %s |parent: %p| left: %p| right: %p }\", style = \"filled\", color = \"black\", fillcolor = \"lightgrey\" \n];\n", node, op, node->parent, node->left, node->right);
    } else if (IS_VAL) {
        dumpPrint("struct%p [\nlabel = \"{value: %lf |parent: %p| left: %p| right: %p }\", style = \"filled\", color = \"black\", fillcolor = \"green\" \n];\n", node, node->value->dbl, node->parent, node->left, node->right);
    } else if (IS_VAR) {
        //printf("im in var and cur var is %s and type is %d\n", CUR_VAR, CUR_TYPE);
        dumpPrint("struct%p [\nlabel = \"{var: %s |parent: %p| left: %p| right: %p }\", style = \"filled\", color = \"black\", fillcolor = \"cyan\" \n];\n", node, node->value->varValue, node->parent, node->left, node->right);
    }

    if (node->parent != NULL) {
        dumpPrint("struct%p -> struct%p [weight=900 constraint=true color=red];\n", node->parent, node);
    }
    if (node->left != NULL) {
        DrawTree(node->left, DumpFile);
    }
    if (node->right != NULL) {
        DrawTree(node->right, DumpFile);
    }
}

void setHTML(const char* image) {
    ASSERT(image != nullptr);

    printHTML("<img src = \"%s\">", image);
    printHTML("<hr>");
}

char* IntToString(int num) {
    char* str = (char*)(calloc(10, sizeof(char)));
    int tmp = num;
    int decimal = 1;
    int digits =  0;
    while (tmp > 0) {
        tmp/=10;
        digits++;
        decimal*=10;
    }
    decimal/=10;
    for (int i = 0; i < digits; i++) {
        str[i] = num / decimal + '0';
        num = num%decimal;
        decimal/=10;
    }
    return str;
}

char* GetOpName(int op) {
    char oper[5] = " ";
    switch (op)
    {
    case OP_ADD:
        strcpy(oper, "+");
        break;
    
    case OP_SUB:
        strcpy(oper, "-");
        break;

    case OP_MUL:
        strcpy(oper, "*");
        break;
    
    case OP_DIV:
        strcpy(oper, "/");
        break;

    case OP_DEG:
        strcpy(oper, "^");
        break;

    case OP_SIN:
        strcpy(oper, "sin");
        break;

    case OP_COS:
        strcpy(oper, "cos");
        break;

    case OP_TG:
        strcpy(oper, "tg");
        break;

    case OP_CTG:
        strcpy(oper, "ctg");
        break;
    }

    return strdup(oper);
}

struct node* GetG(char* DiffData) {
    s = DiffData;
    struct node* newNode = GetE();
    assert(*s == '\0');

    return newNode;
}

struct node* GetN() {
    struct node* newNode = NULL;
    double val = 0;
    const char *sOld = s;

    while ('0' <= *s && *s <= '9') {
        val = val * 10 + *s - '0';
        s++;
    }

    //printf("got value %lf\n", val);

    if (s == sOld) {
        if ('a' <= *s && *s <= 'z') {
            char *var = (char*)(calloc(2, sizeof(char)));
            assert(var != NULL);
            var[0] = *s;
            newNode = CreateNode(VAR_TYPE, -1, -1, var, NULL, NULL);
            s++;
        } else {
            printf("ERROR: SYNTAX ERROR\n");
        }
    } else {
        newNode = CreateNode(VAL_TYPE, -1, val, NULL, NULL, NULL);
    }
    //printf("new node created, val is %lf\n\n", newNode->value->dbl);
    return newNode;
}

struct node* GetE() {
    struct node* opNode = NULL;
    struct node* nodeL = GetT();
    
    while (*s == '+' || *s == '-') {
        //printf("in GetE %c is read\n", *s);
        char op = *s;
        s++;

        struct node* nodeR = GetT();
        //printf("\nin GetE nodeR is %lf\n", nodeR->value->dbl);

        if (op == '+'){
            opNode = ADD(nodeL, nodeR);
        } else {
            opNode = SUB(nodeL, nodeR);
        }
        nodeL = opNode;
    }

    return nodeL;
}

struct node* GetT() {
    struct node* opNode = NULL;
    struct node* nodeL = GetD();
    while (*s == '*' || *s == '/') {
        char op = *s;
        s++;
        struct node* nodeR = GetD();
        if (op == '*'){
            opNode = MUL(nodeL, nodeR);
        } else {
            opNode = DIV(nodeL, nodeR);
        }

        nodeL = opNode;
    }

    return nodeL;
}

struct node* GetD() {
    struct node* opNode = NULL;
    struct node* nodeL = GetP();
    while (*s == '^') {
        s++;
        struct node* nodeR = GetP();
        opNode = DEG(nodeL, nodeR);
        nodeL = opNode;
    }

    return nodeL;
}

struct node* GetP() {
    struct node* node1 = NULL;
    if (*s == '(') {
        s++;
        node1 = GetE();
        assert(*s == ')');
        s++;
    } else if (*s == 's' || *s == 'c' || *s == 't') {
        int OP = GetOperator();
        struct node* arg = GetP();
        node1 = NEW_OP_NODE(OP, arg, NULL);
    } else {
        node1 = GetN();
    }
    return node1;
}

int GetOperator(){
    if (*s == 's') {
        if (*(s+1) == 'i' && *(s+2) == 'n') {
            s+=3;
            return OP_SIN;
        }
    } else if (*s == 'c') {
        if (*(s+1) == 'o' && *(s+2) == 's') {
            s+=3;
            return OP_COS;
        } else if (*(s+1) == 't' && *(s+2) == 'g') {
            s+=3;
            return OP_CTG;
        }
    } else if (*s == 't') {
        if (*(s+1) == 'g') {
            s+=2;
            return OP_TG;
        }
    }
}

struct node* CreateNode(int type, int op, double dbl, char *varValue, struct node* nodeL, struct node* nodeR) {

    struct node* newNode = (struct node*)(calloc(1, sizeof(struct node)));
    assert(newNode != NULL);

    newNode->value = (struct DiffNode*)(calloc(1, sizeof(struct DiffNode)));
    assert(newNode->value != NULL);

    if (type == OP_TYPE) {
        newNode->value->type = type;
        newNode->value->op   = op;

    } else if ( type == VAL_TYPE) {
        newNode->value->type = type;
        newNode->value->dbl  = dbl;

    } else if (type == VAR_TYPE) {
        newNode->value->type     = type;
        newNode->value->varValue = varValue;

    } else {
        printf("ERROR: incorrect type of node\n");
        exit(1);
    }
    newNode->left  = nodeL;
    newNode->right = nodeR;
    if (nodeL)
        nodeL->parent = newNode;
    if (nodeR)
        nodeR->parent = newNode;

    return newNode;
}

struct node* CopyNode(struct node* node)
{
    assert(node != NULL);

    struct node* newNode = (struct node*)calloc(1, sizeof(struct node));

    newNode->value = (struct DiffNode*)calloc(1, sizeof(struct node));

    memmove(newNode,        node,        sizeof(node     ));
    memmove(newNode->value, node->value, sizeof(DiffNode ));

    if(node->left) {
        newNode->left = CopyNode(node->left);
        newNode->left->parent = newNode; 
    }

    if( node->right ) {
        newNode->right = CopyNode(node->right);
        newNode->right->parent = newNode; 
    }

    return newNode;
}

int CalcTreeHeight(struct node* node, int curHeight) {
    if (node->left == NULL && node->right == NULL) {
        return curHeight;
    }
    int l_height = 0;
    int r_height = 0;
    if (node->left)
        l_height = CalcTreeHeight(node->left, curHeight + 1);
    if (node->right)
        r_height = CalcTreeHeight(node->right, curHeight + 1);

    return (l_height>r_height?l_height:r_height);
}






