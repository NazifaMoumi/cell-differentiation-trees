#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <dirent.h>
#include <omp.h>
#include <limits.h>
#include <ctype.h>
//#define NUM_OF_FILES 6
#define NUM_DIGITS 2
#define NUM_OF_CORES 40
#define NUM_CHR 25
#define RES_FILE "om_distMatrix.txt"

typedef struct elem {
	int chrno;
	long long startPeak, endPeak;
	struct elem *next;
} Elem;

typedef struct list {
	long long size;
	Elem *head, *tail;
} List;

long chrLengths [] = {249250621, 243199373, 198022430, 191154276, 180915260, 171115067, 159138663, 146364022, 141213431, 135534747, 135006516, 133851895, 115169878, 107349540, 102531392, 90354753, 81195210, 78077248, 59128983, 63025520, 48129895, 51304566, 155270560, 59373566, 16571};

long long *skipChrLengths;
char **fileNames;

long long maxListSize = 0, peakRegionSize;
List **pdata;
Elem **pointers;
int **peakRegion;
long long **distMatrix;
int NUM_OF_FILES;

void initSkipChrLengths() {
	int i;
	skipChrLengths = calloc(NUM_CHR, sizeof(long long));

	skipChrLengths[0] = 0;
	for(i = 1; i < NUM_CHR; i++) {
		skipChrLengths[i] = skipChrLengths[i - 1] + chrLengths[i - 1];
	}
}

int findFilePos(const char *fileName) {
	int i = 0;
	char *pos = calloc(NUM_DIGITS, sizeof(char));
	while(isdigit(fileName[i])) {
		pos[i] = fileName[i];
		i++;
	}
	i = atoi(pos) - 1;
	free(pos);
	return i;
}


void initFilenames() {
	FILE *fp = fopen("file_sequence.txt", "w");
	int i, lens, flag=0;
	char *temp;
	int n, cn=0;
	DIR *dir = opendir(".");
	fileNames = calloc(NUM_OF_FILES, sizeof(char *));
	struct dirent **namelist;
	n = scandir(".", &namelist, 0, alphasort);
	if (n < 0)
        perror("scandir");
	else {
        for (i = 0; i < n; i++) {
	    	temp = namelist[i]->d_name;
		lens = strlen(temp);
		if (lens > 4) {
			if ( ((temp[lens-4] == '.') && (temp[lens-3] == 'b') && (temp[lens-2] == 'e') && (temp[lens-1] == 'd')) ||
			((temp[lens-4] == '.') && (temp[lens-3] == 'B') && (temp[lens-2] == 'E') && (temp[lens-1] == 'D')) )
			{
				fileNames[cn] = calloc(lens+5, sizeof(char));
				strcpy(fileNames[cn], temp);
				printf("filename:   %s \n", fileNames[cn]);
				fprintf(fp, "%d\t", cn+1);
				fprintf(fp, "%s\n", fileNames[cn]);
				cn = cn + 1;
			}
	        }
            free(namelist[i]);
            }
        }
	fclose(fp);
 	/*for(i = 0; i < NUM_OF_FILES; i++) {
		int pos;
		struct dirent* file = readdir(dir);
		printf("\nhere %d", file->d_name[0]);
		if(!isdigit(file->d_name[0])) {
			i--;
			continue;
		}

		pos = findFilePos(file->d_name);
		fileNames[pos] = calloc(5 + strlen(file->d_name) + 1, sizeof(char));
		//strcat(fileNames[pos], "data/");
		printf("\n%s", file->d_name);
		strcat(fileNames[pos], file->d_name);
		printf("\n%s", fileNames[pos]);
	}*/
	closedir(dir);
}

/*void initFilenames() {
	int i;
	DIR *dir = opendir("data");
	fileNames = calloc(NUM_OF_FILES, sizeof(char *));

	for(i = 0; i < NUM_OF_FILES; i++) {
		int pos;
		struct dirent* file = readdir(dir);
		if(!isdigit(file->d_name[0])) {
			i--;
			continue;
		}

		pos = findFilePos(file->d_name);
		fileNames[pos] = calloc(5 + strlen(file->d_name) + 1, sizeof(char));
		strcat(fileNames[pos], "data/");
		strcat(fileNames[pos], file->d_name);
	}
	closedir(dir);
}*/

void initListsAndMatrices() {
	int i;
	pdata = calloc(NUM_OF_FILES, sizeof(List *));
	peakRegion = calloc(NUM_OF_FILES, sizeof(int *));
	pointers = calloc(NUM_OF_FILES, sizeof(Elem *));
	distMatrix = calloc(NUM_OF_FILES, sizeof(long long *));

	for(i = 0; i < NUM_OF_FILES; i++) {
		pdata[i] = malloc(sizeof(List));
		pdata[i]->size = 0;
		pdata[i]->head = NULL;

		distMatrix[i] = calloc(NUM_OF_FILES, sizeof(long long));
	}
}

int stringToInt(char *str, int *pos) {
	int res = 0;
	while(str[*pos] == ' ' || str[*pos] == '\t') {
		(*pos)++;
	}
	while(isdigit(str[*pos])) {
		res = res * 10 + str[*pos] - '0';
		(*pos)++;
	}
	return res;
}

long stringToLong(char *str, int *pos) {
	long res = 0;
	while(str[*pos] == ' ' || str[*pos] == '\t') {
		(*pos)++;
	}
	while(isdigit(str[*pos])) {
		res = res * 10 + str[*pos] - '0';
		(*pos)++;
	}

	if(str[*pos] == '.') {
		int dec = 0, numDigits = 0;
		(*pos)++;

		while(isdigit(str[*pos])) {
			numDigits++;
			dec = dec * 10 + str[*pos] - '0';
			(*pos)++;
		}

		if(str[*pos] == 'e') {
			int deg;
			(*pos)++; (*pos)++;
			deg = stringToInt(str, pos);

			res = res * pow(10, deg) + dec * pow(10, deg - numDigits);
		} else {
			exit(-1);
		}
	} else if(str[*pos] == 'e') {
		int deg;
		(*pos)++; (*pos)++;
		deg = stringToInt(str, pos);

		res = res * pow(10, deg);
	}

	return res;
}

void addElemToList(List *l, Elem *newElem) {
	if(l->head == NULL) {
		l->head = newElem;
	} else {
		l->tail->next = newElem;
	}
	l->tail = newElem;
	(l->size)++;
}

void createPeaks() {
	int fileno;

	omp_set_num_threads(NUM_OF_CORES);
	#pragma omp parallel for schedule(static, 1)
	for(fileno = 0; fileno < NUM_OF_FILES; fileno++) {
		List *l;
		char line [100];
		FILE *fp = fopen(fileNames[fileno], "r");
		l = pdata[fileno];

		while(fgets(line, 100, fp) != NULL) {
			//printf("%d out of %d \n", omp_get_thread_num(), omp_get_num_threads()); fflush(stdout);
			int chrno, i = 4, flagM = 0;

			if(line[3] == 'X') {
				chrno = 23;
			} else if(line[3] == 'Y') {
				flagM = 1;
			} else if(line[3] == 'M') {
				flagM = 1;
			} else {
				i = 3;
				chrno = stringToInt(line, &i);
			}

			if(flagM == 0) {
				long long skip;
				Elem *newElem = malloc(sizeof(Elem));

				skip = skipChrLengths[chrno - 1];

				newElem->chrno = chrno;
				newElem->startPeak = skip + stringToLong(line, &i);
				newElem->endPeak = skip + stringToLong(line, &i);
				newElem->next = NULL;

				addElemToList(l, newElem);
			}
		}
		fclose(fp);
	}

	for(fileno = 0; fileno < NUM_OF_FILES; fileno++) {
		if(pdata[fileno]->size > maxListSize) {
			maxListSize = pdata[fileno]->size;
		}
	}

	for(fileno = 0; fileno < NUM_OF_FILES; fileno++) {
		peakRegion[fileno] = calloc(10 * maxListSize, sizeof(int));
		pointers[fileno] = pdata[fileno]->head;
	}
}

List **initListOfLists() {
	int i;
	List **l = calloc(NUM_OF_FILES, sizeof(List *));
	for(i = 0; i < NUM_OF_FILES; i++) {
		l[i] = malloc(sizeof(List));
		l[i]->size = 0;
		l[i]->head = NULL;
	}
	return l;
}

void freeList(List *l) {
	Elem *prev = NULL, *cur = l->head;
	while(cur != NULL) {
		prev = cur;
		cur = cur->next;
		free(prev);
	}
	free(l);
}

void freeListOfLists(List **l) {
	int i;
	for(i = 0; i < NUM_OF_FILES; i++) {
		freeList(l[i]);
	}
	free(l);
}

long long createPeakRegions() {
	long long countIR = 0;
	int i;

	while(1) {
		long long IRStart, IREnd, IREndOld;
		int IRChrno;
		long long *localMaxs = calloc(NUM_OF_FILES, sizeof(long long));
		List **lists = initListOfLists();

		if(pointers[0] != NULL) {
			IRStart = pointers[0]->startPeak;
			IREnd = pointers[0]->endPeak;
			IRChrno = pointers[0]->chrno;
		} else {
			IRStart = LONG_MAX;
			IREnd = LONG_MAX;
			IRChrno = INT_MAX;
		}

		for(i = 1; i < NUM_OF_FILES; i++) {
			if(pointers[i] != NULL && ((IRStart > pointers[i]->startPeak && pointers[i]->chrno == IRChrno) || (pointers[i]->chrno < IRChrno))) {
				IRStart = pointers[i]->startPeak;
				IREnd = pointers[i]->endPeak;
				IRChrno = pointers[i]->chrno;
			}
		}

		IREndOld = -1;
		while(IREnd != IREndOld) {
			long long max;
			IREndOld = IREnd;

			omp_set_num_threads(NUM_OF_CORES);
			#pragma omp parallel for schedule(static, 1)
			for(i = 0; i < NUM_OF_FILES; i++) {
				while(pointers[i] != NULL && IRChrno == pointers[i]->chrno && ((IRStart <= pointers[i]->startPeak && IREnd >= pointers[i]->startPeak) || (IRStart <= pointers[i]->endPeak && IREnd >= pointers[i]->endPeak) || (IRStart >= pointers[i]->startPeak && IREnd <= pointers[i]->endPeak))) {
					Elem *newElem = malloc(sizeof(Elem));
					newElem->startPeak = pointers[i]->startPeak;
					newElem->endPeak = pointers[i]->endPeak;
					newElem->next = NULL;
					addElemToList(lists[i], newElem);

					if(localMaxs[i] < newElem->endPeak) {
						localMaxs[i] = newElem->endPeak;
					}

					pointers[i] = pointers[i]->next;
				}

				while(pointers[i] != NULL && IRChrno == pointers[i]->chrno && IRStart > pointers[i]->startPeak && IRStart > pointers[i]->endPeak) {
					pointers[i] = pointers[i]->next;
				}
			}

			max = 0;
			for(i = 0; i < NUM_OF_FILES; i++) {
				if(max < localMaxs[i]) {
					max = localMaxs[i];
				}
			}
			IREnd = max;
		}

		for(i = 0; i < NUM_OF_FILES; i++) {
			if(lists[i]->head != NULL) {
				peakRegion[i][countIR] = 1;
			}
		}

		countIR++;
		free(localMaxs);
		freeListOfLists(lists);

		for(i = 0; i < NUM_OF_FILES; i++) {
			if(pointers[i] != NULL) {
				break;
			}
		}

		if(i == NUM_OF_FILES) {
			break;
		}
	}

	return countIR;
}

void writePeakRegions() {
	int i;
	long long j;
	FILE *fp = fopen("overlap_datarepresentation_interesting_regions_c.txt", "w");
	fprintf(fp, "%d ", NUM_OF_FILES);
	fprintf(fp, " %d ", peakRegionSize);
	fprintf(fp, "\n");
	for(i = 0; i < NUM_OF_FILES; i++) {
		for(j = 0; j < peakRegionSize; j++) {
			fprintf(fp, "%d ", peakRegion[i][j]);
		}
		fprintf(fp, "\n");
	}
	fclose(fp);
}

void calcDistances() {
	int i, j;
	long long k;
	long dist;

	omp_set_num_threads(NUM_OF_CORES);
	for(i = 0; i < NUM_OF_FILES - 1; i++) {
		for(j = i + 1; j < NUM_OF_FILES; j++) {
			dist = 0;
			#pragma omp parallel for schedule(static, 100) reduction(+: dist)
			for(k = 0; k < peakRegionSize; k++) {
				dist += abs(peakRegion[i][k] - peakRegion[j][k]);
			}
			distMatrix[i][j] = dist;
		}
	}
}

void writeDistMatrix() {
	int i, j;
	FILE *fp = fopen(RES_FILE, "w");
	for(i = 0; i < NUM_OF_FILES; i++) {
		for(j = 0; j < NUM_OF_FILES; j++) {
			fprintf(fp, "%lld ", distMatrix[j][i]);
		}
		fprintf(fp, "\n");
	}
	fclose(fp);
}

void freeMem() {
	int i;
	for(i = 0; i < NUM_OF_FILES; i++) {
		free(fileNames[i]);
		freeList(pdata[i]);
		free(peakRegion[i]);
		free(distMatrix[i]);
	}

	free(skipChrLengths);
	free(fileNames);
	free(pdata);
	free(peakRegion);
	free(pointers);
	free(distMatrix);
}

int main(int argc, char *argv[]) {
   if (argc==2) {
	NUM_OF_FILES = atoi(argv[1]);

	initSkipChrLengths();
	initFilenames();
	initListsAndMatrices();
	printf("init done\n"); fflush(stdout);

	createPeaks();
	printf("peaks created\n"); fflush(stdout);

	peakRegionSize = createPeakRegions();
	printf("peak regions created\n"); fflush(stdout);

	calcDistances();
	printf("distances calculated\n"); fflush(stdout);

	writeDistMatrix();
	writePeakRegions();
	freeMem();
  }
  else {
        printf("\nNeed to give total number of input bed files as an argument\n");
  }
}
