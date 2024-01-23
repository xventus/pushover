{*
  Function block for sending PUSH NOTIFICATION message through the PushOver gateway
  Function designed for Foxtrot 1, which cannot send TLS messages and cannot create UNI channel.
  Note: The message size must be chosen with respect to the size of the parameter length.
*}

  FUNCTION_BLOCK fbPushNotificationGtw
    VAR_INPUT
      trigger:  BOOL R_EDGE;  // push notification trigger
      token : STRING;         // pushover token - required
      user : STRING;          // push over user name- required
      title : STRING;         // push over message title
      device : STRING;        // push over device name
      message : STRING;       // push over message content - required
      sound : STRING;         // play specific sound
      gatewayUrl : STRING;    // gateway URL
      chanCode : UINT;        // UNI TCP channel
    END_VAR
    VAR_OUTPUT
       successful : BOOL := FALSE;
       busy : BOOL := FALSE;
       error : BOOL := FALSE;
    END_VAR
    VAR_IN_OUT
    END_VAR
    VAR
      postRequest : fbHttpRequestL3;
      splitUrl : fbSplitUrlAddress;
      ipAddress : TIPadr;
      hostName : STRING;
      act : STRING;
      messageContent : STRING[255];
      inboundData : THttpBuffer;
      contentType : STRING[33] := 'application/x-www-form-urlencoded';
    END_VAR
    VAR_TEMP
    END_VAR

    IF  trigger THEN
         successful := FALSE;
         busy := FALSE;
         error := FALSE;
    END_IF

    splitUrl( split := trigger,
              urlAddress := gatewayUrl,
              host := hostName,
              action := act,
              ipAdr := ipAddress,
              port := postRequest.Port);


    messageContent := 'token=' + token + '&user=' + user + '&message=' + message;

    IF title <> '' THEN
        messageContent := messageContent +  '&title=' + title;
    END_IF

    IF device <> '' THEN
        messageContent := messageContent +  '&device=' + device;
    END_IF

    IF sound <> '' THEN
        messageContent := messageContent +  '&sound=' + sound;
    END_IF

    postRequest( Post := splitUrl.done,
                 chanCode := chanCode,
                 ContentType := contentType,
                 IPadr := ipAddress,
                 Action := act,
                 Host := hostName,
                 postLen := LEN(messageContent),
                 Data := void(messageContent),
                 RecvData := inboundData,
                 Close:=TRUE
                );

    busy :=  postRequest.busy;

    IF  postRequest.Done AND postRequest.Result = 200  THEN
        successful:= true;
    END_IF

    IF postRequest.Done AND postRequest.Result <> 200  THEN
        error:= true;
    END_IF


  END_FUNCTION_BLOCK



