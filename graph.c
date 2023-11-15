#include "graph.h"
#include <stdio.h>
#include <stdlib.h>

/* initialise an empty graph */
/* return pointer to initialised graph */
Graph *init_graph(void)
{
    Graph *g;

    g = initialise_linked_list();

    return g;
}

void free_graph(Graph *graph) /*node->vertex->linked list->node->edge*/
{

    Node *node;
    Vertex *node_vertex;
    Node *temp;

    if (graph == NULL)
    {
        fprintf(stderr, "error: unable to free graph. func: free_graph\n");
        return;
    }

    if (graph->head == NULL)
    {
        free(graph);
        return;
    }

    node = graph->head;
    while (node)
    {
        node_vertex = node->data;
        node = node->next;
        remove_vertex(graph, node_vertex->id);
    }

    node = graph->head;
    while (node)
    {
        temp = node;
        node = node->next;
        free_node(temp);
    }

    /*frees graph pointer when everything else is freed*/
    free(graph);
}

/* initialise a vertex */
/* return pointer to initialised vertex */
Vertex *init_vertex(int id)
{
    /*
    typedef struct Vertex {
        int id;
        LinkedList *edges;
    } Vertex;
    */
    Vertex *v;

    if (id < 0 || id > 255 || id == 0)
    {
        fprintf(stderr, "error: invalid id. func: init_vertex\n");
        return NULL;
    }

    if (!(v = (Vertex *)malloc(sizeof(struct Vertex))))
    {
        fprintf(stderr, "error: unable to intit vertex. func: init_vertex\n");
        return NULL;
    }

    v->id = id;

    if (!(v->edges = initialise_linked_list()))
    {
        fprintf(stderr, "error: unable to intit edges linked list. func: init_vertex\n");
        return NULL;
    }

    return v;
}

/* release memory for initialised vertex */
void free_vertex(Vertex *vertex)
{
    /*
    typedef struct Vertex {
        int id;
        LinkedList *edges;
    } Vertex;
    */

    if (vertex == NULL)
    {
        fprintf(stderr, "error?: provided invalid vertex. func: free_vertex\n");
        return;
    }

    /* free edges linked list*/
    free_linked_list(vertex->edges);

    vertex->id = 0;

    free(vertex);

    return;
}

/* initialise an edge. */
/* return pointer to initialised edge. */
Edge *init_edge(void)
{
    /*
    typedef struct Edge {
        double weight;
        Vertex *vertex;
    } Edge;
    */

    Edge *e;

    if (!(e = (Edge *)malloc(sizeof(struct Edge))))
    {
        fprintf(stderr, "error: unable to intit edge. func: init_edge\n");
        return NULL;
    }

    e->vertex = NULL;
    e->weight = 0.00;

    return e;
}

/* release memory for initialised edge. */
void free_edge(Edge *edge)
{
    if (edge == NULL)
    {
        fprintf(stderr, "error?: NULL edge pointer provided. func: free_edge\n");
        return;
    }

    edge->vertex = NULL;
    edge->weight = 0.00;

    free(edge);
}

/* find vertex with specified id in graph. */
/* return pointer to vertex, or NULL if no vertex found. */
Vertex *find_vertex(Graph *graph, int id)
{
    Node *node_parse;
    Vertex *vertex_check;

    if (graph == NULL || graph->head == NULL)
    {
        fprintf(stderr, "error: NULL graph argument. func: find_vertex\n");
        return NULL;
    }

    if (id < 0 || id > 255 || id == 0)
    {
        fprintf(stderr, "error: invalid id. func: find_vertex\n");
        return NULL;
    }

    node_parse = graph->head;
    while (node_parse)
    {
        vertex_check = node_parse->data;
        if (vertex_check->id == id)
        {
            return vertex_check;
        }
        else
        {
            node_parse = node_parse->next;
        }
    }

    /*value not found when iterating loop*/
    return NULL;
}

/* return pointer to edge from vertex with id from, to vertex with id to. */
/* return NULL if no edge */
Edge *get_edge(Graph *graph, int from, int to)
{
    Vertex *vert_from;
    Vertex *vert_to;
    Node *form_vert_ll;
    Edge *found_edge;

    if (graph == NULL || graph->head == NULL)
    {
        fprintf(stderr, "error: NULL graph arguemnt. func: get_edge\n");
        return NULL;
    }

    if (from < 0 || from > 255 || from == 0)
    {
        fprintf(stderr, "error: invalid id. func: get_edge\n");
        return NULL;
    }

    if (to < 0 || to > 255 || to == 0)
    {
        fprintf(stderr, "error: invalid id. func: get_edge\n");
        return NULL;
    }

    vert_from = find_vertex(graph, from);
    vert_to = find_vertex(graph, to);

    if (vert_from == NULL || vert_to == NULL)
    {
        fprintf(stderr, "error: edge cannot exist if one of the vertex is NULL. func: get_edge\n");
        return NULL;
    }

    if (vert_from->edges == NULL || vert_from->edges->head == NULL)
    {
        fprintf(stderr, "error: vertex has no edges to search. func: get_edge\n");
        return NULL;
    }

    form_vert_ll = vert_from->edges->head;
    while (form_vert_ll)
    {
        found_edge = form_vert_ll->data;

        if (found_edge->vertex->id == to)
        {
            /*edge found*/
            return found_edge;
        }
    }

    /*found edge not returned in loop*/
    return NULL;
}

/* create and add vertex with specified id to graph. */
/* return pointer to vertex or NULL if an error occurs. */
/* if vertex with id already exists, return pointer to existing vertex. */
Vertex *add_vertex(Graph *graph, int id)
{
    Vertex *added_vertex;

    if (graph == NULL || graph->head == NULL)
    {
        fprintf(stderr, "error: NULL graph argument. func: add_vertex\n");
        return NULL;
    }

    if (id < 0 || id > 255 || id == 0)
    {
        fprintf(stderr, "error: invalid id. func: add_vertex\n");
        return NULL;
    }

    added_vertex = find_vertex(graph, id);
    if (added_vertex != NULL)
    {
        return added_vertex;
    }

    /*vertex with id does not exist*/

    /*create vertex*/
    added_vertex = init_vertex(id);

    /*add vertex to graph*/
    append_linked_list(graph, (void *)added_vertex);

    return added_vertex;
}

/* add directed edge with specified weight between vertex with id from */
/* to vertex with id to. */
/* if no vertices with specified ids (from or to) exist */
/* then the vertices will be created. */
/* multiple edges between the same pair of vertices are allowed. */
/* return pointer to edge, or NULL if an error occurs found. */
Edge *add_edge(Graph *graph, int from, int to, double weight)
{

    Vertex *from_vertex;
    Vertex *to_vertex;
    Edge *added_edge;

    from_vertex = NULL;
    to_vertex = NULL;

    if (graph == NULL)
    {
        fprintf(stderr, "error: NULL graph arguemnt. func: add_edge\n");
        return NULL;
    }

    if (from < 0 || from > 255 || from == 0)
    {
        fprintf(stderr, "error: invalid id. func: edd_edge\n");
        return NULL;
    }

    if (to < 0 || to > 255 || to == 0)
    {
        fprintf(stderr, "error: invalid id. func: edd_edge\n");
        return NULL;
    }

    if (weight < 0)
    {
        fprintf(stderr, "error: weight. func: add_edge\n");
        return NULL;
    }

    /*manually find vertex*/
    if (graph->head != NULL)
    {
        from_vertex = find_vertex(graph, from);
        to_vertex = find_vertex(graph, to);
    }

    if (from_vertex == NULL)
    {

        if (!(from_vertex = init_vertex(from)))
        {
            fprintf(stderr, "error: unable to intit vertex. func: add_edge\n");
            return NULL;
        }

        append_linked_list(graph, (void *)from_vertex);
    }

    if (to_vertex == NULL)
    {

        if (!(to_vertex = init_vertex(to)))
        {
            fprintf(stderr, "error: unable to intit vertex. func: add_edge\n");
            return NULL;
        }

        append_linked_list(graph, (void *)to_vertex);
    }

    /*add edge from from_vert*/
    if (!(added_edge = init_edge()))
    {
        fprintf(stderr, "error: unable to init edge. func: add_edge\n");
        return NULL;
    }

    added_edge->vertex = to_vertex;
    added_edge->weight = weight;

    append_linked_list(from_vertex->edges, (void *)added_edge);

    return added_edge;
}

/* add two edges to graph, one from vertex with id from to vertex with id to, */
/* and one from vertex with id to to vertex with id from. */
/* both edges should have the same weight */
/* if no vertices with specified ids (from or to) exist */
/* then the vertices will be created. */
/* multiple edges between the same pair of vertices are allowed. */
void add_edge_undirected(Graph *graph, int from, int to, double weight)
{

    Vertex *vert_from;
    Vertex *vert_to;

    vert_from = NULL;
    vert_to = NULL;

    if (graph == NULL)
    {
        fprintf(stderr, "error: NULL graph arguemnt. func: add_edge_undirected\n");
        return;
    }

    if (from < 0 || from > 255 || from == 0)
    {
        fprintf(stderr, "error: invalid id. func: add_edge_undirected\n");
        return;
    }

    if (to < 0 || to > 255 || to == 0)
    {
        fprintf(stderr, "error: invalid id. func: add_edge_undirected\n");
        return;
    }

    if (weight < 0)
    {
        fprintf(stderr, "error: weight. func: add_edge_undirected\n");
        return;
    }

    if (graph->head != NULL)
    {
        vert_from = find_vertex(graph, from);
        vert_to = find_vertex(graph, to);
    }

    if (vert_from == NULL)
    {

        if (!(vert_from = init_vertex(from)))
        {
            fprintf(stderr, "error: unable to init vertex. func: add_edge_undirected\n");
            return;
        }

        append_linked_list(graph, (void *)vert_from);
    }

    if (vert_to == NULL)
    {

        if (!(vert_to = init_vertex(to)))
        {
            fprintf(stderr, "error: unable to init vertex. func: add_edge_undirected\n");
            return;
        }

        append_linked_list(graph, (void *)vert_to);
    }

    if (!(add_edge(graph, from, to, weight)))
    {
        fprintf(stderr, "error: unable to add edge. func: add_edge_undirected\n");
        return;
    }

    if (!(add_edge(graph, to, from, weight)))
    {
        fprintf(stderr, "error: unable to add edge. func: add_edge_undirected\n");
        return;
    }

    return;
}

/* remove vertex with specified id from graph. */
/* remove all edges between specified vertex and any other vertices in graph. */
void remove_vertex(Graph *graph, int id)
{

    Vertex *removed_vertex;
    Vertex *search_removed_vertex;
    Node *list_node;

    /*sanity check*/
    if (graph == NULL || graph->head == NULL)
    {
        fprintf(stderr, "error: NULL graph argument. func: remove_vertex\n");
        return;
    }

    if (id < 0 || id > 255 || id == 0)
    {
        fprintf(stderr, "error: invalid id. func: remove_vertex\n");
        return;
    }

    removed_vertex = find_vertex(graph, id);
    if (removed_vertex == NULL)
    {
        fprintf(stderr, "error: vertex to remove doesn't exist. func: remove_vertex\n");
        return;
    }

    /*exists a vertex to remove*/
    if (removed_vertex->edges == NULL || removed_vertex->edges->head == NULL)
    {
        /* miss and free vertex in loop*/
    }
    else
    {
        remove_edges(graph, id);
    }

    /*vertex had edges to remove but still keeps edges enitialised*/

    list_node = graph->head;
    while (list_node)
    {
        search_removed_vertex = list_node->data;
        if (search_removed_vertex->id == id)
        {

            /*frees vertex data structure*/
            free_vertex(search_removed_vertex);

            /*appends nodes in the graph*/
            if (list_node->prev != NULL && list_node->next != NULL)
            {
                list_node->prev->next = list_node->next;
                list_node->next->prev = list_node->prev;
                free_node(list_node);
                return;
            }
            else if (list_node->prev == NULL && list_node->next != NULL)
            {
                graph->head = list_node->next;
                list_node->next->prev = NULL;
                free_node(list_node);

                return;
            }
            else if (list_node->prev != NULL && list_node->next == NULL)
            {
                list_node->prev->next = NULL;
                graph->tail = list_node->prev;
                free_node(list_node);
                return;
            }
            else if (list_node->prev == NULL && list_node->next == NULL)
            {
                graph->head = NULL;
                graph->tail = NULL;
                free_node(list_node);
                return;
            }
        }
        list_node = list_node->next;
    }
}

/* remove all edges from vertex with id from to vertex with id to from graph. */
/* remove all edges from vertex with id from to vertex with id to from graph. */
void remove_edge(Graph *graph, int from, int to)
{

    /*find vetrex from && vertex to*/
    Vertex* vert_from;
    Vertex* vert_to;
    Node* node_to_ll;
    Edge* to_remove;

    if(graph == NULL || graph->head == NULL){
        fprintf(stderr,"error: NULL graph arguemnt. func: remove_edge\n");
        return;
    }

    if(from < 0 || from > 255 || from == 0){
        fprintf(stderr,"error: invalid id. func: remove_edge\n");
        return;
    }

    if(to < 0 || to > 255 || to == 0){
        fprintf(stderr,"error: invalid id. func: remove_edge\n");
        return;
    }

    vert_from = find_vertex(graph,from);
    vert_to = find_vertex(graph,to);

    if(vert_from == NULL || vert_to == NULL){
        fprintf(stderr,"error: id: from/to not in graph. func: remove_edge\n");
        return;
    }

    if(vert_from->edges == NULL || vert_from->edges->head == NULL){
        fprintf(stderr,"error: vetrex from id has no edges to free. func: remove_edge\n");
        return;
    }


    node_to_ll = vert_from->edges->head;

    while(node_to_ll){
        to_remove = node_to_ll->data;

        if(to_remove->vertex->id == to){
            free_edge(to_remove);

            if(node_to_ll->prev != NULL && node_to_ll->next != NULL){
                node_to_ll->prev->next = node_to_ll->next;
                node_to_ll->next->prev = node_to_ll->prev;
                free_node(node_to_ll);
                return;
            } else if(node_to_ll->prev == NULL && node_to_ll->next != NULL){
                vert_from->edges->head = node_to_ll->next;
                node_to_ll->next->prev = NULL;
                free_node(node_to_ll);
                return;
            } else if(node_to_ll->prev != NULL && node_to_ll->next == NULL){
                node_to_ll->prev->next = NULL;
                free_node(node_to_ll);
                return;
            } else if(node_to_ll->prev == NULL && node_to_ll->next == NULL){
                vert_from->edges->head = NULL;
                vert_from->edges->tail = NULL;
                free_node(node_to_ll);
                return;
            }

        }

        node_to_ll = node_to_ll->next;
    }

    return;

}


/* remove all edges from vertex with specified id. */
void remove_edges(Graph *graph, int id)
{

    Vertex *vert_id;
    Node *node_to_ll;
    Node *temp;
    Edge *edge;

    if (graph == NULL || graph->head == NULL)
    {
        fprintf(stderr, "error: NULL graph argument. func: remove_edges\n");
        return;
    }

    if (id < 0 || id > 255 || id == 0)
    {
        fprintf(stderr, "error: invalid id. func: remove_edges\n");
        return;
    }

    if (graph->head == NULL)
    {
        fprintf(stderr, "error: no vertex's in graph to remove edges. func: remove_edges");
        return;
    }
    /*else there are vertex's to search*/

    vert_id = find_vertex(graph, id);

    if (vert_id == NULL)
    {
        fprintf(stderr, "error: id has no vertex assocaite. func: remove_edges\n");
        return;
    }

    if (vert_id->edges == NULL || vert_id->edges->head == NULL)
    {
        fprintf(stderr, "error?: vertex has no edges to remove. func: remove_edges\n");
        return;
    }

    node_to_ll = vert_id->edges->head;
    while (node_to_ll)
    {
        temp = node_to_ll;
        node_to_ll = node_to_ll->next;
        edge = temp->data;
        free_edge(edge);
        free_node(temp);
    }

    vert_id->edges->head = NULL;
    vert_id->edges->tail = NULL;

    return;
}

int *get_vertices(Graph *graph, int *count)
{
    Node *node;
    Vertex *vertex;
    int length, i;
    int *array;
    length = 0;
    i = 0;

    if (graph == NULL)
    {
        fprintf(stderr, "error: cannot search empty graph. func: get_verticies\n");
        return 0;
    }
    if (graph->head == NULL)
    {
        fprintf(stderr, "error: graph is empty, cannot fetch vertexes. func: get_verticies\n");
        return 0;
    }

    node = graph->head;
    while (node)
    {
        if (node->data != NULL)
        {
            length = length + 1;
        }
        node = node->next;
    }

    *count = length;

    if (!(array = malloc(sizeof(int) * length)))
    {
        fprintf(stderr, "error: unable to init vertex array. func: get_verticies\n");
        return NULL;
    }

    node = NULL;

    node = graph->head;
    while (i < length && node)
    {
        vertex = node->data;
        array[i] = vertex->id;
        i++;
        node = node->next;
    }

    return array;
}

/* return array of pointers to edges for a given vertex. */
/* array of edges should be dynamically allocated */
/* set count to be number of edges of vertex */
/* return NULL if no edges from/to vertex */
Edge **get_edges(Graph *graph, Vertex *vertex, int *count)
{

    int length, i;
    Node *node;
    Edge **edge_array;

    length = i = 0;

    if (graph == NULL || graph->head == NULL)
    {
        fprintf(stderr, "error: no graph provided to find edges. func: get_edges\n");
        return NULL;
    }

    if (vertex == NULL || vertex->edges == NULL || vertex->edges->head == NULL)
    {
        fprintf(stderr, "error: vertex provided doesnt exist. func: get_edges\n");
        return NULL;
    }

    if (vertex->id < 1 || vertex->id > 255 || vertex->id == 0)
    {
        fprintf(stderr, "error: invalid vertex id provided. func: get_edges\n");
        return NULL;
    }

    node = vertex->edges->head;
    while (node)
    {
        length = length + 1;
        node = node->next;
    }

    *count = length;

    if (!(edge_array = (Edge **)malloc(sizeof(Edge *) * length)))
    {
        fprintf(stderr, "error: cannot create edges array. func: get_edges\n");
        return NULL;
    }

    node = vertex->edges->head;
    while (i < length && node)
    {
        edge_array[i] = node->data;
        i++;
        node = node->next;
    }

    if (edge_array == NULL)
    {
        fprintf(stderr, "error: get edges array is NUL. func: get_edges\n");
        return NULL;
    }

    return edge_array;
}

/* return id of destination node of edge. */
int edge_destination(Edge *edge)
{
    int edgeDest;

    if (edge == NULL)
    {
        fprintf(stderr, "error: edge provided doesn't exist. func: edge_destination\n");
        return 0;
    }

    edgeDest = edge->vertex->id;

    return edgeDest;
}

/* return weight of edge. */
double edge_weight(Edge *edge)
{
    double weit;

    if (edge == NULL)
    {
        fprintf(stderr, "error: edge provided doesn't exist. func: edge_weight\n");
        return 0;
    }

    weit = edge->weight;

    return weit;
}

/* output all vertices and edges in graph. */
/* each vertex in the graphs should be printed on a new line */
/* each vertex should be printed in the following format: */
/* vertex_id: edge_to_vertex[weight] edge_to_vertex[weight] ... */
/* for example: */
/* 1: 3[1.00] 5[2.00] */
/* indicating that vertex id 1 has edges to vertices 3 and 5 */
/* with weights 1.00 and 2.00 respectively */
/* weights should be output to two decimal places */
void print_graph(Graph *graph)
{
    Node *node;
    Vertex *node_vertex;
    Node *vertex_ll_node;
    Edge *vertex_ll_edge;

    if (graph == NULL || graph->head == NULL)
    {
        fprintf(stderr, "error: graph is NULL. func: print_graph\n");
        return;
    }

    /*
    iterate each vertex
    edges linked list
    each edge in linked list
    print next vertex id and weight
    */

    node = graph->head;
    while (node)
    {
        node_vertex = node->data; /*iterate over each vertex*/
        fprintf(stdout, "%d:", node_vertex->id);
        if (node_vertex->edges != NULL && node_vertex->edges->head != NULL)
        {
            vertex_ll_node = node_vertex->edges->head;
            while (vertex_ll_node)
            { /*iterate over each edge in vertex*/
                vertex_ll_edge = vertex_ll_node->data;
                fprintf(stdout, "%d", vertex_ll_edge->vertex->id);
                fprintf(stdout, "[%0.2f] ", vertex_ll_edge->weight);
                vertex_ll_node = vertex_ll_node->next;
            }

            fprintf(stdout, "\n");
        }

        node = node->next;
    }

    return;
}
