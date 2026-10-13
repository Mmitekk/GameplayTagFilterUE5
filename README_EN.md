# GameplayTag Filter (Unreal Engine)

Five Blueprint nodes that fetch actors with a given Gameplay Tag from an **array** or a **map** in a single node — instead of wiring ForEach → Actor Has Tag → Branch → Add.

**[🇷🇺 Русская версия →](README.md)** · Interactive tabbed version: **[README.html](README.html)**

## Compatibility

- Unreal Engine **5.2 and 5.8** (Win64); source has no version-specific code
- Single Runtime module; works with standard **Actor Tags** (the FName list on the actor's panel), no extra systems required

## First things first: what is MatchCount

`MatchCount` is simply **"how many were found"** — it always equals the length of the output array. It duplicates `OutActors.Num()` so you don't have to: one `MatchCount > 0` comparison answers "did we find anything", and it's convenient to print or display.

The `Gameplay Tag Container` input of the second node is just a **list of tags** (a set), nothing special.

## The nodes — pin by pin

### 1. Filter Actors by Tag
"I have an array of actors — keep only those that have tag X."

| Pin | Meaning |
|---|---|
| `Actors` | The actors to check |
| `Tag` | The tag to search for |
| `bExactMatch` | `false` (default) — hierarchical: query `Damage` finds both `Damage` and `Damage.Fire`. `true` — exact name only |
| `OutActors` | The result: actors whose tags matched (input order preserved) |
| `MatchCount` | How many were found. Equals `OutActors` length — handy for a quick "found anything?" check |

### 2. Filter Actors by Tags
The same, but searching by **several** tags at once.

| Pin | Meaning |
|---|---|
| `Actors` | The actors to check |
| `Tags` | The list of tags to search for (a Gameplay Tag Container) |
| `MatchType` | **Any** — one tag from the list is enough. **All** — every tag from the list is required |
| `bExactMatch` | Same as the first node |
| `OutActors` | Actors that passed the filter |
| `MatchCount` | How many were found |

### 3. Find Actors in Tag Map
"I have a map where the **key is a tag** and the **value is an actor**. Fetch entries by tag."

| Pin | Meaning |
|---|---|
| `ActorMap` | The `Tag → Actor` map itself. Each entry: key = tag, value = actor |
| `Tag` | The query tag. Compared against the entry **keys** |
| `bExactMatch` | `false` — query `Damage` also finds the entry keyed `Damage.Fire`; `true` — exact key only |
| `OutActors` | Actors from all entries whose key matched |
| `OutMatchedKeys` | Which keys matched. Useful with hierarchical search: you queried `Status.ChangeFreeze` and the key `Status.ChangeFreeze.Grill` matched — here you see which branch responded |
| `MatchCount` | How many entries matched (equals `OutActors` length) |

### 4. Find Actors in Tag Groups
"A tag → **several** actors". A map with an array inside is forbidden in Blueprint (no nested containers), so such data is stored as an **array of structs** `FTaggedActorArray` — the struct has two fields: `Tag` (the group's tag) and `Actors` (the group's actors). The node does the same as the previous one but returns all actors from all matching groups at once.

| Pin | Meaning |
|---|---|
| `Groups` | The array of groups. Each group = one tag + the actors belonging to it |
| `Tag` | The query tag. Compared against each group's tag |
| `bExactMatch` | `false` — query `Damage` also finds a group tagged `Damage.Fire`; `true` — exact name only |
| `OutActors` | All actors from all matching groups, merged into one array |
| `OutMatchedTags` | The tags of the groups that matched (each listed once) |
| `MatchCount` | How many actors were returned. May exceed the number of matched groups — a group usually holds several actors |

### 5. Filter Actors Map by Tag
"A map where the **key is an actor** and the **value is its list of tags**. Filter actors by tag."

| Pin | Meaning |
|---|---|
| `ActorMap` | The `Actor → Tags` map itself |
| `Tag` | The query tag. Checked against each entry's **tag list** (the value) |
| `bExactMatch` | As usual: `false` — hierarchical, `true` — exact |
| `OutActors` | Key actors whose tag list contains the query |
| `MatchCount` | How many were found |

## General matching rules

- Entries with empty/invalid actors are silently skipped; an empty query tag → empty result.
- **Hierarchy** only works for tags registered in the project's tag dictionary (**Project Settings → Gameplay Tags**). Custom names outside the dictionary match exactly only.
- Hierarchy direction is always parent → children: a parent query finds descendants, not the other way around.

## Example

```text
Actor array:   Campfire [Damage.Fire]   Bow [Damage]   Door [Door]

Filter Actors by Tag (query: Damage, bExactMatch = false)
   → Campfire, Bow        (MatchCount = 2)

Filter Actors by Tag (query: Damage, bExactMatch = true)
   → Bow                  (MatchCount = 1)
```

## Installation

Copy the `GameplayTagFilter` folder into `<Project>/Plugins/` and restart the editor. The nodes appear in the palette under the **Tag Filter** category.

## Building from source

Via UAT (standalone plugin build):

```bat
"<Engine>\Engine\Build\BatchFiles\RunUAT.bat" BuildPlugin ^
  "-Plugin=<Path>\GameplayTagFilter.uplugin" "-Package=<OutDir>" -TargetPlatforms=Win64 -NoSubmit
```

Or as part of a project: copy the plugin folder into `<Project>/Plugins/` and build the project target (`Build.bat <Project>Editor Win64 Development -project="<Project>.uproject"`).

> Note: in UE 5.x the module must contain an explicit `IMPLEMENT_MODULE(FDefaultModuleImpl, GameplayTagFilter);` — UHT does not always generate the boilerplate (already handled in this source).
