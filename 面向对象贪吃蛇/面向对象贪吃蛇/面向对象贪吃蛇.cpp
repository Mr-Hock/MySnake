#include "stdafx.h"
#include <iostream>
#include <windows.h>
#include <conio.h>
#include <vector>
#include <time.h>
#include "MyHead.h"
#pragma comment(lib, "winmm.lib")

#define DIR_UP 0
#define DIR_DOWN 1
#define DIR_LEFT 2
#define DIR_RIGHT 3

#define WALL_WIDE 45
#define WALL_HIGH 45

#define MODE_NORMAL 0
#define MODE_WEAR 1

#define FOOD_ADDME 0//增加自己长度
#define FOOD_SUBME 1//减短自己长度
#define FOOD_ADDPT 2//增加对方长度
#define FOOD_SUBPT 3//减短对方长度
#define FOOD_BACKPT 4//使对方反方向
#define FOOD_STOPPT 5//使对方无法转弯
#define FOOD_ADDBULLET 6//获得一个炮弹

using namespace std;
using std::vector;

//动态数组，用于储存每个类的坐标点数组
vector <POINTDATA*> pSnake;
vector <POINTDATA*> pSnake2;
vector <POINTDATA*> pFood;
vector <POINTDATA*> pWall;
vector <POINTDATA*> pIce;
vector <POINTDATA*> pBullet;
vector <POINTDATA*> pMap;

Ranking MyRank;

//主要游戏类
class Bullet
{
public:
	bool LaunchBullet(int x,int y,int dir)//发炮
	{
		switch (dir)
		{
		case DIR_RIGHT:
			if (x + 2 >= WALL_WIDE - 1)//如果在墙壁或者障碍物前面不能发炮
				return false;
			if(IsWall(x,y)|| IsWall(x+1, y)|| IsWall(x+2, y) || IsBullet(x +2, y))
				return false;
			break;
		case DIR_LEFT:
			if (x - 2 <= 0)
				return false;
			if (IsWall(x, y) || IsWall(x - 1, y) || IsWall(x - 2, y) || IsBullet(x - 2, y))
				return false;
			break;
		case DIR_DOWN:
			if (y + 2 >= WALL_HIGH)
				return false;
			if (IsWall(x, y) || IsWall(x , y+1) || IsWall(x , y+2) || IsBullet(x , y+2))
				return false;
			break;
		case DIR_UP:
			if (y - 2 <= 0)
				return false;
			if (IsWall(x, y) || IsWall(x, y - 1) || IsWall(x, y - 2) || IsBullet(x , y-2))
				return false;
			break;
		default:
			break;
		}
		//炮弹打出位置从蛇头前进方向+2
		if (dir==DIR_UP)pBullet.push_back(new POINTDATA(x , y - 2, 0xc, "○", dir));
		else if (dir == DIR_DOWN)pBullet.push_back(new POINTDATA(x , y + 2, 0xc, "○", dir));
		else if (dir == DIR_LEFT)pBullet.push_back(new POINTDATA(x-2, y , 0xc, "○", dir));
		else if (dir == DIR_RIGHT)pBullet.push_back(new POINTDATA(x + 2, y, 0xc, "○", dir));
		return true;
	}
	void MoveBulletPoint()//炮弹的移动，和蛇身类似，擦除前一个，然后再打印出来
	{
		if (pBullet.size() == 0)return;

		//要用倒序循环，不然删除的时候会影响后续判断的数组元素位置
		for (int i= pBullet.size()-1;i>=0;i--)
		{
			if (pBullet[i]->m_type == DIR_UP)//按方向判断
			{
				PrintPoint(pBullet[i]->m_x, pBullet[i]->m_y, "  ", pBullet[i]->m_color);//擦除前一个坐标图案
				pBullet[i]->m_y--;
				if (pBullet[i]->m_y <= 0 || IsWall(pBullet[i]->m_x, pBullet[i]->m_y))//如果打到墙壁或者障碍物
				{
					pBullet.erase(pBullet.begin()+i);//擦除这个炮弹
				}

			}
			else if (pBullet[i]->m_type == DIR_DOWN)
			{
				PrintPoint(pBullet[i]->m_x, pBullet[i]->m_y, "  ", pBullet[i]->m_color);
				pBullet[i]->m_y++;
				if (pBullet[i]->m_y >= WALL_HIGH - 1 || IsWall(pBullet[i]->m_x, pBullet[i]->m_y))
				{
					pBullet.erase(pBullet.begin() + i);
				}
			}
			else if (pBullet[i]->m_type == DIR_LEFT)
			{
				PrintPoint(pBullet[i]->m_x, pBullet[i]->m_y, "  ", pBullet[i]->m_color);
				pBullet[i]->m_x--;
				if (pBullet[i]->m_x <=0 || IsWall(pBullet[i]->m_x, pBullet[i]->m_y))
				{
					pBullet.erase(pBullet.begin() + i);
				}
			}
			else if (pBullet[i]->m_type == DIR_RIGHT)
			{
				PrintPoint(pBullet[i]->m_x, pBullet[i]->m_y, "  ", pBullet[i]->m_color);
				pBullet[i]->m_x++;
				if (pBullet[i]->m_x >= WALL_WIDE - 1 || IsWall(pBullet[i]->m_x, pBullet[i]->m_y))
				{
					pBullet.erase(pBullet.begin() + i);
				}
			}

		}
	}
	bool IsWall(int x, int y)//是否打到墙壁或者障碍物了
	{
		for (auto &val : pWall) { if (val->m_x == x && val->m_y == y) return true; }
		return false;
	}
	int IsSnake()//是否打到蛇了，打到1p返回1，2p返回2
	{
		int x = 0;
		int y = 0;
		for (auto &val : pBullet) 
		{ 
			x = val->m_x;
			y = val->m_y;
			for (auto &val : pSnake) { if (val->m_x == x && val->m_y == y) return 1; }
			for (auto &val : pSnake2) { if (val->m_x == x && val->m_y == y) return 2; }
		}

		return 0;
	}
	bool IsBullet(int x, int y)
	{
		for (auto &val : pBullet)
		{
			if (val->m_x == x && val->m_y == y) return true;
		}
		return false;
	}
	void PrintBulletPoint()
	{
		for (auto &val : pBullet) {
			PrintPoint(val->m_x, val->m_y, val->m_data, val->m_color);
		}
	}
};
class Ice
{
public:
	void CreatIce(int x,int y, int wide, int higt )
	{

		for (int w = 0; w < wide; w++)
		{
			for (int h = 0; h < higt; h++)
			{
					pIce.push_back(new POINTDATA(x + w, y + h, 0x9, "○"));
			}
		}
	}
	bool IsOnIce(int x, int y)
	{
		for (auto &val : pIce) { if (val->m_x == x && val->m_y == y) return true; }
		return false;
	}
	void PrintIcePoint()
	{
		for (auto &val : pIce) {
			PrintPoint(val->m_x, val->m_y, val->m_data, val->m_color);
		}
	}
};
class Map
{
	int nType = 0;
	int nColor[10] = { 0x9 ,0xa ,0xb ,0xc,0xd,0xe,0xf,0x9 ,0xb,0xe };
public:
	Map(bool creat = true)
	{
		if (creat == false)return;

		CreatBasicWall(WALL_WIDE, WALL_HIGH);//打印绘图区四周基本墙壁
		PrintEmptyFrame(45, 0, 14, 33, 0xa, "□");//打印工具框
		PrintEmptyFrame(45, 35, 14, 8, 0xa, "□");//打印保存框

		PrintTools();//打印障碍物选项
		PrintPaintTips();//打印操作提示
		PrintButton();//打印保存和退出按钮
	}
	~Map()
	{
		vector <POINTDATA*>().swap(pMap);
	}
	void PrintTools()
	{
		int px = 48;
		int py = 2;
		PrintPoint(px, py, "墙壁类,不可穿透", 0xc);

		PrintPoint(px, py + 2, "→ ■ 蓝色墙壁", 0x9);
		PrintPoint(px, py + 4, "   ■ 绿色墙壁", 0xa);
		PrintPoint(px, py + 6, "   ■ 浅绿墙壁", 0xb);
		PrintPoint(px, py + 8, "   ■ 红色墙壁", 0xc);
		PrintPoint(px, py + 10, "   ■ 紫色墙壁", 0xd);
		PrintPoint(px, py + 12, "   ■ 黄色墙壁", 0xe);
		PrintPoint(px, py + 14, "   ■ 白色墙壁", 0xf);

		PrintPoint(px, py + 16, "冰块类,可穿透加速", 0xc);
		PrintPoint(px, py + 18, "   ○ 加速冰块  ", 0x9);

		PrintPoint(px, py + 20, "蛇类，初始出现位置", 0xc);
		PrintPoint(px, py + 22, "   ● 1P 蛇头  ", 0xb);
		PrintPoint(px, py + 24, "   ● 2P 蛇头  ", 0xe);
	}
	int PrintTools(int x, int y)
	{
		int px = 48;
		int py = 2;

		if (x >= px && y == py + 2)nType = 0;
		else if (x >= px && y == py + 4)nType = 1;
		else if (x >= px && y == py + 6)nType = 2;
		else if (x >= px && y == py + 8)nType = 3;
		else if (x >= px && y == py + 10)nType = 4;
		else if (x >= px && y == py + 12)nType = 5;
		else if (x >= px && y == py + 14)nType = 6;
		else if (x >= px && y == py + 18)nType = 7;
		else if (x >= px && y == py + 22)nType = 8;
		else if (x >= px && y == py + 24)nType = 9;


		switch (nType)
		{
		case 0:
			PrintPoint(px, py, "墙壁类,不可穿透", 0xc);

			PrintPoint(px, py + 2, "→ ■ 蓝色墙壁", 0x9);
			PrintPoint(px, py + 4, "   ■ 绿色墙壁", 0xa);
			PrintPoint(px, py + 6, "   ■ 浅绿墙壁", 0xb);
			PrintPoint(px, py + 8, "   ■ 红色墙壁", 0xc);
			PrintPoint(px, py + 10, "   ■ 紫色墙壁", 0xd);
			PrintPoint(px, py + 12, "   ■ 黄色墙壁", 0xe);
			PrintPoint(px, py + 14, "   ■ 白色墙壁", 0xf);

			PrintPoint(px, py + 16, "冰块类,可穿透加速", 0xc);
			PrintPoint(px, py + 18, "   ○ 加速冰块  ", 0x9);

			PrintPoint(px, py + 20, "蛇类，初始出现位置", 0xc);
			PrintPoint(px, py + 22, "   ● 1P 蛇头  ", 0xb);
			PrintPoint(px, py + 24, "   ● 2P 蛇头  ", 0xe);
			break;

		case 1:
			PrintPoint(px, py, "墙壁类,不可穿透", 0xc);

			PrintPoint(px, py + 2, "   ■ 蓝色墙壁", 0x9);
			PrintPoint(px, py + 4, "→ ■ 绿色墙壁", 0xa);
			PrintPoint(px, py + 6, "   ■ 浅绿墙壁", 0xb);
			PrintPoint(px, py + 8, "   ■ 红色墙壁", 0xc);
			PrintPoint(px, py + 10, "   ■ 紫色墙壁", 0xd);
			PrintPoint(px, py + 12, "   ■ 黄色墙壁", 0xe);
			PrintPoint(px, py + 14, "   ■ 白色墙壁", 0xf);

			PrintPoint(px, py + 16, "冰块类,可穿透加速", 0xc);
			PrintPoint(px, py + 18, "   ○ 加速冰块  ", 0x9);

			PrintPoint(px, py + 20, "蛇类，初始出现位置", 0xc);
			PrintPoint(px, py + 22, "   ● 1P 蛇头  ", 0xb);
			PrintPoint(px, py + 24, "   ● 2P 蛇头  ", 0xe);
			break;

		case 2:
			PrintPoint(px, py, "墙壁类,不可穿透", 0xc);

			PrintPoint(px, py + 2, "   ■ 蓝色墙壁", 0x9);
			PrintPoint(px, py + 4, "   ■ 绿色墙壁", 0xa);
			PrintPoint(px, py + 6, "→ ■ 浅绿墙壁", 0xb);
			PrintPoint(px, py + 8, "   ■ 红色墙壁", 0xc);
			PrintPoint(px, py + 10, "   ■ 紫色墙壁", 0xd);
			PrintPoint(px, py + 12, "   ■ 黄色墙壁", 0xe);
			PrintPoint(px, py + 14, "   ■ 白色墙壁", 0xf);

			PrintPoint(px, py + 16, "冰块类,可穿透加速", 0xc);
			PrintPoint(px, py + 18, "   ○ 加速冰块  ", 0x9);

			PrintPoint(px, py + 20, "蛇类，初始出现位置", 0xc);
			PrintPoint(px, py + 22, "   ● 1P 蛇头  ", 0xb);
			PrintPoint(px, py + 24, "   ● 2P 蛇头  ", 0xe);
			break;
		case 3:
			PrintPoint(px, py, "墙壁类,不可穿透", 0xc);

			PrintPoint(px, py + 2, "   ■ 蓝色墙壁", 0x9);
			PrintPoint(px, py + 4, "   ■ 绿色墙壁", 0xa);
			PrintPoint(px, py + 6, "   ■ 浅绿墙壁", 0xb);
			PrintPoint(px, py + 8, "→ ■ 红色墙壁", 0xc);
			PrintPoint(px, py + 10, "   ■ 紫色墙壁", 0xd);
			PrintPoint(px, py + 12, "   ■ 黄色墙壁", 0xe);
			PrintPoint(px, py + 14, "   ■ 白色墙壁", 0xf);

			PrintPoint(px, py + 16, "冰块类,可穿透加速", 0xc);
			PrintPoint(px, py + 18, "   ○ 加速冰块  ", 0x9);

			PrintPoint(px, py + 20, "蛇类，初始出现位置", 0xc);
			PrintPoint(px, py + 22, "   ● 1P 蛇头  ", 0xb);
			PrintPoint(px, py + 24, "   ● 2P 蛇头  ", 0xe);
			break;
		case 4:
			PrintPoint(px, py, "墙壁类,不可穿透", 0xc);

			PrintPoint(px, py + 2, "   ■ 蓝色墙壁", 0x9);
			PrintPoint(px, py + 4, "   ■ 绿色墙壁", 0xa);
			PrintPoint(px, py + 6, "   ■ 浅绿墙壁", 0xb);
			PrintPoint(px, py + 8, "   ■ 红色墙壁", 0xc);
			PrintPoint(px, py + 10, "→ ■ 紫色墙壁", 0xd);
			PrintPoint(px, py + 12, "   ■ 黄色墙壁", 0xe);
			PrintPoint(px, py + 14, "   ■ 白色墙壁", 0xf);

			PrintPoint(px, py + 16, "冰块类,可穿透加速", 0xc);
			PrintPoint(px, py + 18, "   ○ 加速冰块  ", 0x9);

			PrintPoint(px, py + 20, "蛇类，初始出现位置", 0xc);
			PrintPoint(px, py + 22, "   ● 1P 蛇头  ", 0xb);
			PrintPoint(px, py + 24, "   ● 2P 蛇头  ", 0xe);
			break;
		case 5:
			PrintPoint(px, py, "墙壁类,不可穿透", 0xc);

			PrintPoint(px, py + 2, "   ■ 蓝色墙壁", 0x9);
			PrintPoint(px, py + 4, "   ■ 绿色墙壁", 0xa);
			PrintPoint(px, py + 6, "   ■ 浅绿墙壁", 0xb);
			PrintPoint(px, py + 8, "   ■ 红色墙壁", 0xc);
			PrintPoint(px, py + 10, "   ■ 紫色墙壁", 0xd);
			PrintPoint(px, py + 12, "→ ■ 黄色墙壁", 0xe);
			PrintPoint(px, py + 14, "   ■ 白色墙壁", 0xf);

			PrintPoint(px, py + 16, "冰块类,可穿透加速", 0xc);
			PrintPoint(px, py + 18, "   ○ 加速冰块  ", 0x9);

			PrintPoint(px, py + 20, "蛇类，初始出现位置", 0xc);
			PrintPoint(px, py + 22, "   ● 1P 蛇头  ", 0xb);
			PrintPoint(px, py + 24, "   ● 2P 蛇头  ", 0xe);
			break;
		case 6:
			PrintPoint(px, py, "墙壁类,不可穿透", 0xc);

			PrintPoint(px, py + 2, "   ■ 蓝色墙壁", 0x9);
			PrintPoint(px, py + 4, "   ■ 绿色墙壁", 0xa);
			PrintPoint(px, py + 6, "   ■ 浅绿墙壁", 0xb);
			PrintPoint(px, py + 8, "   ■ 红色墙壁", 0xc);
			PrintPoint(px, py + 10, "   ■ 紫色墙壁", 0xd);
			PrintPoint(px, py + 12, "   ■ 黄色墙壁", 0xe);
			PrintPoint(px, py + 14, "→ ■ 白色墙壁", 0xf);

			PrintPoint(px, py + 16, "冰块类,可穿透加速", 0xc);
			PrintPoint(px, py + 18, "   ○ 加速冰块  ", 0x9);

			PrintPoint(px, py + 20, "蛇类，初始出现位置", 0xc);
			PrintPoint(px, py + 22, "   ● 1P 蛇头  ", 0xb);
			PrintPoint(px, py + 24, "   ● 2P 蛇头  ", 0xe);
			break;
		case 7:
			PrintPoint(px, py, "墙壁类,不可穿透", 0xc);

			PrintPoint(px, py + 2, "   ■ 蓝色墙壁", 0x9);
			PrintPoint(px, py + 4, "   ■ 绿色墙壁", 0xa);
			PrintPoint(px, py + 6, "   ■ 浅绿墙壁", 0xb);
			PrintPoint(px, py + 8, "   ■ 红色墙壁", 0xc);
			PrintPoint(px, py + 10, "   ■ 紫色墙壁", 0xd);
			PrintPoint(px, py + 12, "   ■ 黄色墙壁", 0xe);
			PrintPoint(px, py + 14, "   ■ 白色墙壁", 0xf);

			PrintPoint(px, py + 16, "冰块类,可穿透加速", 0xc);
			PrintPoint(px, py + 18, "→ ○ 加速冰块  ", 0x9);

			PrintPoint(px, py + 20, "蛇类，初始出现位置", 0xc);
			PrintPoint(px, py + 22, "   ● 1P 蛇头  ", 0xb);
			PrintPoint(px, py + 24, "   ● 2P 蛇头  ", 0xe);
			break;
		case 8:
			PrintPoint(px, py, "墙壁类,不可穿透", 0xc);

			PrintPoint(px, py + 2, "   ■ 蓝色墙壁", 0x9);
			PrintPoint(px, py + 4, "   ■ 绿色墙壁", 0xa);
			PrintPoint(px, py + 6, "   ■ 浅绿墙壁", 0xb);
			PrintPoint(px, py + 8, "   ■ 红色墙壁", 0xc);
			PrintPoint(px, py + 10, "   ■ 紫色墙壁", 0xd);
			PrintPoint(px, py + 12, "   ■ 黄色墙壁", 0xe);
			PrintPoint(px, py + 14, "   ■ 白色墙壁", 0xf);

			PrintPoint(px, py + 16, "冰块类,可穿透加速", 0xc);
			PrintPoint(px, py + 18, "   ○ 加速冰块  ", 0x9);

			PrintPoint(px, py + 20, "蛇类，初始出现位置", 0xc);
			PrintPoint(px, py + 22, "→ ● 1P 蛇头  ", 0xb);
			PrintPoint(px, py + 24, "   ● 2P 蛇头  ", 0xe);
			break;
		case 9:
			PrintPoint(px, py, "墙壁类,不可穿透", 0xc);

			PrintPoint(px, py + 2, "   ■ 蓝色墙壁", 0x9);
			PrintPoint(px, py + 4, "   ■ 绿色墙壁", 0xa);
			PrintPoint(px, py + 6, "   ■ 浅绿墙壁", 0xb);
			PrintPoint(px, py + 8, "   ■ 红色墙壁", 0xc);
			PrintPoint(px, py + 10, "   ■ 紫色墙壁", 0xd);
			PrintPoint(px, py + 12, "   ■ 黄色墙壁", 0xe);
			PrintPoint(px, py + 14, "   ■ 白色墙壁", 0xf);

			PrintPoint(px, py + 16, "冰块类,可穿透加速", 0xc);
			PrintPoint(px, py + 18, "   ○ 加速冰块  ", 0x9);

			PrintPoint(px, py + 20, "蛇类，初始出现位置", 0xc);
			PrintPoint(px, py + 22, "   ● 1P 蛇头  ", 0xb);
			PrintPoint(px, py + 24, "→ ● 2P 蛇头  ", 0xe);
			break;
		default:
			break;
		}
		return nType;
	}
	void PrintPaintTips()
	{
		int px = 48;
		int py = 28;
		PrintPoint(px, py, "绘画方法：", 0xc);

		PrintPoint(px, py + 1, "① 右侧选择类块", 0xa);
		PrintPoint(px, py + 2, "② 按鼠标左键绘图", 0xa);
		PrintPoint(px, py + 3, "③ 按鼠标右键擦除", 0xa);

	}
	void PrintButton()
	{
		int px = 48;
		int py = 37;
		PrintPoint(px, py, "保存到地图1    ", 0xa);
		PrintPoint(px, py + 2, "保存到地图2    ", 0xa);
		PrintPoint(px, py + 4, "退出自定义编辑", 0xc);

	}
	int PrintButton(int x, int y, bool success)
	{
		int px = 48;
		int py = 37;
		int nButton = -1;
		if (x >= px && y == py)nButton = 0;
		else if (x >= px && y == py + 2)nButton = 1;
		else if (x >= px && y == py + 4)nButton = 2;

		switch (nButton)
		{
		case 0:
			PrintPoint(px, py, "保存到地图1..OK    ", 0xa);
			if (success == false)PrintPoint(px, py, "保存到地图1..Error", 0xa);
			PrintPoint(px, py + 2, "保存到地图2        ", 0xa);
			PrintPoint(px, py + 4, "退出自定义编辑", 0xc);
			break;
		case 1:
			PrintPoint(px, py, "保存到地图1        ", 0xa);
			PrintPoint(px, py + 2, "保存到地图2..OK    ", 0xa);
			if (success == false)PrintPoint(px, py + 2, "保存到地图2..Error", 0xa);
			PrintPoint(px, py + 4, "退出自定义编辑", 0xc);
			break;
		case 2:
			PrintPoint(px, py, "保存到地图1        ", 0xa);
			PrintPoint(px, py + 2, "保存到地图2        ", 0xa);
			PrintPoint(px, py + 4, "退出自定义编辑", 0xc);
			break;
		default:
			break;
		}
		return nButton;
	}
	void CreatBasicWall(int wide, int higt)
	{
		for (int w = 0; w < wide; w++)
		{
			for (int h = 0; h < higt; h++)
			{
				if (h == 0 || h == higt - 1)
				{
					pMap.push_back(new POINTDATA(w, h, 10, "■", 88));
				}
				else if (w == 0 || w == wide - 1)
				{
					pMap.push_back(new POINTDATA(w, h, 10, "■", 88));
				}

			}
		}
	}
	void PaintMap(int x, int y)
	{
		if (IsMapPoint(x, y))return;

		if (nType <= 6)//小于6都是墙壁，颜色不同而已
		{
			pMap.push_back(new POINTDATA(x, y, nColor[nType], "■", nType));
			PrintPoint(x, y, "■", nColor[nType]);
		}
		else if (nType == 7)//冰块
		{
			pMap.push_back(new POINTDATA(x, y, nColor[nType], "○", nType));
			PrintPoint(x, y, "○", nColor[nType]);
		}
		else if (nType == 8 && IsHaveSnakePoint(1) == false)//1p蛇头   只能存在一个蛇头
		{
			pMap.push_back(new POINTDATA(x, y, nColor[nType], "●", nType));
			PrintPoint(x, y, "●", nColor[nType]);
		}
		else if (nType == 9 && IsHaveSnakePoint(2) == false)//2p蛇头
		{
			pMap.push_back(new POINTDATA(x, y, nColor[nType], "●", nType));
			PrintPoint(x, y, "●", nColor[nType]);
		}

	}
	void EraseWall(int x, int y)
	{
		if (IsMapPoint(x, y) == false)return;

		bool nRet = DeleteWallPoint(x, y);
		if (nRet)PrintPoint(x, y, "  ", 0);

	}
	bool DeleteWallPoint(int x, int y)
	{
		if (pMap.size() == 0)return false;

		for (int i = pMap.size() - 1; i >= 0; i--)
		{
			if (pMap[i]->m_type < 88)
			{
				if (pMap[i]->m_x == x && pMap[i]->m_y == y)
				{
					pMap.erase(pMap.begin() + i);
					return true;
				}
			}
		}
		return false;
	}
	bool IsMapPoint(int x, int y)//是否为有效绘画点
	{
		for (auto &val : pMap)
		{
			if (val->m_x == x && val->m_y == y)return true;
		}
		return false;
	}
	bool IsHaveSnakePoint(int p)//p:1 = 1P  2 = 2P
	{
		for (auto &val : pMap)
		{
			if (p == 1 && val->m_type == 8)return true;
			if (p == 2 && val->m_type == 9)return true;
		}
		return false;
	}
	int GetClickFrame(int x, int y)//返回0：绘画界面 1：工具界面 2：保存界面 -1：无效区域
	{
		if (x > 0 && x < WALL_WIDE && y >0 && y < WALL_HIGH)return 0;
		else if (x > WALL_WIDE && x < WALL_WIDE + 14 && y >0 && y < 35)return 1;
		else if (x > WALL_WIDE && x < WALL_WIDE + 14 && y >35 && y < WALL_HIGH)return 2;
		return -1;
	}
	void PrintBasicWallPoint()//打印全部绘画出来的点
	{
		for (auto &val : pMap) {
			PrintPoint(val->m_x, val->m_y, val->m_data, val->m_color);
		}
	}
	void ClsAll(int wide, int higt)//清除所有点
	{
		for (int w = 1; w < wide - 1; w++)
		{
			for (int h = 1; h < higt - 1; h++)
			{
				PrintPoint(w, h, "  ", 0);
			}
		}
	}
	bool SaveMap(int num)
	{
		if (IsHaveSnakePoint(1) == false) return false;

		//保存前先把坐标分类，压入不同类型的动态数组内
		for (auto &val : pMap)
		{
			if (val->m_type <= 6)//type小于等于6都为墙壁类，只是颜色不同
			{
				pWall.push_back(new POINTDATA(val->m_x, val->m_y, val->m_color, "■", 1));
			}
			else if (val->m_type == 7)//冰块
			{
				pIce.push_back(new POINTDATA(val->m_x, val->m_y, val->m_color, "○"));
			}
			else if (val->m_type == 8)//1p蛇头初始位置
			{
				pSnake.push_back(new POINTDATA(val->m_x, val->m_y, val->m_color, "●"));
			}
			else if (val->m_type == 9)//2p蛇头初始位置
			{
				pSnake2.push_back(new POINTDATA(val->m_x, val->m_y, val->m_color, "●"));
			}
		}

		//然后再按动态数组类型进行保存，先储存数组元素的数量，然后再接着储存数组内的数据。
		FILE *pfile;
		char *szFileName = num == 1 ? "Map1.dat" : "Map2.dat";

		int nsize = 0;

		//储存pSnake数据
		nsize = pSnake.size();
		fopen_s(&pfile, szFileName, "wb");//先用写出方式打开，覆盖原文件
		fwrite(&nsize, 1, sizeof(int), pfile);
		fclose(pfile);
		for (auto &val : pSnake)
		{
			fopen_s(&pfile, szFileName, "ab");//再用追加方式打开，在文件后面继续追加数组数据
			fwrite(val, 1, sizeof(POINTDATA), pfile);
			fclose(pfile);
		}

		//储存pSnake2数据
		nsize = pSnake2.size();
		fopen_s(&pfile, szFileName, "ab");
		fwrite(&nsize, 1, sizeof(int), pfile);
		fclose(pfile);
		for (auto &val : pSnake2)
		{
			fopen_s(&pfile, szFileName, "ab");
			fwrite(val, 1, sizeof(POINTDATA), pfile);
			fclose(pfile);
		}

		//储存pWall数据
		nsize = pWall.size();
		fopen_s(&pfile, szFileName, "ab");
		fwrite(&nsize, 1, sizeof(int), pfile);
		fclose(pfile);
		for (auto &val : pWall)
		{
			fopen_s(&pfile, szFileName, "ab");
			fwrite(val, 1, sizeof(POINTDATA), pfile);
			fclose(pfile);
		}

		//储存pIce数据
		nsize = pIce.size();
		fopen_s(&pfile, szFileName, "ab");
		fwrite(&nsize, 1, sizeof(int), pfile);
		fclose(pfile);
		for (auto &val : pIce)
		{
			fopen_s(&pfile, szFileName, "ab");
			fwrite(val, 1, sizeof(POINTDATA), pfile);
			fclose(pfile);
		}

		return true;
	}
	void OpenMap(int num)//读取地图，
	{

		FILE *pfile;
		char *szFileName = num == 1 ? "Map1.dat" : "Map2.dat";

		int nsize = 0;

		//读取pSnake数据
		fopen_s(&pfile, szFileName, "rb");
		fread(&nsize, 1, sizeof(int), pfile);
		//先读取前4字节,数组的数量，然后for循环依次读入指定数量的数据，放入临时内存里
		//再将temp->m_data重新赋值为图案，因为读出来的是上一次字符串地址，并不是图案

		for (int i = 0; i < nsize; i++)
		{
			POINTDATA *temp = new POINTDATA;//申请一个临时内存空间
			fread(temp, 1, sizeof(POINTDATA), pfile);//读取的数据放入该内存空间
			temp->m_data = "●";//将m_data重新赋值
			pSnake.push_back(new POINTDATA(temp->m_x, temp->m_y, temp->m_color, temp->m_data, temp->m_type));
			delete temp;//最后释放内存空间
		}

		//读取pSnake2数据
		fread(&nsize, 1, sizeof(int), pfile);
		for (int i = 0; i < nsize; i++)
		{
			POINTDATA *temp = new POINTDATA;
			fread(temp, 1, sizeof(POINTDATA), pfile);
			temp->m_data = "●";
			pSnake2.push_back(new POINTDATA(temp->m_x, temp->m_y, temp->m_color, temp->m_data, temp->m_type));
			delete temp;
		}

		//读取pWall数据
		fread(&nsize, 1, sizeof(int), pfile);
		for (int i = 0; i < nsize; i++)
		{
			POINTDATA *temp = new POINTDATA;
			fread(temp, 1, sizeof(POINTDATA), pfile);
			temp->m_data = "■";
			pWall.push_back(new POINTDATA(temp->m_x, temp->m_y, temp->m_color, temp->m_data, temp->m_type));
			delete temp;
		}

		//读取pIce数据
		fread(&nsize, 1, sizeof(int), pfile);
		for (int i = 0; i < nsize; i++)
		{
			POINTDATA *temp = new POINTDATA;
			fread(temp, 1, sizeof(POINTDATA), pfile);
			temp->m_data = "○";
			pIce.push_back(new POINTDATA(temp->m_x, temp->m_y, temp->m_color, temp->m_data, temp->m_type));
			delete temp;
		}

		fclose(pfile);

	}
	void PrintErrorInfo(bool print)
	{
		int px = 48;
		int py = 34;
		if (print)PrintPoint(px, py, "至少初始化1P蛇头", 0xc);
		else PrintPoint(px, py, "                  ", 0xc);


	}
};

class Snake
{
	int m_Dir;
	int m_SnakeLen = 0;
	int m_SnakeLevel = 1;
	bool m_Pause = false;
	int m_Rank=0;
	int m_back = 0;
	int m_stop = 0;
	int m_bullet = 0;
public:
	Snake(int dir = DIR_LEFT) :m_Dir(dir) {};
	Snake(int x, int y, int color, char* data, int dir = DIR_LEFT, bool creatsnake = true) :m_Dir(dir)
	{
		if (creatsnake) { AddPoint(x, y, color, data); }
		//pSnake.push_back(new POINTDATA(x,y,color, data));
	}
	int GetBulletNum()
	{
		return m_bullet;
	}
	void AddBulletNum()
	{
		m_bullet++;
	}
	void SubBulletNum()
	{
		m_bullet--;
	}
	void SetState(int state)//1:反方向 2：禁止转向
	{
		switch (state)
		{
		case 1:
			m_back = time(NULL);
			break;
		case 2:
			m_stop = time(NULL);
			break;
		default:
			break;
		}
	}
	bool GetState(int state)//1:反方向 2：禁止转向
	{
		switch (state)
		{
		case 1:
			if (time(NULL) - m_back >= 5)return false;
			return true;
			break;
		case 2:
			if (time(NULL) - m_stop >= 5)return false;
			return true;
			break;
		default:
			break;
		}
		return false;
	}
	bool GetPause()
	{
		return m_Pause;
	}
	void SetPause(bool stop)
	{
		m_Pause = stop;
	}
	int GetDir()
	{
		return m_Dir;
	}
	void SetDir(int key)
	{
		bool nRet;
		nRet=GetState(1);
		if (nRet)
		{
			switch (key)
			{
			case DIR_UP:key = DIR_DOWN; break;
			case DIR_DOWN:key = DIR_UP; break;
			case DIR_LEFT:key = DIR_RIGHT; break;
			case DIR_RIGHT:key = DIR_LEFT; break;
			}
		}

		nRet = GetState(2);
		if (nRet) return;

		if (m_Dir == DIR_UP && key!= DIR_DOWN) { m_Dir = key; }
		else if (m_Dir == DIR_DOWN && key != DIR_UP) { m_Dir = key; }
		else if (m_Dir == DIR_LEFT && key != DIR_RIGHT) { m_Dir = key; }
		else if (m_Dir == DIR_RIGHT && key != DIR_LEFT) { m_Dir = key; }
	}
	int GetSnakeX()
	{
		return pSnake[0]->m_x;
	}
	int GetSnakeY()
	{
		return pSnake[0]->m_y;
	}
	int GetSnakeLen()
	{
		return m_SnakeLen;
	}
	void AddSnakeLen()
	{
		m_SnakeLen++;
	}
	int GetSnakeSpeed()
	{
		m_SnakeLevel = m_Rank < 100 ? 1 : m_Rank / 100+1;

		if (m_SnakeLevel ==1) { return 150; }
		else if (m_SnakeLevel <= 2) { return 140; }
		else if (m_SnakeLevel <= 3) { return 130; }
		else if (m_SnakeLevel <= 4) { return 120; }
		else if (m_SnakeLevel <= 5) { return 110; }
		else if (m_SnakeLevel <= 6) { return 100; }
		else if (m_SnakeLevel <= 7) { return 90; }
		else if (m_SnakeLevel <= 8) { return 80; }
		else if (m_SnakeLevel <= 9) { return 70; }
		else if (m_SnakeLevel <= 10) { return 60; }
		else if (m_SnakeLevel <= 11) { return 50; }
		else if (m_SnakeLevel <= 12) { return 40; }
		else if (m_SnakeLevel <= 13) { return 30; }
		else if (m_SnakeLevel <= 14) { return 20; }
		else if (m_SnakeLevel <= 15) { return 10; }
		else if (m_SnakeLevel <= 16) { return 5; }
		else if (m_SnakeLevel <= 17) { return 2; }
		return 2;
	}
	int GetSnakeLevel()
	{
/*
		if (m_Rank <= 100) { return 1; }
		else if (m_Rank <= 200) { return 2; }
		else if (m_Rank <= 300) { return 3; }
		else if (m_Rank <= 400) { return 4; }
		else if (m_Rank <= 500) { return 5; }
		else if (m_Rank <= 600) { return 6; }
		else if (m_Rank <= 700) { return 7; }
		else if (m_Rank <= 800) { return 8; }
		else if (m_Rank <= 900) { return 9; }*/
		return m_SnakeLevel;
	}
	int GetRank()
	{
		return m_Rank;
	}
	void AddRank()
	{
		m_Rank+=10;
	}
	void SetRank(int rank)
	{
		m_Rank = rank;
	}
	void AddPoint(int x, int y, int color, char* data= "●")
	{
		pSnake.push_back(new POINTDATA(x, y, color, data));
		m_SnakeLen++;
	}
	void SubPoint(int color)
	{
		int last=pSnake.size()-1;
		if (last <= 0)return;
		PrintPoint(pSnake[last]->m_x, pSnake[last]->m_y, "  ", color);
		pSnake.pop_back();
		m_SnakeLen--;
	}
	void MovePoint()
	{
		{
			switch (m_Dir)
			{
			case DIR_RIGHT:
			{
				int last_m_x = pSnake[pSnake.size() - 1]->m_x;
				int last_m_y = pSnake[pSnake.size() - 1]->m_y;
				int last_m_color = pSnake[pSnake.size() - 1]->m_color;
				char* last_m_data = pSnake[pSnake.size() - 1]->m_data;

				PrintPoint(last_m_x, last_m_y, "  ", last_m_color);

				int first_m_x = pSnake[0]->m_x;
				int first_m_y = pSnake[0]->m_y;
				int first_m_color = pSnake[0]->m_color;
				char* first_m_data = pSnake[0]->m_data;

				pSnake.erase(pSnake.end() - 1);


				pSnake.insert(pSnake.begin(), new POINTDATA(first_m_x + 1, first_m_y, first_m_color, first_m_data));
				break;
			}
			case DIR_LEFT:
			{
				int last_m_x = pSnake[pSnake.size() - 1]->m_x;
				int last_m_y = pSnake[pSnake.size() - 1]->m_y;
				int last_m_color = pSnake[pSnake.size() - 1]->m_color;
				char* last_m_data = pSnake[pSnake.size() - 1]->m_data;

				PrintPoint(last_m_x, last_m_y, "  ", last_m_color);

				int first_m_x = pSnake[0]->m_x;
				int first_m_y = pSnake[0]->m_y;
				int first_m_color = pSnake[0]->m_color;
				char* first_m_data = pSnake[0]->m_data;

				pSnake.erase(pSnake.end() - 1);

				pSnake.insert(pSnake.begin(), new POINTDATA(first_m_x - 1, first_m_y, first_m_color, first_m_data));
				break;
			}
			case DIR_UP:
			{
				int last_m_x = pSnake[pSnake.size() - 1]->m_x;
				int last_m_y = pSnake[pSnake.size() - 1]->m_y;
				int last_m_color = pSnake[pSnake.size() - 1]->m_color;
				char* last_m_data = pSnake[pSnake.size() - 1]->m_data;

				PrintPoint(last_m_x, last_m_y, "  ", last_m_color);

				int first_m_x = pSnake[0]->m_x;
				int first_m_y = pSnake[0]->m_y;
				int first_m_color = pSnake[0]->m_color;
				char* first_m_data = pSnake[0]->m_data;

				pSnake.erase(pSnake.end() - 1);

				pSnake.insert(pSnake.begin(), new POINTDATA(first_m_x, first_m_y-1, first_m_color, first_m_data));
				break;
			}
			case DIR_DOWN:
			{
				int last_m_x = pSnake[pSnake.size() - 1]->m_x;
				int last_m_y = pSnake[pSnake.size() - 1]->m_y;
				int last_m_color = pSnake[pSnake.size() - 1]->m_color;
				char* last_m_data = pSnake[pSnake.size() - 1]->m_data;

				PrintPoint(last_m_x, last_m_y, "  ", last_m_color);

				int first_m_x = pSnake[0]->m_x;
				int first_m_y = pSnake[0]->m_y;
				int first_m_color = pSnake[0]->m_color;
				char* first_m_data = pSnake[0]->m_data;

				pSnake.erase(pSnake.end() - 1);

				pSnake.insert(pSnake.begin(), new POINTDATA(first_m_x, first_m_y + 1, first_m_color, first_m_data));
				break;
			}
			default:
				break;
			}

		}
	}
	bool IsTouchSnake(int x, int y)
	{
		int i = 0;
		for (auto &val : pSnake)
		{
			if (i == 0) {
				i++; continue;
			}
			if (val->m_x == x && val->m_y == y)
			{
				return true;
			}
		}
		return false;
	}
	bool IsTouchPartnerSnake(int x, int y)
	{
		int i = 0;
		for (auto &val : pSnake2)
		{
			if (i == 0) {
				i++; continue;
			}
			if (val->m_x == x && val->m_y == y)
			{
				return true;
			}
		}
		return false;
	}
	void PrintSnakePoint()
	{
		for (auto &val : pSnake) 
		{
			PrintPoint(val->m_x, val->m_y, val->m_data, val->m_color); 
		}
	}
	bool WearWall()
	{
		//取出蛇头x和y，蛇头位于动态数组的首位置。

		//如果蛇头在右侧墙壁边缘，就将蛇头x位置直接置为左侧墙壁边缘位置1
		if (pSnake[0]->m_x == WALL_WIDE - 1) { pSnake[0]->m_x = 1; return true; }

		//如果蛇头在左侧墙壁边缘，就将蛇头x位置直接置为右侧墙壁边缘位置
		else if (pSnake[0]->m_x == 0) { pSnake[0]->m_x = WALL_WIDE - 2; return true;}

		//如果蛇头在上面墙壁边缘，就将蛇头y位置直接置为底部墙壁边缘位置
		else if (pSnake[0]->m_y == 0) {pSnake[0]->m_y = WALL_HIGH - 2; return true;}

		//如果蛇头在下面墙壁边缘，就将蛇头y位置直接置为上面墙壁边缘位置=1
		else if (pSnake[0]->m_y == WALL_HIGH - 1) {pSnake[0]->m_y = 1; return true;}
		return false;
	}
};
class Snake2
{
	int m_Dir;
	int m_SnakeLen = 0;
	int m_SnakeLevel = 1;
	bool m_Pause = false;
	int m_Rank = 0;
	int m_bullet = 0;
	int m_back = 0;//反向时间
	int m_stop = 0;//禁止转向时间
public:
	int GetBulletNum()
	{
		return m_bullet;
	}
	void AddBulletNum()
	{
		m_bullet++;
	}
	void SubBulletNum()
	{
		m_bullet--;
	}
	void SetState(int state)//置入异常状态
	{
		switch (state)
		{
		case 1://1:反方向 2：禁止转向
			m_back = time(NULL);//取出现在的时间，赋值
			break;
		case 2:
			m_stop = time(NULL);
			break;
		default:
			break;
		}
	}
	bool GetState(int state)//取异常是否有效
	{
		switch (state)//1:反方向 2：禁止转向
		{
		case 1://取出现在的时间与设置的时间判断，大于5秒则失效
			if (time(NULL) - m_back >= 5)return false;
			return true;
			break;
		case 2:
			if (time(NULL) - m_stop >= 5)return false;
			return true;
			break;
		default:
			break;
		}
		return false;
	}
	bool GetPause()
	{
		return m_Pause;
	}
	Snake2(int dir = DIR_LEFT) :m_Dir(dir) {};
	Snake2(int x, int y, int color, char* data, int dir = DIR_LEFT, bool creatsnake = true) :m_Dir(dir)
	{
		if (creatsnake) { AddPoint(x, y, color, data); }
		//pSnake2.push_back(new POINTDATA(x,y,color, data));
	}
	void SetPause(bool stop)
	{
		m_Pause = stop;
	}
	int GetDir()
	{
		return m_Dir;
	}
	void SetDir(int key)
	{
		bool nRet;
		nRet = GetState(1);
		if (nRet)
		{
			switch (key)
			{
			case DIR_UP:key = DIR_DOWN; break;
			case DIR_DOWN:key = DIR_UP; break;
			case DIR_LEFT:key = DIR_RIGHT; break;
			case DIR_RIGHT:key = DIR_LEFT; break;
			}
		}

		nRet = GetState(2);
		if (nRet) return;

		if (m_Dir == DIR_UP && key != DIR_DOWN) { m_Dir = key; }
		else if (m_Dir == DIR_DOWN && key != DIR_UP) { m_Dir = key; }
		else if (m_Dir == DIR_LEFT && key != DIR_RIGHT) { m_Dir = key; }
		else if (m_Dir == DIR_RIGHT && key != DIR_LEFT) { m_Dir = key; }
	}

	int GetSnakeX()
	{
		return pSnake2[0]->m_x;
	}
	int GetSnakeY()
	{
		return pSnake2[0]->m_y;
	}
	void AddSnakeLen()
	{
		m_SnakeLen++;
	}
	int GetSnakeLen()
	{
		return m_SnakeLen;
	}
	int GetSnakeSpeed()
	{
		m_SnakeLevel = m_Rank < 100 ? 1 : m_Rank / 100 + 1;

		if (m_SnakeLevel == 1) { return 150; }
		else if (m_SnakeLevel <= 2) { return 140; }
		else if (m_SnakeLevel <= 3) { return 130; }
		else if (m_SnakeLevel <= 4) { return 120; }
		else if (m_SnakeLevel <= 5) { return 110; }
		else if (m_SnakeLevel <= 6) { return 100; }
		else if (m_SnakeLevel <= 7) { return 90; }
		else if (m_SnakeLevel <= 8) { return 80; }
		else if (m_SnakeLevel <= 9) { return 70; }
		else if (m_SnakeLevel <= 10) { return 60; }
		else if (m_SnakeLevel <= 11) { return 50; }
		else if (m_SnakeLevel <= 12) { return 40; }
		else if (m_SnakeLevel <= 13) { return 30; }
		else if (m_SnakeLevel <= 14) { return 20; }
		else if (m_SnakeLevel <= 15) { return 10; }
		else if (m_SnakeLevel <= 16) { return 5; }
		else if (m_SnakeLevel <= 17) { return 2; }
		return 2;
	}

	int GetSnakeLevel()
	{
		/*
		if (m_Rank <= 100) { return 1; }
		else if (m_Rank <= 200) { return 2; }
		else if (m_Rank <= 300) { return 3; }
		else if (m_Rank <= 400) { return 4; }
		else if (m_Rank <= 500) { return 5; }
		else if (m_Rank <= 600) { return 6; }
		else if (m_Rank <= 700) { return 7; }
		else if (m_Rank <= 800) { return 8; }
		else if (m_Rank <= 900) { return 9; }*/
		return m_SnakeLevel;
	}
	int GetRank()
	{
		return m_Rank;
	}
	void AddRank()
	{
		m_Rank += 10;
	}
	void SetRank(int rank)
	{
		m_Rank = rank;
	}

	void AddPoint(int x, int y, int color, char* data = "●")
	{
		pSnake2.push_back(new POINTDATA(x, y, color, data));
		m_SnakeLen++;
	}
	void SubPoint(int color)
	{
		int last = pSnake2.size() - 1;
		if (last <= 0)return;
		PrintPoint(pSnake2[last]->m_x, pSnake2[last]->m_y, "  ", color);
		pSnake2.pop_back();
		m_SnakeLen--;
	}

	void MovePoint()
	{
		{
			switch (m_Dir)
			{
			case DIR_RIGHT:
			{
				int last_m_x = pSnake2[pSnake2.size() - 1]->m_x;
				int last_m_y = pSnake2[pSnake2.size() - 1]->m_y;
				int last_m_color = pSnake2[pSnake2.size() - 1]->m_color;
				char* last_m_data = pSnake2[pSnake2.size() - 1]->m_data;

				PrintPoint(last_m_x, last_m_y, "  ", last_m_color);

				int first_m_x = pSnake2[0]->m_x;
				int first_m_y = pSnake2[0]->m_y;
				int first_m_color = pSnake2[0]->m_color;
				char* first_m_data = pSnake2[0]->m_data;

				pSnake2.erase(pSnake2.end() - 1);


				pSnake2.insert(pSnake2.begin(), new POINTDATA(first_m_x + 1, first_m_y, first_m_color, first_m_data));
				break;
			}
			case DIR_LEFT:
			{
				int last_m_x = pSnake2[pSnake2.size() - 1]->m_x;
				int last_m_y = pSnake2[pSnake2.size() - 1]->m_y;
				int last_m_color = pSnake2[pSnake2.size() - 1]->m_color;
				char* last_m_data = pSnake2[pSnake2.size() - 1]->m_data;

				PrintPoint(last_m_x, last_m_y, "  ", last_m_color);

				int first_m_x = pSnake2[0]->m_x;
				int first_m_y = pSnake2[0]->m_y;
				int first_m_color = pSnake2[0]->m_color;
				char* first_m_data = pSnake2[0]->m_data;

				pSnake2.erase(pSnake2.end() - 1);

				pSnake2.insert(pSnake2.begin(), new POINTDATA(first_m_x - 1, first_m_y, first_m_color, first_m_data));
				break;
			}
			case DIR_UP:
			{
				int last_m_x = pSnake2[pSnake2.size() - 1]->m_x;
				int last_m_y = pSnake2[pSnake2.size() - 1]->m_y;
				int last_m_color = pSnake2[pSnake2.size() - 1]->m_color;
				char* last_m_data = pSnake2[pSnake2.size() - 1]->m_data;

				PrintPoint(last_m_x, last_m_y, "  ", last_m_color);

				int first_m_x = pSnake2[0]->m_x;
				int first_m_y = pSnake2[0]->m_y;
				int first_m_color = pSnake2[0]->m_color;
				char* first_m_data = pSnake2[0]->m_data;

				pSnake2.erase(pSnake2.end() - 1);

				pSnake2.insert(pSnake2.begin(), new POINTDATA(first_m_x, first_m_y - 1, first_m_color, first_m_data));
				break;
			}
			case DIR_DOWN:
			{
				int last_m_x = pSnake2[pSnake2.size() - 1]->m_x;
				int last_m_y = pSnake2[pSnake2.size() - 1]->m_y;
				int last_m_color = pSnake2[pSnake2.size() - 1]->m_color;
				char* last_m_data = pSnake2[pSnake2.size() - 1]->m_data;

				PrintPoint(last_m_x, last_m_y, "  ", last_m_color);

				int first_m_x = pSnake2[0]->m_x;
				int first_m_y = pSnake2[0]->m_y;
				int first_m_color = pSnake2[0]->m_color;
				char* first_m_data = pSnake2[0]->m_data;

				pSnake2.erase(pSnake2.end() - 1);

				pSnake2.insert(pSnake2.begin(), new POINTDATA(first_m_x, first_m_y + 1, first_m_color, first_m_data));
				break;
			}
			default:
				break;
			}

		}
	}
	bool IsTouchSnake(int x, int y)
	{
		int i = 0;
		for (auto &val : pSnake2)
		{
			if (i == 0) {
				i++; continue;
			}
			if (val->m_x == x && val->m_y == y)
			{
				return true;
			}
		}
		return false;
	}
	bool IsTouchPartnerSnake(int x, int y)
	{
		int i = 0;
		for (auto &val : pSnake)
		{
			if (i == 0) {
				i++; continue;
			}
			if (val->m_x == x && val->m_y == y)
			{
				return true;
			}
		}
		return false;
	}

	void PrintSnakePoint()
	{
		for (auto &val : pSnake2)
		{
			PrintPoint(val->m_x, val->m_y, val->m_data, val->m_color);
		}
	}

	bool WearWall()
	{
		if (pSnake2[0]->m_x == WALL_WIDE - 1) { pSnake2[0]->m_x = 1; return true; }
		else if (pSnake2[0]->m_x == 0) { pSnake2[0]->m_x = WALL_WIDE - 2; return true; }
		else if (pSnake2[0]->m_y == 0) { pSnake2[0]->m_y = WALL_HIGH - 2; return true; }
		else if (pSnake2[0]->m_y == WALL_HIGH - 1) { pSnake2[0]->m_y = 1; return true; }
		return false;
	}
};
class Wall
{
	int m_wide;
	int m_higt;
public:

	Wall(int wide = 20, int higt = 20, int GameMode = MODE_NORMAL, bool partner = false, bool custom = false) { CreatWall(wide, higt, GameMode, partner, custom); }
	int GetWallWide() { return m_wide; }
	int GetWallHigh() { return m_higt; }
	void CreatWall(int wide, int higt, int GameMode, bool partner, bool custom)
	{
		for (int w = 0; w<wide; w++)
		{
			for (int h = 0; h < higt; h++)
			{
				if (h == 0 || h == higt - 1)
				{
					pWall.push_back(new POINTDATA(w, h, 10, "■"));
				}
				else if (w == 0 || w == wide - 1)
				{
					pWall.push_back(new POINTDATA(w, h, 10, "■"));
				}

			}
		}

		if (custom == false)
		{
			if (GameMode == MODE_NORMAL)
			{
				CreatWallFor15PB(10, 5, 0x9);
				CreatWallForTen(13, 30, 0xd);
				CreatWallForTen(30, 30, 0xf);
			}
			else
			{
				if (partner)
				{
					CreatWallFor15PB(10, 20, 0x9);

					CreatWallForTen(10, 10, 0xc);
					CreatWallForTen(34, 10, 0xd);

					CreatWallForTen(10, 35, 0xe);
					CreatWallForTen(34, 35, 0xf);

				}
				else
				{
					CreatWallForTen(22, 20, 0xd);
				}
			}
		}

		PrintWallPoint();
	}
	void CreatWallFor15PB(int x, int y, int color)
	{
		pWall.push_back(new POINTDATA(x, y, color, "■", 1));
		pWall.push_back(new POINTDATA(x, y + 1, color, "■", 1));
		pWall.push_back(new POINTDATA(x, y + 2, color, "■", 1));
		pWall.push_back(new POINTDATA(x, y + 3, color, "■", 1));
		pWall.push_back(new POINTDATA(x, y + 4, color, "■", 1));
		pWall.push_back(new POINTDATA(x, y + 5, color, "■", 1));
		pWall.push_back(new POINTDATA(x, y + 6, color, "■", 1));


		pWall.push_back(new POINTDATA(x + 5, y, color, "■", 1));
		pWall.push_back(new POINTDATA(x + 6, y, color, "■", 1));
		pWall.push_back(new POINTDATA(x + 7, y, color, "■", 1));
		pWall.push_back(new POINTDATA(x + 4, y, color, "■", 1));
		pWall.push_back(new POINTDATA(x + 4, y + 1, color, "■", 1));
		pWall.push_back(new POINTDATA(x + 4, y + 2, color, "■", 1));
		pWall.push_back(new POINTDATA(x + 4, y + 3, color, "■", 1));
		pWall.push_back(new POINTDATA(x + 5, y + 3, color, "■", 1));
		pWall.push_back(new POINTDATA(x + 6, y + 3, color, "■", 1));
		pWall.push_back(new POINTDATA(x + 7, y + 3, color, "■", 1));
		pWall.push_back(new POINTDATA(x + 7, y + 4, color, "■", 1));
		pWall.push_back(new POINTDATA(x + 7, y + 5, color, "■", 1));
		pWall.push_back(new POINTDATA(x + 7, y + 6, color, "■", 1));
		pWall.push_back(new POINTDATA(x + 6, y + 6, color, "■", 1));
		pWall.push_back(new POINTDATA(x + 5, y + 6, color, "■", 1));
		pWall.push_back(new POINTDATA(x + 4, y + 6, color, "■", 1));

		//pWall.push_back(new POINTDATA(16, 5, color, "■",1));
		pWall.push_back(new POINTDATA(x + 13, y, color, "■", 1));
		pWall.push_back(new POINTDATA(x + 14, y, color, "■", 1));
		pWall.push_back(new POINTDATA(x + 15, y, color, "■", 1));
		pWall.push_back(new POINTDATA(x + 15, y + 1, color, "■", 1));
		pWall.push_back(new POINTDATA(x + 15, y + 2, color, "■", 1));
		pWall.push_back(new POINTDATA(x + 15, y + 3, color, "■", 1));
		pWall.push_back(new POINTDATA(x + 14, y + 3, color, "■", 1));
		pWall.push_back(new POINTDATA(x + 13, y + 3, color, "■", 1));
		//pWall.push_back(new POINTDATA(16, 8, color, "■"));
		//pWall.push_back(new POINTDATA(x+12, y+3, color, "■", 1));
		pWall.push_back(new POINTDATA(x + 11, y, color, "■", 1));
		pWall.push_back(new POINTDATA(x + 11, y + 1, color, "■", 1));
		pWall.push_back(new POINTDATA(x + 11, y + 2, color, "■", 1));
		pWall.push_back(new POINTDATA(x + 11, y + 3, color, "■", 1));
		pWall.push_back(new POINTDATA(x + 11, y + 4, color, "■", 1));
		pWall.push_back(new POINTDATA(x + 11, y + 5, color, "■", 1));
		pWall.push_back(new POINTDATA(x + 11, y + 6, color, "■", 1));

		pWall.push_back(new POINTDATA(x + 19, y, color, "■", 1));
		pWall.push_back(new POINTDATA(x + 19, y + 1, color, "■", 1));
		pWall.push_back(new POINTDATA(x + 19, y + 2, color, "■", 1));
		pWall.push_back(new POINTDATA(x + 19, y + 3, color, "■", 1));
		pWall.push_back(new POINTDATA(x + 19, y + 4, color, "■", 1));
		pWall.push_back(new POINTDATA(x + 19, y + 5, color, "■", 1));
		pWall.push_back(new POINTDATA(x + 19, y + 6, color, "■", 1));


		/*
		pWall.push_back(new POINTDATA(24, 5, 10, "■"));
		pWall.push_back(new POINTDATA(25, 5, 10, "■"));
		pWall.push_back(new POINTDATA(26, 5, 10, "■"));
		pWall.push_back(new POINTDATA(27, 5, 10, "■"));
		pWall.push_back(new POINTDATA(27, 6, 10, "■"));
		pWall.push_back(new POINTDATA(27, 7, 10, "■"));
		pWall.push_back(new POINTDATA(27, 8, 10, "■"));
		pWall.push_back(new POINTDATA(26, 8, 10, "■"));
		pWall.push_back(new POINTDATA(25, 8, 10, "■"));
		pWall.push_back(new POINTDATA(24, 8, 10, "■"));*/

		//pWall.push_back(new POINTDATA(24, 11, color, "■"));
		pWall.push_back(new POINTDATA(x + 21, y + 6, color, "■", 1));
		pWall.push_back(new POINTDATA(x + 22, y + 6, color, "■", 1));
		pWall.push_back(new POINTDATA(x + 23, y + 6, color, "■", 1));
		pWall.push_back(new POINTDATA(x + 23, y + 5, color, "■", 1));
		pWall.push_back(new POINTDATA(x + 23, y + 4, color, "■", 1));
		pWall.push_back(new POINTDATA(x + 23, y + 3, color, "■", 1));
		pWall.push_back(new POINTDATA(x + 22, y + 3, color, "■", 1));
		pWall.push_back(new POINTDATA(x + 21, y + 3, color, "■", 1));
		//pWall.push_back(new POINTDATA(24, 8, color, "■"));



	}
	void CreatWallForTen(int x, int y, int color)
	{
		pWall.push_back(new POINTDATA(x, y - 3, color, "■", 1));
		pWall.push_back(new POINTDATA(x, y - 2, color, "■", 1));
		pWall.push_back(new POINTDATA(x, y - 1, color, "■", 1));
		pWall.push_back(new POINTDATA(x, y, color, "■", 1));
		pWall.push_back(new POINTDATA(x, y + 1, color, "■", 1));
		pWall.push_back(new POINTDATA(x, y + 2, color, "■", 1));
		pWall.push_back(new POINTDATA(x, y + 3, color, "■", 1));


		pWall.push_back(new POINTDATA(x - 3, y, color, "■", 1));
		pWall.push_back(new POINTDATA(x - 2, y, color, "■", 1));
		pWall.push_back(new POINTDATA(x - 1, y, color, "■", 1));
		pWall.push_back(new POINTDATA(x + 1, y, color, "■", 1));
		pWall.push_back(new POINTDATA(x + 2, y, color, "■", 1));
		pWall.push_back(new POINTDATA(x + 3, y, color, "■", 1));


	}
	bool IsTouchWall(int x, int y)
	{
		for (auto &val : pWall) { if (val->m_x == x && val->m_y == y) return true; }
		return false;
	}
	void PrintWallPoint()
	{
		for (auto &val : pWall) {
			PrintPoint(val->m_x, val->m_y, val->m_data, val->m_color);
			Sleep(5);
		}
	}
};
class Food
{
	int m_FoodWide;
	int m_FoodHiht;
public:
	Food(int wide = 20, int high = 20) :m_FoodWide(wide), m_FoodHiht(high) { AddFood(1, false); }
	int IsHaveFood(int x, int y)
	{
		//if (x == pFood[0]->m_x && y == pFood[0]->m_y) { return true; }
		for (auto &val : pFood)
		{
			if (x == val->m_x && y == val->m_y) { return val->m_type; }
		}
		return -1;
	}
	void AddFood(int foodnum = 1, bool partner = false)
	{
		for (int i = 0; i<foodnum; i++)
		{
			int nFoodX, nFoodY;

			while (1)
			{
			here2:
				nFoodX = GetRandNum(1, m_FoodWide);
				nFoodY = GetRandNum(2, m_FoodHiht);
				if (nFoodX > 0 && nFoodX < m_FoodWide  && nFoodY > 1 && nFoodY < m_FoodHiht)
				{
					for (auto &val : pWall) { if (val->m_x == nFoodX && val->m_y == nFoodY) goto here2; }
					for (auto &val : pSnake) { if (val->m_x == nFoodX && val->m_y == nFoodY) goto here2; }
					goto here;
				}
			}

		here:
			int nRandNum = GetRandNum(1, partner ? 30 : 10);
			int nType;
			int nColor;
			char *nData;

			/*
			第一个为背景，第二个则为前景
			0 = 黑色       8 = 灰色
			1 = 蓝色       9 = 淡蓝色
			2 = 绿色       A = 淡绿色
			3 = 浅绿色     B = 淡浅绿色
			4 = 红色       C = 淡红色
			5 = 紫色       D = 淡紫色
			6 = 黄色       E = 淡黄色
			7 = 白色       F = 亮白色*/

			switch (nRandNum)
			{
			case 1:
				nType = FOOD_SUBME;
				nColor = 0xd;
				nData = "★";
				break;

			case 11:
				nType = FOOD_ADDPT;
				nColor = 0xe;
				nData = "☆";
				break;
			case 12:
				nType = FOOD_SUBPT;
				nColor = 0xd;
				nData = "☆";
				break;

			case 13:
				nType = FOOD_BACKPT;
				nColor = 0xe;
				nData = "卍";
				break;
			case 14:
				nType = FOOD_STOPPT;
				nColor = 0xd;
				nData = "卐";
				break;
			case 15:case 16:case 17:case 18:case 19:case 20:
				nType = FOOD_ADDBULLET;
				nColor = 0xc;
				nData = "○";
				break;
			default:
				nType = FOOD_ADDME;
				nColor = 0xe;
				nData = "★";
				break;
			}

			pFood.push_back(new POINTDATA(nFoodX, nFoodY, nColor, nData, nType));
		}
	}
	void HaveFood(int x, int y, bool partner)
	{
		PrintPoint(x, y, "  ", 10);

		int i = 0;
		for (auto &val : pFood)
		{
			if (x == val->m_x && y == val->m_y) { break; }
			i++;
		}
		auto itr = pFood.begin() + i;
		//PrintPoint(pFood[0]->m_x, pFood[0]->m_y, "  ", pFood[0]->m_color);
		itr = pFood.erase(itr);
		Food::AddFood(1, partner);

	}
	void PrintFoodPoint()
	{
		for (auto &val : pFood) { PrintPoint(val->m_x, val->m_y, val->m_data, val->m_color); }
	}
};

//鼠标函数，自定义地图使用
bool MouseEvenProc(MOUSE_EVENT_RECORD mer, Map &MyMap)
{
	static int nMouseFlags = 0;//使用静态局部变量，储存鼠标按下状态
	nMouseFlags = mer.dwButtonState;//将鼠标按下状态赋值nMouseFlags

	switch (mer.dwEventFlags)
	{
	case 0://鼠标被单击
		if (nMouseFlags == FROM_LEFT_1ST_BUTTON_PRESSED)//左键按下
		{
			int nMouseX = mer.dwMousePosition.X / 2;
			int nMouseY = mer.dwMousePosition.Y;
			int nFrame =MyMap.GetClickFrame(nMouseX, nMouseY);

			if (nFrame ==0)
			{
				MyMap.PaintMap(nMouseX, nMouseY);
			}
			else if(nFrame ==1)
			{
				MyMap.PrintTools(nMouseX, nMouseY);
			}
			else if (nFrame == 2)
			{
				int nButtom=MyMap.PrintButton(nMouseX, nMouseY,true);
				if (nButtom == 0 || nButtom == 1)
				{
					bool bRet=MyMap.SaveMap(nButtom + 1);
					if (bRet==false)
					{
						MyMap.PrintButton(nMouseX, nMouseY, bRet);
						MyMap.PrintErrorInfo(true);
					}
					else MyMap.PrintErrorInfo(false);

				}
				else return nButtom == 2 ? true : false;
				
			}
			
		}
		else if (nMouseFlags == RIGHTMOST_BUTTON_PRESSED)//右键按下
		{
			int nMouseX = mer.dwMousePosition.X / 2;
			int nMouseY = mer.dwMousePosition.Y;
			int nFrame = MyMap.GetClickFrame(nMouseX, nMouseY);

			if (nFrame == 0)
			{
				MyMap.EraseWall(nMouseX, nMouseY);
			}

		}
		break;
	case DOUBLE_CLICK://鼠标被双击
		break;
	case MOUSE_MOVED://鼠标被移动
		int nMouseX = mer.dwMousePosition.X / 2;
		int nMouseY = mer.dwMousePosition.Y;
		int nFrame = MyMap.GetClickFrame(nMouseX, nMouseY);
		//取当前点击区域0：绘画界面 1：工具界面 2：保存界面
		if (nFrame == 0 && nMouseFlags== FROM_LEFT_1ST_BUTTON_PRESSED)//按下左键且当前坐标在绘图区
		{
			MyMap.PaintMap(nMouseX, nMouseY);//调用绘图的方法
		}
		else if (nFrame == 0 && nMouseFlags == RIGHTMOST_BUTTON_PRESSED)//按下右键
		{
			MyMap.EraseWall(nMouseX, nMouseY);
		}
		break;
	}
	return false;
}
void MessageLoop(Map &MyMap)//鼠标消息循环
{
	HANDLE HND = GetStdHandle(STD_INPUT_HANDLE);
	INPUT_RECORD stcRcord = { 0 };
	DWORD dwRead;
	SetConsoleMode(HND, ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT);
	while (1)
	{
		ReadConsoleInput(HND, &stcRcord, 1, &dwRead);
		if (stcRcord.EventType == KEY_EVENT)
		{
			if (stcRcord.Event.KeyEvent.bKeyDown)
			{
				switch (stcRcord.Event.KeyEvent.wVirtualKeyCode)
				{
				case 87:case 38:
					//cout << "上" << endl;

					break;
				case 83:case 40:
					//cout << "下" << endl;

					break;
				case 65:case 37:
					//cout << "左" << endl;

					break;
				case 68:case 39:
					//cout << "右" << endl;

					break;
				case 32:
					break;
				default:
					//cout << "键盘按下" << stcRcord.Event.KeyEvent.wVirtualKeyCode << endl;
					break;
				}
			}
		}
		else if (stcRcord.EventType == MOUSE_EVENT)
		{
			bool nRet=MouseEvenProc(stcRcord.Event.MouseEvent, MyMap);
			if (nRet == true)break;
		}
	}
};

//关于保存与读取存档，暂停游戏，重新开始，退出函数
bool LoadGame(int GameMode, int &nDir,int &nRank)
{
	FILE *pfile;
	char *szFileName = GameMode == MODE_NORMAL ? "Normal.sav":"Wear.sav";

	fopen_s(&pfile, szFileName, "rb");
	if (pfile == NULL)return false;
	
	int nsize = 0;


	//读取pSnake数据
	//先读取前4字节,数组的数量，然后for循环依次读入指定数量的数据，放入临时内存里
	//再将temp->m_data重新赋值为图案，因为读出来的是上一次字符串地址，并不是图案

	fread(&nsize, 1, sizeof(int), pfile);//数组数量
	for (int i=0;i<nsize;i++)
	{
		POINTDATA *temp = new POINTDATA;//申请一个临时内存空间
		fread(temp, 1, sizeof(POINTDATA), pfile);
		temp->m_data = "●";//重新赋值
		pSnake.push_back(new POINTDATA(temp->m_x, temp->m_y, temp->m_color, temp->m_data, temp->m_type));
		delete temp;//用完之后要释放
	}

	fread(&nDir, 1, sizeof(int), pfile);//这是蛇的方向
	fread(&nRank, 1, sizeof(int), pfile);//最后是分数

	fclose(pfile);
	//remove(szFileName);
	return true;

}
void SaveGame(int GameMode, Ice &MyIce, Wall &MyWall, Food &MyFood, Snake &MySnake)
{
	FILE *pfile;
	char *szFileName = GameMode == MODE_NORMAL ? "Normal.sav" : "Wear.sav";

	int nsize = 0;

	//先储存数组元素的数量，然后再接着储存数组内的数据，移动方向，分数
	//储存pSnake数据
	nsize=pSnake.size();//取出数组成员数量
	fopen_s(&pfile, szFileName, "wb");
	fwrite(&nsize, 1, sizeof(int), pfile);
	fclose(pfile);

	for (auto &val : pSnake) 
	{ 
		fopen_s(&pfile, szFileName, "ab");//循环写出数组内的数据
		fwrite(val, 1, sizeof(POINTDATA), pfile);
		fclose(pfile);
	}

	fopen_s(&pfile, szFileName, "ab");
	int nDir = MySnake.GetDir();
	fwrite(&nDir, 1, sizeof(int), pfile);//写出蛇的方向
	fclose(pfile);

	fopen_s(&pfile, szFileName, "ab");
	int nRank = MySnake.GetRank();
	fwrite(&nRank, 1, sizeof(int), pfile);//最后写出分数记录
	fclose(pfile);

}
void CleanGame()
{
	vector <POINTDATA*>().swap(pSnake);
	vector <POINTDATA*>().swap(pSnake2);
	vector <POINTDATA*>().swap(pFood);
	vector <POINTDATA*>().swap(pWall);
	vector <POINTDATA*>().swap(pIce);
	vector <POINTDATA*>().swap(pBullet);
}
void StartGame(int GameMode = MODE_NORMAL,bool Load=false,bool Partner=false,bool CustomGame=false,int MapNum=1)
{
	int TempSnakeLevel = 1;
	int TempSnake2Level = 1;
	int SnakeX ;
	int SnakeY ;
	int SnakeX2;
	int SnakeY2;
	bool Quick=false;

	int nDir = DIR_DOWN;
	int nDir2 = DIR_DOWN;
	int nRank = 0;
	bool bLoad = false;
	bool bCrearSnake = false;


	Ice MyIce;
	Bullet MyBullet;
	Wall MyWall(WALL_WIDE, WALL_HIGH, GameMode, Partner, CustomGame);
	Food MyFood(WALL_WIDE, WALL_HIGH);

	if (CustomGame)
	{
		bCrearSnake = false;
		nDir = DIR_DOWN;
		nDir2 = DIR_DOWN;
		if (pSnake2.size() > 0)Partner = true;
		else Partner = false;
	}
	else if(Load)//是否选择载入存档
	{
		bCrearSnake = false;
		bLoad = LoadGame(GameMode, nDir, nRank);
	}
	else
	{
		bCrearSnake = true;
		nDir = DIR_DOWN;
	}
	Snake MySnake(2, 6, 11, "●", nDir, bCrearSnake);
	Snake2 MySnake2(42, 6, 0xe, "●", nDir2, bCrearSnake);

	if (CustomGame)MySnake.AddSnakeLen();
	if (CustomGame && Partner)MySnake2.AddSnakeLen();

	if (Load == true && bLoad==true)//载入存档成功需要设置上一次方向与分数
	{
		MySnake.SetRank(nRank);
		MySnake.SetDir(nDir);
	}

	if (Load == false || bLoad == false )//载入存档失败或者选择新游戏时需要增加长度
	{
		if (CustomGame == false)
		{
			MySnake.AddPoint(2, 5, 11);
			MySnake.AddPoint(2, 4, 11);
			MySnake.AddPoint(2, 3, 11);
			MySnake.AddPoint(2, 2, 11);

			if (Partner) {
				MySnake2.AddPoint(42, 5, 0xe);
				MySnake2.AddPoint(42, 4, 0xe);
				MySnake2.AddPoint(42, 3, 0xe);
				MySnake2.AddPoint(42, 2, 0xe);
			}
		}


	}

	if (GameMode == MODE_WEAR && Partner==false && CustomGame==false)//如果为穿墙模式创造冰面
	{
		MyIce.CreatIce(10, 12, 5, 20);
		MyIce.CreatIce(30, 12, 5, 20);
		MyIce.CreatIce(12, 5, 21, 4);
		MyIce.CreatIce(12, 35, 21, 4);
	}

	PrintEmptyFrame(45, 0, 14,14, 0xa, "□");//打印游戏右上框
	PrintEmptyFrame(45, 16, 14, 17, 0xa, "□");//打印游戏右中框


	while (1)
	{
		int nkey;

		if (_kbhit())
		{
			nkey = _gettch();
			switch (nkey)
			{
			case 'w':case 'W'://1p的方向按键
				if(MySnake.GetDir()== DIR_UP)Quick=true;//如果当前方向与按键方向一致，则开启加速状态Quick
				MySnake.SetDir(DIR_UP);//如果不一致，则调用Snake类方法进行方向的改变
				break;
			case 's':case 'S':
				if (MySnake.GetDir() == DIR_DOWN)Quick = true;
				MySnake.SetDir(DIR_DOWN);
				break;
			case 'a':case 'A':
				if (MySnake.GetDir() == DIR_LEFT)Quick = true;
				MySnake.SetDir(DIR_LEFT);
				break;
			case 'd':case 'D':
				if (MySnake.GetDir() == DIR_RIGHT)Quick = true;
				MySnake.SetDir(DIR_RIGHT);
				break;
			case 'f':case 'F'://1p的发炮弹按键
				//MyBullet.LaunchBullet(MySnake.GetSnakeX(), MySnake.GetSnakeY(), MySnake.GetDir());
				if (MySnake.GetBulletNum() > 0)//是否吃到炮弹食物，可叠加
				{
					bool nLaunch =MyBullet.LaunchBullet(MySnake.GetSnakeX(), MySnake.GetSnakeY(), MySnake.GetDir());
					if(nLaunch)MySnake.SubBulletNum();//发炮后减去炮弹食物数量
				}
				else
				{
					if (MySnake.GetSnakeLen()>5)//蛇身长度大于5可以发炮
					{
						bool nLaunch = MyBullet.LaunchBullet(MySnake.GetSnakeX(), MySnake.GetSnakeY(), MySnake.GetDir());
						if (nLaunch)MySnake.SubPoint(11);//发炮后自身长度减1
					}
				}
				break;

				//双人模式右边操作
				if (Partner) {
			case 'i':case 'I':
				if (MySnake2.GetDir() == DIR_UP)Quick = true;
				MySnake2.SetDir(DIR_UP);
				break;
			case 'k':case 'K':
				if (MySnake2.GetDir() == DIR_DOWN)Quick = true;
				MySnake2.SetDir(DIR_DOWN);
				break;
			case 'j':case 'J':
				if (MySnake2.GetDir() == DIR_LEFT)Quick = true;
				MySnake2.SetDir(DIR_LEFT);
				break;
			case 'l':case 'L':
				if (MySnake2.GetDir() == DIR_RIGHT)Quick = true;
				MySnake2.SetDir(DIR_RIGHT);
				break;
			case 'p':case 'P':
				//MyBullet.LaunchBullet(MySnake2.GetSnakeX(), MySnake2.GetSnakeY(), MySnake2.GetDir());
				if (MySnake2.GetBulletNum() > 0)
				{
					bool nLaunch = MyBullet.LaunchBullet(MySnake2.GetSnakeX(), MySnake2.GetSnakeY(), MySnake2.GetDir());
					if (nLaunch)MySnake2.SubBulletNum();
				}
				else
				{
					if (MySnake2.GetSnakeLen() > 5)
					{
						bool nLaunch = MyBullet.LaunchBullet(MySnake2.GetSnakeX(), MySnake2.GetSnakeY(), MySnake2.GetDir());
						if (nLaunch)MySnake2.SubPoint(11);
					}
				}
				break;
				}


			case 32://空格键暂停游戏
				MySnake.SetPause(true);
				break;
			default:
				//PrintPoint(2, 2, nkey, 0xe);
				break;
			}
		}

		MySnake.MovePoint();//移动蛇身体
		MyBullet.MoveBulletPoint();//移动炮弹

		if (Partner) { MySnake2.MovePoint(); }//移动蛇身体
		

		SnakeX = MySnake.GetSnakeX();
		SnakeY = MySnake.GetSnakeY();

		if (Partner) {
			SnakeX2 = MySnake2.GetSnakeX();
			SnakeY2 = MySnake2.GetSnakeY();
		}


		if (MyWall.IsTouchWall(SnakeX, SnakeY))//是否碰到墙壁
		{
			if (GameMode== MODE_NORMAL)//判断是否为普通模式
			{
				if (Partner == false)//判断是否为双人模式
				{
					MyRank.SetNormalRank(MySnake.GetRank());//保存当前分数记录
					MyRank.SaveRank();
				}

				GameOverData://定位被goto到的位置
				int nRet = GameOver(MySnake.GetRank(),Partner,0);//显示GameOver图案与菜单
				if (nRet == 0)//返回0=选择第一个重新开始
				{
					system("cls");
					CleanGame();//清除所有动态数组数据
					if (CustomGame)//如果为自定义地图，则载入自定义地图
					{
						Map MyMap(false);//Map类构造函数传入false则不进入地图编辑模式，只进行类对象声明
						MyMap.OpenMap(MapNum);//调用Map类内的读取自定义地图方法
					}
					StartGame(GameMode, false, Partner, CustomGame,MapNum);//最后重新开始游戏
					break;//跳出本次流程
				};
				break;
			}
			else//判断是否为穿墙模式
			{ 
				bool nRet=MySnake.WearWall(); //判断墙是否可穿，墙有外墙和内墙分类，外墙可穿内墙不可穿
				if(nRet==false) goto GameOverData;//不可穿goto到GameOverData数据段
			}
		}//是否碰到墙壁//是否碰到墙壁
		if (Partner && MyWall.IsTouchWall(SnakeX2, SnakeY2))//是否碰到墙壁
		{
			if (GameMode == MODE_NORMAL)//判断是否为普通模式
			{
				if (Partner == false)
				{
					MyRank.SetNormalRank(MySnake.GetRank());//保存当前分数记录
					MyRank.SaveRank();
				}


			GameOverData2:
				int nRet = GameOver(MySnake2.GetRank(), Partner, 1);//显示GameOver图案与菜单
				if (nRet == 0)//选择第一个重新开始
				{
					system("cls");
					CleanGame();
					if (CustomGame)
					{
						Map MyMap(false);
						MyMap.OpenMap(MapNum);
					}
					StartGame(GameMode, false, Partner, CustomGame,MapNum);
					break;
				};
				break;
			}
			else//为穿墙模式
			{
				bool nRet = MySnake2.WearWall(); //判断墙是否可穿
				if (nRet == false) goto GameOverData2;//不可穿goto到GameOverData
			}
		}

		if (MySnake.IsTouchSnake(SnakeX, SnakeY))//是否碰到自己身体
		{
			if(Partner==false)
			{
				if (GameMode == MODE_NORMAL) { MyRank.SetNormalRank(MySnake.GetRank()); }//普通模式就选择保存普通模式记录
				else { MyRank.SetWearRank(MySnake.GetRank()); }//穿墙模式就选择保存穿墙模式记录
				MyRank.SaveRank();
			}


			int nRet = GameOver(MySnake.GetRank(), Partner, 0);//显示GameOver图案与菜单
			if (nRet == 0)//选择第一个重新开始
			{
				system("cls");
				CleanGame();
				if (CustomGame)
				{
					Map MyMap(false);
					MyMap.OpenMap(MapNum);
				}
				StartGame(GameMode, false, Partner, CustomGame,MapNum);
				break;
			};
			break;
		}
		if (Partner && MySnake2.IsTouchSnake(SnakeX2, SnakeY2))//是否碰到自己身体
		{
			if (Partner == false)
			{
				if (GameMode == MODE_NORMAL) { MyRank.SetNormalRank(MySnake.GetRank()); }//普通模式就选择保存普通模式记录
				else { MyRank.SetWearRank(MySnake.GetRank()); }//穿墙模式就选择保存穿墙模式记录
				MyRank.SaveRank();
			}



			int nRet = GameOver(MySnake2.GetRank(), Partner, 1);//显示GameOver图案与菜单
			if (nRet == 0)//选择第一个重新开始
			{
				system("cls");
				CleanGame();
				if (CustomGame)
				{
					Map MyMap(false);
					MyMap.OpenMap(MapNum);
				}
				StartGame(GameMode, false, Partner, CustomGame,MapNum);
				break;
			};
			break;
		}

		SnakeX = MySnake.GetSnakeX();//如果穿墙后需要再取一次坐标
		SnakeY = MySnake.GetSnakeY();
		if (Partner) {
			SnakeX2 = MySnake2.GetSnakeX();
			SnakeY2 = MySnake2.GetSnakeY();
		}

		if (Partner && MySnake.IsTouchPartnerSnake(SnakeX, SnakeY))//判断是否碰到对方身体
			{
				int nRet = GameOver(MySnake.GetRank(), Partner, 0);//显示GameOver图案与菜单
				if (nRet == 0)//选择第一个重新开始
				{
					system("cls");
					CleanGame();
					if (CustomGame)
					{
						Map MyMap(false);
						MyMap.OpenMap(MapNum);
					}
					StartGame(GameMode, false, Partner, CustomGame,MapNum);
					break;
				};
				break;
			}
		if (Partner && MySnake2.IsTouchPartnerSnake(SnakeX2, SnakeY2))//判断是否碰到对方身体
			{
				int nRet = GameOver(MySnake2.GetRank(), Partner, 1);//显示GameOver图案与菜单
				if (nRet == 0)//选择第一个重新开始
				{
					system("cls");
					CleanGame();
					if (CustomGame)
					{
						Map MyMap(false);
						MyMap.OpenMap(MapNum);
					}
					StartGame(GameMode, false, Partner, CustomGame,MapNum);
					break;
				};
				break;
			}

		if (MyFood.IsHaveFood(SnakeX, SnakeY)== FOOD_ADDME)//是否吃到加长食物
		{
			MyFood.HaveFood(SnakeX, SnakeY,Partner);
			MySnake.AddPoint(SnakeX, SnakeY, 11);
			MySnake.AddRank();
		}
		if (Partner && MyFood.IsHaveFood(SnakeX2, SnakeY2) == FOOD_ADDME)//是否吃到加长食物
		{
			MyFood.HaveFood(SnakeX2, SnakeY2, Partner);
			MySnake2.AddPoint(SnakeX2, SnakeY2, 0xe);
			MySnake2.AddRank();
		}

		if (MyFood.IsHaveFood(SnakeX, SnakeY) == FOOD_SUBME)//是否吃到变短食物
		{
			MyFood.HaveFood(SnakeX, SnakeY, Partner);
			MySnake.SubPoint(11);
			MySnake.AddRank();
		}
		if (Partner && MyFood.IsHaveFood(SnakeX2, SnakeY2) == FOOD_SUBME)//是否吃到变短食物
		{
			MyFood.HaveFood(SnakeX2, SnakeY2, Partner);
			MySnake2.SubPoint(0xe);
			MySnake2.AddRank();
		}

		if (Partner && MyFood.IsHaveFood(SnakeX, SnakeY) == FOOD_ADDPT)
		{
			MyFood.HaveFood(SnakeX, SnakeY, Partner);
			MySnake2.AddPoint(SnakeX2, SnakeY2,0xe);
			MySnake.AddRank();
		}
		if (Partner && MyFood.IsHaveFood(SnakeX2, SnakeY2) == FOOD_ADDPT)
		{
			MyFood.HaveFood(SnakeX2, SnakeY2, Partner);
			MySnake.AddPoint(SnakeX, SnakeY,0xb);
			MySnake2.AddRank();
		}

		if (Partner && MyFood.IsHaveFood(SnakeX, SnakeY) == FOOD_SUBPT)
		{
			MyFood.HaveFood(SnakeX, SnakeY, Partner);
			MySnake2.SubPoint(0xe);
			MySnake.AddRank();
		}
		if (Partner && MyFood.IsHaveFood(SnakeX2, SnakeY2) == FOOD_SUBPT)
		{
			MyFood.HaveFood(SnakeX2, SnakeY2, Partner);
			MySnake.SubPoint(0xb);
			MySnake2.AddRank();
		}

		if (Partner && MyFood.IsHaveFood(SnakeX, SnakeY) == FOOD_BACKPT)//吃到了让对方的方向反转食物
		{
			MyFood.HaveFood(SnakeX, SnakeY, Partner);//调用Food类方法，重新生成食物
			MySnake2.SetState(1);//设置对方状态1:反方向 2：禁止转向
			MySnake.AddRank();//自己加10分
		}
		if (Partner && MyFood.IsHaveFood(SnakeX2, SnakeY2) == FOOD_BACKPT)
		{
			MyFood.HaveFood(SnakeX2, SnakeY2, Partner);
			MySnake.SetState(1);
			MySnake2.AddRank();
		}

		if (Partner && MyFood.IsHaveFood(SnakeX, SnakeY) == FOOD_STOPPT)
		{
			MyFood.HaveFood(SnakeX, SnakeY, Partner);
			MySnake2.SetState(2);
			MySnake.AddRank();
		}
		if (Partner && MyFood.IsHaveFood(SnakeX2, SnakeY2) == FOOD_STOPPT)
		{
			MyFood.HaveFood(SnakeX2, SnakeY2, Partner);
			MySnake.SetState(2);
			MySnake2.AddRank();
		}

		if (Partner && MyFood.IsHaveFood(SnakeX, SnakeY) == FOOD_ADDBULLET)
		{
			MyFood.HaveFood(SnakeX, SnakeY, Partner);
			MySnake.AddBulletNum();
			MySnake.AddRank();
		}
		if (Partner && MyFood.IsHaveFood(SnakeX2, SnakeY2) == FOOD_ADDBULLET)
		{
			MyFood.HaveFood(SnakeX2, SnakeY2, Partner);
			MySnake2.AddBulletNum();
			MySnake2.AddRank();
		}
		if (Partner)
		{
			int nBullet = MyBullet.IsSnake();
			if (nBullet == 1)
			{
				int nRet = GameOver(MySnake.GetRank(), Partner, 0);//显示GameOver图案与菜单
				if (nRet == 0)//选择第一个重新开始
				{
					system("cls");
					CleanGame();
					if (CustomGame)
					{
						Map MyMap(false);
						MyMap.OpenMap(MapNum);
					}
					StartGame(GameMode, false, Partner, CustomGame,MapNum);
					break;
				};
				break;
			}
			if (nBullet == 2)
			{
				int nRet = GameOver(MySnake.GetRank(), Partner, 1);//显示GameOver图案与菜单
				if (nRet == 0)//选择第一个重新开始
				{
					system("cls");
					CleanGame();
					if (CustomGame)
					{
						Map MyMap(false);
						MyMap.OpenMap(MapNum);
					}
					StartGame(GameMode, false, Partner, CustomGame,MapNum);
					break;
				};
				break;
			}
		}


		if (TempSnakeLevel!=MySnake.GetSnakeLevel() && TempSnakeLevel<=3)//是否升级，升一级多添加一个食物
		{
			TempSnakeLevel = MySnake.GetSnakeLevel();
			MyFood.AddFood();
		}
		if (TempSnake2Level != MySnake2.GetSnakeLevel() && TempSnake2Level <= 3 && Partner)//是否升级，升一级多添加一个食物
		{
			TempSnake2Level = MySnake2.GetSnakeLevel();
			MyFood.AddFood();
		}


		MyIce.PrintIcePoint();//打印冰面
		MySnake.PrintSnakePoint();//打印蛇
		if (Partner) { MySnake2.PrintSnakePoint(); }//打印蛇
		MyFood.PrintFoodPoint();//打印食物
		MyBullet.PrintBulletPoint();//打印炮弹

		//游戏右侧显示信息与提示
		if (Partner)
		{
			PrintPoint(48, 2, GameMode == MODE_NORMAL ? "普通模式(双人对战)" : "穿墙模式(双人对战)", 0xc);
			PrintInfo(49, 5, " 1P 得分：", MySnake.GetRank(), 0xb);
			PrintInfo(49, 6, " 1P 等级：", MySnake.GetSnakeLevel(), 0xb);
			PrintInfo(49, 7, " 1P 炮弹：", MySnake.GetBulletNum(), 0xb);

			PrintInfo(49, 9, " 2P 得分：", MySnake2.GetRank(), 0xe);
			PrintInfo(49, 10, " 2P 等级：", MySnake2.GetSnakeLevel(), 0xe);
			PrintInfo(49, 11, " 2P 炮弹：", MySnake2.GetBulletNum(), 0xe);

			PrintPoint(46, 13, " 炮弹不足可牺牲>5长度放炮", 0xc);

		}
		else
		{
			PrintPoint(48, 2, GameMode == MODE_NORMAL ? "普通模式(单人自嗨)" : "穿墙模式(单人自嗨)", 0xc);
			PrintInfo(49, 5, "最高记录：", GameMode == MODE_NORMAL ? MyRank.GetNormalRank() : MyRank.GetWearRank(), 10);
			PrintInfo(49, 8, "当前得分：", MySnake.GetRank(), 10);
			PrintInfo(49, 11, "当前等级：", MySnake.GetSnakeLevel(), 10);
		}


		PrintGameTips(49,18,Partner);

		int nRet=PauseGame(MySnake.GetPause(),Partner,CustomGame);//判断游戏是否被暂停
		MySnake.SetPause(false);
		if (nRet == 0){}//继续游戏
		if (nRet == 1) //重新开始
		{ 
			system("cls");
			CleanGame();

			CleanGame();
			if(CustomGame)
			{
				Map MyMap(false);
				MyMap.OpenMap(MapNum);
			}

			StartGame(GameMode,false,Partner, CustomGame,MapNum);
			break; 
		};
		if (nRet == 2) { SaveGame(GameMode,MyIce,MyWall,MyFood,MySnake); break;}//回到主页与存档设置
		if (nRet == 3) {  break; }


		if (MyIce.IsOnIce(SnakeX, SnakeY))//是否在冰面上
		{
			Sleep(MySnake.GetSnakeSpeed()/2);//加速一倍
		}
		else
		{
			if (Partner) {
				//双人模式，取速度快的一方改变速度
				int Snake1Speed = MySnake.GetSnakeSpeed();
				int Snake2Speed = MySnake2.GetSnakeSpeed();
				Sleep(Snake1Speed>Snake2Speed? Snake2Speed: Snake1Speed);
			}
			else
			{
				Sleep(Quick == true ? 0 : MySnake.GetSnakeSpeed());//如果连续按键则触发Quick，延时设为0
			}
		}
		
		Quick = false;
	}
}
void CustomGame()
{
	Map MyMap;
	MyMap.PrintBasicWallPoint();

	MessageLoop(MyMap);

}

int main()
{
	PlayMusic(0);
	system("title My Snake");
	system("mode con cols=120 lines=45");
	keybd_event(VK_SHIFT, 0, 0, 0);
	Sleep(100);
	keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);
	srand(( unsigned int) time(NULL));//置随机种子

	int nCode;
	while (1)
	{
		system("cls");
		nCode = HomePage();

		if (nCode==10 || nCode==11 || nCode == 12 || nCode == 13 || nCode == 14)
		{
			if (nCode == 13|| nCode == 14)
			{
				CleanGame();
				Map MyMap(false);
				MyMap.OpenMap(nCode==13?1:2);

			}

				system("cls");
				StartGame(MODE_NORMAL, nCode == 10 ? true : false, nCode == 12 ? true : false,nCode==13 || nCode == 14 ?true:false, nCode == 13 ? 1 : 2);
				CleanGame();
		}
		else if (nCode == 20 || nCode == 21 || nCode == 22 || nCode == 23 || nCode == 24)
		{
			if (nCode == 23 || nCode == 24)
			{
				CleanGame();
				Map MyMap(false);
				MyMap.OpenMap(nCode == 23 ? 1 : 2);
			}

				system("cls");
				StartGame(MODE_WEAR, nCode == 20 ? true : false, nCode == 22 ? true : false, nCode == 23 || nCode == 24 ? true : false, nCode == 23 ? 1 : 2);
				CleanGame();


		}
		else if (nCode == 30 || nCode == 31)
		{
			system("cls");
			CustomGame();
		}

		else { break; }
	}
    return 0;
}

