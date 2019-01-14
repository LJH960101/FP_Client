// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(NetworkGameInstance, Log, All);
#define MYLOG_CALLINFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))
#define MYLOG_S(Verbosity) UE_LOG(LogTemp, Verbosity, TEXT("%s"), *MYLOG_CALLINFO);
#define MYLOG(Verbosity, Format, ...) UE_LOG(LogTemp, Verbosity, TEXT("%s %s"), *MYLOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))