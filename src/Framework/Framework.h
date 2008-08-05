#	pragma once

#define EXAMPLEUNMANAGEDDLL_API __declspec(dllexport)

class EXAMPLEUNMANAGEDDLL_API Framework
{
public:
	Framework();
	~Framework();
	void createWindow(HWND hwnd);
};