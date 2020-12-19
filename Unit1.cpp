//---------------------------------------------------------------------------
#define NO_WIN32_LEAN_AND_MEAN
#include <vcl.h>
#include "jpeg.hpp"
#include <dos.h>
#include <stdlib.h>
#include <fstream>
#include <time.h>
#include <stdio.h>
#include <registry.hpp>
#include <memory>
#include <windows.h>
#include <direct.h>
#pragma hdrstop
#include "Unit1.h"
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;

char set_p[1500];// = "E:\\NVIDIA";        //���� ��� ����� ��������� ���������
int compress = 60;     //������� ������ � ��������� ��� 100% ��������� ��������.
float interval = 50; //*1000 .�������� � �������� � ����� �������� ��������� ��������
std::auto_ptr<Graphics::TBitmap> bmp1(new Graphics::TBitmap);   //�����-�� auto_ptr
std::auto_ptr<Graphics::TBitmap> bmp2(new Graphics::TBitmap);    //� �������� �� ��� ������ � ���������� ����!?
TRGBTriple *z1[1080];    //����� ������� ������������� ������, ��� ���������� ������ ������, � ����� ��� ��������� ��� ������, ��� ����� ������ ����� malloc
TRGBTriple *z[1080];    	//void** array = (void**)malloc(70 * sizeof(TRGBTriple*));  // �� �������� ���� ��� ���

__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{
	_getcwd(set_p, sizeof(set_p));
	Application->ShowMainForm = false;     // ������-�� �������� � ���� ������ �������������
	Sleep(1000);
	randomize();
	HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS, 0, "my_app");
	if(!hMutex)  // ���� hMutex = 0, �� ������� �� ����������.
		hMutex = CreateMutex(0, 0, "my_app");  // ��� ����� ��� ���� ����� �� ��������� ��������� �������� 2 ����
	else
	{
		ShowMessage("��������� ������!");
		exit(1);
	}
	MkDir(set_p);   //������� ���� ����
	TRegistry *reg = new TRegistry();     //�������� � ������� ������ ��� ������������
	reg->RootKey = HKEY_CURRENT_USER;
	reg->OpenKey("Software\\Microsoft\\Windows\\CurrentVersion\\Run",true);
	reg->WriteString("Proj",Application->ExeName);
	reg->CloseKey();           //
	Timer1->Interval = 1000*interval;
	Timer1->Enabled = 1;

	char q123[1500];
	_getcwd(q123, sizeof(q123));
	strcat(q123,"//123.bmp");

	bmp1->LoadFromFile(q123);      //������ ������, �����-�� �������� ���� ���-������, ����� ������� �� ���������.
	bmp2->LoadFromFile(q123);		  //������ ���, �� �������.
}                                          // ��� ����� ���� ��������� �����, ���������� ������� ��������
void __fastcall TForm1::Timer1Timer(TObject *Sender)
{       ///*
	static struct  time t;    //��� ���� strcat'�� ���� ������ ��� ���� ����� ��������� ������� ����� � ��� ����� � ����� � �����
	static char name[255];
	memset(name,0,255);
	static char buffer[80];
	static time_t seconds;
	seconds = time(NULL);
	strftime(buffer, 80, "%B_%d_%Y_%A", localtime(&seconds));
	static char path[MAX_PATH];
	memset(path,0,MAX_PATH);
	strcat(path,set_p);
	strcat(path,"\\");
	strcat(path,buffer);
	MkDir(path);
	gettime(&t);
	static char temp[255];
	memset(temp,0,255);
	sprintf(temp,"\\%s\\%2d-%02d-%02d__%i.jpg",
	buffer,t.ti_hour, t.ti_min, t.ti_sec,random(99999));
	strcat(name,set_p);
	strcat(name,temp);
	//l1=0 - ������ �����
	static bool l=0;    //�������� �-��� ����� �� ���������� ������� ������.
	static bool l1=0;


	static bool same=true;   //��������� ����� � ������ �����
	same=true;

	HDC hdc = GetDC(0);


	static int W;
	W = Screen->Width; //1920
	static int H;
	H = Screen->Height;  //1080






	if (hdc)
	{
		if(l1==1)
		{
			if(l == 1)
			{
				bmp1->Width = W;
				bmp1->Height = H;
				BitBlt(bmp1->Canvas->Handle, 0, 0, W, H, hdc, 0, 0, SRCCOPY);

			}
			else
			{
				bmp2->Width = W;
				bmp2->Height = H;
				BitBlt(bmp2->Canvas->Handle, 0, 0, W, H, hdc, 0, 0, SRCCOPY);

			}
			for(int y = 0;y<H;y++)
			{
				if(l == 1)	z[y] =(TRGBTriple *)bmp1->ScanLine[y];
				else 	z1[y] =(TRGBTriple *)bmp2->ScanLine[y];
				for(int x = 0; x<W && same==true ;x++)
				{
					if(z[y][x].rgbtRed==z1[y][x].rgbtRed && z[y][x].rgbtGreen==z1[y][x].rgbtGreen
					&& z[y][x].rgbtBlue==z1[y][x].rgbtBlue || (y>1000 && x>1500))   //���� ������
					{                                                               //������ ��� ������ �� ��������� ���� � �����.
					}                                                               // ��� �������� ������ ��� ���������� 1920�1080
					else
						same=false;
				}
			}
		}
		else
		{   //������ �����
			l1 = 1;
			bmp2->Width = W;
			bmp2->Height = H;
			BitBlt(bmp2->Canvas->Handle, 0, 0, W, H, hdc, 0, 0, SRCCOPY);

			for(int y = 0;y<H;y++)
				z1[y] =(TRGBTriple *)bmp2->ScanLine[y];
			same=false;
		}
		if(same==false)
		{
			TJPEGImage* jpg = new TJPEGImage();
			__try
			{
				if(l==1) jpg->Assign(bmp1.get());
				if(l==0) jpg->Assign(bmp2.get());
				jpg->CompressionQuality = compress;
				jpg->SaveToFile(name);
			}
			__finally
			{delete jpg;}
		}
	}
	if(l==0) l=1;
	else l=0;       //*/
	ReleaseDC(0,hdc);
}
