// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoreTypes.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "Components/ActorComponent.h"
#include "Networking.h"
#include "Engine/World.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "TwitchInteractionComponent.generated.h"


USTRUCT(BlueprintType)
struct TWITCHINTERACTION_API FTwitchIrcMessage
{
	GENERATED_USTRUCT_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString BadgeInfo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString Badges;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString Color;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString Emotes;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString Flags;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString Id;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString Mod;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString RoomId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString Subscriber;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString MessageTimeStamp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString Turbo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString UserId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString UserType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString UserIRCLogin;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString MessageType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString ChannelName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString Bits;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString Message;
};

DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnCommandReceived, const FString&, commandName, const TArray<FString>&, commandOptions, const FString&, senderUsername);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMessageReceived, const FTwitchIrcMessage&, message);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TWITCHINTERACTION_API UTwitchInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTwitchInteractionComponent();
	bool Init;
	
	UPROPERTY(BlueprintAssignable, Category = "Message Events")
		FMessageReceived OnMessageReceived;

	UPROPERTY(BlueprintAssignable, Category = "Message Events")
		FMessageReceived OnBitsMessageReceived;

	UPROPERTY(BlueprintAssignable, Category = "Message Events")
		FMessageReceived OnNoticeReceived;

	UPROPERTY(BlueprintAssignable, Category = "Message Events")
		FMessageReceived OnRoomstateReceived;

	UPROPERTY(BlueprintAssignable, Category = "Message Events")
		FMessageReceived OnUserstateReceived;
	UPROPERTY(BlueprintAssignable, Category = "Message Events")
		FMessageReceived OnUserNoticeReceived;
	UPROPERTY(BlueprintAssignable, Category = "Message Events")
		FMessageReceived OnReconnectReceived;
	UPROPERTY(BlueprintAssignable, Category = "Message Events")
		FMessageReceived OnHostTargetReceived;
	UPROPERTY(BlueprintAssignable, Category = "Message Events")
		FMessageReceived OnClearMsgReceived;
	UPROPERTY(BlueprintAssignable, Category = "Message Events")
		FMessageReceived OnClearChatReceived;
	UPROPERTY(BlueprintAssignable, Category = "Message Events")
		FMessageReceived OnJoinReceived;
	UPROPERTY(BlueprintAssignable, Category = "Message Events")
		FMessageReceived OnPartReceived;


	// Authentication token. Need to get it from official Twitch API
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
		FString OAuthToken;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
		FString OAuthTokenType;

	// Username. Must be in lowercaps
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
		FString Username;

	// Channel to join upon successful connection	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
		FString Channel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Commands Setup")
		FString CommandCharacter = "!";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Commands Setup")
		FString OptionsCharacter = " ";

	UFUNCTION(BlueprintCallable, Category = "Setup")
		void SetUserInfo(const FString _oauth, const FString _authType,  const  FString _username, const FString _channel);

	UFUNCTION(BlueprintCallable, Category = "Messages")
		bool SendMessage(FString _message, UPARAM(DisplayName = "Send Message") bool _sendTo = true, FString _channel = "");

	UFUNCTION(BlueprintCallable, Category = "Setup")
		bool Connect(FString& result);
	UFUNCTION(BlueprintCallable, Category = "Setup")
		bool AuthenticateTwitchChat(FString& _result);
	void ReceiveData();

	TArray<FString> ProcessMessage(const FString _message, TArray<FString>& outSenderUsername, bool _filterUserOnly = false);


	UFUNCTION(BlueprintCallable, Category = "Commands Setup")
		void SetupCommandCharacters(const FString _commandChar, const FString _optionsChar);
	UFUNCTION(BlueprintCallable, Category = "Commands Setup")
		bool RegisterCommand(const FString _commandName, const FOnCommandReceived& _callbackFunction, FString& _result);
	UFUNCTION(BlueprintCallable, Category = "Commands Setup")
		bool UnregisterCommand(const FString _commandName, FString& _result);


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
private:
	FSocket* Socket;
	FTimerHandle UpdateTimer;
	TMap<FString, FOnCommandReceived> boundEvents;
	FString GetCommandString(const FString& _message, TArray<FString>& outOptions);
	FTwitchIrcMessage ExtractMessageDetails(const FString& _rawMessage);
};
