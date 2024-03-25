#pragma once

#include "common.h"
#include "list.h"

void list_sort(void *priv, struct list_head *head, list_cmp_func_t cmp);