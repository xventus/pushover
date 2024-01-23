//
// vim: ts=4 et
// Copyright (c) 2023 Petr Vanek, petr@fotoventus.cz
//
/// @file   literals.h
/// @author Petr Vanek

#pragma once

#include <stdio.h>
#include <string_view>


class literals
{
public:

    // internal - do not modify
    
    // internal task name
    static constexpr const char *tsk_led{"LEDTSK"};
    static constexpr const char *tsk_web{"WEBTSK"};
    static constexpr const char *tsk_wifi{"WIFITSK"};

    // AP definition
    static constexpr const char *ap_name{"PUSHOVER AP"};
    static constexpr const char *ap_passwd{""};

    // KV & files
    static constexpr const char *kv_namespace{"pushover"};
    static constexpr const char *kv_ssid{"ssid"};
    static constexpr const char *kv_dns{"dns"};
    static constexpr const char *kv_passwd{"pass"};
    static constexpr const char *kv_ip{"ip"};
    static constexpr const char *kv_gtw{"gw"};
    static constexpr const char *kv_mask{"mask"};
    static constexpr const char *kv_pshover{"pshover"};
    static constexpr const char *kv_val_pshover{"https://api.pushover.net/1/messages.json"};
    
    // spiffs filenames
    static constexpr const char *kv_fl_index{"/spiffs/index.html"}; 
    static constexpr const char *kv_fl_apb{"/spiffs/ap_beg.html"};
    static constexpr const char *kv_fl_ape{"/spiffs/ap_end.html"}; 
    static constexpr const char *kv_fl_style{"/spiffs/style.css"}; 
    static constexpr const char *kv_fl_psh{"/spiffs/psh.png"}; 
    static constexpr const char *kv_fl_finish{"/spiffs/finish.html"};

    // const
    static constexpr const char *c_apjson {"application/json"};
    static constexpr const char *c_token {"token"};
    static constexpr const char *c_user {"user"};
    static constexpr const char *c_message {"message"};
    static constexpr const char *c_style {"/style.css"};
    static constexpr const char *c_css {"text/css"};
    static constexpr const char *c_image {"image/png"};
    static constexpr const char *c_ok {"OK"};
   
    
    
  
    


};