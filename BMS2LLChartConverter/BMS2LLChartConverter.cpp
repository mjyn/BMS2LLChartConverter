#include <stdio.h>
#include <Windows.h>
#include <string.h>
#include <math.h>
#define TRUE 1
#define FALSE 0
char CurrentLine[1024];
int CurrentBPM;

int IsBPMLine()
{
	if (strnicmp("#BPM", CurrentLine, 4) == 0)
	{
		return 1;
	}
}

//int IsSkipCurrentLine()

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		printf("���ļ�ʧ�ܡ�\n");
		system("pause");
		return(0);
	}
	FILE *SourceFilePt;
	SourceFilePt = fopen(argv[1], "r");
	if (SourceFilePt == NULL)
	{
		printf("���ļ�ʧ�ܡ�\n");
		system("pause");
		return(0);
	}

	while (!feof(SourceFilePt))
		//��ѭ��
	{
		fgets(CurrentLine, 1024, SourceFilePt);
		if (IsBPMLine() == TRUE)
		{
			int i, j;
			for (i = 5; CurrentLine[i] == ' '; i++) {}
			for (j = i; ((CurrentLine[i] >= '0') && (CurrentLine[i] <= '9')); i++) {}
			i--;
			//�˴�j�ǵ�һ��Ϊ���ֵ��ַ� ִ����Ϻ�i�������һ������λ
			for (int k = 0; i >= j; i--)
			{
				CurrentBPM = CurrentBPM + (CurrentLine[i] - '0') * pow(10, k);
				k++;
			}
		}
	}
}

