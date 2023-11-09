#include "CurrentDataModelManager.h"
#include "../FileOfWriteAndRead.h"
#include <iostream>
#include <sstream>
#include <iomanip>

#define SETTING_LOCAL_FILE_NAME "Setting.txt"
CurrentDataModelManager* CurrentDataModelManager::instance = NULL;
CurrentDataModelManager* CurrentDataModelManager::get_instance() {
	if (instance == nullptr)
	{
		instance = new CurrentDataModelManager;
	}
	return instance;
}

CurrentDataModelManager::CurrentDataModelManager()
{
	mpCurrentDataModel = FileOfWriteAndRead::read_setting_local_file(SETTING_LOCAL_FILE_NAME);
}

CurrentDataModelManager::~CurrentDataModelManager()
{
	if (mpCurrentDataModel != nullptr) {
		delete mpCurrentDataModel;
		mpCurrentDataModel = nullptr;
	}
}

void CurrentDataModelManager::set_duration_time(long time) {
	mpCurrentDataModel->set_duration_time(time);
}

long CurrentDataModelManager::get_duration_time() {
	return mpCurrentDataModel->get_duration_time();
}
void CurrentDataModelManager::set_player_state(QMedia::QPlayerState state) {
	mpCurrentDataModel->set_player_state(state);
}

QMedia::QPlayerState CurrentDataModelManager::get_player_state() {
	return mpCurrentDataModel->get_player_state();
}

void  CurrentDataModelManager::set_first_frame_time(long  elapsed_time) {
	mpCurrentDataModel->set_first_frame_time(elapsed_time);
}

long CurrentDataModelManager::get_first_frame_time() {
	return mpCurrentDataModel->get_first_frame_time();
}
void CurrentDataModelManager::set_is_seeking(bool is_seeking) {
	mpCurrentDataModel->set_is_seeking(is_seeking);
}

bool CurrentDataModelManager::get_is_seeking() {
	return mpCurrentDataModel->get_is_seeking();
}

void CurrentDataModelManager::set_decoder(QMedia::QPlayerSetting::QPlayerDecoder decoder) {
	mpCurrentDataModel->set_decoder(decoder);
	FileOfWriteAndRead::motify_setting_local_file(SETTING_LOCAL_FILE_NAME, "decoder", std::to_string((int)decoder));
}

QMedia::QPlayerSetting::QPlayerDecoder CurrentDataModelManager::get_decoder() {
	return mpCurrentDataModel->get_decoder();
}

void CurrentDataModelManager::set_seek_mode(QMedia::QPlayerSetting::QPlayerSeek seek_mode) {
	mpCurrentDataModel->set_seek_mode(seek_mode);
	FileOfWriteAndRead::motify_setting_local_file(SETTING_LOCAL_FILE_NAME, "seekMode", std::to_string((int)seek_mode));
}

QMedia::QPlayerSetting::QPlayerSeek CurrentDataModelManager::get_seek_mode() {
	return mpCurrentDataModel->get_seek_mode();
}

void CurrentDataModelManager::set_player_start(QMedia::QPlayerSetting::QPlayerStart player_start) {
	mpCurrentDataModel->set_player_start(player_start);
	FileOfWriteAndRead::motify_setting_local_file(SETTING_LOCAL_FILE_NAME, "playerStart", std::to_string((int)player_start));
}

QMedia::QPlayerSetting::QPlayerStart CurrentDataModelManager::get_player_start() {
	return mpCurrentDataModel->get_player_start();
}

void CurrentDataModelManager::set_render_ratio(QMedia::QPlayerSetting::QPlayerRenderRatio render_ratio) {
	mpCurrentDataModel->set_render_ratio(render_ratio);
	FileOfWriteAndRead::motify_setting_local_file(SETTING_LOCAL_FILE_NAME, "renderRatio", std::to_string((int)render_ratio));
}

QMedia::QPlayerSetting::QPlayerRenderRatio CurrentDataModelManager::get_render_ratio() {
	return mpCurrentDataModel->get_render_ratio();
}

void CurrentDataModelManager::set_blind(QMedia::QPlayerSetting::QPlayerBlind blind) {
	mpCurrentDataModel->set_blind(blind);
	FileOfWriteAndRead::motify_setting_local_file(SETTING_LOCAL_FILE_NAME, "blind", std::to_string((int)blind));
}

QMedia::QPlayerSetting::QPlayerBlind CurrentDataModelManager::get_blind() {
	return mpCurrentDataModel->get_blind();
}

void CurrentDataModelManager::set_sei_enable(bool sei_enable) {
	mpCurrentDataModel->set_sei_enable(sei_enable);
	FileOfWriteAndRead::motify_setting_local_file(SETTING_LOCAL_FILE_NAME, "seiEnable", std::to_string(sei_enable));
}

bool CurrentDataModelManager::get_sei_enable() {
	return mpCurrentDataModel->get_sei_enable();
}

void CurrentDataModelManager::set_background_enable(bool background_enable) {
	mpCurrentDataModel->set_background_enable(background_enable);
	FileOfWriteAndRead::motify_setting_local_file(SETTING_LOCAL_FILE_NAME, "backgroundEnable", std::to_string(background_enable));
}

bool CurrentDataModelManager::get_background_enable() {
	return mpCurrentDataModel->get_background_enable();
}

void CurrentDataModelManager::set_quality_immediatyly(QualityImmediatyly quality_immediatyly) {
	mpCurrentDataModel->set_quality_immediatyly(quality_immediatyly);
	FileOfWriteAndRead::motify_setting_local_file(SETTING_LOCAL_FILE_NAME, "qualityImmediatyly", std::to_string(quality_immediatyly));
}

QualityImmediatyly CurrentDataModelManager::get_quality_immediatyly() {
	return mpCurrentDataModel->get_quality_immediatyly();
}

void CurrentDataModelManager::set_subtitle_enable(bool subtitle_enable) {
	mpCurrentDataModel->set_subtitle_enable(subtitle_enable);
	FileOfWriteAndRead::motify_setting_local_file(SETTING_LOCAL_FILE_NAME, "subtitleEnable", std::to_string(subtitle_enable));
}

bool CurrentDataModelManager::get_subtitle_enable() {
	return mpCurrentDataModel->get_subtitle_enable();
}

void CurrentDataModelManager::set_subtitle_name(const std::string& subtitle_name) {
	mpCurrentDataModel->set_subtitle_name(subtitle_name);
	FileOfWriteAndRead::motify_setting_local_file(SETTING_LOCAL_FILE_NAME, "subtitleName", subtitle_name);
}

std::string CurrentDataModelManager::get_subtitle_name() {
	return mpCurrentDataModel->get_subtitle_name();
}

void CurrentDataModelManager::set_play_speed(float play_speed) {
	mpCurrentDataModel->set_play_speed(play_speed);
	std::stringstream stream;
	stream << std::fixed << std::setprecision(2) << play_speed;
	FileOfWriteAndRead::motify_setting_local_file(SETTING_LOCAL_FILE_NAME, "playSpeed", stream.str());
}

float CurrentDataModelManager::get_play_speed() {
	return mpCurrentDataModel->get_play_speed();
}

void CurrentDataModelManager::set_mute_enable(bool mute_enable) {
	mpCurrentDataModel->set_mute_enable(mute_enable);
	FileOfWriteAndRead::motify_setting_local_file(SETTING_LOCAL_FILE_NAME, "muteEnable", std::to_string(mute_enable));
}

bool CurrentDataModelManager::get_mute_enable() {
	return mpCurrentDataModel->get_mute_enable();
}

void CurrentDataModelManager::set_player_start_position(long player_start_position) {
	mpCurrentDataModel->set_player_start_position(player_start_position);
	FileOfWriteAndRead::motify_setting_local_file(SETTING_LOCAL_FILE_NAME, "playStartPositon", std::to_string(player_start_position));
}

long CurrentDataModelManager::get_player_start_position() {
	return mpCurrentDataModel->get_player_start_position();
}

void CurrentDataModelManager::set_force_authentication_enable(bool force_authentication_enable) {
	mpCurrentDataModel->set_force_authentication_enable(force_authentication_enable);
	FileOfWriteAndRead::motify_setting_local_file(SETTING_LOCAL_FILE_NAME, "forceAuthenticationEnable", std::to_string(force_authentication_enable));
}

bool CurrentDataModelManager::get_force_authentication_enable() {
	return mpCurrentDataModel->get_force_authentication_enable();
}