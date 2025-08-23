#ifndef PARSER_JSON_H
#define PARSER_JSON_H

typedef u32_t json_token_type_t;
enum
{
    JSON_TOKEN_TYPE_brace_open,
    JSON_TOKEN_TYPE_brace_close,
    JSON_TOKEN_TYPE_bracket_open,
    JSON_TOKEN_TYPE_bracket_close,
    JSON_TOKEN_TYPE_comma,
    JSON_TOKEN_TYPE_colon,
    JSON_TOKEN_TYPE_string,
    JSON_TOKEN_TYPE_number,
    JSON_TOKEN_TYPE_true,
    JSON_TOKEN_TYPE_false,
    JSON_TOKEN_TYPE_null,
    JSON_TOKEN_TYPE_error,
};

typedef struct json_token_t json_token_t;
struct json_token_t
{
    buffer_t          value;
    json_token_type_t type;
};

typedef struct json_entry_t json_entry_t;
struct json_entry_t
{
    buffer_t      label;
    buffer_t      value;
    json_entry_t* next;
    json_entry_t* child;
};

typedef struct json_parser_t json_parser_t;
struct json_parser_t
{
    arena_t* arena;
    buffer_t source;
    u64_t    position;
    b64_t    has_error;
};

internal void          json_parse(const char* file_path, arena_t* arena);
internal json_entry_t* json_parse_entry(json_parser_t* parser, buffer_t* label, json_token_t* token);
internal json_entry_t* json_parse_list(json_parser_t* parser, json_token_type_t end_type, b32_t has_labels);
internal json_token_t  json_parse_token(json_parser_t* parser);

internal b32_t        json_parser_is_valid(json_parser_t* parser);
internal void         json_parser_error(json_parser_t* parser);

internal b32_t        json_is_whitespace(buffer_t* buffer, u64_t pos);
internal b32_t        json_is_number(buffer_t* buffer, u64_t pos);

#endif //PARSER_JSON_H
