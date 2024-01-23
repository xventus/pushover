//
// vim: ts=4 et
// Copyright (c) 2024 Petr Vanek, petr@fotoventus.cz
//
/// @file   https_cli.h
/// @author Petr Vanek
#pragma once

#include <string>
#include "esp_log.h"
#include "esp_http_client.h"

class HTTPSClient
{
private:
    std::string _url;
    esp_http_client_config_t _config;
    esp_http_client_handle_t _client;

    static esp_err_t http_event_handler(esp_http_client_event_t *evt)
    {
        return ESP_OK;
    }

public:
    HTTPSClient(const std::string &url) : _url(url)
    {
        _config = {
            .url = _url.c_str(),
            .event_handler = http_event_handler,
            .skip_cert_common_name_check = true,
        };
    }

    ~HTTPSClient()
    {
        esp_http_client_cleanup(_client);
    }

    bool post(const std::string &data)
    {
        bool rc = false;

        do
        {
            _client = esp_http_client_init(&_config);
            if (_client == nullptr)
            {
                break;
            }

            esp_http_client_set_method(_client, HTTP_METHOD_POST);
            esp_http_client_set_post_field(_client, data.c_str(), data.length());

            esp_err_t err = esp_http_client_perform(_client);

            if (err == ESP_OK)
            {
                rc = true;
            }
        } while (false);
        return rc;
    }
};
