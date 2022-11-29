#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "log.h"
#include "dump.h"
#include "html.h"
#include "tex.h"


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
void OptimizeTree(struct node* node);
double CalculateValue(int OP, double val1, double val2);
void replaceNodes(struct node* node, struct node* newNode);

int CalcTreeHeight(struct node* node, int curHeight);
void PrintTex(struct node* node);

void recursTex(struct node* node);


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
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_DEG,
    OP_SIN,
    OP_COS,
    OP_TG,
    OP_CTG
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

    GraphTreeDump(tree.root);
    printf("done reading graph dump\n");

    OptimizeTree(tree.root);
    GraphTreeDump(tree.root);
    printf("done optimized graph dump\n");

    struct node* differentiated = Diff((const node*)tree.root);
    GraphTreeDump(differentiated);
    printf("done diff graph dump\n");

    OptimizeTree(differentiated);
    GraphTreeDump(differentiated);
    printf("done diff opt graph dump\n");

    printf("\ntrying to teeeeeex\n");
    PrintTex(differentiated);
    printf("tex doooooooooone\n");

    TreeDtor(&tree);
    printf("done dtoring tree\n");
}

void recursTex(struct node* node) {

    if (IS_OP) {
        switch (CUR_OP)
        {
        case OP_ADD:
            recursTex(node->left);
            texPr("+");
            recursTex(node->right);
            break;
        
        case OP_SUB:
            recursTex(node->left);
            texPr("-");
            recursTex(node->right);
            break;

        case OP_MUL:
            recursTex(node->left);
            texPr("\\cdot ");
            recursTex(node->right);
            break;

        case OP_DEG:
            recursTex(node->left);
            texPr("^{");
            recursTex(node->right);
            texPr("}");
            break;

        case OP_DIV:
            texPr("\\frac{");
            recursTex(node->left);
            texPr("}{");
            recursTex(node->right);
            texPr("}");
            break;

        case OP_SIN:
            texPr("\\sin{");
            if (node->left->left)
                texPr("(");
            recursTex(node->left);
            if (node->left->left)
                texPr(")");
            texPr("}");
            break;

        case OP_COS:
            texPr("\\cos{");
            recursTex(node->left);
            texPr("}");
            break;

        case OP_TG:
            texPr("\\tg{");
            recursTex(node->left);
            texPr("}");
            break;

        case OP_CTG:
            texPr("\\ctg{");
            recursTex(node->left);
            texPr("}");
            break;
        }
    } else if (IS_VAL) {
        texPr("%.1lf", CUR_VAL);
    } else if (IS_VAR) {
        texPr("%s", CUR_VAR);
    }
}

void PrintTex(struct node* node) {
    texFile = openTex();

    logprint("trying to recurs tex\n");

    texPr("$");
    recursTex(node);
    texPr("$");

    logprint("done recurs tex\n");

    CloseTex(texFile);

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

void OptimizeTree(struct node* node) {
    int height = CalcTreeHeight(node, 1);
    printf("Height is %d\n", height);
    while (height > 0) {
        height--;
        RecursiveOptimize(node);
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
    }
}

void RecursiveOptimize(struct node* node) {
    if (IS_OP) {
        if (IS_UN) {
            RecursiveOptimize(node->left);
        } else {

            if ((IS_VAL_L) && (IS_VAL_R)) {

                double res = CalculateValue(CUR_OP, L_VAL, R_VAL);
                struct node* valNode = CreateNode(VAL_TYPE, 0, res, NULL, NULL, NULL);

                replaceNodes(node, valNode);
                //add free
                node = valNode;
                return ;
            }
            if (IS_VAL_L && L_VAL == 0.0 && CUR_OP == OP_MUL ||
                IS_VAL_R && R_VAL == 0.0 && CUR_OP == OP_MUL) {

                struct node* nullNode = CreateNode(VAL_TYPE, 0, 0.0, NULL, NULL, NULL);
                replaceNodes(node, nullNode);
                //add recursive free func
                node = nullNode;
                return;
            }

            if (IS_VAL_R && R_VAL == 1.0 && (CUR_OP == OP_MUL || CUR_OP == OP_DIV || CUR_OP == OP_DEG) ||
                IS_VAL_R && R_VAL == 0.0 && (CUR_OP == OP_ADD || CUR_OP == OP_SUB                   )) {

                replaceNodes(node, node->left);
                node = node->left;
                RecursiveOptimize(node);
            } 
            else if 
                (IS_VAL_L && L_VAL == 1.0 && CUR_OP == OP_MUL ||
                IS_VAL_L && L_VAL == 0.0 && CUR_OP == OP_ADD) {

                replaceNodes(node, node->right);
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

    size_t chars = fread(DiffData, sizeof(char), 50, SourceFile);
    DiffData[chars-1] = '\0';
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






