#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "value.h"

#define BUFFER_TRANCHE 2

enum Token
{
    LPAREN, // == 0
    LBRACKET,
    LCURLY,
    QUOTE,
    BACKQUOTE,
    TILDE, // == 5
    TILDE_AMPERSAND,
    AMPERSAND,
    CARAT,
    RPAREN,
    RBRACKET, // == 10
    RCURLEY,
    IDENTIFIER,
    LITERAL_STRING,
    LITERAL_NUMBER,
    ERROR, // == 15
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

// static void printer_lexer(char *msg, Lexer *lexer)
// {
//     printf("ls: %s: {., %d, {%d, %d, %d}, %d, (%d, %d, %d)\n", msg, lexer->content_length, lexer->start.offset, lexer->start.column, lexer->start.line, lexer->token, lexer->end.offset, lexer->end.column, lexer->end.line);
// }

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

static void set_token(Lexer *lexer, enum Token token, Position *start, Position *end)
{
    lexer->start = *start;
    lexer->token = token;
    lexer->end = *end;
}

static void next_token(Lexer *lexer)
{
    Position cursor = lexer->end;

    advance_position(lexer, &cursor);

    while (1)
    {
        int current = CHARACTER_AT_POSITION(lexer, &cursor);
        if (current > 0 && current < 33)
            advance_position(lexer, &cursor);
        else if (current == ';')
        {
            advance_position(lexer, &cursor);
            current = CHARACTER_AT_POSITION(lexer, &cursor);
            while (current != 0 && current != 13)
            {
                advance_position(lexer, &cursor);
                current = CHARACTER_AT_POSITION(lexer, &cursor);
            }
        }
        else
            break;
    }

    switch (CHARACTER_AT_POSITION(lexer, &cursor))
    {
    case 0:
        set_token(lexer, EOS, &cursor, &cursor);
        break;

    case '(':
        set_token(lexer, LPAREN, &cursor, &cursor);
        break;

    case ')':
        set_token(lexer, RPAREN, &cursor, &cursor);
        break;

    case '{':
        set_token(lexer, LCURLY, &cursor, &cursor);
        break;

    case '}':
        set_token(lexer, RCURLEY, &cursor, &cursor);
        break;

    case '[':
        set_token(lexer, LBRACKET, &cursor, &cursor);
        break;

    case ']':
        set_token(lexer, RBRACKET, &cursor, &cursor);
        break;

    case '\'':
        set_token(lexer, QUOTE, &cursor, &cursor);
        break;

    case '`':
        set_token(lexer, BACKQUOTE, &cursor, &cursor);
        break;

    case '~':
    {
        Position start = cursor;

        advance_position(lexer, &cursor);
        if (CHARACTER_AT_POSITION(lexer, &cursor) == '@')
            set_token(lexer, TILDE_AMPERSAND, &start, &cursor);
        else
            set_token(lexer, TILDE, &cursor, &cursor);
        break;
    }

    case '^':
        set_token(lexer, CARAT, &cursor, &cursor);
        break;

    default:
    {
        Position start = cursor;

        int current = CHARACTER_AT_POSITION(lexer, &cursor);
        if (current >= '0' && current <= '9')
        {
            current = CHARACTER_AT_NEXT_POSITION(lexer, &cursor);
            while (current >= '0' && current <= '9')
            {
                advance_position(lexer, &cursor);
                current = CHARACTER_AT_NEXT_POSITION(lexer, &cursor);
            }
            set_token(lexer, LITERAL_NUMBER, &start, &cursor);
            break;
        }
        else if (current == '"')
        {
            advance_position(lexer, &cursor);
            current = CHARACTER_AT_NEXT_POSITION(lexer, &cursor);
            while (1)
            {
                if (current == 0)
                {
                    set_token(lexer, ERROR, &start, &cursor);
                    break;
                }
                else if (current == '"')
                {
                    advance_position(lexer, &cursor);
                    set_token(lexer, LITERAL_STRING, &start, &cursor);
                    break;
                }
                else
                {
                    advance_position(lexer, &cursor);
                    current = CHARACTER_AT_NEXT_POSITION(lexer, &cursor);
                }
            }
        }
        else
        {
            current = CHARACTER_AT_NEXT_POSITION(lexer, &cursor);
            // \s\[\]{}('"`,;
            while (current > 32 && current != '[' && current != ']' && current != '{' && current != '}' && current != '(' && current != ')' && current != '\'' && current != '"' && current != '`' && current != ',' && current != ';')
            {
                advance_position(lexer, &cursor);
                current = CHARACTER_AT_NEXT_POSITION(lexer, &cursor);
            }
            set_token(lexer, IDENTIFIER, &start, &cursor);
        }
    }
    }
}

static Lexer initialise_lexer(char *content)
{
    Position position = {-1, 1, 0};

    Lexer lexer = {content, strlen(content), position, ERROR, position};
    next_token(&lexer);

    return lexer;
}

static ReturnValue parse(Lexer *lexer)
{
    switch (lexer->token)
    {
    case QUOTE:
    {
        next_token(lexer);
        ReturnValue v = parse(lexer);
        if (IS_SUCCESSFUL(v))
        {
            Value *quote = mkSymbol("quote");
            Value *result = mkPair(quote, v.value);
            UNPIN(quote);
            UNPIN(v.value);

            ReturnValue result1 = {0, result};
            return result1;
        }
        else
            return v;
    }
    case LITERAL_STRING:
    {
        lexer->content[lexer->end.offset] = 0;
        Value *v = mkString((char *)(lexer->content + lexer->start.offset + 1));
        lexer->content[lexer->end.offset] = '"';

        next_token(lexer);

        ReturnValue result = {0, v};
        return result;
    }

    case LITERAL_NUMBER:
    {
        int c = lexer->content[lexer->end.offset + 1];
        lexer->content[lexer->end.offset + 1] = 0;
        Value *v = mkNumber(atoi((char *)(lexer->content + lexer->start.offset)));
        lexer->content[lexer->end.offset + 1] = c;

        next_token(lexer);
        ReturnValue result = {0, v};
        return result;
    }

    case IDENTIFIER:
    {
        int c = lexer->content[lexer->end.offset + 1];
        lexer->content[lexer->end.offset + 1] = 0;
        Value *v = (lexer->content[lexer->start.offset] == ':')
                       ? mkKeyword((char *)(lexer->content + lexer->start.offset))
                       : mkSymbol((char *)(lexer->content + lexer->start.offset));
        lexer->content[lexer->end.offset + 1] = c;

        next_token(lexer);
        ReturnValue result = {0, v};
        return result;
    }

    case LPAREN:
    {
        next_token(lexer);

        if (lexer->token == RPAREN)
        {
            next_token(lexer);

            ReturnValue result = {0, VNil};
            return result;
        }
        else
        {
            ReturnValue car = parse(lexer);

            if (IS_SUCCESSFUL(car))
            {
                Value *head = mkPair(car.value, VNil);
                UNPIN(car.value);
                Value *cursor = head;

                while (1)
                {
                    if (lexer->token == EOS)
                    {
                        ReturnValue result = {1, head};
                        return result;
                    }
                    else if (lexer->token == RPAREN)
                    {
                        next_token(lexer);
                        ReturnValue result = {0, head};
                        return result;
                    }
                    else
                    {
                        ReturnValue next = parse(lexer);

                        if (IS_SUCCESSFUL(next))
                        {
                            cursor->pairV.cdr = mkPair(next.value, VNil);
                            UNPIN(next.value);
                            UNPIN(cursor->pairV.cdr);
                            cursor = CDR(cursor);
                        }
                        else
                        {
                            UNPIN(head);
                            return next;
                        }
                    }
                }
            }
            else
                return car;
        }
    }

    case LBRACKET:
    {
        next_token(lexer);
        if (lexer->token == RBRACKET)
        {
            next_token(lexer);

            ReturnValue result = {0, VEmptyVector};
            return result;
        }
        else
        {
            ReturnValue v = parse(lexer);

            if (IS_SUCCESSFUL(v))
            {
                Value **buffer = (Value **)malloc(BUFFER_TRANCHE * sizeof(Value *));
                int buffer_length = BUFFER_TRANCHE;
                int buffer_end = 1;
                buffer[0] = v.value;

                UNPIN(v.value);

                while (1)
                {
                    if (lexer->token == EOS)
                    {
                        ReturnValue result = {1, mkVector(buffer, buffer_end)};
                        free(buffer);

                        return result;
                    }
                    else if (lexer->token == RBRACKET)
                    {
                        next_token(lexer);
                        ReturnValue result = {0, mkVector(buffer, buffer_end)};
                        free(buffer);

                        return result;
                    }
                    else
                    {
                        ReturnValue next = parse(lexer);

                        if (IS_SUCCESSFUL(next))
                        {
                            if (buffer_end == buffer_length - 1)
                            {
                                int new_buffer_length = buffer_length + BUFFER_TRANCHE;
                                Value **new_buffer = (Value **)malloc(new_buffer_length * sizeof(Value *));
                                memcpy(new_buffer, buffer, buffer_end * sizeof(Value *));
                                free(buffer);
                                buffer_length = new_buffer_length;
                                buffer = new_buffer;
                            }
                            buffer[buffer_end] = next.value;
                            buffer_end += 1;
                            UNPIN(next.value);
                        }
                        else
                        {
                            free(buffer);
                            return next;
                        }
                    }
                }
            }
            else
                return v;
        }
    }


    default:
    {
        next_token(lexer);

        ReturnValue result = {0, VNil};
        return result;
    }
    }
}

ReturnValue Reader_read(char *content)
{
    struct LexerState lexer = initialise_lexer(content);

    // printer_lexer("initial", &lexer);

    return parse(&lexer);
}
