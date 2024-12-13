#ifndef ABSYN_HEADER
#define ABSYN_HEADER

#include<string>
#include<vector>

//C++ Abstract Syntax Interface generated by the BNF Converter.

/********************   TypeDef Section    ********************/

typedef int Integer;
typedef char Char;
typedef double Double;
typedef std::string String;
typedef std::string Ident;


/********************   Forward Declarations    ********************/

class Program;

class TopDef;

class Arg;

class Block;

class Stmt;

class Item;

class Type;

class Expr;

class AddOp;

class MulOp;

class RelOp;

class Prog;

class FnDef;

class Ar;

class Blk;

class Empty;

class BStmt;

class Decl;

class NoInit;

class Init;

class Ass;

class Incr;

class Decr;

class Ret;

class VRet;

class Cond;

class CondElse;

class While;

class SExp;

class Int;

class Str;

class Bool;

class Void;

class Fun;

class EVar;

class ELitInt;

class ELitTrue;

class ELitFalse;

class EApp;

class EString;

class Neg;

class Not;

class EMul;

class EAdd;

class ERel;

class EAnd;

class EOr;

class Plus;

class Minus;

class Times;

class Div;

class Mod;

class LTH;

class LE;

class GTH;

class GE;

class EQU;

class NE;

class ListTopDef;

class ListArg;

class ListStmt;

class ListItem;

class ListType;

class ListExpr;

/********************   Three address code   **********************/

class Quadruple;

class QuadBlk;

class QuadAss1;

class QuadAss2;

class QuadCopy;

class QuadJmp;

class QuadLabel;

class QuadIf;

class QuadParam;

class QuadCall;

class QuadFunBegin;

class QuadReturn;

class QuadReturnNoVal;

class QuadRetrieve;

class QuadArg;

/********************   Visitor Interfaces    ********************/
class Visitor {
public:
    virtual ~Visitor() {}

    virtual void visitProgram(Program *p) = 0;

    virtual void visitTopDef(TopDef *p) = 0;

    virtual void visitArg(Arg *p) = 0;

    virtual void visitBlock(Block *p) = 0;

    virtual void visitStmt(Stmt *p) = 0;

    virtual void visitItem(Item *p) = 0;

    virtual void visitType(Type *p) = 0;

    virtual void visitExpr(Expr *p) = 0;

    virtual void visitAddOp(AddOp *p) = 0;

    virtual void visitMulOp(MulOp *p) = 0;

    virtual void visitRelOp(RelOp *p) = 0;

    virtual void visitProg(Prog *p) = 0;

    virtual void visitFnDef(FnDef *p) = 0;

    virtual void visitAr(Ar *p) = 0;

    virtual void visitBlk(Blk *p) = 0;

    virtual void visitEmpty(Empty *p) = 0;

    virtual void visitBStmt(BStmt *p) = 0;

    virtual void visitDecl(Decl *p) = 0;

    virtual void visitNoInit(NoInit *p) = 0;

    virtual void visitInit(Init *p) = 0;

    virtual void visitAss(Ass *p) = 0;

    virtual void visitIncr(Incr *p) = 0;

    virtual void visitDecr(Decr *p) = 0;

    virtual void visitRet(Ret *p) = 0;

    virtual void visitVRet(VRet *p) = 0;

    virtual void visitCond(Cond *p) = 0;

    virtual void visitCondElse(CondElse *p) = 0;

    virtual void visitWhile(While *p) = 0;

    virtual void visitSExp(SExp *p) = 0;

    virtual void visitInt(Int *p) = 0;

    virtual void visitStr(Str *p) = 0;

    virtual void visitBool(Bool *p) = 0;

    virtual void visitVoid(Void *p) = 0;

    virtual void visitFun(Fun *p) = 0;

    virtual void visitEVar(EVar *p) = 0;

    virtual void visitELitInt(ELitInt *p) = 0;

    virtual void visitELitTrue(ELitTrue *p) = 0;

    virtual void visitELitFalse(ELitFalse *p) = 0;

    virtual void visitEApp(EApp *p) = 0;

    virtual void visitEString(EString *p) = 0;

    virtual void visitNeg(Neg *p) = 0;

    virtual void visitNot(Not *p) = 0;

    virtual void visitEMul(EMul *p) = 0;

    virtual void visitEAdd(EAdd *p) = 0;

    virtual void visitERel(ERel *p) = 0;

    virtual void visitEAnd(EAnd *p) = 0;

    virtual void visitEOr(EOr *p) = 0;

    virtual void visitPlus(Plus *p) = 0;

    virtual void visitMinus(Minus *p) = 0;

    virtual void visitTimes(Times *p) = 0;

    virtual void visitDiv(Div *p) = 0;

    virtual void visitMod(Mod *p) = 0;

    virtual void visitLTH(LTH *p) = 0;

    virtual void visitLE(LE *p) = 0;

    virtual void visitGTH(GTH *p) = 0;

    virtual void visitGE(GE *p) = 0;

    virtual void visitEQU(EQU *p) = 0;

    virtual void visitNE(NE *p) = 0;

    virtual void visitListTopDef(ListTopDef *p) = 0;

    virtual void visitListArg(ListArg *p) = 0;

    virtual void visitListStmt(ListStmt *p) = 0;

    virtual void visitListItem(ListItem *p) = 0;

    virtual void visitListType(ListType *p) = 0;

    virtual void visitListExpr(ListExpr *p) = 0;

    virtual void visitQuadruple(Quadruple *q) = 0;

    virtual void visitQuadBlk(QuadBlk *q) = 0;

    virtual void visitQuadAss1(QuadAss1 *q) = 0;

    virtual void visitQuadAss2(QuadAss2 *q) = 0;

    virtual void visitQuadCopy(QuadCopy *q) = 0;

    virtual void visitQuadJmp(QuadJmp *q) = 0;

    virtual void visitQuadLabel(QuadLabel *q) = 0;

    virtual void visitQuadIf(QuadIf *q) = 0;

    virtual void visitQuadParam(QuadParam *q) = 0;

    virtual void visitQuadCall(QuadCall *q) = 0;

    virtual void visitQuadFunBegin(QuadFunBegin *q) = 0;

    virtual void visitQuadReturn(QuadReturn *q) = 0;

    virtual void visitQuadReturnNoVal(QuadReturnNoVal *q) = 0;

    virtual void visitQuadRetrieve(QuadRetrieve *q) = 0;

    virtual void visitQuadArg(QuadArg *q) = 0;

    virtual void visitInteger(Integer x) = 0;

    virtual void visitChar(Char x) = 0;

    virtual void visitDouble(Double x) = 0;

    virtual void visitString(String x) = 0;

    virtual void visitIdent(Ident x) = 0;

};


class Visitable {
public:
    virtual ~Visitable() {}

    virtual void accept(Visitor *v) = 0;

    virtual Visitable *clone() const = 0;
};


/********************   Abstract Syntax Classes    ********************/

class Program : public Visitable {
public:
    [[nodiscard]] virtual Program *clone() const = 0;

    int line_number;
};

class TopDef : public Visitable {
public:
    [[nodiscard]] virtual TopDef *clone() const = 0;

    int line_number;
};

class Arg : public Visitable {
public:
    [[nodiscard]] virtual Arg *clone() const = 0;

    int line_number;
};

class Block : public Visitable {
public:
    [[nodiscard]] virtual Block *clone() const = 0;

    int line_number;
};

class Stmt : public Visitable {
public:
    [[nodiscard]] virtual Stmt *clone() const = 0;

    int line_number;
};

class Item : public Visitable {
public:
    [[nodiscard]] virtual Item *clone() const = 0;

    int line_number;
};

class Type : public Visitable {
public:
    [[nodiscard]] virtual Type *clone() const = 0;

    int line_number;
};

class Expr : public Visitable {
public:
    [[nodiscard]] virtual Expr *clone() const = 0;

    int line_number;
};

class AddOp : public Visitable {
public:
    [[nodiscard]] virtual AddOp *clone() const = 0;

    int line_number;
};

class MulOp : public Visitable {
public:
    [[nodiscard]] virtual MulOp *clone() const = 0;

    int line_number;
};

class RelOp : public Visitable {
public:
    [[nodiscard]] virtual RelOp *clone() const = 0;

    int line_number;
};

class Quadruple : public Visitable {
public:
    [[nodiscard]] virtual Quadruple *clone() const = 0;
};

class QuadArg {
public:
    QuadArg(bool isValue, int value, const String &var);

    QuadArg(bool isValue, String stringValue, const String &var);

    QuadArg();

    bool isValue;
    String type;
    String stringValue;
    int value;
    String var;
};

class QuadAss1 : public Quadruple {
public:
    Ident res;
    QuadArg arg;
    String op;

    QuadAss1(const QuadAss1 &);

    QuadAss1(const Ident &res, const QuadArg &arg, const String &op);

    QuadAss1 &operator=(const QuadAss1 &);

    virtual void accept(Visitor *v);

    [[nodiscard]] virtual QuadAss1 *clone() const;

    void swap(QuadAss1 &);
};

class QuadAss2 : public Quadruple {
public:
    Ident res;
    QuadArg arg1;
    QuadArg arg2;
    String op;

    QuadAss2(const QuadAss2 &);

    QuadAss2(const Ident &res, const QuadArg &arg1, const QuadArg &arg2, const String &op);

    QuadAss2 &operator=(const QuadAss2 &);

    virtual void accept(Visitor *v);

    [[nodiscard]] virtual QuadAss2 *clone() const;

    void swap(QuadAss2 &);
};

class QuadCopy : public Quadruple {
public:
    Ident res;
    QuadArg arg;

    QuadCopy(const QuadCopy &);

    QuadCopy(const Ident &res, const QuadArg &arg);

    QuadCopy &operator=(const QuadCopy &);

    virtual void accept(Visitor *v);

    [[nodiscard]] virtual QuadCopy *clone() const;

    void swap(QuadCopy &);
};

class QuadJmp : public Quadruple {
public:
    Ident label;

    String jmpOp;

    QuadJmp(const QuadJmp &);

    QuadJmp(const Ident &label, const String &jmpOp);

    QuadJmp &operator=(const QuadJmp &);

    virtual void accept(Visitor *v);

    [[nodiscard]] virtual QuadJmp *clone() const;

    void swap(QuadJmp &);
};

class QuadLabel : public Quadruple {
public:
    Ident label;

    QuadLabel(const QuadLabel &);

    QuadLabel(const Ident &label);

    QuadLabel &operator=(const QuadLabel &);

    virtual void accept(Visitor *v);

    [[nodiscard]] virtual QuadLabel *clone() const;

    void swap(QuadLabel &);
};

class QuadIf : public Quadruple {
public:
    QuadArg cond;
    Ident label1;
    Ident label2;

    QuadIf(const QuadIf &);

    QuadIf(const QuadArg &cond, const Ident &label1, const Ident &label2);

    QuadIf &operator=(const QuadIf &);

    virtual void accept(Visitor *v);

    [[nodiscard]] virtual QuadIf *clone() const;

    void swap(QuadIf &);
};

class QuadParam : public Quadruple {
public:
    QuadArg arg;
    Ident ident;

    QuadParam(const QuadParam &);

    QuadParam(const QuadArg &arg, const Ident &ident1);

    QuadParam &operator=(const QuadParam &);

    virtual void accept(Visitor *v);

    [[nodiscard]] virtual QuadParam *clone() const;

    void swap(QuadParam &);
};

class QuadCall : public Quadruple {
public:
    Ident label;
    int argsNum;

    QuadCall(const QuadCall &);

    QuadCall(const Ident &label, int argsNum);

    QuadCall &operator=(const QuadCall &);

    virtual void accept(Visitor *v);

    [[nodiscard]] virtual QuadCall *clone() const;

    void swap(QuadCall &);
};

class QuadFunBegin : public Quadruple {
public:
    Ident ident;

    QuadFunBegin();

    QuadFunBegin(const String ident);

    QuadFunBegin(const QuadFunBegin &);

    QuadFunBegin &operator=(const QuadFunBegin &);

    virtual void accept(Visitor *v);

    [[nodiscard]] virtual QuadFunBegin *clone() const;

    void swap(QuadFunBegin &);

};

class QuadReturn : public Quadruple {
public:
    QuadArg ret;

    QuadReturn(const QuadReturn &);

    QuadReturn(const QuadArg &ret);

    QuadReturn &operator=(const QuadReturn &);

    virtual void accept(Visitor *v);

    [[nodiscard]] virtual QuadReturn *clone() const;

    void swap(QuadReturn &);
};

class QuadReturnNoVal : public Quadruple {
public:

    QuadReturnNoVal();

    QuadReturnNoVal(const QuadReturnNoVal &);

    QuadReturnNoVal &operator=(const QuadReturnNoVal &);

    virtual void accept(Visitor *v);

    [[nodiscard]] virtual QuadReturnNoVal *clone() const;

    void swap(QuadReturnNoVal &);
};

class QuadRetrieve : public Quadruple {
public:
    String res;

    QuadRetrieve();

    QuadRetrieve(const String res);

    QuadRetrieve(const QuadRetrieve &);

    QuadRetrieve &operator=(const QuadRetrieve &);

    virtual void accept(Visitor *v);

    [[nodiscard]] virtual QuadRetrieve *clone() const;

    void swap(QuadRetrieve &);
};

class QuadBlk : public Block {
public:
    std::vector<Quadruple *> *quadlist;

    QuadBlk(const QuadBlk &other);

    QuadBlk();

    QuadBlk &operator=(const QuadBlk &other);

    explicit QuadBlk(std::vector<Quadruple *> *list);

    ~QuadBlk();

    virtual void accept(Visitor *v);

    virtual QuadBlk *clone() const;

    void swap(QuadBlk &);
};


class Prog : public Program {
public:
    ListTopDef *listtopdef_;

    Prog(const Prog &);

    Prog &operator=(const Prog &);

    explicit Prog(ListTopDef *p1);

    ~Prog();

    virtual void accept(Visitor *v);

    [[nodiscard]] virtual Prog *clone() const;

    void swap(Prog &);
};

class FnDef : public TopDef {
public:
    Type *type_;
    Ident ident_;
    ListArg *listarg_;
    Block *block_;

    FnDef(const FnDef &);

    FnDef &operator=(const FnDef &);

    FnDef(Type *p1, Ident p2, ListArg *p3, Block *p4);

    ~FnDef();

    virtual void accept(Visitor *v);

    [[nodiscard]] virtual FnDef *clone() const;

    void swap(FnDef &);
};

class Ar : public Arg {
public:
    Type *type_;
    Ident ident_;

    Ar(const Ar &);

    Ar &operator=(const Ar &);

    Ar(Type *p1, Ident p2);

    ~Ar();

    virtual void accept(Visitor *v);

    [[nodiscard]] virtual Ar *clone() const;

    void swap(Ar &);
};


class Blk : public Block {
public:
    ListStmt *liststmt_;

    Blk(const Blk &);

    Blk &operator=(const Blk &);

    explicit Blk(ListStmt *p1);

    ~Blk();

    virtual void accept(Visitor *v);

    virtual Blk *clone() const;

    void swap(Blk &);
};

class Empty : public Stmt {
public:

    Empty(const Empty &);

    Empty &operator=(const Empty &);

    Empty();

    ~Empty();

    virtual void accept(Visitor *v);

    [[nodiscard]] virtual Empty *clone() const;

    void swap(Empty &);
};

class BStmt : public Stmt {
public:
    Block *block_;

    BStmt(const BStmt &);

    BStmt &operator=(const BStmt &);

    explicit BStmt(Block *p1);

    ~BStmt();

    virtual void accept(Visitor *v);

    [[nodiscard]] virtual BStmt *clone() const;

    void swap(BStmt &);
};

class Decl : public Stmt {
public:
    Type *type_;
    ListItem *listitem_;

    Decl(const Decl &);

    Decl &operator=(const Decl &);

    Decl(Type *p1, ListItem *p2);

    ~Decl();

    virtual void accept(Visitor *v);

    [[nodiscard]] virtual Decl *clone() const;

    void swap(Decl &);
};

class Ass : public Stmt {
public:
    Ident ident_;
    Expr *expr_;

    Ass(const Ass &);

    Ass &operator=(const Ass &);

    Ass(Ident p1, Expr *p2);

    ~Ass();

    virtual void accept(Visitor *v);

    [[nodiscard]] virtual Ass *clone() const;

    void swap(Ass &);
};

class Incr : public Stmt {
public:
    Ident ident_;

    Incr(const Incr &);

    Incr &operator=(const Incr &);

    explicit Incr(Ident p1);

    ~Incr();

    virtual void accept(Visitor *v);

    [[nodiscard]] virtual Incr *clone() const;

    void swap(Incr &);
};

class Decr : public Stmt {
public:
    Ident ident_;

    Decr(const Decr &);

    Decr &operator=(const Decr &);

    explicit Decr(Ident p1);

    ~Decr();

    virtual void accept(Visitor *v);

    [[nodiscard]] virtual Decr *clone() const;

    void swap(Decr &);
};

class Ret : public Stmt {
public:
    Expr *expr_;

    Ret(const Ret &);

    Ret &operator=(const Ret &);

    explicit Ret(Expr *p1);

    ~Ret();

    virtual void accept(Visitor *v);

    [[nodiscard]] virtual Ret *clone() const;

    void swap(Ret &);
};

class VRet : public Stmt {
public:

    VRet(const VRet &);

    VRet &operator=(const VRet &);

    VRet();

    ~VRet();

    virtual void accept(Visitor *v);

    [[nodiscard]] virtual VRet *clone() const;

    void swap(VRet &);
};

class Cond : public Stmt {
public:
    Expr *expr_;
    Stmt *stmt_;

    Cond(const Cond &);

    Cond &operator=(const Cond &);

    Cond(Expr *p1, Stmt *p2);

    ~Cond();

    virtual void accept(Visitor *v);

    [[nodiscard]] virtual Cond *clone() const;

    void swap(Cond &);
};

class CondElse : public Stmt {
public:
    Expr *expr_;
    Stmt *stmt_1;
    Stmt *stmt_2;

    CondElse(const CondElse &);

    CondElse &operator=(const CondElse &);

    CondElse(Expr *p1, Stmt *p2, Stmt *p3);

    ~CondElse();

    virtual void accept(Visitor *v);

    [[nodiscard]] virtual CondElse *clone() const;

    void swap(CondElse &);
};

class While : public Stmt {
public:
    Expr *expr_;
    Stmt *stmt_;

    While(const While &);

    While &operator=(const While &);

    While(Expr *p1, Stmt *p2);

    ~While();

    virtual void accept(Visitor *v);

    [[nodiscard]] virtual While *clone() const;

    void swap(While &);
};

class SExp : public Stmt {
public:
    Expr *expr_;

    SExp(const SExp &);

    SExp &operator=(const SExp &);

    explicit SExp(Expr *p1);

    ~SExp();

    virtual void accept(Visitor *v);

    [[nodiscard]] virtual SExp *clone() const;

    void swap(SExp &);
};

class NoInit : public Item {
public:
    Ident ident_;

    NoInit(const NoInit &);

    NoInit &operator=(const NoInit &);

    explicit NoInit(Ident p1);

    ~NoInit();

    virtual void accept(Visitor *v);

    [[nodiscard]] virtual NoInit *clone() const;

    void swap(NoInit &);
};

class Init : public Item {
public:
    Ident ident_;
    Expr *expr_;

    Init(const Init &);

    Init &operator=(const Init &);

    Init(Ident p1, Expr *p2);

    ~Init();

    virtual void accept(Visitor *v);

    [[nodiscard]] virtual Init *clone() const;

    void swap(Init &);
};

class Int : public Type {
public:

    Int(const Int &);

    Int &operator=(const Int &);

    Int();

    ~Int();

    virtual void accept(Visitor *v);

    [[nodiscard]] virtual Int *clone() const;

    void swap(Int &);
};

class Str : public Type {
public:

    Str(const Str &);

    Str &operator=(const Str &);

    Str();

    ~Str();

    virtual void accept(Visitor *v);

    [[nodiscard]] virtual Str *clone() const;

    void swap(Str &);
};

class Bool : public Type {
public:

    Bool(const Bool &);

    Bool &operator=(const Bool &);

    Bool();

    ~Bool();

    virtual void accept(Visitor *v);

    [[nodiscard]] virtual Bool *clone() const;

    void swap(Bool &);
};

class Void : public Type {
public:

    Void(const Void &);

    Void &operator=(const Void &);

    Void();

    ~Void();

    virtual void accept(Visitor *v);

    [[nodiscard]] virtual Void *clone() const;

    void swap(Void &);
};

class Fun : public Type {
public:
    Type *type_;
    ListType *listtype_;

    Fun(const Fun &);

    Fun &operator=(const Fun &);

    Fun(Type *p1, ListType *p2);

    ~Fun();

    virtual void accept(Visitor *v);

    [[nodiscard]] virtual Fun *clone() const;

    void swap(Fun &);
};

class EVar : public Expr {
public:
    Ident ident_;

    EVar(const EVar &);

    EVar &operator=(const EVar &);

    explicit EVar(Ident p1);

    ~EVar();

    virtual void accept(Visitor *v);

    [[nodiscard]] virtual EVar *clone() const;

    void swap(EVar &);
};

class ELitInt : public Expr {
public:
    Integer integer_;

    ELitInt(const ELitInt &);

    ELitInt &operator=(const ELitInt &);

    explicit ELitInt(Integer p1);

    ~ELitInt();

    virtual void accept(Visitor *v);

    [[nodiscard]] virtual ELitInt *clone() const;

    void swap(ELitInt &);
};

class ELitTrue : public Expr {
public:

    ELitTrue(const ELitTrue &);

    ELitTrue &operator=(const ELitTrue &);

    ELitTrue();

    ~ELitTrue();

    virtual void accept(Visitor *v);

    [[nodiscard]] virtual ELitTrue *clone() const;

    void swap(ELitTrue &);
};

class ELitFalse : public Expr {
public:

    ELitFalse(const ELitFalse &);

    ELitFalse &operator=(const ELitFalse &);

    ELitFalse();

    ~ELitFalse();

    virtual void accept(Visitor *v);

    [[nodiscard]] virtual ELitFalse *clone() const;

    void swap(ELitFalse &);
};

class EApp : public Expr {
public:
    Ident ident_;
    ListExpr *listexpr_;

    EApp(const EApp &);

    EApp &operator=(const EApp &);

    EApp(Ident p1, ListExpr *p2);

    ~EApp();

    virtual void accept(Visitor *v);

    [[nodiscard]] virtual EApp *clone() const;

    void swap(EApp &);
};

class EString : public Expr {
public:
    String string_;

    EString(const EString &);

    EString &operator=(const EString &);

    explicit EString(String p1);

    ~EString();

    virtual void accept(Visitor *v);

    [[nodiscard]] virtual EString *clone() const;

    void swap(EString &);
};

class Neg : public Expr {
public:
    Expr *expr_;

    Neg(const Neg &);

    Neg &operator=(const Neg &);

    explicit Neg(Expr *p1);

    ~Neg();

    virtual void accept(Visitor *v);

    [[nodiscard]] virtual Neg *clone() const;

    void swap(Neg &);
};

class Not : public Expr {
public:
    Expr *expr_;

    Not(const Not &);

    Not &operator=(const Not &);

    explicit Not(Expr *p1);

    ~Not();

    virtual void accept(Visitor *v);

    [[nodiscard]] virtual Not *clone() const;

    void swap(Not &);
};

class EMul : public Expr {
public:
    Expr *expr_1;
    MulOp *mulop_;
    Expr *expr_2;

    EMul(const EMul &);

    EMul &operator=(const EMul &);

    EMul(Expr *p1, MulOp *p2, Expr *p3);

    ~EMul();

    virtual void accept(Visitor *v);

    [[nodiscard]] virtual EMul *clone() const;

    void swap(EMul &);
};

class EAdd : public Expr {
public:
    Expr *expr_1;
    AddOp *addop_;
    Expr *expr_2;

    EAdd(const EAdd &);

    EAdd &operator=(const EAdd &);

    EAdd(Expr *p1, AddOp *p2, Expr *p3);

    ~EAdd();

    virtual void accept(Visitor *v);

    [[nodiscard]] virtual EAdd *clone() const;

    void swap(EAdd &);
};

class ERel : public Expr {
public:
    Expr *expr_1;
    RelOp *relop_;
    Expr *expr_2;

    ERel(const ERel &);

    ERel &operator=(const ERel &);

    ERel(Expr *p1, RelOp *p2, Expr *p3);

    ~ERel();

    virtual void accept(Visitor *v);

    [[nodiscard]] virtual ERel *clone() const;

    void swap(ERel &);
};

class EAnd : public Expr {
public:
    Expr *expr_1;
    Expr *expr_2;

    EAnd(const EAnd &);

    EAnd &operator=(const EAnd &);

    EAnd(Expr *p1, Expr *p2);

    ~EAnd();

    virtual void accept(Visitor *v);

    [[nodiscard]] virtual EAnd *clone() const;

    void swap(EAnd &);
};

class EOr : public Expr {
public:
    Expr *expr_1;
    Expr *expr_2;

    EOr(const EOr &);

    EOr &operator=(const EOr &);

    EOr(Expr *p1, Expr *p2);

    ~EOr();

    virtual void accept(Visitor *v);

    [[nodiscard]] virtual EOr *clone() const;

    void swap(EOr &);
};

class Plus : public AddOp {
public:

    Plus(const Plus &);

    Plus &operator=(const Plus &);

    Plus();

    ~Plus();

    virtual void accept(Visitor *v);

    [[nodiscard]] virtual Plus *clone() const;

    void swap(Plus &);
};

class Minus : public AddOp {
public:

    Minus(const Minus &);

    Minus &operator=(const Minus &);

    Minus();

    ~Minus();

    virtual void accept(Visitor *v);

    [[nodiscard]] virtual Minus *clone() const;

    void swap(Minus &);
};

class Times : public MulOp {
public:

    Times(const Times &);

    Times &operator=(const Times &);

    Times();

    ~Times();

    virtual void accept(Visitor *v);

    [[nodiscard]] virtual Times *clone() const;

    void swap(Times &);
};

class Div : public MulOp {
public:

    Div(const Div &);

    Div &operator=(const Div &);

    Div();

    ~Div();

    virtual void accept(Visitor *v);

    [[nodiscard]] virtual Div *clone() const;

    void swap(Div &);
};

class Mod : public MulOp {
public:

    Mod(const Mod &);

    Mod &operator=(const Mod &);

    Mod();

    ~Mod();

    virtual void accept(Visitor *v);

    [[nodiscard]] virtual Mod *clone() const;

    void swap(Mod &);
};

class LTH : public RelOp {
public:

    LTH(const LTH &);

    LTH &operator=(const LTH &);

    LTH();

    ~LTH();

    virtual void accept(Visitor *v);

    [[nodiscard]] virtual LTH *clone() const;

    void swap(LTH &);
};

class LE : public RelOp {
public:

    LE(const LE &);

    LE &operator=(const LE &);

    LE();

    ~LE();

    virtual void accept(Visitor *v);

    [[nodiscard]] virtual LE *clone() const;

    void swap(LE &);
};

class GTH : public RelOp {
public:

    GTH(const GTH &);

    GTH &operator=(const GTH &);

    GTH();

    ~GTH();

    virtual void accept(Visitor *v);

    [[nodiscard]] virtual GTH *clone() const;

    void swap(GTH &);
};

class GE : public RelOp {
public:

    GE(const GE &);

    GE &operator=(const GE &);

    GE();

    ~GE();

    virtual void accept(Visitor *v);

    [[nodiscard]] virtual GE *clone() const;

    void swap(GE &);
};

class EQU : public RelOp {
public:

    EQU(const EQU &);

    EQU &operator=(const EQU &);

    EQU();

    ~EQU();

    virtual void accept(Visitor *v);

    [[nodiscard]] virtual EQU *clone() const;

    void swap(EQU &);
};

class NE : public RelOp {
public:

    NE(const NE &);

    NE &operator=(const NE &);

    NE();

    ~NE();

    virtual void accept(Visitor *v);

    [[nodiscard]] virtual NE *clone() const;

    void swap(NE &);
};


class ListTopDef : public Visitable, public std::vector<TopDef *> {
public:
    virtual void accept(Visitor *v);

    [[nodiscard]] virtual ListTopDef *clone() const;
};

class ListArg : public Visitable, public std::vector<Arg *> {
public:
    virtual void accept(Visitor *v);

    [[nodiscard]] virtual ListArg *clone() const;
};

class ListStmt : public Visitable, public std::vector<Stmt *> {
public:
    virtual void accept(Visitor *v);

    [[nodiscard]] virtual ListStmt *clone() const;
};

class ListItem : public Visitable, public std::vector<Item *> {
public:
    virtual void accept(Visitor *v);

    [[nodiscard]] virtual ListItem *clone() const;
};

class ListType : public Visitable, public std::vector<Type *> {
public:
    virtual void accept(Visitor *v);

    [[nodiscard]] virtual ListType *clone() const;
};

class ListExpr : public Visitable, public std::vector<Expr *> {
public:
    virtual void accept(Visitor *v);

    [[nodiscard]] virtual ListExpr *clone() const;
};


#endif
