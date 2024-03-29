//
// vim: ts=4 et
// Copyright (c) 2023 Petr Vanek, petr@fotoventus.cz
//
/// @file   web_task.h
/// @author Petr Vanek

#pragma once

#include "hardware.h"
#include "rptask.h"
#include "access_point.h"
#include "literals.h"
#include "wifi_scanner.h"
#include "https_cli.h"

class WebTask : public RPTask
{
public:
 enum class Mode {
		ClearAPInfo,
	    Setting,     
        Control, 	
        Unknown       };

	WebTask();
	virtual ~WebTask();
	void command(Mode mode);
	void apInfo(const APInfo& ap);
protected:
	void loop() override;

private:
	std::string urlDecode(const std::string &src);
	
private:
	Mode            _mode {Mode::Unknown};
	QueueHandle_t 	_queue;
	QueueHandle_t 	_queueAP;
	QueueHandle_t 	_queueLcs;
};
