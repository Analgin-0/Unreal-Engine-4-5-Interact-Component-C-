// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT6_API UInteractComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Sets default values for this component's properties
	UInteractComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY()
	TArray<AActor*>ArrInteractBeginFocus;
	UPROPERTY()
	ACharacter* OwnerCharacter = nullptr;

	UPROPERTY(Replicated)
	APlayerController* OwnerController = nullptr;

	UPROPERTY()
	AActor* InteractActor = nullptr;

	UPROPERTY()
	FTimerHandle LineTraceTimer;

	UPROPERTY()
	USkeletalMeshComponent* TraceStartComponent = nullptr;

	UFUNCTION()
	FVector Interact_Traectory_Trace();
	UFUNCTION()
	void Interactable_Trace();

public:

	UPROPERTY(EditAnywhere, Category = "Parameters")
	float Trace_Lenght = 200.f;

	UPROPERTY(EditAnywhere, Category = "Parameters")
	float Trace_Radius = 2.f;

	UPROPERTY(EditAnywhere, Category = "Parameters")
	FName StartBoneSocketName = TEXT("head");

	UPROPERTY(EditAnywhere, Category = "Parameters")
	TEnumAsByte<ECollisionChannel> TraceChannelType;

	UPROPERTY(EditAnywhere, Category = "Parameters")
	bool UseDefaultMeshStartTrace = true;

	UPROPERTY(EditAnywhere, Category = "Parameters")
	bool AutoActivateInteractTrace = true;

	UPROPERTY(EditAnywhere, Category = "Parameters")
	bool VisibilityLineTrace = true;

	UFUNCTION(BlueprintCallable)
	void Interact();

	UFUNCTION(BlueprintCallable)
	void StartLineTraceLoop();

	UFUNCTION(BlueprintCallable)
	void StopLineTraceLoop();

	UFUNCTION(BlueprintCallable)
	void SetStartTrace_SK(USkeletalMeshComponent* mesh);

	UFUNCTION(BlueprintCallable)
	void SetController(class APlayerController* controller);

	UFUNCTION(BlueprintCallable)
	void OnCreateDefaultSubObject(const AActor* object);
		
};
