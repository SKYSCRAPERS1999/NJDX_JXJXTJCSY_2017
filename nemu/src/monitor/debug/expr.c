#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ, 
  TK_PLUS, TK_MINUS, TK_TIME, TK_MULTIPLY, TK_DIVIDE,
  TK_SP_L, TK_SP_R, 
  TK_LESS, TK_MORE, TK_NUM,
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
  {"\\+", TK_PLUS},         // plus
  {"\\-", TK_MINUS},		// minus
  {"\\*", TK_MULTIPLY},	    // time
  {"\\/",  TK_DIVIDE}, 	    // divide
  {"\\(", TK_SP_L},         // small left parenthesis
  {"\\)", TK_SP_R},         // small right parenthesis
  {"[0-9]+", TK_NUM},       // number
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

int eval(uint32_t p, uint32_t q);

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

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  /* TODO: Insert codes to evaluate the expression. */
  int ans = eval(0, nr_token - 1);
  printf("%d\n", ans);	
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

static int num_sign_lhs = 1;

int eval(uint32_t p, uint32_t q)
{
    // printf("Length = %d\n", q - p + 1);
	for (int i = p; i <= q; i++) printf("%d ", tokens[i].type);
	printf("\n");
	if (p > q)
	{
		return 0;
	}
	else if (p == q)
	{
		uint32_t len = strlen(tokens[p].str);
		int num = 0;
		for (uint32_t i = 0; i < len; i++)
		{
			num = 10 * num + (tokens[p].str[i] - '0');
		}
		if (num_sign_lhs == -1)
		{
			num_sign_lhs = 1;
			return -num;
		}else{return num;}
	}
    else if (check_parentheses(p, q) == true)
	{
		if (num_sign_lhs == -1)
		{
			num_sign_lhs = 1;
			return -eval(p + 1, q - 1);
		}else{return eval(p + 1, q - 1);}
	}
	else 
    {
		// calculate op
		int op = p;
		int cnt = 0;
		int first_md = -1, first_pm = -1;
		for (uint32_t i = p; i <= q; i++)
		{
			 if (tokens[i].type == TK_SP_L) { cnt++;}
			 else if (tokens[i].type == TK_SP_R) { cnt--;}
			 else if (cnt == 0)
			 {
				switch (tokens[i].type)
				{
					case TK_PLUS:
					case TK_MINUS:
					{
						if (first_pm != -1) {break;}
						uint32_t j = i;
						while (j > p && (tokens[j].type == TK_PLUS || tokens[j].type == TK_MINUS))
						{
							j--;
						}
						if (j >= p && (tokens[j].type == TK_MULTIPLY || tokens[j].type == TK_DIVIDE))
						{
							break;
						}
						first_pm = i;
						break;
					}
					case TK_MULTIPLY:
					case TK_DIVIDE:
					{
						if (first_md != -1) {break;}
						first_md = i;
						break;
					}
					case TK_NOTYPE:
					case TK_NUM: {break; }
					default: assert(0);
				}
			 }
		}
		if (first_pm != -1) 
		{
			op = first_pm;
		}else if (first_md != -1)
		{
			op = first_md;
		}else{ assert(0);}
        
		printf("op = %d\n", op);
        if (op == p)
		{
			if (tokens[op].type == TK_MINUS)
			{
				num_sign_lhs = -num_sign_lhs;
				return eval(p + 1, q);
			}else if (tokens[op].type == TK_PLUS)
			{
				return eval(p + 1, q);
			}
		}

		int val1 = eval(p, op - 1);
		int val2 = eval(op + 1, q);
        printf("val1 = %d, val2 =  %d\n", val1, val2);
		
		if (num_sign_lhs == -1)
		{
			num_sign_lhs = 1;
			switch (tokens[op].type)
			{
				case TK_PLUS: return -val1 + val2;
				case TK_MINUS: return -val1 - val2;
				case TK_MULTIPLY: return -val1 * val2;
				case TK_DIVIDE: return -val1 / val2;
				default: assert(0);
			}
		}
		else
		{  
			switch (tokens[op].type)
			{
				case TK_PLUS: return val1 + val2;
				case TK_MINUS: return val1 - val2;
				case TK_MULTIPLY: return val1 * val2;
				case TK_DIVIDE: return val1 / val2;
				default: assert(0);
			}
		}
	}
}
