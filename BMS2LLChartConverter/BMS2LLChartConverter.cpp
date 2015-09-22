#include <stdio.h>
#include <Windows.h>
#include <string.h>
#include <math.h>
#define TRUE 1
#define FALSE 0
#define BPMLINE 0
#define DEFINEBPMLINE 1
#define LNTYPELINE 2
#define CHANGEBPMLINE 3
#define CHANGEBEATCOUNTLINE 4

char CurrentLine[1024];
int LNType;
double BPMDefine[36][36];
struct MeasureStruct
{
	double ChangedBPM[9];
	double BPMChangeTime[9];//����ʱ��
	double BPMChangePlace[9];//С��λ�ã�%
	double StartTime;
	double StartBPM;
	int BeatCount = 0;
	int BPMChangeInMeasure = 0;
	//��С�ڵ��б��Ϊ1�������С�ڳ�ʼ�����Ϊ0
	int BeatCountChangeInMeasure = 0;
	//������Ŀ����ʱ��֧��С�ᵱ�е� ÿС���������
}Measure[1000];
int MaxMeasure;

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
	if ((CurrentLine[0] == '#') && (CurrentLine[4] == '0') && ((CurrentLine[5] == '3') || (CurrentLine[5] == '8')) && (CurrentLine[1] >= '0') && (CurrentLine[1] <= '9'))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
int IsChangeBeatCountLine()
{
	if ((CurrentLine[0] == '#') && (CurrentLine[4] == '0') && (CurrentLine[5] == '2') && (CurrentLine[1] >= '0') && (CurrentLine[1] <= '9'))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
int IsMainDataLine()
{
	if ((CurrentLine[0] != '#') || (CurrentLine[6] != ':'))
	{
		return 0;
	}
	for (int i = 1; i < 6; i++)
	{
		if ((CurrentLine[i] <= '0') || (CurrentLine[i] >= '9'))
		{
			if ((CurrentLine[i] <= 'A') || (CurrentLine[i] >= 'Z'))
			{
				return 0;
			}
		}
	}
	return 1;
}

int MainDataLine_GetLength()
{
	int Length = 0, i = 7;
	while (1)
	{
		if (CurrentLine[i] == 10)
		{
			break;
		}
		Length++;
		i = i + 2;
	}
	return Length;
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
int ReadMeasureCount()//����Ҳ����ȷ��maxmeasure
{
	int Count = 0;
	Count = Count + (CurrentLine[1] - '0') * 100 + (CurrentLine[2] - '0') * 10 + (CurrentLine[3] - '0');
	if (Count > MaxMeasure)
	{
		MaxMeasure = Count;
	}
	return Count;
}

int ProcessCurrentLineNum(int LineType)
{
	switch (LineType)
	{
	case BPMLINE:
		Measure[0].StartBPM = ReadNumber(5);
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
	case CHANGEBPMLINE:
		int Length = MainDataLine_GetLength();
		int CurrentMeasure;
		int digit1, digit2;
		int ChangeTimesCount = 0;
		for (int i = 0; i < Length;)
		{
			if ((CurrentLine[7 + 2 * i] == '0') && (CurrentLine[7 + 2 * i + 1] == '0'))
			{
				i++;
			}
			else
			{
				CurrentMeasure = ReadMeasureCount();
				if (i != 0)
				{
					Measure[CurrentMeasure].BPMChangeInMeasure = 1;
					
				}
				
				digit1 = CurrentLine[7 + 2 * i];
				digit2 = CurrentLine[7 + 2 * i + 1];

				int x, y;
				if ((digit1 >= '0') && (digit1 <= '9'))
				{
					x = digit1 - '0';
				}
				else
				{
					x = digit1 - 'A' + 10;
				}
				if ((digit2 >= '0') && (digit2 <= '9'))
				{
					y = digit2 - '0';
				}
				else
				{
					y = digit2 - 'A' + 10;
				}

				if (CurrentLine[5] == '3')
				//BPMͨ��Ϊ3��ʮ������BPM
				{
					if (Measure[CurrentMeasure].BPMChangeInMeasure == 1)
					{
						Measure[CurrentMeasure].BPMChangePlace[ChangeTimesCount] = (double)i / (double)Length;
						Measure[CurrentMeasure].ChangedBPM[ChangeTimesCount] = x * 16 + y;
					}
					else
					{
						Measure[CurrentMeasure].StartBPM = x * 16 + y;
					}
				}
				else
				//BPMͨ��Ϊ8������BPM
				{
					if (Measure[CurrentMeasure].BPMChangeInMeasure == 1)
					{
						Measure[CurrentMeasure].BPMChangePlace[ChangeTimesCount] = i / Length;
						Measure[CurrentMeasure].ChangedBPM[ChangeTimesCount] = BPMDefine[x][y];
					}
					else
					{
						Measure[CurrentMeasure].StartBPM = BPMDefine[x][y];
					}
				}
				if (Measure[CurrentMeasure].BPMChangeInMeasure == 1)
				{
					ChangeTimesCount++;
				}
				i++;
			}
		}
		break;
	case CHANGEBEATCOUNTLINE:
		int CurrentMeasure;
		CurrentMeasure = ReadMeasureCount();
		Measure[CurrentMeasure].BeatCountChangeInMeasure = 1;
		Measure[CurrentMeasure].BeatCount = 4 * ReadNumber(7);
		break;
	}
	return 0;
}

int InitializeMeasures()
{

}

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

	for (int i = 0; i < 36; i++)
	{
		for (int j = 0; j < 36; j++)
		{
			BPMDefine[i][j] = NULL;
		}
	}//��ʼ��BPM������

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
		{
			ProcessCurrentLineNum(CHANGEBPMLINE);
		}
		//�˴���Ϊ���е�BPM���嶼����׼��ʽ��MAIN DATA FIELD֮ǰ��ɡ�
		if (IsChangeBeatCountLine())
		{
			ProcessCurrentLineNum(CHANGEBEATCOUNTLINE);
		}
	}
	fclose(SourceFilePt);

	InitializeMeasures();
	
}
