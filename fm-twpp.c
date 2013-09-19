#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NODE_LIMIT 10000
#define NET_LIMIT 10000
#define BLOCK_LIMIT 2 
struct gain{
	char T ;
	int g ;
};
typedef struct gain G ;

struct linkCounter{
	int index ;
	int count ;
} ;
typedef struct linkCounter LC ;
struct vertex {
	char block ;
	unsigned char locked  ;
	unsigned char selected ;
	int gain[BLOCK_LIMIT] ;
	int ldc;
	LC* ld;
};
typedef struct vertex V;
struct net{
	int ldsc ;
	int ldc; 
	LC* ld;
};
typedef struct net N;

struct move{
	int nodeIndex;
	char block ;
};
typedef struct move M ;

M movearr[NODE_LIMIT];
V vertexarr[NODE_LIMIT];
N netarr[NET_LIMIT];
int maxGain[NODE_LIMIT];
int sumsize ;

int computeCellGain( N * narr, V * varr, char F, char T);
M  getCellWithMaxGain(const V * varr, int way);

int main(int argc, char ** argv){
	M tempmove ;
	memset(vertexarr, 0, sizeof(V) * NODE_LIMIT);
	memset(netarr, 0, sizeof(N) * NET_LIMIT);
	memset(movearr, 0, sizeof(M) * NODE_LIMIT);
	if(argc == 2){
		FILE *input = NULL;
		int state = 0;
		int index, Vindex ; 
		int ldindex = 0 ;
		input = fopen(argv[1], "r");
		for(;;){
			char c = fgetc(input);
			int number ;
			if(c == 'N' || c == 'e' || c == 't' ){
				state = 0;
				continue;
			}else{if( c <= '9' && c >= '0' ){
				ungetc(c, input);
				fscanf(input, "%d", &number);
				switch(state){
				int i ;
				case 0:
				index = number ;
				break ;

				case 1:
				netarr[index].ldsc = number ;
				break ;

				case 2:
				ldindex =  netarr[index].ldc - 1;
				for(i = ldindex ; i >= 0  ; i--){
					if(netarr[index].ld[i].index == number){
						ldindex = i  ;
						break;
					}
				}
				if(i == -1){
					netarr[index].ld = (LC*) realloc(netarr[index].ld, sizeof(LC) * (netarr[index].ldc + 1)) ;
					netarr[index].ldc += 1 ;
					ldindex += 1 ;
					netarr[index].ld[ldindex].count = 0 ;
				}
				Vindex = netarr[index].ld[ldindex].index = number;
				netarr[index].ld[ldindex].count += 1 ;

				ldindex = vertexarr[Vindex].ldc - 1;
				for(i = ldindex; i>= 0 ; i--){
					if(vertexarr[Vindex].ld[i].index == index){
						ldindex = i ;
						break ;
					}
				}
				if(i== - 1){
					vertexarr[Vindex].ld = (LC*) realloc(vertexarr[Vindex].ld, sizeof(LC) * (vertexarr[Vindex].ldc + 1));
					vertexarr[Vindex].ldc += 1;
					ldindex += 1 ;
					vertexarr[Vindex].ld[ldindex].count = 0;
				}
				vertexarr[Vindex].ld[ldindex].index = index ;
				vertexarr[Vindex].ld[ldindex].count += 1;
				
				if(Vindex % 2 == 0){
					vertexarr[Vindex].block = 'A' ;
				}else{
					vertexarr[Vindex].block = 'B' ;
				}
				break ;

				default:
				break ;
				}
			}else{if( c == ':' ){
				state = 2; 
				continue;
			}else{if( c == '('){
				state = 1;
				ldindex = 0 ;
				continue;
			}else{if( c == ' ' || c == ')'){
				continue;
			}else{if(c == EOF){
				break;
			}}}}}}
		}
		sumsize = index ;
	}else{
		fprintf(stderr, "too few arguments\n");
		exit(EXIT_FAILURE);
	}

	int i = 0;
	for( i = 1; netarr[i].ldc != 0 ; i++){
		int j = 0;
		printf("Net %d(%d): ", i, netarr[i].ldc);
		for(j = 0; j < netarr[i].ldc; j++){
			printf("%d(%d) ", netarr[i].ld[j].index, netarr[i].ld[j].count);
		} 
		putchar('\n');
		
	}
	for( i = 1; vertexarr[i].ldc != 0 ; i++){
		int j = 0 ;
		printf("Node %d(%d)[%c]: ", i, vertexarr[i].ldc, vertexarr[i].block);
		for(j = 0; j < vertexarr[i].ldc; j++){
			printf("%d(%d) ", vertexarr[i].ld[j].index, vertexarr[i].ld[j].count);
		} 
		putchar('\n');
	}
	computeCellGain(netarr, vertexarr, 'A', 'B');
	computeCellGain(netarr, vertexarr, 'B', 'A');
	for(i = 0; ; i++){
		tempmove = getCellWithMaxGain(vertexarr, 2);
		if(tempmove.nodeIndex == 0 ){
			break ;
		}else{
			vertexarr[tempmove.nodeIndex].locked = 1 ;
			movearr[i] = tempmove ;
			updateGains();
		}
	}
}


int computeCellGain( N * narr, V * varr, char F, char T){
	int i, j, k;
	for( i = 1; varr[i].block != 0 ; i++){
		if(varr[i].block == F && !varr[i].locked){
			varr[i].gain[T - 'A'] = 0 ;	
			for( j = 0; j < varr[i].ldc; j++){
				int Fcount = 0, Tcount = 0;
				for(k = 0; k < narr[varr[i].ld[j].index].ldc ; k++){
					int index = narr[varr[i].ld[j].index].ld[k].index;
					if( varr[index].block == F){
						Fcount += 1 ;
					}else{if(varr[index].block == T){
						Tcount += 1 ;
					}}
				}
				if(Fcount == 1){
					varr[i].gain[T - 'A'] += 1;
//					printf("haha +1 , node [%d] and net (%d)\n", i, varr[i].ld[j].index);
				}else {if(Tcount == 0 ){
					varr[i].gain[T - 'A'] -= 1;
//					printf("oo:( -1 , node [%d] and net (%d)\n", i, varr[i].ld[j].index);
				}}
			}
			printf("   Gain of move [%d] from %c to %c: %d\n", i, F, T, varr[i].gain[T - 'A'] );
		}
	}
	return 0;
}
float ratioOfBlock(char block){
	switch(block){
	case 'A':
	return 0.5 ;

	case 'B':
	return 0.5 ;
	
	default:
	return -1.0;
	}
}
int sizeOfBlock(const V * varr, char block){
	int i , count = 0;
	for(i = 1; varr[i].block != 0 ; i++){
		if(varr[i].block == block){
			count += 1 ;
		}
	}
	return count ;
}
float freeCells(const V * varr){
	int i , count = 0;
	for(i = 1; varr[i].block != 0; i++){
		if(varr[i].locked){
			count += 1 ;
		}
	}
	return count  ;
}
int balanceCriterion(const V * varr, int V, char F, char T){
	if( 
	(int)(ratioOfBlock(F) * V - freeCells(varr)/5.0) <= sizeOfBlock(varr, F) &&
	(int)(ratioOfBlock(F) * V + freeCells(varr)/5.0) >= sizeOfBlock(varr, F) &&
	(int)(ratioOfBlock(T) * V - freeCells(varr)/5.0) <= sizeOfBlock(varr, T) &&
	(int)(ratioOfBlock(T) * V + freeCells(varr)/5.0) >= sizeOfBlock(varr, T) ){
		return 1 ;
	}else{
		return 0;
	}
}
M getCellWithMaxGain(const V * varr, int way){
	int i ,j;
	int maxGain = -NET_LIMIT, maxIndex = 0;
	char maxBlock ;
	M move;
	for(i = 1; varr[i].block != 0 ; i++){
		if(varr[i].locked){continue;}
		for(j = 0;j < way && j != varr[i].block - 'A'; j++){
			if(varr[i].gain[j] > maxGain && balanceCriterion(varr, 20, varr[i].block, j + 'A') ){
				maxGain = varr[i].gain[j] ;
				maxIndex = i ;
				maxBlock = j + 'A'; 
			}else{if(varr[i].gain[j] == maxGain && balanceCriterion(varr, 20, varr[i].block, j + 'A')){
				if(sizeOfBlock(varr, varr[i].block) - sizeOfBlock(varr, j - 'A') > 
				sizeOfBlock(varr, varr[maxIndex].block) - sizeOfBlock(varr, maxBlock)){
					maxIndex = i ;
					maxBlock = j + 'A' ;
				}
			}}
		}
	}
	move.nodeIndex = maxIndex ;
	move.block = maxBlock ;
	
	return move;
}
int updateGains(V * varr){
}
/*
int getBlockSize(unsigned char block, const V * varr){
	int i ;
	for(i = 1; varr[i].block == block && i <= sumsize ; i++){}
	return i ;
}
float getBlockRatio(unsigned char block){
	switch(block){
	case 'A': return 0.5 ; 
	case 'B': return 0.5 ;
	default : break ;
	}
}
int  selectCell(const V * varr){
	int i = 1;
	int baseCellIndex = getCellWithMaxGain(varr) ;
	if(getBlockSize(varr[baseCellIndex].block, varr) - 1 >= getBlockRatio(varr[baseCellIndex].block) * sumsize){
	}
}
*/
