#include <stdio.h>
#include <string.h>

#include "value.h"

enum Token
{
    LPAREN,
    LBRACKET,
    LCURLY,
    QUOTE,
    BACKQUOTE,
    TILDE,
    TILDE_AMPERSAND,
    AMPERSAND,
    CARAT,
    RPAREN,
    RBRACKET,
    RCURLEY,
    IDENTIFIER,
    LITERAL_STRING,
    LITERAL_NUMBER,
    ERROR,
    EOS,
};

typedef struct PositionStruct
{
    int offset;
    int line;
    int column;
} Position;

typedef struct LexerState
{
    char *content;
    int content_length;
    Position start;
    enum Token token;
    Position end;
} Lexer;

#define CHARACTER_AT_POSITION(lexer, position) ((lexer)->content[(position)->offset])
#define CHARACTER_AT_NEXT_POSITION(lexer, position) ((position)->offset < (lexer)->content_length ? (lexer)->content[(position)->offset + 1] : 0)

static void printer_lexer(char *msg, Lexer *lexer)
{
    printf("ls: %s: {., %d, {%d, %d, %d}, %d, (%d, %d, %d)\n", msg, lexer->content_length, lexer->start.offset, lexer->start.column, lexer->start.line, lexer->token, lexer->end.offset, lexer->end.column, lexer->end.line);
}

static void advance_position(Lexer *lexer, Position *position)
{
    if (position->offset < lexer->content_length)
    {
        int char_at_position = CHARACTER_AT_POSITION(lexer, position);

        if (char_at_position == '\n')
        {
            position->line += 1;
            position->column = 1;
        }
        else
        {
            position->column += 1;
        }
        position->offset += 1;
    }
}

static Lexer set_token(Lexer *lexer, enum Token token, Position *start, Position *end)
{
    Lexer result = {lexer->content, lexer->content_length, *start, token, *end};
    // printer_lexer("set_token", &result);
    return result;
}

static Lexer next_token(Lexer lexer)
{
    Lexer result = lexer;
    Position cursor = lexer.end;

    advance_position(&result, &cursor);

    while (1)
    {
        int current = CHARACTER_AT_POSITION(&lexer, &cursor);
        if (current > 0 && current < 33)
            advance_position(&result, &cursor);
        else if (current == ';')
        {
            advance_position(&result, &cursor);
            current = CHARACTER_AT_POSITION(&lexer, &cursor);
            while (current != 0 && current != 13)
            {
                advance_position(&result, &cursor);
                current = CHARACTER_AT_POSITION(&lexer, &cursor);
            }
        }
        else
            break;
    }

    switch (CHARACTER_AT_POSITION(&lexer, &cursor))
    {
    case 0:
        return set_token(&result, EOS, &cursor, &cursor);

    case '(':
        return set_token(&result, LPAREN, &cursor, &cursor);

    case ')':
        return set_token(&result, RPAREN, &cursor, &cursor);

    case '{':
        return set_token(&result, LCURLY, &cursor, &cursor);

    case '}':
        return set_token(&result, RCURLEY, &cursor, &cursor);

    case '[':
        return set_token(&result, LBRACKET, &cursor, &cursor);

    case ']':
        return set_token(&result, RBRACKET, &cursor, &cursor);

    case '\'':
        return set_token(&result, QUOTE, &cursor, &cursor);

    case '`':
        return set_token(&result, BACKQUOTE, &cursor, &cursor);

    case '~':
    {
        Position start = cursor;

        advance_position(&result, &cursor);
        if (CHARACTER_AT_POSITION(&lexer, &cursor) == '@')
            return set_token(&result, TILDE_AMPERSAND, &start, &cursor);
        else
            return set_token(&result, TILDE, &cursor, &cursor);
    }

    case '^':
        return set_token(&result, CARAT, &cursor, &cursor);

    default:
    {
        Position start = cursor;

        int current = CHARACTER_AT_POSITION(&lexer, &cursor);
        if (current >= '0' && current <= '9')
        {
            current = CHARACTER_AT_NEXT_POSITION(&lexer, &cursor);
            while (current >= '0' && current <= '9')
            {
                advance_position(&result, &cursor);
                current = CHARACTER_AT_NEXT_POSITION(&lexer, &cursor);
            }
            return set_token(&result, LITERAL_NUMBER, &start, &cursor);
        }
        else if (current == '"')
        {
            advance_position(&result, &cursor);
            current = CHARACTER_AT_NEXT_POSITION(&lexer, &cursor);
            while (1)
            {
                if (current == 0)
                {
                    return set_token(&result, ERROR, &start, &cursor);
                }
                else if (current == '"')
                {
                    advance_position(&result, &cursor);
                    return set_token(&result, LITERAL_STRING, &start, &cursor);
                }
                else
                {
                    advance_position(&result, &cursor);
                    current = CHARACTER_AT_NEXT_POSITION(&lexer, &cursor);
                }
            }
        }
        else
        {
            current = CHARACTER_AT_NEXT_POSITION(&lexer, &cursor);
            // \s\[\]{}('"`,;
            while (current > 32 && current != '[' && current != ']' && current != '{' && current != '}' && current != '(' && current != ')' && current != '\'' && current != '"' && current != '`' && current != ',' && current != ';')
            {
                advance_position(&result, &cursor);
                current = CHARACTER_AT_NEXT_POSITION(&lexer, &cursor);
            }
            return set_token(&result, CARAT, &start, &cursor);
        }
    }
    }
}

static Lexer initialise_lexer(char *content)
{
    Position position = {-1, 1, 0};

    Lexer lexer = {content, strlen(content), position, ERROR, position};

    return next_token(lexer);
}

static ReturnValue parse(Lexer *lexer)
{
    switch (lexer->token)
    {
    case LITERAL_STRING:
    {
        lexer->content[lexer->end.offset] = 0;
        Value *v = mkString((char *)(lexer->content + lexer->start.offset + 1));
        lexer->content[lexer->end.offset] = '"';
        ReturnValue result = {0, v};
        return result;
    }

    case LITERAL_NUMBER:
    {
        int c = lexer->content[lexer->end.offset + 1];
        lexer->content[lexer->end.offset + 1] = 0;
        Value *v = mkNumber(atoi((char *)(lexer->content + lexer->start.offset)));
        lexer->content[lexer->end.offset + 1] = c;
        ReturnValue result = {0, v};
        return result;
    }

    default:
    {
        ReturnValue result = {0, VNil};
        return result;
    }
    }
}

ReturnValue Reader_read(char *content)
{
    struct LexerState lexer = initialise_lexer(content);

    // printf("ls: content: %s\n", content);
    // printer_lexer("initial", &lexer);

    return parse(&lexer);
}
