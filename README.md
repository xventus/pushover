# PushOver Gateway for Foxtrot


## Hardware

---
| ESP32 GPIO  | Note         |
|------------|---------------|
|   GPIO_NUM_2  | A BUTTON |
|   GPIO_NUM_0  | FLASH BUTTON |
|   GPIO_NUM_5  | HEART BEAT LED |



## HTTP command

Send message via gateway

`curl -X POST -H "Content-Type: application/x-www-form-urlencoded" -d "token=axxxxxxxxxx&user=uxxxxxxxxxxxx&title=TITLE_HERE&device=DEVICE&message=MESSAGE" http://192.168.2.224/push`



## Web interface

<img src="imgs/ap.png" alt="AP" width="400">
<img src="imgs/co.png" alt="Control" width="400">




