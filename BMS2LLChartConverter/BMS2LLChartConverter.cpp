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
		printf("打开文件失败。\n");
		system("pause");
		return(0);
	}
	FILE *SourceFilePt;
	SourceFilePt = fopen(argv[1], "r");
	if (SourceFilePt == NULL)
	{
		printf("打开文件失败。\n");
		system("pause");
		return(0);
	}

	while (!feof(SourceFilePt))
		//主循环
	{
		fgets(CurrentLine, 1024, SourceFilePt);
		if (IsBPMLine() == TRUE)
		{
			int i, j;
			for (i = 5; CurrentLine[i] == ' '; i++) {}
			for (j = i; ((CurrentLine[i] >= '0') && (CurrentLine[i] <= '9')); i++) {}
			i--;
			//此处j是第一个为数字的字符 执行完毕后i代表最后一个数字位
			for (int k = 0; i >= j; i--)
			{
				CurrentBPM = CurrentBPM + (CurrentLine[i] - '0') * pow(10, k);
				k++;
			}
		}
	}
}

