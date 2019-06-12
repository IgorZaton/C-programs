#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>
#include <string.h>

//#pragma pack(1)

struct Picture
{
	char *name, type[2];
	int width, height, scale;
	int **table;
};

int Comments(FILE* file)
{
	char temp;
	int places = 0;
	fscanf(file, "%c", &temp);
	while (temp == ' ' || temp == '\n')
	{
		while (temp == '#')
		{
			while (temp != '\n')
			{
				fscanf(file, "%c", &temp);
				places++;
			}
			fscanf(file, "%c", &temp);
		}
		fscanf(file, "%c", &temp);
	}
	while (temp == '#')
	{
		while (temp != '\n')
		{
			fscanf(file, "%c", &temp);
			places++;
		}
		fscanf(file, "%c", &temp);
	}
	return places;
}
int UploadFile(struct Picture* pic)
{
	FILE* file;
	char *temp;
	temp = malloc(1 * sizeof(char*));
	pic->name = malloc(256 * sizeof(char*));
	printf("Enter the name of file: ");
	scanf("%s", pic->name);
	file = fopen(pic->name, "r");
	if (file == NULL)
		return 1;

	if (Comments(file) != 0)
		fseek(file, Comments(file) - 3, SEEK_CUR);
	else
		fseek(file, 0, SEEK_SET);
	
	fscanf(file, "%s", pic->type);

	if (Comments(file) != 0)
		fseek(file, Comments(file) - 3, SEEK_CUR);
	else
		fseek(file, -1, SEEK_CUR);
	
	fseek(file, -1, SEEK_CUR);
	fscanf(file, "%d", &(pic->width));

	if (Comments(file) != 0)
		fseek(file, Comments(file) - 3, SEEK_CUR);
	else
		fseek(file, -1, SEEK_CUR);
	
	fseek(file, -1, SEEK_CUR);
	fscanf(file, "%d", &(pic->height));
	
	if (Comments(file) != 0)
		fseek(file, Comments(file) - 3, SEEK_CUR);
	else
		fseek(file, -1, SEEK_CUR);
	
	fseek(file, -1, SEEK_CUR);
	fscanf(file, "%d", &(pic->scale));

	if (Comments(file) != 0)
		fseek(file, Comments(file) - 3, SEEK_CUR);
	else
		fseek(file, -1, SEEK_CUR);
	
	pic->table = malloc(pic->height * sizeof(*(pic->table)));
	if (pic->table != NULL) 
	{
		for (int i = 0; i < pic->height; i++)
		{
			pic->table[i] = malloc(pic->width * sizeof(*pic->table));
			if (pic->table[i] == NULL)
			{
				for (int k = 0; k < i; k++)
				{
					free(pic->table[k]);
				}
				free(pic->table);
				printf("Memory allocation error");
				return 1;
			}
		}
	}
	else 
	{
		printf("Memory allocation error");
		return 1;
	}

	fseek(file, -1, SEEK_CUR);
	
	for (int i = 0; i < pic->height; i++)
	{
		for (int j = 0; j < pic->width; j++)
		{
			if (fscanf(file, "%c", temp) != ' ' && fscanf(file, "%c", temp) != '\n')// 1 2 3 3 #cas
			{
				fseek(file, -1, SEEK_CUR);

				if (Comments(file) != 0)
					fseek(file, Comments(file) - 3, SEEK_CUR);
				else
					fseek(file, -1, SEEK_CUR);
	
				fscanf(file, "%d", &pic->table[i][j]);
			}
			else fseek(file, 1, SEEK_CUR);
		}
	}
	fclose(file);
	return 0;
}
int SaveFile(struct Picture pic)
{
	char name[256];
	printf("Enter the name of file: ");
	scanf("%s", name);
	FILE* file;
	file = fopen(name, "w");
	if (file == NULL)
	{
		printf("Saving error");
		return 1;
	}
	fprintf(file, "%c%c\n",pic.type[0],pic.type[1]);
	fprintf(file, "%d %d\n", pic.width, pic.height);
	fprintf(file, "%d\n", pic.scale);
	for (int i = 0; i < pic.height; i++)
	{
		for (int j = 0; j < pic.width; j++)
		{
			fprintf(file, "%d ", pic.table[i][j]);
		}
		fprintf(file, "\n");
	}

	fclose(file);
	return 0;
}
int AddPicture(struct Picture **tab, int *PicCounter, int *current)
{
	struct Picture *temp;
	(*PicCounter)++;
	temp = realloc(*tab, (*PicCounter) * sizeof(*temp));
	if (temp != NULL)
	{
		struct Picture *pic;
		pic = malloc(sizeof(*pic));
		UploadFile(pic);
		*tab = temp;
		(*tab)[(*PicCounter)-1] = *pic;
		(*current) = (*PicCounter)-1;
		printf("The %s is the current picture now.\n", (*tab)[(*current)].name);
		free(pic);
	}
	else {
		printf("Memory reallocation error");
		free(*tab);
		(*PicCounter)--;
		return 1;
	}
	return 0;
}
int DeletePicture(struct Picture **tab, int *PicCounter, int *current)
{
	int PicToDelete = -1;
	char wronganswer[256];
	for (int i = 0; i < (*PicCounter); i++)
	{
		printf("%d %s\n", i + 1, (*tab)[i].name);
	}
	printf("Current pic is %s\n", (*tab)[*current].name);
	printf("Enter the number of pic to delete: ");
	scanf("%d", &PicToDelete);
	while ((PicToDelete > (*PicCounter)) || (PicToDelete < 1)) 
	{
		scanf("%s", wronganswer);
		printf("Wrong answer\nEnter the number of pic to delete: ");
		scanf("%d", &PicToDelete);
		
	}
	if (PicToDelete == (*current)+1)
	{
		printf("You've deleted current picture.\n The current pic now is the first one.\n");
		(*current) = 0;
	}
	if (PicToDelete < (*current)+1) (*current)--;
	
	free((*tab)[PicToDelete - 1].name);
	for (int i = 0; i < (*tab)[PicToDelete - 1].height; i++)
	{
		free((*tab)[PicToDelete - 1].table[i]);
	}
	free((*tab)[PicToDelete - 1].table);
	
	for (int i = PicToDelete - 1; i < (*PicCounter) - 1; i++)
	{
		(*tab)[i] = (*tab)[i + 1];
	}

	(*PicCounter)--;
	if ((*PicCounter) == 0) { printf("There in no picture left."); return 1; }

	struct Picture *temp;
	temp = realloc(*tab, ((*PicCounter)) * sizeof(*temp));
	if (temp != NULL)
	{
		*tab = temp;
		printf("Picture has been deleted.\n");
		return 0;
	}
	else
	{
		(*PicCounter)++;
		printf("Memory reallocation error or there is no picture left.\n");
		return 1;
	}
}
void ActivePictures(struct Picture **tab, int *PicCounter, int *current)
{
	for (int i = 0; i < (*PicCounter); i++)
	{
		printf("%d %s\n", i+1, (*tab)[i].name);
	}
	printf("Your current picture is %d. %s\n", (*current) + 1, (*tab)[(*current)].name);
}
int ChoosePicture(struct Picture **tab, int *current, int *PicCounter)
{
	char wronganswer[256];
	if ((*PicCounter) == 0)
	{
		printf("There's no picture to work with\nUse option 1 to add a picture\n");
		return 1;
	}
	for (int i = 0; i < (*PicCounter); i++)
	{
		printf("%d %s\n", i + 1, (*tab)[i].name);
	}
	printf("Your current picture is %d. %s\n", (*current)+1,(*tab)[(*current)].name);
	printf("Choose current picture by number: ");
	while (!scanf("%d", current))
	{
		scanf("%s", wronganswer);
		printf("Choose current picture by number: ");
		scanf("%d", current);
	}
	while (*current < 1 || *current > *PicCounter)
	{
		printf("Choose current picture by number: ");
		while (!scanf("%d", current))
		{
			scanf("%s", wronganswer);
			printf("Choose current picture by number: ");
			scanf("%d", current);
		}
	}
	(*current)--;
	return 0;
}
void Mirror(struct Picture *pic)
{
	int temp;
	for (int i = 0; i < pic->height; i++)
	{
		for (int j = 0; j < pic->width / 2; j++)
		{
			temp = pic->table[i][j];
			pic->table[i][j] = pic->table[i][pic->width - j - 1];
			pic->table[i][pic->width - j - 1] = temp;

		}
	}
}
int Spin(struct Picture *pic)
{
	int **temp;
	int Reps;
	char wronganswer[256];
	printf("Enter the number of spins: ");
	while (!scanf("%d", &Reps))
	{
		scanf("%s", wronganswer);
		printf("Wrong answer\nEnter the number of spins: ");
		scanf("%d", &Reps);
	}
	while (Reps < 0)
	{
		printf("Wrong answer\nEnter the number of spins: ");
		scanf("%d", &Reps);
	}
	for (int r = 0; r < Reps; r++)
	{
		
		if (r % 2 == 1) {
			temp = malloc(pic->height * sizeof(*temp));
			if (temp != NULL)
			{
				for (int i = 0; i < pic->height; i++)
				{
					temp[i] = malloc(pic->width * sizeof(*temp));
					if (temp[i] == NULL)
					{
						for (int k = 0; k < i; k++)
						{
							free(temp[k]);
						}
						free(temp);
						printf("Memory allocation error");
						return 1;
					}
				}
			}
			else
			{
				printf("Memory allocation error");
				return 1;
			}
			for (int i = 0; i < pic->height; i++)
			{
				for (int j = 0; j < pic->width; j++)
				{
					temp[i][j] = pic->table[j][pic->height - i - 1];
				}
			}
			for (int i = 0; i < pic->width; i++)
			{
				free(pic->table[i]);
			}
			free(pic->table);

			pic->table = temp;

			temp = NULL;
		}
		else
		{
			temp = malloc(pic->width * sizeof(*temp));
			if (temp != NULL)
			{
				for (int i = 0; i < pic->width; i++)
				{
					temp[i] = malloc(pic->height * sizeof(*temp));
					if (temp[i] == NULL)
					{
						for (int k = 0; k < i; k++)
						{
							free(temp[k]);
						}
						free(temp);
						printf("Memory allocation error");
						return 1;
					}
				}
			}
			else
			{
				printf("Memory allocation error");
				return 1;
			}

			for (int i = 0; i < pic->width; i++)
			{
				for (int j = 0; j < pic->height; j++)
				{
					temp[i][j] = pic->table[j][pic->width - i - 1];
				}
			}
			for (int i = 0; i < pic->height; i++)
			{
				free(pic->table[i]);
			}
			free(pic->table);

			pic->table = temp;

			temp = NULL;
		}
	}
	return 0;
}
int Histogram(struct Picture pic)
{
	int *value;
	value = calloc(pic.scale , sizeof(*value));
	for (int i = 0; i < pic.height; i++)
	{
		for (int j = 0; j < pic.width; j++)
		{
			value[pic.table[i][j]]++;
		}
	}
	char name[256];
	printf("Enter the name of histogram file: ");
	scanf("%s", name);
	FILE* file;
	file = fopen(name, "w");
	if (file == NULL)
	{
		printf("Saving error");
		return 1;
	}
	for (int i = 0; i < pic.scale; i++)
	{
		fprintf(file, "%d;%d\n", i, value[i]);
	}
	fclose(file);
	return 0;
}
void Thresholding(struct Picture *pic)
{
	int band;
	char wronganswer[256];
	printf("Enter the value of band: ");
	scanf("%d", &band);
	while (band<1 || band>(pic->scale) - 1)
	{
		scanf("%s", wronganswer);
		printf("Enter the value of band: ");
		scanf("%d", &band);
	}
	for (int i = 0; i < pic->height; i++)
	{
		for (int j = 0; j < pic->width; j++)
		{
			if (pic->table[i][j] <= band) pic->table[i][j] = 0;
			else if (pic->table[i][j] > band) pic->table[i][j] = pic->scale;
		}
	}
}
void Negative(struct Picture *pic)
{
	for (int i = 0; i < pic->height; i++)
	{
		for (int j = 0; j < pic->width; j++)
		{
			pic->table[i][j] = pic->scale - pic->table[i][j] - 1;
		}
	}
}
void ImpulseNoise(struct Picture *pic)
{
	int noiseChance, saltorpepper;

	for (int i = 0; i < pic->height; i++)
	{
		for (int j = 0; j < pic->width; j++)
		{
			noiseChance = rand() % 100;
			if (noiseChance < 10) 
			{
				saltorpepper = rand() % 2;
				if (saltorpepper == 0) pic->table[i][j] = 0;
				else pic->table[i][j] = pic->scale - 1;
			}
		}
	}
}
void Gauss(struct Picture *pic)
{
	int sum = 0;
	for (int i = 1; i < pic->height - 1; i++)
	{
		for (int j = 1; j < pic->width - 1; j++)
		{
			sum = pic->table[i][j] * 4 + (pic->table[i - 1][j] + pic->table[i][j - 1] + pic->table[i + 1][j] + pic->table[i][j + 1]) * 2 + pic->table[i - 1][j - 1] + pic->table[i + 1][j + 1] + pic->table[i + 1][j - 1] + pic->table[i - 1][j + 1];
			pic->table[i][j] = sum / 16;
		}
	}
}
int InsertionSort(struct Picture *pic,int k,int m)
{
	int *temp;
	temp = malloc(9 * sizeof(*temp));
	temp[0] = pic->table[k][m];
	temp[1] = pic->table[k-1][m-1];
	temp[2] = pic->table[k-1][m];
	temp[3] = pic->table[k][m-1];
	temp[4] = pic->table[k+1][m+1];
	temp[5] = pic->table[k+1][m];
	temp[6] = pic->table[k][m+1];
	temp[7] = pic->table[k-1][m+1];
	temp[8] = pic->table[k+1][m-1];
	
	int curr;
	int ind; 
	for (int i = 1; i < 9 ; i++)
	{
		curr = temp[i];
		ind = i;
		while (ind > 0 && curr < temp[ind - 1])
		{
			temp[ind] = temp[ind - 1];
			ind--;
		}
		temp[ind] = curr;
	}
	return temp[4];
}
void Median(struct Picture *pic)
{
	for (int i = 1; i < pic->height - 1; i++)
	{
		for (int j = 1; j < pic->width - 1; j++)
		{
			pic->table[i][j] = InsertionSort(pic, i, j);
		}
	}
}
void FreeMemory(struct Picture *pic)
{
	free(pic->name);
	for (int i = 0; i < pic->height;i++)
		free(pic->table[i]);
	free(pic->table);
}
void LookUpTable(struct Picture *pic, int ***tab)
{
	(*tab) = malloc(3 * sizeof(*(*tab)));
	for (int i = 0; i < 3; i++)
	{
		(*tab)[i] = malloc(((pic->scale)+1) * sizeof(*(*tab)));
	}
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < (pic->scale)+1; j++)
		{
			(*tab)[i][j] = rand() % 256;
		}
	}
}
void RealLUT(struct Picture *pic, int ***tab)
{
	int R = 255, G = 0, B = 0;
	(*tab) = malloc(3 * sizeof(*(*tab)));
	for (int i = 0; i < 3; i++)
	{
		(*tab)[i] = malloc(((pic->scale) + 1) * sizeof(*(*tab)));
	}
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < (pic->scale)+1; j++)
		{
			if (j == (pic->scale)) (*tab)[i][j] = 255;
			else 
			{
				if (i == 0) (*tab)[i][j] = R;
				if (i == 1) (*tab)[i][j] = G;
				if (i == 2) (*tab)[i][j] = B;
				if (R == 255 && B == 0) G = G + 5;
				if (G == 255 && B == 0) R = R - 5;
				if (G == 255 && R == 0) B = B + 5;
				if (B == 255 && R == 0) G = G - 5;
				if (B == 255 && G == 0) R = R + 5;
			}
		}
	}
}
void CreatePPM(int **lut, int ***ppm, struct Picture *pic, int *ppmscale)
{
	int k = 0;
	int m = 0;
	(*ppm) = malloc(pic->height * sizeof(*(*ppm)));
	for (int i = 0; i < pic->height; i++)
	{
		(*ppm)[i] = malloc(3 * pic->width * sizeof(*(*ppm)));
	}

	for (int i = 0; i < pic->height; i++)
	{
		for (int j = 0; j < (3 * pic->width)-3; j++)
		{
			if (m % 3 == 0) 
			{
				k++; m = 0;
			}
			(*ppm)[i][j] = pic->table[i][k] * lut[m][pic->table[i][k]];
			if ((*ppm)[i][j]>(*ppmscale)) (*ppmscale) = (*ppm)[i][j];
			if ((*ppmscale) > 65535)
			{
				(*ppmscale) = 65535;
				(*ppm)[i][j] = 65535;
			}
			m++;
		}
		k = 0;
	}
	
	for (int i = 0; i < pic->height; i++)
	{
		for (int j = (3 * pic->width) - 3; j < (3 * pic->width); j++)
		{
			(*ppm)[i][j] = rand() % (*ppmscale);
		}
	}
}
int SavePPM(int **tab,struct Picture *pic, int ppmscale)
{
	char name[256];
	printf("Enter the name of PPM file: ");
	scanf("%s", name);
	FILE* file;
	file = fopen(name, "w");
	if (file == NULL)
	{
		printf("Saving error");
		return 1;
	}
	fprintf(file, "%c%c\n", 'P', '3');
	fprintf(file, "%d %d\n", pic->width, pic->height);
	fprintf(file, "%d\n", ppmscale);
	for (int i = 0; i < pic->height; i++)
	{
		for (int j = 0; j < (3*pic->width); j++)
		{
			fprintf(file, "%d ", tab[i][j]);
		}
		fprintf(file, "\n");
	}

	fclose(file);
	return 0;
}
void Menu()
{
	system("cls");
	printf("\tPGM IMAGE PROCESSING PROGRAM\n");
	printf("1. Add Picture\n");
	printf("2. Save Picture\n");
	printf("3. Show Upluaded Pictures\n");
	printf("4. Choose Picture To Work With\n");
	printf("5. Delete Picture\n");
	printf("6. Mirror Reflection\n");
	printf("7. Spin k*90deg\n");
	printf("8. Create Histogram\n");
	printf("9. Thershold\n");
	printf("10. Create Negative\n");
	printf("11. Add Some Noise\n");
	printf("12. Gauss Filter\n");
	printf("13. Median Filter\n");
	printf("14. Add Some Fancy Colors\n");
	printf("15. Add Some Real Colors\n");
	printf("16. Exit\n");
}
int Error(int PicCounter)
{
	if (PicCounter == 0)
	{
		printf("You don't have any pictures\nAdd pictures using function no.1\n");
		return 1;
	}
	return 0;
}

int main()
{
	setlocale(LC_ALL, "polish_poland");
	srand(time(NULL));
	struct Picture *tabofpics;
	int **lut;
	int **ppm, ppmscale = - 1;
	int piccounter = 0;
	int current = -1;
	int choice;
	char wrongchoice[256];
	tabofpics = malloc(NULL);
	
	while (1) 
	{
		Menu();
		scanf("%d", &choice);
		switch (choice)
		{
		case 1:
			AddPicture(&tabofpics, &piccounter, &current);
			system("pause");
			break;
		case 2:
			if (Error(piccounter) == 1) { system("pause"); break; }
			SaveFile(tabofpics[current]);
			system("pause");
			break;
		case 3:
			if (Error(piccounter) == 1) { system("pause"); break; }
			ActivePictures(&tabofpics, &piccounter, &current);
			system("pause");
			break;
		case 4:
			ChoosePicture(&tabofpics,&current, &piccounter);
			system("pause");
			break;
		case 5:
			if (Error(piccounter) == 1) { system("pause"); break; }
			DeletePicture(&tabofpics, &piccounter, &current);
			system("pause");
			break;
		case 6:
			if (Error(piccounter) == 1) { system("pause"); break; }
			Mirror(&tabofpics[current]);
			system("pause");
			break;
		case 7:
			if (Error(piccounter) == 1) { system("pause"); break; }
			Spin(&tabofpics[current]);
			system("pause");
			break;
		case 8:
			if (Error(piccounter) == 1) { system("pause"); break; }
			Histogram(tabofpics[current]);
			system("pause");
			break;
		case 9:
			if (Error(piccounter) == 1) { system("pause"); break; }
			Thresholding(&tabofpics[current]);
			system("pause");
			break;
		case 10:
			if (Error(piccounter) == 1) { system("pause"); break; }
			Negative(&tabofpics[current]);
			system("pause");
			break;
		case 11:
			if (Error(piccounter) == 1) { system("pause"); break; }
			ImpulseNoise(&tabofpics[current]);
			system("pause");
			break;
		case 12:
			if (Error(piccounter) == 1) { system("pause"); break; }
			Gauss(&tabofpics[current]);
			system("pause");
			break;
		case 13:
			if (Error(piccounter) == 1) { system("pause"); break; }
			Median(&tabofpics[current]);
			system("pause");
			break;
		case 14:
			if (Error(piccounter) == 1) {system("pause"); break;}
			LookUpTable(&tabofpics[current], &lut);
			CreatePPM(lut, &ppm, &tabofpics[current], &ppmscale);
			SavePPM(ppm, &tabofpics[current], ppmscale);
			system("pause");
			break;
		case 15:
			if (Error(piccounter) == 1) { system("pause"); break; }
			RealLUT(&tabofpics[current], &lut);
			CreatePPM(lut, &ppm, &tabofpics[current], &ppmscale);
			SavePPM(ppm, &tabofpics[current], ppmscale);
			system("pause");
			break;
		case 16:
			printf("Thanks for using!");
			for (int i = 0; i < piccounter; i++)
			{
				FreeMemory(&tabofpics[i]);
			}
			return 1;
		default:
			scanf("%s", wrongchoice);
		}
	}
	return 0;
}