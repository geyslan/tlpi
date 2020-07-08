#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <unistd.h>

extern char end;

typedef struct mchunk {
	size_t size;
	struct mchunk *prev;
	struct mchunk *next;
} mchunk;

mchunk *free_chunk_head;
size_t page_size;

static inline void *chunk_header_addr(mchunk *chunk);
static inline void *chunk_data_addr(mchunk *chunk);
static inline void *chunk_end_addr(mchunk *chunk);

void chunk_use(mchunk *chunk);
void chunk_split(mchunk *chunk, size_t size_need, size_t size_avail);
void chunk_add(mchunk *freed);

static inline void *chunk_header_addr(mchunk *chunk)
{
	if (!chunk)
		return NULL;
	return (char *) chunk - sizeof(*chunk);
}

static inline void *chunk_data_addr(mchunk *chunk)
{
	if (!chunk)
		return NULL;
	return (char *) chunk + sizeof(*chunk);
}

static inline void *chunk_end_addr(mchunk *chunk)
{
	if (!chunk)
		return NULL;
	return (char *) chunk_data_addr(chunk) + chunk->size;
}

void *my_malloc(size_t size)
{
	void *pb;
	mchunk *fc;
	mchunk *new_chunk;
	size_t chunk_size;

	if (!size)
		return NULL;
	if (!page_size)
		page_size = sysconf(_SC_PAGESIZE);

	if (size > (SIZE_MAX - sizeof(mchunk))) {
		fprintf(stderr, "my_malloc: size_t overflow");
		exit(EXIT_FAILURE);
	}

	fc = free_chunk_head;
	while (fc) {
		if (fc->size == size) {
			chunk_use(fc);
			return chunk_data_addr(fc);
		}
		if (fc->size > size) {
			chunk_split(fc, size, fc->size);
			chunk_use(fc);
			return chunk_data_addr(fc);
		}
		fc = fc->next;
	}

	chunk_size = page_size;
	if (size + sizeof(mchunk) > page_size)
		chunk_size = size + sizeof(mchunk);

	new_chunk = sbrk(chunk_size);
	pb	  = sbrk(0);
	if (new_chunk == (void *) -1) {
		fprintf(stderr, "sbrk");
		return NULL;
	}
	new_chunk->size = (char *) pb - (char *) new_chunk;
	chunk_split(new_chunk, size, new_chunk->size);

	return chunk_data_addr(new_chunk);
}

void chunk_use(mchunk *chunk)
{
	// remove chunk from the free chunk list

	if (chunk == free_chunk_head) {
		if (!chunk->next) {
			free_chunk_head = NULL;
			return;
		}
		free_chunk_head	      = chunk->next;
		free_chunk_head->prev = NULL;
		return;
	}
	chunk->prev->next = chunk->next;
	if (chunk->next)
		chunk->next->prev = chunk->prev;
}

void chunk_split(mchunk *chunk, size_t size_need, size_t size_avail)
{
	mchunk *new_chunk;
	size_t remain = size_avail - size_need;

	// only split if chunk is large enough for another chunk header plus 1 byte
	if (remain <= sizeof(*chunk))
		return;
	chunk->size	= size_need;
	new_chunk	= chunk_end_addr(chunk);
	new_chunk->size = remain - sizeof(*chunk);

	chunk_add(new_chunk);
}

void chunk_add(mchunk *freed)
{
	if (!free_chunk_head) {
		freed->prev	= NULL;
		freed->next	= NULL;
		free_chunk_head = freed;
		return;
	}

	// put on the head
	freed->next	      = free_chunk_head;
	freed->prev	      = NULL;
	free_chunk_head->prev = freed;
	free_chunk_head	      = freed;
}

void my_free(void *ptr)
{
	if (!ptr)
		return;

	// access the header
	chunk_add(chunk_header_addr((mchunk *) ptr));
}

void my_heapstat()
{
	mchunk *chunk = free_chunk_head;
	int counter   = 0;

	printf("----------------------------------------\n");
	printf("program break:  %p\n", sbrk(0));
	while (chunk) {
		printf("(%d) FREE CHUNK\n", ++counter);
		printf(" chunk address: %p\n", (void *) chunk);
		printf(" chunk size:    %zu\n", chunk->size);
		printf("  chunk prev:   %p\n", (void *) chunk->prev);
		printf("  chunk next:   %p\n", (void *) chunk->next);
		printf("\n");
		chunk = chunk->next;
	}
	printf("\n");
}

int main(void)
{
	void *p1, *p2, *p3, *p4, *p5, *p6;

	my_heapstat();

	p1 = my_malloc(10);
	my_heapstat();

	p2 = my_malloc(10);
	my_heapstat();

	my_free(p2);
	//my_heapstat();

	p3 = my_malloc(35);
	my_heapstat();

	my_free(p1);
	//my_heapstat();

	p4 = my_malloc(10000);
	my_heapstat();

	my_free(p3);
	//my_heapstat();

	p5 = my_malloc(10);
	my_heapstat();

	my_free(p4);
	//my_heapstat();

	p6 = my_malloc(500);
	my_heapstat();

	my_free(p6);
	//my_heapstat();

	my_free(p5);

	my_heapstat();

	return 0;
}