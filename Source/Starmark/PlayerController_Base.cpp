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

}