// Fill out your copyright notice in the Description page of Project Settings.

#include "TCPClient.h"


// Sets default values for this component's properties
UTCPClient::UTCPClient()
{
	Address = TEXT("127.0.0.1");
	Port = 6767;
}


void UTCPClient::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (Socket)
	{
		Socket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Socket);
	}
}

void UTCPClient::Connect()
{
	if (bSocketConnected) {
		UE_LOG(LogBlueprintUserMessages, Warning, TEXT("Socket is already connected."));
		return;
	}
	Socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("default"), false);
	FIPv4Address ip;
	FIPv4Address::Parse(Address, ip);
	TSharedRef<FInternetAddr> addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	addr->SetIp(ip.Value);
	addr->SetPort(Port);
	bool connected = Socket->Connect(*addr);
	if (connected) {
		OnConnect.Broadcast(true);
		bSocketConnected = true;
		GetWorld()->GetTimerManager().SetTimer(ListenerHandle, this, &UTCPClient::Listener, 0.01, true);
	}
	else {
		OnConnect.Broadcast(false);
		bSocketConnected = false;
	}
}

bool UTCPClient::Send(FString StringToSend = "")
{
	if (StringToSend == "") {
		return false;
	}
	TCHAR *StringToSendChar = StringToSend.GetCharArray().GetData();
	int32 Size = FCString::Strlen(StringToSendChar);
	uint8* MessageSize = new uint8[4];
	TArray<uint8> ToSend;
	FMemory::Memcpy(MessageSize, &Size, 4);
	ToSend.Insert((uint8*)TCHAR_TO_UTF8(StringToSendChar), Size, 0);
	ToSend.Insert(MessageSize, 4, 0);
	int32 Sent = 0;
	bool Success = Socket->Send(ToSend.GetData(), ToSend.Num(), Sent);
	return Success;
}

void UTCPClient::Listener()
{
	TArray<uint8> ReceivedData;
	uint32 Size;
	if (Socket->HasPendingData(Size))
	{
		ReceivedData.Init(0, Size);
		int32 Read = 0;
		Socket->Recv(ReceivedData.GetData(), ReceivedData.Num(), Read);
		if (bHasPartial) {
			ReceivedData.Insert(PartialMessage.GetData(), PartialMessage.Num(), 0);
			PartialMessage.Empty();
			bHasPartial = false;
		}
	}
	if (ReceivedData.Num() <= 0)
	{
		bool HasPendingConnection = false;
		Socket->HasPendingConnection(HasPendingConnection);
		if (HasPendingConnection) {
			if (SocketConnectionTimeout == 1000) {
				OnDisconnect.Broadcast();
				bSocketConnected = false;
				GetWorld()->GetTimerManager().ClearTimer(ListenerHandle);
			}
			SocketConnectionTimeout++;
			return;
		}
		return;
	}
	if (SocketConnectionTimeout > 0) {
		SocketConnectionTimeout = 0;
	}
	int32 BytesProcessed = 0;
	while (BytesProcessed != ReceivedData.Num()) {
		int32 MessageSize = 0;
		bool bPartialMessageSize = false;
		FString Message = TEXT("");
		if ((ReceivedData.Num() - BytesProcessed) >= 4) {
			FMemory::Memcpy(&MessageSize, ReceivedData.GetData() + BytesProcessed, 4);
		}
		else {
			UE_LOG(LogBlueprintUserMessages, Warning, TEXT("Partial Message Size Detected!"));
			bPartialMessageSize = true;
		}
		if (bPartialMessageSize || (MessageSize > ReceivedData.Num() - BytesProcessed)) {
			UE_LOG(LogBlueprintUserMessages, Warning, TEXT("Partial Detected!"));
			PartialMessage.Insert(ReceivedData.GetData() + BytesProcessed, ReceivedData.Num() - BytesProcessed, 0);
			bHasPartial = true;
			break;
		}
		BytesProcessed += 4;
		FUTF8ToTCHAR Src = FUTF8ToTCHAR((const ANSICHAR*)ReceivedData.GetData() + BytesProcessed, MessageSize);
		Message.AppendChars(Src.Get(), Src.Length());
		BytesProcessed += MessageSize;
		OnMessage.Broadcast(*Message);
	}
	return;
}