#include "Cube.h"
#include "Components/StaticMeshComponent.h"

#include "Engine/FLoaderOBJ.h"

#include "GameFramework/Actor.h"

ACube::ACube()
{
    // Begin Test
    //StaticMeshComponent->SetStaticMesh(FManagerOBJ::GetStaticMesh(L"Contents/helloBlender.obj"));
    RootComponent = StaticMeshComponent;
    StaticMeshComponent->SetStaticMesh(FManagerOBJ::GetStaticMesh(L"Contents/Reference/Reference.obj"));
    // End Test
}

void ACube::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    //SetActorRotation(GetActorRotation() + FRotator(0, 0, 1));

}
