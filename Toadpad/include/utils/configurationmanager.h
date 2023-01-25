#ifndef CONFIGURATION_MANAGER_H
#define CONFIGURATION_MANAGER_H

#define MAX_LENGTH 64

#include "common.h"
#include "pugixml.hpp"

constexpr const char* CONFIG_ROOT = "chadribleu.toadpad.config";

constexpr int APP_SHOW_STATUS_BAR		= 0;
constexpr int APP_ENABLE_DARK_THEME		= 1;
constexpr int APP_SHOW_PATH				= 2;
constexpr int MAINWINDOW_WIDTH			= 3;
constexpr int MAINWINDOW_HEIGHT			= 4;
constexpr int EDITOR_ZOOM_NUMERATOR		= 5;
constexpr int EDITOR_FONT_FAMILY		= 6;
constexpr int EDITOR_FONT_SIZE			= 7;
constexpr int EDITOR_FONT_UNIT			= 8;
constexpr int EDITOR_ENABLE_WORD_WRAP	= 9;
constexpr int TABITEM_HEIGHT			= 10;
constexpr int TABITEM_MAX_WIDTH			= 11;
constexpr int TABITEM_MOVEABLE			= 12;
constexpr int TABITEM_BOLD_TEXT			= 13;
constexpr int TABITEM_SHOW_CLOSE		= 14;
constexpr int NUM_SETTINGS				= 15;

enum FontSelectorMode { System, Toadpad };
enum FontUnit { Pixel, Point };

struct ConfigurationState 
{
	// general
	char language[128];
	bool show_status_bar;
	bool show_path_titlebar;
	bool splash_screen;
	bool double_buffering;
	FontSelectorMode font_selector_mode;
	unsigned int max_recent_files;
	bool prompt_save_dial;
	bool cached_files;

	// editor
	char default_font_family[128];
	unsigned default_font_size;
	FontUnit font_unit;
	float zoom_percentage;
	bool keep_zoom_setting;
	bool word_wrap;
	bool line_num;

	// appearance
	bool dark_theme;
	char splash_bmp_path[MAX_PATH];

	// tabs
	bool tabs_use_fixed_width;
	bool tabs_text_wrap;
	bool tabs_show_color_accent;
	bool tabs_closeable;
	unsigned int tabs_height;
	unsigned int tabs_maximum_width;
	bool tabs_bold_text;
	bool tabs_moveable;
	bool single_doc_mode;
};

class ConfigurationManager
{
public:
	ConfigurationManager(const ConfigurationManager&) = delete;
	ConfigurationManager& operator=(const ConfigurationManager&) = delete;

	static ConfigurationManager* GetInstance();

	bool Load(std::filesystem::path path);
	bool Update(const char* key, const char* value);
	pugi::xml_text Get(const char* key);
	bool Check(const pugi::xml_node& start);
	bool Create(const std::filesystem::path path);

	inline const ConfigurationState& GetConfigState() { return m_ConfigState; }

private:
	ConfigurationManager();
	~ConfigurationManager() = default;

	ConfigurationState m_ConfigState;

	bool DoChecking(const pugi::xml_node& node_info, int key_index);
	bool m_WasLoaded;
	std::filesystem::path m_FilePath;
	pugi::xml_document m_Document;
	static std::mutex s_Mutex;
	static std::atomic<ConfigurationManager*> s_This;

	static constexpr const char* s_Keys[] = 
	{
		"application.show_status_bar",  "application.enable_dark_theme","titlebar.show_path",
		"mainwindow.width", "mainwindow.height", "editor.zoom_numerator",
		"editor.font_family", "editor.font_size", "editor.font_unit",
		"editor.enable_word_wrap", "tabitem.height", "tabitem.max_width", "tabitem.moveable",
		"tabitem.bold_text", "tabitem.show_close_button"
	};
	static constexpr const char* s_Values[] = 
	{
	"true", "false", "false", 
	"800", "600", "1", 
	"Consolas", "12", "px", 
	"false", "30", "150", "false", 
	"true", "true"
	};
};

#endif
