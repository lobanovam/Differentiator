#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "log.h"
#include "dump.h"
#include "html.h"

struct node* GetG(char* DiffData);
struct node* GetE();
struct node* GetN();
struct node* GetT();
struct node* GetP();

const char* s = NULL;
const char png[5] = ".png";

void TreeCtor(struct Tree* tree, struct DiffNode* rootData);
void TreeDtor(struct Tree* tree);
void NodesDtor(struct node* node);

struct node* CreateLeftNode(struct node* node, struct DiffNode* val);
struct node* CreateRightNode(struct node* node, struct DiffNode* val);
struct node* CreateNode(int type, int op, double dbl, char *varValue);

size_t SetDiffValue(struct node* node, const char* DiffData);
int getOpType(const char* tmp_op);

void readDiffData(struct Tree* tree);
void ScanDiffData(const char* DiffData, struct node* node);

void setHTML(const char* image);
void DrawTree(struct node* node, FILE* DumpFile);
void GraphTreeDump(struct node* root);

char* IntToString(int num);

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

enum Op_types
{
    OP_ADD = '+',
    OP_SUB = '-',
    OP_MUL = '*',
    OP_DIV = '/',
    OP_DEG = '^'
};

struct DiffNode
{
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
    printf("done graph dump\n");

    TreeDtor(&tree);
    printf("done dtoring tree\n");

}

int getOpType(const char* tmp_op) {
    int op = *tmp_op;
    switch (op)
    {
    case OP_ADD:
        return OP_ADD;
    case OP_SUB:
        return OP_SUB;
    case OP_MUL:
        return OP_MUL;
    case OP_DIV:
        return OP_DIV;
    case OP_DEG:
        return OP_DEG;

    default:
        return -1;
    }

} 

/*size_t SetDiffValue(struct node* node, const char* DiffData) {
    node->value = (struct DiffNode*)(calloc(1, sizeof(struct DiffNode)));
    int ch_count = 0;
    double num = 0;
    char op = ' ';
    printf("will be trying to scan %s\n", DiffData);
    if (sscanf(DiffData, "%lf%n", &num, &ch_count)) {
        printf("scanned num is %lf\n", num);
        node->value->type = VAL_TYPE;
        node->value->dbl = num;
    } else {
        char* tmp_op = (char*)(calloc(20, sizeof(char)));
        sscanf(DiffData, "%[^ ()]%n", tmp_op, &ch_count);
        

        int op_type = getOpType(tmp_op);
        if (op_type == -1) {
            printf("scanned var is %s\n", tmp_op);
            node->value->type = VAR_TYPE;
            node->value->varValue = tmp_op;
        } else {
            printf("scanned op is %s\n", tmp_op);
            node->value->type = OP_TYPE;
            node->value->op = op_type;
        }
    }
    printf("done SetDiff value\n");
    printf("ch count is %d\n", ch_count);
    return ch_count;
}*/

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
    printf("%d\n", chars);
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


    if (node->value->type == OP_TYPE) {
        dumpPrint("struct%p [\nlabel = \"{op: %c |parent: %p| left: %p| right: %p }\", style = \"filled\", color = \"black\", fillcolor = \"lightgrey\" \n];\n", node, node->value->op, node->parent, node->left, node->right);
    } else if (node->value->type == VAL_TYPE) {
        printf("dealing with val %lf\n", node->value->dbl);
        dumpPrint("struct%p [\nlabel = \"{value: %lf |parent: %p| left: %p| right: %p }\", style = \"filled\", color = \"black\", fillcolor = \"green\" \n];\n", node, node->value->dbl, node->parent, node->left, node->right);
    } else if (node->value->type == VAR_TYPE) {
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

    if (s == sOld) {
        if ('a' <= *s && *s <= 'z') {
            char *var = (char*)(calloc(2, sizeof(char)));
            assert(var != NULL);
            var[0] = *s;
            newNode = CreateNode(VAR_TYPE, -1, -1, var);
            s++;
        } else {
            printf("ERROR: SYNTAX ERROR\n");
        }
    } else {
        newNode = CreateNode(VAL_TYPE, -1, val, NULL);
    }
    
    return newNode;
}

struct node* GetE() {
    struct node* opNode = NULL;
    struct node* nodeL = GetT();
    
    while (*s == '+' || *s == '-') {
        printf("in GetE %c is read\n", *s);
        char op = *s;
        s++;

        struct node* nodeR = GetT();
        printf("\nin GetE nodeR is %lf\n", nodeR->value->dbl);

        if (op == '+'){
            opNode = CreateNode(OP_TYPE, OP_ADD, 0, NULL);
        } else {
            opNode = CreateNode(OP_TYPE, OP_SUB, 0, NULL);
        }
        opNode->left = nodeL;
        opNode->right = nodeR;
        nodeL->parent = opNode;
        nodeR->parent = opNode;

        nodeL = opNode;
    }

    return nodeL;
}

struct node* GetT() {
    struct node* opNode = NULL;
    struct node* nodeL = GetP();
    while (*s == '*' || *s == '/') {
        char op = *s;
        s++;
        struct node* nodeR = GetP();
        if (op == '*'){
            opNode = CreateNode(OP_TYPE, OP_MUL, 0, NULL);
            
        } else {
            opNode = CreateNode(OP_TYPE, OP_DIV, 0, NULL);
        }
        opNode->left = nodeL;
        opNode->right = nodeR;
        nodeL->parent = opNode;
        nodeR->parent = opNode;

        nodeL = opNode;
    }

    return nodeL;
}

struct node* GetP() {
    struct node* node1 = NULL;
    if (*s == '(')
    {
        s++;
        node1 = GetE();
        assert(*s == ')');
        s++;
    } else {
        node1 = GetN();
    }
   
    return node1;
}

struct node* CreateNode(int type, int op, double dbl, char *varValue) {
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

    return newNode;
}






