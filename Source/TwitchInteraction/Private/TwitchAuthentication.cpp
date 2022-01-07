// Copyright (c) 2021 Zeplin455.

#include "TwitchAuthentication.h"
#include "Serialization/JsonSerializer.h"
#include "JsonObjectConverter.h"

DEFINE_LOG_CATEGORY(HttpAuthLog);

TArray<UTwitchChat*> UTwitchAuthentication::GlobalTwitchChatComponents;
TArray<UTwitchPubSub*> UTwitchAuthentication::GlobalEventSubComponents;
FTokenReceived UTwitchAuthentication::GlobalTokenReceived;

// Sets default values for this component's properties
UTwitchAuthentication::UTwitchAuthentication()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UTwitchAuthentication::BeginPlay()
{
	Super::BeginPlay();

	
}

void UTwitchAuthentication::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	auto HttpServerModule = &FHttpServerModule::Get();
	HttpServerModule->StopAllListeners();
}


// Called every frame
void UTwitchAuthentication::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UTwitchAuthentication::FetchUserInfo(FString _username, FString _twitchToken)
{
	FHttpModule* Http = &FHttpModule::Get();

	FHttpRequestRef Request = Http->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UTwitchAuthentication::ProcessGetUserInfo);
	//This is the url on which to process the request
	Request->SetURL("https://api.twitch.tv/helix/users?login=" + _username);
	Request->SetVerb("GET");
	Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
	Request->SetHeader("Authorization", FString("Bearer ") + _twitchToken);
	Request->SetHeader("Client-Id", clientId);
	Request->ProcessRequest();

	return true;
}

void UTwitchAuthentication::CheckTokenValid()
{
	FHttpModule* Http = &FHttpModule::Get();

	FHttpRequestRef Request = Http->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UTwitchAuthentication::ProcessGetUserInfo);
	//This is the url on which to process the request
	Request->SetURL("https://api.twitch.tv/helix/users?login=" + username);
	Request->SetVerb("GET");
	Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
	Request->SetHeader("Authorization", FString("Bearer ") + token);
	Request->SetHeader("Client-Id", clientId);
	Request->ProcessRequest();
}

void UTwitchAuthentication::Init()
{
	if (TwitchChatComponent != nullptr)
	{
		UTwitchAuthentication::GlobalTwitchChatComponents.Add(TwitchChatComponent);
	}

	if (EventSubComponent != nullptr)
	{
		UTwitchAuthentication::GlobalEventSubComponents.Add(EventSubComponent);
	}

	GlobalTokenReceived = OnTokenReceived;

	FModuleManager::Get().LoadModuleChecked("HttpServer");

	auto HttpServerModule = &FHttpServerModule::Get();

	TSharedPtr<IHttpRouter> HttpRouter = HttpServerModule->GetHttpRouter(listenPort);
	BindRouters(HttpRouter);
	HttpServerModule->StartAllListeners();
	FString Url = FString("https://id.twitch.tv/oauth2/authorize?client_id=") + clientId + FString("&redirect_uri=") + redirectUrl + FString("&response_type=token&scope=") + FString::Join(scopes, TEXT(" "));
	FPlatformProcess::LaunchURL(*Url, NULL, NULL);
}

void UTwitchAuthentication::BindRouters(const TSharedPtr<IHttpRouter>& HttpRouter)
{
	FWebUtil::BindRoute(HttpRouter, TEXT("/Auth"), EHttpServerRequestVerbs::VERB_GET, FBaseHandler::AuthToken);
}

void UTwitchAuthentication::ProcessGetUserInfo(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		FString body = Response->GetContentAsString();
		FTwitchAuthUserInfo userInfo;
		FJsonObjectConverter::JsonObjectStringToUStruct(body, &userInfo, 0, 0);

		if (userInfo.data.Num() > 0)
		{
			OnUserIdReceived.Broadcast(userInfo.data[0].id);
		}
	}
}

void UTwitchAuthentication::ProcessTokenValidateInfo(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		FString body = Response->GetContentAsString();
		FTwitchAuthUserInfo userInfo;
		FJsonObjectConverter::JsonObjectStringToUStruct(body, &userInfo, 0, 0);

		if (userInfo.data.Num() > 0)
		{
			OnTokenValidReceived.Broadcast(true);
		}
		else
		{
			OnTokenValidReceived.Broadcast(false);
		}
	}
	else
	{
		OnTokenValidReceived.Broadcast(false);
	}
}

TUniquePtr<FHttpServerResponse> FBaseHandler::AuthToken(const FHttpServerRequest& Request)
{
	if (!Request.QueryParams.IsEmpty())
	{
		FString token = *Request.QueryParams.Find("access_token");
		UE_LOG(HttpAuthLog, Log, TEXT("Auth Token %s"), *token);

		if (UTwitchAuthentication::GlobalEventSubComponents.Num() > 0)
		{
			for (int i = 0; i < UTwitchAuthentication::GlobalEventSubComponents.Num(); ++i)
			{
				UTwitchAuthentication::GlobalEventSubComponents[i]->authToken = token;
			}
		}

		if (UTwitchAuthentication::GlobalTwitchChatComponents.Num() > 0)
		{
			for (int i = 0; i < UTwitchAuthentication::GlobalTwitchChatComponents.Num(); ++i)
			{
				UTwitchAuthentication::GlobalTwitchChatComponents[i]->OAuthToken = token;
			}
		}

		UTwitchAuthentication::GlobalTokenReceived.Broadcast(token);

		return FWebUtil::OkResponse(true, 0);
	}
	else
	{

		return FWebUtil::JsResponse(true, 0);
	}
}


FHttpRouteHandle FWebUtil::BindRoute(const TSharedPtr<IHttpRouter>& HttpRouter, FString Path, const EHttpServerRequestVerbs& Verb, const FHttpResponser& HttpResponser)
{
	// VERB_NONE not supported!
	if (HttpRouter == nullptr || Verb == EHttpServerRequestVerbs::VERB_NONE)
	{
		return nullptr;
	}

	FString VerbString = GetHttpVerbStringFromEnum(Verb);
	UE_LOG(HttpAuthLog, Log, TEXT("Binding router: %s\t%s"), *VerbString, *Path);

	// check if HTTP path is valid
	FHttpPath HttpPath(Path);
	if (!HttpPath.IsValidPath())
	{
		UE_LOG(HttpAuthLog, Warning, TEXT("Invalid http path: %s"), *Path);
#if WITH_EDITOR
		if (GEngine != nullptr)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red,
				FString::Printf(TEXT("Bind HTTP router failed! invalid path: %s"), *Path));
		}
#endif
		return nullptr;
	}

	// bind router
	auto RouteHandle = HttpRouter->BindRoute(HttpPath, Verb, FWebUtil::CreateHandler(HttpResponser));
	if (RouteHandle == nullptr)
	{
		UE_LOG(HttpAuthLog, Warning, TEXT("Bind failed: %s\t%s"), *VerbString, *Path);
		return nullptr;
	}
#if WITH_EDITOR
	if (GEngine != nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Cyan,
			FString::Printf(TEXT("Bind HTTP router: %s\t%s"), *VerbString, *Path));
	}
#endif
	return RouteHandle;
}

FHttpRequestHandler FWebUtil::CreateHandler(const FHttpResponser& HttpResponser)
{
	return [HttpResponser](const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete)
	{
		auto Response = HttpResponser(Request);
		if (Response == nullptr)
		{
			return false;
		}
		OnComplete(MoveTemp(Response));
		return true;
	};
}

TSharedPtr<FJsonObject> FWebUtil::GetRequestJsonBody(const FHttpServerRequest& Request)
{
	// check if content type is application/json
	bool IsUTF8JsonContent = IsUTF8JsonRequestContent(Request);
	if (!IsUTF8JsonContent)
	{
		UE_LOG(HttpAuthLog, Warning, TEXT("caught request not in utf-8 application/json body content!"));
		return nullptr;
	}

	// body to utf8 string
	TArray<uint8> RequestBodyBytes = Request.Body;
	FString RequestBodyString = FString(UTF8_TO_TCHAR(RequestBodyBytes.GetData()));
#if WITH_EDITOR
	if (GEngine != nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, RequestBodyString);
	}
#endif

	// string to json
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(RequestBodyString);
	TSharedPtr<FJsonObject> RequestBody;
	if (!FJsonSerializer::Deserialize(JsonReader, RequestBody))
	{
		UE_LOG(HttpAuthLog, Warning, TEXT("failed to parse request string to json: %s"), *RequestBodyString);
		return nullptr;
	}
	return RequestBody;
}

template <typename UStructType>
static UStructType* FWebUtil::GetRequestUStructBody(const FHttpServerRequest& Request)
{
	TSharedPtr<FJsonObject> JsonBody = FWebUtil::GetRequestJsonBody(Request);
	if (JsonBody == nullptr)
	{
		return nullptr;
	}
	UStructType* UStructBody;
	if (!FJsonObjectConverter::JsonObjectToUStruct(JsonBody, UStructBody))
	{
		UE_LOG(UHttpLog, Warning, TEXT("failed to parse json body to ustruct!"))
			return nullptr;
	}
	return UStructBody;
}

TUniquePtr<FHttpServerResponse> FWebUtil::SuccessResponse(TSharedPtr<FJsonObject> Data, FString Message)
{
	return JsonResponse(Data, Message, true, SUCCESS_CODE);
}

TUniquePtr<FHttpServerResponse> FWebUtil::SuccessResponse(TSharedPtr<FJsonObject> Data)
{
	return SuccessResponse(Data, TEXT(""));
}

TUniquePtr<FHttpServerResponse> FWebUtil::SuccessResponse(FString Message)
{
	return SuccessResponse(MakeShareable(new FJsonObject()), Message);
}

TUniquePtr<FHttpServerResponse> FWebUtil::ErrorResponse(TSharedPtr<FJsonObject> Data, FString Message, int32 Code)
{
	if (Code == SUCCESS_CODE)
	{
		Code = DEFAULT_ERROR_CODE;
	}
	return JsonResponse(Data, Message, false, Code);
}

TUniquePtr<FHttpServerResponse> FWebUtil::ErrorResponse(TSharedPtr<FJsonObject> Data, FString Message)
{
	return ErrorResponse(Data, Message, DEFAULT_ERROR_CODE);
}

TUniquePtr<FHttpServerResponse> FWebUtil::ErrorResponse(FString Message, int32 Code)
{
	return ErrorResponse(MakeShareable(new FJsonObject()), Message, Code);
}

TUniquePtr<FHttpServerResponse> FWebUtil::ErrorResponse(FString Message)
{
	return ErrorResponse(MakeShareable(new FJsonObject()), Message, DEFAULT_ERROR_CODE);
}

/** ========================== Private Methods ======================= */

FString FWebUtil::GetHttpVerbStringFromEnum(const EHttpServerRequestVerbs& Verb)
{
	switch (Verb)
	{
	case EHttpServerRequestVerbs::VERB_GET:
		return TEXT("GET");
	case EHttpServerRequestVerbs::VERB_POST:
		return TEXT("POST");
	case EHttpServerRequestVerbs::VERB_PUT:
		return TEXT("PUT");
	case EHttpServerRequestVerbs::VERB_DELETE:
		return TEXT("DELETE");
	case EHttpServerRequestVerbs::VERB_PATCH:
		return TEXT("PATCH");
	case EHttpServerRequestVerbs::VERB_OPTIONS:
		return TEXT("OPTIONS");
	default:
		return TEXT("UNKNOWN_VERB");
	}
}

TUniquePtr<FHttpServerResponse> FWebUtil::JsonResponse(TSharedPtr<FJsonObject> Data, FString Message, bool Success, int32 Code)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	JsonObject->SetObjectField(TEXT("data"), Data);
	JsonObject->SetStringField(TEXT("message"), Message);
	JsonObject->SetBoolField(TEXT("success"), Success);
	JsonObject->SetNumberField(TEXT("code"), (double)Code);
	FString JsonString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
	return FHttpServerResponse::Create(JsonString, TEXT("application/json"));
}

TUniquePtr<FHttpServerResponse> FWebUtil::JsResponse(bool Success, int32 Code)
{
	FString PageResponse = "<!DOCTYPE html PUBLIC\"ISO / IEC 15445:2000//DTD HTML//EN\"><html><head><title>UE App</title></head><body><p>Attempting to grab token...</p></body><script>window.onload=function(){window.location.href=window.location.href.replace('#','?');}</script></html>";
	return FHttpServerResponse::Create(PageResponse, TEXT("text/html"));
}

TUniquePtr<FHttpServerResponse> FWebUtil::OkResponse(bool Success, int32 Code)
{
	FString PageResponse = "<!DOCTYPE html PUBLIC\"ISO / IEC 15445:2000//DTD HTML//EN\"><html><head><title>UE App</title></head><body><p>Token received...</p></body><script>window.onload=function(){window.close();}</script></html>";
	return FHttpServerResponse::Create(PageResponse, TEXT("text/html"));
}

bool FWebUtil::IsUTF8JsonRequestContent(const FHttpServerRequest& Request)
{
	bool bIsUTF8JsonContent = false;
	for (auto& HeaderElem : Request.Headers)
	{
		auto LowerKey = HeaderElem.Key.ToLower();
		if (LowerKey == TEXT("content-type"))
		{
			for (auto& Value : HeaderElem.Value)
			{
				auto LowerValue = Value.ToLower();
				// not strict check
				if (LowerValue.Contains(TEXT("charset=")) && !LowerValue.Contains(TEXT("charset=utf-8")))
				{
					return false;
				}
				if (LowerValue.Contains(TEXT("application/json")) || LowerValue.Contains(TEXT("text/json")))
				{
					bIsUTF8JsonContent = true;
				}
			}
		}
#if WITH_EDITOR
		auto Value = FString::Join(HeaderElem.Value, TEXT(","));
		FString Message;
		Message.Append(HeaderElem.Key);
		Message.AppendChars(TEXT(": "), 2);
		Message.Append(Value);
		if (GEngine != nullptr)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, Message);
		}
#endif
	}
	return bIsUTF8JsonContent;
}