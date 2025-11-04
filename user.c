#include "graph.h"

// using namespace std;
int AND_LT[5][5] = {
  // 0,1,x,D,DB
    {0,0,0,0,0},    //0
    {0,1,x,D,DB},    //1
    {0,x,x,x,x},    //x
    {0,D,x,D,0},    //D
    {0,DB,x,0,DB}     //DB
    };

int OR_LT[5][5] = {
  // 0,1,x,D,DB
    {0,1,x,D,DB},    //0
    {1,1,1,1,1},    //1
    {x,1,x,x,x},    //x
    {D,1,x,D,1},    //D
    {DB,1,x,1,DB}     //DB
    };  

int NOT_LT[5] = {1,0,x,DB,D};

int XOR_LT[3][3] = {
    {0,1,x},
    {1,0,x},
    {x,x,x}
    };

// This function will exctract the number of primary inputs and primary outputs from the graph
void getNum_PInputs_POutputs(NODE *graph, int Max, int *PrimInputs, int *PrimOutputs)
{
    int i;
    for(i=1;i<=Max;i++){
        if(graph[i].Type!=0){
            if(graph[i].Type==1){
               (*PrimInputs)++;
            }
            else if(graph[i].Nfo == 0){
                (*PrimOutputs)++;
            }
        }
    }
}

int ReadVecArr(FILE *fvec,int PrimInputs, PATTERN *PatternsVector)
{
    int i,VecIndex,lenVec;
    lenVec = PrimInputs+2;
    char Inputs[lenVec];

    VecIndex = 0;
    while(fgets(Inputs, lenVec, fvec)) {
        for (i=0;i<PrimInputs;i++){
            if (Inputs[i] == 'x'){
                PatternsVector[VecIndex].Pattern[i] = 2;
            } else if (Inputs[i] == '1'){
                PatternsVector[VecIndex].Pattern[i] = 1;
            } else if (Inputs[i] == '0'){
                PatternsVector[VecIndex].Pattern[i] = 0;
            }      
        }
        VecIndex++;
    }
    return VecIndex;
}

// Logic simulation
void logicSimulation(NODE *graph, int max, int VecIndex, PATTERN *PatternsVector, char *InOutPattern, int faultIndex, FAULT *Fault, int *isFaultDetetcted )
{
    int node_index;
    LIST *tempFin; 
    int PatternIndex = 0;
    int input1,input2,output;
    int firstOut = 0;

    int InOutPatIdx = 0;
    int printIdx = 0;

    *isFaultDetetcted = 0;

    for(node_index=1;node_index<=max;node_index++){
        if(graph[node_index].Type!=0){

            // Assign a pattern to the inputs
            if(graph[node_index].Type==INPT){
                // printf("%d",PatternsVector[VecIndex].Pattern[PatternIndex]);
                if (PatternsVector[VecIndex].Pattern[PatternIndex] == 2){
                    InOutPattern[InOutPatIdx] = '2';

                }
                else if (PatternsVector[VecIndex].Pattern[PatternIndex] == 1){
                    InOutPattern[InOutPatIdx] = '1';
                }
                else{  
                    InOutPattern[InOutPatIdx] = '0';

                }
                InOutPatIdx ++;                
            
                graph[node_index].Cval = PatternsVector[VecIndex].Pattern[PatternIndex];
                PatternIndex ++;
            }

            //  Assign outputs of FROM gates 
            else if (graph[node_index].Type==FROM)
            {
                tempFin = graph[node_index].Fin;
                graph[node_index].Cval = graph[tempFin->id].Cval;   
            }

            //  Assign outputs of BUFF gates 
            else if (graph[node_index].Type==BUFF)
            {
                tempFin = graph[node_index].Fin;
                graph[node_index].Cval = graph[tempFin->id].Cval;   
            }

            else if (graph[node_index].Type==AND)
            {
                tempFin = graph[node_index].Fin;

                graph[node_index].Cval = graph[tempFin->id].Cval;
                tempFin = tempFin->next; 
                while(tempFin!=NULL){
                    input1 = graph[tempFin->id].Cval;
                    graph[node_index].Cval = AND_LT[input1][graph[node_index].Cval];   
                    tempFin = tempFin->next; 
                    }
            }

            else if (graph[node_index].Type==NAND)
            {
                // TODO: have to loop until the number of inputs
                tempFin = graph[node_index].Fin;

                graph[node_index].Cval = graph[tempFin->id].Cval;
                tempFin = tempFin->next; 
                while(tempFin!=NULL){
                    input1 = graph[tempFin->id].Cval;
                    graph[node_index].Cval = AND_LT[input1][graph[node_index].Cval];   
                    tempFin = tempFin->next; 
                    }
                graph[node_index].Cval= NOT_LT[graph[node_index].Cval];
            }

            else if (graph[node_index].Type==OR)
            {
                // TODO: have to loop until the number of inputs
                tempFin = graph[node_index].Fin;

                graph[node_index].Cval = graph[tempFin->id].Cval;
                tempFin = tempFin->next; 
                while(tempFin!=NULL){
                    input1 = graph[tempFin->id].Cval;
                    graph[node_index].Cval = OR_LT[input1][graph[node_index].Cval];   
                    tempFin = tempFin->next; 
                    }
            }

            else if (graph[node_index].Type==NOR)
            {
                // TODO: have to loop until the number of inputs
                tempFin = graph[node_index].Fin;

                graph[node_index].Cval = graph[tempFin->id].Cval;
                tempFin = tempFin->next; 
                while(tempFin!=NULL){
                    input1 = graph[tempFin->id].Cval;
                    graph[node_index].Cval = OR_LT[input1][graph[node_index].Cval];   
                    tempFin = tempFin->next; 
                    }
                graph[node_index].Cval= NOT_LT[graph[node_index].Cval];
            }

            //  Assign outputs of NOT gates 
            else if (graph[node_index].Type==NOT)
            {
                tempFin = graph[node_index].Fin;
                graph[node_index].Cval = NOT_LT[graph[tempFin->id].Cval];   
            }

            else if (graph[node_index].Type==XOR)
            {
                // TODO: have to loop until the number of inputs
                tempFin = graph[node_index].Fin;

                graph[node_index].Cval = graph[tempFin->id].Cval;
                tempFin = tempFin->next; 
                while(tempFin!=NULL){
                    input1 = graph[tempFin->id].Cval;
                    graph[node_index].Cval = XOR_LT[input1][graph[node_index].Cval];   
                    tempFin = tempFin->next; 
                    }
            }

            else if (graph[node_index].Type==XNOR)
            {
                // TODO: have to loop until the number of inputs
                tempFin = graph[node_index].Fin;

                graph[node_index].Cval = graph[tempFin->id].Cval;
                tempFin = tempFin->next; 
                while(tempFin!=NULL){
                    input1 = graph[tempFin->id].Cval;
                    graph[node_index].Cval = XOR_LT[input1][graph[node_index].Cval];   
                    tempFin = tempFin->next; 
                    }
                graph[node_index].Cval= NOT_LT[graph[node_index].Cval];
            }

            // Set the fault value if specified
            // faultIndex -1 is fault-free simulation
            if (faultIndex != -1 && graph[node_index].id == Fault[faultIndex].NodeID){
                // printf("fault %d stuck at %d   ",graph[node_index].id,Fault[faultIndex].StuckVal);
                if (Fault[faultIndex].StuckVal == 0 && graph[node_index].Cval == 1){
                    graph[node_index].Cval = D;
                    
                }
                else if (Fault[faultIndex].StuckVal == 1 && graph[node_index].Cval == 0){
                    graph[node_index].Cval = DB;
                }
                else {
                    graph[node_index].Cval = Fault[faultIndex].StuckVal;
                }
            } 
            
            
            // Print the output
            if (graph[node_index].Po==1){
                               
                // For the first output, add a space infront to seperate from the inputs
                if (firstOut == 0){

                    if (graph[node_index].Cval == 2){
                        InOutPattern[InOutPatIdx] = ' ';
                        InOutPatIdx ++;
                        InOutPattern[InOutPatIdx] = '2';

                    }
                    else if (graph[node_index].Cval == 1){
                        InOutPattern[InOutPatIdx] = ' ';
                        InOutPatIdx ++;
                        InOutPattern[InOutPatIdx] = '1';
                    }
                    else if (graph[node_index].Cval == 3){
                        InOutPattern[InOutPatIdx] = ' ';
                        InOutPatIdx ++;
                        InOutPattern[InOutPatIdx] = '3'; 
                        *isFaultDetetcted = 1;
                    }
                    else if (graph[node_index].Cval == 4){
                        InOutPattern[InOutPatIdx] = ' ';
                        InOutPatIdx ++;
                        InOutPattern[InOutPatIdx] = '4'; 
                        *isFaultDetetcted = 1;
                    }
                    else{
                        InOutPattern[InOutPatIdx] = ' ';
                        InOutPatIdx ++;
                        InOutPattern[InOutPatIdx] = '0'; 
                    }
                    firstOut ++;
                }                 
                else{
                    if (graph[node_index].Cval == 2){
                        InOutPattern[InOutPatIdx] = '2';
                    }
                    else if (graph[node_index].Cval == 1){
                        InOutPattern[InOutPatIdx] = '1';
                    }
                    else if (graph[node_index].Cval == 3){                       
                        InOutPattern[InOutPatIdx] = '3';
                        *isFaultDetetcted = 1;
                    }
                    else if (graph[node_index].Cval == 4){                       
                        InOutPattern[InOutPatIdx] = '4';
                        *isFaultDetetcted = 1;
                    }
                    else{                       
                        InOutPattern[InOutPatIdx] = '0';
                    }
                }
                InOutPatIdx ++;
            }
        }
        
    }
    for(printIdx=0;printIdx<InOutPatIdx;printIdx++){
        printf("%c",InOutPattern[printIdx]);
    }
}


// Fault simulation related functions
int ReadFaults(FILE *ffau,FAULT *FaultVector){
    int i,FaultIndex;
    char line[Mfln];
    char templine[Mfln];
    int tempIndex;

    int ID, stuckVal;
    ID = 0;
    stuckVal = 0;
    // tempIndex = 0;

    FaultIndex = 0;
    while(fgets(line, Mfln, ffau)) {
        tempIndex = 0;
        for(i=0;i<Mfln;i++){
            if(line[i] != ' '){
                templine[tempIndex] = line[i];
                tempIndex ++;
            }
        }
        sscanf(templine, "%d/%d",&ID,&stuckVal); 
            FaultVector[FaultIndex].NodeID = ID;  
            FaultVector[FaultIndex].StuckVal = stuckVal;  
        FaultIndex++;
    }
    return FaultIndex;
}