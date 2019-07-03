/************************************************************
 * DigraphProperties.c
 * Tyler Hoang
 * tydhoang
 * PA4
 * Reads each line of an input file, creating a Digraph from the first line, then prints
 * properties of the Digraph based on the next input lines
 ************************************************************/
#include "Digraph.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX 999999 //used to set the max size of the input line

int main (int argc, char* argv[]) {
  FILE* out;
  FILE* in;
  char line[MAX]; // char array to store words from input file
  
  // check command line for correct number of arguments
  if( argc != 3 ){
    printf("Usage: %s <input file> <output file>\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  
  // open input file for reading
  in = fopen(argv[1], "r");
  if( in==NULL ){
    printf("Unable to read from file %s\n", argv[1]);
    exit(EXIT_FAILURE);
  }
  
  // open output file for writing
  out = fopen (argv[2], "w");
  if( out==NULL ){
    printf("Unable to write to file %s\n", argv[2]);
    exit(EXIT_FAILURE);
  }

  fgets(line, MAX, in); // get the first line of the input file
  int i = 0; // used to iterate through the input line
  int p = 0; // used later to mark where the first comma in the line is
  int value = 0; // will be used when the number is converted to an int
  char number = *(line); // set number to be the first digit in the line
  
  while (((int) number < 58) && ((int) number > 47)) { // until p encounters a comma                         
    value = value * 10 + (number - 48); // convert the digit to an int and add it to the existing value      
    p++; // increment p                                                                                      
    number = *(line + p); // move onto the next digit                                                    
  }  
  
  int vertices = value; //set numVertices to value
  if (vertices == 0) {
    fprintf(out, "%s", line);
    fprintf(out, "ERROR\n");
    fclose(in);
    fclose(out);
    return (EXIT_SUCCESS);
  }

  value = 0; // reset value to be used for the next digit
  Digraph myDigraph = newDigraph(vertices); // create a new Digraph object with numVertices

  bool newSet = true; // will be used after each comma is encountered to signify a new set of vertices
  int adjListsLocation = 0; // will later be used as vertex u
  int neighborToBeAdded = 0; // will later be used as vertex v
  
  while(p + i < strlen(line)) { // iterate through each of the next characters in the first line
    if (!(((int) number < 58) && ((int) number > 47))) { // if the character is not a digit
      if ((int) number != 44) { // if the character is not a comma, which means it is a space
	i++;
	number = *(line + p + i); // move onto the next digit
	continue;
      }
    }
    
    value = 0; // reset value after every space
    if (((int) number < 58) && ((int) number > 47)) { // if number is between '0' and '9' (ASCII values)
      while (((int) number < 58) && ((int) number > 47)) { // until i encounters a space
	value = value * 10 + (number - 48); // convert the digit to an int and add it to the existing value
	i++; // increment i
	number = *(line + p + i); // move onto the next digit
      }
      
      if (newSet) { // if this is a new set of vertices
	adjListsLocation = value;
	newSet = false; // this is no longer a new set of vertices
	continue; // move onto the next digit
      }
      
      if (!newSet) { // while in this current set of vertices
	neighborToBeAdded = value;
	if (neighborToBeAdded > getOrder(myDigraph) || adjListsLocation > getOrder(myDigraph)) { // if any input vertices are greater than numVertices
	  fprintf(out, "%s", line);
	  fprintf(out, "ERROR\n"); // this Digraph is an error
	  freeDigraph(&myDigraph);
	  fclose(in);
	  fclose(out);
	  return (EXIT_SUCCESS);
	}

	addEdge(myDigraph, adjListsLocation, neighborToBeAdded); // add the edge to the Digraph
	continue; // move onto the next set of vertices
      }
    }
    
    if ((int) number == 44) { // if the current character is a comma
      if (adjListsLocation == 0 || neighborToBeAdded == 0) { // if all the program has iterated through was the first digit in the line
	i++;
	number = *(line + p + i);
	newSet = true;
	continue; // begin iterating through the set of vertices
      }
      
      newSet = true;
      value = 0;
      i++;
      number = *(line + p + i);
      continue; // move onto the next set of vertices
    }
  }

  /////////////////////////////////////////////////////////////////////
  // Graph has been created
  /////////////////////////////////////////////////////////////////////
  
  // chars that will be used to see input lines have certain keywords
  const char PRINTDIGRAPH[20] = "PrintDigraph";
  const char GETORDER[20] = "GetOrder";
  const char GETSIZE[20] ="GetSize";
  const char GETOUTDEGREE[20] = "GetOutDegree";
  const char ADDEDGE[20] = "AddEdge";
  const char DELETEEDGE[20] = "DeleteEdge";
  const char GETCOUNTSCC[20] = "GetCountSCC";
  const char GETNUMSCCVERTICES[20] = "GetNumSCCVertices";
  const char INSAMESCC[20] = "InSameSCC";
  
  while (fgets(line, MAX, in) != NULL) { // while there is a next line in the input file
    
    char *printdigraph = strstr(line, PRINTDIGRAPH); // if input line is PrintDigraph
    char *getorder = strstr(line, GETORDER); // if input line is GetOrder
    char *getsize = strstr(line, GETSIZE); // if input line is GetSize
    char *getoutdegree = strstr(line, GETOUTDEGREE); // if input line is GetOutDegree
    char *addedge = strstr(line, ADDEDGE); // if input line is AddEdge
    char *deleteedge = strstr(line, DELETEEDGE); // if input line is DeleteEdge
    char *getcountscc = strstr(line, GETCOUNTSCC); // if input line is GetCountSCC    
    char *getnumsccvertices = strstr(line, GETNUMSCCVERTICES); // if input line is GetNumSCCVerices
    char *insamescc = strstr(line, INSAMESCC); // if input line is InSameScc

    if (!printdigraph && !getorder && !getsize && !getoutdegree && !addedge && !deleteedge && !getcountscc && !getnumsccvertices && !insamescc) { // if the input line is an unknown command
      if (strlen(line) == 1) {
	continue;
      }
      fprintf(out, "%s", line);
      fprintf(out, "ERROR\n");
    }

    if (printdigraph) { // if the input line is PrintDigraph
      if (strlen(line) > 13) { // if there are any characters after PrintDigraph
	fprintf(out, "%s", line);
	fprintf(out, "ERROR\n");
      }
      
      else {
	fprintf(out, "PrintDigraph\n");
	printDigraph(out, myDigraph);
      }
    }
    
    if (getorder) { // if the input line is GetOrder
      int order = getOrder(myDigraph);
      if (strlen(line) > 9) { // if there are any characters after GetOrder
        fprintf(out, "%s", line);
        fprintf(out, "ERROR\n");
      }

      else{
	fprintf(out, "GetOrder\n");
	fprintf(out, "%d\n", order);
      }
    }
    
    if (getsize) { // if the input line is GetSize
      int size = getSize(myDigraph);
      if (strlen(line) > 8) { // if there are any characters after GetSize
        fprintf(out, "%s", line);
        fprintf(out, "ERROR\n");
      }

      else{
	fprintf(out, "GetSize\n");
	fprintf(out, "%d\n", size);
      }
    }

    if (getoutdegree) { // if the input line is getOutDegree
      int digit = 0; // will be used when the char is converted to an int
      int k = 0; // used to iterate through the input line
      char ch = *(line + 13); // set ch to be right after the chars 'getOutDegree'
      
      if (!(((int) ch < 58) && ((int) ch > 47))) { // if there is no number provided
	fprintf(out, "%s", line);
	fprintf(out, "ERROR\n");
      }
      
      else{ // there is a number provided
	while (((int) ch < 58) && ((int) ch > 47)) { // until k encounters a space
	  if (k + 13 == strlen(line) - 1) { // once the end of the input line as been reached
	    break;
	  }
	  digit = digit * 10 + (ch - 48); // convert ch to an int and add it to the existing value of digit
	  k++; // increment k
	  ch = *(line + 13 + k); // move onto the next digit
	}
	
	int u = digit; // set u to be the converted int    
	if (u > getOrder(myDigraph) || u < 1) { // if u is greater than numVertices or less than 1
	  fprintf(out, "%s", line);
	  fprintf(out, "ERROR\n");
	}
	else if (strlen(line) - 1 > 13 + k) { // if there is an additional number
	  fprintf(out, "%s", line);
          fprintf(out, "ERROR\n");
	}
	
	else {
	  fprintf(out, "GetOutDegree %d\n", u);
	  fprintf(out, "%d\n", getOutDegree(myDigraph, u));
	}
      }
    }

    if (addedge) { // if the input line is AddEdge
      bool twoDigits = true; // used to check if the input line includes 2 vertices
      int digit = 0; // will be used when the char is converted to an int
      int k = 0; // used to iterate through the input line
      char ch = *(line + 8); // set ch to be right after the chars 'AddEdge'
      
      if (!(((int) ch < 58) && ((int) ch > 47))) { // if there are no numbers provided
	fprintf(out, "%s", line);
	fprintf(out, "ERROR\n");
      }
      
      else { // there is at least one number provided
	while (((int) ch < 58) && ((int) ch > 47)) { // until k encounters a space 
	  digit = digit * 10 + (ch - 48); // convert ch to an int and add it to the existing value of digit
	  k++; // increment k
	  ch = *(line + 8 + k); // move onto the next digit
	}
	
	int u = digit; // set the first vertex to digit
	digit = 0; // reset digit for the next number
	ch = *(line + 9 + k); // set ch to be the first digit of the next number
	
	if (!(((int) ch < 58) && ((int) ch > 47))) { // if there is only one number provided
	  twoDigits = false; // there aren't two number provided
	  fprintf(out, "%s", line);
	  fprintf(out, "ERROR\n"); 
	}
	
	if (twoDigits) { //there are two digits provided
	  while (((int) ch < 58) && ((int) ch > 47)) { // until k encounters a space
	    digit = digit * 10 + (ch - 48); // convert ch to an int and add it to the existing value of digit
	    k++; // increment k
	            
	    if (k + 9 == strlen(line) - 1) { // if the program has reached the end of this input line
	      break;
	    }
	    ch = *(line + 9 + k); // move onto the next digit
	  }
	      
	  int v = digit; // set the second vertex to digit

	  if ((u > getOrder(myDigraph)) || (v > getOrder(myDigraph)) || u < 1 || v < 1) { // if u or v is greater than numVertices or if u or v are less than 1
	    fprintf(out, "%s", line);
	    fprintf(out, "ERROR\n");
	  }

	  else if (strlen(line) > 9 + k + 1) { // if there is an additional number
	    fprintf(out, "%s", line);
	    fprintf(out, "ERROR\n");
	  }
	      
	  else { // u and v are legal vertices
	    fprintf(out, "AddEdge %d %d\n", u, v);
	    fprintf(out, "%d\n", addEdge(myDigraph, u, v));
	  }
	}    
      }
    }

    if (deleteedge) { // if the input line is DeleteEdge
      bool twoDigits = true; // used to check if the input line includes 2 vertices
      int digit = 0; // will be used when the char is converted to an int
      int k = 0; // used to iterate through the input line
      char ch = *(line + 11); // set ch to be right after the chars 'DeleteEdge'
      
      if (!(((int) ch < 58) && ((int) ch > 47))) { // if there are no numbers provided
	fprintf(out, "%s", line);
	fprintf(out, "ERROR\n");
      }
      
      else { // there is at least one number provided
	while (((int) ch < 58) && ((int) ch > 47)) { // until k encounters a space 
	  digit = digit * 10 + (ch - 48); // convert ch to an int and add it to the existing value of digit
	  k++; // increment k
	  ch = *(line + 11 + k); // move onto the next digit
	}
	
	int u = digit; // set the first vertex to digit
	digit = 0; // reset digit for the next number
	ch = *(line + 12 + k); // set ch to be the first digit of the next number
	
	if (!(((int) ch < 58) && ((int) ch > 47))) { // if there is only one number provided
	  twoDigits = false; // there aren't two number provided
	  fprintf(out, "%s", line);
	  fprintf(out, "ERROR\n"); 
	}
	
	if (twoDigits) { //there are two digits provided
	  while (((int) ch < 58) && ((int) ch > 47)) { // until k encounters a space
	    digit = digit * 10 + (ch - 48); // convert ch to an int and add it to the existing value of digit
	    k++; // increment k
	            
	    if (k + 12 == strlen(line) - 1) { // if the program has reached the end of this input line
	      break;
	    }
	    ch = *(line + 12 + k); // move onto the next digit
	  }
	      
	  int v = digit; // set the second vertex to digit

	  if ((u > getOrder(myDigraph)) || (v > getOrder(myDigraph)) || u < 1 || v < 1) { // if u or v is greater than numVertices or if u or v are less than 1
	    fprintf(out, "%s", line);
	    fprintf(out, "ERROR\n");
	  }

	  else if(strlen(line) > 12 + k + 1) { // if there is an additional number
	    fprintf(out, "%s", line);
	    fprintf(out, "ERROR\n");
	  }
	      
	  else { // u and v are legal vertices
	    fprintf(out, "DeleteEdge %d %d\n", u, v);
	    fprintf(out, "%d\n", deleteEdge(myDigraph, u, v));
	  }
	}    
      }
    }

    if (getcountscc) { // if the input line is GetCountSCC                                                                                                                                                 
      if (strlen(line) > 12) { // if there are any characters after GetCountSCC                                                                                                                           
        fprintf(out, "%s", line);
	fprintf(out, "ERROR\n");
      }

      else {
        fprintf(out, "GetCountSCC\n");
        fprintf(out, "%d\n", getCountSCC(myDigraph));
      }
    }

    if (getnumsccvertices) { // if the input line is GetNumSCCVertices                                                                                                                                              
      int digit = 0; // will be used when the char is converted to an int                                                                                                                                  
      int k = 0; // used to iterate through the input line                                                                                                                                                 
      char ch = *(line + 18); // set ch to be right after the chars 'getNumSCCVertices'                                                                                                                         

      if (!(((int) ch < 58) && ((int) ch > 47))) { // if there is no number provided                                                                                                                       
        fprintf(out, "%s", line);
        fprintf(out, "ERROR\n");
      }

      else{ // there is a number provided                                                                                                                                                                  
        while (((int) ch < 58) && ((int) ch > 47)) { // until k encounters a space                                                                                                                         
          if (k + 18 == strlen(line) - 1) { // once the end of the input line as been reached                                                                                                              
            break;
          }
          digit = digit * 10 + (ch - 48); // convert ch to an int and add it to the existing value of digit                                                                                                
          k++; // increment k                                                                                                                                                                              
          ch = *(line + 18 + k); // move onto the next digit                                                                                                                                               
        }

        int u = digit; // set u to be the converted int                                                                                                                                                    
        if (u > getOrder(myDigraph) || u < 1) { // if u is greater than numVertices or less than 1                                                                                                         
          fprintf(out, "%s", line);
          fprintf(out, "ERROR\n");
        }

        else if (strlen(line) - 1 > 18 + k) { // if there is an additional number                                                                                                                          
          fprintf(out, "%s", line);
          fprintf(out, "ERROR\n");
        }

        else {
          fprintf(out, "GetNumSCCVertices %d\n", u);
	  fprintf(out, "%d\n", getNumSCCVertices(myDigraph, u));
        }
      }
    }

    if (insamescc) { // if the input line is InSameSCC
      bool twoDigits = true; // used to check if the input line includes 2 vertices
      int digit = 0; // will be used when the char is converted to an int
      int k = 0; // used to iterate through the input line
      char ch = *(line + 10); // set ch to be right after the chars 'InSameSCC'

      if (!(((int) ch < 58) && ((int) ch > 47))) { // if there are no numbers provided
        fprintf(out, "%s", line);
        fprintf(out, "ERROR\n");
      }

      else { // there is at least one number provided
        while (((int) ch < 58) && ((int) ch > 47)) { // until k encounters a space
          digit = digit * 10 + (ch - 48); // convert ch to an int and add it to the existing value of digit
          k++; // increment k
          ch = *(line + 10 + k); // move onto the next digit
        }

        int u = digit; // set the first vertex to digit
        digit = 0; // reset digit for the next number
        ch = *(line + 11 + k); // set ch to be the first digit of the next number

        if (!(((int) ch < 58) && ((int) ch > 47))) { // if there is only one number provided
          twoDigits = false; // there aren't two number provided
          fprintf(out, "%s", line);
          fprintf(out, "ERROR\n");
        }

        if (twoDigits) { //there are two digits provided
          while (((int) ch < 58) && ((int) ch > 47)) { // until k encounters a space
            digit = digit * 10 + (ch - 48); // convert ch to an int and add it to the existing value of digit
            k++; // increment k

            if (k + 11 == strlen(line) - 1) { // if the program has reached the end of this input line
              break;
            }
            ch = *(line + 11 + k); // move onto the next digit
          }

          int v = digit; // set the second vertex to digit
          if ((u > getOrder(myDigraph)) || (v > getOrder(myDigraph)) || u < 1 || v < 1) { // if u or v is greater than numVertices or if u or v are less than 1
            fprintf(out, "%s", line);
            fprintf(out, "ERROR\n");
          }

          else if(strlen(line) > 11 + k + 1) { // if there is an additional number
            fprintf(out, "%s", line);
            fprintf(out, "ERROR\n");
          }

          else { // u and v are legal vertices
            fprintf(out, "InSameSCC %d %d\n", u, v);
	    if (inSameSCC(myDigraph, u, v) == 0) {
	      fprintf(out, "NO\n");
	    }
	    else if (inSameSCC(myDigraph, u, v) == 1) {
	      fprintf(out, "YES\n");
	    }
	    else if (inSameSCC(myDigraph, u, v) == -1) {
	      fprintf(out, "ERROR\n");
	    }
          }
        }
      }
    }

    // set all the const chars to NULL
    printdigraph = NULL;
    getorder = NULL;
    getsize = NULL;
    getoutdegree = NULL;
    addedge = NULL;
    deleteedge = NULL;
    getcountscc = NULL;
    getnumsccvertices = NULL;
    insamescc = NULL;
  }
  
  freeDigraph(&myDigraph); // safely deallocate the heap memory used for the Digraph
  fclose(in);
  fclose(out);
  return (EXIT_SUCCESS);
}
