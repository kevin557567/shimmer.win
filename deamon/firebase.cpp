#include "firebase.h"
#include <iostream>
#include <string>
#include <stdio.h>

#include "firebase/app.h"
#include "firebase/remote_config.h"
#include "firebase/util.h"

#include "DataEncrypt.h"

static std::string firebase_url   = "https://console.firebase.google.com/u/0/project/quantum-41089";
static std::string firebase_doc  = "{\"client\":[{\"services\":{\"appinvite_service\":{\"status\":1},\"analytics_service\":{\"status\":1}},\"api_key\":[{\"current_key\":\"AIzaSyAQIaBpi-8u7v1e4t1qqNnXpGNciWRFDno\"}],\"client_info\":{\"mobilesdk_app_id\":\"1:779891098319:ios:dc89d7cdcf13dbefabedba\",\"android_client_info\":{\"package_name\":\"com.circle.win-circle\"}}}],\"configuration_version\":\"1\",\"project_info\":{\"storage_bucket\":\"circle-74453.appspot.com\",\"project_id\":\"circle-74453\",\"project_number\":\"779891098319\"}}";

void firebase_config(std::string url, std::string doc) {
	firebase_url = url;
	firebase_doc = doc;
}

void get_configure(std::map<std::string, std::string>& v ) {
	::firebase::AppOptions op;
	op.set_database_url( firebase_url.c_str() );
	::firebase::AppOptions::LoadFromJsonConfig(firebase_doc.c_str(), &op);
	::firebase::App* mApp = firebase::App::Create(op);
	firebase::remote_config::RemoteConfig* cfg = firebase::remote_config::RemoteConfig::GetInstance(mApp);
	firebase::remote_config::ConfigSettings settings;
	settings.fetch_timeout_in_milliseconds = 20000;
	settings.minimum_fetch_interval_in_milliseconds = 5000;
	cfg->EnsureInitialized();
	cfg->SetConfigSettings(settings);
	//cfg->SetConfigSettingsLastResult();//保存结果，下次启动如果拉取失败直接使用
	//cfg->FetchLastResult();
	for (int tryTimes = 3; tryTimes > 0; tryTimes--) {
		cfg->FetchAndActivate();
		if (cfg->GetKeys().size() > 0)
			break;
		Sleep(3000);
	}
	for (auto var : cfg->GetKeys()) {
		auto str = cfg->GetString(var.c_str());
		v.insert(std::make_pair(var, str));
		std::string data =  CDataEncrypt::EncryptA_ECB(str.c_str(), "ABCDEF");
	}
	delete cfg;
	delete mApp;
}
