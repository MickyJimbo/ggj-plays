// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Networking.h"
#include "Components/ActorComponent.h"
#include "TCPClient.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDisconnectStatus);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FConnectStatus, bool, Connected);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMessageReceived, FString, Message);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GLOBALGAMEJAMPLAYS_API UTCPClient : public UActorComponent
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable, Category = "TCPClient")
		FConnectStatus OnConnect;

	UPROPERTY(BlueprintAssignable, Category = "TCPClient")
		FDisconnectStatus OnDisconnect;

	UPROPERTY(BlueprintAssignable, Category = "TCPClient")
		FMessageReceived OnMessage;

	UPROPERTY(EditAnywhere, Category = "TCPClient")
		FString Address;

	UPROPERTY(EditAnywhere, Category = "TCPClient")
		int32 Port;

	// Sets default values for this component's properties
	UTCPClient();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable, category = "TCPClient")
		void Connect();

	UFUNCTION(BlueprintCallable, category = "TCPClient")
		bool Send(FString StringToSend);

private:

	FSocket * Socket;

	FTimerHandle ListenerHandle;

	bool bHasPartial;
	TArray<uint8> PartialMessage;

	bool bSocketConnected = false;

	int32 SocketConnectionTimeout = 0;

	void Listener();

};