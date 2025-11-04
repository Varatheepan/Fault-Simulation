#include "graph.h"
/***************************************************************************************************************************
 Main Function(Serial and Parallel Fault Simulation)
****************************************************************************************************************************/
void main(int argc,char **argv)
{
FILE *fisc,*fvec,*ffau,*fres;             //file pointers used for .isc file, .vec file, .faults file and resultfile
int Max,Opt,Npi,Npo,Total,Tfs;              //maxnode id,option,tot no of PIs,tot no of Pos,Tot no of input patterns& faults in.vec in.faults
// int Num_IO[2];
int NumPrimInputs,NumPrimOutputs,NumVec, OutLineLen;

int vec_index = 0;
int input_index = 0;
int name_index = 0;

int NumFaults = 0;
int faultIndex = 0;
int isFaultDetected = 0;
char faultSring[20];

// int faultSim = 0;

NODE graph[Mnod];                         //structure used to store the ckt information in .isc file 
PATTERN vector[Mpt];                      //structure used to store the input vectors information in .vec file 

FAULT stuck[Mft];                      //structure used to store the faults information in .faults file
int a,b,c,d;                             //random variables


//Read the .isc file and store the information in graph structure
fisc=fopen(argv[1],"r");                           //file pointer to open .isc file 
Max=0; 
Max=ReadIsc(fisc,graph);                 //read .isc file and return index of last node in graph formed
fclose(fisc);                            //close file pointer for .isc file
// PrintCircuit(graph,Max);                 //print all members of graph structure

// Number of primaty inputs
NumPrimInputs = 0;

// Number of primaty outputs
NumPrimOutputs = 0;

// Extract number of primary inputs and outputs from the graph
getNum_PInputs_POutputs(graph, Max,&NumPrimInputs,&NumPrimOutputs);
printf("Num of Primary Inputs: %d, Primary outputs: %d \n",NumPrimInputs,NumPrimOutputs);

//Read the .vec file and store the information in  vector structure
fvec = fopen(argv[2],"r"); 
NumVec = ReadVecArr(fvec,NumPrimInputs, vector);

// Creating output file name 
char OutFileName[20];
while (name_index <= 20)
{
    if (argv[2][name_index] == '.'){
        OutFileName[name_index] = '.';
        name_index ++;
        OutFileName[name_index] = 'r';
        name_index ++;
        OutFileName[name_index] = 'e';
        name_index ++;
        OutFileName[name_index] = 's';
        name_index ++;
        break;
    }
    else {
        OutFileName[name_index] = argv[2][name_index];
    }
    name_index ++;
}

// Array to store one input output pattern 
char InOutPattern[Mlin*2];

fres = fopen(OutFileName,"w"); 

OutLineLen = NumPrimInputs +  NumPrimOutputs + 2;

// Read Fault list file
ffau = fopen(argv[3],"r"); 
NumFaults = ReadFaults(ffau,stuck);
printf("Num of faults: %d \n", NumFaults);

//Perform Logic Simulation for each Input vector and print the Pos .val in output file   
// for(vec_index=0;vec_index<NumVec;vec_index++)
for(faultIndex=0;faultIndex<NumFaults;faultIndex++)
{   
    if (faultIndex != 0){
        fputs("\n", fres);
    }

    //fault pattern to write in result file
    bzero(faultSring,strlen(faultSring)); 
    sprintf(faultSring,"%d/%d\n", stuck[faultIndex].NodeID,stuck[faultIndex].StuckVal);

    // Write the output to th output file
    fputs((char*)faultSring, fres);

    // printf("fault-free output: ");
    // Logic Simulation
    // logicSimulation(graph, Max, vec_index, vector, InOutPattern, -1, stuck, &isFaultDetected);

    // faultSim = 1;
    // for(faultIndex=0;faultIndex<NumFaults;faultIndex++)
    for(vec_index=0;vec_index<NumVec;vec_index++)
        {

            printf("Node %d stuck-at-%d   Input/output: ", stuck[faultIndex].NodeID,stuck[faultIndex].StuckVal);

            logicSimulation(graph, Max, vec_index, vector, InOutPattern, faultIndex, stuck, &isFaultDetected);
            
            // Write the output to th output file
            fputs((char*)InOutPattern, fres);

            if (isFaultDetected == 1){
                printf(" DETECTED\n");
                fputs(" DETECTED\n", fres);
            }
            else{
                printf(" NOT DETECTED\n");
                fputs(" NOT DETECTED\n", fres);
            }
            // printf("\n");

        }
        printf("\n");
}

// PrintCircuit(graph,Max);                 //print all members of graph structure
fclose(fres);                                                  //close file pointer for .out file

ClearCircuit(graph,Mnod);                                      //clear memory for all members of graph
//for(a=0;a<Total;a++){ bzero(vector[a].piv,Mpi); }                //clear memory for all members of vector
return;
}//end of main
/****************************************************************************************************************************/

