// Fill out your copyright notice in the Description page of Project Settings.

#include "TwitchEventSub.h"
#include "Serialization/JsonSerializer.h"
#include "JsonObjectConverter.h"

// Sets default values for this component's properties
UTwitchEventSub::UTwitchEventSub()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UTwitchEventSub::BeginPlay()
{
	Super::BeginPlay();

	// ...
	FModuleManager::Get().LoadModuleChecked("WebSockets");
}

void UTwitchEventSub::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Disconnect();
}


// Called every frame
void UTwitchEventSub::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UTwitchEventSub::SetInfo(const FString _oauth, const FString _authType, const FString _channelId)
{
	authToken = _oauth;
	authType = _authType;
	channelId = _channelId;
	Init = true;
}

bool UTwitchEventSub::Connect(FString& result)
{
	const FString ServerURL = TEXT("wss://pubsub-edge.twitch.tv:443/"); // Your server URL. You can use ws, wss or wss+insecure.
	const FString ServerProtocol = TEXT("wss");


	Socket = FWebSocketsModule::Get().CreateWebSocket(ServerURL, ServerProtocol);

	Socket->OnConnected().AddLambda([this]() -> void {
		// This code will run once connected.
			UE_LOG(LogTemp, Warning, TEXT("Connected to twitch pubsub"));
			SendMessage("{\"type\":\"PING\"}");
			RequestEventSubs();

			GetWorld()->GetTimerManager().SetTimer(UpdateTimer, this, &UTwitchEventSub::UpdatePing, 10.f, true);
		});

	Socket->OnConnectionError().AddLambda([](const FString& Error) -> void {
		// This code will run if the connection failed. Check Error to see what happened.
		});

	Socket->OnClosed().AddLambda([](int32 StatusCode, const FString& Reason, bool bWasClean) -> void {
		// This code will run when the connection to the server has been terminated.
		// Because of an error or a call to Socket->Close().
		});

	Socket->OnMessage().AddLambda([this](const FString& Message) -> void {
			ProcessMessage(Message);
		});

	Socket->OnRawMessage().AddLambda([](const void* Data, SIZE_T Size, SIZE_T BytesRemaining) -> void {
		// This code will run when we receive a raw (binary) message from the server.
		});

	Socket->OnMessageSent().AddLambda([](const FString& MessageString) -> void {
		// This code is called after we sent a message to the server.
		});


	Socket->Connect();

	//if (Socket->IsConnected())
	//{
	//	RequestEventSubs();
	//	return true;
	//}
	//else
	//{
	//	return false;
	//}
	return true;
}

bool UTwitchEventSub::Disconnect()
{
	if (Socket != nullptr && Socket->IsConnected())
	{
		Socket->Close();
	}

	return true;
}

bool UTwitchEventSub::SendMessage(FString _message)
{
	if (Socket != nullptr && Socket->IsConnected())
	{
		UE_LOG(LogTemp, Warning, TEXT("SEND - %s"), *_message);
		Socket->Send(_message);
		return true;
	}
	else
	{
		return false;
	}
}

void UTwitchEventSub::UpdatePing()
{
	if (Socket == nullptr && Socket->IsConnected())
	{
		return;
	}

	pingTicker += 10;

	//ping every 1 minute
	if (pingTicker >= 60)
	{
		pingTicker = 0;
		LastUpdate = FDateTime::Now();
		SendMessage("{\"type\":\"PING\"}");
	}
}

void UTwitchEventSub::RequestEventSubs()
{
	FTwitchEventSubRequest requestInfo;
	requestInfo.type = "LISTEN";
	requestInfo.nonce = FString::FromInt(requestCounter++);
	requestInfo.data.auth_token = authToken;

	//requestInfo.data.topics = listenTopics;
	requestInfo.data.topics.Add(FString("channel-bits-events-v2.") + FString(channelId));
	requestInfo.data.topics.Add(FString("channel-bits-badge-unlocks.") + FString(channelId));
	requestInfo.data.topics.Add(FString("channel-points-channel-v1.") + FString(channelId));
	requestInfo.data.topics.Add(FString("channel-subscribe-events-v1.") + FString(channelId));
	

	FString result;
	FJsonObjectConverter::UStructToJsonObjectString(requestInfo, result, 0, 0, 0, nullptr, false);

	SendMessage(result);

}

void UTwitchEventSub::ProcessMessage(const FString _jsonStr)
{
	FTwitchMessage targetMessage;

	UE_LOG(LogTemp, Warning, TEXT("RECV - %s"),*_jsonStr);

	if (!FJsonObjectConverter::JsonObjectStringToUStruct(_jsonStr, &targetMessage, 0, 0))
	{
		//ERROR
	}

	if (targetMessage.type == "PONG")
	{
		//Pong received
	}

	if (targetMessage.type == "RESPONSE")
	{

	}

	if (targetMessage.type == "MESSAGE")
	{
		if (targetMessage.data.topic.StartsWith("channel-bits-events-v2"))
		{
			FTwitchEventBits twitchEventBitsMessage;
			FJsonObjectConverter::JsonObjectStringToUStruct(targetMessage.data.message, &twitchEventBitsMessage, 0, 0);
			OnBitsEventReceived.Broadcast(twitchEventBitsMessage);
		}

		if (targetMessage.data.topic.StartsWith("channel-bits-badge-unlocks"))
		{
			FTwitchEventBitsBadge twitchEventBitsBadgeMessage;
			FJsonObjectConverter::JsonObjectStringToUStruct(targetMessage.data.message, &twitchEventBitsBadgeMessage, 0, 0);
			OnBitsBadgeEventReceived.Broadcast(twitchEventBitsBadgeMessage);
		}

		if (targetMessage.data.topic.StartsWith("channel-subscribe-events-v1"))
		{
			FTwitchEventSubscribe twitchSubscribeMessage;
			FJsonObjectConverter::JsonObjectStringToUStruct(_jsonStr, &twitchSubscribeMessage, 0, 0);
			OnSubscribeEventReceived.Broadcast(twitchSubscribeMessage.data.message);
		}
	}

	if (targetMessage.type == "reward-redeemed")
	{
		FTwitchEventRedeem twitchRedeemMessage;
		FJsonObjectConverter::JsonObjectStringToUStruct(_jsonStr, &twitchRedeemMessage, 0, 0);
		OnRedeemEventReceived.Broadcast(twitchRedeemMessage);
	}
}

