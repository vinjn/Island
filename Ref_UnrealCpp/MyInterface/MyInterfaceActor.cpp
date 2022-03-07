// Harrison McGuire
// UE4 Version 4.20.2
// https://github.com/Harrison1/unrealcpp
// https://severallevels.io
// https://harrisonmcguire.com

#include "MyInterfaceActor.h"
#include "Components/TimelineComponent.h"

// Sets default values
AMyInterfaceActor::AMyInterfaceActor()
{
	MyTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("My Timeline"));

	Name = FString(TEXT("Harry"));
}

void AMyInterfaceActor::MoveToLocation(float value) 
{
	SetActorLocation(FMath::Lerp(Start, End, value));
}

void AMyInterfaceActor::ReactToTriggerBegin()
{
	IMyInterface::ReactToTriggerBegin();

	if (MyCurve) 
	{
		FOnTimelineFloat TimelineCallback;
		TimelineCallback.BindUFunction(this, FName("MoveToLocation"));
		MyTimeline->AddInterpFloat(MyCurve, TimelineCallback);
		MyTimeline->SetLooping(false);
		Start = GetActorLocation();
		End = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
		MyTimeline->PlayFromStart();
	}
}

void AMyInterfaceActor::ReactToTriggerEnd()
{
	IMyInterface::ReactToTriggerEnd();

	if (MyCurve)
	{
		FOnTimelineFloat TimelineCallback;
		TimelineCallback.BindUFunction(this, FName("MoveToLocation"));
		MyTimeline->AddInterpFloat(MyCurve, TimelineCallback);
		MyTimeline->SetLooping(false);
		End = Start;
		Start = GetActorLocation();
		MyTimeline->PlayFromStart();
	}
}
