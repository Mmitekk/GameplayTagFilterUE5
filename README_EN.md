# GameplayTag Filter (Unreal Engine)

Five Blueprint nodes that fetch actors with a given Gameplay Tag from an **array** or a **map** in a single node — instead of wiring ForEach → Actor Has Tag → Branch → Add.

**[🇷🇺 Русская версия →](README.md)**

## Compatibility

- Unreal Engine **5.2 and 5.8** (Win64); source has no version-specific code
- Single Runtime module, GAS (AbilitySystemComponent) **not required** — works with standard Actor Tags (FName)

## Installation

Copy the `GameplayTagFilter` folder into `<Project>/Plugins/` and restart the editor.
The nodes appear in the palette under the **Tag Filter** category.

## Nodes

| Node | Input | Output |
|---|---|---|
| **Filter Actors by Tag** | actor array + one tag | matching actors + `MatchCount` |
| **Filter Actors by Tags** | actor array + tag container + **Any/All** mode | matching actors + `MatchCount` |
| **Find Actors in Tag Map** | `TMap<FGameplayTag, AActor*>` + tag (compares **keys**) | actors + matched keys + `MatchCount` |
| **Find Actors in Tag Groups** | array of `FTaggedActorArray {Tag, Actors[]}` + tag | actors from all matching groups + their tags + `MatchCount` |
| **Filter Actors Map by Tag** | `TMap<AActor*, FGameplayTagContainer>` + tag (compares **tag values**) | actors whose tags matched + `MatchCount` |

All nodes are **BlueprintPure** — no exec pins.

## Tag matching (the `bExactMatch` pin)

- **false** (default) — hierarchical matching, same as GAS: the query `Damage` finds `Damage` and `Damage.Fire`, but not the parent (query `Damage.Fire` will NOT find an actor tagged `Damage`).
- **true** — exact name match only.

For the `Tag → Actor` map the lookup runs on **keys**: the query `Damage` also returns the entry keyed `Damage.Fire`.
Hierarchy only applies to tags registered in the project's tag dictionary (**Project Settings → Gameplay Tags**); unregistered names match exactly only.

## Example

```text
Actor array:   Campfire [Damage.Fire]   Bow [Damage]   Door [Door]

Filter Actors by Tag (query: Damage, bExactMatch = false)
   → Campfire, Bow        (MatchCount = 2)

Filter Actors by Tag (query: Damage, bExactMatch = true)
   → Bow                  (MatchCount = 1)
```

## Why "tag → actor array" is an array of structs

Blueprint does not allow `TMap<FGameplayTag, TArray<AActor*>>` (nested containers are forbidden in maps), so the "tag → actor group" case is exposed as **Find Actors in Tag Groups**, taking an array of the `FTaggedActorArray` struct (`Tag` + `Actors`) — same semantics.

## Building from source

Via UAT (standalone plugin build):

```bat
"<Engine>\Engine\Build\BatchFiles\RunUAT.bat" BuildPlugin ^
  "-Plugin=<Path>\GameplayTagFilter.uplugin" "-Package=<OutDir>" -TargetPlatforms=Win64 -NoSubmit
```

Or as part of a project: copy the plugin folder into `<Project>/Plugins/` and build the project target (`Build.bat <Project>Editor Win64 Development -project="<Project>.uproject"`).

> Note: in UE 5.x the module must contain an explicit `IMPLEMENT_MODULE(FDefaultModuleImpl, GameplayTagFilter);` — UHT does not always generate the boilerplate (already handled in this source).
