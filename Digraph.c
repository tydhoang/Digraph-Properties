/************************************************************
 * Digraph.c
 * Tyler Hoang
 * Contains the code for the functions and descriptions in Digraph.h
 ************************************************************/
#include "Digraph.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/**
 * DigraphObj typedef struct that is used to construct a Digraph object
 */
typedef struct DigraphObj {
  int numVertices; // keeps track of the number of vertices in the Digraph
  int numEdges; // keeps track of the number of edges in the Digraph
  int numSCCs; // keeps track of the number of SCCs in the Digraph
  List* adjLists; // keeps track of each vertex's neighbors
  int* markers; // used to mark the vertices
  int* times; // used to keep track of the starting time of each vertex
  int* finishTimes; // used to keep track of the finishing time of each vertex
  int largestFinishTime; // keeps track of the largest finish time (to know which vertex to start DFS from after reversal)
  int lowestFinishTime; // keeps track of the lowest finish time
  int SCCVertices; // keeps track of the number of vertices in an SCC
  List currentSCCList; // used as the current List of vertices in an SCC
  List* SCCLists; // keeps track of all the SCCs in the Digraph and their respective vertices
} DigraphObj;

/*** Constructors-Destructors ***/

/**
 * newDigraph method that returns a Digraph which points to a new empty Digraph object with numVertices vertices and no edges
 *
 * @param - numVertices - the number of vertices to be in the Digraph
 * @return - the new Digraph
 */
Digraph newDigraph(int numVertices) {
  Digraph g = malloc(sizeof(DigraphObj)); // allocate a Graph structure from heap memory
  g->numVertices = numVertices; // set numVertices
  g->numEdges = 0; // initialize numEdges
  g->numSCCs = 0; // initialize numSCCs
  g->largestFinishTime = -1; // initialize largestFinishTime
  g->lowestFinishTime = 999999; // initialize lowestFinishTime to be some arbitrary large number
  g-> SCCVertices = 0;

  g->adjLists = malloc(sizeof(List) * numVertices); // allocate a List array structure from heap memory
  for (int i = 0; i < numVertices; i++) {
    g->adjLists[i] = newList(); // create a new List for each element of adjLists
  }

  g->SCCLists = malloc(sizeof(List) * numVertices); // allocate a List array structure from heap memory
  for (int i = 0; i < numVertices; i++) {
    g->SCCLists[i] = newList(); // create a new List for each element of SCCLists
  }

  g->markers = malloc(sizeof(int) * numVertices); // allocate an int array structure from heap memory
  for (int i = 0; i < numVertices; i++) {
    g->markers[i] = UNVISITED; //initialize each element as unvisited
  }

  g->times = malloc(sizeof(int) * numVertices); // allocate an int array structure from heap memory
  for (int i = 0; i < numVertices; i++) {
    g->times[i] = 0; //initialize each element to 0
  }

  g->currentSCCList = newList(); // initialize currentSCCList to be used when creating the array of SCCLists

  g->finishTimes = malloc(sizeof(int) * numVertices); // allocate an int array structure from heap memory
  for (int i =0; i < numVertices; i++) {
    g->finishTimes[i] = 0; // initialize each element as unvisited
  }

  return g;
}

/**
 * freeDigraph method used to deallocate the heap memory used for the Diraph
 *
 * @param - pG - the pointer to the Diraph
 */
void freeDigraph(Digraph* pG) {
  clearDigraph(*pG); // clear every List in the Digraph first
  free(*pG); // free the memory
  *pG = NULL;  // safely set the pointer to NULL
}

/**
 * clear method that resets the Digraph to the empty state
 *
 * @param - G - the Digraph
 */
void clearDigraph(Digraph G) {
  for (int i = 0; i < getOrder(G); i++) { // iterate through the adjLists array
    List L = G->adjLists[i];
    freeList(&L); // free each List
  }

  for (int i = 0; i < getOrder(G); i++) { // iterate through the SCCLists array
    List L = G->SCCLists[i];
    freeList(&L); // free each List
  }

  List K = G->currentSCCList;
  freeList(&K); // free the currentSCCList List
  free(G->adjLists); // free the adjLists array
  free(G->SCCLists); // free the SCCLists array
  free(G->markers); // free the markers array
  free(G->times); // free the times array
  free(G->finishTimes); // free the finishTimes array
  G->adjLists = NULL; // set the adjLists pointer to NULL
  G->SCCLists = NULL; // set the SCCLists pointer to NULL
  G->markers = NULL; // set the markers pointer to NULL
  G->times = NULL; // set the times pointer to NULL
  G->finishTimes = NULL; // set the finishTimes pointer to NULL
}

/*** Access functions ***/

/**
 * getOrder method that returns the number of vertices in G
 *
 * @param - G - the Digraph
 * @return - numVertices
 */
int getOrder(Digraph G) {
  return G->numVertices;
}

/**
 * getSize method that returns the number of edges in G
 *
 * @param - G - the Digraph
 * @return - numEdges
 */
int getSize(Digraph G) {
  return G->numEdges;
}

/**
 * getOutDegree method that returns the number of neighbors that vertex v has in G
 *
 * @param - G - the Digraph
 * @param - u - the vertex
 * @return - the number of neighbors
 */
int getOutDegree(Digraph G, int u) {
  return length(G->adjLists[u - 1]);
}

/**
 * getNeighbors method that returns the List of neighbors of vertex u in G
 *
 * @param - G - the Digraph
 * @param - u - the vertex
 * @return - the neighbor List
 */
List getNeighbors(Digraph G, int u) {
  return G->adjLists[u - 1];
}

/*** Manipulation procedures ***/

/**
 * addEdge method that adds an edge between two vertices in G, going from u to v
 *
 * @param - G - the Graph
 * @param - u - a vertex
 * @param - v - a vertex
 * @return - 0 if this edge is legal, otherwise return 1
 */
int addEdge(Digraph G, int u, int v){
  if (u < 1 || v < 1) { // if u or v are less than 1
    return -1; // illegal
  }
  if (u > G->numVertices || v > G->numVertices) { // if u or v are greater than numVertices
    return -1; // illegal
  }

  bool exists = false; // used to see if the one vertex exists in the other vertex's List of neighbors
  Node currentU = getFront(G->adjLists[u-1]); // set currentU to be the first Node in u's List of neighbors

  if (currentU == NULL) { // if u doesn't have a List of neighbors                                                   
    G->numEdges++;
    append(G->adjLists[u - 1], v - 1); // add v to the List                                                          
    exists = true; // exists is set to true here so that the program will skip over the next if statement            
  }

  if (currentU != NULL && !exists) { // if u has a List of neighbors
    while(currentU != NULL) { // traverse through the List
      if (getValue(currentU) == v - 1) { // if v is already in u's List of neighbors
	exists = true;
	return 1;
	break;
      }
      currentU = getNextNode(currentU);;
    }
  }
  
  if(!exists) { // if this is a new edge being added and if u has a List of neighbors
    Node traversal = getFront(G->adjLists[u - 1]);
    // insert v into the neighor List in descending order (keeps the Lists sorted)
    while (traversal != NULL) { // traverse through u's List of neighbors      
      if (v-1 < getValue(traversal)) { // if v is less than the current neighbor in the traversal
	G->numEdges++;
	insertBefore(G->adjLists[u - 1], traversal, v - 1); // insert v before this value
	break;
      }
      else { // if v is greater than the current neighbor in the traversal
	traversal = getNextNode(traversal); // move onto the next neighbor
      }
    }

    if (traversal == NULL) { // if v is greater than all the neighbors of u
      G->numEdges++;
      append(G->adjLists[u - 1], v - 1); // add v to the end of u's neighbor List
    }
  }

  return 0;
}

/**
 * deleteEdge method that deletes an edge between two vertices in G, going from u to v
 *
 * @param - G - the Graph
 * @param - u - a vertex
 * @param - v - a vertex
 * @return - 0 if this edge was successfully deleted, otherwise return 1
 */
int deleteEdge(Digraph G, int u, int v) {
  if (u < 1 || v < 1) { // if u or v are less than 1
    return -1; // illegal
  }
  if (u > G->numVertices || v > G->numVertices) { // if u or v are greater than numVertices
    return -1; // illegal
  }

  bool exists = false; // used to see if the v vertex exists in the u vertex's List of neighbors
  Node currentU = getFront(G->adjLists[u - 1]); // set currentU to be the first Node in u's List of neighbors

  if (currentU != NULL) { // if u has a List of neighbors
    while(currentU != NULL) { // traverse through the List
      if (getValue(currentU) == v - 1) { // if v is in u's List of neighbors
        exists = true;
        break;
      }
      currentU = getNextNode(currentU);;
    }
  }

  if (exists) { // if v is in u's List of neighbors
    G->numEdges--; 
    detachNode(G->adjLists[u - 1], currentU); // detach the v Node
    deleteNode(G->adjLists[u - 1], currentU); // delete the v Node
  }

  if (currentU == NULL) { // if u doesn't have a List of neighbors
    return 1;
  }

  if(!exists) { // if v is not in u's List of neighbors
    return 1;
  }

  return 0;
}

/**
 * visitAll method that sets every element in markers to be unvisited
 *
 * @param - G - the Digraph
 */
void unvisitAll(Digraph G){
  for (int i = 0; i < G->numVertices; i++) {
    G->markers[i] = UNVISITED; // set every element in the markers array to be unvisited
  }
}

/**
 * getMark method that returns the mark for vertex u
 *
 * @param - G - the Digraph
 * @param - u - the vertex
 * @return - UNVISITED, INPROGRESS, or ALLDONE
 */
int getMark(Digraph G, int u){
  return G->markers[u-1];
}

/**
 * setMark method that sets the mark for vertex u
 *
 * @param - G - the Digraph
 * @paran - u - the vertex
 * @param - theMark - the mark to set u to
 */
void setMark(Digraph G, int u, int theMark){
  G->markers[u-1] = theMark;
}

/**
 * DFS method that will be used for finding the SCCs and their properties
 *
 * @param - G - the Digraph
 * @param - u - the starting vertex
 * @param - t - the current time
 * @return - the current time
 */
int DFS(Digraph G, int u, int t) {
  int currentTime = t;
  G->times[u - 1] = currentTime;
  currentTime++;
  setMark(G, u, INPROGRESS); // set the current starting vertex as INPROGRESS
  List L = getNeighbors(G, u);

  Node current = getFront(L);
  while (current != NULL) { // traverse through the neighbors of the current vertex
    if (getMark(G, getValue(current) + 1) == UNVISITED) { // if the neighbor is UNVISITED
      prepend(G->currentSCCList, getValue(current) + 1); // add this vertex to the current SCC List
      currentTime = DFS(G, getValue(current) + 1, currentTime); // set the neighbor as the next starting point
      currentTime++;
    }
    current = getNextNode(current);
  }
  if (currentTime > G->largestFinishTime) { // set the largestFinishTime
    G->largestFinishTime = currentTime;
  }
  if (currentTime < G->lowestFinishTime) { // set the lowestFinishTime
    G->lowestFinishTime = currentTime;
  }

  G->finishTimes[u - 1] = currentTime; // keep track of each vertex's finish time
  G->markers[u - 1] = ALLDONE; // if all of the current vertex's neighbors are visited, mark the current vertex as ALLDONE
  return currentTime;
}

/**
 * getCountSCC method that returns the number of SCCs in G. Also creates the SCCLists array for numSCCVertices and inSameSCC
 *
 * @param - G - the Digraph
 * @return - the number of SCCs
*/
int getCountSCC(Digraph G) {
  unvisitAll(G); // reset the markers of the Digraph
  G->numSCCs = 0; // reset numSCCs to 0
  G->largestFinishTime = -1;
  G->lowestFinishTime = 999999;

  for (int e = 0; e < G->numVertices; e++) { // clear each SCCList in SCCLists
    clear(G->SCCLists[e]);
  }

  for (int i = 0; i < getOrder(G); i++) { // perform DFS on every vertex in G to find the finish times
    if (G->markers[i] == UNVISITED) {
      G->times[i] = G->largestFinishTime + 1; // when DFS moves onto the next UNVISITED vertex, the start time for it is +1 over the current largest finish time
      DFS(G, i + 1, G->times[i]); // perform depth first search on all unvisited vertices
    }
  }

  Digraph RG = newDigraph(G->numVertices); // this will be used to find the number of SCCs in G
  for(int i = 0; i < G->numVertices; i++) { // create a new graph that is G but with all the edges reversed
    Node current = getFront(G->adjLists[i]);
    while (current != NULL) {
      append(RG->adjLists[getValue(current)], i);
      current = getNextNode(current);
    }
  }

  //RG is created so that the actual Digraph G isn't altered
  unvisitAll(RG); // set all the vertices in RG to be UNVISITED
  int k = 0; // used to iterate through the SCCLists array
  for (int i = G->largestFinishTime; i >= G->lowestFinishTime; i--) { // set i to be the largest finish time of G and decrement lower to the next highest finish time
    int startingPoint = 0;
    for (int j = 0; j < G->numVertices; j++) {
      if (G->finishTimes[j] == i) {
	startingPoint = j; // set the starting point to be the current largest finish time
      }
    }
    if (RG->markers[startingPoint] == UNVISITED) { // if this starting point is UNVISITED, this is a new SCC in G
      G->numSCCs++; // increment numSCC in G
      clear(RG->currentSCCList); // reset currentSCCList for this current SCC in G
      prepend(RG->currentSCCList, startingPoint + 1); // prepend the starting point into this current SCCList
      DFS(RG, startingPoint + 1, RG->times[startingPoint]); // perform depth first search starting from this current starting point, creating this current SCCList

      Node newcurrent = getFront(RG->currentSCCList);
      while (newcurrent != NULL) {
	append(G->SCCLists[k], getValue(newcurrent)); // add this currentSCCList to the SCCLists array
	newcurrent = getNextNode(newcurrent);
      }
      k++; // move onto the next index in the SCCLists array so that the next SCCList can be added into it                                                                                     
    }
  }

  // RG has been used to create the SCCLists array, so it's job is finished
  freeDigraph(&RG);
  return G->numSCCs;
}

/**
 * getNumSCCVertices method that returns the number of vertices in the SCC that contains vertex u in G
 *
 * @param - G - the Digraph
 * @param - u - the vertex
 * @return - the number of vertices in the SCC that contains u
 */
int getNumSCCVertices(Digraph G, int u) {
  getCountSCC(G); // create the SCCLists array, which contains each SCC in G and the respective vertices in each SCC
  int num = 0;

  for (int i = 0; i < G->numVertices; i++) { // find u in SCCLists
    Node current = getFront(G->SCCLists[i]);
    while (current != NULL) {
      if (getValue(current) == u) {
	num = length(G->SCCLists[i]); // the length of the SCCList that u is in is the number of vertices in the SCC
      }
      current = getNextNode(current);
    }
  }
    return num;
}

/**
 * inSameSCC method that returns 1 if u and v are in the same SCC, 0 if they are not, and -1 if they are illegal vertices
 *
 * @param - G - the Digraph
 * @param - u - a vertex
 * @param - v - another vertex
 */
int inSameSCC (Digraph G, int u, int v) {
  getCountSCC(G); // create the SCCLists array, which contains each SCC in G and the respective vertices in each SCC
  if (u == v) { // if u and v are the same vertex
    return 1; // they are in the same SCC
  }
  if (u < 1 || v < 1) { // if u or v are less than 1
    return -1; // illegal
  }
  if (u > G->numVertices || v > G->numVertices) { // if u or v are greater than numVertices
    return -1; // illegal
  }

  bool uExists = false; // used to check if u is in an SCC List
  bool vExists = false; // used to check if v is in an SCC List
  for (int i = 0; i < G->numVertices; i++) {
    Node current = getFront(G->SCCLists[i]);
    while (current != NULL) { // iterate through each SCCList in the SCCLists array
      if (getValue(current) == u) { // if u exists in the current SCCList
	uExists = true;
      }
      else if (getValue(current) == v) { // if v exists in the current SCCList
	vExists = true;
      }
      current = getNextNode(current);
    }

    if (uExists && vExists) { // if both u and v are in the current SCCList, they are in the same SCC
      break;
    }
    else { // if both u and v aren't found in the SCCList
      uExists = false;
      vExists = false;
    } // move onto the next SCCList in the SCCLists array
  }

  if (uExists && vExists) {
    return 1; // they are in the same SCC
  }

  return 0; // u and v are not in the same SCC
}

/**
 * printDigraph method that prints the Digraph to the file pointed by out. Edges
 * are printed in sorted order
 *
 * @param - out - the file to be printed to
 * @param - G - the Digraph
 */
void printDigraph(FILE* out, Digraph G) {
  fprintf(out, "%d", G->numVertices); // first print numVertices
  for (int i = 0; i < getOrder(G); i++) { // iterate through the adjLists array
    Node current = getFront(G->adjLists[i]);
    while(current != NULL) { // traverse through each neighbors List
      fprintf(out, ", %d %d", i + 1, getValue(current) + 1);
      current = getNextNode(current);
    }
  }
  fprintf(out, "\n");
}
