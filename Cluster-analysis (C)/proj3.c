/*
 * File: proj3.c
 * Date: 01.12.2015
 * Author: Ján Gula,xgulaj00
 * Project: IZP-DU3
 * Compiler: GNU GCC
 * OS: Windows 10
 * Description: Cluster analysis using single linkage method.
*/

#include <errno.h>
#include <ctype.h> // isdigit
#include <float.h> // FLT_MAX
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h> // Pow
#include <limits.h> // INT_MAX

/*****************************************************************
 * To turn off debugging macros :
 *   a) Set an argument -DNDEBUG
 *   b) Write #define NDEBUG before #include <assert.h>
 */

#ifdef NDEBUG
#define debug(s)
#define dfmt(s, ...
#define dint(i)
#define dfloat(f)
#else

// Writes the debugging string
#define debug(s) printf("- %s\n", s)

// Writes a formated debugging output
#define dfmt(s, ...) printf(" - "__FILE__":%u: "s"\n",__LINE__,__VA_ARGS__)

// Writes debugging information about a variable
#define dint(i) printf(" - " __FILE__ ":%u: " #i " = %d\n", __LINE__, i)

// Writes debugging information about a variable of type float
#define dfloat(f) printf(" - " __FILE__ ":%u: " #f " = %g\n", __LINE__, f)

#endif

/*
	Structure of objects , whereby each object has parameters 
	id = identification number , 
	x = position on axis x , 
	y = position on axis y.
*/

struct obj_t {
    int id;
    float x;
    float y;
};

/*
	Structure of clusters , whereby each cluster has parameters 
	size = number of objects in a cluster , 
	capacity = max number of objects in a cluster , 
	*obj = objects in each cluster.
*/

struct cluster_t {
    int size;
    int capacity;
    struct obj_t *obj;
};

/*
	Function allocates memory for cap clusters and initializes cluster parameters to a default value.(size = 0, capacity=cap)
	Function arguments:
	*c = Cluster which we want to initialize.
	cap = Defines the number of objects allocated for the cluster.
*/

void init_cluster(struct cluster_t *c, int cap)
{
    c->obj = malloc(cap*sizeof(struct obj_t)); // Implicit type cast to (struct cluster_t *)
    assert(c != NULL);
    assert(cap >= 0);
    c->size = 0;
    c->capacity = cap;
    if (c->obj == NULL)
    return;
}

/*
	Function frees the pointer to objects and sets cluster parameters to zero.
	Function arguments:
	*c = Cluster of objects that we want to clear.
*/

void clear_cluster(struct cluster_t *c)
{
    free(c->obj); 
    c->obj = NULL;
    c->size = 0;
    c->capacity = 0;
}

// Chunk of cluster objects. Value recommended for reallocation.
const int CLUSTER_CHUNK = 10;

/*
 	Reallocates memory for clusters according to the new capacity given ( Only if the new capacity is bigger than the old one ).
 	Function arguments:
 	*c = Cluster of objects that we want to resize.
 	new_cap = New capacity of the cluster that we want to resize.
	Return value:
	Returns a resized cluster.
 */

struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap)
{
    assert(c);
    assert(c->capacity >= 0);
    assert(new_cap >= 0);

    if (c->capacity >= new_cap)
        return c;

    size_t size = sizeof(struct obj_t) * new_cap;

    void *arr = realloc(c->obj, size);
    if (arr == NULL)
        return NULL;

    c->obj = (struct obj_t *) arr;
    c->capacity = new_cap;
    return c;
}

/*
	Adds an object to the end of the cluster. The function will extend the cluster (times 2) if the object doesnt fit into the cluster.
	Function arguments:
	*c = Cluster of objects to which we want to add an object.
	obj = Object which we want to add to the end of the cluster.
 */

void append_cluster(struct cluster_t *c, struct obj_t obj)
{
    int i = c->size;
    assert(c != NULL);
        while (c->capacity < c->size+1)
        {
            c = resize_cluster(c, c->capacity * 2);
        }
    c->obj[i].id = obj.id;
    c->obj[i].x = obj.x;
    c->obj[i].y = obj.y;
    c->size++;
}
 
void sort_cluster(struct cluster_t *c);

/*
	This function adds all objects of cluster 2 into cluster 1. If needed the function will extend cluster 1.
	The objects will be sorted in an ascending order according to their ID. Cluster 2 will be unchanged in this function.
	Function arguments:
	*c1 = Cluster to which we want to add all objects of the second cluster.
	*c2 = Cluster from which we will take all objects.
 */

void merge_clusters(struct cluster_t *c1, struct cluster_t *c2)
{
    int i;
    assert(c1 != NULL);
    assert(c2 != NULL);
        while (c1->capacity < c1->size + c2->size) // Does cluster 1 + 2 FIT into cluster 1 ?
            c1 = resize_cluster(c1, c1->capacity * 2); // If not, extend the first cluster

        for (i = 0; i < c2->size; ++i)
        {
            c1->obj[c1->size].x  =  c2->obj[i].x;
            c1->obj[c1->size].y  =  c2->obj[i].y;
            c1->obj[c1->size].id =  c2->obj[i].id;
            c1->size++;
        }
	sort_cluster(c1);
}

/*
	Deletes a cluster from an array of clusters.
	Function arguments:
	*carr = Array of clusters from which we want to delete a cluster.
	narr = Number of clusters in the array of clusters. (carr).
	idx = Position of the cluster that we want to delete, in the array of clusters. (carr)
	Return value:
	Returns the new number of clusters in the array of clusters.
*/

int remove_cluster(struct cluster_t *carr, int narr, int idx)
{
    int i;
    assert(idx < narr);
    assert(narr > 0);
	clear_cluster(&carr[idx]);
        for (i = idx; i < narr-1; ++i)
        {
            carr[i] = carr[i+1];
        }
	return narr-1;
}

/*
	Calculates the Euclidean distance between two objects.
	Function arguments:
	*o1 = First object from which we want to calculate the distance.
	*o2 = Second object from which we want to calculate the distance.
	Return value:
	Returns the Euclidean distance of the first and second object given.
 */

float obj_distance(struct obj_t *o1, struct obj_t *o2)
{
    float dist;
    assert(o1 != NULL);
    assert(o2 != NULL);
    dist = pow(o1->x - o2->x, 2) + pow(o1->y - o2->y, 2);
    return dist;
}

/*
	Calculates the distance between two clusters according to the method of single-linkage clustering.
	Function arguments:
	*c1 = First cluster from which we want to calculate the distance.
	*c2 = Second cluster from which we want to calculate the distance.
	Return value:
	Returns the distance between clusters 1 and 2 according to the method of single-linkage clustering.
*/

float cluster_distance(struct cluster_t *c1, struct cluster_t *c2)
{
    int i, j;
    float min = FLT_MAX, dist; // distance
    assert(c1 != NULL);
    assert(c1->size > 0);
    assert(c2 != NULL);
    assert(c2->size > 0);
	for (i = 0; i < c1->size; i++)
    {
		for (j = 0; j < c2->size; j++)
		{
			dist = obj_distance(&c1->obj[i], &c2->obj[j]);
			if (dist < min) min = dist;
			else            min = min;
		}
	}
	return min;
}

/*
	Functions finds closest clusters from an array of clusters. 
	Function arguments:
	*carr = Array of clusters in which we will search for neighbours.
	narr = Size of the array of clusters.
	*c1 = Position of the first neighbour in the array of clusters.
	*c2 = Position of the second neighbour in the array of clusters.
*/

void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2)
{
    int i, j;
	float min = FLT_MAX, dist;
    assert(narr > 0);
	for (i = 0; i < narr; ++i)
    {
		for (j = i + 1; j < narr; ++j)
		{
			dist = cluster_distance(&carr[i], &carr[j]);
			if (dist < min)
            {
				min = dist;
				*c1 = i; 
				*c2 = j;
			}
		}
	}
}

// Auxiliary function for sorting.
static int obj_sort_compar(const void *a, const void *b)
{
    const struct obj_t *o1 = a;
    const struct obj_t *o2 = b;
    if (o1->id < o2->id) return -1;
    if (o1->id > o2->id) return 1;
    return 0;
}

/*
	Ascendently sorts the cluster, according to the ID of objects in the cluster.
	Function arguments:
	*c = Cluster which we want to sort.
*/

void sort_cluster(struct cluster_t *c)
{
    qsort(c->obj, c->size, sizeof(struct obj_t), &obj_sort_compar);
}

/*
	Prints given cluster to stdout.
	Function arguments:
	*c = Cluster which we want to print.
*/

void print_cluster(struct cluster_t *c)
{
    int i;
    for ( i = 0; i < c->size; i++)
    {
        if (i) putchar(' ');
        printf("%d[%g,%g]", c->obj[i].id, c->obj[i].x, c->obj[i].y);
    }
    putchar('\n');
}

/*
	Function loads objects from the file given and creates a cluster for each object loaded. Clusters are added to an array of clusters. 
	Function allocates memory for an array of clusters and assigns a pointer to the first item of the array. 
	Function arguments:
	*filename = Name of the file from which we want to read data.
	**arr = Pointer to an array of clusters.
	Return value:
	Returns the number of loaded objects.
*/

int load_clusters(char *filename, struct cluster_t **arr)
{
	int i, count;
	FILE *f;
	struct obj_t objects;  
	struct cluster_t *array;
    assert(arr != NULL);
	f = fopen(filename, "r");
    if (f == NULL){
		fprintf(stderr,"Couldnt load your file.\n");
        	return -1;
    }
    *arr = NULL;
	fscanf(f, "count=%d", &count);
	if(count > INT_MAX)
	{
		fprintf(stderr,"Count exceeded INT_MAX.\n");
		return -1;

	}
	array = malloc(count*sizeof(struct cluster_t)); // Implicit type cast to (struct cluster_t *)
    if (array == NULL) {
        fprintf(stderr,"Array failed to allocate.\n");
      	return 0;
    }
	for (i = 0; i < count; i++) {
		fscanf(f, "%d %f %f", &objects.id, &objects.x, &objects.y);
        if (objects.x > 1000 || objects.x < 0) {
            free(array);
            array=NULL;
            while(i>0)
            	i = remove_cluster(array,i,i-1);

        	fprintf(stderr,"Axis x is lower than zero.\n");
        	return -1;
        }
        if(objects.y > 1000 || objects.y < 0) {
            free(array);
            array=NULL;
            while(i>0)
                i = remove_cluster(array,i,i-1);
			fclose(f);
        	return -1;
        }
		init_cluster(&array[i], 1);
		if (array[i].obj == NULL) {	// If an error occurs delete everything initialized.
			while (i > 0)
				i = remove_cluster(array, i, i-1);
        	return 0;
		}
		append_cluster(&array[i], objects); // Add an object to the end of the cluster.
	}
	*arr = array;
	fclose(f);
	return count;
}

/*
	Prints first n = narr clusters.
	Function arguments:
	*carr = First cluster in an array of clusters.
	narr = Number of clusters which we want to print from the array of clusters.
*/
void print_clusters(struct cluster_t *carr, int narr)
{
    int i;
    printf("Clusters:\n");
    for (i = 0; i < narr; i++)
    {
        printf("cluster %d: ", i);
        print_cluster(&carr[i]);
    }
}

int main(int argc, char *argv[])
{
	int c1, c2, clcount, i = 1; // Cluster count
    struct cluster_t *clusters;
    if (argc == 3)
        sscanf(argv[2], "%d", &i);
	else if(argc == 2)
        	i = 1;
    	else {
			fprintf(stderr,"Invalid number of arguments.\n");
			return -1;
		}
	if (i < 0) {
        fprintf(stderr,"Number of clusters is less than zero.\n");
        return -1;
    }
	if ( i > INT_MAX ) {
		fprintf(stderr,"Number of clusters exceeded INT_MAX.");
		return -1;
	}
	clcount = load_clusters(argv[1], &clusters);
    if (clusters == NULL) {
        while (clcount > 0) // Releasing each cluster
            clcount = remove_cluster(clusters, clcount, clcount-1);

        fprintf(stderr,"Cluster loading failed.\n");
        return -1;
    }
	while (clcount > i) {
		find_neighbours(clusters, clcount, &c1, &c2);
		merge_clusters(&clusters[c1], &clusters[c2]);
		clcount = remove_cluster(clusters, clcount, c2);
	}
	print_clusters(clusters, clcount);
    while (clcount > 0)
        clcount = remove_cluster(clusters, clcount, clcount-1);

	free(clusters);
	return 0;
}

