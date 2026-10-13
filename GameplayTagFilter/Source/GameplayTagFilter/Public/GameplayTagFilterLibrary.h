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

	/** The tag this group of actors belongs to. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tag Filter")
	FGameplayTag Tag;

	/** Actors belonging to this tag. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tag Filter")
	TArray<TObjectPtr<AActor>> Actors;
};

/**
 * Blueprint library: fetch actors from arrays / maps by Gameplay Tag in one node,
 * instead of looping with "Actor Has Tag" + branch.
 *
 * Matching rules:
 * - bExactMatch = true: tag names must be identical.
 * - bExactMatch = false (default): hierarchical, like native GAS matching —
 *   query "Damage" matches "Damage" and "Damage.Fire"; it does NOT match "Damage".
 *   NOTE for actor FName tags: only FNames registered in the project's Gameplay
 *   Tag dictionary participate in hierarchy; unregistered names match exact-only.
 */
UCLASS()
class GAMEPLAYTAGFILTER_API UGameplayTagFilterLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/**
	 * Returns the actors whose standard Actor Tags match the given Gameplay Tag.
	 * Replaces: ForEach (Array) -> Actor Has Tag -> Branch -> Add.
	 * @param Actors       Input array of actors.
	 * @param Tag          Gameplay Tag to search for.
	 * @param bExactMatch  If true, only the exact tag matches; if false, child tags match too ("Damage" finds "Damage.Fire").
	 * @param OutActors    All actors that matched.
	 * @param MatchCount   Number of matched actors.
	 */
	UFUNCTION(BlueprintPure, Category = "Tag Filter",
		meta = (Keywords = "filter actors array gameplay tag tags has", ShortToolTip = "Get actors from an array whose tags match"))
	static void FilterActorsByTag(const TArray<AActor*>& Actors,
		const FGameplayTag& Tag, bool bExactMatch,
		TArray<AActor*>& OutActors, int32& MatchCount);

	/**
	 * Returns the actors whose standard Actor Tags match ANY or ALL tags of the given tag container.
	 * @param Actors       Input array of actors.
	 * @param Tags         Container of Gameplay Tags to search for.
	 * @param MatchType    Any = at least one tag matches; All = every tag must match.
	 * @param bExactMatch  If true, only exact tag names match; if false, child tags match too.
	 * @param OutActors    All actors that matched.
	 * @param MatchCount   Number of matched actors.
	 */
	UFUNCTION(BlueprintPure, Category = "Tag Filter",
		meta = (Keywords = "filter actors array gameplay tag container any all", ShortToolTip = "Get actors from an array matching any/all tags"))
	static void FilterActorsByTags(const TArray<AActor*>& Actors,
		const FGameplayTagContainer& Tags, EGameplayContainerMatchType MatchType, bool bExactMatch,
		TArray<AActor*>& OutActors, int32& MatchCount);

	/**
	 * Finds entries in a Tag -> Actor map whose KEY matches the query tag (hierarchical by default:
	 * query "Damage" also finds the entry keyed "Damage.Fire").
	 * @param ActorMap       Map from Gameplay Tag to actor.
	 * @param Tag            Gameplay Tag to search for.
	 * @param bExactMatch    If true, only the exact key matches; if false, child keys match too.
	 * @param OutActors      Actors stored under all matched keys.
	 * @param OutMatchedKeys The map keys that matched (parallel to OutActors).
	 * @param MatchCount     Number of matched entries.
	 */
	UFUNCTION(BlueprintPure, Category = "Tag Filter",
		meta = (Keywords = "find actor map gameplay tag key lookup", ShortToolTip = "Lookup actors in a Tag->Actor map by tag"))
	static void FindActorsInTagMap(const TMap<FGameplayTag, AActor*>& ActorMap,
		const FGameplayTag& Tag, bool bExactMatch,
		TArray<AActor*>& OutActors, TArray<FGameplayTag>& OutMatchedKeys, int32& MatchCount);

	/**
	 * Finds entries in a tag-group list (Tag + Array of actors, see FTaggedActorArray) whose tag matches
	 * the query tag, and returns the combined actors.
	 * (TMap<FGameplayTag, TArray<AActor*>> is not allowed in Blueprint — nested containers —
	 * so an array of FTaggedActorArray is used instead.)
	 * @param Groups         Tag groups to search.
	 * @param Tag            Gameplay Tag to search for.
	 * @param bExactMatch    If true, only the exact group tag matches; if false, child tags match too.
	 * @param OutActors      Combined actors from all matched groups.
	 * @param OutMatchedTags Tags of the groups that matched (each listed once).
	 * @param MatchCount     Number of actors returned.
	 */
	UFUNCTION(BlueprintPure, Category = "Tag Filter",
		meta = (Keywords = "find actors tag groups array map", ShortToolTip = "Get actors from Tag->Array groups by tag"))
	static void FindActorsInTagGroups(const TArray<FTaggedActorArray>& Groups,
		const FGameplayTag& Tag, bool bExactMatch,
		TArray<AActor*>& OutActors, TArray<FGameplayTag>& OutMatchedTags, int32& MatchCount);

	/**
	 * Filters an Actor -> Tags map, returning the actors (map keys) whose tag container matches the query tag.
	 * @param ActorMap     Map from actor to its tag container.
	 * @param Tag          Gameplay Tag to search for.
	 * @param bExactMatch  If true, only the exact tag matches; if false, child tags match too.
	 * @param OutActors    Actors (map keys) that matched.
	 * @param MatchCount   Number of matched actors.
	 */
	UFUNCTION(BlueprintPure, Category = "Tag Filter",
		meta = (Keywords = "filter actor map tag container value", ShortToolTip = "Filter an Actor->Tags map by tag"))
	static void FilterActorsMapByTag(const TMap<AActor*, FGameplayTagContainer>& ActorMap,
		const FGameplayTag& Tag, bool bExactMatch,
		TArray<AActor*>& OutActors, int32& MatchCount);
};
