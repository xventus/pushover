//
// vim: ts=4 et
// Copyright (c) 2023 Petr Vanek, petr@fotoventus.cz
//
/// @file   application.h
/// @author Petr Vanek

#include "application.h"
#include "hardware.h"
#include "content_file.h"
#include "driver/uart.h"
#include "button.h"
#include "key_val.h"

// global application instance as singleton and instance acquisition.


Application *Application::getInstance()
{
    static Application instance;
    return &instance;
}

Application::Application() : _heartBeat(HEART_BEAT_LED)
{
}

Application::~Application()
{
    done();
}

void Application::checkRenewAP() 
{
     // reset AP check
    Button b1(A_BUTTON);
    KeyVal& kv = KeyVal::getInstance();
   
    if (b1.isPressed()) {
		kv.writeString(literals::kv_ssid, "");
    }
}

void Application::init()
{
    // Begin initialization of core functions so that individual 
    // wrappers no longer need to execute
    
    // initi file system
    ConentFile::initFS();

    // initialize NVS
    KeyVal& kv = KeyVal::getInstance();
    kv.init(literals::kv_namespace ,true, false);

    // initialize newtwork interfaces
    ESP_ERROR_CHECK(esp_netif_init());

    // default event loop
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    vTaskDelay(100 / portTICK_PERIOD_MS); 
 
    checkRenewAP();

   
}

void Application::run()
{
    // task create
    do
    {
        if (!_heartBeat.init(literals::tsk_led, tskIDLE_PRIORITY + 1ul, configMINIMAL_STACK_SIZE*10))
            break;

        if (!_wifiTask.init(literals::tsk_web, tskIDLE_PRIORITY + 1ul, 4086))
            break;

        if (!_webTask.init(literals::tsk_wifi, tskIDLE_PRIORITY + 1ul, 6000))
            break;


    } while (false);
    
    
}

void Application::done()
{
    // fail 
    KeyVal& kv = KeyVal::getInstance();
    kv.done();
}

Application *Application::operator->()
{
    return Application::getInstance();
}

Application  *Application::operator->() const
{
    return Application::getInstance();
}
