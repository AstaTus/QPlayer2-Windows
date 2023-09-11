#pragma once
#include "framework.h"
#include "QIPlayerListenersHeader.h"
#include "toast2.h"
namespace QMedia {
    class QIPlayerContext;
    class QMediaModel;
}
class VideoRenderWindow;
class PlayerWindow;
class UrlListWindow;
class CurrentDataModelManager;
class MainWindow :
	public QMedia::QIPlayerStateChangeListener,
	QMedia::QIPlayerRenderListener,
	QMedia::QIPlayerFPSListener,
	QMedia::QIPlayerAudioDataListener,
	QMedia::QIPlayerAudioListener,
	QMedia::QIPlayerAuthenticationListener,
	QMedia::QIPlayerBiteRateListener,
	QMedia::QIPlayerBufferingListener,
	QMedia::QIPlayerCommandNotAllowListener,
	QMedia::QIPlayerDownloadListener,
	QMedia::QIPlayerFormatListener,
	QMedia::QIPlayerMediaNetworkListener,
	QMedia::QIPlayerProgressListener,
	QMedia::QIPlayerQualityListener,
	QMedia::QIPlayerSeekListener,
	QMedia::QIPlayerShootVideoListener,
	QMedia::QIPlayerSEIDataListener,
	QMedia::QIPlayerSpeedListener,
	QMedia::QIPlayerSubtitleListener,
	QMedia::QIPlayerVideoDataListener,
	QMedia::QIPlayerVideoDecodeListender,
	QMedia::QIPlayerVideoFrameSizeChangeListener
{

private:
    static LRESULT CALLBACK main_window_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static BOOL CALLBACK resize_child_windows_proc(HWND hwndChild, LPARAM lParam);
   
    void url_Click_call_back(HWND hwnd, QMedia::QMediaModel* model);

	void on_state_changed(QMedia::QPlayerState state) override;

    void on_first_frame_rendered(long elapsed_time) override;

    void on_fps_changed(long fps) override;

	void on_audio_data(int sample_rate, QMedia::QSampleFormat format, int channel_num, QMedia::QChannelLayout channel_layout, uint8_t* audio_data, uint64_t size) override;

	void on_mute_changed(bool is_mute) override;

	 void on_authentication_failed(QMedia::QAuthenticationErrorType error) override;

	 void on_authentication_success() override;

	 void on_bite_rate_changed(int bit_rate) override;

	 void on_buffering_start() override;

	 void on_buffering_end() override;

	 void on_command_not_allow(std::string command_name, QMedia::QPlayerState state) override;

	 void on_download_changed(int download_speed, int buffer_pos) override;

	 void on_format_not_support() override;

	 void on_reconnect_start(std::string user_type, QMedia::QUrlType url_type, std::string url, int retry_time) override;

	 void on_reconnect_end(std::string user_type, QMedia::QUrlType url_type, std::string url, int retry_time, QMedia::QOpenError error) override;

	 void on_open_failed(std::string user_type, QMedia::QUrlType url_type, std::string url, QMedia::QOpenError error) override;

	 void on_progress_changed(long progress, long duration) override;

	 void on_quality_switch_start(std::string user_type, QMedia::QUrlType url_type, int old_quality, int new_quality) override;

	 void on_quality_switch_complete(std::string user_type, QMedia::QUrlType url_type, int old_quality, int new_quality) override;

	 void on_quality_switch_canceled(std::string user_type, QMedia::QUrlType url_type, int old_quality, int new_quality) override;

	 void on_quality_switch_failed(std::string user_type, QMedia::QUrlType url_type, int old_quality, int new_quality) override;

	 void on_quality_switch_retry_later(std::string user_type, QMedia::QUrlType url_type) override;
	
	 void on_seek_success() override;

	 void on_seek_failed() override;

	 void on_shoot_video_success(uint8_t* image_data, uint64_t size, int width, int height, QMedia::QShootVideoType type) override;

	 void on_shoot_video_failed() override;

	 void on_sei_data(uint8_t* data, uint64_t size) override;

	 void on_speed_changed(int speed) override;

	 void on_subtitle_text_changed(std::string text) override;

	 void on_subtitle_name_changed(std::string name) override;

	 void on_subtitle_enable(bool enable) override;

	 void on_subtitle_loaded(std::string name, bool result) override;

	 void on_subtitle_decoded(std::string name, bool result) override;

	 void on_video_data(int width, int height, QMedia::QVideoType video_type, uint8_t* buffer, uint64_t size) override;

	 void on_video_decode_by_type(QMedia::QDecoderType type) override;

	 void on_not_support_codec_format(int codec_id) override;

	 void on_video_frame_size_changed(int width, int height) override;

	void add_listeners();
public:
    MainWindow(HINSTANCE hInstance, int nCmdShow);

    ~MainWindow();

    MainWindow(const MainWindow&) = delete;

    MainWindow(MainWindow&&) = delete;

    MainWindow& operator=(const MainWindow&) = delete;

    MainWindow& operator=(MainWindow&&) = delete;

    HWND get_hwnd();
private:
    LRESULT on_create();
    LRESULT on_resize();
    LRESULT on_create_play_menu();
     
    //HWND createVideoRenderWindow(HWND parent_hwnd);

    static int get_render_window_height(int parent_window_height);
    static int get_render_window_width(int parent_window_width);
    void button_click(int button_id);


	void seek_bar_click(long current_time);


	std::string to_date_string(long time);

    bool notify_resize_to_player(LPRECT proot_window_rect);
private:

    HWND mHwnd;
    HINSTANCE mHinstance;


	std::string mProgressTimeStr;


    CurrentDataModelManager* mpCurrentDataModelManger;

    static const int MAX_LOADSTRING = 100;
    WCHAR mTitle[MAX_LOADSTRING];                  // �������ı�
    WCHAR mWindowClass[MAX_LOADSTRING];            // ����������


	PlayerWindow* mpPlayerWindow;

	UrlListWindow* mUrlListWindow;

	toast2* mToastWindow;

	HWND mPlayButton;

	HWND mSeekBar;

	HWND mFirstFrameText;

	HWND mFPSText;

	HWND mPlayerStateText;

	HWND mPlayerDownLoadText;

	HWND mPlayerBiteRateText;

	HWND mPlayerProgressAndDurationText;
	//CurrentDataModelManager* mpCurrentDataModelManager;
};

 