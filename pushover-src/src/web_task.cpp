
//
// vim: ts=4 et
// Copyright (c) 2023 Petr Vanek, petr@fotoventus.cz
//
/// @file   web_task.cpp
/// @author Petr Vanek

#include <stdio.h>
#include <memory.h>
#include <math.h>
#include <stdlib.h>
#include <ctype.h>

#include "application.h"
#include "web_task.h"
#include "http_server.h"
#include "key_val.h"
#include "content_file.h"
#include "http_request.h"
#include <cJSON.h>

WebTask::WebTask()
{
	_queue = xQueueCreate(2, sizeof(int));
	_queueAP = xQueueCreate(2, sizeof(APInfo));
}

WebTask::~WebTask()
{
	done();
	if (_queue)
		vQueueDelete(_queue);
	if (_queueAP)
		vQueueDelete(_queueAP);
}


std::string WebTask::urlDecode(const std::string& src) {
    std::string ret;
    ret.reserve(src.length()); 

    for (std::size_t i = 0; i < src.length(); ++i) {
        if (src[i] == '%') {
            if (i + 2 < src.length()) {
                int value = 0;
                std::istringstream is(src.substr(i + 1, 2));
                if (is >> std::hex >> value) {
                    ret += static_cast<char>(value);
                    i += 2;
                } else {
                    // invalid sequence keep %
                    ret += '%';
                }
            }
        } else if (src[i] == '+') {
            ret += ' ';
        } else {
            ret += src[i];
        }
    }

    return ret;
}


void WebTask::loop()
{

	APInfo apinf = {
		.ap_name = "",
		.rssi = 0
	};
	HttpServer server;
	std::string apinfo;

	while (true)
	{ // Loop forever
		
		// AP info update - used only in registration
		auto resap = xQueueReceive(_queueAP, (void *)&apinf, 0);
		if (resap == pdTRUE)
		{  
			apinfo += "<li><pre>";
			apinfo += apinf.ap_name;
			apinfo += "  RSSI: ";
			apinfo += std::to_string(apinf.rssi);
			apinfo += "</pre></li>";
		}
		

		int receivedMode;
		auto res = xQueueReceive(_queue, (void *)&receivedMode, 0);
		if (res == pdTRUE)
		{
			Mode mode = static_cast<Mode>(receivedMode);
			if (mode == Mode::Unknown)
			{
				server.stop();
			}
			else if (mode == Mode::Control)
			{
				server.stop();
				server.start();
				server.registerUriHandler("/", HTTP_GET, [&apinfo](httpd_req_t *req) -> esp_err_t
										  {
					ConentFile ap1cntb(literals::kv_fl_index);
				
    				auto cnt =  ap1cntb.readContnet();
 					httpd_resp_send(req, cnt.c_str(), cnt.length());
                    return ESP_OK; });

				server.registerUriHandler(literals::c_style, HTTP_GET, [](httpd_req_t *req) -> esp_err_t
										  {
					 httpd_resp_set_type(req, literals::c_css);
					 ConentFile ap1cnt(literals::kv_fl_style);
    				 auto contnet = ap1cnt.readContnet();
                    httpd_resp_send(req, contnet.c_str(), contnet.length());
                    return ESP_OK; });

				server.registerUriHandler("/psh.png", HTTP_GET, [](httpd_req_t *req) -> esp_err_t
										  {
					 httpd_resp_set_type(req, literals::c_image);
					 ConentFile ap1cnt(literals::kv_fl_psh);
    				 auto contnet = ap1cnt.readContnet();
                     httpd_resp_send(req, contnet.c_str(), contnet.length());
                    return ESP_OK; });

				// commands 
				server.registerUriHandler("/push", HTTP_POST, [this](httpd_req_t *req) -> esp_err_t {
					char content[255] = {0}; 
					int received = httpd_req_recv(req, content, sizeof(content) - 1);
					if (received <= 0) { 
						if (received == HTTPD_SOCK_ERR_TIMEOUT) {
							httpd_resp_send_408(req);
						}
						return ESP_FAIL;
					}

					// url encoded params
					std::string data(content);
					std::stringstream data_stream(data);
					std::string segment;
					std::unordered_map<std::string, std::string> params;

					while (std::getline(data_stream, segment, '&')) {
						size_t pos = segment.find('=');
						if (pos != std::string::npos) {
							std::string key = segment.substr(0, pos);
							std::string value = urlDecode(segment.substr(pos + 1));
							params[key] = value;
						}
					}

				// check - mandatory items
				bool hasToken = params.find(literals::c_token) != params.end();
				bool hasUser = params.find(literals::c_user) != params.end();
				bool hasDevice = params.find(literals::c_message) != params.end();

				if (!hasToken || !hasUser || !hasDevice) {
						// no mandatory items
						httpd_resp_send_500(req);
						return ESP_FAIL;
					}

					KeyVal& kv = KeyVal::getInstance();
					auto srv = kv.readString(literals::kv_pshover, literals::kv_val_pshover);
					if (srv.empty()) {
						httpd_resp_send_500(req);
						return ESP_FAIL;
					}

					// TODO: convert to task 
					HTTPSClient client(srv);
					if (!client.post(content)) {
						httpd_resp_send_500(req);
						return ESP_FAIL;
					}
				
					httpd_resp_set_hdr(req, "Connection", "close");
					httpd_resp_send(req, literals::c_ok, HTTPD_RESP_USE_STRLEN);
					return ESP_OK; 
				});

				
			}
			else if (mode == Mode::Setting)
			{
				server.stop();
				server.start();

				// AP main page
				server.registerUriHandler("/", HTTP_GET, [&apinfo](httpd_req_t *req) -> esp_err_t
										  {
					ConentFile ap1cntb(literals::kv_fl_apb);
					ConentFile ap1cnte(literals::kv_fl_ape);
    				 auto contnetb = ap1cntb.readContnet();
                    contnetb += apinfo;
					contnetb += ap1cnte.readContnet();
 					httpd_resp_send(req, contnetb.c_str(), contnetb.length());
                    return ESP_OK; });

				server.registerUriHandler(literals::c_style, HTTP_GET, [](httpd_req_t *req) -> esp_err_t
										  {
					 httpd_resp_set_type(req, literals::c_css);
					 ConentFile ap1cnt(literals::kv_fl_style);
    				 auto contnet = ap1cnt.readContnet();
                    httpd_resp_send(req, contnet.c_str(), contnet.length());
                   
                    return ESP_OK; });


				server.registerUriHandler("/kv", HTTP_GET, [&apinfo](httpd_req_t *req) -> esp_err_t
					{

					KeyVal& kv = KeyVal::getInstance();
					cJSON *root = cJSON_CreateObject();
					if (root) 
					{
						
						auto v = kv.readString(literals::kv_ssid);
						cJSON_AddStringToObject(root, literals::kv_ssid, v.c_str());  

						// can be comment for sec. reason if you want	
						v = kv.readString(literals::kv_passwd);
						cJSON_AddStringToObject(root, literals::kv_passwd, v.c_str());  
						
						v = kv.readString(literals::kv_ip);
						cJSON_AddStringToObject(root, literals::kv_ip, v.c_str());  
						
						v = kv.readString(literals::kv_mask);
						cJSON_AddStringToObject(root, literals::kv_mask, v.c_str());  
						
						v = kv.readString(literals::kv_gtw);
						cJSON_AddStringToObject(root, literals::kv_gtw, v.c_str());

						v = kv.readString(literals::kv_dns);
						cJSON_AddStringToObject(root, literals::kv_dns, v.c_str());

						v = kv.readString(literals::kv_pshover, literals::kv_val_pshover);
						if (v.empty()) {
							v = literals::kv_val_pshover;
						}
						
						cJSON_AddStringToObject(root, literals::kv_pshover, v.c_str());  
						
						httpd_resp_set_type(req, literals::c_apjson);

						char *json_string = cJSON_Print(root);
						if (json_string != nullptr) {
						httpd_resp_send(req, json_string, strlen(json_string));
						}

						cJSON_Delete(root);
						if (json_string) free(json_string);
					}
							
				return ESP_OK; });

				// AP setting answer
				server.registerUriHandler("/", HTTP_POST, [this](httpd_req_t *req) -> esp_err_t {
					char content[300] = {0}; 
					int received = httpd_req_recv(req, content, sizeof(content) - 1);
					if (received <= 0) { 
						if (received == HTTPD_SOCK_ERR_TIMEOUT) {
							httpd_resp_send_408(req);
						}
						return ESP_FAIL;
					}

					// parse response & store configuration
					std::map<std::string, std::string> formData = HttpReqest::parseFormData(std::string(content));
					KeyVal& kv = KeyVal::getInstance();
					
					kv.writeString(literals::kv_ssid, HttpReqest::getValue(formData, literals::kv_ssid).c_str());
					kv.writeString(literals::kv_passwd, HttpReqest::getValue(formData, literals::kv_passwd).c_str());
					kv.writeString(literals::kv_ip, HttpReqest::getValue(formData, literals::kv_ip).c_str());
					kv.writeString(literals::kv_gtw, HttpReqest::getValue(formData, literals::kv_gtw).c_str());
					kv.writeString(literals::kv_mask, HttpReqest::getValue(formData, literals::kv_mask).c_str());
					kv.writeString(literals::kv_dns, HttpReqest::getValue(formData, literals::kv_dns).c_str());
					auto urlencodedUrl = HttpReqest::getValue(formData, literals::kv_pshover);
					auto pshdecoded = urlDecode(urlencodedUrl);
					kv.writeString(literals::kv_pshover, pshdecoded.c_str());
					
					
					// switch to Stop mode & check configuration
					Application::getInstance()->getWifiTask()->switchMode(WifiTask::Mode::Stop);

					// Response & swith mode 
					ConentFile respContnetDialog(literals::kv_fl_finish);
    				auto finish = respContnetDialog.readContnet();
					httpd_resp_send(req, finish.c_str(), finish.length());

					return ESP_OK; 
				});
			}
		}

		vTaskDelay(500 / portTICK_PERIOD_MS);
	}
}

void WebTask::apInfo(const APInfo &ap)
{
	if (_queueAP)
	{
		xQueueSendToBack(_queueAP, (void *)&ap, 0);
	}
}

void WebTask::command(Mode mode)
{
	if (_queue)
	{
		int modeToSend = static_cast<int>(mode);
		xQueueSendToBack(_queue, (void *)&modeToSend, 0);
	}
}
