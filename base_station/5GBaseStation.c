/*
 ============================================================================
 Name        : 5GBaseStation.c
 Author      : phill king
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/time.h>

typedef struct Point{
	int x;
	int y;
} Point;
#define LOCAL

#define MAXSIZE 2048
static int graph[MAXSIZE][MAXSIZE];
static int dir[4][2] = {{-1,0}, {1,0},{0,1},{0,-1}};


static int adj[2040200][5];

int preProcessor(Point* places, int size,Point** res, int resIndex){
	for(int i=0; i<size; i++){
		Point* p = &places[i];
		if(p->x == 0) continue;
		int count = 0;
		Point next;
		for(int j=0; j<4; j++){
			int nx = p->x + dir[j][0];
			int ny = p->y + dir[j][1];
			if(graph[nx][ny] != 0){
				count++;
				if(count>1) break;
				next.x = nx;
				next.y = ny;
			}
		}

		if(count == 1){
			res[resIndex][0] = *p;
			res[resIndex][1] = next;
			resIndex++;
			p->x = 0;
			int index = graph[next.x][next.y]-1;
			places[index].x = 0;
			graph[p->x][p->y] = 0;
			graph[next.x][next.y] = 0;
		}else if(count == 0){
			res[resIndex++][0] = *p;
			p->x = 0;
			graph[p->x][p->y] = 0;
		}
	}
	//	printf("count:%d\n", resIndex);
	for(int i=size-1; i>=0; i--){
		Point* p = &places[i];
		if(p->x == 0) continue;
		int count = 0;
		Point next;
		for(int j=0; j<4; j++){
			int nx = p->x + dir[j][0];
			int ny = p->y + dir[j][1];
			if(graph[nx][ny] != 0){
				count++;
				if(count>1) break;
				next.x = nx;
				next.y = ny;
			}

		}

		if(count == 1){
			res[resIndex][0] = *p;
			res[resIndex][1] = next;
			resIndex++;
			p->x = 0;
			int index = graph[next.x][next.y]-1;
			places[index].x = 0;
			graph[p->x][p->y] = 0;
			graph[next.x][next.y] = 0;
		}else if(count == 0){
			res[resIndex++][0] = *p;
			p->x = 0;
			graph[p->x][p->y] = 0;
		}
	}
	//	printf("count:%d\n", resIndex);
	return resIndex;
}

Point** getMinStations(Point* places, const int size,  int* returnSize){
	//	memset(graph, 0, sizeof (graph));
	for(int i=0; i<size; i++){
		graph[places[i].x][places[i].y] = i+1;
	}

	// allocate memory for return 2d array
	Point **res = (Point**) malloc(size * sizeof(Point*));
	for(int i=0; i<size; i++){
		res[i] = (Point*) calloc(2, sizeof(Point));

	}
	int resIndex = 0;

	// pre-processor

	//	resIndex = preProcessor(places, size, res, resIndex);
	for(int i=0; i<size; i++){
		Point* p = &places[i];
		if(p->x == 0) continue;
		int count = 0;
		Point next;
		for(int j=0; j<4; j++){
			int nx = p->x + dir[j][0];
			int ny = p->y + dir[j][1];
			if(graph[nx][ny] != 0){
				count++;
				if(count>1) break;
				next.x = nx;
				next.y = ny;
			}
		}

		if(count == 1){
			res[resIndex][0] = *p;
			res[resIndex][1] = next;
			resIndex++;
			p->x = 0;
			int index = graph[next.x][next.y]-1;
			places[index].x = 0;
			graph[p->x][p->y] = 0;
			graph[next.x][next.y] = 0;
		}else if(count == 0){
			res[resIndex++][0] = *p;
			p->x = 0;
			graph[p->x][p->y] = 0;
		}
	}
	//	printf("count:%d\n", resIndex);
	for(int i=size-1; i>=0; i--){
		Point* p = &places[i];
		if(p->x == 0) continue;
		int count = 0;
		Point next;
		for(int j=0; j<4; j++){
			int nx = p->x + dir[j][0];
			int ny = p->y + dir[j][1];
			if(graph[nx][ny] != 0){
				count++;
				if(count>1) break;
				next.x = nx;
				next.y = ny;
			}
		}

		if(count == 1){
			res[resIndex][0] = *p;
			res[resIndex][1] = next;
			resIndex++;
			p->x = 0;
			int index = graph[next.x][next.y]-1;
			places[index].x = 0;
			graph[p->x][p->y] = 0;
			graph[next.x][next.y] = 0;
		}else if(count == 0){
			res[resIndex++][0] = *p;
			p->x = 0;
			graph[p->x][p->y] = 0;
		}
	}
	//	printf("count:%d\n", resIndex);


	//
	int* left = (int*) malloc(size*sizeof(int));
	int* right = (int*) malloc(size*sizeof(int));
	left[0] = 0;
	right[0] = 0;
	int leftSize = 1;
	int rightSize = 1;
	for(int i=0; i<size; i++){
		if(places[i].x == 0) {
			continue;
		}
		if( (places[i].x+places[i].y)%2 == 0){
			left[leftSize++] = i;
		}else{
			right[rightSize++] = i;
		}
	}

	for(int i=1; i<rightSize; i++){
		const Point* p = &places[right[i]];
		graph[p->x][p->y] = i;
	}

	for(int i=1; i<leftSize; i++){
		const Point* p = &places[left[i]];
		for(int j=0; j<4; j++){
			int nx = p->x + dir[j][0];
			int ny = p->y + dir[j][1];
			if(graph[nx][ny]!=0){
				adj[i][0]++;
				int index = adj[i][0];
				adj[i][index] = graph[nx][ny];
			}
		}
	}

	int* matchLeft = (int*) calloc(leftSize, sizeof(int));
	int* matchRight = (int*) calloc(rightSize, sizeof(int));
	int* visited = (int*) calloc(rightSize, sizeof(int));
	int* mark = (int*) calloc(leftSize, sizeof(int));
	int* q = (int*) malloc(rightSize * sizeof(int));
	int* prev = (int*) malloc(leftSize * sizeof(int));
	for(int i=1; i<leftSize; i++){
		if(matchLeft[i] != 0) continue;
		int head = 0;
		int tail = 0;
		q[tail++] = i;
		prev[i] = -1;
		int flag = 0;
		while(!flag && (head<tail) ){
			const int u = q[head++];
			for(int j=1;!flag && j<=adj[u][0]; j++){
				const int v = adj[u][j];
				if(visited[v] != i && mark[visited[v]]==0){
					visited[v] = i;
					q[tail++] = matchRight[v];
					if(matchRight[v] == 0){
						flag = 1;
						int d = u;
						int e = v;
						while(d!=-1){
							int tmp = matchLeft[d];
							matchRight[e] = d;
							matchLeft[d] = e;
							e = tmp;
							d = prev[d];

						}
					}else{
						prev[matchRight[v]] = u;
					}
				}
			}
		}
		if(flag == 0){
			mark[i] = 1;
		}
	}

	// collect the result
	for(int i=1; i<leftSize; i++){
		res[resIndex][0] = places[left[i]];
		if(matchLeft[i] != 0){
			res[resIndex][1] = places[right[matchLeft[i]]];
		}
		resIndex++;
	}
	for(int i=1; i<rightSize; i++){
		if(matchRight[i] == 0){
			res[resIndex++][0] = places[right[i]];
		}
	}

	free(q);
	free(prev);
	free(matchLeft);
	free(matchRight);
	free(visited);
	free(mark);
	*returnSize = resIndex;


	return res;
}

int main(int argc, char* argv[]) {

	const char* file_name = "data.txt";

	if(argc == 2)
	{
		file_name = argv[1];
	}
	char outputFile[128] = {0};
	strcpy(outputFile, file_name);

	char* tmp = strrchr(outputFile, '.');
	sprintf(tmp, ".out");


#ifdef LOCAL
	freopen(file_name, "r", stdin);
	//	freopen(outputFile, "w", stdout);
	FILE* fp = fopen(outputFile, "w");
#endif

	int total = 0;
	scanf("%d", &total);
	Point* places = (Point* )malloc(total * sizeof(Point));

	int px = 0;
	int py = 0;
	int index = 0;
	int n = total;
	while(n--){
		scanf("%d,%d",&px, &py);
		Point p;
		p.x = px;
		p.y = py;
		places[index++] = p;
	}
	//	printf("%d\n", total);
	//	for(int i=0; i<total; i++){
	//		printf("%d, %d\n", places[i].x, places[i].y);
	//	}

	struct timeval start, end;

	// start timer.
	gettimeofday(&start, NULL);
	int num = 0;
	Point** res = getMinStations(places, total, &num);


	// stop timer.
	gettimeofday(&end, NULL);

	// Calculating total time taken by the program.
	double time_taken;

	time_taken = (end.tv_sec - start.tv_sec) * 1e6;
	time_taken = (time_taken + (end.tv_usec -
			start.tv_usec)) * 1e-6;
	printf("It takes %f seconds.\n", time_taken);
	//	printf("%d\n", num);
	//	for(int i=0; i<num; i++){
	//		for(int j=0; j<2; j++){
	//			if(res[i][j].x<=0 || res[i][j].x>=10000 || res[i][j].y<=0 || res[i][j].y>=10000){
	//				continue;
	//			}
	//			printf("%d,%d;", res[i][j].x, res[i][j].y);
	//		}
	//		printf("\n");
	//	}

#ifdef LOCAL
	fprintf(fp,"%d\n", num);
	for(int i=0; i<num; i++){
		for(int j=0; j<2; j++){
			if(res[i][j].x<=0 || res[i][j].x>=10000 || res[i][j].y<=0 || res[i][j].y>=10000){
				continue;
			}
			fprintf(fp,"%d,%d;", res[i][j].x, res[i][j].y);
		}
		fprintf(fp, "\n");
	}
	fclose(fp);
#endif
	fflush(stdin);
	free(places);
	for(int i=0; i<num; i++){
		free(res[i]);
	}
	free(res);

	return EXIT_SUCCESS;
}
