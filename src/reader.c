#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "buffer.h"
#include "exceptions.h"
#include "map.h"
#include "printer.h"
#include "value.h"

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
    ERROR_UNENCLOSED_QUOTE, // == 15
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
    char *source_name;
    char *content;
    int content_length;
    Position start;
    enum Token token;
    Position end;
} Lexer;

#define CHARACTER_AT_POSITION(lexer, position) ((lexer)->content[(position)->offset])
#define CHARACTER_AT_NEXT_POSITION(lexer, position) ((position)->offset < (lexer)->content_length ? (lexer)->content[(position)->offset + 1] : 0)

static struct Exception_Position *mkExceptionPosition(struct LexerState *lexer)
{
    struct Exception_Position *position = (struct Exception_Position *)malloc(sizeof(struct Exception_Position));
    position->source_name = strdup(lexer->source_name);
    position->startOffset = lexer->start.offset;
    position->startColumn = lexer->start.column;
    position->startLine = lexer->start.line;
    position->endOffset = lexer->end.offset;
    position->endColumn = lexer->end.column;
    position->endLine = lexer->end.line;

    int c = lexer->content[lexer->end.offset + 1];
    lexer->content[lexer->end.offset + 1] = 0;
    position->content = strdup((char *)(lexer->content + lexer->start.offset));
    lexer->content[lexer->end.offset + 1] = c;

    return position;
}

static void freeExceptionPosition(struct Exception_Position *position)
{
    free(position->source_name);
    free(position->content);
    free(position);
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
            position->column += 1;

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
            while (current != 0 && current != 13 && current != 10)
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
        {
            set_token(lexer, TILDE_AMPERSAND, &start, &cursor);
        }
        else
            set_token(lexer, TILDE, &start, &start);
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
            current = CHARACTER_AT_NEXT_POSITION(lexer, &cursor);

            while (1)
            {
                if (current == 0)
                {
                    set_token(lexer, ERROR_UNENCLOSED_QUOTE, &start, &cursor);
                    break;
                }
                else if (current == '\\')
                {
                    advance_position(lexer, &cursor);
                    advance_position(lexer, &cursor);
                    current = CHARACTER_AT_NEXT_POSITION(lexer, &cursor);
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
            while (current > 32 && current != '[' && current != ']' && current != '{' && current != '}' && current != '(' && current != ')' && current != '"' && current != '`' && current != ',')
            {
                advance_position(lexer, &cursor);
                current = CHARACTER_AT_NEXT_POSITION(lexer, &cursor);
            }
            set_token(lexer, IDENTIFIER, &start, &cursor);
        }
    }
    }
}

static Lexer initialise_lexer(char *source_name, char *content)
{
    Position position = {-1, 1, 0};

    Lexer lexer = {source_name, content, strlen(content), position, EOS, position};
    next_token(&lexer);

    return lexer;
}

static Value *parse(Lexer *lexer)
{
    switch (lexer->token)
    {
    case QUOTE:
    {
        next_token(lexer);
        Value *v = parse(lexer);
        return (IS_SUCCESSFUL(v)) ? mkPair(mkSymbol("quote"), mkPair(v, VNull)) : v;
    }

    case BACKQUOTE:
    {
        next_token(lexer);
        Value *v = parse(lexer);
        return (IS_SUCCESSFUL(v)) ? mkPair(mkSymbol("quasiquote"), mkPair(v, VNull)) : v;
    }

    case TILDE:
    {
        next_token(lexer);
        Value *v = parse(lexer);
        return (IS_SUCCESSFUL(v)) ? mkPair(mkSymbol("unquote"), mkPair(v, VNull)) : v;
    }

    case TILDE_AMPERSAND:
    {
        next_token(lexer);
        Value *v = parse(lexer);
        return (IS_SUCCESSFUL(v)) ? mkPair(mkSymbol("splice-unquote"), mkPair(v, VNull)) : v;
    }

    case LITERAL_STRING:
    {
        char *to = (char *)malloc(lexer->end.offset - lexer->start.offset);
        char *to_free = to;
        char *from = lexer->content + lexer->start.offset + 1;

        while (*from != '"')
        {
            if (*from == '\\')
            {
                from += 1;
                if (*from == 'x')
                {
                    from += 1;
                    char *start = from;

                    while (*from != ';' && *from != '\0')
                        from += 1;

                    if (*from == ';')
                    {
                        *from = '\0';
                        char ch = (char)strtol(start, NULL, 16);

                        if (ch < 0)
                            ch = 0;

                        *to = ch;
                        from += 1;
                    }
                    else
                    {
                        free(to_free);
                        struct Exception_Position *position = mkExceptionPosition(lexer);
                        Value *e = exceptions_illegal_token(position);
                        freeExceptionPosition(position);
                        return e;
                    }
                }
                else
                {
                    if (*from == 'n')
                        *to = 10;
                    else if (*from == 't')
                        *to = 9;
                    else if (*from == 'r')
                        *to = 13;
                    else if (*from == '"')
                        *to = '"';
                    else if (*from == '\\')
                        *to = '\\';
                    else
                    {
                        free(to_free);
                        struct Exception_Position *position = mkExceptionPosition(lexer);
                        Value *e = exceptions_illegal_token(position);
                        freeExceptionPosition(position);
                        return e;
                    }

                    from += 1;
                }
            }
            else
            {
                *to = *from;
                from += 1;
            }
            to += 1;
        }
        *to = '\0';

        next_token(lexer);
        return mkStringUse(to_free);
    }

    case LITERAL_NUMBER:
    {
        int c = lexer->content[lexer->end.offset + 1];
        lexer->content[lexer->end.offset + 1] = 0;
        Value *v = mkNumber(atoi((char *)(lexer->content + lexer->start.offset)));
        lexer->content[lexer->end.offset + 1] = c;

        next_token(lexer);

        return v;
    }

    case IDENTIFIER:
    {
        int c = lexer->content[lexer->end.offset + 1];
        lexer->content[lexer->end.offset + 1] = 0;

        if (lexer->content[lexer->start.offset] == ':')
        {
            Value *v = mkKeyword((char *)(lexer->content + lexer->start.offset));
            lexer->content[lexer->end.offset + 1] = c;

            next_token(lexer);
            return v;
        }
        else
        {
            char *s = strdup((char *)(lexer->content + lexer->start.offset));
            lexer->content[lexer->end.offset + 1] = c;
            next_token(lexer);

            if (strcmp(s, ".") != 0 && strchr(s, '.') != NULL)
            {
                Value *root = VNull;
                Value **root_cursor = &root;

                Value_append_to_list(mkSymbol("map-get"), &root_cursor);

                int is_first = 1;
                char *s_cursor = s;
                char *rest = strchr(s_cursor, '.');
                while (rest != NULL)
                {
                    *rest = '\0';
                    if (strcmp(s_cursor, "") != 0)
                    {
                        Value_append_to_list(
                            is_first ? mkSymbol(s_cursor) : mkPair(mkSymbol("quote"), mkPair(mkSymbol(s_cursor), VNull)),
                            &root_cursor);
                        is_first = 0;
                    }
                    s_cursor = rest + 1;
                    rest = strchr(s_cursor, '.');
                }
                if (strcmp(s_cursor, "") != 0)
                    Value_append_to_list(
                        mkPair(mkSymbol("quote"), mkPair(mkSymbol(s_cursor), VNull)),
                        &root_cursor);
                free(s);
                return root;
            }
            else if (strcmp(s, "#t") == 0)
            {
                free(s);
                return VTrue;
            }
            else if (strcmp(s, "#f") == 0)
            {
                free(s);
                return VFalse;
            }
            else if (strncmp(s, "#\\", 2) == 0)
            {
                if (strlen(s) != 3)
                {
                    struct Exception_Position *position = mkExceptionPosition(lexer);
                    Value *e = exceptions_illegal_token(position);
                    freeExceptionPosition(position);
                    return e;
                }

                return mkCharacter(s[2]);
            }
            else if (strncmp(s, "#x", 2) == 0)
            {
                char ch = (char)strtol(s + 2, NULL, 16);

                if (ch < 0)
                    ch = 0;

                return mkCharacter(ch);
            }
            else
            {
                // if (strcmp(s, "pair") == 0)
                //     printf("%s:%d:%d\n", lexer->source_name, lexer->start.line, lexer->start.column);

                Value *result = mkSymbol(s);
                free(s);
                return result;
            }
        }
    }

    case LPAREN:
    {
        next_token(lexer);
        if (lexer->token == RPAREN)
        {
            next_token(lexer);
            return VNull;
        }

        Value *car = parse(lexer);
        if (!IS_SUCCESSFUL(car))
            return car;

        Value *head = mkPair(car, VNull);
        Value *cursor = head;

        while (1)
        {
            if (lexer->token == EOS)
            {
                struct Exception_Position *position = mkExceptionPosition(lexer);
                Value *e = exceptions_unexpected_end_of_stream(")", position);
                freeExceptionPosition(position);
                return e;
            }

            if (lexer->token == RPAREN)
            {
                next_token(lexer);
                return head;
            }

            Value *next = parse(lexer);
            if (!IS_SUCCESSFUL(next))
                return next;

            cursor->pairV.cdr = mkPair(next, VNull);
            cursor = CDR(cursor);
        }
    }

    case LBRACKET:
    {
        next_token(lexer);
        if (lexer->token == RBRACKET)
        {
            next_token(lexer);
            return VEmptyVector;
        }

        Value *v = parse(lexer);
        if (!IS_SUCCESSFUL(v))
            return v;

        Buffer *buffer = buffer_init(sizeof(Value *));
        buffer_append(buffer, &v, 1);

        while (1)
        {
            if (lexer->token == EOS)
            {
                buffer_free(buffer);
                struct Exception_Position *position = mkExceptionPosition(lexer);
                Value *e = exceptions_unexpected_end_of_stream("]", position);
                freeExceptionPosition(position);
                return e;
            }

            if (lexer->token == RBRACKET)
            {
                next_token(lexer);
                Value *result = mkVectorUse((Value **)buffer->buffer, buffer->items_count);
                buffer_free_use(buffer);
                return result;
            }

            Value *next = parse(lexer);
            if (!IS_SUCCESSFUL(next))
            {
                buffer_free(buffer);
                return next;
            }

            buffer_append(buffer, &next, 1);
        }
    }

    case LCURLY:
    {
        Value *result = map_create(0);

        next_token(lexer);

        if (lexer->token == RCURLEY)
        {
            next_token(lexer);
            return result;
        }

        Value *m1_key = parse(lexer);
        if (!IS_SUCCESSFUL(m1_key))
            return m1_key;

        Value *m1_value = parse(lexer);
        if (!IS_SUCCESSFUL(m1_value))
            return m1_value;

        map_set_bang(result, m1_key, m1_value);

        while (1)
        {
            if (lexer->token == EOS)
            {
                struct Exception_Position *position = mkExceptionPosition(lexer);
                Value *e = exceptions_unexpected_end_of_stream("}", position);
                freeExceptionPosition(position);
                return e;
            }

            if (lexer->token == RCURLEY)
            {
                next_token(lexer);
                return result;
            }

            Value *mi_key = parse(lexer);
            if (!IS_SUCCESSFUL(mi_key))
                return mi_key;

            Value *mi_value = parse(lexer);
            if (!IS_SUCCESSFUL(mi_value))
                return mi_value;

            map_set_bang(result, mi_key, mi_value);
        }
    }

    case ERROR_UNENCLOSED_QUOTE:
    {
        struct Exception_Position *position = mkExceptionPosition(lexer);
        Value *e = exceptions_unexpected_end_of_stream("\"", position);
        freeExceptionPosition(position);
        return e;
    }

    case RPAREN:
    case RBRACKET:
    case RCURLEY:
    {
        struct Exception_Position *position = mkExceptionPosition(lexer);
        Value *e = exceptions_unexpected_token(position);
        freeExceptionPosition(position);
        return e;
    }

    default:
    {
        next_token(lexer);
        return VNull;
    }
    }
}

Value *Reader_read(char *source_name, char *content)
{
    struct LexerState lexer = initialise_lexer(source_name, content);

    Value *result = parse(&lexer);

    if (IS_EXCEPTION(result) || lexer.token == EOS)
        return result;
    else
    {
        struct Exception_Position *position = mkExceptionPosition(&lexer);
        Value *e = exceptions_expected_token("EOS", position);
        freeExceptionPosition(position);
        return e;
    }
}

Value *Reader_read_many(char *source_name, char *content)
{
    struct LexerState lexer = initialise_lexer(source_name, content);

    Value *result = VNull;
    Value **cursor = &result;

    while (1)
    {
        if (lexer.token == EOS)
            return result;

        Value *term = parse(&lexer);

        if (IS_EXCEPTION(term))
            return term;

        Value *term_pair = mkPair(term, VNull);
        *cursor = term_pair;
        cursor = &(CDR(term_pair));
    }
}
