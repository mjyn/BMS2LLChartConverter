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
	double BPMChangeTime[9];//绝对时间
	double BPMChangePlace[9];//小节位置（%
	double StartTime;
	double StartBPM;
	int BeatCount = 0;
	int BPMChangeInMeasure = 0;
	//在小节当中变更为1，如果在小节初始变更则为0
	int BeatCountChangeInMeasure = 0;
	//本程序目标暂时不支持小结当中的 每小节拍数变更
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
	//此处j是第一个为数字的字符 执行完毕后i代表最后一个数字位 k代表第一个小数位 l代表最后一个小数位
	for (int m = 0; i >= j; i--)
	{
		output = output + (CurrentLine[i] - '0')*pow(10, m);
		m++;
	}
	if (IsFloat)
	{
		int a = 1;//小数点后位数
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
int ReadMeasureCount()//这里也用于确认maxmeasure
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
				//BPM通道为3，十六进制BPM
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
				//BPM通道为8，定义BPM
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

	for (int i = 0; i < 36; i++)
	{
		for (int j = 0; j < 36; j++)
		{
			BPMDefine[i][j] = NULL;
		}
	}//初始化BPM定义区

	while (!feof(SourceFilePt))
		//第一次循环，读取起始BPM、BPM定义、所有的BPM通道以及节拍通道的内容
		//循环后计算每个小节的起始时间和拍数，并标记是否存在小节中途变速。
	{
		fgets(CurrentLine, 1024, SourceFilePt);
		//读取一行

		if (IsBPMLine())
		{
			ProcessCurrentLineNum(BPMLINE);
		}
		//若为#BPM行，获取BPM
		if (IsDefineBPMLine())
		{
			ProcessCurrentLineNum(DEFINEBPMLINE);
		}
		if (IsLNTYPELine())
		{
			ProcessCurrentLineNum(LNTYPELINE);
		}
		//若为BPM定义行，读入定义数
		if (IsChangeBPMLine())
		{
			ProcessCurrentLineNum(CHANGEBPMLINE);
		}
		//此处认为所有的BPM定义都按标准格式在MAIN DATA FIELD之前完成。
		if (IsChangeBeatCountLine())
		{
			ProcessCurrentLineNum(CHANGEBEATCOUNTLINE);
		}
	}
	fclose(SourceFilePt);

	InitializeMeasures();
	
}
