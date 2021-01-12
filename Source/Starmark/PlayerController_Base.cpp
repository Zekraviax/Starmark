#include "PlayerController_Base.h"



#include "NavigationSystem.h"
#include "Engine/World.h"



APlayerController_Base::APlayerController_Base()
{
	bShowMouseCursor = true;
}

void APlayerController_Base::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("SetDestination", IE_Pressed, this, &APlayerController_Base::OnSetDestinationPressed);
	InputComponent->BindAction("SetDestination", IE_Released, this, &APlayerController_Base::OnSetDestinationReleased);
}

void APlayerController_Base::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	// keep updating the destination every tick while desired
	if (bMoveToMouseCursor)
	{
		MoveToMouseCursor();
	}
}

void APlayerController_Base::MoveToMouseCursor()
{
	//UE_LOG(LogTemp, Warning, TEXT("MoveToMouseCursor"));
	if (CurrentSelectedPawn)
	{
		//UE_LOG(LogTemp, Warning, TEXT("CurrentSelectedPawn"));
		if (CurrentSelectedPawn->GetCursorToWorld())
		{
			//UE_LOG(LogTemp, Warning, TEXT("MoveToLocation"));
			//UNavigationSystem::SimpleMoveToLocation(this, CurrentSelectedPawn->GetCursorToWorld()->GetComponentLocation());
		}
	}
	else
	{
		// Trace to see what is under the mouse cursor
		FHitResult Hit;
		GetHitResultUnderCursor(ECC_Visibility, false, Hit);

		if (Hit.bBlockingHit)
		{
			// We hit something, move there
			//SetNewMoveDestination(Hit.ImpactPoint);
		}
	}
}

void APlayerController_Base::OnSetDestinationPressed()
{
	// set flag to keep updating destination until released
	bMoveToMouseCursor = true;
}

void APlayerController_Base::OnSetDestinationReleased()
{
	// clear flag to indicate we should stop updating the destination
	bMoveToMouseCursor = false;
}