// Fill out your copyright notice in the Description page of Project Settings.

#include "FriendInfo.h"
#include "Steamworks/Steamv139/sdk/public/steam/steam_api.h"
#include "../Core/NetworkGameInstance.h"
#include "Engine/Texture2D.h"
#include <string>

UFriendInformation::UFriendInformation() {}
UFriendInformation::~UFriendInformation() {
}
UFriendInfo::UFriendInfo() {
}
UFriendInfo::~UFriendInfo() {
	InitArray();
}
void UFriendInfo::RefreshFriendInfo() {
	auto world = GetWorld();
	if (world == nullptr) {
		MYLOG(Error, TEXT("Get World Error"));
		return;
	}
	UNetworkGameInstance* gameInstance = Cast<UNetworkGameInstance>(world->GetGameInstance());
	if (gameInstance == nullptr) {
		MYLOG(Error, TEXT("GameInstance Casting Error"));
		return;
	}
	if (gameInstance->OnSteam()) {
		InitArray();

		// Get Friends Info
		uint64 myId = SteamUser()->GetSteamID().ConvertToUint64();
		ISteamFriends* steamFriends = SteamFriends();
		int friendCount = steamFriends->GetFriendCount(k_EFriendFlagAll);
		for (int i = 0; i < friendCount; ++i) {
			CSteamID id = steamFriends->GetFriendByIndex(i, k_EFriendFlagAll);

			FriendGameInfo_t info;
			steamFriends->GetFriendGamePlayed(id, &info);
			std::string TestString = steamFriends->GetFriendPersonaName(id);
			FString name = UTF8_TO_TCHAR(TestString.c_str());

			UFriendInformation* newInformation = NewObject<UFriendInformation>(this);
			newInformation->name = name;
			newInformation->steamID = id.ConvertToUint64();
			newInformation->avatar = GetSteamAvatar(newInformation->steamID);
			FriendInformations.Add(newInformation);
		}
	}
	else {
		MYLOG(Warning, TEXT("Steam is not running."));
	}
}
UTexture2D* UFriendInfo::GetSteamAvatar(uint64 steamID)
{
	UNetworkGameInstance* gameInstance = Cast<UNetworkGameInstance>(GetWorld()->GetGameInstance());
	if (gameInstance == nullptr) {
		MYLOG(Error, TEXT("GameInstance Casting Error"));
		return nullptr;
	}
	uint32 Width;
	uint32 Height;

	if (gameInstance->OnSteam())
	{
		CSteamID PlayerRawID(steamID);
		//Getting the PictureID from the SteamAPI and getting the Size with the ID
		int Picture = SteamFriends()->GetMediumFriendAvatar(PlayerRawID);
		SteamUtils()->GetImageSize(Picture, &Width, &Height);


		if (Width > 0 && Height > 0)
		{
			//Creating the buffer "oAvatarRGBA" and then filling it with the RGBA Stream from the Steam Avatar
			BYTE *oAvatarRGBA = new BYTE[Width * Height * 4];


			//Filling the buffer with the RGBA Stream from the Steam Avatar and creating a UTextur2D to parse the RGBA Steam in
			SteamUtils()->GetImageRGBA(Picture, (uint8*)oAvatarRGBA, 4 * Height * Width * sizeof(char));

			//Swap R and B channels because for some reason the games whack
			for (uint32 i = 0; i < (Width * Height * 4); i += 4)
			{
				uint8 Temp = oAvatarRGBA[i + 0];
				oAvatarRGBA[i + 0] = oAvatarRGBA[i + 2];
				oAvatarRGBA[i + 2] = Temp;
			}

			UTexture2D* Avatar = UTexture2D::CreateTransient(Width, Height, PF_B8G8R8A8);

			//MAGIC!
			uint8* MipData = (uint8*)Avatar->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
			FMemory::Memcpy(MipData, (void*)oAvatarRGBA, Height * Width * 4);
			Avatar->PlatformData->Mips[0].BulkData.Unlock();

			//Setting some Parameters for the Texture and finally returning it
			Avatar->PlatformData->NumSlices = 1;
			Avatar->NeverStream = true;
			//Avatar->CompressionSettings = TC_EditorIcon;

			Avatar->UpdateResource();
			delete[] oAvatarRGBA;
			return Avatar;
		}
		return nullptr;
	}
	return nullptr;
}
void UFriendInfo::InitArray()
{
	FriendInformations.Empty();
}
