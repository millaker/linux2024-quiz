#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define container_of(ptr, type, member) \
    ((type *) ((char *) (ptr) - (size_t) & (((type *) 0)->member)))

#define list_entry(ptr, type, member) container_of(ptr, type, member)

#define hlist_for_each(pos, head) \
    for (pos = (head)->first; pos; pos = pos->next)

#define hlist_for_each_safe(pos, n, head)        \
    for (pos = (head)->first; pos && ({          \
                                  n = pos->next; \
                                  1;             \
                              });                \
         pos = n)

struct hlist_node {
    struct hlist_node *next, **pprev;
};
struct hlist_head {
    struct hlist_node *first;
};

static inline void INIT_HLIST_HEAD(struct hlist_head *h)
{
    h->first = NULL;
}

static inline void hlist_add_head(struct hlist_node *n, struct hlist_head *h)
{
    if (h->first)
        h->first->pprev = &n->next;
    n->next = h->first;
    n->pprev = &h->first;
    h->first = n;
}

struct TreeNode {
    int val;
    struct TreeNode *left, *right;
};

struct order_node {
    struct hlist_node node;
    int val;
    int idx;
};

static int find(int num, int size, const struct hlist_head *heads)
{
    struct hlist_node *p;
    int hash = (num < 0 ? -num : num) % size;
    hlist_for_each(p, &heads[hash])
    {
        struct order_node *on = list_entry(p, struct order_node, node);
        if (num == on->val)
            return on->idx;
    }
    return -1;
}

static struct TreeNode *dfs(int *preorder,
                            int pre_low,
                            int pre_high,
                            int *inorder,
                            int in_low,
                            int in_high,
                            struct hlist_head *in_heads,
                            int size)
{
    if (in_low > in_high || pre_low > pre_high)
        return NULL;

    struct TreeNode *tn = malloc(sizeof(*tn));
    tn->val = preorder[pre_low];
    int idx = find(preorder[pre_low], size, in_heads);
    tn->left = dfs(preorder, pre_low + 1, pre_low + (idx - in_low), inorder,
                   in_low, idx - 1, in_heads, size);
    tn->right = dfs(preorder, pre_high - (in_high - idx - 1), pre_high, inorder,
                    idx + 1, in_high, in_heads, size);
    return tn;
}

static inline void node_add(int val,
                            int idx,
                            int size,
                            struct hlist_head *heads)
{
    struct order_node *on = malloc(sizeof(*on));
    on->val = val;
    on->idx = idx;
    int hash = (val < 0 ? -val : val) % size;
    hlist_add_head(&on->node, &heads[hash]);
}

static struct TreeNode *buildTree(int *preorder,
                                  int preorderSize,
                                  int *inorder,
                                  int inorderSize)
{
    struct hlist_head *in_heads = malloc(inorderSize * sizeof(*in_heads));
    for (int i = 0; i < inorderSize; i++)
        INIT_HLIST_HEAD(&in_heads[i]);
    for (int i = 0; i < inorderSize; i++)
        node_add(inorder[i], i, inorderSize, in_heads);

    return dfs(preorder, 0, preorderSize - 1, inorder, 0, inorderSize - 1,
               in_heads, inorderSize);
}

static struct TreeNode *buildTree_mod(int *preorder, int *inorder, int size)
{
    struct hlist_head *in_heads = malloc(size * sizeof(*in_heads));
    for (int i = 0; i < size; i++)
        INIT_HLIST_HEAD(&in_heads[i]);
    for (int i = 0; i < size; i++)
        node_add(inorder[i], i, size, in_heads);

    int max_level = 2 * size;
    struct info {
        int pbegin;
        int pend;
        int ibegin;
        int iend;
        struct TreeNode **parent;
    };
    struct info *stk = malloc(sizeof(*stk) * max_level);
    struct TreeNode *res = NULL;

    stk[0].pbegin = 0;
    stk[0].pend = size - 1;
    stk[0].ibegin = 0;
    stk[0].iend = size - 1;
    stk[0].parent = &res;

    int i = 0;

    while (i > 0) {
        // if not legal, return NULL to parent
        if (stk[i].pbegin > stk[i].pend) {
            *(stk[i].parent) = NULL;
            i--;
            continue;
        }
        int val = preorder[stk[i].pbegin];
        int idx = find(val, size, in_heads);
        struct TreeNode *temp = malloc(sizeof(*temp));
        temp->val = val;
        *(stk[i].parent) = temp;
        // push left subtree
        stk[i].pbegin = stk[i].pbegin + 1;
        stk[i].pend = stk[i].pbegin + (idx - stk[i].ibegin);
        stk[i].iend = idx - 1;
        stk[i].parent = &temp->left;

        // push right subtree
        stk[i + 1].pbegin = stk[i].pend - (stk[i].iend - idx - 1);
        stk[i + 1].pend = stk[i].pend;
        stk[i + 1].ibegin = idx + 1;
        stk[i + 1].iend = stk[i].iend;
        stk[i + 1].parent = &temp->right;
        i += 1;
    }

    free(stk);
    for (int i = 0; i < size; i++) {
        struct hlist_node *curr, *n;
        hlist_for_each_safe(curr, n, &in_heads[i])
        {
            struct order_node *entry =
                list_entry(curr, struct order_node, node);
            free(entry);
        }
    }
    free(in_heads);
    return res;
}

static int curr = 0;
void inorder(struct TreeNode *root, int *ans)
{
    if (!root)
        return;
    inorder(root->left, ans);
    assert(ans[curr++] == root->val);
    inorder(root->right, ans);
}

void preorder(struct TreeNode *root, int *ans)
{
    if (!root)
        return;
    assert(ans[curr++] == root->val);
    preorder(root->left, ans);
    preorder(root->right, ans);
}

void freeTree(struct TreeNode *root)
{
    if (!root)
        return;
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

void verify(struct TreeNode *root, int *pre, int *in)
{
    curr = 0;
    preorder(root, pre);
    curr = 0;
    inorder(root, in);
    freeTree(root);
}

int main()
{
    int pre[5] = {3, 9, 20, 15, 7};
    int in[5] = {9, 3, 15, 20, 7};

    // Test case 1
    // struct TreeNode *root = buildTree(pre, 5, in, 5);
    struct TreeNode *root = buildTree_mod(pre, in, 5);
    verify(root, pre, in);

    // Test case 2: same preorder, different inorder
    // inorder: [3,2,1]
    pre[0] = 1;
    pre[1] = 2;
    pre[2] = 3;
    in[0] = 3;
    in[1] = 2;
    in[2] = 1;

    // root = buildTree(pre, 3, in, 3);
    root = buildTree_mod(pre, in, 3);
    verify(root, pre, in);


    // inorder: [2,1,3]
    in[0] = 2;
    in[1] = 1;
    in[2] = 3;
    // root = buildTree(pre, 3, in, 3);
    root = buildTree_mod(pre, in, 3);
    verify(root, pre, in);

    // Test case 3: same inorder, different preorder
    //  preorder: [3,1,2]
    in[0] = 2;
    in[1] = 1;
    in[2] = 3;
    pre[0] = 3;
    pre[1] = 1;
    pre[2] = 2;
    // root = buildTree(pre, 3, in, 3);
    root = buildTree_mod(pre, in, 3);
    verify(root, pre, in);

    // preorder: [1,2,3]
    pre[0] = 1;
    pre[1] = 2;
    pre[2] = 3;
    // root = buildTree(pre, 3, in, 3);
    root = buildTree_mod(pre, in, 3);
    verify(root, pre, in);

    printf("All test case passed\n");
    return 0;
}