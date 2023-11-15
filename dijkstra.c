/*
 *  dijkstra.c
 *  ProgrammingPortfolio
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include "graph.h"
#include "dijkstra.h"


/* find shortest paths between source node id and all other nodes in graph. */
/* upon success, returns an array containing a table of shortest paths.  */
/* return NULL if *graph is uninitialised or an error occurs. */
/* each entry of the table array should be a Path */
/* structure containing the path information for the shortest path between */
/* the source node and every node in the graph. If no path exists to a */
/* particular desination node, then next should be set to -1 and weight */
/* to DBL_MAX in the Path structure for this node */
Path *dijkstra(Graph *graph, int id, int *pnEntries)
{
    Vertex* startingVertex;
    
    Path* path;
    int** R;
    Edge* edge;
    int* vt;
    int vert_count;
    int i;
    int starting_pos;
    double** D;
    int static_vert_count;
    int* static_count;
    int c;
    double minimum;
    int smallest_val;
    Edge** current_edges;
    Vertex* current_node;
    int next_node;
    Edge* current_edge;
    int current_edges_count;
    double new_edge_weight;
    double current_edge_weight;
    double edge_weight_from_u_v;
    double weight_comparison;
    int found;
    int max_vertex;
    int max_vertex_length;
    int *max_array;
    int next_hop;

    found =0;
    minimum = 0.00;
    vert_count = 0;
    i = 0;
    starting_pos =0;
    current_edges_count = 0;
    path = NULL;

    /*
    S = nodes not visited (excluding start)
    R = route taken to get to network
    D = weight / cost of that visit
    */

    /* Insert your implementation of Dijkstra's algorithm here */

    /*error checking*/
    if(graph == NULL){
        fprintf(stderr,"error: inavlid graph argument\n");
        return NULL;
    }
    
    if(graph->head == NULL){
        fprintf(stderr,"error: inavlid graph argument (no elements)\n");
        return NULL;        
    }

    if(id < 1 || id > 255 || id == 0){
        fprintf(stderr,"error: id element not provided\n");
        return NULL;
    }

    startingVertex = find_vertex(graph,id);   /*returns NULL if not present*/
    if(startingVertex == NULL){
        fprintf(stderr,"error: starting vertex not in graph\n");
        return NULL;
    }

    /*function to put find each veretex  -Represents node graph in table*/
    vt = get_vertices(graph,&vert_count);
    if(vt == NULL || vert_count == 0){
        fprintf(stderr,"error: no vertex's in graph\n");
        return NULL;
    }

    printf("id to remove: %d\n",id);

    for (c = 0; c < vert_count; c++){
       printf("%d\n", vt[c]); 
    }
    printf("vert count: %d\n",vert_count);


    /*removing starting vertex from array*/
    for(i = 0; i < vert_count;i++){
        if(vt[i] == id){
            starting_pos = i+1;
            break;
        }
    }

    if(starting_pos < 0 || starting_pos > vert_count)
    {
        fprintf(stderr,"error: invalid element position\n");
        return NULL;
    } else {
        for(i = starting_pos -1; i < vert_count -1; i++){
            vt[i] = vt[i+1]; 
        }
    }

    vert_count--;


    printf("Resultant array:\n");
    for(c = 0; c < vert_count; c++){
       printf("%d\n", vt[c]); 
    }

    printf("arrat length: %d\n",vert_count);


    /*table S(vt) is contains all vertex instead of starting id*/
    static_count = get_vertices(graph,&static_vert_count);              /*original vertex list*/
    if(static_vert_count == 0 || static_count == NULL){
        fprintf(stderr,"static_vert_count = 0 || static_count == NULL, cannot init R & D\n");
        return NULL;
    }

    /*indexes rows*/
    if(!((R = (int **)malloc(static_vert_count * sizeof(int *))))){
        fprintf(stderr,"failure to initialise rows of array D\n");
        return NULL;
    }  

    /*indexes columns*/
    for(i=0; i < static_vert_count;i++){
        if(!((R[i] = (int *)malloc(sizeof(int) * 2)))){
            fprintf(stderr,"failure to initialise columns of R\n");
            return NULL;
        }
    }

    if(!(D = (double **)malloc(static_vert_count * sizeof(double *)))){
        fprintf(stderr,"failure to initialise array D\n");
        return NULL;
    }    /*all tables are initialised with memory*/

    for(i=0;i<static_vert_count;i++){
        if(!((D[i] = (double *)malloc(sizeof(double) * 2)))){
            fprintf(stderr,"failure to initialise columns of D\n");
            return NULL;
        }
    }

    for(i=0; i<static_vert_count; i++){
        R[i][0] = static_count[i]; 
        D[i][0] = (double)static_count[i];                  
         
        if(R[i][0] == id && D[i][0] == id){                  /*R[i][0] == id to remove*/
            R[i][1] = -1;                  /*set to -1 (doesent exist)*/
            D[i][1] = DBL_MAX;
        } else {
            edge = get_edge(graph,id,static_count[i]);
            if(edge == NULL){
                /*no edge pointer*/
                R[i][1] = -1;
                D[i][1] = DBL_MAX;
            } else {
                R[i][1] = edge->vertex->id;
                D[i][1] = edge->weight;
            }   
        }                             
    }

    /*table S,R,D all initialised with values*/

    printf("\nPrinting the 2D Array R\n");
 	for(i = 0; i < static_vert_count; i++)
  	{
  		for(c = 0; c < 2; c++)
  		{
  			printf("%d  ", R[i][c]);
		}

   		printf("\n");
  	}  	

    printf("\nPrinting the 2D Array D\n");
 	for(i = 0; i < static_vert_count; i++)
  	{
  		for(c = 0; c < 2; c++)
  		{
  			printf("%0.2f  ", D[i][c]);
		}

   		printf("\n");
  	}

    printf("list of nodes: unvisited\n");

    for(i=0;i<vert_count;i++){
        printf("univsited vertex: %d\n",vt[i]);
    }


    /*
    algorithm  in video sldies
    vt is array 
    vert_count is the count of the unvisited verts
    */

    while(vert_count != 0)
    {  

        /*getting smallest weight*/
        smallest_val = vt[0];
        for(c=0; c < static_vert_count; c++)
        {
            if(D[c][0] == id)
            {
                /*move onto next*/
            } else if (D[c][0] == smallest_val){
                minimum =  D[c][1];
                smallest_val = D[c][0];
                break;
            }
        }

        for(c=0;c<vert_count;c++){
            for(i=0; i< static_vert_count; i++)
            {
                if(D[i][0] == id)
                {
                    /*move onto next*/
                } else if(vt[c] == D[i][0]){
                    
                    if(D[i][1] < minimum)
                    {
                        minimum = D[i][1];
                        smallest_val = D[i][0];
                    }
                }
            }    
        }


        printf("minimum distance: %0.2f\n",minimum);
        printf("minimum value: %d\n",smallest_val);

        /*if smallest is DBL_MAX then there are no node connections*/
        if(minimum == DBL_MAX)
        {
            /*abort this node as none connect to it*/
            fprintf(stderr,"smallest value is DBL_MAX: must abort\n");
            break;

        } else {
            /*removing starting vertex from array*/
            for(i = 0; i < vert_count;i++)
            {
                if(vt[i] == smallest_val)
                {
                    starting_pos = i+1;
                    break;
                }
            }

            if(starting_pos < 0 || starting_pos > vert_count)
            {
                fprintf(stderr,"error: invalid element position\n");
                return NULL;
            } else {
                for(i = starting_pos -1; i < vert_count -1; i++)
                {
                    vt[i] = vt[i+1]; 
                }
            }

            vert_count--;

            /*prints new S array and length*/
            printf("Resultant array:\n");
            for(c = 0; c < vert_count; c++)
            {
                printf("%d\n", vt[c]); 
            }

            printf("array length: %d\n",vert_count);

            /*now gets node working on and node edges*/
            current_node = find_vertex(graph,smallest_val);
            if(current_node == NULL)
            {
                fprintf(stderr,"error: when trying to find smallest graph in node returned NULL\n");
                return NULL;
            }

            /*gets edges of that node*/
            current_edges = get_edges(graph,current_node,&current_edges_count);
            if(current_edges == NULL)
            {
                fprintf(stderr,"error: get edges returned NULL\n");                     /*could produce error*/
                return NULL;
            }
            
            /*id node has no edges then set them to -1 && DBL_MAX*/
            if(current_edges_count == 0)
            {
                /*set node next hop to -1*/
                /*set distance as DBL_MAX*/
                for(i=0; i< static_vert_count; i++){
                    if(D[i][0] == id && R[i][0] == id){
                        /*move onto next*/
                    } else if(D[i][0] == smallest_val && R[i][0] == smallest_val){
                        D[i][1] = DBL_MAX;
                        R[i][1] = -1;
                    }
                }
            } else {
                /*loop thorugh edges count */    /*for each node V such that (current_node,edge->vertex) is an edge */
                for(i=0;i<current_edges_count;i++){
                    /*gets the first edge*/
                    current_edge = current_edges[i];
                    /*gets that edge weight*/
                    edge_weight_from_u_v = edge_weight(current_edge);
                    /*gets the next node ID*/
                    next_node = edge_destination(current_edge);
                    /*check edge destination is in S array*/
                    for(c=0;c<vert_count;c++){
                        if(vt[c] == next_node){
                            /*node still in S*/
                            found = 1;
                            break;
                        }
                    }

                    if(found == 0){
                        /*vertex was not in S*/
                        fprintf(stderr,"error: vertex of edge not in array S\n");
                    } else {
                        /*vertex was in S so gets the edge weight to current node to add to edge weight*/
                        for(c=0; c< static_vert_count; c++){
                            if(D[c][0] == id){
                                /*move onto next*/
                            } else {
                                if(D[c][0] == smallest_val){
                                    current_edge_weight = D[c][1];
                                }
                            }
                        }

                        new_edge_weight = current_edge_weight + edge_weight_from_u_v;

                        /*gets the table edge weight for the edge pointer*/
                        for(c=0; c< static_vert_count; c++){
                            if(D[c][0] == id){
                                /*move onto next*/
                            } else {
                                if(D[c][0] == next_node){
                                    weight_comparison = D[c][1];
                                }
                            }
                        }

                        /*getting next hop from source node*/
                        next_hop = current_node->id;
                        for(c=0;c<static_vert_count;c++){
                            if(R[c][0] == next_hop){
                                if(R[c][1] == id){
                                    /*previous is source so keep as same*/
                                    next_hop = current_node->id;
                                }else {
                                    /*recent hop is not id*/
                                    next_hop = R[c][1];
                                }
                            }
                        }

                        if(new_edge_weight < weight_comparison){
                            for(c=0; c< static_vert_count; c++){
                                if(D[c][0] == id && R[c][0]){
                                    /*move onto next*/
                                } else {
                                    if(D[c][0] == next_node && R[c][0] == next_node){
                                        D[c][1] = new_edge_weight;
                                        R[c][1] = next_hop;
                                        break;
                                    }
                                }
                            }   
                        }

                    }   /*ends found == true if statement*/

                }   /* ends for loop for each edge for that vertex*/

            }   /* ends if statement to make sure there are edges in edge list*/
            
            free(current_edges);

        }   /*ends else statement for DBL_MAX not being minimum*/

    }   /*ends while loop for vert_count*/

    /*allocating path array*/
    /*array size needs to be of highest vertex*/

    max_array = get_vertices(graph,&max_vertex_length);
    max_vertex = max_array[0];
    for(i=0;i<max_vertex_length;i++){
        if(max_array[i] > max_vertex){
            max_vertex = max_array[i];
        }
    }

    max_vertex += 1;
    printf("max element array is: %d\n",max_vertex);

    path = (Path *)malloc(sizeof(Path) * max_vertex);

    for(i=0;i<max_vertex_length;i++){
        for(c=0;c<static_vert_count;c++){
            if(max_array[i] == R[c][0]){
                path[max_array[i]].next_hop = R[c][1];
                path[max_array[i]].weight = D[c][1]; 
            }
        }
    }

    *pnEntries = max_vertex;

    free(vt);
    free(static_count);
    free(max_array);

    for(i=0;i<static_vert_count;i++){
        free(R[i]);
        free(D[i]);
    }

    free(D);
    free(R);

    /*return R*/
    return path;
}
