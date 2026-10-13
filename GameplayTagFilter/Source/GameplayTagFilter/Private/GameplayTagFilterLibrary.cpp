#include "GameplayTagFilterLibrary.h"

#include "GameFramework/Actor.h"
#include "GameplayTagsManager.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_MODULE(FDefaultModuleImpl, GameplayTagFilter);

namespace
{
	// AActor tags are plain FNames, so hierarchical matching resolves each name through the
	// GameplayTag dictionary; names that are not registered tags can only match exactly.
	bool ActorTagsContain(const AActor* Actor, const FGameplayTag& Tag, bool bExactMatch)
	{
		const FName TagName = Tag.GetTagName();
		for (const FName& ActorTag : Actor->Tags)
		{
			if (ActorTag == TagName)
			{
				return true;
			}
			if (bExactMatch)
			{
				continue;
			}
			const FGameplayTag Resolved = UGameplayTagsManager::Get().RequestGameplayTag(ActorTag, /*ErrorIfNotFound=*/false);
			if (Resolved.IsValid() && Resolved.MatchesTag(Tag))
			{
				return true;
			}
		}
		return false;
	}
}

void UGameplayTagFilterLibrary::FilterActorsByTag(const TArray<AActor*>& Actors,
	FGameplayTag Tag, bool bExactMatch,
	TArray<AActor*>& OutActors, int32& MatchCount)
{
	OutActors.Reset();
	if (!Tag.IsValid())
	{
		MatchCount = 0;
		return;
	}

	for (AActor* Actor : Actors)
	{
		if (IsValid(Actor) && ActorTagsContain(Actor, Tag, bExactMatch))
		{
			OutActors.Add(Actor);
		}
	}
	MatchCount = OutActors.Num();
}

void UGameplayTagFilterLibrary::FilterActorsByTags(const TArray<AActor*>& Actors,
	FGameplayTagContainer Tags, EGameplayContainerMatchType MatchType, bool bExactMatch,
	TArray<AActor*>& OutActors, int32& MatchCount)
{
	OutActors.Reset();
	MatchCount = 0;
	if (Tags.IsEmpty())
	{
		return;
	}

	const bool bMatchAny = (MatchType == EGameplayContainerMatchType::Any);
	for (AActor* Actor : Actors)
	{
		if (!IsValid(Actor))
		{
			continue;
		}

		bool bResult = !bMatchAny;
		for (const FGameplayTag& QueryTag : Tags)
		{
			const bool bTagMatched = ActorTagsContain(Actor, QueryTag, bExactMatch);
			if (bMatchAny && bTagMatched)
			{
				bResult = true;
				break;
			}
			if (!bMatchAny && !bTagMatched)
			{
				bResult = false;
				break;
			}
		}

		if (bResult)
		{
			OutActors.Add(Actor);
		}
	}
	MatchCount = OutActors.Num();
}

void UGameplayTagFilterLibrary::FindActorsInTagMap(const TMap<FGameplayTag, AActor*>& ActorMap,
	FGameplayTag Tag, bool bExactMatch,
	TArray<AActor*>& OutActors, TArray<FGameplayTag>& OutMatchedKeys, int32& MatchCount)
{
	OutActors.Reset();
	OutMatchedKeys.Reset();
	if (!Tag.IsValid())
	{
		MatchCount = 0;
		return;
	}

	for (const TPair<FGameplayTag, AActor*>& Pair : ActorMap)
	{
		const bool bMatched = bExactMatch ? Pair.Key.MatchesTagExact(Tag) : Pair.Key.MatchesTag(Tag);
		if (bMatched && IsValid(Pair.Value))
		{
			OutActors.Add(Pair.Value);
			OutMatchedKeys.Add(Pair.Key);
		}
	}
	MatchCount = OutActors.Num();
}

void UGameplayTagFilterLibrary::FindActorsInTagGroups(const TArray<FTaggedActorArray>& Groups,
	FGameplayTag Tag, bool bExactMatch,
	TArray<AActor*>& OutActors, TArray<FGameplayTag>& OutMatchedTags, int32& MatchCount)
{
	OutActors.Reset();
	OutMatchedTags.Reset();
	if (!Tag.IsValid())
	{
		MatchCount = 0;
		return;
	}

	for (const FTaggedActorArray& Group : Groups)
	{
		const bool bMatched = bExactMatch ? Group.Tag.MatchesTagExact(Tag) : Group.Tag.MatchesTag(Tag);
		if (!bMatched)
		{
			continue;
		}
		OutMatchedTags.Add(Group.Tag);
		for (const TObjectPtr<AActor>& Actor : Group.Actors)
		{
			if (IsValid(Actor))
			{
				OutActors.Add(Actor);
			}
		}
	}
	MatchCount = OutActors.Num();
}

void UGameplayTagFilterLibrary::FilterActorsMapByTag(const TMap<AActor*, FGameplayTagContainer>& ActorMap,
	FGameplayTag Tag, bool bExactMatch,
	TArray<AActor*>& OutActors, int32& MatchCount)
{
	OutActors.Reset();
	if (!Tag.IsValid())
	{
		MatchCount = 0;
		return;
	}

	for (const TPair<AActor*, FGameplayTagContainer>& Pair : ActorMap)
	{
		const bool bMatched = bExactMatch ? Pair.Value.HasTagExact(Tag) : Pair.Value.HasTag(Tag);
		if (bMatched && IsValid(Pair.Key))
		{
			OutActors.Add(Pair.Key);
		}
	}
	MatchCount = OutActors.Num();
}
