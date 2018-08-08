#include <iostream>
#include <libxml.h>
#include <string>
#include <map>

using namespace std;


class Setting
{
	protected:
//		std::map<string, string> setting_map;
		void* configuration;
		string setting_file_path;

	public:
		Setting(const string& setting_file_path) : configuration(0), setting_file_path(setting_file_path) { }
		virtual ~Setting(){}
		string find(const string& key);
		bool insert(const string& key, const string& content);
		const string& get_settingFilePath() { return setting_file_path; }
		static std::map<string, Setting*>& get_settingFileMap();

		virtual bool initialize() = 0;
		virtual bool serialize() = 0;
		virtual bool deserialize() = 0;
		virtual bool reset_setting() = 0;
	private:
		static std::map<string, Setting*> setting_file_map;
};

class XmlSetting : public Setting
{
	public:
		XmlSetting(const string& xml_setting_file_path);
		virtual ~XmlSetting();

		virtual bool initialize();
		virtual bool serialize();
		virtual bool deserialize();
		virtual bool reset_setting();
		virtual bool convert_setting_struct2xml() = 0;
		virtual bool convert_setting_xml2struct() = 0;


	private:
		xmlDocPtr xml_doc;
};

std::map<string, Setting*> Setting::setting_file_map;

XmlSetting::XmlSetting(const string& xml_setting_file_path) : Setting(xml_setting_file_path), xml_doc(0) { }

bool XmlSetting::initialize()
{
}

/**
  * @function XmlSetting:
  *
  */
bool XmlSetting::serialize()
{
}

bool XmlSetting::deserialize()
{
	this->xml_doc = xmlReadFile(this->setting_file_path.c_str(), NULL, 0);
	if(xml_doc == NULL)
	{
		return false;
	}

	xmlNodePtr cur = NULL;
	xmlChar* typeString = NULL;

	cur = xmlDocGetRootElement(doc);
	if(cur == NULL)
	{
		return false;
	}
	cur = cur->xmlChildrenNode;
	while(cur != NULL)
	{
	}

	return true;
}
