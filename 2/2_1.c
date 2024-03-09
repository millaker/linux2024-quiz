#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define container_of(ptr, type, member) \
    ((type *) ((char *) (ptr) - (size_t) & (((type *) 0)->member)))

#define list_entry(ptr, type, member) container_of(ptr, type, member)

#define hlist_for_each(pos, head) \
    for (pos = (head)->first; pos; pos = pos->next)

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

int main()
{
    int pre[5] = {3, 9, 20, 15, 7};
    int in[5] = {9, 3, 15, 20, 7};

    // Test case 1
    struct TreeNode *root = buildTree(pre, 5, in, 5);

    curr = 0;
    preorder(root, pre);
    curr = 0;
    inorder(root, in);

    freeTree(root);

    // Test case 2: same preorder, different inorder
    // inorder: [3,2,1]
    pre[0] = 1;
    pre[1] = 2;
    pre[2] = 3;
    in[0] = 3;
    in[1] = 2;
    in[2] = 1;
    root = buildTree(pre, 3, in, 3);
    curr = 0;
    preorder(root, pre);
    curr = 0;
    inorder(root, in);
    freeTree(root);

    // inorder: [2,1,3]
    in[0] = 2;
    in[1] = 1;
    in[2] = 3;
    root = buildTree(pre, 3, in, 3);
    curr = 0;
    preorder(root, pre);
    curr = 0;
    inorder(root, in);
    freeTree(root);

    // Test case 3: same inorder, different preorder
    //  preorder: [3,1,2]
    in[0] = 2;
    in[1] = 1;
    in[2] = 3;
    pre[0] = 3;
    pre[1] = 1;
    pre[2] = 2;
    root = buildTree(pre, 3, in, 3);
    curr = 0;
    preorder(root, pre);
    curr = 0;
    inorder(root, in);
    freeTree(root);

    // preorder: [1,2,3]
    pre[0] = 1;
    pre[1] = 2;
    pre[2] = 3;
    root = buildTree(pre, 3, in, 3);
    curr = 0;
    preorder(root, pre);
    curr = 0;
    inorder(root, in);
    freeTree(root);

    printf("All test case passed\n");
    return 0;
}