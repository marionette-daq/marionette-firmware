#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define MAX_TOK 4

%%{
  machine fetch_parser;

  action start_cmd {
      if( cmd_count >= MAX_TOK )
      {
         fgoto *fetch_parser_error;
      }
      else
      {
        cmd_tok[cmd_count++] = p;
      }
    }
  action start_data {
      if( data_count >= MAX_TOK )
      {
         fgoto *fetch_parser_error;
      }
      else
      {
        data_tok[data_count++] = p;
      }
    }
  action end_cmd { *p = '\0'; }
  action end_data { *p = '\0'; }
  action goto_error { fgoto *fetch_parser_error; }

  ws = [\t ];
  hex_number = '0x' . xdigit+;
  dec_number = digit+;
  oct_number = '0' . [0-7]+;
  not_null = [^\0];
  not_single = [^\'];
  not_double = [^\"];
  not_escape = [^\\];
  escape_str = '\\' . not_null;

  data_string_double = '\"' . ( escape_str | (not_double & not_escape & not_null) )* . '\"';
  data_string_single = '\'' . ( escape_str | (not_single & not_escape & not_null) )* . '\'';
  data_string = (data_string_double | data_string_single);

  data_const = ( '_' | alpha ) . ( '-' | '_' | '.' | ':' | alnum )*;
  data_number = [\-+]? . ( hex_number | dec_number | oct_number );

  command_name = ((alnum | '_')+) >start_cmd %end_cmd;

  command_list = command_name . ( '.' . command_name )*;

  data_item = (data_string | data_const | data_number) >start_data %end_data;

  data_list = ws* . data_item . ( ws* . ',' . ws* . data_item )* . ws*;

  main := ws* . command_list . ws* . ( '(' . data_list? . ')' )? . ws* . 0? @{ fbreak; };

  write data;
}%%

void fetch_parser(char * input_str, uint32_t max_len)
{
  uint32_t cs;
  char * p = input_str;
  char * pe = input_str + max_len;
  char * eof = pe;

  char * cmd_tok[MAX_TOK];
  char * data_tok[MAX_TOK];
  uint32_t cmd_count = 0;
  uint32_t data_count = 0; 
  
  printf("'%s'\n", p);

  %% write init;
  %% write exec;

  printf("cs = %d, p = %u\n", cs, (p-input_str));

  for( uint32_t i = 0; i < cmd_count; i++ )
  {
    printf("cmd_tok[%d]: '%s'\n", i, cmd_tok[i]); 
  }

  for( uint32_t i = 0; i < data_count; i++ )
  {
    printf("data_tok[%d]: '%s'\n", i, data_tok[i]); 
  }

  if( cs == %%{ write start; }%% )
  {
    printf("Start state\n");
  }
  else if( cs == %%{ write error; }%% )
  {
    printf("Error state\n");
  }
  else if( cs >= %%{ write first_final; }%% )
  {
    printf("Final state\n");
  }
  else
  {
    printf("EOF\n");
  }
}

uint8_t hex_value(char c)
{
  if( c >= '0' && c <= '9' )
  {
    return (c-'0');
  }
  else if( c >= 'a' && c <= 'f' )
  {
    return (10+c-'a');
  }
  else if( c >= 'A' && c <= 'F' )
  {
    return (10+c-'A');
  }
  else
  {
    return 0;
  }
}

%%{
  machine string_parser;

  action oct_0 { value = *p-'0'; } 
  action oct_n { value = (value << 3) | (*p-'0'); }
  action hex_0 { value = hex_value(*p); }
  action hex_1 { value = (value << 4) | hex_value(*p); }
  action write_value { printf("\\x%02x, '%c', %d, %d\n", value, value, len, len+1); *(wp++)=value; len++; }
  action write_char  { printf("\\x%02x, '%c', %d, %d\n", *p, *p, len, len+1); *(wp++)=*p; len++; }
  
  not_null = [^\0];
  not_single = [^\'];
  not_double = [^\"];
  not_escape = [^\\];

  basic_escape = (  'n' >{ value='\n'; } | 
                    'r' >{ value='\r'; } | 
                    't' >{ value='\t'; } | 
                    'f' >{ value='\f'; } | 
                    'v' >{ value='\v'; } |
                    'a' >{ value='\a'; } |
                    'b' >{ value='\b'; } |
                    '\\' >{ value='\\'; } |
                    '\"' >{ value='\"'; } |
                    '\'' >{ value='\''; } );
  
  hex_escape = [xX] . xdigit >hex_0 . xdigit >hex_1;

  oct_escape = [0-3] >oct_0 . [0-7] >oct_n . [0-7] >oct_n;
  
  str_char_single = ( not_escape & not_single & not_null ) >write_char;
  str_char_double = ( not_escape & not_double & not_null ) >write_char;

  str_escape = '\\' . ( basic_escape | hex_escape | oct_escape ) %write_value;

  single_quote = '\'' . ( str_char_single | str_escape )* . '\'';
  double_quote = '\"' . ( str_char_double | str_escape )* . '\"';

  main := ( single_quote | double_quote ) . 0? @{ fbreak; };

  write data;
}%%


void string_parser( const char * input_str, uint32_t max_len, char * output_str, uint32_t * output_len )
{
  uint32_t cs;
  const char * p = input_str;
  const char * pe = input_str + max_len;
  const char * eof = pe;
  char * wp = output_str;

  uint8_t value;
  
  uint32_t len = 0;

  printf("'%s'\n", p);

  %% write init;
  %% write exec;

  *output_len = len;

  printf("cs = %d, p = %u, len = %u\n", cs, (p-input_str), len);

  for( int i = 0; i < len; i++ )
  {
    if( isprint(output_str[i]) )
    {
      printf("%c", output_str[i]);
    }
    else
    {
      printf("\\x%02x", output_str[i]);
    }
  }
  printf("\n");

  if( cs == %%{ write start; }%% )
  {
    printf("Start state\n");
  }
  else if( cs == %%{ write error; }%% )
  {
    printf("Error state\n");
  }
  else if( cs >= %%{ write first_final; }%% )
  {
    printf("Final state\n");
  }
  else
  {
    printf("EOF\n");
  }
}

typedef struct {
  char port;
  uint8_t pin;
} port_pin_t;

#define GPIOA 'a'
#define GPIOB 'b'
#define GPIOC 'c'
#define GPIOD 'd'
#define GPIOE 'e'
#define GPIOF 'f'
#define GPIOG 'g'
#define GPIOH 'h'
#define GPIOI 'i'

%%{
  machine gpio_parser;

  action parse_port { pp->port = tolower(*p); }
  action parse_pin_0 { pp->pin = *p-'0'; }
  action parse_pin_1 { pp->pin = 10 + (*p-'0'); }

  stm_pin             = ('0' . [0-9] >parse_pin_0) | 
                        ('1' . [0-5] >parse_pin_1) | 
                        ([0-9] >parse_pin_0);
  stm_port            = [a-iA-I] >parse_port;
  stm_port_pin        = [pP] . stm_port . stm_pin;

  delim = [\-_.:];

  digital_port_pin    = 'D'i . 'IO'i? . delim? .
                        (
                          '0'? . '0' %{ pp->port = GPIOG; pp->pin = 0; } |
                          '0'? . '1' %{ pp->port = GPIOG; pp->pin = 1; } |
                          '0'? . '2' %{ pp->port = GPIOG; pp->pin = 2; } |
                          '0'? . '3' %{ pp->port = GPIOG; pp->pin = 3; } |
                          '0'? . '4' %{ pp->port = GPIOG; pp->pin = 4; } |
                          '0'? . '5' %{ pp->port = GPIOG; pp->pin = 5; } |
                          '0'? . '6' %{ pp->port = GPIOG; pp->pin = 6; } |
                          '0'? . '7' %{ pp->port = GPIOG; pp->pin = 7; } |
                          '0'? . '8' %{ pp->port = GPIOE; pp->pin = 0; } |
                          '0'? . '9' %{ pp->port = GPIOE; pp->pin = 7; } |
                          '10' %{ pp->port = GPIOE; pp->pin =  8; } |
                          '11' %{ pp->port = GPIOF; pp->pin = 11; } |
                          '12' %{ pp->port = GPIOF; pp->pin = 12; } |
                          '13' %{ pp->port = GPIOF; pp->pin = 13; } |
                          '14' %{ pp->port = GPIOF; pp->pin = 14; } |
                          '15' %{ pp->port = GPIOF; pp->pin = 15; } |
                          '16' %{ pp->port = GPIOH; pp->pin =  2; } |
                          '17' %{ pp->port = GPIOH; pp->pin =  3; } |
                          '18' %{ pp->port = GPIOH; pp->pin =  5; } |
                          '19' %{ pp->port = GPIOH; pp->pin =  6; } |
                          '20' %{ pp->port = GPIOH; pp->pin =  9; } |
                          '21' %{ pp->port = GPIOH; pp->pin = 14; } |
                          '22' %{ pp->port = GPIOB; pp->pin =  8; } |
                          '23' %{ pp->port = GPIOB; pp->pin =  9; } |
                          '24' %{ pp->port = GPIOE; pp->pin =  5; } |
                          '25' %{ pp->port = GPIOE; pp->pin =  6; } |
                          '26' %{ pp->port = GPIOE; pp->pin =  9; } |
                          '27' %{ pp->port = GPIOE; pp->pin = 13; } |
                          '28' %{ pp->port = GPIOH; pp->pin = 10; } |
                          '29' %{ pp->port = GPIOH; pp->pin = 11; } |
                          '30' %{ pp->port = GPIOH; pp->pin = 12; } |
                          '31' %{ pp->port = GPIOA; pp->pin = 15; } |
                          '32' %{ pp->port = GPIOI; pp->pin =  1; } |
                          '33' %{ pp->port = GPIOI; pp->pin =  0; } |
                          '34' %{ pp->port = GPIOI; pp->pin =  2; } |
                          '35' %{ pp->port = GPIOI; pp->pin =  3; } |
                          '36' %{ pp->port = GPIOH; pp->pin = 15; } |
                          '37' %{ pp->port = GPIOD; pp->pin =  7; } |
                          '38' %{ pp->port = GPIOG; pp->pin = 13; } |
                          '39' %{ pp->port = GPIOG; pp->pin =  8; } |
                          '40' %{ pp->port = GPIOG; pp->pin = 12; } |
                          '41' %{ pp->port = GPIOG; pp->pin = 14; } |
                          '42' %{ pp->port = GPIOG; pp->pin =  9; } |
                          '43' %{ pp->port = GPIOG; pp->pin = 11; } |
                          '44' %{ pp->port = GPIOF; pp->pin =  0; } |
                          '45' %{ pp->port = GPIOF; pp->pin =  1; } |
                          '46' %{ pp->port = GPIOE; pp->pin =  1; } |
                          '47' %{ pp->port = GPIOG; pp->pin = 15; } |
                          '48' %{ pp->port = GPIOA; pp->pin =  0; } |
                          '49' %{ pp->port = GPIOA; pp->pin =  1; } |
                          '50' %{ pp->port = GPIOE; pp->pin =  2; } |
                          '51' %{ pp->port = GPIOE; pp->pin =  3; } |
                          '52' %{ pp->port = GPIOD; pp->pin =  8; } |
                          '53' %{ pp->port = GPIOD; pp->pin =  9; } |
                          '54' %{ pp->port = GPIOD; pp->pin = 11; } |
                          '55' %{ pp->port = GPIOD; pp->pin = 12; } |
                          '56' %{ pp->port = GPIOD; pp->pin =  5; } |
                          '57' %{ pp->port = GPIOD; pp->pin =  6; } |
                          '58' %{ pp->port = GPIOD; pp->pin =  3; } |
                          '59' %{ pp->port = GPIOD; pp->pin =  4; } |
                          '60' %{ pp->port = GPIOI; pp->pin =  8; } |
                          '61' %{ pp->port = GPIOI; pp->pin = 11; } |
                          '62' %{ pp->port = GPIOD; pp->pin =  1; } |
                          '63' %{ pp->port = GPIOG; pp->pin = 10; } |
                          '64' %{ pp->port = GPIOB; pp->pin = 15; } |
                          '65' %{ pp->port = GPIOI; pp->pin =  4; } |
                          '66' %{ pp->port = GPIOD; pp->pin =  0; } |
                          '67' %{ pp->port = GPIOC; pp->pin = 13; } |
                          '68' %{ pp->port = GPIOI; pp->pin = 10; } |
                          '69' %{ pp->port = GPIOB; pp->pin = 14; }
                        );
  timer_port_pin      = ( 'T'i | 'TIM'i | 'TMR'i ) . delim? . 
                        (
                          ('0' . delim . '0') %{ pp->port = GPIOB; pp->pin =  8; } |
                          ('0' . delim . '1') %{ pp->port = GPIOB; pp->pin =  9; } |
                          ('1' . delim . '0') %{ pp->port = GPIOE; pp->pin =  5; } |
                          ('1' . delim . '1') %{ pp->port = GPIOE; pp->pin =  6; } |
                          ('2' . delim . '0') %{ pp->port = GPIOE; pp->pin =  9; } |
                          ('2' . delim . '1') %{ pp->port = GPIOE; pp->pin = 13; } |
                          ('3' . delim . '0') %{ pp->port = GPIOH; pp->pin = 10; } |
                          ('3' . delim . '1') %{ pp->port = GPIOH; pp->pin = 11; } |
                          ('3' . delim . '2') %{ pp->port = GPIOH; pp->pin = 12; } |
                          ('4' . delim . '0') %{ pp->port = GPIOA; pp->pin = 15; } 
                        );

  spi_port_pin        = 'S'i . 'PI'i? . delim? .
                        (
                          '0' . delim? . 'SCK'i  %{ pp->port = 'i'; pp->pin =  1; } | 
                          '0' . delim? . 'MOSI'i %{ pp->port = 'i'; pp->pin =  3; } | 
                          '0' . delim? . 'MISO'i %{ pp->port = 'i'; pp->pin =  2; } | 
                          '0' . delim? . 'NSS'i  %{ pp->port = 'i'; pp->pin =  0; } |
                          '1' . delim? . 'SCK'i  %{ pp->port = 'g'; pp->pin = 13; } | 
                          '1' . delim? . 'MOSI'i %{ pp->port = 'g'; pp->pin = 14; } | 
                          '1' . delim? . 'MISO'i %{ pp->port = 'g'; pp->pin = 12; } | 
                          '1' . delim? . 'NSS'i  %{ pp->port = 'g'; pp->pin =  8; }
                        );
  i2c_port_pin        = 'I'i . '2C'i? . delim? . ( '0' . delim? )? .
                        (
                          'SDA'i %{ pp->port = GPIOF; pp->pin =  0; } |
                          'SCL'i %{ pp->port = GPIOF; pp->pin =  1; }
                        );
  uart_port_pin       = 'U'i . 'ART'i? . delim? .
                        (
                          '0' . delim? . 'TX'i  %{ pp->port = GPIOA; pp->pin =  0; } |
                          '0' . delim? . 'RX'i  %{ pp->port = GPIOA; pp->pin =  1; } |
                          '1' . delim? . 'TX'i  %{ pp->port = GPIOD; pp->pin =  8; } |
                          '1' . delim? . 'RX'i  %{ pp->port = GPIOD; pp->pin =  9; } |
                          '1' . delim? . 'CTS'i %{ pp->port = GPIOD; pp->pin = 11; } |
                          '1' . delim? . 'RTS'i %{ pp->port = GPIOD; pp->pin = 12; } |
                          '2' . delim? . 'TX'i  %{ pp->port = GPIOD; pp->pin =  5; } |
                          '2' . delim? . 'RX'i  %{ pp->port = GPIOD; pp->pin =  6; } |
                          '2' . delim? . 'CTS'i %{ pp->port = GPIOD; pp->pin =  3; } |
                          '2' . delim? . 'RTS'i %{ pp->port = GPIOD; pp->pin =  4; }
                        );

  main := ( stm_port_pin | digital_port_pin | timer_port_pin | spi_port_pin | i2c_port_pin | uart_port_pin ) . 0? @{ fbreak; };

  write data;
}%%

void gpio_parser( const char * input_str, uint32_t max_len, port_pin_t * pp )
{
  uint32_t cs;
  const char * p = input_str;
  const char * pe = input_str + max_len;
  const char * eof = pe;

  pp->port = '_';
  pp->pin = 0;

  printf("'%s'\n", p);

  %% write init;
  %% write exec;
  
  printf("cs = %d, p = %u\n", cs, (p-input_str));

  printf("port = %c, pin=%02x\n", pp->port, pp->pin);

  if( cs == %%{ write start; }%% )
  {
    printf("Start state\n");
  }
  else if( cs == %%{ write error; }%% )
  {
    printf("Error state\n");
  }
  else if( cs >= %%{ write first_final; }%% )
  {
    printf("Final state\n");
  }
  else
  {
    printf("EOF\n");
  }
}

int main(int argc, char *argv[])
{
  if( argc < 2 )
  {
    printf("usage: <fsm> <input_str>\n");
    return 1;
  }

  if( strcasecmp("fetch", argv[1]) == 0 )
  {
    printf("==== FETCH PARSER ========\n");

    fetch_parser(argv[2], strlen(argv[2]));
  }
  else if( strcasecmp("string", argv[1]) == 0 )
  {
    printf("==== STRING PARSER =======\n");

    char str_out[32];
    uint32_t str_len = 0;

    string_parser(argv[2], strlen(argv[2]), str_out, &str_len);
  }
  else if( strcasecmp("gpio", argv[1]) == 0 )
  {
    printf("==== GPIO PARSER =========\n");

    port_pin_t pp;

    gpio_parser(argv[2], strlen(argv[2]), &pp);
  }
  else
  {
    printf("unknown fsm, '%s'\n", argv[1]);
    return 1;
  }
  
  return 0;
}

