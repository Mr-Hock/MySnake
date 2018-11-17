#pragma once
class POINTDATA
{
public:
	int m_x;//X����
	int m_y;//Y����
	int m_color;//�õ���ɫ
	char* m_data;//�õ���ʾ��ͼ��
	int m_type;//��չ������

	POINTDATA() {};//���ι��캯����ʼ������Աֵ
	POINTDATA(int x, int y, int color, char* data,int type=0 ) :
	m_x(x), m_y(y), m_color(color), m_data(data), m_type(type) {}
};
struct RANKING
{
	int NormalRank = 0;
	int WearlRank = 0;
};
class Ranking
{
	RANKING RK;

public:
	Ranking() { LoadRank(); }

	void LoadRank()
	{
		FILE *pfile;
		fopen_s(&pfile, "snake.sav", "rb");
		if (pfile == NULL)return;
		fread(&RK, 1, sizeof(RK), pfile);
		fclose(pfile);
	}
	void SaveRank()
	{
		FILE *pfile;
		fopen_s(&pfile, "snake.sav", "wb");
		if (pfile == NULL)return;
		fwrite(&RK, 1, sizeof(RK), pfile);
		fclose(pfile);
	}
	int GetNormalRank()
	{
		return RK.NormalRank;
	}
	int GetWearRank()
	{
		return RK.WearlRank;
	}
	void SetNormalRank(int rank)
	{
		if(rank>RK.NormalRank)RK.NormalRank = rank;
	}
	void SetWearRank(int rank)
	{
		if (rank > RK.WearlRank)RK.WearlRank = rank;
	}
protected:
private:
};

//���ڸ��ִ�ӡ�ĺ���
void PrintPoint(WORD x, WORD y, char* data, int color)
{
	using std::cout;

	HANDLE HND;
	HND = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_CURSOR_INFO cci;
	cci.bVisible = false;
	cci.dwSize = 1;
	SetConsoleCursorInfo(HND, &cci);

	COORD pos = { x * 2,y };
	SetConsoleCursorPosition(HND, pos);


	SetConsoleTextAttribute(HND, color);
	//SetConsoleTextAttribute(HND, rand()%16);

	cout << data;

}
void PrintPoint(WORD x, WORD y, int data, int color)
{
	using std::cout;

	HANDLE HND;
	HND = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_CURSOR_INFO cci;
	cci.bVisible = false;
	cci.dwSize = 1;
	SetConsoleCursorInfo(HND, &cci);

	COORD pos = { x * 2,y };
	SetConsoleCursorPosition(HND, pos);


	SetConsoleTextAttribute(HND, color);
	//SetConsoleTextAttribute(HND, rand()%16);

	cout << data;

}
void PrintInfo(WORD x, WORD y, char* data, int num, int color)
{
	using std::cout;

	HANDLE HND;
	HND = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_CURSOR_INFO cci;
	cci.bVisible = false;
	cci.dwSize = 1;
	SetConsoleCursorInfo(HND, &cci);

	SetConsoleTextAttribute(HND, color);

	COORD pos = { x * 2 ,y };
	SetConsoleCursorPosition(HND, pos);
	cout << data << num;
}
void PrintEmptyFrame(int x, int y, int wide, int higt, int color, char*data = "��")
{
	for (int w = 0; w <= wide; w++)
	{
		for (int h = 0; h <= higt; h++)
		{
			if (h == 0 || h == higt)
			{
				PrintPoint(w + x, h + y, data, color);
			}
			else if (w == 0 || w == wide)
			{
				PrintPoint(w + x, h + y, data, color);
			}
		}
	}

}
void PrintTilteFrame(int color)
{
	for (int w = 1; w <= 58; w++)
	{
		for (int h = 1; h <= 43; h++)
		{
			if (h == 1 || h == 43)
			{
				PrintPoint(w, h, "��", color);
			}
			else if (w == 1 || w == 58)
			{
				PrintPoint(w, h, "��", color);
			}
		}
	}

}
void PrintGameTips(int x, int y, bool partner = false)
{
	if (partner)
	{
		PrintPoint(x, y, "����˵��:", 0xc);
		PrintPoint(x, y + 1, "WASD ���� F �ڵ�", 0xb);
		PrintPoint(x, y + 2, "IKJL ���� P �ڵ�", 0xe);

		PrintPoint(x, y + 4, "����˵����", 0xc);
		PrintPoint(x, y + 5, "�� ��ǽ�ɴ�", 0xa);
		PrintPoint(x, y + 6, "�� �������ɴ�", 0xc);
		PrintPoint(x, y + 8, "�� �Լ��䳤", 0xe);
		PrintPoint(x, y + 9, "�� �Լ����", 0xd);
		PrintPoint(x, y + 10, "�� �Է��䳤", 0xe);
		PrintPoint(x, y + 11, "�� �Է����", 0xd);
		PrintPoint(x, y + 12, "�d �Է����� 5��", 0xe);
		PrintPoint(x, y + 13, "�e �Է���ת�� 5��", 0xd);
		PrintPoint(x, y + 14, "�� �ڵ�", 0xc);

	}
	else
	{
		PrintPoint(x, y, "��ϷTips:", 0xc);
		PrintPoint(x, y + 2, "WASD ���Ʒ���", 0xb);
		PrintPoint(x, y + 4, "�������� ����", 10);
		PrintPoint(x, y + 6, "�ո���ͣ/�ָ�", 10);

		PrintPoint(x, y + 8, "�� ��ǽ�ɴ�", 0xa);
		PrintPoint(x, y + 9, "�� �������ɴ�", 0xc);
		PrintPoint(x, y + 10, "�� �䳤ʳ��90%", 0xe);
		PrintPoint(x, y + 11, "�� ���ʳ��10%", 0xd);
		PrintPoint(x, y + 12, "�� �������", 0x9);
	}

}
void PrintTilteText(int x, int y)
{
	using std::cout;
	using std::endl;

	PrintPoint(x, y, " ������    ��     ��    ������    ��   ��   ��������   ", 0xb);
	PrintPoint(x, y + 1, "��    ��   ����   ��   ��    ��   ��  ��    ��         ", 0x2);
	PrintPoint(x, y + 2, "��         �� ��  ��   ��    ��   �� ��     ��         ", 0x6);
	PrintPoint(x, y + 3, " ������    ��  �� ��   ��������   ����      ��������   ", 0xc);
	PrintPoint(x, y + 4, "      ��   ��   ����   ��    ��   �� ��     ��         ", 0xa);
	PrintPoint(x, y + 5, "��    ��   ��     ��   ��    ��   ��  ��    ��         ", 0x5);
	PrintPoint(x, y + 6, " ������    ��     ��   ��    ��   ��   ��   ��������   ", 0xe);

}
void PrintWinLoseText(int x, int y, int type)
{
	using std::cout;
	using std::endl;

	if (type == 0)
	{
		PrintPoint(x, y, "��          �� ", 0xd);
		PrintPoint(x, y + 1, "  ��      ��   ", 0xd);
		PrintPoint(x, y + 2, "    ��  ��     ", 0xd);
		PrintPoint(x, y + 3, "      ��       ", 0xd);
		PrintPoint(x, y + 4, "    ��  ��     ", 0xd);
		PrintPoint(x, y + 5, "  ��      ��   ", 0xd);
		PrintPoint(x, y + 6, "��          �� ", 0xd);
	}
	else
	{
		PrintPoint(x, y, "            �� ", 0xa);
		PrintPoint(x, y + 1, "           ��   ", 0xa);
		PrintPoint(x, y + 2, "          ��     ", 0xa);
		PrintPoint(x, y + 3, "         ��       ", 0xa);
		PrintPoint(x, y + 4, "��      ��        ", 0xa);
		PrintPoint(x, y + 5, "  ��   ��         ", 0xa);
		PrintPoint(x, y + 6, "    ����        ", 0xa);
	}

}
void PrintGameOverText(int x, int y, int ncode, int rank, int color, bool partner, int loser)
{
	using std::cout;
	using std::endl;

	PrintPoint(x, y, "       ����������", color);
	PrintPoint(x, y + 1, "    ���������������� ", color);
	PrintPoint(x, y + 2, "  ������ ������ ������ ", color);
	PrintPoint(x, y + 3, " ������    ��    ������ ", color);
	PrintPoint(x, y + 4, "  ������  ����  ������", color);
	PrintPoint(x, y + 5, "   ������������������", color);
	PrintPoint(x, y + 6, "     ��������������", color);
	PrintPoint(x, y + 7, "     ���� ���� ����", color);
	PrintPoint(x, y + 8, "       Game Over!        ", 0xa);
	if (partner)
	{
		PrintPoint(x, y + 9, loser == 0 ? "        2P ʤ����" : "        1P ʤ����", 0xe);
	}
	else
	{
		PrintInfo(x, y + 9, "       ���ε÷֣�", rank, 0xa);
	}
	if (ncode == 0)
	{
		PrintPoint(x, y + 10, "        ���¿�ʼ        ", 0x8b);
		PrintPoint(x, y + 11, "        �ص���ҳ        ", 0xb);
	}
	if (ncode == 1)
	{
		PrintPoint(x, y + 10, "        ���¿�ʼ        ", 0xb);
		PrintPoint(x, y + 11, "        �ص���ҳ        ", 0x8b);
	}
	PrintPoint(x, y + 12, "     ���� ���� ���� ", color);
	PrintPoint(x, y + 13, "      ������������", color);
}
void PrintPauseText(int x, int y, int ncode)
{
	switch (ncode)
	{
	case 0:
		PrintPoint(x, y, "��������Ϸ��", 0xa);
		PrintPoint(x, y + 2, "�����¿�ʼ��", 2);
		PrintPoint(x, y + 4, "���ص���ҳ��", 2);
		break;
	case 1:
		PrintPoint(x, y, "��������Ϸ��", 2);
		PrintPoint(x, y + 2, "�����¿�ʼ��", 0xa);
		PrintPoint(x, y + 4, "���ص���ҳ��", 2);
		break;
	case 2:
		PrintPoint(x, y, "��������Ϸ��", 2);
		PrintPoint(x, y + 2, "�����¿�ʼ��", 2);
		PrintPoint(x, y + 4, "���ص���ҳ��", 0xa);
		break;
	case 3:
		PrintPoint(x, y, "             ", 0xa);
		PrintPoint(x, y + 2, "             ", 0xa);
		PrintPoint(x, y + 4, "             ", 0xa);
		break;
	default:
		break;
	}
}
int PrintSaveGameText(int x, int y, int code)
{
	PrintPoint(x, y, "�Ƿ񱣴���ȣ�", 0xa);
	PrintPoint(x, y + 2, "    ���ǡ�    ", 0xa);
	PrintPoint(x, y + 4, "    ����    ", 2);

	int nkey;

	while (1)
	{
		nkey = _gettch();

		switch (nkey)
		{
		case 'w':case 'W':case 72:
			if (code > 2) code--;
			break;
		case 's':case 'S':case 80:
			if (code < 3) code++;
			break;
		case 13:case 32:
			return code;
		default:
			break;
		}

		switch (code)
		{
		case 2:
			PrintPoint(x, y, "�Ƿ񱣴���ȣ�", 0xa);
			PrintPoint(x, y + 2, "    ���ǡ�    ", 0xa);
			PrintPoint(x, y + 4, "    ����    ", 2);
			break;
		case 3:
			PrintPoint(x, y, "�Ƿ񱣴���ȣ�", 0xa);
			PrintPoint(x, y + 2, "    ���ǡ�    ", 2);
			PrintPoint(x, y + 4, "    ����    ", 0xa);
			break;
		default:
			break;
		}
	}

}
void PrintRank(int x, int y, int color)
{
	Ranking MyRank;
	PrintPoint(x, y, "��Ӣ�����а�", color);
	PrintInfo(x, y + 2, "��ͨģʽ��", MyRank.GetNormalRank(), color);
	PrintInfo(x, y + 4, "��ǽģʽ��", MyRank.GetWearRank(), color);
}
void PrintTilteMenu(int ncode, int x, int y)
{
	switch (ncode)
	{
	case 0:
		PrintPoint(x, y, "��ʼ��Ϸ(��ͨģʽ)", 0xb);
		PrintPoint(x, y + 2, "��ʼ��Ϸ(��ǽģʽ)", 3);
		PrintPoint(x, y + 4, "�Զ�����Ϸ", 3);
		PrintPoint(x, y + 6, "������Ϸ", 3);
		break;
	case 1:
		PrintPoint(x, y, "��ʼ��Ϸ(��ͨģʽ)", 3);
		PrintPoint(x, y + 2, "��ʼ��Ϸ(��ǽģʽ)", 0xb);
		PrintPoint(x, y + 4, "�Զ�����Ϸ", 3);
		PrintPoint(x, y + 6, "������Ϸ", 3);
		break;
	case 2:
		PrintPoint(x, y, "��ʼ��Ϸ(��ͨģʽ)", 3);
		PrintPoint(x, y + 2, "��ʼ��Ϸ(��ǽģʽ)", 3);
		PrintPoint(x, y + 4, "�Զ�����Ϸ", 0xb);
		PrintPoint(x, y + 6, "������Ϸ", 3);
		break;
	case 3:
		PrintPoint(x, y, "��ʼ��Ϸ(��ͨģʽ)", 3);
		PrintPoint(x, y + 2, "��ʼ��Ϸ(��ǽģʽ)", 3);
		PrintPoint(x, y + 4, "�Զ�����Ϸ", 3);
		PrintPoint(x, y + 6, "������Ϸ", 0xb);
		break;
	default:
		break;
	}

}

//�������ܺ���
int GetRandNum(int Min,int Max)
{
	//srand(time(NULL));
	return rand() % Max + Min;
}
bool IsHaveFile(int GameMode)
{
	FILE *pfile;
	char *szFileName = GameMode == 0 ? "Normal.sav" : "Wear.sav";
	fopen_s(&pfile, szFileName, "rb");
	if (pfile == NULL)
	{
		return false;
	}
	fclose(pfile);
	return true;
}
bool IsHaveMap(int MapNum)
{
	FILE *pfile;
	char *szFileName = MapNum == 3 ? "Map1.dat" : "Map2.dat";
	fopen_s(&pfile, szFileName, "rb");
	if (pfile == NULL)
	{
		return false;
	}
	fclose(pfile);
	return true;
}
bool DeleteMap(int MapNum)
{
	char *szFileName = MapNum == 1 ? "Map1.dat" : "Map2.dat";
	int nRet = remove(szFileName);
	return nRet ==0?true:false;//ɾ���ɹ�����0��ʧ��-1
}
void PlayMusic(int type)
{
	switch (type)
	{
	case 0:
		PlaySoundA("sound\\snake.wav", NULL, SND_ASYNC | SND_NODEFAULT);
		break;
	case 1:
		PlaySoundA("sound\\food.wav", NULL, SND_ASYNC | SND_NODEFAULT);
		break;
	case 2:
		PlaySoundA("sound\\att.wav", NULL, SND_ASYNC | SND_NODEFAULT);
		break;
	case 88:
		break;
	default:
		break;
	}

}

//���ڲ˵�ѡ������ĺ���
int SelectMap(int x, int y)
{
	int nCode = 3;
	PrintEmptyFrame(50, 31, 7, 10, 0xb);
	
	PrintPoint(x, y, "����ͼ1", 0xb);
	PrintPoint(x, y + 2, "  ��ͼ2", 3);
	PrintPoint(x, y + 4, "  ����", 3);

	while (1)
	{
		int nkey;

		nkey = _gettch();
		switch (nkey)
		{
		case 'w':case 'W':case 72:
			if (nCode > 3) { nCode = nCode--; }
			break;
		case 's':case 'S':case 80:
			if (nCode < 5) { nCode = nCode++; }
			break;
		case 13:case 32:
			if (nCode == 3 || nCode == 4)//ѡ���ͼ1����2
			{
				bool nRet = IsHaveMap(nCode);
				if (nRet == false)
				{
					PrintPoint(x , y - 1, "��ͼ������", 0xc);
					break;
				}
			}
			PrintEmptyFrame(50, 31, 7, 10, 0xb,"  ");//����������
			PrintEmptyFrame(20, 31, 30, 10, 0xb);//�ػ���һ�����

			PrintPoint(x, y, "        ", 3);//����˵�����
			PrintPoint(x, y + 2, "        ", 3);
			PrintPoint(x, y + 4, "        ", 3);
			
			PrintPoint(x, y - 1, "           ", 0xc);

			return nCode;
			break;
		}

		switch (nCode)
		{
		case 3:
			PrintPoint(x, y, "����ͼ1", 0xb);
			PrintPoint(x, y + 2, "  ��ͼ2", 3);
			PrintPoint(x, y + 4, "  ����", 3);

			break;
		case 4:
			PrintPoint(x, y, "  ��ͼ1", 3);
			PrintPoint(x, y + 2, "����ͼ2", 0xb);
			PrintPoint(x, y + 4, "  ����", 3);
			break;
		case 5:
			PrintPoint(x, y, "  ��ͼ1", 3);
			PrintPoint(x, y + 2, "  ��ͼ2", 3);
			PrintPoint(x, y + 4, "������", 0xb);
			break;
		}

	}
}
int SelectNewGame(int x, int y,int GameMode)
{
	int nCode = 0;
		
		PrintEmptyFrame(20, 31, 30, 10, 0xb);

		PrintPoint(x, y, "��������һ����Ϸ", 0xb);
		PrintPoint(x,y+2, "  �µ���Ϸ(����)", 3);
		PrintPoint(x, y + 4, "  �µ���Ϸ(˫��)", 3);
		PrintPoint(x, y + 6, "  �µ��Զ�����Ϸ", 3);

		while (1)
		{
			int nkey;

			nkey = _gettch();
			switch (nkey)
			{
			case 'w':case 'W':case 72:
				if (nCode > 0) { nCode = nCode--; }
				break;
			case 's':case 'S':case 80:
				if (nCode < 3) { nCode = nCode++; }
				break;
			case 13:case 32:
				if (nCode==0)//ѡ���������
				{
					bool nRet = IsHaveFile(GameMode);
					if (nRet == false)
					{
						PrintPoint(x + 1, y - 1, "�浵�ļ������ڣ�", 0xc);
						break;
					}
				}
				PrintPoint(x +1, y - 1, "                    ", 0xc);

				if (nCode == 3)//ѡ�������ͼ
				{
					nCode=SelectMap(52,33);
				}

				if (nCode == 5) { nCode = 3; break; }
				return GameMode==0?nCode+10: nCode+20;

			}


			switch (nCode)
			{
			case 0:
				PrintPoint(x, y, "��������һ����Ϸ", 0xb);
				PrintPoint(x, y + 2, "  �µ���Ϸ(����)", 3);
				PrintPoint(x, y + 4, "  �µ���Ϸ(˫��)", 3);
				PrintPoint(x, y + 6, "  �µ��Զ�����Ϸ", 3);

				break;
			case 1:
				PrintPoint(x, y, "  ������һ����Ϸ", 3);
				PrintPoint(x, y + 2, "���µ���Ϸ(����)", 0xb);
				PrintPoint(x, y + 4, "  �µ���Ϸ(˫��)", 3);
				PrintPoint(x, y + 6, "  �µ��Զ�����Ϸ", 3);

				break;
			case 2:
				PrintPoint(x, y, "  ������һ����Ϸ", 3);
				PrintPoint(x, y + 2, "  �µ���Ϸ(����)", 3);
				PrintPoint(x, y + 4, "���µ���Ϸ(˫��)", 0xb);
				PrintPoint(x, y + 6, "  �µ��Զ�����Ϸ", 3);

				break;
			case 3:
				PrintPoint(x, y, "  ������һ����Ϸ", 3);
				PrintPoint(x, y + 2, "  �µ���Ϸ(����)", 3);
				PrintPoint(x, y + 4, "  �µ���Ϸ(˫��)", 3);
				PrintPoint(x, y + 6, "���µ��Զ�����Ϸ", 0xb);

				break;
			}

		}
}
int SelectCustomGame(int x, int y)
{
	int nCode = 0;
	PrintEmptyFrame(20, 31, 30, 10, 0xb);

	PrintPoint(x, y, "��������ͼ", 0xb);
	PrintPoint(x, y + 2, "  ɾ����ͼ1", 3);
	PrintPoint(x, y + 4, "  ɾ����ͼ2", 3);

	while (1)
	{
		int nkey;

		nkey = _gettch();

		PrintPoint(x+1, y - 1, "             ", 0xc);//�����ʾ����

		switch (nkey)
		{
		case 'w':case 'W':case 72:
			if (nCode > 0) { nCode = nCode--; }
			break;
		case 's':case 'S':case 80:
			if (nCode < 2) { nCode = nCode++; }
			break;
		case 13:case 32:
			if (nCode ==0 )	return 30 + nCode;
			bool bRet=DeleteMap(nCode);
			if(bRet)PrintPoint(x + 1, y - 1, "ɾ���ɹ���", 0xc);
			else PrintPoint(x + 1, y - 1, "��ͼ������", 0xc);
			break;
		}

		switch (nCode)
		{
		case 0:
			PrintPoint(x, y, "��������ͼ", 0xb);
			PrintPoint(x, y + 2, "  ɾ����ͼ1", 3);
			PrintPoint(x, y + 4, "  ɾ����ͼ2", 3);

			break;
		case 1:
			PrintPoint(x, y, "  ������ͼ", 3);
			PrintPoint(x, y + 2, "��ɾ����ͼ1", 0xb);
			PrintPoint(x, y + 4, "  ɾ����ͼ2", 3);
			break;
		case 2:
			PrintPoint(x, y, "  ������ͼ", 3);
			PrintPoint(x, y + 2, "  ɾ����ͼ1", 3);
			PrintPoint(x, y + 4, "��ɾ����ͼ2", 0xb);
			break;
		}

	}
}
int HomePage()
{
	int nCode = 0;

	PrintTilteFrame(7);

	PrintTilteText(16,10);

	PrintEmptyFrame(24,21,10,8,14);
	PrintRank(26, 23, 14);

	PrintEmptyFrame(20, 31, 18,10, 0xb);
	PrintTilteMenu(nCode, 25, 33);

	while (1)
	{
		int nkey;

		nkey = _gettch();
		switch (nkey)
		{
		case 'w':case 'W':case 72:
			if (nCode > 0) { nCode = nCode--; }			
			break;
		case 's':case 'S':case 80:
			if (nCode <3) { nCode = nCode++; }
			break;
		case 13:case 32:
			if (nCode ==0 || nCode == 1 ) { nCode = SelectNewGame(40,33,nCode); }
			if (nCode == 2) { nCode = SelectCustomGame(40, 33 ); }
			return nCode;
		default:
			
			//cout << nkey;
			break;			
		}

		switch (nCode)
		{
		case 0:
			PrintTilteMenu(nCode, 25, 33);
			break;
		case 1:
			PrintTilteMenu(nCode, 25, 33);
			break;
		case 2:
			PrintTilteMenu(nCode, 25, 33);
			break;
		case 3:
			PrintTilteMenu(nCode, 25, 33);
			break;
		}

	}

}
int PauseGame(bool pause,bool partner, bool custom)
{
	if (pause == false) return -1;

	int nkey;
	int ncode = 0;
	PrintEmptyFrame(45, 35, 14, 8, 0xa, "��");

	PrintPauseText(49, 37, ncode);


	while (pause)
	{
		nkey = _gettch();
		
		switch (nkey)
		{
		case 'w':case 'W':case 72:
			if (ncode > 0) ncode--;
			break;
		case 's':case 'S':case 80:
			if (ncode <2) ncode++;
			break;
		case 13:case 32:
			if(ncode==2 && partner==false && custom==false) ncode = PrintSaveGameText(49, 37, ncode);
			if (ncode == 2 && partner || ncode == 2 && custom)ncode = 3;
			PrintPauseText(49, 37, 3);
			PrintEmptyFrame(45, 35, 14, 8, 0xa, "  ");

			return ncode;
		}

		PrintPauseText(49, 37, ncode);

	}
	return -1;
}
int GameOver(int rank, bool partner, int loser = 0)
{
	if (partner)
	{
		PrintWinLoseText(6, 15, loser == 0 ? 0 : 1);
		PrintWinLoseText(33, 15, loser == 0 ? 1 : 0);
	}

	PrintGameOverText(16, 15, 0, rank, 0xf, partner, loser);

	int nCode = 0;

	while (1)
	{
		int nkey;

		nkey = _gettch();
		switch (nkey)
		{
		case 'w':case 'W':case 72:case 'i':case 'I':
			if (nCode > 0) { nCode = nCode--; }
			break;
		case 's':case 'S':case 80:case 'k':case 'K':
			if (nCode < 1) { nCode = nCode++; }
			break;
		case 13:case 32:
			return nCode;
		default:
			//cout << nkey;
			break;
		}

		PrintGameOverText(16, 15, nCode, rank, 0xf, partner, loser);
	}

}

