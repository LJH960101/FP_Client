// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "EngineMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(NetworkGameInstance, Log, All);
#define MYLOG_CALLINFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))
#define MYLOG_S(Verbosity) UE_LOG(LogTemp, Verbosity, TEXT("%s"), *MYLOG_CALLINFO);
#define MYLOG(Verbosity, Format, ...) UE_LOG(LogTemp, Verbosity, TEXT("%s %s"), *MYLOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))
#define	LOG_SCREEN_A(Format, ...) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT(Format), ##__VA_ARGS__));
#define LOG_SCREEN(Format) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT(Format)));