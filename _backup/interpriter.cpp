#include "interpriter.h"


// Интерпритация одного оператора или блока.-------------------------------------------------------------------------------------------------------------------------------------------------------------

void Programm::interp_block(void)
{
    int value;
    char block = 0;
    do {
        token_type = get_token(); // При интерпритации одного оператора возврат после первой точки с запятой.
        // определение типа лексемы
        if(token_type == IDENTIFIER) { // Это не зарегестрированное слово, обрабатывается выражение.
            putback();  //возврат лексемы во входной поток для дальнейшей обработки функцией eval_exp()
            eval_exp(&value);  //обработка выражения
            if(token[0]!=';')
                sntx_err(SEMI_EXPECTED);}
        else
            if(token_type==BLOCK) { //если это граничитель блока
                if(token[0] == '{') // блок
                    block = 1; // интерпритация блока, а не оператора
                else
                    return;} // это }, возврат
            else //зарезервированное слово
                switch(tokenType) {
                case CHAR:
                case INT:
                case BOOL:   //объявление локальной переменной
                    putback();
                    decl_local();
                    break;
                case RETURN: //возврат из вызова функции
                    func_ret();
                    return;
                case IF:     //обработка оператора if
                    exec_if();
                    break;
                case ELSE:   //обработка оператора else
                    find_eob(); //поиск конца блока else и продолжение выполнения
                    break;
                case WHILE:  //обработка цикла while
                    exec_while();
                    break;
                case DO:     //обработка цикла do-while
                    exec_do();
                    break;
                case FOR:    //обработка цикла for
                    exec_for();
                    break;
                case END:
                    exit(0);}}
    while (tokenType != FINISHED && block);
}

// Загрузка программы. ------------------------------------------------------------------------------------------------------
int Programm::load_program(char *p)
{
    int i=0;
    do {
        *p = pr->toPlainText().toStdString().c_str()[i];
        p++; i++;
    }
    while(i<pr->toPlainText().toStdString().length());
    *(p) = '\0';
    return 1;
}

// Это буква------------------------------------------------------------------------------------------------------------------------
bool Programm::ismyalpha(char c)
{
    if(strchr("ABCDEFGHIJKMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyzАБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯабвгдеёжзийклмнопрстуфхцчшщъыьэюя",c))
        return 1;
    return 0;
}

// Найти адреса всех функций в программе и запомнить глобальные переменные.--------------------------------------------------------------------------------------------------------------------------------------------
void Programm::prescan(void)
{
    char *tp;
    QString temp;
    Tokens datatype;
    int brace = 0;  //Если brace = 0, то текущая позиция указателя программы находится в не какой-либо функции.

    func_index = 0;
    do {
        while(brace) {  /* обход кода функции */
            get_token();
            if(token[0] == '{')
                brace++;
            if(token[0] == '}')
                brace--;
        }

        tp = prog; /* запоминание текущей позиции */
        get_token();

        if(tokenType == CHAR || tokenType == INT || tokenType== BOOL) { // тип глобальной переменной или возвращаемого значения функции */
            datatype = tokenType; // storage datatype
            get_token();

            if(token_type == IDENTIFIER){
                temp = token; // storage function name or variable
                get_token();

                if(token[0] != '(') { /* это должна быть глобальная переменная */
                    prog = tp; /* возврат в начало объявления */
                    decl_global();}
                else
                    if(token[0] == '(') {  /* это должна быть функция */
                        func_table[func_index].loc = prog;
                        func_table[func_index].ret_type = datatype;
                        func_table[func_index].func_name = temp;

                        while(*prog != ')') prog++;
                        prog++;

                        func_index++;}//сейчас prog указывает на открывающуюся фигурную скобку функции
                    else
                        putback();
            }
        }
        else
            if(token[0] == '{')
                brace++;
    }
    while(tokenType != FINISHED);
    prog = p_buf;
}

// Возврат адреса точки входа данной функции. Возврат NULL, если не надена.---------------------------------------------------------------------------------------------------------------------------------------------
char *Programm::find_func(QString name)
{
    int i;
    for(i=0; i < func_index; i++)
        if(name == func_table[i].func_name)
            return func_table[i].loc;
    return '\0';
}

//Объявление глобальной переменной.-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Programm::decl_global()
{
    int vartype;

    get_token();  // определение типа
    vartype = tokenType; // storage type of variable
    do { /* обработка списка */
        Variable newVar;
        newVar.value = 0; // initialise
        newVar.v_type = vartype;

        get_token();  // read name

        global_vars[gvar_index].var_name = token;
        get_token();
        gvar_index++;}
    while(token[0] == ',');
    if(token[0] != ';')
        sntx_err(SEMI_EXPECTED);
}

//Объявление локальной переменной.---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Programm::decl_local()
{
    Variable i;

    get_token();  /* определение типа */
    i.v_type = tokenType;
    i.value = 0;  /* инициализация нулем */

    do { // обработка списка
        get_token(); // определение имя пременной
        i.var_name = token;
        local_push(i);
        //        eval_exp(&i.value);
        get_token();
        //       if (*token == '=')
    }
    while(token[0] == ',');

    if(token[0] != ';')
        sntx_err(SEMI_EXPECTED);
}

// Вызов функции. -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Programm::call(void)
{
    char *loc, *temp;
    int lvartemp;

    loc = find_func(token); // найти точку входа функции
    if(loc == NULL)
        sntx_err(FUNC_UNDEF); // функция не определена
    else {
        lvartemp = lvartos;  // запоминание индекса стека локальных переменных
        get_args();  // получение аргумента функции
        temp = prog; // запоминание адреса возврата
        func_push(lvartemp);  // запоминание индекса стека локальных переменных
        prog = loc;  // переустановка prog в начало функции
        get_params(); // загрузка параметров функции значениями аргументов
        interp_block(); // интерпретация функции
        prog = temp; // восстановление prog
        lvartos = func_pop();} // восстановление стека локальных переменных
}

// Заталкивание аргументов функций в стек локальных переменных.-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Programm::get_args(void)
{
    int value, count, temp[NUM_PARAMS];
    Variable i;

    switch (tokenType){
    case BOOL: i.value = new bool; break;
    case INT: i.value = new int; break;
    case CHAR: i.value = new char; break;}
    count = 0;
    get_token();
    if(token[0] != '(')
        sntx_err(PAREN_EXPECTED);
    do {   // обработка списка значений
        eval_exp(&value);
        temp[count] = value;  // временное запоминание
        get_token();
        count++;}
    while(token[0] == ',');
    count--;
    for(; count>=0; count--) { // затолкнуть в local_var_stack в обратном порядке
        i.value = (void *)temp[count];
        i.v_type = ARG;
        local_push(i);}
}

// Получение параметров функции. ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Programm::get_params(void)
{
    Variable *p;
    int i;

    i = lvartos-1;
    do { /* обработка списка параметров */
        get_token();
        p = &local_var_stack[i];
        if(token[0] != ')' ) {
            if(tokenType != INT && tokenType != CHAR)
                sntx_err(TYPE_EXPECTED);
            p->v_type = token_type;
            get_token();
            p->var_name = token; // связывание имени пераметров с аргументом, уже находящимся в стеке локальных переменных
            get_token();
            i--;}
        else
            break;}
    while(token[0] == ',');
    if(token[0] != ')')
        sntx_err(PAREN_EXPECTED);
}

// Возврат из функции. ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Programm::func_ret(void)
{
    int value = 0;

    eval_exp(&value); // получение возвращаемого значения, если оно есть
    ret_value = value;
}

// Затолкнуть локальную переменную.--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Programm::local_push(struct Variable i)
{
    local_var_stack.append(i);
    lvartos++;
}

// Выталкивание индекса в стеке локальных переменных.------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int Programm::func_pop(void)
{
    functos--;
    if(functos < 0)
        sntx_err(RET_NOCALL);
    return call_stack[functos];
}

// Запись индекса в стек локальных переменных. --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Programm::func_push(int i)
{
    if(functos > NUM_FUNC)
        sntx_err(NEST_FUNC);
    call_stack[functos] = i;
    functos++;
}

// Присваивание переменной значения.--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Programm::assign_var(QString var_name, int value)
{
    int i;
    for(i=lvartos-1; i >= call_stack[functos-1]; i--) { // проверка наличия локальной переменной
        if(local_var_stack[i].var_name == var_name) {
            local_var_stack[i].value = (void*)value;
            return;}}
    if(i < call_stack[functos-1])  // если переменная нелокальная, ищем ее в таблице глобальных переменных
        for(i=0; i < NUM_GLOBAL_VARS; i++)
            if(global_vars[i].var_name == var_name) {
                global_vars[i].value = (void*)value;
                return;
            }
    sntx_err(NOT_VAR); /* переменная не найдена */
}

// Получение значения переменной. --------------------------------------------------------------------------------------------------------------------------------------------------------------------
int Programm::find_var(QString s)
{
    int i;

    for(i=lvartos-1; i >= call_stack[functos-1]; i--) // проверка наличия переменной
        if(local_var_stack[i].var_name == s){
            rvalue = local_var_stack[i].value;
            return i;}

    for(i=0; i < NUM_GLOBAL_VARS; i++) // в противном случае проверим, может быть это глобальная переменная
        if(global_vars[i].var_name == s){
            rvalue = global_vars[i].value;
            return -i;}

    sntx_err(NOT_VAR); /* переменная не найдена */
}

// Если индентификатор является переменной, то возвращается 1, иначе 0.
int Programm::is_var(QString s)
{
    int i;

    for(i=lvartos-1; i >= call_stack[functos-1]; i--)   /* это локальная переменная ? */
        if(local_var_stack[i].var_name == token)
            return 1;
    for(i=0; i < NUM_GLOBAL_VARS; i++) /* если нет - поиск среди глобальных переменных */
        if(global_vars[i].var_name == s)
            return 1;

    return 0;
}

/* Выполнение оператора if. */
void Programm::exec_if(void)
{
    int cond;

    eval_exp(&cond); /* вычисление if-выражения */

    if(cond) { /* истина - интерпретация if-предложения */
        interp_block();
    }
    else { /* в противном случае пропуск if-предложения
            и выполнение else-предложения, если оно есть */
        find_eob(); /* поиск конца блока */
        get_token();

        if(tokenType != ELSE) {
            putback();  /* восстановление лексемы,
                     если else-предложение отсутсвует */
            return;
        }
        interp_block();
    }
}

/* Выполнение цикла while. */
void Programm::exec_while(void)
{
    int cond;
    char *temp;

    putback();
    temp = prog;  /* запоминание адреса начала цикла while */
    get_token();
    eval_exp(&cond);  /* вычисление управляющего выражения */
    if(cond) interp_block();  /* если оно истинно, то выполнить
                               интерпритацию */
    else {  /* в противном случае цикл пропускается */
        find_eob();
        return;
    }
    prog = temp;  /* возврат к началу цикла */
}

/* Выполнение цикла do. */
void Programm::exec_do(void)
{
    int cond;
    char *temp;

    putback();
    temp = prog;  /* запоминание адреса начала цикла */

    get_token(); /* найти начало цикла */
    interp_block(); /* интерпритация цикла */
    get_token();
    if(tokenType != WHILE) sntx_err(WHILE_EXPECTED);
    eval_exp(&cond); /* проверка условия цикла */
    if(cond) prog = temp; /* если условие истинно,
  то цикл выполняется, в противном случае происходит
  выход из цикла */
}

/* Поиск конца блока. */
void Programm::find_eob(void)
{
    int brace;

    get_token();
    brace = 1;
    do {
        get_token();
        if(token[0] == '{') brace++;
        else if(token[0] == '}') brace--;
    } while(brace);
}

/* Выполнение цикла for. */
void Programm::exec_for(void)
{
    int cond;
    char *temp, *temp2;
    int brace ;

    get_token();
    eval_exp(&cond);  /* инициализирующее выражение */
    if(token[0] != ';')
        sntx_err(SEMI_EXPECTED);
    prog++; /* пропуск ; */
    temp = prog;
    for(;;) {
        eval_exp(&cond);  /* проверка условия */
        if(token[0] != ';')
            sntx_err(SEMI_EXPECTED);
        prog++; /* пропуск ; */
        temp2 = prog;

        /* поиск начала тела цикла */
        brace = 1;
        while(brace) {
            get_token();
            if(token[0] == '(') brace++;
            if(token[0] == ')') brace--;
        }

        if(cond) interp_block();  /* если условие выполнено,
                                 то выполнить интерпритацию */
        else {  /* в противном случае обойти цикл */
            find_eob();
            return;
        }
        prog = temp2;
        eval_exp(&cond); /* вполнение инкремента */
        prog = temp;  /* возврат в начало цикла */
    }
}

/* Рекурсивный нисходящий синтаксический анализатор
   целочисленных выражений, содержащих переменные
   и вызовы функций.
*/


/* Здесь функции "стандартной библиотеки"
   объявлены таким образом, что их можно
   поместить во внутренюю таблицу функции.
*/


// Точка входа в синтаксический анализатор выражений. ---------------------------------------------------------------------------------------
void Programm::eval_exp(int *value)
{
    get_token();
    if(!token[0]) {
        sntx_err(NO_EXP);
        return;
    }
    if(token[0] == ';') {
        *value = 0; /* пустое выражение */
        return;
    }
    eval_exp0(value);
    putback(); /* возврат последней лексемы во входной поток */
}

// Обработка выражения в присваивании --------------------------------------------------------------------------------------------------------------------
void Programm::eval_exp0(int *value)
{
    QString temp;  /* содержит имя переменной,
                         которой присваивается значение */
    int temp_tok;

    if(token_type == IDENTIFIER) {
        if(is_var(token)) {  /* если эта переменная,
       посмотреть, присваивается ли ей значение */
            temp = token;
            temp_tok = token_type;
            get_token();
            if(token[0] == '=') {  /* это присваивание */
                get_token();
                eval_exp0(value);  /* вычислить присваемое значение */
                assign_var(temp, *value);  /* присвоить значение */
                return;
            }
            else {  /* не присваивание */
                putback();  /* востановление лексемы */
                token = temp;
                token_type = temp_tok;
            }
        }
    }
    eval_exp1(value);
}

// Обработка операций сравнения. --------------------------------------------------------------------------------------------------------------------
void Programm::eval_exp1(int *value)
{
    int partial_value;
    register char op;
    char relops[7] = {
        LEES, LEES_EQ, GREAT, GREAT_EQ, EQ, NOT_EQ, 0
    };

    eval_exp2(value);
    op = token[0];
    if(strchr(relops, op)) {
        get_token();
        eval_exp2(&partial_value);
        switch(op) {  /* вычисление результата операции сравнения */
        case LEES:
            *value = *value < partial_value;
            break;
        case LEES_EQ:
            *value = *value <= partial_value;
            break;
        case GREAT:
            *value = *value > partial_value;
            break;
        case GREAT_EQ:
            *value = *value >= partial_value;
            break;
        case EQ:
            *value = *value == partial_value;
            break;
        case NOT_EQ:
            *value = *value != partial_value;
            break;
        }
    }
}

//  Суммирование или вычисление двух термов. ---------------------------------------------------------------------------------------
void Programm::eval_exp2(int *value)
{
    register char  op;
    int partial_value;

    eval_exp3(value);
    while((op = token[0]) == '+' || op == '-') {
        get_token();

        eval_exp3(&partial_value);
        switch(op) { /* суммирование или вычитание */
        case '-':
            *value = *value - partial_value;
            break;
        case '+':
            *value = *value + partial_value;
            break;
        }
    }
}

// Умножение или деление двух множителей.--------------------------------------------------------------------------------------------------------------------
void Programm::eval_exp3(int *value)
{
    register char  op;
    int partial_value, t;

    eval_exp4(value);
    while((op = token[0]) == '*' || op == '/' || op == '%') {
        get_token();
        eval_exp4(&partial_value);
        switch(op) { /* умножение, деление или деление целых */
        case '*':
            *value = *value * partial_value;
            break;
        case '/':
            if(partial_value == 0) sntx_err(DIV_BY_ZERO);
            *value = (*value) / partial_value;
            break;
        case '%':
            t = (*value) / partial_value;
            *value = *value-(t * partial_value);
            break;
        }
    }
}

// Унарный + или -. --------------------------------------------------------------------------------------------------------------------
void Programm::eval_exp4(int *value)
{
    char  op;

    op = '\0';
    if(token[0] == '+' || token[0] == '-') {
        op = token[0];
        get_token();}
    eval_exp5(value);
    if(op)
        if(op == '-')
            *value = -(*value);
}

// Обработка выражения в скобках. --------------------------------------------------------------------------------------------------------------------
void Programm::eval_exp5(int *value)
{
    if((token[0] == '(')) {
        get_token();
        eval_exp0(value);   /* вычисление подвыражения */
        if(token[0] != ')')
            sntx_err(PAREN_EXPECTED);
        get_token();}
    else
        atom(value);
}

// Получение значения числа, переменной или функции. ---------------------------------------------------------------------------------------
void Programm::atom(int *value)
{
    switch(token_type) {
    case IDENTIFIER:
        if(intern_func.contains(token)) {  /* вызов функции из "стандартной билиотеки" */
            void (*functionPtr)() = intern_func.value(token);
            *value = functionPtr();}
        else
            if(find_func(token)) { /* вызов функции, определенной пользователем */
                call();
                *value = ret_value;}
            else{
                find_var(token); /* получение значения переменной */
                switch(tokenType){
                case INT:
                    *value = (int)rvalue;
                    break;

                case CHAR:
                    *value = (char)rvalue;
                    break;
                case BOOL: *value = (bool)rvalue; break;
                    get_token();
                    return;
                case NUMBER: /* числовая константа */
                    *value = atoi(token.toStdString().c_str());
                    get_token();
                    return;
                case DELIMITER: /* это символьная константа? */
                    if(token[0] == '\'') {
                        *value = *prog;
                        prog++;
                        if(*prog!='\'')
                            sntx_err(QUOTE_EXPECTED);
                        prog++;
                        get_token();
                        return;}
                }
            }
        if(token[0]==')')
            return; /* обработка пустого выражения */
        else sntx_err(SYNTAX); /* синтаксическая ошибка */
    default:
        sntx_err(SYNTAX);} /* синтаксическая ошибка */
}

// Вывод сообщения об ошибке. -------------------------------------------------------------------------------------------------------
void Programm::sntx_err(int error)
{
    char *p, *temp;
    int linecount = 0;
    register int i;

    QString e[]= {
        "синтаксическая ошибка", //SYNTAX
        "несбалансированные скобки", //UNBAL_PARENS
        "выражение отсутствует",//NO_EXP
        "ожидается знак равенства",//EQUALS_EXPECTED
        "неизвестная переменная",//NOT_VAR
        "ошибка в параметре",//PARAM_ERR
        "ожидается точка с запятой",//SEMI_EXPECTED
        "несбалансированные фигурные скобки",//UNBAL_BRACES
        "функция не определена",//FUNC_UNDEF
        "ожидается спецификатор типа",//TYPE_EXPECTED
        "слишком много вложенных вызовов функций",//NEST_FUNC
        "оператор return вне функции",//RET_NOCALL
        "ожидаются скобки",//PAREN_EXPECTED
        "ожидается while",//WHILE_EXPECTED
        "ожидается закрывающаяся кавычка",//QUOTE_EXPECTED
        "не строка",//NOT_TEMP
        "слишком много локальных переменных",//TOO_MANY_LVARS
        "деление на ноль",//DIV_BY_ZERO
        "Не найден вход в процедуру int main()"//NO_MAIN
    };

    /*   p = p_buf;
    if(prog)
        while(p != prog) {  //поиск номера строки с ошибкой
            p++;
            if(*p == '\r') {
                linecount++;
            }
        }*/
    co->insertPlainText(QString("[!!! Ошибка] Строка %1").arg(linecount)+" "+QString(e[error])+". Выражение \""+token+"\"\n");

    //Form1->PrgStpExecute(NULL); //Остановка программы;
}

//Замена служебных символов ----------------------------------------------------------------------------------
void Programm::RplcSysSmb(char *text)
{
    if(strchr(text,'\\')){
        size_t i = 0;
        while (text[i] != '\0'){
            if (text[i] == '\\' && (text[i+1] == 'n' || text[i+1] == 'r' || text[i+1] == 't')){
                switch (text.toStdString().c_str()[i+1]){
                case 'n':
                    text[i] = '\n';
                    break;

                case 'r':
                    text[i] = '\r';
                    break;

                case 't':
                    text[i] = '\t';
                    break;
                }

                // move to left
                size_t r = i+1;
                do{
                    token[r] = token[r+1];
                    r++;}
                while (token[r] != '\0');
            }
            i++;
        }
    }
}

// Считывание лексемы из входного потока. ------------------------------------------------------------------------------------------
int Programm::get_token(void)
{
    QString temp;
    token_type = 0;
    tokenType = NULLV;

    while(iswhite(*prog) && *prog)// пропуск пробелов, символов табуляции и пустой строки
        ++prog;

    if(*prog == '\0') { // конец текста программы
        token = "";
        tokenType = FINISHED;
        return (token_type = DELIMITER);}

    if(*prog == '/' && *(prog+1) == '*'){ // это комментарий   /*  */
        prog += 2;
        while(*prog != '*' && *(prog+1) != '/'){
            if(*(prog) == '\0')
                break;
            prog++;}}

    if(*prog == '/' && *(prog+1) == '/'){ // это комментарий   //  \r
        prog += 2;
        while(*prog != '\r'){
            if(*(prog) == '\0')
                break;
            prog++;}}

    if(strchr("{}", *prog)) { // ограничение блока
        temp.insert(temp.size(),*prog);
        prog++;
        return (token_type = BLOCK);}

    if(strchr("!<>=", *prog)) { // возможно, это оператор сравнения
        switch(*prog) {
        case '=':
            if(*(prog+1) == '=') { //Это оператор сравнения  ==
                prog+=2;
                temp.insert(temp.size(),EQ);
                temp.insert(temp.size(),EQ);}
            break;

        case '!':
            if(*(prog+1) == '=') { //Это оператор сравнения  !=
                prog+=2;
                temp.insert(temp.size(),NOT_EQ);
                temp.insert(temp.size(),NOT_EQ);}
            break;

        case '<':
            if(*(prog+1) == '=') {  //Это оператор сравнения  <=
                prog+=2;
                temp.insert(temp.size(),LEES_EQ);
                temp.insert(temp.size(),LEES_EQ);}
            else {                  //Это оператор сравнения  <
                prog++;
                temp.insert(temp.size(),LEES);}
            break;

        case '>':
            if(*(prog+1) == '=') {   //Это оператор сравнения  >=
                prog+=2;
                temp.insert(temp.size(),GREAT_EQ);
                temp.insert(temp.size(),GREAT_EQ);}
            else {
                prog++;          //Это оператор сравнения  >
                temp.insert(temp.size(),GREAT);}
            break;
        }

        if(token[0])
            return(token_type = DELIMITER);}

    if(strchr("+-*^/%=;(),'", *prog)){ // разделитель
        temp.insert(temp.size(),*prog++); // продвижение на следующую позицию
        token = temp;
        //prog++;
        return (token_type = DELIMITER);}

    if(*prog=='"') { // строка в кавычках
        prog++;
        while(*prog != '"'&& *prog != '\r')
            temp.insert(temp.size(),*prog++);
        if(*prog == '\r')
            sntx_err(SYNTAX);
        prog++;
        return (token_type = STRING);}

    if(isdigit(*prog)) { // число
        while(!isdelim(*prog))
            temp.insert(temp.size(),*prog++);

        return (token_type = NUMBER);}

    if(ismyalpha(*prog)) { // переменная или оператор
        while(!isdelim(*prog))
            temp.insert(temp.size(),*prog++);
        token_type = TEMP;
        token = temp;}

    if(token_type == TEMP) { // эта строка является оператором или переменной?
        tokenType = look_up(token); // преобразовать во внутренее представление

        if(tokenType != NULLV) // это зарезервированное слово
            token_type = KEYWORD;
        else
            token_type = IDENTIFIER;}

    return token_type;
}

// Возврат лексемы во входной поток.----------------------------------------------------------------------------
void Programm::putback(void)
{
    prog -= token.size();
}

//Поиск внутреннего представления лексемы в таблице лексем.-------------------------------------------------------------------
Tokens Programm::look_up(QString s)
{
    QString p = s.toLower();

    for(int i=0; keyWords[i].command.size(); i++) { // есть ли лексемы в таблице?
        if(keyWords[i].command == p)
            return keyWords[i].tok;}

    return NULLV; // незнакомый оператор
}

// Возвращает идекс функции во внутреннеq библиотеке, или -1, если не найдена. ----------------------------------------------
int Programm::internal_func(QString s)
{
    for(int i=0; intern_func[i].f_name.size(); i++) {
        if(intern_func[i].f_name.toStdString().c_str() == s)
            return i;}
    return -1;
}

// Возвращает 1, если с - разделитель. ----------------------------------------------------------------------------
int Programm::isdelim(char c)
{
    if(strchr(" !;,+-<>'/*%^=()", c) || c == 9 || c == '\r' || c == 0)
        return 1;
    return 0;
}

// Возвращает 1, если с - пробел или табуляция. ---------------------------------------------------------------------------------------
int Programm::iswhite(char c)
{
    if(c == ' ' || c == '\t' || *prog == '\r' || *prog == '\n')
        return 1;
    return 0;
}

//***** Библиотека функций Little C *******/
//
//      Сюда можно добавлять новые функции.
//      Только нужно так же добавить в таблицу функций intern_func

//getchar() возвращает код символа введёный в консоль --------------------------------------------------------------------------------

void Programm::call_getche()
{
    /*Form1->enter = 2;
  Form1->console->ReadOnly = false;
  Form1->SetFocus();
  Form1->console->SetFocus();
  Form1->Programm->Suspend();
  while(*prog!=')') prog++;
  prog++;   // продвижение к концу строки*/
    return 0;//Form1->Programm->s[0];
}

// Вывод символа на экран. -----------------------------------------------------------------------------------------------------------
void Programm::call_putch()
{
    int value;
    get_token();
    if(token[0]!='(')
        sntx_err(PAREN_EXPECTED);

    eval_exp(&value);
    co->insertPlainText(QString("%1").arg((char)value));
    get_token();

    if(token[0]!=')')
        sntx_err(PAREN_EXPECTED);

    //  get_token();

    //  if(*token!=';')
    //  sntx_err(SEMI_EXPECTED);
    return 0;
}

// Вызов функции puts().-------------------------------------------------------------------------------------------------------------------------------------------
void Programm::callPuts(void)
{
    get_token();
    if(token[0]!='(')
        sntx_err(PAREN_EXPECTED);

    get_token();

    if(token_type!=STRING)
        sntx_err(QUOTE_EXPECTED);

    co->insertPlainText(token);
    get_token();

    if(token[0]!=')')
        sntx_err(PAREN_EXPECTED);

    get_token();

    if(token[0]!=';')
        sntx_err(SEMI_EXPECTED);

    putback();
    return 0;
}

// Встроенная функция консольного вывода. ------------------------------------------------------------------------------------------------------------------------------------------
void Programm::call_print(void)
{
    int i;

    get_token();
    if(token[0]!='(')  sntx_err(PAREN_EXPECTED);

    get_token();
    if(token_type==STRING) { /* вывод строки */
        co->insertPlainText(token);
    }
    else {  /* вывод числа */
        putback();
        eval_exp(&i);
        co->insertPlainText(QString("%1").arg(i));
    }

    get_token();

    if(token[0]!=')') sntx_err(PAREN_EXPECTED);

    get_token();
    if(token[0]!=';')
        sntx_err(SEMI_EXPECTED);
    putback();
    return 0;
}

// Считывание целого числа с клавиатуры. ------------------------------------------------------------------------------------------------------------------------------------------
void Programm::getnum(void)
{
    /*Form1->enter = 1;
  Form1->n = 0;
  Form1->console->ReadOnly = false;
  Form1->SetFocus();
  Form1->console->SetFocus();
  Form1->console->OnChange(NULL);
  Form1->Programm->Suspend();
      while(*prog != ')') prog++;
  prog++;   продвижение к концу строки */
    return 0;//StrToInt(Form1->Programm->s);
    // while(get);

}

// Встроенная функция случайного числа. ----------------------------------------------------------------------------------------------------------
void Programm::rand(void)
{
    int i;
    i = 0;
    get_token();
    if(token.at(0) != '(')  sntx_err(PAREN_EXPECTED);

    get_token();
    if(token_type==INT) {
        putback();
        eval_exp(&i);
    }
    else {
        sntx_err(PAREN_EXPECTED);
    }

    get_token();

    if(token[0]!=')') sntx_err(PAREN_EXPECTED);

    get_token();
    if(token[0]!=';')
        sntx_err(SEMI_EXPECTED);
    putback();
    return 0;//random(StrToInt(i));
}

//---------------------------------------------------------------------------

//   Important: Methods and properties of objects in VCL can only be
//   used in a method called using Synchronize, for example:
//
//      Synchronize(UpdateCaption);
//
//   where UpdateCaption could look like:
//
//      void __fastcall Programm::UpdateCaption()
//      {
//        Form1->Caption = "Updated in a thread";
//      }
//---------------------------------------------------------------------------

Programm::Programm() : QThread()
{

}

//---------------------------------------------------------------------------
void Programm::Execute(QString programText)
{
    intern_func.insert("getche", call_getche);
    intern_func.insert("putch", call_putch);
    intern_func.insert("puts", callPuts);
    intern_func.insert("print", call_print);
    intern_func.insert("getch", callGetch);
    intern_func.insert("random", rand);

    keyWords.insert("if", IF);
    keyWords.insert("else", ELSE);
    keyWords.insert("for", FOR);
    keyWords.insert("do", DO);
    keyWords.insert("while", WHILE);
    keyWords.insert("char", CHAR);
    keyWords.insert("int", INT);
    keyWords.insert("bool", BOOL);
    keyWords.insert("return", RETURN);
    keyWords.insert("break", BREAK);

    //co->clear();
    *rvalue = (int) 0;

    /* выделение памяти для программы */
    size_t programTextSize = programText.toStdString().length()+1;

    if((p_buf = (char *) malloc(programTextSize)) == NULL) {
        //co->insertPlainText("Выделить память не удалось");
        return;
    }

    memcpy(p_buf, programText.toStdString().c_str(), programTextSize);

//    загрузка программы для выполнения
//    if(!load_program(p_buf)) {
//        //co->insertPlainText("Не удалось загрузить программу");
//        return;
//    }//exit(1);

    RplcSysSmb(p_buf); //replace system symbols


    prog = p_buf; //set programm pointer to start

    prescan(); /* определение адресов всех функций
                и глобальных переменных программы */

    lvartos = 0;     /* инициализация индекса стека локальных переменных */
    functos = 0;     /* инициализация индекса стека вызова (CALL) */

    /* первой вызывается main() */
    prog = find_func("main"); /* поиск точки входа программы */

    if(!prog) { /* функция main() неправильна или отсутствует */
        sntx_err(NO_MAIN);
        return;
    }

    prog--; /* возврат к открывающейся скобке ( */
    token = "main";
    call(); /* начало интерпритации main() */
    // Form1->PrgStpExecute(NULL);
}
//---------------------------------------------------------------------------
