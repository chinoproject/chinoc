%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include "token.h"
    #include "gc.h"
%}
delim   [ \t]
ws      {delim}+

letter  [a-zA-Z]
digit   [0-9]
ASC     [\x-\x7f]
ASCN    [\x-\t\v-\x7f]
U       [\x-\xbf]
U2      [\xc2-\xdf]
U3      [\xe0-\xef]
U4      [\xf0-\xf4]
UANY    {ASC}|{U2}{U}|{U3}{U}{U}|{U4}{U}{U}{U}
UANYN   {ASCN}|{U2}{U}|{U3}{U}{U}|{U4}{U}{U}{U}

id          [a-zA-Z_][a-zA-Z_0-9]*
integer     [-1]*{digit}+
string      \"({ASCN}|{U2}{U}|{U3}{U}{U}|{U4}{U}{U}{U})*\"
number      [-]*(((\.{digit}+)|({digit}+\.{digit}*)))(([e][+-]?{digit}+)?)
%%
{ws}        { /*skip*/ }
if          {return IF;}
else        {return ELSE;}
switch      {return SWITCH;}
struct      {return STRUCT;}
union       {return UNION;}
enum        {return ENUM;}
typedef     {return TYPEDEF;}
const       {return CONST;}
unsigned    {return UNSIGNED;}
signed      {return SIGNED;}
extern      {return EXTERN;}
register    {}
static      {return STATIC;}
do          {return DO;}
while       {return WHILE;}
goto        {return GOTO;}
continue    {return CONTINUE;}
break       {return BREAK;}
default     {return DEFAULT;}
return      {return RETURN;}
i4          {return INT;}
i2          {return SHORT;}
i1          {return CHAR;}
float       {return FLOAT;}
double      {return DOUBLE;}
void        {return VOID;}
case        {return CASE;}
">>="       {return LSHIFT_ASSIGN;}
"<<="       {return RSHIFT_ASSIGN;}
">>"        {return RIGHT_SHIFT;}
"<<"        {return LEFT_SHIFT;}
"<="        {return LE;}
">="        {return GE;}
"->"        {return POINT;}
"<"         {return LT;}
">"         {return GT;}
"=="        {return EQ;}
"="         {return ASSIGN;}
"~="        {return NOT_ASSIGN;}
"+="        {return ADD_ASSIGN;}
"-="        {return SUB_ASSIGN;}
"*="        {return MUL_ASSIGN;}
"/="        {return DIV_ASSIGN;}
"&="        {return AND_ASSIGN;}
"|="        {return OR_ASSIGN;}
"^="        {return XOR_ASSIGN;}
"++"        {return INC;}
"--"        {return DEC;}
"?"         {return QUMARK;}
"!="        {return NE;}
"&&"        {return LOGIC_AND;}
"||"        {return LOGIC_OR;}
"!"         {return LOGIC_NOT;}
"&"         {return BIT_AND;}
"|"         {return BIT_OR;}
"~"         {return BIT_NOT;}
"^"         {return BIT_XOR;}
"("         {return LP;}
")"         {return RP;}
"["         {return LB;}
"]"         {return RB;}
"{"         {return LBB;}
"}"         {return RBB;}
":"         {return COLON;}
"+"         {return ADD;}
"-"         {return MINUS;}
"*"         {return STAR;}
"/"         {return DIV;}
"%"         {return MOD;}
"."         {return DOT;}
";"         {return SEM;}
"'"         {return SQ;}
"\""        {return DQ;}
","         {return COMMA;}
{string}    {metadata_ptr = set_content(STRING); return STRING;}
{id}        {printf("id");if(!strcmp(yytext,"sizeof")) return SIZEOF; metadata_ptr = set_content(ID); return ID;}
{number}    {metadata_ptr = set_content(NUMBER); return NUMBER;}
{integer}   {metadata_ptr = set_content(INTEGER); return INTEGER;}
%%

struct metadata_struct *set_content(int type) {
    struct metadata_struct *id = _malloc(sizeof(struct metadata_struct));
    if (id == NULL) {
        printf("OOM\n");
        exit(1);
    }
    size_t len = strlen(yytext);
    id->type = type;
    id->content = _malloc(sizeof(char)*(len + 1));
    if (id->content == NULL) {
        printf("OOM");
        free(id);
        exit(2);
    }
    strncpy(id->content,yytext,len);
    id->content[len] = 0;
    id->len = len;
    return id;
}