// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractComponent.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "InteractInterface.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UInteractComponent::UInteractComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UInteractComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (UseDefaultMeshStartTrace)
		TraceStartComponent = OwnerCharacter->GetMesh();

	if (IsValid(OwnerCharacter) && AutoActivateInteractTrace && TraceStartComponent)
		StartLineTraceLoop();
	else
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("erorr interact component"),
			true, true, FLinearColor::Red, 5.f);

	OwnerController = Cast<APlayerController>(OwnerCharacter->Controller);// OwnerCharacter->Controller->CastToPlayerController();


}


// Called every frame
void UInteractComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


void UInteractComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UInteractComponent, OwnerController);
}

FVector UInteractComponent::Interact_Traectory_Trace()
{


	if (OwnerController == nullptr)
	{
		OwnerController = Cast<APlayerController>(OwnerCharacter->Controller);
		if (OwnerController == nullptr) {
			return FVector();
		}
	}
	FVector trace_start = OwnerController->PlayerCameraManager->GetActorTransform().GetLocation(); // camera_manager->GetActorTransform().GetLocation();

	FVector trace_end = OwnerController->PlayerCameraManager->GetActorForwardVector() * Trace_Lenght + trace_start; //camera_manager->GetActorForwardVector() * Trace_Lenght + trace_start;

	TArray<AActor*> ignore_trace_actor{ GetOwner() };

	FHitResult hit_res;


	UKismetSystemLibrary::LineTraceSingle(GetWorld(), trace_start, trace_end,
		UEngineTypes::ConvertToTraceType(TraceChannelType.GetValue()), false,
		ignore_trace_actor, EDrawDebugTrace::None, hit_res, true,
		FLinearColor::Red, FLinearColor::Green, 5.0f);

	if (hit_res.bBlockingHit)
		return hit_res.Location;

	return hit_res.TraceEnd;
}

void UInteractComponent::Interactable_Trace()
{
	FVector start = TraceStartComponent->GetSocketLocation(StartBoneSocketName);
	FVector end = Interact_Traectory_Trace();
	TArray<AActor*> ignore_trace_actor{ GetOwner() };
	FHitResult hit_res;

	UKismetSystemLibrary::SphereTraceSingle(GetWorld()
		, start, end, Trace_Radius, UEngineTypes::ConvertToTraceType(TraceChannelType.GetValue()),
		false, ignore_trace_actor, VisibilityLineTrace ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None, hit_res, true, FLinearColor::Yellow,
		FLinearColor::Green, 0.2f);

	bool check_implement_interface = UKismetSystemLibrary::DoesImplementInterface(
		hit_res.GetActor(), UInteractInterface::StaticClass());

	if (check_implement_interface && hit_res.bBlockingHit)
	{
		if (!InteractActor)
		{
			InteractActor = hit_res.GetActor();
			for (size_t i = 0; i < ArrInteractBeginFocus.Num(); i++) {
				if (ArrInteractBeginFocus[i] == InteractActor) {

					return;
				}
			}


			ArrInteractBeginFocus.Add(InteractActor);
			IInteractInterface::Execute_BeginFocus(InteractActor);
		}
		else if (InteractActor != hit_res.GetActor() && InteractActor)
		{
			ArrInteractBeginFocus.Remove(InteractActor);
			IInteractInterface::Execute_EndFocus(InteractActor);
			InteractActor = hit_res.GetActor();
			IInteractInterface::Execute_BeginFocus(InteractActor);
			ArrInteractBeginFocus.Add(InteractActor);
		}
		else
		{
			InteractActor = hit_res.GetActor();
			for (size_t i = 0; i < ArrInteractBeginFocus.Num(); i++) {
				if (ArrInteractBeginFocus[i] == InteractActor) {

					return;
				}
			}
			IInteractInterface::Execute_BeginFocus(InteractActor);

		}
	}
	else if (InteractActor)
	{
		ArrInteractBeginFocus.Remove(InteractActor);
		IInteractInterface::Execute_EndFocus(InteractActor);
		InteractActor = nullptr;
	}
}

void UInteractComponent::Interact()
{
	if (IsValid(InteractActor))
		IInteractInterface::Execute_Interact(InteractActor, OwnerCharacter);
}

void UInteractComponent::StartLineTraceLoop()
{
	if (LineTraceTimer.IsValid())
		GetWorld()->GetTimerManager().UnPauseTimer(LineTraceTimer);
	else
		GetWorld()->GetTimerManager().SetTimer(LineTraceTimer, this, &UInteractComponent::Interactable_Trace, 0.01f, true);
}

void UInteractComponent::StopLineTraceLoop()
{
	if (LineTraceTimer.IsValid())
		GetWorld()->GetTimerManager().PauseTimer(LineTraceTimer);
}

void UInteractComponent::SetStartTrace_SK(USkeletalMeshComponent* mesh)
{
	if (mesh)
		TraceStartComponent = mesh;
}

void UInteractComponent::SetController(APlayerController* controller)
{
	if (controller)
		OwnerController = controller;
}

void UInteractComponent::OnCreateDefaultSubObject(const AActor* object)
{
	//if (object->GetActorLocation() > FVector(1,1,1))
	//{
	//	
	//}

}

