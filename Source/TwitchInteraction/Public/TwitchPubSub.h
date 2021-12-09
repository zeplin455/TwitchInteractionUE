// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Networking.h"
#include "Misc/DateTime.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "WebSocketsModule.h"
#include "IWebSocket.h"  
#include "TwitchPubSub.generated.h"

USTRUCT(BlueprintType)
struct FTwitchEventSubscribeDataMessageEmotes
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		int32 start;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		int32 end;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		int32 id;
};

USTRUCT(BlueprintType)
struct FTwitchEventSubscribeDataMessage
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString message;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		TArray<FTwitchEventSubscribeDataMessageEmotes> emotes;

};

USTRUCT(BlueprintType)
struct FTwitchEventSubscribeMessage
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString channel_id;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString channel_name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString time;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString sub_plan;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString sub_plan_name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		int32 months;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString context;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		bool is_gift;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString recipient_id;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString recipient_user_name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString recipient_display_name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString cumulative_months;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString streak_months;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		int32 multi_month_duration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString display_name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString user_name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString user_id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FTwitchEventSubscribeDataMessage sub_message;
};

USTRUCT(BlueprintType)
struct FTwitchEventSubscribeData
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString topic;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FTwitchEventSubscribeMessage message;
};

USTRUCT(BlueprintType)
struct FTwitchEventSubscribe
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString type;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FTwitchEventSubscribeData data;
};

USTRUCT(BlueprintType)
struct FTwitchEventRedeemRedemptionUser
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString id;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString login;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString display_name;
};

USTRUCT(BlueprintType)
struct FTwitchEventRedeemRedemption
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString id;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FTwitchEventRedeemRedemptionUser user;
};

USTRUCT(BlueprintType)
struct FTwitchEventRedeemRewardMaxPreStream
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		bool is_enabled;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		int32 max_per_stream;
};

USTRUCT(BlueprintType)
struct FTwitchEventRedeemRewardImages
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString url_1x;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString url_2x;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString url_4x;
};

USTRUCT(BlueprintType)
struct FTwitchEventRedeemReward
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString id;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString channel_id;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString title;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString prompt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		int32 cost;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		bool is_user_input_required;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		bool is_sub_only;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FTwitchEventRedeemRewardImages image;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FTwitchEventRedeemRewardImages default_image;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString background_color;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		bool is_enabled;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		bool is_paused;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		bool is_in_stock;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		bool should_redemptions_skip_request_queue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FTwitchEventRedeemRewardMaxPreStream max_per_stream;
};

USTRUCT(BlueprintType)
struct FTwitchEventRedeemData
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString timestamp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString channel_id;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString redeemed_at;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString user_input;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString status;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FTwitchEventRedeemRedemption redemption;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FTwitchEventRedeemReward reward;
};

USTRUCT(BlueprintType)
struct FTwitchEventRedeemMessage
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString type;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FTwitchEventRedeemData data;
};

USTRUCT(BlueprintType)
struct FTwitchEventRedeemRoot
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString type;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FTwitchEventRedeemMessage data;
};

USTRUCT(BlueprintType)
struct FTwitchEventRedeemDataRoot
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString topic;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FTwitchEventRedeemData message;
};

USTRUCT(BlueprintType)
struct FTwitchEventBitsBadge
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString user_id;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString user_name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString channel_id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString channel_name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString badge_tier;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString chat_message;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString time;
};
USTRUCT(BlueprintType)
struct FTwitchEventBitsBadgeMessage
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString topic;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FTwitchEventBitsBadge message;
};

USTRUCT(BlueprintType)
struct FTwitchEventBitsBadgeRoot
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString type;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FTwitchEventBitsBadgeMessage data;
};

USTRUCT(BlueprintType)
struct FTwitchEventBadgeEntitlemmentData
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		int32 new_version;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		int32 previous_version;
};

USTRUCT(BlueprintType)
struct FTwitchEventBitsData
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		int32 bits_used;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString channel_id;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString chat_message;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString context;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		bool is_anonymous;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString message_id;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString message_type;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString time;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		int32 total_bits_used;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString user_id;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString user_name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString version;
};



USTRUCT(BlueprintType)
struct FTwitchEventBits
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FTwitchEventBitsData data;
};

USTRUCT(BlueprintType)
struct FTwitchEventBitsMessage
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
		FString topic;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FTwitchEventBits message;
};

USTRUCT()
struct FTwitchEventBitsRoot
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
		FString type;
	UPROPERTY()
		FTwitchEventBitsMessage data;
};

USTRUCT()
struct FTwitchEventSubRequestData
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
		TArray<FString> topics;
	UPROPERTY()
		FString auth_token;
};

USTRUCT()
struct FTwitchEventSubRequest
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
		FString type;
	UPROPERTY()
		FString nonce;
	UPROPERTY()
		FTwitchEventSubRequestData data;
};

USTRUCT()
struct FTwitchMessageData
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
		FString topic;
	UPROPERTY()
		FString message;
};

USTRUCT()
struct FTwitchMessage
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
		FString type;
	UPROPERTY()
		FString nonce;
	UPROPERTY()
		FString error;
	UPROPERTY()
		FTwitchMessageData data;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBitsEventReceived, const FTwitchEventBitsData&, bitsEventInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBitsBadgeEventReceived, const FTwitchEventBitsBadge&, bitsBadgeEventInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRedeemEventReceived, const FTwitchEventRedeemData&, redeemEventInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSubscribeEventReceived, const FTwitchEventSubscribeMessage&, subscribeEventInfo);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TWITCHINTERACTION_API UTwitchPubSub : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTwitchPubSub();
	bool Init;

	UPROPERTY()
	FString channelId;
	UPROPERTY()
	FString authToken;
	UPROPERTY()
	FString authType;
	UPROPERTY()
	TArray<FString> listenTopics;


protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void ProcessMessage(const FString _jsonStr);
	void RequestEventSubs();
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Setup")
	void SetInfo(const FString _oauth, const FString _authType = "Bearer", const FString _channelId = "0");

	UFUNCTION(BlueprintCallable, Category = "Setup")
	bool Connect(FString& result);
	UFUNCTION(BlueprintCallable, Category = "Setup")
	bool Disconnect();

	UFUNCTION(BlueprintCallable, Category = "Setup")
	bool SendMessage(FString _message);
	void UpdatePing();


	UPROPERTY(BlueprintAssignable, Category = "Message Events")
		FBitsEventReceived OnBitsEventReceived;
	UPROPERTY(BlueprintAssignable, Category = "Message Events")
		FBitsBadgeEventReceived OnBitsBadgeEventReceived;

	UPROPERTY(BlueprintAssignable, Category = "Message Events")
		FRedeemEventReceived OnRedeemEventReceived;

	UPROPERTY(BlueprintAssignable, Category = "Message Events")
		FSubscribeEventReceived OnSubscribeEventReceived;

private:
	uint32 requestCounter = 0;
	TSharedPtr<IWebSocket> Socket;
	FTimerHandle UpdateTimer;
	uint32 pingTicker = 0;
	FDateTime LastUpdate;
		
};
