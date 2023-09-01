#include "PlayerMenuSettingModelManager.h"
#include "DemoLog.h"


#define TAG          "PlayerMenuSettingModelManager"

PlayerMenuSettingModelManager::PlayerMenuSettingModelManager(HWND hwnd)
{
	mHwnd = hwnd;
	mpMenuSettingModels = new std::list<PlayerMenuSettingModel*>();

	add_setting_model("���ſ���", ID_PLAY_CHONTROL);
	add_setting_child_model(create_play_control_list(), ID_PLAY_CHONTROL);

	add_setting_model("���뷽ʽ", ID_DECODER);
	add_setting_child_model(create_decoder_list(), ID_DECODER);

	add_setting_model("seek��ʽ", ID_SEEK);
	add_setting_child_model(create_seek_list(), ID_SEEK);

	add_setting_model("�𲥷�ʽ", ID_SEEK_START);
	add_setting_child_model(create_start_seek_list(), ID_SEEK_START);

	add_setting_model("��Ȩ��ʽ", ID_AURHENTICATION);
	add_setting_child_model(create_authentication_list(), ID_AURHENTICATION);

	add_setting_model("��Ⱦ����", ID_RENDER_RADIO);
	add_setting_child_model(create_render_radio_list(), ID_RENDER_RADIO);

	add_setting_model("ɫ���仯", ID_BLIND);
	add_setting_child_model(create_blind_list(), ID_BLIND);

	add_setting_model("SEI�ص�", ID_SEI);
	add_setting_child_model(create_sei_list(), ID_SEI);

	add_setting_model("��̨����", ID_BACKGROUND);
	add_setting_child_model(create_background_play_list(), ID_BACKGROUND);

	add_setting_model("�������л�", ID_QUALITY_CHANGE);
	add_setting_child_model(create_quality_change_list(), ID_QUALITY_CHANGE);

	add_setting_model("��Ļ����", ID_SUBTITLE);
	add_setting_child_model(create_subtitle_list(), ID_SUBTITLE);
}



PlayerMenuSettingModelManager::~PlayerMenuSettingModelManager()
{
	if (mpMenuSettingModels != nullptr && !mpMenuSettingModels->empty())
	{
		for (int i = 0; i < mpMenuSettingModels->size(); i++)
		{
			auto it = mpMenuSettingModels->begin();
			std::advance(it, i);
			delete (*it);
			*it = nullptr;
		}
	}
}

void PlayerMenuSettingModelManager::add_setting_model(std::string menu_name, int id) {
	PlayerMenuSettingModel* inner_model = new PlayerMenuSettingModel();
	inner_model->id = id;
	inner_model->mName = menu_name;
	//HMENU play_menu = CreatePopupMenu();
	//inner_model->mHmenu = play_menu;
	mpMenuSettingModels->emplace_back(inner_model);
}

void PlayerMenuSettingModelManager::add_setting_child_model(std::list<ChildMenu*>* child_menu, int parent_menu_id) {
	PlayerMenuSettingModel* inner_model = nullptr;
	for (int index = 0; index < mpMenuSettingModels->size();index++)
	{
		auto it = mpMenuSettingModels->begin();
		std::advance(it, index);
		if ((*it)->id == parent_menu_id)
		{
			inner_model = *it;
			break;
		}
	}
	if (inner_model == nullptr)
	{
		DemoLog::log_string(TAG, __LINE__, "add_setting_child_model :inner_model is nil");
		return;
	}
	HMENU child_menu_hmenu = CreatePopupMenu();

	PlayerChildMenuModel* child_menu_model = new PlayerChildMenuModel();
	child_menu_model->mHmenu = child_menu_hmenu;
	child_menu_model->mpMenus = std::move(child_menu);
	inner_model->mpChildMenu = child_menu_model;
	

}


std::list<ChildMenu*>* PlayerMenuSettingModelManager::create_play_control_list() {

	std::list<ChildMenu*>* play_control = new std::list<ChildMenu*>;
	
	play_control->emplace_back(create_child_menu("�ָ�����", ID_RESUME_BUTTON, false));
	play_control->emplace_back(create_child_menu("��ͣ����", ID_PAUSE_BUTTON, false));
	play_control->emplace_back(create_child_menu("ֹͣ����", ID_STOP_BUTTON, false));

	return play_control;
}

std::list<ChildMenu*>* PlayerMenuSettingModelManager::create_decoder_list() {
	std::list<ChildMenu*>* decoder_list = new std::list<ChildMenu*>;

	decoder_list->emplace_back(create_child_menu("�Զ�", ID_AUTO_DECODER_BUTTON, true));
	decoder_list->emplace_back(create_child_menu("���", ID_SOFT_DECODER_BUTTON, false));
	decoder_list->emplace_back(create_child_menu("Ӳ��", ID_HARD_DECODER_BUTTON, false));

	return decoder_list;
}

std::list<ChildMenu*>* PlayerMenuSettingModelManager::create_seek_list() {
	std::list<ChildMenu*>* seek_list = new std::list<ChildMenu*>;

	seek_list->emplace_back(create_child_menu("�ؼ�֡seek", ID_SEEK_NORMAL_BUTTON, true));
	seek_list->emplace_back(create_child_menu("��׼seek", ID_SEEK_ACCURATE_BUTTON, false));

	return seek_list;
}

std::list<ChildMenu*>* PlayerMenuSettingModelManager::create_start_seek_list() {
	std::list<ChildMenu*>* start_seek_list = new std::list<ChildMenu*>;

	start_seek_list->emplace_back(create_child_menu("�𲥲���", ID_SEEK_START_PLAYING_BUTTON, true));
	start_seek_list->emplace_back(create_child_menu("����ͣ", ID_SEEK_START_PAUSE_BUTTON, false));

	return start_seek_list;
}

std::list<ChildMenu*>* PlayerMenuSettingModelManager::create_authentication_list() {
	std::list<ChildMenu*>* authentication_list = new std::list<ChildMenu*>;

	authentication_list->emplace_back(create_child_menu("��һ��ˢ�¼�Ȩ��Ϣ", ID_AURHENTICATION_BUTTON, true));

	return authentication_list;
}

std::list<ChildMenu*>* PlayerMenuSettingModelManager::create_render_radio_list() {
	std::list<ChildMenu*>* render_radio_list = new std::list<ChildMenu*>;

	render_radio_list->emplace_back(create_child_menu("�Զ�", ID_RENDER_RADIO_AUTO_BUTTON, true));
	render_radio_list->emplace_back(create_child_menu("����", ID_RENDER_RADIO_STRETCH_BUTTON, false));
	render_radio_list->emplace_back(create_child_menu("����", ID_RENDER_RADIO_FULL_SCREEN_BUTTON, false));
	render_radio_list->emplace_back(create_child_menu("16:9", ID_RENDER_RADIO_16_9_BUTTON, false));
	render_radio_list->emplace_back(create_child_menu("4:3", ID_RENDER_RADIO_4_3_BUTTON, false));

	return render_radio_list;
}

std::list<ChildMenu*>* PlayerMenuSettingModelManager::create_blind_list() {
	std::list<ChildMenu*>* blind_list = new std::list<ChildMenu*>;

	blind_list->emplace_back(create_child_menu("���˾�", ID_BLIND_NONE_BUTTON, true));
	blind_list->emplace_back(create_child_menu("��/���˾�", ID_BLIND_RED_BUTTON, false));
	blind_list->emplace_back(create_child_menu("��/���˾�", ID_BLIND_GREEN_BUTTON, false));
	blind_list->emplace_back(create_child_menu("��/���˾�", ID_BLIND_BLUE_BUTTON, false));

	return blind_list;
}

std::list<ChildMenu*>* PlayerMenuSettingModelManager::create_sei_list() {
	std::list<ChildMenu*>* sei_list = new std::list<ChildMenu*>;

	sei_list->emplace_back(create_child_menu("����SEI�ص�", ID_SEI_OPEN_BUTTON, true));
	sei_list->emplace_back(create_child_menu("�ر�SEI�ص�", ID_SEI_CLOSE_BUTTON, false));

	return sei_list;
}

std::list<ChildMenu*>* PlayerMenuSettingModelManager::create_background_play_list() {
	std::list<ChildMenu*>* background_play_list = new std::list<ChildMenu*>;

	background_play_list->emplace_back(create_child_menu("������̨����", ID_BACKGROUND_OPEN_BUTTON, true));
	background_play_list->emplace_back(create_child_menu("�رպ�̨����", ID_BACKGROUND_CLOSE_BUTTON, false));

	return background_play_list;
}

std::list<ChildMenu*>* PlayerMenuSettingModelManager::create_quality_change_list() {
	std::list<ChildMenu*>* quality_change_list = new std::list<ChildMenu*>;

	quality_change_list->emplace_back(create_child_menu("�����л�", ID_QUALITY_CHANGE_IMMEDIATYLY_TRUE_BUTTON, true));
	quality_change_list->emplace_back(create_child_menu("�޷��л���ֻʹ�õ㲥��", ID_QUALITY_CHANGE_IMMEDIATYLY_FALSE_BUTTON, false));
	quality_change_list->emplace_back(create_child_menu("ֱ������&�㲥�޷�", ID_QUALITY_CHANGE_IMMEDIATYLY_CUSTOM_BUTTON, false));

	return quality_change_list;
}

std::list<ChildMenu*>* PlayerMenuSettingModelManager::create_subtitle_list() {
	std::list<ChildMenu*>* subtitle_list = new std::list<ChildMenu*>;

	subtitle_list->emplace_back(create_child_menu("�ر�", ID_SUBTITLE_CLOSE_BUTTON, true));
	subtitle_list->emplace_back(create_child_menu("����", ID_SUBTITLE_CHINESE_BUTTON, false));
	subtitle_list->emplace_back(create_child_menu("Ӣ��", ID_SUBTITLE_ENGLISH_BUTTON, false));

	return subtitle_list;
}

ChildMenu* PlayerMenuSettingModelManager::create_child_menu(std::string name, int id, bool is_selected) {
	ChildMenu* inner_child_menu = new ChildMenu();
	inner_child_menu->mId = id;
	inner_child_menu->mIsSelected = is_selected;
	inner_child_menu->mName = name;
	return inner_child_menu;
}


std::list<PlayerMenuSettingModel*>* PlayerMenuSettingModelManager::get_menu_setting_model() {
	return mpMenuSettingModels;
}