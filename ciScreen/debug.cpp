#include "debug.h"
#include <tchar.h>
#include <iostream>
#include <sstream>
#include <windows.h>
using namespace std;

#ifndef __DEBUG__
#define __DEBUG__
#endif

#define TEXT_WHITE   FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE
#define TEXT_RED     FOREGROUND_RED
#define TEXT_GREEN   FOREGROUND_GREEN
#define TEXE_BLUE    FOREGROUND_BLUE
#define TEXT_GRAY    FOREGROUND_INTENSITY
#define TEXT_YELLOW  FOREGROUND_RED|FOREGROUND_GREEN

#define BG_BLACK 
#define STATE_NONE  0
#define STATE_INFO  1
#define STATE_WARN  2
#define STATE_ERROR 3
#define STATE_TIMER 4

CDebugConsole::CDebugConsole(){
	//Nothing to do
	started = false;
	feature = false;
	savedState   = STATE_NONE;
	savedContent = "";
	maxlabel = 6;
	repeat = 0;
	first = false;
    defaultColors = 0;
	LARGE_INTEGER temp;
	QueryPerformanceFrequency(&temp);
	freq = temp.QuadPart;

}

void CDebugConsole::start(){
	if(!started){
		if(createConsole()){
			DWORD count;
			string str = "\tDebug Tools Kit for Windows Power By XM\n";
			WriteConsole(console, str.c_str(), str.length(), &count, NULL);
			str = "------------------------------------------------------\n";
			WriteConsole(console, str.c_str(), str.length(), &count, NULL);
		}
		started = true;
	}
}

void CDebugConsole::setTextColor(WORD color){
	if(color != NULL){
        SetConsoleTextAttribute(console, color);
	}
}

BOOL CDebugConsole::createConsole() {
    BOOL feature = true;
    if(AllocConsole()){
        feature = true;
    }
	SetConsoleTitle(_T("Debug Console"));
	console = GetStdHandle(STD_OUTPUT_HANDLE);
	if(console == INVALID_HANDLE_VALUE){
		return false;
	}
	if(SetConsoleMode(console, ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT ) == 0){
		return false;
	}
    CONSOLE_SCREEN_BUFFER_INFO con_info;
    GetConsoleScreenBufferInfo(console, &con_info);
    defaultColors = con_info.wAttributes;
	return true;
}

void CDebugConsole::outputLabel(state sta){
    CONSOLE_SCREEN_BUFFER_INFO con_info;
    GetConsoleScreenBufferInfo(console, &con_info);
    int savedColors = con_info.wAttributes;
	setTextColor(TEXT_GRAY);
	string label ="";
	switch(sta){
	case STATE_INFO:
		label = "INFO:";
		break;
	case STATE_WARN:
		label = "WARN:";
		break;
	case STATE_ERROR:
		label = "ERROR:";
		break;
	case STATE_TIMER:
		label = "TIMER:";
		break;
	default:
		label = "UNKNOWN:";
		break;
	}
	DWORD count;
	WriteConsole(console, label.c_str(), label.length(), &count, NULL);
    //WriteConsole(std, label.c_str(), label.length(), &count, NULL);
    SetConsoleTextAttribute(console, savedColors);
}

void CDebugConsole::output(string str,state sta){
	DWORD count;
	if(sta == savedState && savedContent == str){
		//collapse output
		//in XXXX times
        repeat ++;
		string tips ;
        stringstream strStream;
        strStream<<" (repeat in "<<repeat<<" times)";
        tips = strStream.str();
        WriteConsole(console, "\r", 1, &count, 0); // New Line
        outputLabel(sta);
        WriteConsole(console, str.c_str(), str.length(), &count, NULL);
        WriteConsole(console, tips.c_str(), tips.length(), &count, NULL);
	}else{
		//label change
		if(first){
			WriteConsole(console, "\n", 1, &count, 0); // New Line
		}else{
			first = true;
		}
        outputLabel(sta);
		WriteConsole(console, str.c_str(), str.length(), &count, NULL);
        repeat = 1;
	}
    savedState = sta;
    savedContent = str;
    SetConsoleTextAttribute(console, defaultColors);
}

void CDebugConsole::info(char *fmt, ...){
	char s[300];
	va_list argptr;
	int cnt;

	va_start(argptr, fmt);
	cnt = vsprintf(s, fmt, argptr);
	va_end(argptr);
	string str = s;
	info(str);
}

void CDebugConsole::warn(char *fmt, ...){
	char s[300];
	va_list argptr;
	int cnt;
	va_start(argptr, fmt);
	cnt = vsprintf(s, fmt, argptr);
	va_end(argptr);
	string str = s;
	warn(str);
}

void CDebugConsole::error(char *fmt, ...){
	char s[300];
	va_list argptr;
	int cnt;

	va_start(argptr, fmt);
	cnt = vsprintf(s, fmt, argptr);
	va_end(argptr);
	string str = s;
	error(str);
}

void CDebugConsole::info(string str){
    start();
	setTextColor(TEXT_WHITE);
	output(str,STATE_INFO);
}

void CDebugConsole::warn(string str){
    start();
	setTextColor(TEXT_GREEN);
	output(str,STATE_WARN);
}

void CDebugConsole::error(string str){		
    start();
	setTextColor(TEXT_RED);
	output(str, STATE_ERROR);
}

void CDebugConsole::info(float str){
    stringstream ss;
    ss<<str;
    string tips = ss.str();
    info(tips);
}

void CDebugConsole::warn(float str){
    stringstream ss;
    ss<<str;
    string tips = ss.str();
    warn(tips);
}

void CDebugConsole::error(float str){		
    stringstream ss;
    ss<<str;
    string tips = ss.str();
    error(tips);
}

LONGLONG CDebugConsole::getCounters(){
	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);
	return counter.QuadPart;
}

int CDebugConsole::tic(){
	LONGLONG count = getCounters();
	int key = timers.size();
	timers.insert(make_pair(key, count));
	return key;
}

void CDebugConsole::toc(string tips){
	int key = timers.size() - 1;
	toc(key, tips);
}

void CDebugConsole::toc(int key, string tips){
	LONGLONG count = getCounters();
	start();
	if(key == -1){
		key = timers.size() - 1;
	}
	stringstream ss;
	if(key < 0){
		ss<<"Timer is not started before toc";
		setTextColor(TEXT_RED);
	}else{
		if(timers.count(key)){
			double t = (double)(count - timers[key]) / freq;
			if( t < 10){
				ss<<tips<<" "<< t * 1000 << " ms";
			}else{
				ss<<tips<<" "<< t  << " s";
			}
			setTextColor(TEXT_YELLOW);
		}else{
			ss<<"Can not find the specified key";
			setTextColor(TEXT_RED);
		}
	}
	tips = ss.str();
	output(tips, STATE_TIMER);
}
