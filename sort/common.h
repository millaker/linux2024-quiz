#pragma once
#include "list.h"
typedef int (*list_cmp_func_t)(void *,
                               const struct list_head *,
                               const struct list_head *);
