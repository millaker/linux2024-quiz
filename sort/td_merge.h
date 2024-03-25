#pragma once

#include "common.h"
#include "list.h"

void td_sort(void *priv, struct list_head *head, list_cmp_func_t cmp);