#include "UrlListWindow.h"
#include <iostream>
#include "resource.h"
#include "DemoLog.h"
#include <CommCtrl.h>

#define TAG					    "UrlListWindow"



UrlListWindow::UrlListWindow(HWND hwnd, HINSTANCE hinstance)
	: mHwnd(nullptr),
	mHinstance(nullptr)
{

	HINSTANCE hInst = (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE);
	mHinstance = hInst;
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = main_url_list_window_proc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInst;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = TEXT(L"UrlListWindow");
	wcex.hIconSm = NULL;

	if (!RegisterClassExW(&wcex)) {
		throw "UrlListWindow  create failed!";
	}

	mHwnd = CreateWindowW(wcex.lpszClassName, TEXT(L"UrlListWindow"), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		10, 10, 80, 20, hwnd, NULL, hInst, NULL);

	if (mHwnd == nullptr)
	{
		throw "main window create failed!";
	}

	SetWindowLongPtr(mHwnd, GWLP_USERDATA, (LONG_PTR)this);

	mpModelManager = new PlayerUrlListModelManager();

	on_list_create();
}

UrlListWindow::~UrlListWindow()
{
}

LRESULT CALLBACK UrlListWindow::main_url_list_window_proc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param) {
	UrlListWindow* url_window = (UrlListWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

	switch (message)
	{
	case WM_SIZE:
	{
		if (url_window != nullptr) {
			//url_window->url_list_resize();
		}
		break;
	}
	case WM_COMMAND:
	{
		int wmId = LOWORD(w_param);
		break;
	}
	case WM_NOTIFY:
	{
		NMHDR* pnmhdr = (NMHDR*)l_param;
		if (pnmhdr->hwndFrom == url_window->mListWindow)
		{
			if (pnmhdr->code == LVN_ITEMACTIVATE)
			{
				LPNMITEMACTIVATE pnmia = (LPNMITEMACTIVATE)l_param;
				int selected_index = pnmia->iItem;
				// �����ﴦ��˫���б���ͼ����߼�
				// ����ʹ�� selectedIndex ����ȡ��ѡ�������
				url_window->url_click(selected_index);
			}
		}
		break;
	}
	default:
		break;
	}
	return (DefWindowProc(hwnd, message, w_param, l_param));
}

void UrlListWindow::url_list_resize() {
	RECT windowRect;
	GetWindowRect(mHwnd, &windowRect);

	// ���㴰�ڵĿ�Ⱥ͸߶�
	int width = windowRect.right - windowRect.left;
	int height = windowRect.bottom - windowRect.top;
	SetWindowPos(mListWindow, nullptr, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER);

}
LRESULT UrlListWindow::on_list_create() {

	// �����б���ͼ�ؼ�
	mListWindow = CreateWindow(WC_LISTVIEW, "", WS_CHILD | WS_VISIBLE, 0, 0, 500, 400, mHwnd, NULL, NULL, NULL);

	// �����б���ͼ�ؼ�����ʽ
	ListView_SetExtendedListViewStyle(mListWindow, LVS_EX_FULLROWSELECT);

	// �����ͷ
	LVCOLUMNW  lvColumn;
	lvColumn.mask = LVCF_TEXT | LVCF_WIDTH;
	lvColumn.pszText = (LPWSTR)_T("url");
	lvColumn.cx = 300;
	ListView_InsertColumn(mListWindow, 0, &lvColumn);

	// ���������
	LVITEMW  lvItem;
	lvItem.mask = LVIF_TEXT;
	lvItem.iSubItem = 0;
	for (int i = 0; i < mpModelManager->get_url_models_count();i++)
	{
		lvItem.pszText = (LPWSTR)_T(mpModelManager->get_url_model_for_index(i)->mName.c_str());
		lvItem.iItem = i;
		ListView_InsertItem(mListWindow, &lvItem);
	}



	return 0;
}

LPWSTR UrlListWindow::string_to_LPWSTR(const std::string& str) {
	int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
	LPWSTR buffer = new WCHAR[size];
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, buffer, size);
	return buffer;
}
HWND UrlListWindow::get_hwnd() {
	return mHwnd;
}

void  UrlListWindow::url_click(int url_id) {

	mPlayControlCallback(mHwnd,mpModelManager->get_url_model_for_index(url_id)->mpModel);
	
}

void UrlListWindow::set_play_control_callback(playCallbackFunction callBack) {
	mPlayControlCallback = callBack;
}