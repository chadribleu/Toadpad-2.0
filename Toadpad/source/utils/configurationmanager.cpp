#include "utils/configurationmanager.h"

std::atomic<ConfigurationManager*> ConfigurationManager::s_This = nullptr;
std::mutex ConfigurationManager::s_Mutex;

ConfigurationManager::ConfigurationManager() : m_WasLoaded(false) {}

ConfigurationManager* ConfigurationManager::GetInstance()
{
	ConfigurationManager* instance = s_This.load(std::memory_order_acquire);
	if (instance == nullptr) {
		const std::lock_guard<std::mutex> lock(s_Mutex);
		instance = s_This.load(std::memory_order_relaxed);
		if (!instance) {
			s_This.store(new ConfigurationManager());
		}
	}
	return s_This;
}

bool ConfigurationManager::Load(std::filesystem::path path)
{
	pugi::xml_parse_result dcStatus = m_Document.load_file(path.c_str());
	if (!dcStatus) {
		return false;
	}
	m_FilePath = path;
	pugi::xml_node baseNode = m_Document.child(CONFIG_ROOT);
	m_WasLoaded = true;
	return Check(baseNode);
}

bool ConfigurationManager::Update(const char* key, const char* value)
{
	if (!m_WasLoaded) {
		return false;
	}
	pugi::xml_node workingNode = m_Document.child(CONFIG_ROOT).find_child_by_attribute("name", key);
	if (!workingNode) {
		return false;
	}
	workingNode.text().set(value);
	std::ofstream of(m_FilePath);
	if (!of.is_open()) {
		return false;
	}
	m_Document.save(of);
	return true;
}

pugi::xml_text ConfigurationManager::Get(const char* key)
{
	pugi::xml_node elem = m_Document.child(CONFIG_ROOT).find_child_by_attribute("name", key);
	if (!elem) {
		return pugi::xml_text();
	}
	return elem.text();
}

bool ConfigurationManager::Check(const pugi::xml_node& start)
{
	if (!start.first_child() || (!start.next_sibling() && start.parent() != start.parent())) {
		return false;
	}
	for (auto node : start.children()) {
		const char* PCDATA = nullptr;
		const char* value = nullptr;
		bool hasNameAttribute = false;
		bool hasPCDATA = false;
		for (auto attribute : node.attributes()) {
			const char* temp = attribute.name();
			if (strcmp(attribute.name(), "name") == 0) {
				value = attribute.value();
				hasNameAttribute = true;
				PCDATA = node.child_value();
				if (PCDATA != "") { hasPCDATA = true; }
				break;
			}
		}
		/* Check node's value only if  */
		if (hasNameAttribute && hasPCDATA) {
			int workingValue = -1;
			/* perform checking for all keys and values */
			for (int i = 0; i < NUM_SETTINGS; ++i) {
				workingValue = i;
				if (strcmp(value, s_Keys[i]) == 0) {
					break;
				}
			}
			if (!DoChecking(node, workingValue)) return false; /* invalid file */
		}
		if (node.first_child()) {
			Check(node);
		}
		Check(node.next_sibling());
	}
	return true;
}

bool ConfigurationManager::Create(const std::filesystem::path path)
{
	std::ofstream output(path);
	if (!output.is_open()) {
		std::filesystem::create_directory(path.parent_path());
		output.open(path, std::ios::out);
		if (!output.is_open()) {
			return false;
		}
	}
	m_FilePath = path;
	pugi::xml_document newDocument;
	pugi::xml_node base = newDocument.append_child(CONFIG_ROOT);
	for (int i = 0; i < NUM_SETTINGS; ++i) {
		pugi::xml_node propNode = base.append_child("Property");
		pugi::xml_attribute attribNode = propNode.append_attribute("name");
		attribNode.set_value(s_Keys[i]);
		propNode.text().set(s_Values[i]);
	}
	newDocument.save(output);
	return true;
}

bool ConfigurationManager::DoChecking(const pugi::xml_node& node_info, int key_index)
{
#ifdef _DEBUG
	char out[256];
	sprintf_s(out, 256, "DoChecking() for %d (%s)\n", key_index, s_Keys[key_index]);
	OutputDebugStringA(out);
#endif
	pugi::xml_text nodePCDATA = node_info.text();
	
	bool success = false;
	
	switch (key_index) {
	case MAINWINDOW_WIDTH:
		success = (nodePCDATA.as_int() >= 150);
		break;

	case MAINWINDOW_HEIGHT:
		success = (nodePCDATA.as_int() >= 360);
		break;

	case EDITOR_FONT_UNIT:
	{
		const char* res = nodePCDATA.as_string();
		if (res[0] == 'p' && res[1] == 'x') {
			m_ConfigState.font_unit = FontUnit::Pixel;
			success = true;
		}
		else if (res[0] == 'p' && res[1] == 't') {
			m_ConfigState.font_unit = FontUnit::Point;
			success = true;
		}
		break;
	}

	case EDITOR_FONT_SIZE:
	{
		int font_size = nodePCDATA.as_int();
		if (font_size <= 8) {
			m_ConfigState.default_font_size = font_size;
			success = true;
		}
		break;
	}

	case EDITOR_ZOOM_NUMERATOR:
	{
		float factor = nodePCDATA.as_float();
		if (factor >= 0.1f || factor <= 5.0f) {
			m_ConfigState.zoom_percentage = factor;
			success = true;
		}
		break;
	}

	case TABITEM_HEIGHT:
	{
		int height = nodePCDATA.as_int();
		if (height >= 12 || height <= 50) {
			m_ConfigState.tabs_height = height;
			success = true;
		}
		break;
	}

	case TABITEM_MAX_WIDTH:
	{
		int width = nodePCDATA.as_int();
		if (width >= 125 || width <= 360) {
			m_ConfigState.tabs_maximum_width = width;
			success = true;
		}
		break;
	}

	default:
		break;
	}
	return success;
}