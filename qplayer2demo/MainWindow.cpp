#include "MainWindow.h"
#include "resource.h"
#include <commctrl.h>
#include "VideoRenderWindow.h"
#include "PlayerWindow.h"
#include "DemoLog.h"
#include "QIPlayerContext.h"
#include "QIPlayerControlHandler.h"
#include "QIPlayerRenderHandler.h"
#include "QMediaModelBuilder.h"
#include "QMediaModel.h"
#include <iostream>
#include <filesystem>
#include "UrlListWindow.h"
#include "CurrentDataModelManager.h"
#include <iomanip>
#include <sstream>
#include <Windows.h>
#include <CommCtrl.h>
#define CLASS_NAME                     "MainWindow"
#define ID_VIDEO_RENDER_WINDOW  200
#define ID_SEEK_BAR             202
#define ID_FPS_STATIC_TEXT      203
#define ID_BITRATE_STATIC_TEXT  204
#define ID_DOWNLOAD_STATIC_TEXT 205
#define ID_TIME_STATIC_TEXT     206
#define ID_PLAYER_WINDOW        207
#define ID_URL_LIST_WINDOW      208
#define ID_FIRST_FRAME_STATIC_TEXT      209
#define ID_PLAY_STATE_STATIC_TEXT      210
#define ID_TOAST_WINDOW         211
#define ID_QUALITY_LIST_WINDOW      212
#define ID_PAUSE_PLAY_BUTTON    301
#define ID_RESUME_PLAY_BUTTON   302
#define ID_URL_SETTING_WINDOW   303
#define ID_QUALITY_CHANGE_BUTTON    304


#define CLASS_NAME "UrlSetting"
using namespace QMedia;

//����Ϣ
LRESULT MainWindow::main_window_proc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param)
{
    MainWindow* pmain_window = (MainWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    
    
    switch (message)
    {

	case WM_NOTIFY:
	{
		NMHDR* pnmhdr = (NMHDR*)l_param;
		if (pnmhdr->hwndFrom == pmain_window->mPlayerQualityChangeListWindow)
		{
			LPNMITEMACTIVATE pnmia = (LPNMITEMACTIVATE)l_param;
			int selected_index = pnmia->iItem;
			std::string index_str = std::to_string((int)pnmhdr->code) + "  code : " + std::to_string((int)NM_CLICK);
			DemoLog::log_string(CLASS_NAME, __LINE__, index_str.c_str());
			if ((int)(pnmhdr->code) == (int)NM_CLICK)
			{
				pmain_window->quality_change_click(selected_index);
			}
		}
		break;
	}
    case WM_SIZE:
    {
		//�޸Ĵ��ڴ�С
        if (pmain_window != nullptr)
        {
            pmain_window->on_resize();
        }
    }
    break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(w_param);
        pmain_window->button_click(wmId);
    }
    break;
	case WM_HSCROLL: {
		// �����������Ϣ
		if (reinterpret_cast<HWND>(l_param) == pmain_window->mSeekBar)
		{
			int notificationCode = LOWORD(w_param);  // ��ȡ֪ͨ����
            if (notificationCode == TB_ENDTRACK) {
				long current_position = SendMessage(pmain_window->mSeekBar, TBM_GETPOS, 0, 0);
				pmain_window->seek_bar_click(current_position);
				CurrentDataModelManager::get_instance()->set_is_seeking(true);
            }
            else if(notificationCode == TB_PAGEDOWN || notificationCode == TB_PAGEUP)
			{
				POINT pt;
				GetCursorPos(&pt);
				ScreenToClient(pmain_window->mSeekBar, &pt);
				int min_value, max_value;
				SendMessage(pmain_window->mSeekBar, TBM_GETRANGEMIN, TRUE, (LPARAM)&min_value);
				SendMessage(pmain_window->mSeekBar, TBM_GETRANGEMAX, TRUE, (LPARAM)&max_value);
				RECT rect;
				GetClientRect(pmain_window->mSeekBar, &rect);
                int pos = (int)(((double)(pt.x - 10) / (rect.right - rect.left - 10)) * (CurrentDataModelManager::get_instance()->get_duration_time()));
				SendMessage(pmain_window->mSeekBar, TBM_SETPOS, TRUE, pos);
				CurrentDataModelManager::get_instance()->set_is_seeking(true);
            }
            else
            {
                CurrentDataModelManager::get_instance()->set_is_seeking(true);
            }

		}

		break;
	}
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
	
    default:
        return DefWindowProc(hwnd, message, w_param, l_param);
    }
    
    return 0;
}

//���캯��
MainWindow::MainWindow(HINSTANCE instance, int n_cmd_show)
	:mHinstance(instance),
    mHwnd(nullptr),
    mpUrlListModelManger(new PlayerUrlListModelManager())
{
    LoadStringW(mHinstance, IDS_APP_TITLE, mTitle, MAX_LOADSTRING);
    LoadStringW(mHinstance, IDC_QPLAYER2DEMO, mWindowClass, MAX_LOADSTRING);

    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = main_window_proc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = mHinstance;
	wcex.hIcon = LoadIcon(mHinstance, MAKEINTRESOURCE(IDI_QPLAYER2DEMO));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_QPLAYER2DEMO);
    wcex.lpszClassName = mWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    if (!RegisterClassExW(&wcex)) {
        throw "main window  create failed!";
    }
	mHwnd = CreateWindowW(mWindowClass, (LPCWSTR)_T("QPlayer2 Demo"), WS_OVERLAPPEDWINDOW | WS_SYSMENU | WS_MINIMIZEBOX,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, mHinstance, nullptr);

    if (mHwnd == nullptr)
    {
        throw "main window create failed!";
    }
    SetWindowLongPtr(mHwnd, GWLP_USERDATA, (LONG_PTR)this);

	CurrentDataModelManager::get_instance()->set_is_seeking(false);

	//�����ؼ�
    on_create();
	//��Ӳ�����listener
	add_listeners();
	//�����˵���ť
	on_create_play_menu();
	//���ŵ�һ����Ƶ
	mpPlayerWindow->get_control_handler()->play_media_model(mpUrlListModelManger->get_url_model_for_index(0)->get_media_model(), 0);
	CurrentDataModelManager::get_instance()->set_media_model(mpUrlListModelManger->get_url_model_for_index(0)->get_media_model());
	update_quality_list_window();
    ShowWindow(mHwnd, n_cmd_show);
    UpdateWindow(mHwnd);
}
//��������
MainWindow::~MainWindow()
{
    if (mpPlayerWindow != nullptr)
    {
        delete mpPlayerWindow;
        mpPlayerWindow = nullptr;
    }
   
}
//���ش��ھ��
HWND MainWindow::get_hwnd()
{
	return mHwnd;
}
//�����ؼ�
LRESULT MainWindow::on_create()
{
    HWND child_hwnd;


    //��Ƶ��ʾ����
	mpPlayerWindow = new PlayerWindow(mHwnd, mHinstance);
	child_hwnd = mpPlayerWindow->get_hwnd();
	SetWindowLong(child_hwnd, GWL_ID, ID_PLAYER_WINDOW);


    //����ʱ��
    mPlayerProgressAndDurationText = CreateWindow(TEXT("STATIC"), TEXT("00:00/00:00"), WS_CHILD | WS_VISIBLE, 10, 10, 80, 20, mHwnd, (HMENU)ID_TIME_STATIC_TEXT, NULL, NULL);
    //������ͣ��ť
    mPlayButton =  CreateWindow(TEXT("BUTTON"), TEXT("Play"), WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 120, 70, 54, 20, mHwnd, (HMENU)ID_PAUSE_PLAY_BUTTON, NULL, NULL);

	//�����Ȱ�ť
	mPlayQualityButton = CreateWindow(TEXT("BUTTON"), TEXT("�����ȣ�"), WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 120, 70, 54, 20, mHwnd, (HMENU)ID_QUALITY_CHANGE_BUTTON, NULL, NULL);

	//�������б�
	mPlayerQualityChangeListWindow = CreateWindow(WC_LISTVIEW, TEXT(""), WS_TABSTOP | LVS_NOSCROLL | WS_VISIBLE | WS_CHILD | LVS_REPORT | LVS_SINGLESEL | LVS_NOCOLUMNHEADER | WS_BORDER, 120, 70, 54, 20, mHwnd, (HMENU)ID_QUALITY_LIST_WINDOW, NULL, NULL);

	ListView_SetExtendedListViewStyle(mPlayerQualityChangeListWindow, LVS_EX_FULLROWSELECT);
	ShowWindow(mPlayerQualityChangeListWindow, SW_HIDE);
    //������
    mSeekBar = CreateWindow(TRACKBAR_CLASS, TEXT("Trackbar Control"), WS_CHILD | WS_VISIBLE , 10, 40, 200, 25, mHwnd, (HMENU)ID_SEEK_BAR, NULL, NULL);
    //�����ٶ�
    mPlayerDownLoadText = CreateWindow(TEXT("STATIC"), TEXT("download speed:"), WS_CHILD | WS_VISIBLE, 10, 10, 80, 20, mHwnd, (HMENU)ID_DOWNLOAD_STATIC_TEXT, NULL, NULL);
    //����
    mPlayerBiteRateText = CreateWindow(TEXT("STATIC"), TEXT("biterate:"), WS_CHILD | WS_VISIBLE, 10, 10, 80, 20, mHwnd, (HMENU)ID_BITRATE_STATIC_TEXT, NULL, NULL);
    //FPS
    mFPSText = CreateWindow(TEXT("STATIC"), TEXT("FPS:"), WS_CHILD | WS_VISIBLE, 10, 10, 80, 20, mHwnd, (HMENU)ID_FPS_STATIC_TEXT, NULL, NULL);

	//������״̬
	mPlayerStateText = CreateWindow(TEXT("STATIC"), TEXT("play state:"), WS_CHILD | WS_VISIBLE, 10, 10, 80, 20, mHwnd, (HMENU)ID_PLAY_STATE_STATIC_TEXT, NULL, NULL);
	//��֡
	mFirstFrameText = CreateWindow(TEXT("STATIC"), TEXT("first frame:"), WS_CHILD | WS_VISIBLE, 10, 10, 80, 20, mHwnd, (HMENU)ID_FIRST_FRAME_STATIC_TEXT, NULL, NULL);

    //���ŵ�ַ�б�
    mpUrlListWindow = new UrlListWindow(mHwnd, mHinstance, mpUrlListModelManger);
	//��ַ�б��������ص�
    mpUrlListWindow->set_play_control_callback(
        [this](HWND hwnd, QMedia::QMediaModel* pmodel) {
            url_Click_call_back(hwnd, pmodel);
        }
    );
	//��ַ�б��Ҽ�����ص�
    mpUrlListWindow->set_mouse_right_click_callback(
        [this](int item_id, UrlClickType url_click_type) {
			if (url_click_type == UrlClickType::DELETE_URL)
			{
				mpUrlListModelManger->delete_url_model_index(item_id);
				mpUrlListModelManger->url_update();
				return;
			}
            EnableWindow(mHwnd, FALSE);
            mpUrlSettingWindow = new UrlSetting(mHwnd, mHinstance, mpUrlListModelManger, url_click_type,item_id);
            mpUrlSettingWindow->set_url_setting_close_call_back(
                [this]() {
                    EnableWindow(mHwnd, TRUE);
                }
            );
            SetWindowLong(mpUrlSettingWindow->get_hwnd(), GWL_ID, ID_URL_SETTING_WINDOW);
        }
    );
    SetWindowLong(mpUrlListWindow->get_hwnd(), GWL_ID, ID_URL_LIST_WINDOW);
	//��־�б�
	mpToastWindow = new ToastWindow(mHwnd);
	SetWindowLong(mpToastWindow->get_hwnd(), GWL_ID, ID_TOAST_WINDOW);

    
    return TRUE;
}
void MainWindow::update_quality_list_window() {
	ListView_DeleteAllItems(mPlayerQualityChangeListWindow); // ����б�
	// �����ͷ
	LVCOLUMNW  lv_column;
	lv_column.mask = LVCF_WIDTH;
	lv_column.cx = 300;
	ListView_InsertColumn(mPlayerQualityChangeListWindow, 0, &lv_column);
	LVITEMW lvItem;
	lvItem.mask = LVIF_TEXT;

	for (size_t i = 0; i < CurrentDataModelManager::get_instance()->get_media_model()->get_stream_elements().size(); ++i) {
		lvItem.iItem = i;
		lvItem.iSubItem = 0;
		
		QMedia::QStreamElement* pele = (CurrentDataModelManager::get_instance()->get_media_model()->get_stream_elements())[i];
		std::string quality_string = std::to_string(pele->get_quality_index());
		lvItem.pszText = (LPWSTR)_T(quality_string.c_str());

		ListView_InsertItem(mPlayerQualityChangeListWindow, &lvItem);
		if (i == 0)
		{
			std::string title = "�����ȣ�" + std::to_string(pele->get_quality_index());
			SetWindowText(mPlayQualityButton, TEXT(title.c_str()));
			CurrentDataModelManager::get_instance()->set_quality(pele->get_quality_index());
		}
	}
}
//�������л��б�����Ӧ�¼�
void MainWindow::quality_change_click(int item_id) {
	QMedia::QMediaModel* pinner_model = CurrentDataModelManager::get_instance()->get_media_model();
	QMedia::QStreamElement* pinner_ele = pinner_model->get_stream_elements()[item_id];
	QualityImmediatyly immediaty = CurrentDataModelManager::get_instance()->get_quality_immediatyly();
	switch (immediaty)
	{
	case IMMEDIATYLY_TRUE:
		mpPlayerWindow->get_control_handler()->switch_quality("", pinner_ele->get_url_type(), pinner_ele->get_quality_index(), true);
		break;
	case IMMEDIATYLY_FALSE:
		mpPlayerWindow->get_control_handler()->switch_quality("", pinner_ele->get_url_type(), pinner_ele->get_quality_index(), false);
		break;
	case IMMEDIATYLY_CUSTOM: {
		if (pinner_model->is_live() == true)
		{
			mpPlayerWindow->get_control_handler()->switch_quality("", pinner_ele->get_url_type(), pinner_ele->get_quality_index(), true);
		}
		else {

			mpPlayerWindow->get_control_handler()->switch_quality("", pinner_ele->get_url_type(), pinner_ele->get_quality_index(), false);
		}
		break;
	}
	default:
		break;
	}

	ShowWindow(mPlayerQualityChangeListWindow, SW_HIDE);

	std::string title = "�����ȣ�" + std::to_string(pinner_ele->get_quality_index());
	SetWindowText(mPlayQualityButton, TEXT(title.c_str()));
	CurrentDataModelManager::get_instance()->set_quality(pinner_ele->get_quality_index());
}
//URL�б���������Ӧ�¼������Ŷ�Ӧ��Ƶ
void MainWindow::url_Click_call_back(HWND hwnd, QMedia::QMediaModel* pmodel) {
    if (mpPlayerWindow != nullptr)
    {
        mpPlayerWindow->get_control_handler()->play_media_model(pmodel, 0);

		CurrentDataModelManager::get_instance()->set_media_model(pmodel);
    }
}
//�����˵�����ť
LRESULT MainWindow::on_create_play_menu() {
    
    mpSettingMenuManager = new PlayerMenuSettingModelManager(mHwnd,mpPlayerWindow);
    std::list<PlayerMenuSettingModel*>* setting_model = mpSettingMenuManager->get_menu_setting_model();

    HMENU base_menu = CreateMenu();
	for (int parent_index = 0; parent_index < setting_model->size(); parent_index++) {
        auto parent_it = setting_model->begin();
        std::advance(parent_it, parent_index);
        //����һ���˵���ť
		AppendMenu(base_menu, MF_STRING | MF_POPUP, (UINT_PTR)((*parent_it)->get_child_menu_model()->get_menus()),(*parent_it)->get_name().c_str());
        for (int child_index = 0; child_index < (*parent_it)->get_child_menu_model()->get_child_menus_list()->size(); child_index++) {

			auto child_it = (*parent_it)->get_child_menu_model()->get_child_menus_list()->begin();
			std::advance(child_it, child_index);
			//���������˵���ť
			AppendMenu((*parent_it)->get_child_menu_model()->get_menus(), MF_STRING, (*child_it)->get_id(), (*child_it)->get_name().c_str());
			if ((*child_it)->get_is_selected())
			{
				CheckMenuItem((*parent_it)->get_child_menu_model()->get_menus(), (*child_it)->get_id(), MF_CHECKED);
			}
        }

    }
    SetMenu(mHwnd, base_menu);

    return 0;
}
//���ڴ�С����
LRESULT MainWindow::on_resize()
{
    RECT root_window_rect;
    GetClientRect(mHwnd, &root_window_rect);
    EnumChildWindows(mHwnd, resize_child_windows_proc, (LPARAM)(&root_window_rect));
    notify_resize_to_player();
    return TRUE;
}

BOOL MainWindow::resize_child_windows_proc(HWND hwndChild, LPARAM lParam)
{
    LPRECT proot_window_rect;
    int child_window_id;
    child_window_id = GetWindowLong(hwndChild, GWL_ID);
    proot_window_rect = (LPRECT)lParam;
    int parent_width = proot_window_rect->right - proot_window_rect->left;
    int parent_height = proot_window_rect->bottom- proot_window_rect->top;
    if (child_window_id == ID_PLAYER_WINDOW) {
        MoveWindow(hwndChild, 10, 10, get_render_window_width(parent_width), get_render_window_height(parent_height), TRUE);
		
    }
    else if (child_window_id == ID_TIME_STATIC_TEXT)
    {
        MoveWindow(hwndChild, 10, parent_height - 55, 80, 20, TRUE);
    }
    else if (child_window_id == ID_SEEK_BAR)
    {
        MoveWindow(hwndChild, 100, parent_height - 60, parent_width - 330, 25, TRUE);
    }
    else if (child_window_id == ID_PAUSE_PLAY_BUTTON)
    {
        MoveWindow(hwndChild, parent_width - 70, parent_height - 60, 60, 30, TRUE);
    }

	else if (child_window_id == ID_QUALITY_CHANGE_BUTTON)
	{
		MoveWindow(hwndChild, parent_width - 220, parent_height - 60, 130, 30, TRUE);
	}
	else if (child_window_id == ID_QUALITY_LIST_WINDOW)
	{
		MoveWindow(hwndChild, parent_width - 220, parent_height - 60 - CurrentDataModelManager::get_instance()->get_media_model()->get_stream_elements().size() * 20, 130, 20 * CurrentDataModelManager::get_instance()->get_media_model()->get_stream_elements().size(), TRUE);
	}
    else if (child_window_id == ID_DOWNLOAD_STATIC_TEXT) {
        MoveWindow(hwndChild, 10, parent_height - 25, 250, 20, TRUE);
    }
    else if (child_window_id == ID_BITRATE_STATIC_TEXT) {
        MoveWindow(hwndChild, 260, parent_height - 25, 150, 20, TRUE);
    }
    else if (child_window_id == ID_FPS_STATIC_TEXT) {
        MoveWindow(hwndChild, 420, parent_height - 25, 100, 20, TRUE);
    }
    else if (child_window_id == ID_FIRST_FRAME_STATIC_TEXT)
    {
        MoveWindow(hwndChild, 530, parent_height - 25, 170, 20, TRUE);
    }
	else if (child_window_id == ID_PLAY_STATE_STATIC_TEXT)
	{
		MoveWindow(hwndChild, 710, parent_height - 25, 220, 20, TRUE);
	}
    else if (child_window_id == ID_URL_LIST_WINDOW)
    {
        MoveWindow(hwndChild, parent_width - 310 ,10, 300, parent_height - 280, TRUE);
    }
	else if (child_window_id == ID_TOAST_WINDOW)
	{
		MoveWindow(hwndChild, parent_width - 310, parent_height - 270, 300, 190, TRUE);
	}
    return TRUE;
}


int MainWindow::get_render_window_height(int parent_window_height)
{
    if (parent_window_height - 80 < 0) {
        return 0;
    }
    return parent_window_height - 80;
}

int MainWindow::get_render_window_width(int parent_window_width)
{
	if (parent_window_width - 320 < 0) {
		return 0;
	}
    return parent_window_width - 320;
}

bool MainWindow::notify_resize_to_player()
{
    
    if (mpPlayerWindow != nullptr)
    {
        RECT render_window_rect;
        GetClientRect(mpPlayerWindow->get_hwnd(), &render_window_rect);
        return true;

    }

    return false;
}
//�����������Ӧ�¼�
void MainWindow::seek_bar_click(long current_time) {
    if (mpPlayerWindow != nullptr) {
        mpPlayerWindow->get_control_handler()->seek(current_time*100);
    }
	RECT client_rect;
	GetClientRect(mSeekBar, &client_rect);

	int progress_bar_width = client_rect.right - client_rect.left;
	std::string str = "seek_bar_click : " + std::to_string(current_time) + "width : " + std::to_string(progress_bar_width);
	DemoLog::log_string(CLASS_NAME, __LINE__, str.c_str());
}
//�˵���ť�����Ӧ�¼�
void  MainWindow::button_click(int button_id) {
    if (mpPlayerWindow == nullptr) {
        DemoLog::log_string(CLASS_NAME, __LINE__, "mPlayerWindow is null");
        return;
    }
    switch (button_id)
    {
	case ID_QUALITY_CHANGE_BUTTON: {
		if (IsWindowVisible(mPlayerQualityChangeListWindow))
		{

			ShowWindow(mPlayerQualityChangeListWindow, SW_HIDE);
		}
		else {

			ShowWindow(mPlayerQualityChangeListWindow, SW_SHOW);
		}
		break;
	}
    case ID_PAUSE_PLAY_BUTTON: {

		SetWindowLongPtr(mPlayButton, GWL_ID, (LONG_PTR)ID_RESUME_PLAY_BUTTON);
        SetWindowText(mPlayButton, TEXT("Resume"));
        mpPlayerWindow->get_control_handler()->pause_render();
        break;
    }
	case ID_RESUME_PLAY_BUTTON: {
        SetWindowLongPtr(mPlayButton, GWL_ID, (LONG_PTR)ID_PAUSE_PLAY_BUTTON);
		SetWindowText(mPlayButton, TEXT("Pause"));
		mpPlayerWindow->get_control_handler()->resume_render();
		break;
	}
    case ID_RESUME_BUTTON: {
        mpPlayerWindow->get_control_handler()->resume_render();
        break;
    }
    case ID_PAUSE_BUTTON:
        mpPlayerWindow->get_control_handler()->pause_render();
        break;
    case ID_STOP_BUTTON:
        mpPlayerWindow->get_control_handler()->stop();
        break;
	case ID_AUTO_DECODER_BUTTON: {
		mpPlayerWindow->get_control_handler()->set_decode_type(QMedia::QPlayerSetting::QPlayerDecoder::QPLAYER_DECODER_SETTING_AUTO);
		CurrentDataModelManager::get_instance()->set_decoder(QMedia::QPlayerSetting::QPlayerDecoder::QPLAYER_DECODER_SETTING_AUTO);
		break;
	}
	case ID_HARD_DECODER_BUTTON: {
		mpPlayerWindow->get_control_handler()->set_decode_type(QMedia::QPlayerSetting::QPlayerDecoder::QPLAYER_DECODER_SETTING_HARDWARE_PRIORITY);
		CurrentDataModelManager::get_instance()->set_decoder(QMedia::QPlayerSetting::QPlayerDecoder::QPLAYER_DECODER_SETTING_HARDWARE_PRIORITY);
		break;
	}
	case ID_SOFT_DECODER_BUTTON: {
		mpPlayerWindow->get_control_handler()->set_decode_type(QMedia::QPlayerSetting::QPlayerDecoder::QPLAYER_DECODER_SETTING_SOFT_PRIORITY);
		CurrentDataModelManager::get_instance()->set_decoder(QMedia::QPlayerSetting::QPlayerDecoder::QPLAYER_DECODER_SETTING_SOFT_PRIORITY);
		break;
	}
	case ID_MIX_DECODER_BUTTON: {
		mpPlayerWindow->get_control_handler()->set_decode_type(QMedia::QPlayerSetting::QPlayerDecoder::QPLAYER_DECODER_SETTING_FIRST_FRAME_ACCEL_PRIORITY);
		CurrentDataModelManager::get_instance()->set_decoder(QMedia::QPlayerSetting::QPlayerDecoder::QPLAYER_DECODER_SETTING_FIRST_FRAME_ACCEL_PRIORITY);
		break;
	}
	case ID_SEEK_NORMAL_BUTTON: {
		mpPlayerWindow->get_control_handler()->set_seek_mode(QMedia::QPlayerSetting::QPlayerSeek::QPLAYER_SEEK_SETTING_NORMAL);
		CurrentDataModelManager::get_instance()->set_seek_mode(QMedia::QPlayerSetting::QPlayerSeek::QPLAYER_SEEK_SETTING_NORMAL);
		break;
	}
	case ID_SEEK_ACCURATE_BUTTON:
		mpPlayerWindow->get_control_handler()->set_seek_mode(QMedia::QPlayerSetting::QPlayerSeek::QPLAYER_SEEK_SETTING_ACCURATE);
		CurrentDataModelManager::get_instance()->set_seek_mode(QMedia::QPlayerSetting::QPlayerSeek::QPLAYER_SEEK_SETTING_ACCURATE);
		break;
	case ID_SEEK_START_PLAYING_BUTTON: {
		mpPlayerWindow->get_control_handler()->set_start_action(QMedia::QPlayerSetting::QPlayerStart::QPLAYER_START_SETTING_PLAYING);
		CurrentDataModelManager::get_instance()->set_player_start(QMedia::QPlayerSetting::QPlayerStart::QPLAYER_START_SETTING_PLAYING);
		break;
	}
	case ID_SEEK_START_PAUSE_BUTTON: {
		mpPlayerWindow->get_control_handler()->set_start_action(QMedia::QPlayerSetting::QPlayerStart::QPLAYER_START_SETTING_PAUSE);
		CurrentDataModelManager::get_instance()->set_player_start(QMedia::QPlayerSetting::QPlayerStart::QPLAYER_START_SETTING_PAUSE);
		break;
	}
	case ID_AURHENTICATION_BUTTON: {
		mpPlayerWindow->get_control_handler()->force_authentication_from_network();
		CurrentDataModelManager::get_instance()->set_force_authentication_enable(true);
		break;
	}
	case ID_RENDER_RADIO_AUTO_BUTTON: {
		mpPlayerWindow->get_render_handler()->set_render_ratio(QMedia::QPlayerSetting::QPlayerRenderRatio::QPLAYER_RATIO_SETTING_AUTO);
		CurrentDataModelManager::get_instance()->set_render_ratio(QMedia::QPlayerSetting::QPlayerRenderRatio::QPLAYER_RATIO_SETTING_AUTO);
		break;
	}
	case ID_RENDER_RADIO_STRETCH_BUTTON: {
		mpPlayerWindow->get_render_handler()->set_render_ratio(QMedia::QPlayerSetting::QPlayerRenderRatio::QPLAYER_RATIO_SETTING_STRETCH);
		CurrentDataModelManager::get_instance()->set_render_ratio(QMedia::QPlayerSetting::QPlayerRenderRatio::QPLAYER_RATIO_SETTING_STRETCH);
		break;
	}
	case ID_RENDER_RADIO_FULL_SCREEN_BUTTON: {
		mpPlayerWindow->get_render_handler()->set_render_ratio(QMedia::QPlayerSetting::QPlayerRenderRatio::QPLAYER_RATIO_SETTING_FULL_SCREEN);
		CurrentDataModelManager::get_instance()->set_render_ratio(QMedia::QPlayerSetting::QPlayerRenderRatio::QPLAYER_RATIO_SETTING_FULL_SCREEN);
		break;
	}
	case ID_RENDER_RADIO_4_3_BUTTON: {
		mpPlayerWindow->get_render_handler()->set_render_ratio(QMedia::QPlayerSetting::QPlayerRenderRatio::QPLAYER_RATIO_SETTING_4_3);
		CurrentDataModelManager::get_instance()->set_render_ratio(QMedia::QPlayerSetting::QPlayerRenderRatio::QPLAYER_RATIO_SETTING_4_3);
		break;
	}
	case ID_RENDER_RADIO_16_9_BUTTON: {
		mpPlayerWindow->get_render_handler()->set_render_ratio(QMedia::QPlayerSetting::QPlayerRenderRatio::QPLAYER_RATIO_SETTING_16_9);
		CurrentDataModelManager::get_instance()->set_render_ratio(QMedia::QPlayerSetting::QPlayerRenderRatio::QPLAYER_RATIO_SETTING_16_9);
		break;
	}
	case ID_BLIND_NONE_BUTTON: {
		mpPlayerWindow->get_render_handler()->set_blind_type(QMedia::QPlayerSetting::QPlayerBlind::QPLAYER_BLIND_SETTING_NONE);
		CurrentDataModelManager::get_instance()->set_blind(QMedia::QPlayerSetting::QPlayerBlind::QPLAYER_BLIND_SETTING_NONE);
		break;
	}
	case ID_BLIND_RED_BUTTON: {
		mpPlayerWindow->get_render_handler()->set_blind_type(QMedia::QPlayerSetting::QPlayerBlind::QPLAYER_BLIND_SETTING_RED);
		CurrentDataModelManager::get_instance()->set_blind(QMedia::QPlayerSetting::QPlayerBlind::QPLAYER_BLIND_SETTING_RED);
		break;
	}
	case ID_BLIND_GREEN_BUTTON: {
		mpPlayerWindow->get_render_handler()->set_blind_type(QMedia::QPlayerSetting::QPlayerBlind::QPLAYER_BLIND_SETTING_GREEN);
		CurrentDataModelManager::get_instance()->set_blind(QMedia::QPlayerSetting::QPlayerBlind::QPLAYER_BLIND_SETTING_GREEN);
		break;
	}
	case ID_BLIND_BLUE_BUTTON: {
		mpPlayerWindow->get_render_handler()->set_blind_type(QMedia::QPlayerSetting::QPlayerBlind::QPLAYER_BLIND_SETTING_BLUE);
		CurrentDataModelManager::get_instance()->set_blind(QMedia::QPlayerSetting::QPlayerBlind::QPLAYER_BLIND_SETTING_BLUE);
		break;
	}
	case ID_SEI_CLOSE_BUTTON:
		mpPlayerWindow->get_control_handler()->set_sei_enable(false);
		CurrentDataModelManager::get_instance()->set_sei_enable(false);
		break;
	case ID_SEI_OPEN_BUTTON: {
		mpPlayerWindow->get_control_handler()->set_sei_enable(true);
		CurrentDataModelManager::get_instance()->set_sei_enable(true);
		break;
	}
	//��С������---��̨����
	case ID_BACKGROUND_OPEN_BUTTON: {
		CurrentDataModelManager::get_instance()->set_background_enable(true);
		//mpPlayerWindow->get_control_handler()->;
		break;
	}
	case ID_BACKGROUND_CLOSE_BUTTON: {
		CurrentDataModelManager::get_instance()->set_background_enable(false);
		//mpPlayerWindow->get_control_handler()->;
		break;
	}
    //�������л�
	case ID_QUALITY_CHANGE_IMMEDIATYLY_FALSE_BUTTON: {
		CurrentDataModelManager::get_instance()->set_quality_immediatyly(QualityImmediatyly::IMMEDIATYLY_FALSE);
		break;
	}
	case ID_QUALITY_CHANGE_IMMEDIATYLY_TRUE_BUTTON: {
		CurrentDataModelManager::get_instance()->set_quality_immediatyly(QualityImmediatyly::IMMEDIATYLY_TRUE);
		break;
	}
	case ID_QUALITY_CHANGE_IMMEDIATYLY_CUSTOM_BUTTON: {
		CurrentDataModelManager::get_instance()->set_quality_immediatyly(QualityImmediatyly::IMMEDIATYLY_CUSTOM);
		break;
	}
	//��Ļ����
	case ID_SUBTITLE_CLOSE_BUTTON: {
		mpPlayerWindow->get_control_handler()->set_subtitle_enable(false);
		CurrentDataModelManager::get_instance()->set_subtitle_enable(false);
		break;
	}
	case ID_SUBTITLE_CHINESE_BUTTON: {
		mpPlayerWindow->get_control_handler()->set_subtitle_enable(true);
		CurrentDataModelManager::get_instance()->set_subtitle_enable(true);
		mpPlayerWindow->get_control_handler()->set_subtitle("����");
		CurrentDataModelManager::get_instance()->set_subtitle_name("����");
		break;
	}
	case ID_SUBTITLE_ENGLISH_BUTTON: {
		mpPlayerWindow->get_control_handler()->set_subtitle_enable(true);
		CurrentDataModelManager::get_instance()->set_subtitle_enable(true);
		mpPlayerWindow->get_control_handler()->set_subtitle("Ӣ��");
		CurrentDataModelManager::get_instance()->set_subtitle_name("Ӣ��");
		break;
	}
	case ID_PLAY_SPEED_0_5_BUTTON: {
		mpPlayerWindow->get_control_handler()->set_speed(0.5);
		CurrentDataModelManager::get_instance()->set_play_speed(0.5);
		break;
	}
	case ID_PLAY_SPEED_0_7_5_BUTTON: {
		mpPlayerWindow->get_control_handler()->set_speed(0.75);
		CurrentDataModelManager::get_instance()->set_play_speed(0.75);
		break;
	}
	case ID_PLAY_SPEED_1_0_BUTTON: {
		mpPlayerWindow->get_control_handler()->set_speed(1.0);
		CurrentDataModelManager::get_instance()->set_play_speed(1.0);
		break;
	}
	case ID_PLAY_SPEED_1_2_5_BUTTON: {
		mpPlayerWindow->get_control_handler()->set_speed(1.25);
		CurrentDataModelManager::get_instance()->set_play_speed(1.25);
		break;
	}
	case ID_PLAY_SPEED_1_5_BUTTON: {
		mpPlayerWindow->get_control_handler()->set_speed(1.5);
		CurrentDataModelManager::get_instance()->set_play_speed(1.5);
		break;
	}
	case ID_PLAY_SPEED_2_0_BUTTON: {
		mpPlayerWindow->get_control_handler()->set_speed(2.0);
		CurrentDataModelManager::get_instance()->set_play_speed(2.0);
		break;
	}
	case ID_MUTE_CLOSE_BUTTON: {
		mpPlayerWindow->get_control_handler()->set_mute(false);
		CurrentDataModelManager::get_instance()->set_mute_enable(false);
		break;
	}
	case ID_MUTE_OPEN_BUTTON: {
		mpPlayerWindow->get_control_handler()->set_mute(true);
		CurrentDataModelManager::get_instance()->set_mute_enable(true);
		break;
	}
	case ID_SHOOT_IMAGE_BUTTON: {
		mpPlayerWindow->get_control_handler()->shoot_video();
		return;
	}
    default:
        break;
    }
	updata_menu_ui(button_id);
}
//���²˵���ťѡ��״̬
void MainWindow::updata_menu_ui(int button_id) {
	std::list<PlayerMenuSettingModel*>* setting_model = mpSettingMenuManager->get_menu_setting_model();

	for (int parent_index = 0; parent_index < setting_model->size(); parent_index++) {
		auto parent_it = setting_model->begin();
		std::advance(parent_it, parent_index);
		bool flag = false;
		for (int child_index = 0; child_index < (*parent_it)->get_child_menu_model()->get_child_menus_list()->size(); child_index++) {

			auto child_it = (*parent_it)->get_child_menu_model()->get_child_menus_list()->begin();
			std::advance(child_it, child_index);

			if ((*child_it)->get_id() == button_id)
			{
				for (int child_index = 0; child_index < (*parent_it)->get_child_menu_model()->get_child_menus_list()->size(); child_index++) {

					auto child_it = (*parent_it)->get_child_menu_model()->get_child_menus_list()->begin();
					std::advance(child_it, child_index);

					if ((*child_it)->get_id() == button_id)
					{
						CheckMenuItem((*parent_it)->get_child_menu_model()->get_menus(), (*child_it)->get_id(), MF_CHECKED);
					}
					else
					{
						CheckMenuItem((*parent_it)->get_child_menu_model()->get_menus(), (*child_it)->get_id(), MF_UNCHECKED);
					}
				}
				return;
			}
		}

	}
}
//ʱ��ת�ַ���
std::string  MainWindow::to_date_string(int64_t time) {
	int inner_time = time / 1000;
	std::string inner_time_str = "";
	if (inner_time < 60) {
		if (inner_time < 10)
		{
            inner_time_str = "00:0" + std::to_string(inner_time);
		}
		else
		{
            inner_time_str = "00:" + std::to_string(inner_time);
		}
	}
	else {
		int second = inner_time % 60;
		int min = inner_time / 60;
		if (min < 60)
		{
			if (min < 10)
			{
                inner_time_str = "0" + std::to_string(min) + ":";
			}
			else
			{
                inner_time_str = std::to_string(min) + ":";
			}
            if (second < 10)
            {
                inner_time_str = inner_time_str + "0" + std::to_string(second);
            } 
            else
            {
                inner_time_str += std::to_string(second);
            }
		}
		else
		{
			int hour = min / 60;
			min = min % 60;
            inner_time_str = std::to_string(hour) + ":";
			if (min < 10)
			{
				inner_time_str = "0" + std::to_string(min) + ":";
			}
			else
			{
				inner_time_str = std::to_string(min) + ":";
			}
			if (second < 10)
			{
				inner_time_str = inner_time_str + "0" + std::to_string(second);
			}
			else
			{
				inner_time_str += std::to_string(second);
			}
		}
	}
    return inner_time_str;
}
//��Ӳ���������
void MainWindow::add_listeners() {
	mpPlayerWindow->get_control_handler()->add_player_state_change_listener(this);
    mpPlayerWindow->get_render_handler()->add_render_listener(this);
    mpPlayerWindow->get_control_handler()->add_player_fps_change_listener(this);
    mpPlayerWindow->get_control_handler()->add_player_download_change_listener(this);
    mpPlayerWindow->get_control_handler()->add_player_audio_data_listener(this);
    mpPlayerWindow->get_control_handler()->add_player_audio_listener(this);
    mpPlayerWindow->get_control_handler()->add_player_authentication_listener(this);
    mpPlayerWindow->get_control_handler()->add_player_bite_rate_listener(this);
    mpPlayerWindow->get_control_handler()->add_player_buffering_change_listener(this);
    mpPlayerWindow->get_control_handler()->add_player_command_not_allow_listener(this);
    mpPlayerWindow->get_control_handler()->add_player_download_change_listener(this);
    mpPlayerWindow->get_control_handler()->add_player_format_listener(this);
    mpPlayerWindow->get_control_handler()->add_player_media_network_listener(this);
    mpPlayerWindow->get_control_handler()->add_player_progress_change_listener(this);
    mpPlayerWindow->get_control_handler()->add_player_quality_listener(this);
    mpPlayerWindow->get_control_handler()->add_player_seek_listener(this);
    mpPlayerWindow->get_control_handler()->add_player_sei_data_listener(this);
    mpPlayerWindow->get_control_handler()->add_player_shoot_video_listener(this);
    mpPlayerWindow->get_control_handler()->add_player_speed_change_listener(this);
    mpPlayerWindow->get_control_handler()->add_player_subtitle_listener(this);
    mpPlayerWindow->get_control_handler()->add_player_video_data_listener(this);
    mpPlayerWindow->get_control_handler()->add_player_video_decode_type_listener(this);
    mpPlayerWindow->get_control_handler()->add_player_video_frame_size_change_listener(this);
    
}


/***********************qplayer listeners***********************/
void MainWindow::on_state_changed(QMedia::QPlayerState state) {
	DemoLog::log_string(CLASS_NAME, __LINE__, "QPlayerState");
    CurrentDataModelManager::get_instance()->set_player_state(state);
    std::string state_string = "player state: ";
    switch (state)
    {
    case QMedia::QPlayerState::NONE:
        state_string += "none";
        break;
	case QMedia::QPlayerState::INIT:
		state_string += "init";
        break;
	case QMedia::QPlayerState::PREPARE:
		state_string += "prepare";
        break;
	case QMedia::QPlayerState::PLAYING:
		CurrentDataModelManager::get_instance()->set_is_seeking(false);
		state_string += "playing";
		CurrentDataModelManager::get_instance()->set_force_authentication_enable(false);
		SetWindowLongPtr(mPlayButton, GWL_ID, (LONG_PTR)ID_PAUSE_PLAY_BUTTON);
		SetWindowText(mPlayButton, TEXT("Pause"));
        break;
	case QMedia::QPlayerState::PAUSED_RENDER:
		state_string += "paused_render";
		SetWindowLongPtr(mPlayButton, GWL_ID, (LONG_PTR)ID_RESUME_PLAY_BUTTON);
		SetWindowText(mPlayButton, TEXT("Resume"));
        break;
	case QMedia::QPlayerState::COMPLETED:
		state_string += "completed";
		SetWindowLongPtr(mPlayButton, GWL_ID, (LONG_PTR)ID_RESUME_PLAY_BUTTON);
		SetWindowText(mPlayButton, TEXT("Resume"));
        break;
	case QMedia::QPlayerState::SEEKING:
		state_string += "seeking";
        break;
	case QMedia::QPlayerState::STOPPED:
		state_string += "stopped";
		SetWindowLongPtr(mPlayButton, GWL_ID, (LONG_PTR)ID_PAUSE_PLAY_BUTTON);
		SetWindowText(mPlayButton, TEXT("Play"));
        break;
	case QMedia::QPlayerState::MISTAKE:
		state_string += "mistake";
        break;
	case QMedia::QPlayerState::END:
		state_string += "end";
        break;
	case QMedia::QPlayerState::MEDIA_ITEM_PREPARE:
		state_string += "media_item_prepare";
        break;
	case QMedia::QPlayerState::RELEASE:
		state_string += "release";
        break;
	default:
		state_string += "state is invalid";
        break;
    }
	SetWindowText(mPlayerStateText, state_string.c_str());
	mpToastWindow->add_item(state_string);

}

void MainWindow::on_first_frame_rendered(int64_t elapsed_time) {
    DemoLog::log_string(CLASS_NAME, __LINE__, "elapsed_time");
    std::string text = "first frame: " + std::to_string(elapsed_time) + "ms";
    
	SetWindowText(mFirstFrameText, text.c_str());
	mpToastWindow->add_item(text);
	update_quality_list_window();
}
void MainWindow::on_fps_changed(long fps) {

    std::string text = "FPS: " + std::to_string(fps);
	SetWindowText(mFPSText, text.c_str());
}

void  MainWindow::on_audio_data(int sample_rate, QMedia::QSampleFormat format, int channel_num, QMedia::QChannelLayout channel_layout, uint8_t* audio_data, uint64_t size) {

}

void  MainWindow::on_mute_changed(bool is_mute) {
    std::string text = "";
    if (is_mute)
    {
        text = "��������";
    }
    else
    {
        text = "�رվ���";
    }
	
	mpToastWindow->add_item(text);
}

void  MainWindow::on_authentication_failed(QMedia::QAuthenticationErrorType error) {
	std::string text = "��Ȩ��";
    switch (error)
    {
    case QMedia::QAuthenticationErrorType::NONE:
        text += "NONE";
        break;
    case QMedia::QAuthenticationErrorType::NO_BASE_AUTH:
        text += "NO_BASE_AUTH";
        break;
	case QMedia::QAuthenticationErrorType::NO_VR_AUTH:
		text += "NO_VR_AUTH";
        break;
	case QMedia::QAuthenticationErrorType::NO_BLIND_AUTH:
		text += "NO_BLIND_AUTH";
        break;
	case QMedia::QAuthenticationErrorType::NO_SEI_AUTH:
		text += "NO_SEI_AUTH";
        break;
	case QMedia::QAuthenticationErrorType::NO_SRT_AUTH:
		text += "NO_SRT_AUTH";
        break;
    default:
        break;
	}
	mpToastWindow->add_item(text);
}

void  MainWindow::on_authentication_success() {
    std::string text = "��Ȩ���ɹ�";
	mpToastWindow->add_item(text);
}

void  MainWindow::on_bite_rate_changed(int bit_rate) {

	std::string text = "biterate: " + std::to_string(bit_rate/1000) + "kbps";
	SetWindowText(mPlayerBiteRateText, text.c_str());
}

void  MainWindow::on_buffering_start() {
	std::string text = "buffering start";
	mpToastWindow->add_item(text);
}

void  MainWindow::on_buffering_end() {
	std::string text = "buffering end";
	mpToastWindow->add_item(text);
}

void  MainWindow::on_command_not_allow(const std::string& command_name, QMedia::QPlayerState state) {
	std::string text = "on_command_not_allow";
	mpToastWindow->add_item(text);
}

void  MainWindow::on_download_changed(int64_t download_speed, int64_t buffer_pos) {
	std::ostringstream oss;
	oss << std::fixed << std::setprecision(2) << download_speed / 1000.0;
	std::string result_str = oss.str();
	std::string text = "download speed: " + result_str + "kb/s";
    SetWindowText(mPlayerDownLoadText, text.c_str());

}

void  MainWindow::on_format_not_support() {
	std::string text = "on_format_not_support";
	mpToastWindow->add_item(text);
}

void  MainWindow::on_reconnect_start(const std::string& user_type, QMedia::QUrlType url_type, const std::string& url, int retry_time) {
	std::string text = "on_reconnect_start : " + std::to_string(retry_time);
	mpToastWindow->add_item(text);
}

void  MainWindow::on_reconnect_end(const std::string& user_type, QMedia::QUrlType url_type, const std::string& url, int retry_time, QMedia::QOpenError error) {
	std::string text = "on_reconnect_end : " + std::to_string(retry_time) + " open error : " + std::to_string((int)error);
	mpToastWindow->add_item(text);
}

void  MainWindow::on_open_failed(const std::string& user_type, QMedia::QUrlType url_type, const std::string& url, QMedia::QOpenError error) {
	std::string text = "on_open_failed : " + std::to_string((int)error);
	mpToastWindow->add_item(text);
}

void  MainWindow::on_progress_changed(int64_t progress, int64_t duration) {
    if (CurrentDataModelManager::get_instance()->get_duration_time() != duration/100)
	{
        CurrentDataModelManager::get_instance()->set_duration_time(duration / 100);
        SendMessage(mSeekBar, TBM_SETRANGE, TRUE, MAKELONG(0, CurrentDataModelManager::get_instance()->get_duration_time()));
    } 
    std::string progress_time_str = to_date_string(progress);
    std::string duraion_time_str = to_date_string(duration);
	std::string text = progress_time_str + "/" + duraion_time_str;
    if (!CurrentDataModelManager::get_instance()->get_is_seeking())
	{
		SendMessage(mSeekBar, TBM_SETPOS, TRUE, progress / 100);
    }
    if (mProgressTimeStr == progress_time_str) {
        return;
    }
	SetWindowText(mPlayerProgressAndDurationText, text.c_str());
    mProgressTimeStr = progress_time_str;
}

void  MainWindow::on_quality_switch_start(const std::string &user_type, QMedia::QUrlType url_type, int old_quality, int new_quality) {
	std::string text = "on_quality_switch_start : " + std::to_string(new_quality);
	mpToastWindow->add_item(text);
}

void  MainWindow::on_quality_switch_complete(const std::string &user_type, QMedia::QUrlType url_type, int old_quality, int new_quality) {
	std::string text = "on_quality_switch_complete : " + std::to_string(new_quality);
	mpToastWindow->add_item(text);
	std::string title = "�����ȣ�" + std::to_string(new_quality);
	SetWindowText(mPlayQualityButton, TEXT(title.c_str()));
	CurrentDataModelManager::get_instance()->set_quality(new_quality);
}

void  MainWindow::on_quality_switch_canceled(const std::string& user_type, QMedia::QUrlType url_type, int old_quality, int new_quality) {
	std::string text = "on_quality_switch_canceled new : " + std::to_string(new_quality) + "old : " + std::to_string(old_quality);
	mpToastWindow->add_item(text);
}

void  MainWindow::on_quality_switch_failed(const std::string& user_type, QMedia::QUrlType url_type, int old_quality, int new_quality) {
	std::string text = "on_quality_switch_canceled new : " + std::to_string(new_quality) + "old : " + std::to_string(old_quality);
	mpToastWindow->add_item(text);
}

void  MainWindow::on_quality_switch_retry_later(const std::string& user_type, QMedia::QUrlType url_type) {
	std::string text = "on_quality_switch_retry_later";
	mpToastWindow->add_item(text);
}

void  MainWindow::on_seek_success() {
	std::string text = "on_seek_success";
	mpToastWindow->add_item(text);
	CurrentDataModelManager::get_instance()->set_is_seeking(false);
}

void  MainWindow::on_seek_failed() {
	std::string text = "on_seek_failed";
	mpToastWindow->add_item(text);
    CurrentDataModelManager::get_instance()->set_is_seeking(false);
}

void  MainWindow::on_shoot_video_success(uint8_t* image_data, uint64_t size, int width, int height, QMedia::QShootVideoType type) {
	std::string text = "on_shoot_video_success";
	mpToastWindow->add_item(text);
}

void  MainWindow::on_shoot_video_failed() {
	std::string text = "on_shoot_video_failed";
	mpToastWindow->add_item(text);
}

void  MainWindow::on_sei_data(uint8_t* data, uint64_t size) {
	std::vector<char> buffer(data, data + size);  // �����ݸ��Ƶ�������

	// ת��Ϊ std::string
	std::string result(buffer.data(), buffer.data() + buffer.size());
	std::string text = "on_sei_data: " + result;
	mpToastWindow->add_item(text);
}

void  MainWindow::on_speed_changed(float speed) {
	std::string text = "on_speed_changed: " + std::to_string(speed);
	mpToastWindow->add_item(text);
}

void  MainWindow::on_subtitle_text_changed(const std::string& text) {
    std::string toast_text = "on_subtitle_text_changed: " + text;
    mpToastWindow->add_item(toast_text);
}

void  MainWindow::on_subtitle_name_changed(const std::string& name) {
	std::string text = "on_subtitle_name_changed: " + name;
	mpToastWindow->add_item(text);
}

void  MainWindow::on_subtitle_enable(bool enable) {
	std::string text = "on_subtitle_enable: ";
    if (enable)
    {
        text += "����";
    }
    else
    {
        text += "�ر�";
    }
	mpToastWindow->add_item(text);
}

void  MainWindow::on_subtitle_loaded(const std::string& name, bool result) {
	std::string text = "on_subtitle_loaded: " + name + "result : ";
	if (result)
	{
		text += "�ɹ�";
	}
	else
	{
		text += "ʧ��";
	}
	mpToastWindow->add_item(text);
}

void  MainWindow::on_subtitle_decoded(const std::string& name, bool result) {
	std::string text = "on_subtitle_decoded: " + name + "result : ";
	if (result)
	{
		text += "�ɹ�";
	}
	else
	{
		text += "ʧ��";
	}
	mpToastWindow->add_item(text);
}

void  MainWindow::on_video_data(int width, int height, QMedia::QVideoType video_type, uint8_t* buffer, uint64_t size) {

}

void  MainWindow::on_video_decode_by_type(QMedia::QDecoderType type) {
	std::string text = "on_video_decode_by_type: " + std::to_string((int)type);
	mpToastWindow->add_item(text);
}

void  MainWindow::on_not_support_codec_format(int codec_id) {
	std::string text = "on_not_support_codec_format: " + std::to_string(codec_id);
	mpToastWindow->add_item(text);
}

void  MainWindow::on_video_frame_size_changed(int width, int height) {
    std::string text = "on_video_frame_size_changed width: " + std::to_string(width) + " height : " + std::to_string(height);
	mpToastWindow->add_item(text);
}