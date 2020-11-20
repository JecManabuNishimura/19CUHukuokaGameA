//----------------------------------------------------------
// �t�@�C����		�FDashEnemy.h
// �T�v				�F�_�b�V���G�l�~�[�̐���
// �쐬��			�F19CU0222 �߉Ɠ�
// �X�V���e			�F2020/11/20 �쐬�@�l��Blueprint��Ő��䂷��悤
//----------------------------------------------------------
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DashEnemy.generated.h"

UCLASS()
class GAME_API ADashEnemy : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADashEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	// �����ɍڂ���ꂽ�̂�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
		bool isRidedOn;
	// �������t���O
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
		bool isDead;

	/*UFUNCTION()
		void OnBeginOverlap(class UPrimitiveComponent* HitComp,
			class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);*/
};
