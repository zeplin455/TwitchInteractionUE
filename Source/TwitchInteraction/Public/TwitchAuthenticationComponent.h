// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Runtime/Online/HTTPServer/Public/HttpServerModule.h"
#include "Runtime/Online/HTTPServer/Public/HttpPath.h"
#include "HttpServerRequest.h"
#include "IHttpRouter.h"
#include "Logging/LogMacros.h"
#include "Runtime/Online/HTTPServer/Public/HttpResultCallback.h"
#include "Runtime/Json/Public/Dom/JsonObject.h"
#include "Runtime/Online/HTTPServer/Public/HttpServerResponse.h"
#include "Runtime/Online/HTTPServer/Public/HttpRouteHandle.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "TwitchEventSub.h"
#include "TwitchInteractionComponent.h"
#include "CivetHttpServer.h"
#include "CivetHttpServerHandler.h"
#include "TwitchAuthenticationComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(HttpAuthLog, Log, All);

typedef TFunction<TUniquePtr<FHttpServerResponse>(const FHttpServerRequest& Request)> FHttpResponser;

class UTwitchEventSub;
class UTwitchInteractionComponent;

USTRUCT(BlueprintType)
struct FTwitchAuthUserInfoData
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString id;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString login;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString display_name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString type;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString broadcaster_type;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString profile_image_url;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString offline_image_url;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		int32 view_count;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString email;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		FString created_at;

};

USTRUCT(BlueprintType)
struct FTwitchAuthUserInfo
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
		TArray<FTwitchAuthUserInfoData> data;
};

class FBaseHandler
{
public:
	static TUniquePtr<FHttpServerResponse> AuthToken(const FHttpServerRequest& Request);


};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTokenReceived, const FString&, token);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUserInfoReceived, const FString&, userId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FValidateTokenReceived, bool, success);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TWITCHINTERACTION_API UTwitchAuthenticationComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTwitchAuthenticationComponent();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
		int listenPort = 8080;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
		FString redirectUrl = "http://localhost:8080/Auth";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
		FString userid;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
		FString username;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
		FString clientId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
		FString token;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
		UTwitchEventSub* EventSubComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
		UTwitchInteractionComponent* TwitchChatComponent;

	UPROPERTY(BlueprintAssignable, Category = "Message Events")
		FTokenReceived OnTokenReceived;

	UPROPERTY(BlueprintAssignable, Category = "Message Events")
		FUserInfoReceived OnUserIdReceived;

	UPROPERTY(BlueprintAssignable, Category = "Message Events")
		FValidateTokenReceived OnTokenValidReceived;

	//Global static storage because only on http listener can exist on a port so it must apply to all existing components
	static TArray<UTwitchInteractionComponent*> GlobalTwitchChatComponents;
	static TArray<UTwitchEventSub*> GlobalEventSubComponents;
	static FTokenReceived GlobalTokenReceived;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Setup")
	bool FetchUserInfo(FString _username, FString _twitchToken);
	UFUNCTION(BlueprintCallable, Category = "Setup")
	void CheckTokenValid();
	UFUNCTION(BlueprintCallable, Category = "Setup")
		void Init();

private:
	static void BindRouters(const TSharedPtr<IHttpRouter>& HttpRouter);
	void ProcessGetUserInfo(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void ProcessTokenValidateInfo(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	
		
};


class FWebUtil
{
public:
	/**
	 * Bind a route with handler
	 */
	static FHttpRouteHandle BindRoute(const TSharedPtr<IHttpRouter>& HttpRouter, FString Path, const EHttpServerRequestVerbs& Verb, const FHttpResponser& HttpResponser);

	/**
	 * Create HTTP request handler (controller)
	 * In UE4, invoke OnComplete and return false will cause crash
	 * CreateHandler method is used to wrap the responser, in order to avoid the crash
	 */
	static FHttpRequestHandler CreateHandler(const FHttpResponser& HttpResponser);

	/**
	 * Get request json body, parse TArray<uint8> to TSharedPtr<FJsonObject>
	 */
	static TSharedPtr<FJsonObject> GetRequestJsonBody(const FHttpServerRequest& Request);

	/**
	 * Get Struct body (based on json body, the struct type should be UStruct)
	 */
	template <typename UStructType>
	static UStructType* GetRequestUStructBody(const FHttpServerRequest& Request);

	/**
	 * Success response (data & message)
	 */
	static TUniquePtr<FHttpServerResponse> SuccessResponse(TSharedPtr<FJsonObject> Data, FString Message);

	/**
	 * Success response (data only)
	 */
	static TUniquePtr<FHttpServerResponse> SuccessResponse(TSharedPtr<FJsonObject> Data);

	/**
	 * Success response (message only)
	 */
	static TUniquePtr<FHttpServerResponse> SuccessResponse(FString Message);

	/**
	 * Error response (data & message & code)
	 */
	static TUniquePtr<FHttpServerResponse> ErrorResponse(TSharedPtr<FJsonObject> Data, FString Message, int32 Code);

	/**
	 * Error response (data & message)
	 */
	static TUniquePtr<FHttpServerResponse> ErrorResponse(TSharedPtr<FJsonObject> Data, FString Message);

	/**
	 * Error response (message & code)
	 */
	static TUniquePtr<FHttpServerResponse> ErrorResponse(FString Message, int32 Code);

	/**
	 * Error response (message only)
	 */
	static TUniquePtr<FHttpServerResponse> ErrorResponse(FString Message);
	static TUniquePtr<FHttpServerResponse> JsResponse(bool Success, int32 Code);
	static TUniquePtr<FHttpServerResponse> OkResponse(bool Success, int32 Code);
private:
	/* Success code in response body */
	static const int32 SUCCESS_CODE = 0;
	/* Default error code in response body */
	static const int32 DEFAULT_ERROR_CODE = -1;

	/**
	 * get verb string from enumerate (for logging use)
	 */
	static FString GetHttpVerbStringFromEnum(const EHttpServerRequestVerbs& Verb);

	/**
	 * Create json response from data, message, success status and user defined error code
	 */
	static TUniquePtr<FHttpServerResponse> JsonResponse(TSharedPtr<FJsonObject> Data, FString Message, bool Success, int32 Code);

	

	/**
	 * Check if the body content will be parsed as UTF-8 json by header
	 */
	static bool IsUTF8JsonRequestContent(const FHttpServerRequest& Request);
};