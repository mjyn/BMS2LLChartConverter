#include <stdio.h>
#include <Windows.h>
#include <string.h>
#include <math.h>
#define TRUE 1
#define FALSE 0
#define BPMLINE 0
#define DEFINEBPMLINE 1
#define LNTYPELINE 2

char CurrentLine[1024];
double StartBPM;
int LNType;
double BPMDefine[36][36];
struct MeasureStruct
{
	double BPM[10];
	double BPMChangeTime[10];
	double StartTime;
	int BeatCount;
	int BPMChangeInMeasure;
}Measure[1000];
int IsBPMLine()
{
	if (strnicmp("#BPM ", CurrentLine, 5) == 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
int IsLNTYPELine()
{
	if (strnicmp("#LNTYPE ", CurrentLine, 8) == 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
int IsDefineBPMLine()
{
	if ((strnicmp("#BPM", CurrentLine, 4) == 0) && CurrentLine[4] != ' ')
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
int IsChangeBPMLine()
{
	if ((CurrentLine[0] == '#') && (CurrentLine[4] == '0') && (CurrentLine[5] == '3'))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

double ReadNumber(int StartDigit)
{
	int i, j, k, l, IsFloat;
	double output = 0;
	for (i = StartDigit; CurrentLine[i] == ' '; i++) {}
	for (j = i; ((CurrentLine[i] >= '0') && (CurrentLine[i] <= '9')); i++) {}
	if (CurrentLine[i] == '.')
	{
		IsFloat = 1;
		k = i + 1;
		for (l = k; ((CurrentLine[l] >= '0') && (CurrentLine[l] <= '9')); l++) {}
		l--;
	}
	else
	{
		IsFloat = 0;
	}
	i--;
	//�˴�j�ǵ�һ��Ϊ���ֵ��ַ� ִ����Ϻ�i�������һ������λ k�����һ��С��λ l�������һ��С��λ
	for (int m = 0; i >= j; i--)
	{
		output = output + (CurrentLine[i] - '0')*pow(10, m);
		m++;
	}
	if (IsFloat)
	{
		int a = 1;//С�����λ��
		for (int m = -1; k <= l; k++)
		{
			output = output + (CurrentLine[k] - '0')*pow(10, m);
			m--;
			a = a * 10;
		}
		output = output*a;
		output = (int)(output + 0.5);
		output = output / a;
	}
	return output;
}

int ProcessCurrentLineNum(int LineType)
{
	switch (LineType)
	{
	case BPMLINE:
		StartBPM = ReadNumber(5);
		break;
	case DEFINEBPMLINE:
		int x, y;
		if ((CurrentLine[4] >= '0') && (CurrentLine[4] <= '9'))
		{
			x = CurrentLine[4] - '0';
		}
		else
		{
			x = CurrentLine[4] - 'A' + 10;
		}
		if ((CurrentLine[5] >= '0') && (CurrentLine[5] <= '9'))
		{
			y = CurrentLine[5] - '0';
		}
		else
		{
			y = CurrentLine[5] - 'A' + 10;
		}
		BPMDefine[x][y] = ReadNumber(7);
		break;
	case LNTYPELINE:
		LNType = ReadNumber(8);
		break;
	}
	return 0;
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
		//��һ��ѭ������ȡ��ʼBPM��BPM���塢���е�BPMͨ���Լ�����ͨ��������
		//ѭ�������ÿ��С�ڵ���ʼʱ���������������Ƿ����С����;���١�
	{
		fgets(CurrentLine, 1024, SourceFilePt);
		//��ȡһ��

		if (IsBPMLine())
		{
			ProcessCurrentLineNum(BPMLINE);
		}
		//��Ϊ#BPM�У���ȡBPM
		if (IsDefineBPMLine())
		{
			ProcessCurrentLineNum(DEFINEBPMLINE);
		}
		if (IsLNTYPELine())
		{
			ProcessCurrentLineNum(LNTYPELINE);
		}
		//��ΪBPM�����У����붨����
		if (IsChangeBPMLine())
	}
}
