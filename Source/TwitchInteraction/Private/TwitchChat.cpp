// Fill out your copyright notice in the Description page of Project Settings.


#include "TwitchChat.h"
#include <string>

UTwitchChat::UTwitchChat()
{
	PrimaryComponentTick.bCanEverTick = true;
	if (Socket != nullptr)
	{
		Socket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Socket);
	}
}

void UTwitchChat::BeginPlay()
{
	Super::BeginPlay();
}


void UTwitchChat::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UTwitchChat::SetUserInfo(const FString _oauth, const FString _authType, const  FString _username, const FString _channel)
{
	OAuthToken = _oauth;
	OAuthTokenType = _authType;
	Username = _username;
	Channel = _channel;
	Init = true;
}

bool UTwitchChat::SendMessage(FString _message, bool _sendTo, FString _channel)
{
	if (Socket != nullptr && Socket->GetConnectionState() == ESocketConnectionState::SCS_Connected)
	{
		if (_sendTo)
		{
			_message = "PRIVMSG #" + _channel + " :" + _message;
		}
		_message += "\n";
		TCHAR* serialized_message = _message.GetCharArray().GetData();
		int32 size = FCString::Strlen(serialized_message);
		int32 out_sent;
		return Socket->Send((uint8*)TCHAR_TO_UTF8(serialized_message), size, out_sent);
	}
	else
	{
		return false;
	}
}

bool UTwitchChat::Connect(FString& _result)
{
	ISocketSubsystem* sss = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
	TSharedRef<FInternetAddr> connection_addr = sss->CreateInternetAddr();
	ESocketErrors socket_error = sss->GetHostByName("irc.chat.twitch.tv", *connection_addr); // Name resolution for Twitch IRC server

	if (socket_error != SE_NO_ERROR)
	{
		_result = "Could not resolve hostname!";
		return false; // if the host could not be resolved return false
	}

	const int32 port = 6667; // Standard IRC port
	connection_addr->SetPort(port);

	FSocket* ret_socket = sss->CreateSocket(NAME_Stream, TEXT("TwitchPlay Socket"), false);

	if (ret_socket == nullptr)
	{
		_result = "Could not create socket!";
		return false;
	}

	int32 out_size;
	ret_socket->SetReceiveBufferSize(2 * 1024 * 1024, out_size);
	ret_socket->SetReuseAddr(true);

	bool b_has_connected = ret_socket->Connect(*connection_addr);

	if (!b_has_connected)
	{
		ret_socket->Close();
		sss->DestroySocket(ret_socket);

		_result = "Connection to Twitch IRC failed!";
		return false;
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(UpdateTimer, this, &UTwitchChat::ReceiveData, 0.1f, true);
		Socket = ret_socket;
		return true;
	}
}


void UTwitchChat::ReceiveData()
{
	if (Socket == nullptr)
	{
		return;
	}

	TArray<uint8> data;
	uint32 data_size;
	bool received = false;
	if (Socket->HasPendingData(data_size))
	{
		received = true;
		data.SetNumUninitialized(data_size); // Make space for the data
		int32 data_read;
		Socket->Recv(data.GetData(), data.Num(), data_read); // Receive the data. Hopefully the buffer is large enough
	}

	FString f_string_data = "";
	if (received)
	{
		const std::string c_string_data(reinterpret_cast<const char*>(data.GetData()), data.Num()); // Conversion from uint8 to char
		f_string_data = FString(c_string_data.c_str()); // Conversion from TCHAR to FString
		
	}

	if (f_string_data != "")
	{
		TArray<FString> usernames;
		TArray<FString> parsed_messages;// = ProcessMessage(f_string_data, usernames);
		f_string_data.ParseIntoArrayLines(parsed_messages);

		for (int32 cycle_index = 0; cycle_index < parsed_messages.Num(); cycle_index++)
		{
			FString currentMessage = parsed_messages[cycle_index];
			FTwitchIrcMessage StructuredMessage = ExtractMessageDetails(currentMessage);

			TArray<FString> commandOptions;
			FString command = GetCommandString(StructuredMessage.Message, commandOptions);

			if (command != "" & !command.IsEmpty())
			{
				FOnCommandReceived* registered_command = boundEvents.Find(command);

				if (registered_command != nullptr)
				{
					registered_command->ExecuteIfBound(command, commandOptions, StructuredMessage.DisplayName);

					continue;
				}
			}

			if (StructuredMessage.MessageType == "PRIVMSG")
			{
				OnMessageReceived.Broadcast(StructuredMessage); // Fires the message reception event
			}
			else if(StructuredMessage.MessageType == "PRIVMSG" & !StructuredMessage.Bits.IsEmpty())
			{
				OnBitsMessageReceived.Broadcast(StructuredMessage);
			}
			else if (StructuredMessage.MessageType == "ROOMSTATE")
			{
				OnRoomstateReceived.Broadcast(StructuredMessage);
			}
			else if (StructuredMessage.MessageType == "USERSTATE")
			{
				OnRoomstateReceived.Broadcast(StructuredMessage);
			}
			else if (StructuredMessage.MessageType == "NOTICE")
			{
				OnNoticeReceived.Broadcast(StructuredMessage);
			}
			else if (StructuredMessage.MessageType == "CLEARCHAT")
			{
				OnClearChatReceived.Broadcast(StructuredMessage);
			}
			else if (StructuredMessage.MessageType == "CLEARMSG")
			{
				OnClearMsgReceived.Broadcast(StructuredMessage);
			}
			else if (StructuredMessage.MessageType == "HOSTTARGET")
			{
				OnHostTargetReceived.Broadcast(StructuredMessage);
			}
			else if (StructuredMessage.MessageType == "RECONNECT")
			{
				OnReconnectReceived.Broadcast(StructuredMessage);
			}
			else if (StructuredMessage.MessageType == "USERNOTICE")
			{
				OnUserNoticeReceived.Broadcast(StructuredMessage);
			}
			else if (StructuredMessage.MessageType == "JOIN")
			{
				OnJoinReceived.Broadcast(StructuredMessage);
			}
			else if (StructuredMessage.MessageType == "PART")
			{
				OnPartReceived.Broadcast(StructuredMessage);
			}
		}
	}
}

TArray<FString> UTwitchChat::ProcessMessage(const FString _message, TArray<FString>& outSenderUsername, bool _filterUserOnly)
{
	TArray<FString> ret_messages_content;

	TArray<FString> message_lines;
	_message.ParseIntoArrayLines(message_lines); // A single "message" from Twitch IRC could include multiple lines. Split them now

	for (int32 cycle_line = 0; cycle_line < message_lines.Num(); cycle_line++)
	{
		TArray<FString> _messageSections;
		message_lines[cycle_line].ParseIntoArray(_messageSections, TEXT(";"));


		if (message_lines[cycle_line] == "PING :tmi.twitch.tv")
		{
			SendMessage("PONG :tmi.twitch.tv", false);
			continue; // Skip line parsing
		}

		TArray<FString> message_parts;
		message_lines[cycle_line].ParseIntoArray(message_parts, TEXT(":"));

		TArray<FString> meta;
		message_parts[0].ParseIntoArrayWS(meta);

		FString sender_username = "";
		if (meta[1] == "PRIVMSG") // Type of message should always be in position 1 (or at least I hope so)
		{
			meta[0].Split("!", &sender_username, nullptr);
		}

		if (_filterUserOnly && sender_username == "")
		{
			continue; // Skip line
		}

		if (message_parts.Num() > 1)
		{
			FString message_content = message_parts[1];
			if (message_parts.Num() > 2)
			{
				for (int32 cycle_content = 2; cycle_content < message_parts.Num(); cycle_content++)
				{
					message_content += ":" + message_parts[cycle_content];
				}
			}
			ret_messages_content.Add(message_content);
			outSenderUsername.Add(sender_username);
		}
	}
	return ret_messages_content;
}

bool UTwitchChat::AuthenticateTwitchChat(FString& _result)
{
	if (Socket == nullptr)
	{
		_result = "Connection is not initialized. Call 'Connect' before authenticating";
		return false;
	}

	if (!Init)
	{
		_result = "Can't authenticate. Setup user info first";
		return false;
	}

	bool pass_ok = SendMessage("PASS " + OAuthTokenType + ":" + OAuthToken, false);
	bool nick_ok = SendMessage("NICK " + Username, false);

	bool join_ok = true;
	if (Channel != "")
	{
		join_ok = SendMessage("JOIN #" + Channel, false);
	}

	bool memb_ok = SendMessage("CAP REQ :twitch.tv/membership", false);
	bool tag_ok = SendMessage("CAP REQ :twitch.tv/tags", false);
	bool com_ok = SendMessage("CAP REQ :twitch.tv/commands", false);

	bool b_success = pass_ok && nick_ok && join_ok;

	if (!b_success)
	{
		_result = "Failed to send authentication message";
		return false;
	}

	return (b_success);
}

bool UTwitchChat::RegisterCommand(const FString _command_name, const FOnCommandReceived& _callback_function, FString& _out_result)
{
	if (_command_name == "")
	{
		_out_result = "Command type string is invalid";
		return false;
	}

	FOnCommandReceived* registered_command = boundEvents.Find(_command_name);

	if (registered_command != nullptr)
	{
		*registered_command = _callback_function;
		_out_result = _command_name + " command registered. It overwrote a previous registration of the same type";
		return true;
	}
	else
	{
		boundEvents.Add(_command_name, _callback_function);
		_out_result = _command_name + " command registered";
		return true;
	}
}

bool UTwitchChat::UnregisterCommand(const FString _command_name, FString& _out_result)
{
	if (_command_name == "")
	{
		_out_result = "Command type string is invalid";
		return false;
	}

	if (boundEvents.Remove(_command_name) == 0)
	{
		_out_result = "No command of this type was registered";
		return false;
	}
	else
	{
		_out_result = _command_name + " unregistered";
		return true;
	}
}

FString UTwitchChat::GetCommandString(const FString& _message, TArray<FString>& outOptions)
{
	FString ret_delimited_string = "";
	if (_message == "")
	{
		return "";
	}

	if (_message[0] != CommandCharacter[0])
	{
		return "";
	}

	FString PrepString = _message.Mid(1, _message.Len() - 1);

	PrepString.ParseIntoArray(outOptions, *OptionsCharacter);

	FString command = outOptions[0];
	outOptions.RemoveAt(0);

	return command;
}

void UTwitchChat::SetupCommandCharacters(const FString _commandChar, const FString _optionsChar)
{
	CommandCharacter = _commandChar;
	OptionsCharacter = _optionsChar;
}

FTwitchIrcMessage UTwitchChat::ExtractMessageDetails(const FString& _rawMessage)
{
	FTwitchIrcMessage resultMessage;
	TArray<FString> _messageSections;
	_rawMessage.ParseIntoArray(_messageSections, TEXT(":"));
	
	TArray<FString> TagsSection;
	TArray<FString> UserSection;

	if (_rawMessage == "PING :tmi.twitch.tv")
	{
		SendMessage("PONG :tmi.twitch.tv", false);
		return resultMessage; // Skip line parsing
	}

	if (_messageSections.Num() > 0)
	{
		int index = -1;
		for (int i = 0; i < _messageSections.Num(); ++i)
		{
			index = _messageSections[i].Find("#"+Channel);

			if (index != INDEX_NONE)
			{
				index = i;
				break;
			}
		}

		if (index == INDEX_NONE)
		{
			return resultMessage;
		}

		FString TagsPart = "";

		if (index > 0)
		{
			for (int i = 0; i < index; ++i)
			{
				if (i == 0)
				{
					TagsPart = _messageSections[i];
				}
				else
				{
					TagsPart = TagsPart + ":" + _messageSections[i];
				}
			}
		}

		if (index < _messageSections.Num() - 1)
		{
			for (int i = index + 1; i < _messageSections.Num(); ++i)
			{
				if (i == index + 1)
				{
					resultMessage.Message = _messageSections[i];
				}
				else
				{
					resultMessage.Message = resultMessage.Message + ":" + _messageSections[i];
				}
			}
		}

		

		TagsPart.ParseIntoArray(TagsSection, TEXT(";"));
		_messageSections[index].ParseIntoArray(UserSection, TEXT(" "));
		
		resultMessage.UserIRCLogin = UserSection[0];
		resultMessage.MessageType = UserSection[1];
		resultMessage.ChannelName = UserSection[2].Replace(TEXT("#"), TEXT(""));
	}
	else
	{
		resultMessage.DisplayName = "UNKNOWN";
		resultMessage.Message = _rawMessage;
	}


	for (int32 section = 0; section < TagsSection.Num(); section++)
	{
		TArray<FString> sectionParts;
		TagsSection[section].ParseIntoArray(sectionParts, TEXT("="));
		int index = -1;

		if (sectionParts.Num() > 1)
		{
			if (sectionParts[0] == "@badge-info")
			{
				resultMessage.BadgeInfo = sectionParts[1];
			}
			if (sectionParts[0] == "badges")
			{
				resultMessage.Badges = sectionParts[1];
			}
			if (sectionParts[0] == "color")
			{
				resultMessage.Color = sectionParts[1];
			}
			if (sectionParts[0] == "display-name")
			{
				resultMessage.DisplayName = sectionParts[1];
			}
			if (sectionParts[0] == "emotes")
			{
				resultMessage.Emotes = sectionParts[1];
			}
			if (sectionParts[0] == "flags")
			{
				resultMessage.Flags = sectionParts[1];
			}
			if (sectionParts[0] == "id")
			{
				resultMessage.Id = sectionParts[1];
			}
			if (sectionParts[0] == "mod")
			{
				resultMessage.Mod = sectionParts[1];
			}
			if (sectionParts[0] == "room-id")
			{
				resultMessage.RoomId = sectionParts[1];
			}
			if (sectionParts[0] == "subscriber")
			{
				resultMessage.Subscriber = sectionParts[1];
			}
			if (sectionParts[0] == "tmi-sent-ts")
			{
				resultMessage.MessageTimeStamp = sectionParts[1];
			}
			if (sectionParts[0] == "turbo")
			{
				resultMessage.Turbo = sectionParts[1];
			}
			if (sectionParts[0] == "user-id")
			{
				resultMessage.UserId = sectionParts[1];
			}
			if (sectionParts[0] == "user-type")
			{
				resultMessage.UserType = sectionParts[1];
			}
			if (sectionParts[0] == "bits")
			{
				resultMessage.Bits = sectionParts[1];
			}
		}
	}

	return resultMessage;
}