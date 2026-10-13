#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagContainer.h"
#include "GameplayTagFilterLibrary.generated.h"

class AActor;

/**
 * One tag mapped to a group of actors. Blueprint does not allow TMap with an
 * array as the value (nested containers are forbidden), so "Tag -> Array<Actor>"
 * is represented as an array of this struct.
 */
USTRUCT(BlueprintType)
struct FTaggedActorArray
{
	GENERATED_BODY()

	/** The tag this group of actors belongs to. / Тег, к которому относится эта группа акторов. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tag Filter")
	FGameplayTag Tag;

	/** Actors belonging to this tag. / Акторы, относящиеся к этому тегу. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tag Filter")
	TArray<TObjectPtr<AActor>> Actors;
};

/**
 * Blueprint library: fetch actors from arrays / maps by Gameplay Tag in one node,
 * instead of looping with "Actor Has Tag" + branch.
 *
 * Every node returns OutActors (the result) and MatchCount (how many were found,
 * i.e. just OutActors.Num() — convenient for a quick "did we find anything" check).
 *
 * Matching rules:
 * - bExactMatch = true: tag names must be identical.
 * - bExactMatch = false (default): hierarchical — the query "Damage" matches
 *   "Damage" and "Damage.Fire"; it does NOT match the parent "Damage".
 *   Only FNames registered in the project's Gameplay Tag dictionary participate
 *   in the hierarchy; unregistered names match exact-only.
 */
UCLASS()
class GAMEPLAYTAGFILTER_API UGameplayTagFilterLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/**
	 * Returns the actors from the input array whose standard Actor Tags match the given Gameplay Tag.
	 * Replaces: ForEach (Array) -> Actor Has Tag -> Branch -> Add.
	 * Возвращает акторов из массива, чьи стандартные Actor Tags совпали с указанным тегом.
	 * Заменяет связку ForEach -> Actor Has Tag -> Branch -> Add.
	 */
	UFUNCTION(BlueprintPure, Category = "Tag Filter",
		meta = (Keywords = "filter actors array gameplay tag tags has",
			ShortToolTip = "Get actors from an array whose tags match / Достать акторов из массива по тегу"))
	static void FilterActorsByTag(
		UPARAM(meta=(PinTooltip="Actors to search through / Акторы, которые проверяем")) const TArray<AActor*>& Actors,
		UPARAM(meta=(PinTooltip="The Gameplay Tag to search for / Какой тег ищем")) FGameplayTag Tag,
		UPARAM(meta=(PinTooltip="true = only the exact tag name matches. false = child tags match too: query Damage finds Damage.Fire / true = только точное совпадение имени тега. false = учитываются и дочерние теги: запрос Damage найдёт и Damage.Fire")) bool bExactMatch,
		UPARAM(meta=(PinTooltip="All actors whose tags matched (input order preserved) / Все акторы, чьи теги совпали (в порядке входа)")) TArray<AActor*>& OutActors,
		UPARAM(meta=(PinTooltip="How many actors were found. Same as OutActors.Num() / Сколько акторов нашли (равно длине OutActors)")) int32& MatchCount);

	/**
	 * Returns the actors from the input array whose standard Actor Tags match ANY or ALL
	 * tags of the given list.
	 * Возвращает акторов из массива, чьи теги совпали с ХОТЯ БЫ ОДНИМ или со ВСЕМИ тегами из списка.
	 */
	UFUNCTION(BlueprintPure, Category = "Tag Filter",
		meta = (Keywords = "filter actors array gameplay tag container any all",
			ShortToolTip = "Get actors from an array matching any/all tags / Достать акторов из массива по нескольким тегам"))
	static void FilterActorsByTags(
		UPARAM(meta=(PinTooltip="Actors to search through / Акторы, которые проверяем")) const TArray<AActor*>& Actors,
		UPARAM(meta=(PinTooltip="List of Gameplay Tags to search for / Список тегов, которые ищем")) FGameplayTagContainer Tags,
		UPARAM(meta=(PinTooltip="Any = at least one tag must match. All = every tag must match / Any = достаточно хотя бы одного тега. All = нужны все теги из списка")) EGameplayContainerMatchType MatchType,
		UPARAM(meta=(PinTooltip="true = only exact tag names match. false = child tags match too / true = только точное совпадение. false = учитываются и дочерние теги")) bool bExactMatch,
		UPARAM(meta=(PinTooltip="All actors whose tags matched / Все акторы, чьи теги совпали")) TArray<AActor*>& OutActors,
		UPARAM(meta=(PinTooltip="How many actors were found / Сколько акторов нашли")) int32& MatchCount);

	/**
	 * Searches a map where each entry is "tag = key, actor = value" and returns the actors
	 * stored under entries whose KEY matches the query tag (hierarchical by default:
	 * query "Damage" also finds the entry keyed "Damage.Fire").
	 * Ищет в мапе "тег = ключ, актор = значение" записи, чей КЛЮЧ совпал с запросом
	 * (по умолчанию с иерархией: запрос Damage находит и запись с ключом Damage.Fire).
	 */
	UFUNCTION(BlueprintPure, Category = "Tag Filter",
		meta = (Keywords = "find actor map gameplay tag key lookup",
			ShortToolTip = "Lookup actors in a Tag->Actor map by tag / Найти акторов в мапе Тег->Актoр по тегу"))
	static void FindActorsInTagMap(
		UPARAM(meta=(PinTooltip="The Tag -> Actor map to search. Each entry: key = tag, value = actor / Мапа Тег -> Актoр. Каждая запись: ключ = тег, значение = актор")) const TMap<FGameplayTag, AActor*>& ActorMap,
		UPARAM(meta=(PinTooltip="The Gameplay Tag to search for among the map keys / Тег, который ищем среди ключей мапы")) FGameplayTag Tag,
		UPARAM(meta=(PinTooltip="true = only the exact key matches. false = child keys match too: query Damage finds entry keyed Damage.Fire / true = только точное совпадение ключа. false = учитываются и дочерние: запрос Damage найдёт запись с ключом Damage.Fire")) bool bExactMatch,
		UPARAM(meta=(PinTooltip="Actors stored under all matched keys / Акторы из всех записей, где ключ совпал")) TArray<AActor*>& OutActors,
		UPARAM(meta=(PinTooltip="Which map keys matched. Useful to see which tag branch actually responded / Какие именно ключи совпали — видно, какая ветка тега сработала")) TArray<FGameplayTag>& OutMatchedKeys,
		UPARAM(meta=(PinTooltip="Number of matched entries. Same as OutActors.Num() / Сколько записей совпало")) int32& MatchCount);

	/**
	 * Searches tag groups (each group = one tag + actors belonging to it) and returns all
	 * actors from every group whose tag matches the query, merged into one array.
	 * (TMap<FGameplayTag, TArray<AActor*>> is not allowed in Blueprint — nested containers —
	 * so an array of FTaggedActorArray is used instead.)
	 * Ищет среди групп (каждая группа = один тег + акторы этого тега) и возвращает всех акторов
	 * подошедших групп одним массивом. Вместо запрещённой в BP мапы "тег -> массив" используется
	 * массив структур FTaggedActorArray.
	 */
	UFUNCTION(BlueprintPure, Category = "Tag Filter",
		meta = (Keywords = "find actors tag groups array map",
			ShortToolTip = "Get actors from Tag->Array groups by tag / Найти акторов в группах Тег->Массив акторов"))
	static void FindActorsInTagGroups(
		UPARAM(meta=(PinTooltip="List of tag groups. Each group = one tag + the actors belonging to it / Список групп. Каждая группа = один тег + акторы, к нему относящиеся")) const TArray<FTaggedActorArray>& Groups,
		UPARAM(meta=(PinTooltip="The Gameplay Tag to search for among the group tags / Тег, который ищем среди тегов групп")) FGameplayTag Tag,
		UPARAM(meta=(PinTooltip="true = only the exact group tag matches. false = child tags match too / true = только точное совпадение тега группы. false = учитываются и дочерние теги")) bool bExactMatch,
		UPARAM(meta=(PinTooltip="All actors from every matched group, merged into one array / Все акторы подошедших групп, объединённые в один массив")) TArray<AActor*>& OutActors,
		UPARAM(meta=(PinTooltip="Tags of the groups that matched (each listed once) / Теги групп, которые совпали (каждый один раз)")) TArray<FGameplayTag>& OutMatchedTags,
		UPARAM(meta=(PinTooltip="Number of actors returned. Can be larger than the number of matched groups / Сколько акторов вернули (может быть больше числа совпавших групп)")) int32& MatchCount);

	/**
	 * Filters a map where each entry is "actor = key, its tags = value" and returns the actors
	 * (map keys) whose tag list contains the query tag.
	 * Фильтрует мапу "актор = ключ, его теги = значение" и возвращает акторов, чей список тегов содержит запрос.
	 */
	UFUNCTION(BlueprintPure, Category = "Tag Filter",
		meta = (Keywords = "filter actor map tag container value",
			ShortToolTip = "Filter an Actor->Tags map by tag / Отфильтровать мапу Актoр->Теги по тегу"))
	static void FilterActorsMapByTag(
		UPARAM(meta=(PinTooltip="The Actor -> Tags map to search. Each entry: key = actor, value = list of its tags / Мапа Актoр -> Теги. Каждая запись: ключ = актор, значение = список его тегов")) const TMap<AActor*, FGameplayTagContainer>& ActorMap,
		UPARAM(meta=(PinTooltip="The Gameplay Tag to search for among the tag lists / Тег, который ищем в списках тегов")) FGameplayTag Tag,
		UPARAM(meta=(PinTooltip="true = only the exact tag matches. false = child tags match too / true = только точное совпадение. false = учитываются и дочерние теги")) bool bExactMatch,
		UPARAM(meta=(PinTooltip="Map keys (actors) whose tag list contains the query / Акторы-ключи, в списке тегов которых есть запрос")) TArray<AActor*>& OutActors,
		UPARAM(meta=(PinTooltip="How many actors were found / Сколько акторов нашли")) int32& MatchCount);
};
