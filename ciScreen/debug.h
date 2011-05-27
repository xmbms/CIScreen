/*
 * @Author XM
 * Debug Tools for C++
 * usage:
 *		console.info(...)
 *		console.warn(...)
 *		console.error(...)
 *      console.tic();
 *      console.toc();
 * NOTICE:
 *      please copy debug.h debug.cpp gdebug.cpp to your project and don't use unicode charset.
 * All Right reserved
 */

#ifndef __DEBUG_CONSOLE

#define __DEBUG_CONSOLE

#include <Windows.h>
#include <string>
#include <map>
using namespace std;

typedef int state;

class CDebugConsole{
public:
	void info(char *fmt, ...);
	void info(float str);
	void info(string str);
	
	void error(string str);
	void error(float str);
	void error(char *ftm, ...);

	void warn(string str);
	void warn(float str);
	void warn(char *ftm, ...);
	int tic();
	void toc(int key = -1, string tips = "Elapsed time is");
	void toc(string tips);
	CDebugConsole();


private:
	BOOL createConsole();
	BOOL started ;
	BOOL feature;
	void setTextColor(WORD color);
	void start();
	void output(string str, state sta);
	void outputLabel(state sta);
	void setState(state sta);
	LONGLONG getCounters();
	state savedState;
	string savedContent ;
	int repeat;
    int defaultColors;
	int maxlabel;
	HANDLE console;
	bool first;
	LONGLONG freq;
	map<int, LONGLONG> timers;
};


extern CDebugConsole console;

#endif