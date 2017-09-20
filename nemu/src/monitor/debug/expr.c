#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256,
  TK_EQ, TK_UEQ, TK_LESS, TK_MORE, TK_LE, TK_ME, 
  TK_PLUS, TK_MINUS, TK_MULTIPLY, TK_DIVIDE,
  TK_SP_L, TK_SP_R, 
  TK_NUM, TK_HEXNUM,
  TK_AND, TK_OR, TK_NOT,
  TK_EAX, TK_EBX, TK_ECX, TK_EDX, TK_ESP, TK_ESI, TK_EBP, TK_EDI,
  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},		// spaces
  {"==", TK_EQ},			// equal
  {"!=", TK_UEQ},           // unequal
  {">=", TK_ME},            // more than or equal
  {"<=", TK_LE},            // less than or equal
  {">", TK_MORE},           // more than
  {"<", TK_LESS},           // less than
  {"\\+", TK_PLUS},         // plus
  {"\\-", TK_MINUS},		// minus
  {"\\*", TK_MULTIPLY},	    // time
  {"\\/",  TK_DIVIDE}, 	    // divide
  {"\\(", TK_SP_L},         // small left parenthesis
  {"\\)", TK_SP_R},         // small right parenthesis
  {"0x[0-9|a-f|A-F]+", TK_HEXNUM},      // hex number
  {"[0-9]+", TK_NUM},       // number
  {"&&", TK_AND},           // logical and
  {"\\|\\|", TK_OR},            // logical or
  {"!", TK_NOT},            // logical not
  {"\\$[e|E][a|A][x|X]", TK_EAX}, // read eax
  {"\\$[e|E][b|B][x|X]", TK_EBX}, // read ebx
  {"\\$[e|E][c|C][x|X]", TK_ECX}, // read ecx
  {"\\$[e|E][d|D][x|X]", TK_EDX}, // read edx
  {"\\$[e|E][s|S][p|P]", TK_ESP}, // read esp
  {"\\$[e|E][s|S][i|I]", TK_ESI}, // read esi
  {"\\$[e|E][b|B][p|P]", TK_EBP}, // read ebp
  {"\\$[e|E][d|D][i|I]", TK_EDI}, // read edi

};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

uint32_t eval(uint32_t p, uint32_t q);

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;
  
  nr_token = 0;
  
  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;
				
        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        
        switch (rules[i].token_type) {
				case TK_NOTYPE:
				{
					break;
				}
				case TK_NUM: 
				{	
					tokens[nr_token].type = rules[i].token_type;
				    for (uint32_t j = 0; j < 32; j++)
					{
						tokens[nr_token].str[j] = '\0';
					}
				    for (uint32_t j = 0; j < substr_len && j < 32; j++)
					{
						tokens[nr_token].str[j] = *(substr_start + j);
					}
					break;
				}
				case TK_HEXNUM:
				{
					tokens[nr_token].type = rules[i].token_type;
				    for (uint32_t j = 0; j < 32; j++)
					{
						tokens[nr_token].str[j] = '\0';
					}
				    for (uint32_t j = 0; j < substr_len - 2 && j < 32; j++)
					{
						tokens[nr_token].str[j] = *(substr_start + j + 2);
					}
					break;
				}
				default: 
				{
					tokens[nr_token].type = rules[i].token_type;  
				}
        }
		if (rules[i].token_type != TK_NOTYPE) {nr_token++; }
		break;
      }
    }
    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

uint32_t expr(char *e, bool *success, char type) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  /* TODO: Insert codes to evaluate the expression. */
  int ans = eval(0, nr_token - 1);
  switch (type)
  {
	case 'd': { printf("%d\n", ans); break;}
	case 'h':
	case 'x': { printf("0x%x\n", ans); break;}
  }
  *success = true;

  return 0;
}


bool check_parentheses (uint32_t p, uint32_t q)
{
	if (p > q) return 0;
	bool ok = (tokens[p].type == TK_SP_L && tokens[q].type == TK_SP_R);
    if (ok == 0) return 0;
	int cnt = 0;
	bool flag = 1;
    for (uint32_t i = p + 1; i <= q - 1; i++)
	{
		if (tokens[i].type == TK_SP_L) cnt++;
		if (tokens[i].type == TK_SP_R) cnt--;
		if (cnt < 0) 
		{
			flag = 0;
			break;
		}
	}
	return flag;
}

uint32_t eval(uint32_t p, uint32_t q)
{
    printf("p = %d, q = %d\n", p , q);
	for (int i = p; i <= q; i++) printf("%d ", tokens[i].type);
	printf("\n");
	if (p > q)
	{
		return 0;
	}
	else if (p == q)
	{
		switch (tokens[p].type)
		{
			case TK_EAX: return reg_l(0); 
			case TK_ECX: return reg_l(1);
			case TK_EDX: return reg_l(2);
			case TK_EBX: return reg_l(3);
			case TK_ESP: return reg_l(4);
			case TK_EBP: return reg_l(5);
			case TK_ESI: return reg_l(6);
			case TK_EDI: return reg_l(7);
			case TK_HEXNUM:
			{
				uint32_t len = strlen(tokens[p].str);
				uint32_t num = 0;
				for (uint32_t i = 0; i < len; i++)
				{
					num = 16 * num + (tokens[p].str[i] - '0');
				}
				return num;
			}
			case TK_NUM:
			{
				uint32_t len = strlen(tokens[p].str);
				uint32_t num = 0;
				for (uint32_t i = 0; i < len; i++)
				{
					num = 10 * num + (tokens[p].str[i] - '0');
				}
				return num;
			}
			default: assert(0);	
		}
	}
    else if (check_parentheses(p, q) == true)
	{
		return eval(p + 1, q - 1);
	}
	else 
    {
		// calculate op
		int op = p;
		int cnt = 0;
		int last_md = -1, last_pm = -1, last_cmp = -1, last_ao = -1, first_single = -1;
		for (uint32_t i = p; i <= q; i++)
		{
			 if (tokens[i].type == TK_SP_L) { cnt++;}
			 else if (tokens[i].type == TK_SP_R) { cnt--;}
			 else if (cnt == 0)
			 {
				switch (tokens[i].type)
				{
					case TK_AND:
					case TK_OR:
					{
						last_ao = i;
						break;
					}
					case TK_NOT:
					{
						if (first_single == -1 && i == p)
						{
							first_single = i;
						}
						break;
					}
					case TK_LESS:
					case TK_MORE:
					case TK_LE:
					case TK_ME:
					case TK_UEQ:
					case TK_EQ:
					{
						last_cmp = i;
						break;
					}
					case TK_PLUS:
					case TK_MINUS:
					{   
						if (first_single == -1 && i == p)
						{
							first_single = i;
						}else{
							if (i > p && !(tokens[i - 1].type == TK_MINUS || tokens[i - 1].type == TK_PLUS || tokens[i - 1].type == TK_NOT || tokens[i - 1].type == TK_MULTIPLY ))
							{
								last_pm = i;
							}
						}
						break;
					}
					case TK_MULTIPLY:
					case TK_DIVIDE:
					{
						if (first_single == -1 && i == p)
						{
							first_single = i;
						}else{
							if (i > p && !(tokens[i - 1].type == TK_MINUS || tokens[i - 1].type == TK_PLUS || tokens[i - 1].type == TK_NOT || tokens[i - 1].type == TK_MULTIPLY ))
							{
								last_md = i;
							}
						}
						break;
					}
					case TK_NOTYPE:
					case TK_EAX: 
					case TK_ECX:
					case TK_EDX:
					case TK_EBX:
					case TK_ESP: 
					case TK_EBP: 
					case TK_ESI: 
					case TK_EDI:
					case TK_HEXNUM:
					case TK_NUM: {break; }
					default: assert(0);
				}
			 }
		}

		printf("last_ao = %d # last_pm = %d # last_md = %d # first_single = %d\n", last_ao, last_pm, last_md , first_single);
		//for (int i = 0; i < 8; i++)
		//		printf("reg_l(%d) = %d\n", i, reg_l(i));
		// the priority are listed from lowest to highest
		//
		if (last_ao != -1)
		{
			op = last_ao;
		}else if (last_cmp != -1)
		{
			op = last_cmp;
		}else if (last_pm != -1) 
		{
			op = last_pm;
		}else if (last_md != -1)
		{
			op = last_md;
		}else if (first_single != -1 && first_single == p)
		{
			switch (tokens[first_single].type)
			{
				case TK_PLUS: return eval(p + 1, q);
				case TK_MINUS: return -eval(p + 1, q);
				case TK_NOT: return (!(eval(p + 1, q)));
				case TK_MULTIPLY: return vaddr_read(eval(p + 1, q), 4);
			}
		}else{ assert(0);}
        
		//printf("op = %d\n", op);

		uint32_t val1 = eval(p, op - 1);
		uint32_t val2 = eval(op + 1, q);
        printf("val1 = %d, val2 =  %d\n", val1, val2);
		switch (tokens[op].type)
		{
			case TK_AND: return val1 && val2;
			case TK_OR: return val1 || val2;
			case TK_PLUS: return val1 + val2;
			case TK_MINUS: return val1 - val2;
			case TK_MULTIPLY: return val1 * val2;
			case TK_DIVIDE: return val1 / val2;
			case TK_EQ: return val1 == val2;
			case TK_UEQ: return val1 != val2;
			case TK_MORE: return val1 > val2;
			case TK_LESS: return val1 < val2;
			case TK_ME: return val1 >= val2;
			case TK_LE: return val1 <= val2;
			default: assert(0);
		}
	}
}
