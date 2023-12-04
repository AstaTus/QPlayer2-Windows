#include "PlayStartPostitionWindow.h"
#include <CommCtrl.h>
#define CLASS_NAME  L"PlayStartPostitionWindow"
#define ID_PlAY_START_POSITION_INPUT   1000

#define ID_SUBMIT_BUTTON 1100
#define ID_CANCEL_BUTTON 1101
PlayStartPostitionWindow::PlayStartPostitionWindow(HWND hwnd,HINSTANCE hinstance, PlayStartPostitionWindowCloseCallBackFunction call_back)
{
	mHinstance = GetModuleHandle(NULL);
	WNDCLASSEXW wcex;
	if (GetClassInfoExW(hinstance, CLASS_NAME, &wcex) != 0) {

	}
	else
	{
		wcex.cbSize = sizeof(WNDCLASSEX);

		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = main_play_start_position_window_proc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = mHinstance;
		wcex.hIcon = NULL;
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = NULL;
		wcex.lpszClassName = CLASS_NAME;
		wcex.hIconSm = NULL;

		if (!RegisterClassExW(&wcex)) {
			throw "UrlSetting  create failed!";
		}
	}
	mCloseCallBack = call_back;
	int screen_width = GetSystemMetrics(SM_CXSCREEN);
	int screen_height = GetSystemMetrics(SM_CYSCREEN);
	int window_width = 400;
	int window_height = 150;
	int window_x = (screen_width - window_width) / 2;
	int window_y = (screen_height - window_height) / 2;
	mHwnd = CreateWindowW(wcex.lpszClassName, (LPCWSTR)_T(CLASS_NAME), WS_OVERLAPPEDWINDOW | WS_SYSMENU | WS_MINIMIZEBOX,
		window_x, window_y, window_width, window_height, hwnd, NULL, mHinstance, NULL);
	if (mHwnd == nullptr)
	{
		throw "UrlSetting window create failed!";
	}
	SetWindowLongPtr(mHwnd, GWLP_USERDATA, (LONG_PTR)this);

	on_list_create();

	ShowWindow(mHwnd, SW_SHOW);
	UpdateWindow(mHwnd);
}

PlayStartPostitionWindow::~PlayStartPostitionWindow()
{

}

LRESULT CALLBACK PlayStartPostitionWindow::main_play_start_position_window_proc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param) {

	PlayStartPostitionWindow* pposition_window = (PlayStartPostitionWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	switch (message)
	{
	case WM_COMMAND:
	{
		int item_id = LOWORD(w_param);
		switch (item_id)
		{
		case ID_SUBMIT_BUTTON:
		{
			pposition_window->mCloseCallBack(WindowCloseType::SUBMIT_CLOSE,100);
			break;
		}
		case ID_CANCEL_BUTTON:
		{
			break;
		}
		default:
			break;
		}
	}
	default:
		break;
	}
	return (DefWindowProc(hwnd, message, w_param, l_param));
}

void PlayStartPostitionWindow::on_list_create() {

	RECT windowRect;
	GetWindowRect(mHwnd, &windowRect);
	int width = windowRect.right - windowRect.left;
	int height = windowRect.bottom - windowRect.top;
	CreateWindow(TEXT("STATIC"), TEXT("��ʱ��"), WS_CHILD | WS_VISIBLE, 10, 40, 90, 20, mHwnd, NULL, NULL, NULL);
	mPlayStartPositionInput = CreateWindow(TEXT("EDIT"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER | ES_AUTOHSCROLL, 110, 40, width - 130, 20, mHwnd, (HMENU)ID_PlAY_START_POSITION_INPUT, mHinstance, NULL);

	CreateWindow(TEXT("BUTTON"), TEXT("ȷ��"), WS_CHILD | WS_VISIBLE, width / 2 - 55, height - 70, 50, 20, mHwnd, (HMENU)ID_SUBMIT_BUTTON, NULL, NULL);

	CreateWindow(TEXT("BUTTON"), TEXT("ȡ��"), WS_CHILD | WS_VISIBLE, width / 2, height - 70, 50, 20, mHwnd, (HMENU)ID_CANCEL_BUTTON, NULL, NULL);

}
std::string PlayStartPostitionWindow::wchar_to_string(HWND hwnd) {
	int text_length = GetWindowTextLengthW(hwnd);
	if (text_length == 0)
	{
		return "";
	}
	wchar_t* pbuffer = new wchar_t[text_length + 1];
	GetWindowTextW(hwnd, pbuffer, text_length + 1);

	std::wstring wstr(pbuffer);
	std::string input_text;
	int str_length = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
	if (str_length > 0)
	{
		char* pstr_buffer = new char[str_length];
		WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, pstr_buffer, str_length, NULL, NULL);
		input_text = pstr_buffer;
		delete[] pstr_buffer;
	}
	delete[] pbuffer;
	return input_text;
}