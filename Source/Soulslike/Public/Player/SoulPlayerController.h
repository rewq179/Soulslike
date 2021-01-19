
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Player/ControllerInterface.h"
#include "DataType.h"
#include "SoulPlayerController.generated.h"

/**
 * 용도: HUD를 관리하는 클래스로, SoulCharacter나 ActorComponent에서 접근한다.
 */

UCLASS()
class SOULSLIKE_API ASoulPlayerController : public APlayerController, public IControllerInterface
{
	GENERATED_BODY()
	
public:
	ASoulPlayerController() {}

	////////////////////////////////////////////////////////////////////////////
	//// 인터페이스
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interface")
    void ShowEquipmentWidget();
	virtual void ShowEquipmentWidget_Implementation() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interface")
    void ShowInventoryWidget();
	virtual void ShowInventoryWidget_Implementation() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interface")
    void ShowStatusWidget();
	virtual void ShowStatusWidget_Implementation() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interface")
    void ShowOptionWidget();
	virtual void ShowOptionWidget_Implementation() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interface")
	void ClearItemDescription();
	virtual void ClearItemDescription_Implementation() override;
	
	////////////////////////////////////////////////////////////////////////////
	//// 매뉴 및 HUD 관련
	
	/** True : 메뉴 HUD On, False : Off */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD)
	bool bMenuActive;

	/** True : 플레이어 HUD On, False : Off */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD)
	bool bPlayerHUDActive;
	
	UFUNCTION(Client, Reliable)
    void ClientTurnOffHUD();

	UFUNCTION(BlueprintImplementableEvent, Category = Interact)
    void OnTurnOffHUD();

	////////////////////////////////////////////////////////////////////////////
	//// 플레이어 체력바
	
	UFUNCTION(Client, Reliable)
	void ClientUpdateHpBar(float CurHp, float MaxHp);

	UFUNCTION(BlueprintImplementableEvent, Category = Player)
	void OnUpdateHpValue(float CurHp, float MaxHp);

	UFUNCTION(Client, Reliable)
	void ClientUpdateStaminaBar(float CurStamina, float MaxStamina);

	UFUNCTION(BlueprintImplementableEvent, Category = Player)
	void OnUpdateStaminaValue(float CurStamina, float MaxStamina);

	UFUNCTION(Client, Reliable)
    void ClientUpdateSoulsCount(int32 SoulsCount);

	UFUNCTION(BlueprintImplementableEvent, Category = Player)
    void OnUpdateSoulsCount(int32 SoulsCount);

	////////////////////////////////////////////////////////////////////////////
	//// 플레이어 사망
	
	UFUNCTION(BlueprintImplementableEvent, Category = Player)
	void ShowDeadScreenWidget();

	////////////////////////////////////////////////////////////////////////////
	//// 적 체력바
	
	UFUNCTION(Client, Reliable)
	void ClientShowEnemyHpBar(bool bActive);

	UFUNCTION(BlueprintImplementableEvent, Category = Enemy)
	void OnShowEnemyHpBar(bool bActive);

	UFUNCTION(Client, Reliable)
	void ClientUpdateBossName(const FText& Name);

	UFUNCTION(BlueprintImplementableEvent, Category = Enemy)
	void OnUpdateBossName(const FText& Name);

	UFUNCTION(Client, Reliable)
	void ClientUpdateBossHp(float CurHp, float MaxHp);

	UFUNCTION(BlueprintImplementableEvent, Category = Enemy)
	void OnUpdateBossHp(float CurHp, float MaxHp);

	////////////////////////////////////////////////////////////////////////////
	//// 상호작용
	
	UFUNCTION(Client, Reliable)
	void ClientShowInteractMessage(bool bActive);

	UFUNCTION(BlueprintImplementableEvent, Category = Interact)
	void OnShowInteractMessage(bool bActive);

	UFUNCTION(Client, Reliable)
	void ClientUpdateInteractMessage(const FText& Name, const FVector& ActorLocation);

	UFUNCTION(BlueprintImplementableEvent, Category = Interact)
	void OnUpdateInteractMessage(const FText& Name, const FVector& ActorLocation);

	////////////////////////////////////////////////////////////////////////////
	//// 메뉴

	UFUNCTION(Client, Reliable)
    void ClientShowMenuHUD();

	UFUNCTION(BlueprintImplementableEvent, Category = Menu)
    void OnShowShowMenuHUD(bool bActive);

	UFUNCTION(Client, Reliable)
    void ClientBackMenuHUD();
	
	////////////////////////////////////////////////////////////////////////////
    //// 인벤토리
	
	UFUNCTION(Client, Reliable)
	void ClientShowInventory();

	UFUNCTION(BlueprintImplementableEvent, Category = Inventory)
	void OnShowInventory(bool bActive);

	UFUNCTION(Client, Reliable)
    void ClientUpdateInventory();

	UFUNCTION(BlueprintImplementableEvent, Category = Inventory)
    void OnUpdateInventory();

	////////////////////////////////////////////////////////////////////////////
	//// 장비
	
	UFUNCTION(Client, Reliable)
	void ClientShowEquipment();

	UFUNCTION(BlueprintImplementableEvent, Category = Equipment)
    void OnShowEquipment(bool bActive);

	UFUNCTION(Client, Reliable)
    void ClientUpdateEquipmentSlot();
 
	UFUNCTION(BlueprintImplementableEvent, Category = Equipment)
    void OnUpdateEquipmentSlot();

    UFUNCTION(Client, Reliable)
    void ClientUpdateQuickBar(int32 QuickIndex, UTexture2D* Icon, const FText& Name);
 
	UFUNCTION(BlueprintImplementableEvent, Category = Equipment)
    void OnUpdateQuickBar(int32 QuickIndex, UTexture2D* Icon, const FText& Name);

	UFUNCTION(Client, Reliable)
    void ClientClearQuickBar(int32 QuickIndex);
 
	UFUNCTION(BlueprintImplementableEvent, Category = Equipment)
    void OnClearQuickBar(int32 QuickIndex);

	///////////////////////////////////////////////////////////////////////////////
	//// 장비 설명 HUD

	UFUNCTION(Client, Reliable)
    void ClientUpdateItemDescription(FItemTable ItemTable);
 
	UFUNCTION(BlueprintImplementableEvent, Category = Equipment)
    void OnUpdateItemDescription(FItemTable ItemTable);
	
	UFUNCTION(Client, Reliable)
	void ClientClearItemDescription();
	
	UFUNCTION(BlueprintImplementableEvent, Category = Equipment)
	void OnClearItemDescription();

	////////////////////////////////////////////////////////////////////////////
	//// 스테이터스

	UFUNCTION(Client, Reliable)
	void ClientShowStatus();

	UFUNCTION(BlueprintImplementableEvent, Category = Status)
    void OnShowStatus(bool bActive);
	
	///////////////////////////////////////////////////////////////////////////////
	//// 옵션

	UFUNCTION(Client, Reliable)
    void ClientShowOption();

	UFUNCTION(BlueprintImplementableEvent, Category = Option)
    void OnShowOption(bool bActive);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
