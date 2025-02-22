#include "xlib_helper.h"
#include <cstdio>
#include <cctype>

void upcase_str(char *str)
{
  for(; str; ++str)
  {
    *str = toupper(*str);
  }
}
