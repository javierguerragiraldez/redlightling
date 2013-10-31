
typedef struct plank_t plank_t;

struct plank_t *plk_create(size_t size);
void *plk_alloc(plank_t* plk, size_t size);