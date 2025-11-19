#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define DIE(assertion, call_description)                       \
    do                                                         \
    {                                                          \
        if (assertion)                                         \
        {                                                      \
            fprintf(stderr, "(%s, %d): ", __FILE__, __LINE__); \
            perror(call_description);                          \
            exit(errno);                                       \
        }                                                      \
    } while (0)

#define MAX 256

typedef struct heap
{
    struct satelit *vector;
    int size;
    int capacity;
} heap;

typedef struct satelit
{
    int frq;
    char name[MAX];
    struct satelit *left, *right;
} satelit;

#define GO_UP(x) (((x) - 1) >> 1)
#define GO_LEFT(x) (((x) << 1) + 1)
#define GO_RIGHT(x) (((x) << 1) + 2)

/* functii copiate din laborator*/
typedef struct queue_t queue_t;
struct queue_t
{
    /* Dimensiunea maxima a cozii */
    unsigned int max_size;
    /* Dimensiunea cozii */
    unsigned int size;
    /* Dimensiunea in octeti a tipului de data stocat in coada */
    unsigned int data_size;
    /* Indexul de la care se vor efectua operatiile de front si dequeue */
    unsigned int read_idx;
    /* Indexul de la care se vor efectua operatiile de enqueue */
    unsigned int write_idx;
    /* Bufferul ce stocheaza elementele cozii */
    void **buff;
};

queue_t *
q_create(unsigned int data_size, unsigned int max_size)
{
    queue_t *q = calloc(1, sizeof(*q));
    DIE(!q, "calloc queue failed");

    q->data_size = data_size;
    q->max_size = max_size;

    q->buff = malloc(max_size * sizeof(*q->buff));
    DIE(!q->buff, "malloc buffer failed");

    return q;
}

unsigned int
q_get_size(queue_t *q)
{
    return !q ? 0 : q->size;
}

unsigned int
q_is_empty(queue_t *q)
{
    return !q ? 1 : !q->size;
}

void *
q_front(queue_t *q)
{
    if (!q || !q->size)
        return NULL;

    return q->buff[q->read_idx];
}

int q_dequeue(queue_t *q)
{
    if (!q || !q->size)
        return 0;

    free(q->buff[q->read_idx]);

    q->read_idx = (q->read_idx + 1) % q->max_size;
    --q->size;
    return 1;
}

int q_enqueue(queue_t *q, void *new_data)
{
    void *data;
    if (!q || q->size == q->max_size)
        return 0;

    data = malloc(q->data_size);
    DIE(!data, "malloc data failed");
    memcpy(data, new_data, q->data_size);

    q->buff[q->write_idx] = data;
    q->write_idx = (q->write_idx + 1) % q->max_size;
    ++q->size;

    return 1;
}

void q_clear(queue_t *q)
{
    unsigned int i;
    if (!q || !q->size)
        return;

    for (i = q->read_idx; i != q->write_idx; i = (i + 1) % q->max_size)
    {
        free(q->buff[i]);
    }

    q->read_idx = 0;
    q->write_idx = 0;
    q->size = 0;
}

void q_free(queue_t *q)
{
    if (!q)
        return;

    q_clear(q);
    free(q->buff);
    free(q);
}

/* functii copiate din laborator*/

void heap_insert(heap *heap_sateliti, satelit *add)
{
    if (heap_sateliti->size >= heap_sateliti->capacity)
    {
        heap_sateliti->capacity *= 2;
        heap_sateliti->vector = realloc(heap_sateliti->vector, heap_sateliti->capacity * sizeof(satelit));
        DIE(!heap_sateliti->vector, "eroare reallocare memorie");
    }
    heap_sateliti->vector[heap_sateliti->size] = *add;
    free(add);
    int index = heap_sateliti->size;
    heap_sateliti->size++;

    while (index > 0 && heap_sateliti->vector[GO_UP(index)].frq > heap_sateliti->vector[index].frq)
    {
        satelit temp = heap_sateliti->vector[index];
        heap_sateliti->vector[index] = heap_sateliti->vector[GO_UP(index)];
        heap_sateliti->vector[GO_UP(index)] = temp;
        index = GO_UP(index);
    }
}

struct satelit *pop_min_lexicografic(heap *heap_sateliti)
{
    if (heap_sateliti->size == 0)
    {
        return NULL;
    }

    // caut frecventa minima
    int min_frq = heap_sateliti->vector[0].frq;
    for (int i = 1; i < heap_sateliti->size; i++)
    {
        if (heap_sateliti->vector[i].frq < min_frq)
        {
            min_frq = heap_sateliti->vector[i].frq;
        }
    }

    int min_index = -1; // caut nodul cu frecventa minima si nume lexicografic minim
    for (int i = 0; i < heap_sateliti->size; i++)
    {
        if (heap_sateliti->vector[i].frq == min_frq)
        {
            if (min_index == -1 || strcmp(heap_sateliti->vector[i].name, heap_sateliti->vector[min_index].name) < 0)
            {
                min_index = i;
            }
        }
    }

    satelit *min = malloc(sizeof(satelit));
    DIE(!min, "eroare alocare memorie");
    *min = heap_sateliti->vector[min_index];
    heap_sateliti->vector[min_index] = heap_sateliti->vector[heap_sateliti->size - 1];
    heap_sateliti->size--;

    int index = min_index;
    while (1)
    {
        int left = GO_LEFT(index);
        int right = GO_RIGHT(index);
        int smallest = index;
        if (left < heap_sateliti->size && heap_sateliti->vector[left].frq < heap_sateliti->vector[smallest].frq)
        {
            smallest = left;
        }
        if (right < heap_sateliti->size && heap_sateliti->vector[right].frq < heap_sateliti->vector[smallest].frq)
        {
            smallest = right;
        }
        if (smallest != index)
        {
            satelit aux = heap_sateliti->vector[index];
            heap_sateliti->vector[index] = heap_sateliti->vector[smallest];
            heap_sateliti->vector[smallest] = aux;
            index = smallest;
        }
        else
        {
            break;
        }
    }
    return min;
}

// laboratorul 6 binary tree, modificat pt heapul meu
void print_level(int n, struct satelit *root, FILE *out)
{
    queue_t *q;
    struct satelit *node;
    int i, level = 0;

    q = q_create(sizeof(struct satelit *), MAX);

    q_enqueue(q, &root);

    while (!q_is_empty(q))
    {
        int s = q_get_size(q);
        for (i = 0; i < s; i++)
        {
            node = *(struct satelit **)q_front(q);
            q_dequeue(q);

            if (level == n)
                fprintf(out, "%d-"
                             "%s",
                        node->frq, node->name);

            if (node->left)
                q_enqueue(q, &node->left);
            if (node->right)
                q_enqueue(q, &node->right);
        }
        if (level == n)
            break;
        level++;
    }

    q_free(q);
}

int find_lvl(struct satelit *root)
{
    if (root == NULL)
    {
        return 0;
    }
    int lvl = 0;
    int left = find_lvl(root->left);
    int right = find_lvl(root->right);
    if (left > right)
    {
        lvl = left + 1;
    }
    else
    {
        lvl = right + 1;
    }
    return lvl;
}

struct satelit *build_tree(FILE *in)
{

    int n;
    fscanf(in, "%d", &n);

    heap *h = malloc(sizeof(heap));
    DIE(!h, "eroare alocare memorie");
    h->vector = malloc(n * sizeof(satelit));
    h->size = n;
    h->capacity = n;

    for (int i = 0; i < n; i++)
    {
        fscanf(in, "%d %s", &h->vector[i].frq, h->vector[i].name);
        h->vector[i].left = NULL;
        h->vector[i].right = NULL;
    }

    while (h->size > 1)
    {

        struct satelit *min1 = pop_min_lexicografic(h);
        struct satelit *min2 = pop_min_lexicografic(h);

        struct satelit *newNode = malloc(sizeof(satelit));
        DIE(!newNode, "eroare alocare memorie");
        newNode->frq = min1->frq + min2->frq;
        newNode->left = min1;
        newNode->right = min2;
        strcpy(newNode->name, min1->name);
        strcat(newNode->name, min2->name);
        heap_insert(h, newNode);
    }

    struct satelit *root = pop_min_lexicografic(h);
    free(h->vector);
    free(h);
    return root;
}

void free_tree(struct satelit *root)
{
    if (root == NULL)
    {
        return;
    }
    free_tree(root->left);
    free_tree(root->right);
    free(root);
}

int find_node(struct satelit *root, char *name, char *codificare, int lvl)
{
    if (root == NULL)
        return 0;

    if (strcmp(root->name, name) == 0)
    {
        codificare[lvl] = '\0';
        return 1;
    }

    if (root->left != NULL)
    {
        codificare[lvl] = '0';
        if (find_node(root->left, name, codificare, lvl + 1))
            return 1;
    }

    if (root->right != NULL)
    {
        codificare[lvl] = '1';
        if (find_node(root->right, name, codificare, lvl + 1))
            return 1;
    }

    return 0;
}

int all_names_found(struct satelit *root, char lista_sateliti[MAX][MAX], int nr)
{
    if (root == NULL)
        return 0;
    for (int i = 0; i < nr; i++)
    {
        if (strstr(root->name, lista_sateliti[i]) == NULL)
        {

            return 0;
        }
    }
    return 1;
}

void cerinta1(FILE *in, FILE *out)
{

    struct satelit *root = build_tree(in);

    int lvl;
    lvl = find_lvl(root);
    for (int i = 0; i < lvl; i++)
    {
        print_level(i, root, out);
        fprintf(out, "\n");
    }

    free_tree(root);
}

void cerinta2(FILE *in, FILE *out)
{

    struct satelit *root = build_tree(in);

    int nr;
    fscanf(in, "%d", &nr);
    char coduri[MAX][MAX];
    for (int i = 0; i < nr; i++)
    {
        fscanf(in, "%s", coduri[i]);
    }
    for (int i = 0; i < nr; i++)
    {
        struct satelit *start = root;
        for (int j = 0; j < (int)strlen(coduri[i]); j++)
        {
            if (coduri[i][j] == '0')
            {
                start = start->left;
            }
            else
            {
                start = start->right;
            }
            if (start->left == NULL && start->right == NULL)
            {
                fprintf(out, "%s", start->name);
                start = root;
            }
        }
        fprintf(out, "\n");
    }

    free_tree(root);
}

void cerinta3(FILE *in, FILE *out)
{
    struct satelit *root = build_tree(in);
    int nr;
    fscanf(in, "%d", &nr);
    char cod[MAX] = "";
    char lista_sateliti[MAX][MAX];
    for (int i = 0; i < nr; i++)
    {
        fscanf(in, "%s", lista_sateliti[i]);
        int lvl = 0;
        char codificare[MAX];
        find_node(root, lista_sateliti[i], codificare, lvl);
        strcat(cod, codificare);
    }

    fprintf(out, "%s\n", cod);

    free_tree(root);
}

void cerinta4(FILE *in, FILE *out)
{

    struct satelit *root = build_tree(in);
    int nr;
    fscanf(in, "%d", &nr);
    char lista_sateliti[MAX][MAX];
    for (int i = 0; i < nr; i++)
    {
        fscanf(in, "%s", lista_sateliti[i]);
    }

    while (1)
    {

        if (all_names_found(root->left, lista_sateliti, nr) == 1)
        {
            root = root->left;
        }
        else if (all_names_found(root->right, lista_sateliti, nr) == 1)
        {
            root = root->right;
        }
        else
        {
            fprintf(out, "%s\n", root->name);
            break;
        }
    }

    free_tree(root);
}

typedef struct sateliti_1
{
    char name[MAX];
    int frq;
    int nr_sons;
    struct sateliti_1 **sons;
} sateliti_1;

sateliti_1 *convert_to_graph(struct satelit *root)
{
    if (!root)
        return NULL;
    sateliti_1 *node = malloc(sizeof(sateliti_1));
    DIE(!node, "eroare alocare memorie");
    strcpy(node->name, root->name);
    node->frq = root->frq;
    node->nr_sons = 0;
    node->sons = NULL;

    int cnt = 0;
    if (root->left)
        cnt++;
    if (root->right)
        cnt++;
    if (cnt > 0)
    {
        node->sons = malloc(cnt * sizeof(sateliti_1 *));
        DIE(!node->sons, "eroare alocare memorie");
        int idx = 0;
        if (root->left)
            node->sons[idx++] = convert_to_graph(root->left);
        if (root->right)
            node->sons[idx++] = convert_to_graph(root->right);
        node->nr_sons = cnt;
    }
    return node;
}

void free_graph(sateliti_1 *node)
{
    if (!node)
        return;
    for (int i = 0; i < node->nr_sons; i++)
    {
        free_graph(node->sons[i]);
    }
    free(node->sons);
    free(node);
}

sateliti_1 *find_in_graph(sateliti_1 *root, char *name)
{
    if (!root)
        return NULL;
    if (strcmp(root->name, name) == 0)
        return root;
    for (int i = 0; i < root->nr_sons; i++)
    {
        sateliti_1 *found = find_in_graph(root->sons[i], name);
        if (found)
            return found;
    }
    return NULL;
}

sateliti_1 *merge(FILE *in)
{

    sateliti_1 *root = malloc(sizeof(sateliti_1));
    DIE(!root, "eroare alocare memorie");
    fscanf(in, "%d %s", &root->frq, root->name);
    root->nr_sons = 0;
    root->sons = NULL;

    int nr_parinti;
    fscanf(in, "%d", &nr_parinti);

    for (int i = 0; i < nr_parinti; i++)
    {
        char parinte[MAX];
        fscanf(in, "%s", parinte);

        int nr_copii;
        fscanf(in, "%d", &nr_copii);

        sateliti_1 *parinte_node = find_in_graph(root, parinte);
        parinte_node->sons = malloc(nr_copii * sizeof(sateliti_1 *));
        parinte_node->nr_sons = nr_copii;

        for (int j = 0; j < nr_copii; j++)
        {

            sateliti_1 *copil = malloc(sizeof(sateliti_1));
            DIE(!copil, "eroare alocare memorie");
            fscanf(in, "%d %s", &copil->frq, copil->name);
            copil->nr_sons = 0;
            copil->sons = NULL;
            parinte_node->sons[j] = copil;
        }
    }
    return root;
}

int get_path(sateliti_1 *root, char *name, sateliti_1 **path, int lvl)
{

    if (path)
        path[lvl] = root;

    if (strcmp(root->name, name) == 0)
        return lvl;

    for (int i = 0; i < root->nr_sons; i++)
    {
        int found = get_path(root->sons[i], name, path, lvl + 1);
        if (found != -1) // am gasit nodul
            return found;
    }

    return -1; // nodul nu a fost gasit
}

int distance_between_nodes(sateliti_1 *root, char *name1, char *name2)
{
    sateliti_1 *path1[MAX], *path2[MAX];
    int len1 = get_path(root, name1, path1, 0);
    int len2 = get_path(root, name2, path2, 0);

    int path_comun = 0;
    while (path_comun < len1 && path_comun < len2 && path1[path_comun] == path2[path_comun])
        path_comun++;
    int dist = len1 + len2 - 2 * path_comun + 2; // distnata dintre noduri si radacina -2*partile comune + nodurile efective
    return dist;
}

void cerinta5(FILE *in, FILE *out)
{

    struct satelit *root = build_tree(in);
    sateliti_1 *graf = convert_to_graph(root);

    int nr_arbore;
    fscanf(in, "%d", &nr_arbore);

    for (int i = 0; i < nr_arbore; i++)
    {
        char nod_principal[MAX];
        fscanf(in, "%s", nod_principal);

        sateliti_1 *parinte = find_in_graph(graf, nod_principal);
        sateliti_1 *radacina_graph = merge(in);

        int new_nr_sons = parinte->nr_sons + 1;
        parinte->sons = realloc(parinte->sons, new_nr_sons * sizeof(sateliti_1 *));
        parinte->sons[parinte->nr_sons] = radacina_graph;
        parinte->nr_sons = new_nr_sons;
    }

    char nod1[MAX], nod2[MAX];
    fscanf(in, "%s %s", nod1, nod2);
    int dist = distance_between_nodes(graf, nod1, nod2);
    fprintf(out, "%d\n", dist);

    free_graph(graf);
    free_tree(root);
}

int main(int argc, char *argv[])
{

    DIE(argc != 4, "eroare numar argumente");
    char *option = argv[1];
    FILE *in = fopen(argv[2], "r");
    DIE(!in, "eroare deschidere fisier de intrare");

    FILE *out = fopen(argv[3], "w");
    DIE(!out, "eroare deschidere fisier de iesire");

    if (strcmp(option, "-c1") == 0)
    {
        cerinta1(in, out);
    }
    else if (strcmp(option, "-c2") == 0)
    {
        cerinta2(in, out);
    }
    else if (strcmp(option, "-c3") == 0)
    {
        cerinta3(in, out);
    }
    else if (strcmp(option, "-c5") == 0)
    {
        cerinta5(in, out);
    }
    else if (strcmp(option, "-c4") == 0)
    {
        cerinta4(in, out);
    }

    fclose(in);
    fclose(out);
    return 0;
}
