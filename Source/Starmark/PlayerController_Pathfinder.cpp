#include "PlayerController_Pathfinder.h"


#include "NavigationSystem.h"
#include "AI/NavigationSystemBase.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Character_Pathfinder.h"
#include "Engine/World.h"

APlayerController_Pathfinder::APlayerController_Pathfinder()
{
	bShowMouseCursor = true;
	//DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void APlayerController_Pathfinder::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	// keep updating the destination every tick while desired
	//if (bMoveToMouseCursor)
	//{
	//	MoveToMouseCursor();
	//}
}

void APlayerController_Pathfinder::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("SetDestination", IE_Pressed, this, &APlayerController_Pathfinder::OnSetDestinationPressed);
	InputComponent->BindAction("SetDestination", IE_Released, this, &APlayerController_Pathfinder::OnSetDestinationReleased);

	// support touch devices 
	//InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &APlayerController_Pathfinder::MoveToTouchLocation);
	//InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &APlayerController_Pathfinder::MoveToTouchLocation);

	//InputComponent->BindAction("ResetVR", IE_Pressed, this, &APlayerController_Pathfinder::OnResetVR);
}

//void APlayerController_Pathfinder::OnResetVR()
//{
//	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
//}

void APlayerController_Pathfinder::MoveToMouseCursor()
{
	//if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	//{
	//if (ACharacter_Pathfinder* MyPawn = Cast<ACharacter_Pathfinder>(GetPawn()))
	//{
	//	if (MyPawn->GetCursorToWorld())
	//	{
	//		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("SimpleMoveToLocation")));
	//		UNavigationSystem::SimpleMoveToLocation(this, MyPawn->GetCursorToWorld()->GetComponentLocation());
	//	}
	//}
	////}
	//else
	//{
		////Trace to see what is under the mouse cursor
		//FHitResult Hit;
		//GetHitResultUnderCursor(ECC_Visibility, false, Hit);

		//if (Hit.bBlockingHit)
		//{
		//	//We hit something, move there
		//	SetNewMoveDestination(Hit.ImpactPoint);
		//}
	//}
}

//void APlayerController_Pathfinder::MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location)
//{
//	FVector2D ScreenSpaceLocation(Location);
//
//	// Trace to see what is under the touch location
//	FHitResult HitResult;
//	GetHitResultAtScreenPosition(ScreenSpaceLocation, CurrentClickTraceChannel, true, HitResult);
//	if (HitResult.bBlockingHit)
//	{
//		// We hit something, move there
//		SetNewMoveDestination(HitResult.ImpactPoint);
//	}
//}

void APlayerController_Pathfinder::SetNewMoveDestination(const FVector DestLocation)
{
	//APawn* const MyPawn = GetPawn();
	//if (MyPawn)
	//{
	//	UNavigationSystem* const NavSys = Cast<UNavigationSystem>(GetWorld()->GetNavigationSystem());
	//	float const Distance = FVector::Dist(DestLocation, MyPawn->GetActorLocation());

	//	if (NavSys)
	//	{
	//		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Move To Location")));
	//		NavSys->SimpleMoveToLocation(this, DestLocation);
	//	}
	//	else {
	//		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("NavSys Not Valid")));
	//	}
	//}
	//else {
	//	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("MyPawn Not Valid")));
	//}
}

void APlayerController_Pathfinder::OnSetDestinationPressed()
{
	// set flag to keep updating destination until released
	bMoveToMouseCursor = true;
}

void APlayerController_Pathfinder::OnSetDestinationReleased()
{
	// clear flag to indicate we should stop updating the destination
	bMoveToMouseCursor = false;

	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Mouse Released")));
}
