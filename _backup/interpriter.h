#include <QThread>
#include <QPlainTextEdit>
#include <QString>
#include <QHash>

typedef enum Tokens {
    ARG,
    CHAR,
    INT,
    BOOL,
    IF,
    ELSE,
    FOR,
    DO,
    WHILE,
    SWITCH,
    RETURN,
    BREAK,
    OPENPAREN,  //(
    CLOSEPAREN, //)
    OPENBRACE,  //{
    CLOSEBRACE, //}
    LEES,       // <
    LEES_EQ,    // <=
    GREAT,      // >
    GREAT_EQ,   // >=
    EQ,         // ==
    NOT_EQ,     // !=
    IDENTIFIER,
    NUMBER,
    STRING,
    FINISHED,
} Tokens;

typedef enum DataTypes{
    DT_CHAR,
    DT_INT,
    DT_BOOL,
} DataTypes;

class Programm : public QThread
{
    Q_OBJECT
private:

    struct Variable {
        QString var_name;
        int v_type;
        void *value;
    };

    QHash <QString, Tokens> keyWords;

    struct func_type {
        QString func_name;
        int ret_type;
        char *loc;  // адрес точки входа в функцию
    };

    QHash<QString, void (*)()> intern_func;

    typedef QHash<QString, Variable> Variables;
    typedef QList<Variable> VariablesStack;

    typedef QHash<QString, func_type> Functions;
    typedef QList<func_type> FunctionsStack;

    VariablesStack local_var_stack;
    Variables global_vars;
    Functions func_table;
    FunctionsStack func_stack;

    enum error_msg {
        SYNTAX,
        UNBAL_PARENS,
        NO_EXP,
        EQUALS_EXPECTED,
        NOT_VAR,
        PARAM_ERR,
        SEMI_EXPECTED,
        UNBAL_BRACES,
        FUNC_UNDEF,
        TYPE_EXPECTED,
        NEST_FUNC,
        RET_NOCALL,
        PAREN_EXPECTED,
        WHILE_EXPECTED,
        QUOTE_EXPECTED,
        NOT_TEMP,
        TOO_MANY_LVARS,
        DIV_BY_ZERO,
        NO_MAIN
    };

    void eval_exp(int *value);
    void eval_exp0(int *value);
    void eval_exp1(int *value);
    void eval_exp2(int *value);
    void eval_exp3(int *value);
    void eval_exp4(int *value);
    void eval_exp5(int *value);
    void atom(int *value);
    void sntx_err(int error); // Вывод синтаксической ошибк
    void putback(void);
    void assign_var(QString var_name, int value);
    void call(void);
    void prescan(void);
    void decl_global();
    void decl_local();  // Объявление локальной переменно
    void local_push(Variable i);
    void exec_if(void);
    void find_eob(void);
    void exec_for(void);
    void get_params(void);
    void get_args(void);
    void exec_while(void);
    void func_push(int i);
    void exec_do(void);
    void interp_block(void);
    void func_ret(void);
    void RplcSysSmb(char *text);
    bool ismyalpha(char c);
    void *rvalue;

    int isdelim(char c);
    int iswhite(char c);
    int get_token(void);
    int internal_func(QString s);
    int is_var(QString s);
    int load_program(char *p);
    int func_pop(void);
    int find_var(QString s);
    void call_print(void);

    Tokens   look_up(QString s);

    char *find_func(QString name);

    int call_stack[NUM_FUNC];
    int functos;
    int func_index;
    int lvartos;
    int ret_value; /* возвращаемое значение функции */
    int gvar_index;

    QString token; /* содержит строковое представление лексемы */
    char token_type;

    char *p_buf;
    char *prog;
    char ch;

    Tokens tokenType;

public:
    Programm();
    void Execute(QString progremText);

    void call_getche(void);
    void call_putch(void);
    void callPuts(void);
    void callGetch(void);
    void rand(void);

signals:

};
//---------------------------------------------------------------------------
