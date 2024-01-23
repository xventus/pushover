{*
  Function block for sending PUSH NOTIFICATION message s over pushover.net
  Unfortunately it works only with Foxtrot2. Only, Foxtrot 2 supports sending HTTP over TLS.
  Note: The message size must be chosen with respect to the size of the parameter length.
*}

  FUNCTION_BLOCK fbPushNotification
    VAR_INPUT
      trigger:  BOOL R_EDGE;  // push notification trigger
      token : STRING;         // pushover token - required
      user : STRING;          // push over user name- required
      title : STRING;         // push over message title
      device : STRING;        // push over device name
      message : STRING;       // push over message content - required
      sound : STRING;         // play specific sound
    END_VAR
    VAR_OUTPUT
       successful : BOOL := FALSE;
       busy : BOOL := FALSE;
       error : BOOL := FALSE;
    END_VAR
    VAR_IN_OUT
    END_VAR
    VAR
      tlsPostRequest : fbHttpRequestL3;
      splitUrl : fbSplitUrlAddress;
      ipAddress : TIPadr;
      hostName : STRING;
      act : STRING;
      messageContent : STRING[255];
      inboundData : THttpBuffer;
      contentType : STRING[33] := 'application/x-www-form-urlencoded';
      pushoverURL : STRING := 'https://api.pushover.net/1/messages.json';
    END_VAR
    VAR_TEMP
    END_VAR

    IF  trigger THEN
         successful := FALSE;
         busy := FALSE;
         error := FALSE;
    END_IF
    
    splitUrl( split := trigger,
              urlAddress := pushoverURL,
              host := hostName,
              action := act,
              ipAdr := ipAddress,
              port := tlsPostRequest.Port);



    IF splitUrl.done THEN
      tlsPostRequest.chanCode := OpenUniSocket(protocol := UNI_SSL_CLIENT);
    END_IF;

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

    tlsPostRequest( Post := splitUrl.done,
                 ContentType := contentType,
                 IPadr := ipAddress,
                 Action := act,
                 Host := hostName,
                 postLen := LEN(messageContent),
                 Data := void(messageContent),
                 RecvData := inboundData);

    busy :=  tlsPostRequest.busy;
    
    IF  tlsPostRequest.Done AND tlsPostRequest.Result = 200  THEN
        successful:= true;
    END_IF
    
    IF tlsPostRequest.Done AND tlsPostRequest.Result <> 200  THEN
        error:= true;
    END_IF
      
    

    IF NOT tlsPostRequest.Busy AND tlsPostRequest.chanCode <> 0 THEN
      CloseUniSocket(chanHandle := tlsPostRequest.chanCode);
    END_IF;


  END_FUNCTION_BLOCK



